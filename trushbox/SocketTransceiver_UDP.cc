#include "SocketTransceiver_UDP.hh"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip>
#include <string.h> // for memset
#include <thread>
#include <mutex>
#include <condition_variable>

namespace balloon{

SocketTransceiver_UDP::SocketTransceiver_UDP()
  : sock_fd_(-1), is_running_(false) {
}

SocketTransceiver_UDP::~SocketTransceiver_UDP() {
    close_socket();
}

bool SocketTransceiver_UDP::open_socket(const std::string& hostIp, int port) {
    if (sock_fd_ != -1) {
        std::cerr << "Already open." << std::endl;
        return false;
    }

    // UDPソケットの作成: SOCK_DGRAMを指定
    sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd_ < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }
    
    // 受信スレッドを起動
    is_running_ = true;
    receiving_thread_ = std::thread(&SocketTransceiver_UDP::receiving_thread_func, this);

    std::cout << "Successfully opened UDP socket." << std::endl;
    return true;
}


void SocketTransceiver_UDP::close_socket() {
    if (is_running_) {
        is_running_ = false;
        if (receiving_thread_.joinable()) {
            if (sock_fd_ != -1) {
                // スレッドが recvfrom() でブロックしている場合に、ソケットを閉じて強制終了させる
                close(sock_fd_);
                sock_fd_ = -1; // ソケットを閉じたら無効にする
            }
            receiving_thread_.join();
        }
        std::cout << "Socket: Closed." << std::endl;
    }
}


// UDPには接続状態の概念がないため、このメソッドはTCPに特有
// bool SocketTransceiver::is_connected() const {
//     return sock_fd_ != -1;
// }


void SocketTransceiver_UDP::receiving_thread_func() {
    const size_t buffer_size = 1024;
    std::vector<uint8_t> buffer(buffer_size);
    ssize_t bytes_read;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    while (is_running_) {
        // データが来るまでブロック
        // recvfromで送信元のアドレス情報も取得できる
        bytes_read = recvfrom(sock_fd_, buffer.data(), buffer_size, 0, (struct sockaddr*)&server_addr, &addr_len);

        if (bytes_read > 0) {
            std::lock_guard<std::mutex> lock(data_mutex_);
            received_data_.insert(received_data_.end(), buffer.begin(), buffer.begin() + bytes_read);

            // メインスレッドに新しいデータが利用可能になったことを通知する
            is_data_ready_ = true;
            data_cv_.notify_one();

            std::cout << "Received " << bytes_read << " bytes in the background." << std::endl;
        } else if (bytes_read == 0) {
            std::cout << "Sender disconnected gracefully." << std::endl;
            break; 
        } else {
            if (is_running_) {
                std::cerr << "Failed to read data." << std::endl;
            }
            break; 
        }
    }
}


bool SocketTransceiver_UDP::sendto(std::vector<uint8_t>& command, const std::string& hostIp, int port) {
    if (sock_fd_ < 0) {
        std::cerr << "Socket not open." << std::endl;
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, hostIp.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address." << std::endl;
        return false;
    }

    // sendto() で宛先を指定して送信
    ssize_t bytes_sent = ::sendto(sock_fd_, command.data(), command.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (bytes_sent < 0) {
        std::cerr << "Failed to send command." << std::endl;
        return false;
    }
    return true;
}

std::vector<uint8_t> SocketTransceiver_UDP::get_binary_data() {
    const size_t packet_size = 96;

    std::unique_lock<std::mutex> lock(data_mutex_);
    
    data_cv_.wait(lock, [this, packet_size]{
        return !is_running_ || received_data_.size() >= packet_size;
    });

    if (!is_running_) {
        return {};
    }

    std::vector<uint8_t> data_copy(received_data_.begin(), received_data_.begin() + packet_size);
    received_data_.erase(received_data_.begin(), received_data_.begin() + packet_size);
    
    is_data_ready_ = false;
    
    return data_copy;
}

} // namespace balloon