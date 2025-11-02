#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "TelemetryReceiver_UDP.hh"
#include "SocketTransceiver_UDP.hh"
#include "TelemetryDefinition.hh"
#include "CommandBuilder.hh"
#include "DocumentBuilder.hh"
#include "MongoDBClient.hh"
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace anlnext;

namespace balloon{

TelemetryReceiver_UDP::TelemetryReceiver_UDP()
{    
}

ANLStatus TelemetryReceiver_UDP::mod_define()
{
  // 実行間隔を設定する
  set_run_interval_time_ = 0.5; // 2Hz
  return AS_OK;
}

ANLStatus TelemetryReceiver_UDP::mod_initialize()
{ 
  //今回必要なコマンドの引数
  const std::string& name = "get_status";
  const std::vector<int32_t>& arg_array = {};

  // コマンドを作成
  binary_command_ = command_builder_.make_byte_array(name, arg_array);
  if (binary_command_.empty()) {
    std::cerr << "TelemetryReceiver: command not find" << std::endl;
    return AS_OK;
  }

  ascii_command_ = command_builder_.make_ascii(name, arg_array);
  if (ascii_command_.empty()) {
    std::cerr << "TelemetryReceiver: command not find" << std::endl;
    return AS_OK;
  }
  // MongoDBClientモジュールへのポインタを取得する
  get_module_NC("MongoDBClient", &mongodb_client_);
  if (mongodb_client_ == nullptr) {
      std::cerr << "Error: MongoDBClient module not found." << std::endl;
      return AS_ERROR;
  }

  // サーバーのIPとポートはここに定義
  std::string serverIp_ ="192.168.11.4";
  int port_ = 1234;

  if (!transceiver_.open_socket(serverIp_, port_)) {
      std::cerr << "Socket: ソケットのオープンに失敗しました。" << std::endl;
      return AS_ERROR;
  }

  // ★ここから追加するコード★
  // 日付ベースのファイル名を作成
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d%H%M.bin");
  std::string filename = ss.str();

  // ファイルをオープンする
  outfile.open(filename, std::ios::out | std::ios::binary | std::ios::app);
  if (!outfile.is_open()) {
    std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
    return AS_ERROR;
  }
  std::cout << "Successfully opened " << filename << " for writing." << std::endl;
  // ★追加ここまで★


  return AS_OK;
}

ANLStatus TelemetryReceiver_UDP::mod_analyze()
{
  std::string serverIp_ ="192.168.11.4";
  int port_ = 1234;
  // `send()`関数が実際に何バイトを送信したかを確認する
  bool sent = transceiver_.sendto(ascii_command_, serverIp_, port_);
  if (!sent) {
      std::cerr << "TelemetryReceiver: Failed to send command." << std::endl;
      return AS_ERROR;
  }
  
  // バイナリデータを受信
  std::vector<uint8_t> binary_data = transceiver_.get_binary_data();

  std::cout << "Received raw bytes (" << binary_data.size() << "): ";
  for (const auto& byte : binary_data) {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
  }
  std::cout << std::dec << std::endl;

  if (!binary_data.empty()) {
      std::cout << "Received Data size: " << binary_data.size() << " byte" << std::endl;

      // ファイルにバイナリデータを追記
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

  if (forISAS_.setbinary(binary_data)) {
    forISAS_.interpretISAS();
    forISAS_.generateHK();
    auto doc = forISAS_.getTLMdoc();
    auto doc2 = forISAS_.getTLMdoc2();
    mongodb_client_->push("gnss_status", doc);
    mongodb_client_->push("hk_status", doc2);
  } else {
    // データのセットに失敗した場合、エラーログを出力して正常終了
    std::cerr << "TelemetryReceiver: Received data has an invalid format and was not processed." << std::endl;
    return AS_OK;
  }

  return AS_OK;
}

ANLStatus TelemetryReceiver_UDP::mod_finalize()
{
  // 接続を閉じる
  transceiver_.close_socket();
  
  return AS_OK;    
}

}