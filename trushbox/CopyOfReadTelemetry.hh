#ifndef ReadTelemetry_H
#define ReadTelemetry_H 1

#include <iostream>
#include <vector>

#include <string>
#include <cstdint>
#include <atomic>
#include <thread>
#include <mutex>

namespace balloon {

class ReadTelemetry {
public:
    ReadTelemetry(const std::string& hostIp, int port);
    ~ReadTelemetry();

    // ソケットを確立してデータの受信を開始する
    bool start_listening();
    // データの受信を停止する
    void stop_listening();
    
    // 受信したバイナリデータを返す
    const std::vector<uint8_t> get_binary_data();

private:
    // データ受信スレッドの実行関数
    void listening_thread_func();

    std::string hostIp_;
    int port_;
    int listen_sock_fd_;
    int client_sock_fd_;
    std::atomic<bool> is_running_;
    std::thread listen_thread_;
    std::vector<uint8_t> received_data_;
    std::mutex data_mutex_; // スレッドセーフのためのミューテックス
};

} // namespace balloon

#endif // READ_TELEMETRY_HH