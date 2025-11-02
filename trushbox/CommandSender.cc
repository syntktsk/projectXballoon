#include "CommandSender.hh"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <cstdint>
#include <thread>
#include <chrono>

namespace balloon {

// コンストラクタ
CommandSender::CommandSender(const std::string& serverIp, int port)
    : serverIp_(serverIp), port_(port), sock_fd_(-1), commandBuilder_() {
    // コンストラクタでは、メンバ変数の初期化のみを行う
};

// 接続の確立
bool CommandSender::open_socket() {
    std::cout << "CommandSender: Attempting to connect to " << serverIp_ << ":" << port_ << std::endl;

    // ソケットの作成
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) {
        std::cerr << "CommandSender: Failed to create socket." << std::endl;
        return false;
    }

    // サーバーのアドレス設定
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    
    // IPアドレスの文字列をバイナリ形式に変換
    if (inet_pton(AF_INET, serverIp_.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "CommandSender: Invalid address or address not supported." << std::endl;
        close(sock_fd_);
        return false;
    }

    // サーバーへの接続
    if (connect(sock_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "CommandSender: Connection failed." << std::endl;
        close(sock_fd_);
        return false;
    }

    std::cout << "CommandSender: Successfully connected to " << serverIp_ << ":" << port_ << std::endl;
    return true;
}

// ソケットを閉じる
void CommandSender::close_socket() {
    if (sock_fd_ != -1) {
        close(sock_fd_);
        sock_fd_ = -1;
    }
}

// コマンドの作成
std::vector<uint8_t> CommandSender::CommandBuild(const std::string& name, const std::vector<int32_t>& arg_array){
  
  std::vector<uint8_t> status_command_bytes = commandBuilder_.make_byte_array(name, arg_array);
  std::cout << "Generated" << name << "command with size: " << status_command_bytes.size() << " bytes" << std::endl;
    
    return status_command_bytes;
}


// データの送信
int CommandSender::send(const std::vector<uint8_t>& byte_array) {
    if (sock_fd_ == -1) {
        std::cerr << "CommandSender: Socket is not open." << std::endl;
        return -1;
    }
    // ソケットへの書き込み
    const int bytes_sent = write(sock_fd_, byte_array.data(), byte_array.size());
    if (bytes_sent < 0) {
        std::cerr << "CommandSender: Failed to send data." << std::endl;
    }
    return bytes_sent;
}

} // namespace balloon