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
#include "CommandBuilder.hh" 

namespace balloon {

class SocketTransceiver {
public:
    SocketTransceiver();
    ~SocketTransceiver();

    // ソケットを確立してデータの受信を開始する
    bool open_socket(const std::string& serverIp, int port);
    // データの受信を停止する
    void close_socket();
    bool is_connected() const;
    
    //　データを送信する
    bool send(const std::vector<uint8_t>& command);

    // 受信したバイナリデータを返す
    const std::vector<uint8_t> get_binary_data();

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

#endif // SocketTransceiver_HH