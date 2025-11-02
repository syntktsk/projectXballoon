#include "ReadTelemetry.hh"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

namespace balloon {

ReadTelemetry::ReadTelemetry(const std::string& hostIp, int port)
    : hostIp_(hostIp), port_(port), listen_sock_fd_(-1), client_sock_fd_(-1), is_running_(false) {
}

ReadTelemetry::~ReadTelemetry() {
    stop_listening();
}

bool ReadTelemetry::start_listening() {
    if (is_running_) {
        std::cerr << "Already listening." << std::endl;
        return false;
    }

    // サーバーソケットの作成
    listen_sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock_fd_ < 0) {
        std::cerr << "Failed to create listen socket." << std::endl;
        return false;
    }

    // ソケットアドレスの設定
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, hostIp_.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address." << std::endl;
        close(listen_sock_fd_);
        return false;
    }

    // ソケットをアドレスにバインド
    if (bind(listen_sock_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(listen_sock_fd_);
        return false;
    }

    // クライアントからの接続を待機
    if (listen(listen_sock_fd_, 5) < 0) {
        std::cerr << "Failed to listen." << std::endl;
        close(listen_sock_fd_);
        return false;
    }

    is_running_ = true;
    listen_thread_ = std::thread(&ReadTelemetry::listening_thread_func, this);

    std::cout << "Listening on " << hostIp_ << ":" << port_ << "..." << std::endl;
    return true;
}

void ReadTelemetry::stop_listening() {
    if (is_running_) {
        is_running_ = false;
        if (listen_thread_.joinable()) {
            // スレッドを強制終了するためにソケットを閉じる
            if (listen_sock_fd_ != -1) {
                shutdown(listen_sock_fd_, SHUT_RDWR);
                close(listen_sock_fd_);
            }
            listen_thread_.join();
        }
        if (client_sock_fd_ != -1) {
            close(client_sock_fd_);
            client_sock_fd_ = -1;
        }
        std::cout << "Stopped listening." << std::endl;
    }
}

void ReadTelemetry::listening_thread_func() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    
    while (is_running_) {
        std::cout << "Waiting for a new connection..." << std::endl;
        client_sock_fd_ = accept(listen_sock_fd_, (struct sockaddr*)&client_addr, &client_addr_size);
        if (client_sock_fd_ < 0) {
            if (is_running_) { // listenソケットが閉じられたことによるエラーでなければ表示
                std::cerr << "Failed to accept connection." << std::endl;
            }
            continue;
        }

        std::cout << "Client connected. Receiving data..." << std::endl;

        const size_t buffer_size = 1024;
        std::vector<uint8_t> buffer(buffer_size);
        ssize_t bytes_read;
        
        // データ受信ループ
        while ((bytes_read = read(client_sock_fd_, buffer.data(), buffer_size)) > 0) {
            std::lock_guard<std::mutex> lock(data_mutex_);
            received_data_.insert(received_data_.end(), buffer.begin(), buffer.begin() + bytes_read);
            std::cout << "Received " << bytes_read << " bytes." << std::endl;
        }

        if (bytes_read == 0) {
            std::cout << "Client disconnected gracefully." << std::endl;
        } else {
            std::cerr << "Failed to read data." << std::endl;
        }
        close(client_sock_fd_);
        client_sock_fd_ = -1;
    }
}

const std::vector<uint8_t> ReadTelemetry::get_binary_data() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    std::vector<uint8_t> data_copy = received_data_;
    received_data_.clear(); // データを取得したらクリアする
    return data_copy;
}

} // namespace balloon