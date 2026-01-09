#include "SocketTransceiver_TCP.hh"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip> 

namespace balloon{

SocketTransceiver_TCP::SocketTransceiver_TCP()
  : sock_fd_(-1), is_running_(false) {
}

SocketTransceiver_TCP::~SocketTransceiver_TCP() {
    close_socket();
}

bool SocketTransceiver_TCP::open_socket(const std::string& hostIp, int port) {
    if (is_running_) {
        std::cerr << "Already listening." << std::endl;
        return false;
    }

    // サーバーソケットの作成
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);//TCPだとSTREAM、UDPだとDGRAM
    if (sock_fd_ < 0) {
        std::cerr << "Failed to create client socket." << std::endl;
        return false;
    }

    // ソケットアドレスの設定
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, hostIp.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address." << std::endl;
        close(sock_fd_);
        return false;
    }

    // サーバーへの接続
    if (connect(sock_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to server." << std::endl;
        close(sock_fd_);
        return false;
    }

    // 受信スレッドを起動
    is_running_ = true;
    receiving_thread_ = std::thread(&SocketTransceiver_TCP::receiving_thread_func, this);

    std::cout << "Successfully connected to " << hostIp << ":" << port << std::endl;
    return true;
}


void SocketTransceiver_TCP::close_socket() {
    if (is_running_) {
        is_running_ = false;
        if (receiving_thread_.joinable()) {
            // スレッドを強制終了するためにソケットを閉じる
            if (sock_fd_ != -1) {
                shutdown(sock_fd_, SHUT_RDWR);
                close(sock_fd_);
            }
            receiving_thread_.join();
        }
        if (sock_fd_ != -1) {
            close(sock_fd_);
        }
        sock_fd_ = -1;
        std::cout << "Socket:Disconnected." << std::endl;
    }
}


bool SocketTransceiver_TCP::is_connected() const {
    return sock_fd_ != -1;
}

void SocketTransceiver_TCP::receiving_thread_func() {
    const size_t buffer_size = 1024;
    std::vector<uint8_t> buffer(buffer_size);
    ssize_t bytes_read;

    while (is_running_) {
        // データが来るまでブロック
        bytes_read = read(sock_fd_, buffer.data(), buffer_size);
        if (bytes_read > 0) {
            std::lock_guard<std::mutex> lock(data_mutex_);
            received_data_.insert(received_data_.end(), buffer.begin(), buffer.begin() + bytes_read);

            // メインスレッドに新しいデータが利用可能になったことを通知する
            is_data_ready_ = true;
            data_cv_.notify_one();

            std::cout << "Received " << bytes_read << " bytes in the background." << std::endl;
        }  else if (bytes_read == 0) {
            std::cout << "Server disconnected gracefully." << std::endl;
            break; // サーバーが切断したらループを抜ける
        } else {
            if (is_running_) {
                std::cerr << "Failed to read data." << std::endl;
            }
            break; // エラーが発生したらループを抜ける
        }
    }
}


bool SocketTransceiver_TCP::send(const std::vector<uint8_t>& command) {
    if (!is_running_ || sock_fd_ < 0) {
        std::cerr << "Not connected." << std::endl;
        return false;
    }

    // ここに、送信される実際のデータの中身を表示するログを追加
    std::cout << "Sending " << command.size() << " bytes. Data: ";
    for (const auto& byte : command) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;

    ssize_t bytes_sent = write(sock_fd_, command.data(), command.size());
    if (bytes_sent < 0) {
        std::cerr << "Failed to send command." << std::endl;
        return false;
    }
    std::cout << "Sent " << bytes_sent << " bytes." << std::endl;
    // 送信されたバイト数がコマンドのサイズと一致するか確認
    if (bytes_sent != static_cast<ssize_t>(command.size())) {
        std::cerr << "Warning: Incomplete data sent." << std::endl;
        return false;
    }    
    return true;
}

// const std::vector<uint8_t> SocketTransceiver::get_binary_data() {
//     std::lock_guard<std::mutex> lock(data_mutex_);
//     std::vector<uint8_t> data_copy = received_data_;
//     received_data_.clear(); // データを取得したらクリアする
//     return data_copy;
// }


const std::vector<uint8_t> SocketTransceiver_TCP::get_binary_data() {
    const size_t packet_size = 96;

    std::unique_lock<std::mutex> lock(data_mutex_);
    
    // パケットサイズ以上のデータが揃うまで待機する
    data_cv_.wait(lock, [this, packet_size]{
        return !is_running_ || received_data_.size() >= packet_size;
    });

    if (!is_running_) {
        return {}; // ソケットが閉じられたら空を返す
    }

    // 最初の完全なパケットを抽出
    std::vector<uint8_t> data_copy(received_data_.begin(), received_data_.begin() + packet_size);
    received_data_.erase(received_data_.begin(), received_data_.begin() + packet_size);
    
    is_data_ready_ = false; // フラグをリセット
    
    return data_copy;
}

// const std::vector<uint8_t> SocketTransceiver_TCP::get_binary_data() {
//     std::lock_guard<std::mutex> lock(data_mutex_);
//     if (received_data_queue_.empty()) {
//         return {}; // データがなければ空のベクタを返す
//     }
//     std::vector<uint8_t> data_copy = received_data_queue_.front();
//     received_data_queue_.pop();
//     return data_copy;
// }

} // namespace balloon