#include "CommandSender.hh"

#include <cstdint>
#include <vector>
#include <iostream>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sstream> 
#include <iomanip>
#include <cstdlib>

#define BAUDRATE B1200

#ifdef __APPLE__
#define TCSETS TIOCSETA
#define SERIAL_PORT "/dev/tty.usbserial-FTRTKBUS"
#else
#define SERIAL_PORT "/dev/ttyAMA0"
#endif

namespace balloon
{
static const long RECEIVE_TIMEOUT_MS = 500;

CommandSender::CommandSender()
:sock_fd_(-1){
  CommunicationType_ = 1;//1->Serial 2->Socket
  serial_port_ = SERIAL_PORT;
  
  ServerIp_ ="192.168.10.101";//ou
  port_ = 9090 ;
}

bool CommandSender::open_serial_port()
{
  std::cout << "CommandSender: opening " << serial_port_ << std::endl;

  fd_ = open(serial_port_.c_str(), O_RDWR | O_NONBLOCK);
  if (fd_ < 0) {
    std::cout << "CommandSender: open error" << std::endl;
    return false;
  }

  cfmakeraw(&tio_);
  
  tio_.c_cflag |= CREAD;
  tio_.c_cflag |= CLOCAL;
  tio_.c_cflag |= CS8;
  //tio_.c_cflag |= PARENB;
  tio_.c_cflag &= (~PARENB);
  //tio_.c_cflag |= CSTOPB;
  tio_.c_cflag &= (~CSTOPB);
  cfsetispeed(&tio_, BAUDRATE);
  cfsetospeed(&tio_, BAUDRATE);
  
  tcsetattr(fd_, TCSANOW, &tio_);
  int flags = fcntl(fd_, F_GETFL, 0);
  fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK); // reference: https://github.com/orbcode/orbuculum/issues/15
  return true;
}

void CommandSender::close_serial_port()
{
  close(fd_);
}

// int CommandSender::send(const std::vector<uint8_t>& byte_array)
// {
//   int rval = write(fd_, byte_array.data(), byte_array.size());
//   std::vector<uint8_t> flush_data(128, 0xFF); 
//   write(fd_, flush_data.data(), flush_data.size());
//   tcdrain(fd_);
//   usleep(300000); // 0.3秒あれば10バイトは絶対終わる
//   std::cout << "[DEBUG] Pushed " << byte_array.size() << " bytes with garbage." << std::endl;
//   return rval;
// }

int CommandSender::send(const std::vector<uint8_t>& byte_array)
{
    tcflush(fd_, TCIFLUSH);
    
    // 1. 全てのデータを確実に16進数文字列に変換
    std::stringstream ss;
    for(auto b : byte_array) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    std::string hex_str = ss.str();

    // 2. Pythonの実行
    std::string cmd = "python3 -c \"import serial,time; "
                      "s=serial.Serial('/dev/cu.usbserial-FTRTKBUS', 1200); "
                      "s.write(bytes.fromhex('" + hex_str + "')); "
                      "s.flush(); time.sleep(0.5); s.close()\"";
    
    int result = system(cmd.c_str());

    // --- 上の関数の仕様に合わせるための返り値調整 ---
    if (result == 0) {
        // Pythonが正常終了(0)なら、送ったサイズを返す
        std::cout << "[DEBUG] Python bypass success: " << byte_array.size() << " bytes sent." << std::endl;
        return static_cast<int>(byte_array.size());
    } else {
        // 何らかの理由でPythonが失敗したら、エラー(-1など)を返す
        std::cerr << "[ERROR] Python bypass failed with status: " << result << std::endl;
        return -1; 
    }
}
bool CommandSender::open_socket() {
    if (sock_fd_ != -1) {
        std::cerr << "CommandSender:Already open." << std::endl;
        return false;
    }

    // 1. UDPソケットの作成
    sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd_ < 0) {
        std::cerr << "CommandSender:Failed to create socket." << std::endl;
        return false;
    }

    // 2. 宛先アドレスの設定
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    
    if (inet_pton(AF_INET, ServerIp_.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "CommandSender:Invalid server IP address or address not supported." << std::endl;
        close(sock_fd_);
        sock_fd_ = -1;
        return false;
    }

    // 3. connect() を使ってソケットの宛先を固定
    if (connect(sock_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "CommandSender:Failed to connect (fixed destination): " << strerror(errno) << std::endl;
        close(sock_fd_);
        sock_fd_ = -1;
        return false;
    }

    std::cout << "CommandSender:Successfully opened fixed destination UDP socket to " << ServerIp_ << ":" << port_ << std::endl;
    return true;
}

int CommandSender::sendBySocket(const std::vector<uint8_t>& byte_array){
    if (sock_fd_ < 0) {
        std::cerr << "CommandSender:Socket not open." << std::endl;
        return -1;
    }

    // send() で固定された宛先に送信 (sendtoは不要)
    ssize_t bytes_sent = ::send(sock_fd_, byte_array.data(), byte_array.size(), 0);
    
    if (bytes_sent < 0) {
        std::cerr << "CommandSender:Failed to send command: " << strerror(errno) << std::endl;
        return -1;
    }
    
    if ((size_t)bytes_sent != byte_array.size()) {
         std::cerr << "CommandSender:Warning: Only sent " << bytes_sent << " of " << byte_array.size() << " bytes." << std::endl;
    }

    return static_cast<int>(bytes_sent);
}

void CommandSender::close_socket() {    
    if (sock_fd_ != -1) {
        close(sock_fd_);
        sock_fd_ = -1; // ソケットを閉じたら無効にする
    }
    std::cout << "CommandSender:Socket Closed." << std::endl;
}
} /* namespace balloon */
