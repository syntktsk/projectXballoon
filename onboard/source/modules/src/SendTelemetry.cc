#include "SendTelemetry.hh"
#include "ReceiveEUResponse.hh"
#include "eu_struct.hh"
#include "GetGL860Data.hh"
#include "RelayControl.hh"
#include "RelayControl.hh"
#include <anlnext/ANLManager.hh>
#include <anlnext/BasicModule.hh>
#include <sys/time.h> // gettimeofday用
#include <ctime>    // localtime, strftime用
using namespace anlnext;

namespace balloon {

SendTelemetry::SendTelemetry()
{
  telemdef_ = std::make_shared<TelemetryDefinition>();
  errorManager_ = std::make_shared<ErrorManager>();
  binaryFilenameBase_ = "Telemetry";
  serialPath_ = "/dev/null";
  baudrate_ = B57600;
  openMode_ = O_RDWR;
  savePath_ = "";
}

SendTelemetry::~SendTelemetry() = default;

ANLStatus SendTelemetry::mod_define()
{
  define_parameter("GL860_Data_names", &mod_class::getGL860DataNames_);

  define_parameter("EU_socket_serverIp", &mod_class::EU_serverIp_);
  define_parameter("EU_socket_port", &mod_class::EU_port_);
  //地上系との通信手段の選択
  define_parameter("communication_type", &mod_class::communicationType_);
  // シリアル通信パラメータ
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("save_path", &mod_class::savePath_);
  // ソケット通信パラメータ
  define_parameter("OU_socket_serverIp", &mod_class::OU_serverIp_);
  define_parameter("OU_socket_port", &mod_class::OU_port_);

  define_parameter("open_mode", &mod_class::openMode_);
  define_parameter("save_telemetry", &mod_class::saveTelemetry_);
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  define_parameter("num_telem_per_file", &mod_class::numTelemPerFile_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus SendTelemetry::mod_initialize()
{
  const std::string get_raspi_status_md = "GetRaspiStatus";
  if (exist_module(get_raspi_status_md)) {
    get_module_NC(get_raspi_status_md, &getRaspiStatus_);
  }
  const std::string num_gl860_data = "GetGL860Data";
  if (exist_module(num_gl860_data)) {
    get_module_NC(num_gl860_data, &getGL860Data_);
  }
  const std::string relay_control_md = "RelayControl";
  if (exist_module(relay_control_md)) {
    get_module_NC(relay_control_md, &relayControl_);
  }
  const std::string receive_command_md = "ReceiveCommand";
  if (exist_module(receive_command_md)) {
    get_module_NC(receive_command_md, &receiveCommand_);
  }
  const std::string run_id_manager_md = "RunIDManager";
  if (exist_module(run_id_manager_md)) {
    get_module_NC(run_id_manager_md, &runIDManager_);
  }
  const std::string receive_eu_response_md = "ReceiveEUResponse";
  if (exist_module(receive_eu_response_md)) {
      // get_module_NC は、既存のモジュールへの生ポインタを取得する関数
      get_module_NC(receive_eu_response_md, &receiveEUResponse_);
  }
  // communication
  if (receiveCommand_ != nullptr) {
    this->eu_ = receiveCommand_->getSocket();
    if (chatter_ >= 1) {
      std::cout << "SendTelemetry: Shared socket from ReceiveCommand." << std::endl;
    }
  }
  if (this->eu_ == nullptr) {
    std::cerr << "Error in SendTelemetry::mod_initialize: Failed to share socket from ReceiveCommand." << std::endl;
    getErrorManager()->setError(ErrorType::SEND_TELEMETRY_Ras2_COMMUNICATION_ERROR);
    return AS_ERROR;
  }
  if (receiveEUResponse_ != nullptr) {
    receiveEUResponse_->setSocket(this->eu_);
  }
  lastRegularTelemetryTime_ = std::chrono::steady_clock::now() - TELEMETRY_INTERVAL;

  int ground_status = 0;
  if (communicationType_ == "serial") {
    // 地上系本番: Serial通信 (sc_を初期化)
    sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
    ground_status = sc_->initialize();
    /*追加したよ〜これで動くのは動くけど、少しダメな気がするから、ここは要改善*/
    std::cout << "DEBUG: Force setting AMA2 to 57600bps via system command..." << std::endl;
    std::string cmd = "stty -F /dev/ttyAMA2 57600 raw -echo min 1 time 0";
    system(cmd.c_str()); 
    // その後、もう一度速度を確認してみる
    struct termios t;
    tcgetattr(sc_->FD(), &t);
    std::cout << "DEBUG: Speed after force reset: " << cfgetispeed(&t) << std::endl;
    /*ここまでね*/  
  } else if (communicationType_ == "socket") {
    // 地上系テスト: Socket通信 (ou_を初期化)
    ou_ = std::make_shared<SocketTransceiver>(OU_serverIp_, OU_port_);
    ground_status = ou_->initialize(true);

  } else {
    // 未知の通信タイプ
    std::cerr << "Error in SendTelemetry::mod_initialize: Unknown ground communication type: " 
              << communicationType_ << std::endl;
    getErrorManager()->setError(ErrorType::SEND_TELEMETRY_UNKNOWN_COMM_TYPE_ERROR);
    return AS_ERROR; 
  }
  if (ground_status != 0) {
    std::cerr << "Error in SendTelemetry::mod_initialize: Ground communication initialization failed." << std::endl;
    // エラータイプは、切り替え時のエラーとして汎用的なものを使う
    getErrorManager()->setError(ErrorType::SEND_TELEMETRY_COMMUNICATION_ERROR); 
  }

  return AS_OK;
}

ANLStatus SendTelemetry::mod_analyze()
{
  if (!eu_ && !ou_) {
    std::cerr << "Error in SendTelemetry::mod_initialize: socket in not opened" << std::endl;
    return AS_OK; 
  }

  if (receiveEUResponse_->hasNewData()) {
    telemetryType_ = receiveEUResponse_->getNewDataID();
    inputInfo();
    Sender(); 
    receiveEUResponse_->clearNewDataFlag();
    return AS_OK;
  }

  if (getGL860Data_->IhaveGL860()){
    std::cout << "DEBUG: We recieved a Command about GL860" << std::endl;
    telemetryType_ = 6; 
    inputInfo();
    Sender();
    getGL860Data_->setDoYouHaveGL860(false);
    getGL860Data_->setLastReceivedOpyionGL860("");
    return AS_OK;
  }

  auto now = std::chrono::steady_clock::now();
  if (now - lastRegularTelemetryTime_ >= TELEMETRY_INTERVAL) {
    lastWholeTlm_.clear();
    telemetryType_ = ID_Whole_TELEMETRY; 
    inputInfo();
    Sender(); 
    lastWholeTlm_ = telemdef_->Telemetry();
    lastRegularTelemetryTime_ = now;
  }
  
  std::this_thread::sleep_for(std::chrono::milliseconds(sleepms_));
  return AS_OK;
}

void SendTelemetry::Sender(){
  if (telemetryType_ != ID_NONE) {
    telemdef_->setTelemetryType(telemetryType_);
    telemdef_->generateTelemetry();
    const std::vector<uint8_t>& telemetry = telemdef_->Telemetry();
    // std::cout << "[SendTelemetry] Attempting to send ID: " << telemetryType_ << " | Size: " << telemetry.size() << " bytes" << std::endl;
    int status = -1;
    if (communicationType_ == "serial" && sc_ != nullptr) {
        status = sc_->swrite(telemetry);
    } 
    else if (communicationType_ == "socket" && ou_ != nullptr) {
        status = ou_->sendBinary(telemetry);
    } else {
        std::cerr << "No communication device available!" << std::endl;
    }
    if (status > 0) {
        std::cout << "SendTelemetry: Successfully sent " << status << " bytes." << std::endl;
    }
    const bool failed = (status != static_cast<int>(telemetry.size()));    
    if (failed) {
      std::cerr << "Sending telemetry failed: status = " << status << std::endl;
      getErrorManager()->setError(ErrorType::SEND_TELEMETRY_SWRITE_ERROR);
    }
    
    if (saveTelemetry_) {
      writeTelemetryToFile(failed);
    }

    if (chatter_>=1) {
      std::cout << (int)telemetry.size() << std::endl;
      for (int i=0; i<(int)telemetry.size(); i++) {
        std::cout << i << " " << static_cast<int>(telemetry[i]) << std::endl;
      }
    }
    telemetryType_ = ID_Whole_TELEMETRY; 
  }
}

ANLStatus SendTelemetry::mod_finalize()
{
  return AS_OK;
}

void SendTelemetry::inputInfo()
{
  telemdef_->setTelemetryType(telemetryType_);

  if (runIDManager_!=nullptr) {
    telemdef_->setRunID(runIDManager_->RunID());
  }
  
  if (telemetryType_==1) {
    inputHKVesselInfo();
    inputSoftwareInfo();
  }
  else if (telemetryType_==2) {
    inputEUInfo();
  }
  else if (telemetryType_==3) {
    inputEUInfo();
  }
  else if (telemetryType_==4) {
    inputRelayInfo();
  }
  else if (telemetryType_==5) {
    inputOptionalInfo();
  }
  else if (telemetryType_==6) {
    inputGL860Option();
  }
  else if (telemetryType_==9) {
    inputEUInfo();
    inputHKVesselInfo();
    inputSoftwareInfo();
  }
  else {
    std::cerr << "Error in SendTelemetry::inputInfo(): wrong telemetry type " << telemetryType_ << std::endl;
  }
}
  
void SendTelemetry::inputHKVesselInfo()
{
  if (getGL860Data_ == nullptr) {
    std::cout << "CRITICAL: getGL860Data_ is NULL!" << std::endl;
    return;
  }
  std::vector<int16_t> data = getGL860Data_->getGL860DataVec();
  if (telemdef_ == nullptr) {
    std::cout << "CRITICAL: telemdef_ is NULL!" << std::endl;
    return;
  }
  telemdef_->setGL860value(data);
}

void SendTelemetry::inputGL860Option(){
  std::string newestData = getGL860Data_->lastCommandGL860() +"->"+ getGL860Data_->lastReceivedOptionGL860();
  telemdef_->setGL860option(getGL860Data_->lastReceivedOptionGL860());
  telemdef_->setlastCommandGL860(getGL860Data_->lastCommandGL860());
  std::cout << "sendTelemetry: gl860 option data was sent" << newestData <<std::endl;
}


void SendTelemetry::inputSoftwareInfo() //そのまま
{
  if (receiveCommand_!=nullptr) {
    telemdef_->setLastCommandIndex(receiveCommand_->CommandIndex());
    telemdef_->setLastCommandCode(receiveCommand_ -> CommandCode());
    telemdef_->setCommandRejectCount(receiveCommand_->CommandRejectCount());
  }
  telemdef_->setSoftwareErrorCode(errorManager_->ErrorCode());
}

void SendTelemetry::inputStatusInfo() //使わない
{
  if (getRaspiStatus_!=nullptr) {
    telemdef_->setSDCapacity(getRaspiStatus_->CapacityFree());
  }else {
    std::cerr << "Warning: getRaspiStatus_ is NULL!" << std::endl;
}
}

void SendTelemetry::inputOptionalInfo()
{
  if (receiveEUResponse_ != nullptr){
    telemdef_->setOptionalStrings(receiveEUResponse_->getOptionalStrings());
  }
}

void SendTelemetry::inputRelayInfo() 
{
  if (relayControl_!=nullptr){
    telemdef_->setRelay(relayControl_->LatestStatus());
  }
}

void SendTelemetry::inputEUInfo(){
  // std::cout << "DEBUG: inputEUInfo - Start" << std::endl;
  std::string cmd = "rs0";
  eu_->sendASCII(cmd); 
  // std::cout << "DEBUG: inputEUInfo called with type: " << telemetryType_ << std::endl;
  if (telemetryType_ == 3 || telemetryType_ == 9) {
    telemdef_->setElmoData(receiveEUResponse_->getElmoStatus());
  }
  if (telemetryType_ == 2 || telemetryType_ == 9) {
    telemdef_->setGNSSData(receiveEUResponse_->getGnssStatus());
  }
  if (telemetryType_ == 7 || telemetryType_ == 9) {
    telemdef_->setSensorsData(receiveEUResponse_->getSensorsStatus());
  }
}

void SendTelemetry::writeTelemetryToFile(bool failed)
{
  int type = telemetryType_;
  if (failed) {
    type = 0;
  }
  std::string type_str = "";
  if (type==1) type_str = "HK";
  if (type==2) type_str = "GNSS";
  if (type==3) type_str = "Elmo";
  if (type==4) type_str = "Relays";
  if (type==5) type_str = "Option";
  if (type==6) type_str = "gl860";
  if (type==7) type_str = "Sensors";
  if (type==9) type_str = "Whole";
  if (type==0) type_str = "failed";

  const bool app = true;
  if (fileIDmp_.find(type)==fileIDmp_.end()) {
    fileIDmp_[type] = std::pair<int, int>(0, 0);
  }
  else if (fileIDmp_[type].second==numTelemPerFile_) {
    fileIDmp_[type].first++;
    fileIDmp_[type].second = 0;
  }

  int run_id = 0;
  std::string time_stamp_str = "YYYYMMDDHHMMSS";
  if (runIDManager_) {
    run_id = runIDManager_->RunID();
    time_stamp_str = runIDManager_->TimeStampStr();
  }
  std::ostringstream id_sout;
  id_sout << std::setfill('0') << std::right << std::setw(6) << fileIDmp_[type].first;
  const std::string id_str = id_sout.str();
  std::ostringstream run_id_sout;
  run_id_sout << std::setfill('0') << std::right << std::setw(6) << run_id;
  const std::string run_id_str = run_id_sout.str();
  const std::string filename = binaryFilenameBase_ + "_" + run_id_str + "_" + time_stamp_str + "_" + type_str + "_" + id_str + ".dat";
  
  telemdef_->writeFile(filename, app);
  fileIDmp_[type].second++;
}
void SendTelemetry::writeEUDataToFile(int id, bool append) {
  // 1. ファイル名用のタイムスタンプを取得
  static std::string start_time_str = ""; 
  if (start_time_str == "" && runIDManager_) {
    start_time_str = runIDManager_->TimeStampStr();
  } else if (start_time_str == "") {
    start_time_str = "20260226000000"; // フォールバック用
  }

  // 2. 日時を含んだファイル名を作成
  std::string filename;
  if (id == 300) {
    filename = savePath_ + start_time_str + "_rs0.csv";
  } else if (id == 301) {
    filename = savePath_ + start_time_str + "_rs1.csv";
  } else if (id == 302) {
    filename = savePath_ + start_time_str + "_rs2.csv";
  }

  // 3. 書き込み処理（ここは append=true で呼び出せば追記される）
  std::ofstream ofs;
  if (append) {
    ofs.open(filename, std::ios::app);
  } else {
    ofs.open(filename, std::ios::out);
  }

  if (!ofs) {
    std::cerr << "File open error: " << filename << std::endl;
    return;
  }
  struct timeval timeNow_;
  gettimeofday(&timeNow_, NULL);
  uint32_t time_stamp = static_cast<uint32_t>(timeNow_.tv_sec);

  ofs << std::to_string(receiveCommand_->CommandIndex()) << "," << time_stamp << ",";
  if (id == 300) {
    ofs << receiveEUResponse_->getElmoStatus()<< ",";
    ofs << receiveEUResponse_->getGnssStatus();
    std::cout << "SendTelemetry: saved rs0" << std::endl;
  } else if (id == 301) {
    ofs << receiveEUResponse_->getElmoStatus();
    std::cout << "SendTelemetry: saved rs1" << std::endl;
  } else if (id == 302) {
    ofs << receiveEUResponse_->getGnssStatus();
    std::cout << "SendTelemetry: saved rs2" << std::endl;
  }
  ofs << "\n";
  ofs.flush();
  ofs.close();
}
} /* namespace balloon */
