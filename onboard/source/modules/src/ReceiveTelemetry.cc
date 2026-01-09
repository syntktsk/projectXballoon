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

  if (communicationType_ == "serial"){
    // ... (シリアル処理はそのまま) ...
    // ※シリアルの場合は buffer_ を使うので、後述のループ条件を調整
    // ここでは socket 側の修正を優先します
  } else if (communicationType_ == "socket") {
    fd_set fdset;
    timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    FD_ZERO(&fdset);
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

  // --- データのパースループ ---
  // data_ptr を使うことで、エラーを回避します
  const std::vector<uint8_t>& received_data = *data_ptr; 
  if (byte_read > 0) {
    std::cout << "--- RAW DATA RECEIVE (Before Interpret) ---" << std::endl;
    std::cout << "Byte Read: " << byte_read << std::endl;

    // --- ここから追加 ---
    std::cout << "Data (Hex): ";
    for (int i = 0; i < byte_read; ++i) {
        // 各バイトを2桁の16進数で表示
        printf("%02x ", received_data[i]); 
        
        // 16バイトごとに改行して見やすくする場合（お好みで）
        if ((i + 1) % 16 == 0) std::cout << "\n            ";
    }
    std::cout << std::dec << std::endl; // 10進数表示に戻す
    std::cout << "--------------------------------------------" << std::endl;
    // --- ここまで追加 ---
  }

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
