#include "SendTelemetry.hh"
#include "ReceiveEUResponse.hh"
#include "GetGL860Data.hh"
#include "GetRelayStatus.hh"
#include <anlnext/ANLManager.hh>
#include <anlnext/BasicModule.hh>
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

  const std::string get_relay_status_md = "GetRelayStatus";
  if (exist_module(get_relay_status_md)) {
    get_module_NC(get_relay_status_md, &getRelayStatus_);
  }

//HKStatusに追加したいデータ
  const int num_gl860_data = getGL860DataNames_.size();
  for (int i=0; i<num_gl860_data; i++) {
    const std::string module_name = getGL860DataNames_[i];
    if (exist_module(module_name)) {
      GetGL860Data* ged;
      get_module_NC(module_name, &ged);
      getGL860DataVec_.push_back(ged);
    }
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
    anlnext::ANLStatus status = receiveEUResponse_->mod_initialize();
    
    if (status != anlnext::AS_OK) {
      std::cerr << "Error in SendTelemetry::mod_initialize: ReceiveEUResponse initialization failed." << std::endl;
      getErrorManager()->setError(ErrorType::RECEIVE_EU_RESPONSE_INIT_ERROR); 
    }
  }

  lastRegularTelemetryTime_ = std::chrono::steady_clock::now() - TELEMETRY_INTERVAL;

  int ground_status = 0;

  if (communicationType_ == "serial") {
      // 地上系本番: Serial通信 (sc_を初期化)
      sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
      ground_status = sc_->initialize();
      
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
  if (!eu_ || !ou_) {
    std::cerr << "Error in SendTelemetry::mod_initialize: socket in not opened" << std::endl;
    return AS_OK; 
  }
  if (receiveEUResponse_->hasNewData()) {
    int id = receiveEUResponse_->getNewDataID();
    
    if (id != ID_Whole_TELEMETRY) {
      telemetryType_ = id; 
      inputInfo();
      Sender();
      receiveEUResponse_->clearNewDataFlag(); 
      return AS_OK;
    }
  }
  auto now = std::chrono::steady_clock::now();
  if (now - lastRegularTelemetryTime_ >= TELEMETRY_INTERVAL) {
      std::string cmd = "rs0";
      eu_->sendASCII(cmd); 
      telemetryType_ = ID_Whole_TELEMETRY; 
      inputInfo();
      Sender(); 
      lastRegularTelemetryTime_ = now;     
      receiveEUResponse_->clearNewDataFlag();
  }
  
  std::this_thread::sleep_for(std::chrono::milliseconds(sleepms_));
  return AS_OK;
}

void SendTelemetry::Sender(){
  if (telemetryType_ != ID_NONE) {
    telemdef_->setTelemetryType(telemetryType_);
    telemdef_->generateTelemetry();
    const std::vector<uint8_t>& telemetry = telemdef_->Telemetry();
    std::cout << "[SendTelemetry] Attempting to send ID: " << telemetryType_ << " | Size: " << telemetry.size() << " bytes" << std::endl;
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
        std::cout << "[SendTelemetry] Successfully sent " << status << " bytes." << std::endl;
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
    // inputDetectorInfo();
    inputHKVesselInfo();
    inputSoftwareInfo();
  }
  else if (telemetryType_==2) {
    inputEUInfo();
    // GNSSだけ抜けるように何か操作
  }
  else if (telemetryType_==3) {
    inputEUInfo();
    // Elmoだけ抜けるように何か操作
  }
  else if (telemetryType_==4) {
    inputRelayInfo();
  }
  else if (telemetryType_==5) {
    inputOptionalInfo();
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
  const int n = getGL860DataVec_.size();
  auto getVal = [&](int index) -> float {
      if (index < n && getGL860DataVec_[index] != nullptr) {
          return getGL860DataVec_[index]->Value();
      }
      return 0.0f; 
  };
  telemdef_->setPivotTemp(getVal(0));
  telemdef_->setStarCameraTemp(getVal(1));
  telemdef_->setMirrorTemp(getVal(2));
  telemdef_->setGnssTemp_hk(getVal(3));
  telemdef_->setCulculatorTemp(getVal(4));
  telemdef_->setBatteryTemp(getVal(5));
  telemdef_->setGyroTemp(getVal(6));
  telemdef_->setCmosTemp(getVal(7));
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
  if (getRelayStatus_!=nullptr){
    telemdef_->setRelay_dummy(getRelayStatus_->dummy());
  }
}

void SendTelemetry::inputEUInfo(){
  std::cout << "DEBUG: inputEUInfo called with type: " << telemetryType_ << std::endl;
  if (telemetryType_ == 3 || telemetryType_ == 9) {
    std::cout << "DEBUG: Setting Elmo: " << receiveEUResponse_->EUlastcommand() << std::endl;
    telemdef_->setMotorOnOff(receiveEUResponse_->MotorOnOff());
    telemdef_->setUnitMode(receiveEUResponse_->UnitMode());
    telemdef_->setMoterFault(receiveEUResponse_->MoterFault());
    telemdef_->setErrorCode(receiveEUResponse_->ErrorCode());
    telemdef_->setPosition(receiveEUResponse_->Position());
    telemdef_->setVelocity(receiveEUResponse_->Velocity());
    telemdef_->setI_Qaxis(receiveEUResponse_->I_Qaxis());
    telemdef_->setI_Daxis(receiveEUResponse_->I_Daxis());
    telemdef_->setMaxCurrent(receiveEUResponse_->MaxCurrent());
    telemdef_->setBusVoltage(receiveEUResponse_->BusVoltage());
    telemdef_->setTemperatureInfomation(receiveEUResponse_->TemperatureInfomation());
    telemdef_->setTorqueCommand(receiveEUResponse_->TorqueCommand());
    telemdef_->setJogVelocity(receiveEUResponse_->JogVelocity());
    telemdef_->setPositionAbusolute(receiveEUResponse_->PositionAbusolute());
    telemdef_->setPositionRelative(receiveEUResponse_->PositionRelative());
    telemdef_->setModeflag(receiveEUResponse_->modeflag());
    telemdef_->setEnablefrag(receiveEUResponse_->enablefrag());
    telemdef_->setParameterset(receiveEUResponse_->parameterset());
    telemdef_->setEUlastcommand(receiveEUResponse_->EUlastcommand());
  }

  if (telemetryType_ == 2 || telemetryType_ == 9) {
    telemdef_->setLatitude(receiveEUResponse_->latitude());
    telemdef_->setLongitude(receiveEUResponse_->longitude());
    telemdef_->setHeight(receiveEUResponse_->height());
    telemdef_->setYaw(receiveEUResponse_->yaw());
    telemdef_->setPitch(receiveEUResponse_->pitch());
    telemdef_->setRoll(receiveEUResponse_->roll());
    telemdef_->setTemperature(receiveEUResponse_->temperature());
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

} /* namespace balloon */
