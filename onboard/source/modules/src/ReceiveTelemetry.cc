#include "ReceiveTelemetry.hh"

using namespace anlnext;

namespace balloon {

ReceiveTelemetry::ReceiveTelemetry()
  : maxTelemetry_(32000), baudrate_(B57600), openMode_(O_RDWR)
{
  serialPath_ = "/dev/null";
  binaryFilenameBase_ = "Command";
  comdef_ = std::make_shared<CommandDefinition>(); 
  buffer_.resize(bufferSize_);
  serverIp_ = "127.0.0.1";
  port_ = 9090;
}

ReceiveTelemetry::~ReceiveTelemetry() = default;

ANLStatus ReceiveTelemetry::mod_define()
{
  define_parameter("open_mode", &mod_class::openMode_);
  define_parameter("timeout_sec", &mod_class::timeoutSec_);
  define_parameter("save_command", &mod_class::saveCommand_);
  define_parameter("num_command_per_file", &mod_class::numCommandPerFile_);
  define_parameter("chatter", &mod_class::chatter_);

  //地上系との通信手段の選択
  define_parameter("communication_type", &mod_class::communicationType_);
  // シリアル通信パラメータ
  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("baudrate", &mod_class::baudrate_);
  // ソケット通信パラメータ
  define_parameter("OU_socket_serverIp", &mod_class::OU_serverIp_);
  define_parameter("OU_socket_port", &mod_class::OU_port_);
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  return AS_OK;
}

ANLStatus ReceiveTelemetry::mod_initialize()
{
  buffer_.resize(maxTelemetry_);
  if (communicationType_ == "serial"){
    sc_ = std::make_unique<SerialCommunication>(serialPath_, baudrate_, openMode_);
    sc_->initialize();
    // ★ これを sc_->initialize() の直後に入れて再ビルド！
    std::cout << "Force setting Mac serial port..." << std::endl;
    std::string cmd = "stty -f " + serialPath_ + " 57600 raw -echo";
    system(cmd.c_str());
    /*ここまで追加*/
  }else if(communicationType_ == "socket"){
    ou_ = std::make_unique<SocketTransceiver>(OU_serverIp_, OU_port_);
    ou_ ->initialize(true);
  }else{
    std::cerr << "Read command failed in ReceiveTelemetry::unknown communicationType"<< std::endl;
    return AS_ERROR;
  }
  return AS_OK;
}

ANLStatus ReceiveTelemetry::mod_analyze()
{ 
  int byte_read = 0;
  // ループで使うための参照用変数をあらかじめ定義しておく
  std::vector<uint8_t> dummy_data; 
  const std::vector<uint8_t>* data_ptr = &dummy_data;

  fd_set fdset;
  timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  FD_ZERO(&fdset);

  if (communicationType_ == "serial"){
    FD_SET(sc_->FD(), &fdset);
    int rv = select(sc_->FD() + 1, &fdset, NULL, NULL, &timeout);
    if (rv == -1) {
      std::cerr << "Error in ReceiveTelemetry::mod_analyze: rv = -1" << std::endl;
      valid_ = false;
      return AS_ERROR;
    }
    if (rv == 0) {
      std::cout << "Time out(serial)" << std::endl;
      valid_ = false;
      return AS_OK;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    byte_read = sc_->sread(buffer_, maxTelemetry_);
    if (byte_read == -1) {
      std::cerr << "Read command failed in ReceiveTelemetry::mod_analyze: byte_read = " << byte_read << std::endl;
      return AS_OK;
    }
  } else if (communicationType_ == "socket") {

    FD_SET(ou_->FD(), &fdset);

    int rv = select(ou_->FD() + 1, &fdset, NULL, NULL, &timeout);
    if (rv <= 0) {
      if (rv == 0) std::cout << "Time out (Socket)" << std::endl;
      valid_ = false;
      return AS_OK;
    }

    int telemetry_id = ou_->receiveBinary();
    if (telemetry_id == ID_NONE) {
        valid_ = false;
        return AS_OK;
    }

    // ★ ここでポインタを実際の受信データに向ける
    data_ptr = &(ou_->last_received_raw_data());
    byte_read = data_ptr->size();
    
    telemetry_.clear(); 
    telemetry_.insert(telemetry_.end(), data_ptr->begin(), data_ptr->end());

    if (byte_read == 0) return AS_OK;

  } else {
    return AS_ERROR;
  }

  const std::vector<uint8_t>& received_data = (communicationType_ == "serial") ? buffer_ : *data_ptr;

  for (int i = 0; i < byte_read; i++) {
    valid_ = false;
    const int n = telemetry_.size();
    
    // received_data[i] を使用
    if (n>=3 && telemetry_[n-3]==0xEB && telemetry_[n-2]==0x90 && telemetry_[n-1]==0x5B && received_data[i]==0x6A) {
      telemetry_.clear();
      telemetry_.push_back(0xEB);
      telemetry_.push_back(0x90);
      telemetry_.push_back(0x5B);
      telemetry_.push_back(0x6A);
      continue;
    }
    if (n>=3 && telemetry_[n-3]==0xC5 && telemetry_[n-2]==0xA4 && telemetry_[n-1]==0xD2 && received_data[i]==0x79) {
      telemetry_.push_back(received_data[i]);
      valid_ = true;
      break;
    }
    telemetry_.push_back(received_data[i]);
  }
  if (chatter_>=1) {
    std::cout << "byte_read: " << byte_read << std::endl;
  }
  if (chatter_>=10) {
    std::cout << "byte_read: " << byte_read << std::endl;
    for (int i = 0; i < static_cast<int>(telemetry_.size());i++) {
      std::cout << "telemetry[" << i << "] = "<<static_cast<int>(telemetry_[i]) << std::endl;
    }
  }

  return AS_OK;
}

ANLStatus ReceiveTelemetry::mod_finalize()
{
  return AS_OK;
}

} // namespace balloon
