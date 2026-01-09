#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "TelemetryReceiver.hh"
#include "SocketTransceiver.hh"
#include "TelemetryDefinition.hh"
#include "SendTelemetry.hh"
#include "ErrorManager.hh"
// #include "CommandBuilder.hh"
// #include "DocumentBuilder.hh"
#include "MongoDBClient.hh"
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace anlnext;

namespace balloon{
TelemetryReceiver::TelemetryReceiver()
{
  serialPath_ = "/dev/null";
  binaryFilenameBase_ = "Telemetry";
  // comdef_ = std::make_shared<CommandDefinition>(); 
  buffer_.resize(bufferSize_);
  serverIp_ = "";
  port_ = 1234; 
}

ANLStatus TelemetryReceiver::mod_define()
{
  set_run_interval_time_ = 0.5; // 2Hz

  define_parameter("open_mode", &mod_class::openMode_);
  define_parameter("timeout_sec", &mod_class::timeoutSec_);
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  define_parameter("chatter", &mod_class::chatter_);

  define_parameter("communication_type", &mod_class::communicationType_);

  define_parameter("serial_path", &mod_class::serialPath_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("Telemetry_Length", &mod_class::length_);

  define_parameter("OU_socket_serverIp", &mod_class::OU_serverIp_);
  define_parameter("OU_socket_port", &mod_class::OU_port_);
  return AS_OK;
}

ANLStatus TelemetryReceiver::mod_initialize()
{ 
  const std::string target = "SendTelemetry";
  if (exist_module(target)) {
    get_module_NC(target, &sendTelemetry_);
  }

  const std::string& name = "get_status";
  const std::vector<int32_t>& arg_array = {};

  int ground_status = 0;
  if (communicationType_ == "serial"){
    sc_ = std::make_shared<SerialCommunication>(serialPath_, baudrate_, openMode_);
    ground_status = sc_->initialize();
  }else if(communicationType_ == "socket"){
    ou_ = std::make_shared<SocketTransceiver>(OU_serverIp_, OU_port_);
    ground_status = ou_->initialize();
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
  get_module_NC("MongoDBClient", &mongodb_client_);
  if (mongodb_client_ == nullptr) {
    std::cerr << "Error: MongoDBClient module not found." << std::endl;
    return AS_ERROR;
  }

  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d.bin");
  std::string filename = ss.str();

  outfile.open(filename, std::ios::out | std::ios::binary | std::ios::app);
  if (!outfile.is_open()) {
    std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
    return AS_ERROR;
  }
  std::cout << "Successfully opened " << filename << " for writing." << std::endl;


  return AS_OK;
}

ANLStatus TelemetryReceiver::mod_analyze()
{
  std::vector<uint8_t> binary_data; // ここで宣言
  int bytes_read = 0;

  if (communicationType_ == "socket"){
    bytes_read = ou_->receiveBinary(); // 受信してバイト数を受け取る
    if (bytes_read > 0) {
      binary_data = ou_->last_received_raw_data(); // 実際のデータを取り出す
    }
  } else if (communicationType_ == "serial") {
    binary_data.resize(length_); 
    bytes_read = sc_ -> sread(binary_data, length_); 
  }
 if (bytes_read > 0 && bytes_read < length_) {
    binary_data.resize(bytes_read);
  } else if (bytes_read <= 0){
    binary_data.clear();
  }else{
  std::cerr << "Error: undefined communication type." << std::endl;
  }
  std::cout << "Received raw bytes (" << binary_data.size() << "): ";
  for (const auto& byte : binary_data) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
  }
  std::cout << std::dec << std::endl;

  if (!binary_data.empty()) {
    std::cout << "Received Data size: " << binary_data.size() << " byte" << std::endl;

    if (outfile.is_open()) {
      outfile.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
      std::cout << "Binary data appended to file." << std::endl;
    } else {
      std::cerr << "Error: File is not open for writing." << std::endl;
    }

    std::cout << "Received raw bytes (" << binary_data.size() << "): ";
    for (const auto& byte : binary_data) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;

  } else {
    std::cout << "no Data" << std::endl;
  }

  if (binary_data.empty()) {
    return AS_OK; 
  }
  td_ = std::make_shared<TelemetryDefinition>();
  // db_ = std::make_shared<DocumentBuilder>();
  // pM_ = std::make_shared<PushToMongoDB>();
  if (td_->setTelemetry(binary_data)){
    td_->interpret();
  } else {
     std::cerr << "TelemetryReceiver: Received data has an invalid format and was not processed." << std::endl;
    return AS_OK;
  }

  return AS_OK;
}

ANLStatus TelemetryReceiver::mod_finalize(){
  if (ou_) { // ou_ が存在するときだけ閉じる
    ou_->close_socket();  
  }
  return AS_OK;    
}

ErrorManager* TelemetryReceiver::getErrorManager() {
  if (sendTelemetry_ != nullptr) {
    return sendTelemetry_->getErrorManager(); // SendTelemetry経由で取得
  }
  return nullptr; // 見つからなければNULLを返す（本当は自前で持つのもアリ）
}
}