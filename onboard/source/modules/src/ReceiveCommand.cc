#include "ReceiveCommand.hh"
#include <chrono>
#include <thread>

using namespace anlnext;

namespace balloon {

ReceiveCommand::ReceiveCommand()
  : baudrate_(B1200), openMode_(O_RDWR)
{
  serialPath_ = "/dev/null";
  binaryFilenameBase_ = "Command";
  comdef_ = std::make_shared<CommandDefinition>(); 
  buffer_.resize(bufferSize_);
  serverIp_ = "192.168.10.123";//"192.168.10.121";//Rasi
  port_ = 9090;
}

ReceiveCommand::~ReceiveCommand() = default;

ANLStatus ReceiveCommand::mod_define()
{
  define_parameter("timeout_sec", &mod_class::timeoutSec_);
  define_parameter("save_command", &mod_class::saveCommand_);
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  define_parameter("num_command_per_file", &mod_class::numCommandPerFile_);
  define_parameter("chatter", &mod_class::chatter_);

  define_parameter("EU_socket_serverIp", &mod_class::EU_serverIp_);
  define_parameter("EU_socket_port", &mod_class::EU_port_);
  define_parameter("communication_type", &mod_class::communicationType_);
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("OU_socket_serverIp", &mod_class::OU_serverIp_);
  define_parameter("OU_socket_port", &mod_class::OU_port_);

  return AS_OK;
}

ANLStatus ReceiveCommand::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  const std::string shutdown_system_md = "ShutdownSystem";
  if (exist_module(shutdown_system_md)) {
    get_module_NC(shutdown_system_md, &shutdownSystem_);
  }

  const std::string run_id_manager_md = "RunIDManager";
  if (exist_module(run_id_manager_md)) {
    get_module_NC(run_id_manager_md, &runIDManager_);
  }

  // communication
  eu_ = std::make_shared<SocketTransceiver>(EU_serverIp_, EU_port_);
  const int eu_status = eu_ -> initialize(true); 
  if (eu_status != 0) {
    std::cerr << "Error in ReceiveCommand: EU Socket bind failed." << std::endl;
    // エラー処理
  }
  
  int ground_status = 0;

  if (communicationType_ == "serial") {
      sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
      ground_status = sc_->initialize();
      
  } else if (communicationType_ == "socket") {
      ou_ = std::make_shared<SocketTransceiver>(OU_serverIp_, OU_port_);
      ground_status = ou_->initialize(true);

  } else {
      std::cerr << "Error in SendTelemetry::mod_initialize: Unknown ground communication type: " 
                << communicationType_ << std::endl;
      getErrorManager()->setError(ErrorType::SEND_TELEMETRY_UNKNOWN_COMM_TYPE_ERROR);
      return AS_ERROR; 
  }

  if (ground_status != 0) {
      std::cerr << "Error in SendTelemetry::mod_initialize: Ground communication initialization failed." << std::endl;
      getErrorManager()->setError(ErrorType::SEND_TELEMETRY_COMMUNICATION_ERROR); 
  }
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_analyze()
{
  fd_set fdset;
  timeval timeout;
  int rv = 0;
  int byte_read = 0;
  timeout.tv_sec = timeoutSec_;
  timeout.tv_usec = 0;
  FD_ZERO(&fdset);

  if (communicationType_ == "serial") {
    FD_SET(sc_->FD(), &fdset);
    rv = select(sc_->FD() + 1, &fdset, NULL, NULL, &timeout);
  }else if (communicationType_ == "socket"){
    FD_SET(ou_->FD(), &fdset);
    rv = select(ou_->FD() + 1, &fdset, NULL, NULL, &timeout);
  }

  if (rv == -1) {
    std::cerr << "Error in ReceiveCommand::mod_analyze: rv = -1" << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::RECEIVE_COMMAND_SELECT_ERROR);
    }
    return AS_OK;
  }
  
  if (rv==0) {
    if (chatter_>=1) {
      std::cout << "Time out" << std::endl;
    }
    return AS_OK;
  }

  if (communicationType_ == "serial") {
    std::this_thread::sleep_for(std::chrono::milliseconds(serialReadingTimems_));
    byte_read = sc_->sread(buffer_, bufferSize_);
  }else if (communicationType_ == "socket"){
    const int telemetry_id = ou_->receiveBinary();
    (void)telemetry_id;
    const std::vector<uint8_t>& received_data = ou_->last_received_raw_data();
    byte_read = received_data.size();
    buffer_ = received_data;
    std::cout << "DEBUG(Socket): Received " << byte_read << " bytes from socket." << std::endl;
  }
  
  if (byte_read == -1) {
    std::cerr << "Read command failed in ReceiveCommand::mod_analyze: byte_read = " << byte_read << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::RECEIVE_COMMAND_SREAD_ERROR);
    }
    return AS_OK;
  }

  for (int i=0; i<byte_read; i++) {
    const int n = command_.size();
    if (n>=1 && command_[n-1]==0xEB && buffer_[i]==0x90) {
      command_.clear();
      command_.push_back(0xEB);
      command_.push_back(0x90);
      continue;
    }
    if (n>=1 && command_[n-1]==0xC5 && buffer_[i]==0xA4) {
      command_.push_back(buffer_[i]);
      const bool applied = applyCommand();
      writeCommandToFile(!applied);
      if (!applied) {
        commandRejectCount_++;
        if (sendTelemetry_) {
          sendTelemetry_->getErrorManager()->setError(ErrorType::INVALID_COMMAND);
        }
      }
      continue;
    }
    command_.push_back(buffer_[i]);
  }

  if (chatter_>=1) {
    std::cout << "ReceiveCommand byte_read: " << byte_read << std::endl;
    for (int i=0; i<static_cast<int>(command_.size()); i++) {
      std::cout << "command[" << i << "] = " << static_cast<int>(command_[i]) << std::endl;
    }
  }
  
  return AS_OK;
}

ANLStatus ReceiveCommand::mod_finalize()
{
  return AS_OK;
}

bool ReceiveCommand::applyCommand()
{
  commandIndex_++;
  if (chatter_>=1) {
    std::cout << "command start" << std::endl;
    std::cout << "command index: " << commandIndex_ << std::endl;
  }
  for (int i=0; i<(int)command_.size(); i++) {
    std::cout << static_cast<int>(command_[i]) << std::endl;
  }
  bool status = comdef_ -> setCommand(command_);
  if (!status) {
    return false;
  }
  comdef_->interpret();
  
  const uint16_t code = comdef_->Code();
  const uint16_t argc = comdef_->Argc();
  const std::vector<std::string> arguments = comdef_->Arguments();
  int code_int = static_cast<int>(code) % 100;
  std::string code_int_str = std::to_string(static_cast<int>(code_int));

  if (code==100 && argc==0) {
    if (sendTelemetry_!=nullptr) {
      sendTelemetry_->setTelemetryType(static_cast<int>(TelemetryType::HK));
      return true;
    }
  }

  if (code==101 && argc==0) {
    if (sendTelemetry_!=nullptr) {
      sendTelemetry_->getErrorManager()->resetError();
      return true;
    }
  }

  if (code==102 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setShutdown(true);
      return true;
    }
  }

  if (code==103 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setReboot(true);
      return true;
    }
  }

  if (code==104 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setPrepareShutdown(true);
      return true;
    }
  }

  if (code==105 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setPrepareReboot(true);
      return true;
    }
  }

  if (code==198 && argc==1) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setPrepareSoftwareStop(true);
      shutdownSystem_->setExitStatus(std::stoi(arguments[0]));
      return true;
    }
  }

  if (code==199 && argc==0) {
    if (shutdownSystem_!=nullptr) {
      shutdownSystem_->setSoftwareStop(true);
      return true;
    }
  }
  
  if (code >= 200 && code <= 299) {
      StringCommand_ = "ac" + code_int_str;
      eu_ ->sendASCII(StringCommand_);
      return true;     
  }

  if (code >= 300 && code <= 399) {
      StringCommand_ = "rs" + code_int_str;
      std::cout << "ReceiveCommand: To " << EU_serverIp_ << ":" << EU_port_ << " Data: " << StringCommand_ << std::endl;
      eu_ ->sendASCII(StringCommand_);
      // if(code == 300)

      return true;
  }

  if (code >= 400 && code <= 499) {
      StringCommand_ = "ps" + code_int_str;
      eu_ ->sendASCII(StringCommand_);
      return true;
  }

  if (code >= 500 && code <= 599) {
      std::string arg_str = "";
      if (!arguments.empty()) {
        arg_str = arguments[0];
      }

      // \n を付けず、純粋な文字列だけを作る
      if (code == 501){
        StringCommand_ = "TC=" + arg_str;
      } else if (code == 502){
        StringCommand_ = "JV=" + arg_str;
      } else if (code == 503){
        StringCommand_ = "PA=" + arg_str;
      } else if (code == 504){
        StringCommand_ = "PR=" + arg_str;
      } else if (code == 505){
        StringCommand_ = "MO=" + arg_str;
      } else if (code == 506){
        StringCommand_ = "UM=" + arg_str;
      } else if (code == 599) {
        StringCommand_ = "wc=" + arg_str;
        eu_ ->sendASCII(StringCommand_);
        std::cout << "Sending to EU by Receive: " << StringCommand_ << std::endl;
      }else {
        std::cerr << "ReadCommand: command " << code << " not found" << std::endl;
        return true;
      }
      // デバッグ出力で確認（クォートが表示されないはず）
      std::cout << "Sending to EU: " << StringCommand_ << std::endl;
  
      eu_->sendASCII(StringCommand_);
      return true;
  }

  if (code >=600 && code <=699){
    if (code == 699){
      if (!IhaveGl860Data_){
        lastReceivedGL860_ = "";
        std::string arg_str = "";
        if (!arguments.empty()) {
          arg_str = arguments[0];
        }
        StringCommand_ = arg_str;
        lastReceivedGL860_ = GL860main_->sendAndReceive(StringCommand_);
        IhaveGl860Data_ = true;
        if (!lastReceivedGL860_.empty()){
          lastCommandGL860_ = StringCommand_;
        }
        std::cout << "Sending to GL860 > " << StringCommand_ << std::endl;
      }else{
        std::cout << "sending data now, try again." << std::endl;
        return true;
      }
    }
    return true;
  }

  if (code==900 && argc==0) {
    return true;
  }

  if (code==901 && argc==1) {
    return true;
  }

  if (code==902 && argc==0) {
    return true;
  }

  return false;
}

void ReceiveCommand::writeCommandToFile(bool failed)
{
  int type = 1;
  std::string type_str = "";
  if (failed) {
    type = 0;
  }
  if (type==1) type_str = "normal";
  if (type==0) type_str = "failed";

  const bool app = true;
  if (fileIDmp_.find(type)==fileIDmp_.end()) {
    fileIDmp_[type] = std::pair<int, int>(0, 0);
  }
  else if (fileIDmp_[type].second==numCommandPerFile_) {
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
  
  if (!failed) {
    comdef_->writeFile(filename, app);
  }
  else {
    writeVectorToBinaryFile(filename, app, command_);
  }
  fileIDmp_[type].second++;
}

ErrorManager* ReceiveCommand::getErrorManager() {
    if (sendTelemetry_ != nullptr) {
        return sendTelemetry_->getErrorManager();
    }
    return nullptr;
}

} /* namespace balloon */
