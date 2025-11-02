#ifndef SocketTransceiver_UDP_H
#define SocketTransceiver_UDP_H 1

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable> 
// #include "CommandBuilder.hh" 

namespace balloon {

class SocketTransceiver_UDP {
public:
    SocketTransceiver_UDP();
    ~SocketTransceiver_UDP();

    // ソケットを確立してデータの受信を開始する
    bool open_socket(const std::string& serverIp, int port);
    // データの受信を停止する
    void close_socket();
    // bool is_connected() const;
    
    //　データを送信する
    bool sendto(std::vector<uint8_t>& command, const std::string& hostIp, int port);

    // 受信したバイナリデータを返す
    std::vector<uint8_t> get_binary_data();
    std::vector<uint8_t> get_cc_struct_data();
    std::string get_string_data();

private:
  void receiving_thread_func();

  int sock_fd_;
  std::atomic<bool> is_running_;

  std::thread receiving_thread_;
  std::vector<uint8_t> received_data_;
  std::mutex data_mutex_; // スレッドセーフのためのミューテックス
  std::condition_variable data_cv_;
  bool is_data_ready_ = false; // データが準備できたことを示すフラグ
};

} // namespace balloon

#endif // SocketTransceiver_UDP_HH