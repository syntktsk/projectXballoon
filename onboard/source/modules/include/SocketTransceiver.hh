#ifndef SocketTransceiver_H
#define SocketTransceiver_H 1

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable> 
#include <optional>
// #include "CommandBuilder.hh" 

namespace balloon {
class SendTelemetry;
class SocketTransceiver {
public:
  SocketTransceiver(const std::string& ip, uint16_t port);
  ~SocketTransceiver();
  int initialize(bool soul_bind = false);
  bool open_socket(bool soul_bind = false);
  void close_socket();
  
  //　データを送信する
  int sendBinary(const std::vector<uint8_t>& data);
  int sendASCII(std::string& command);

  // int receiveTelemetry();//主にonboardで構造体とかを受け取るやつ
  int receiveBinary();
  int FD(){return sock_fd_;}
  const std::vector<uint8_t>& last_received_raw_data(){return last_received_raw_data_;}
  size_t received_size() const{return received_size_;}
  // // 受信したバイナリデータを返す
  // std::vector<uint8_t> get_binary_data();
  // std::vector<uint8_t> get_cc_struct_data();
  // std::string get_string_data();

private:
  int sock_fd_;
  std::vector<uint8_t> bytes_read;
  std::vector<uint8_t> last_received_raw_data_;
  std::string serverIp_;
  int port_;
  size_t received_size_;
  // balloon::SendTelemetry* sendTelemetry_ = nullptr;
};

} // namespace balloon

#endif // SocketTransceiver_HH