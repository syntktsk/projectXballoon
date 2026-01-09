#include "SocketTransceiver.hh"
#include "TelemetryStruct.hh"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip>
#include <string.h> // for memset, strerror
#include <vector>
#include <sys/time.h> // for SO_RCVTIMEO
#include <errno.h> // for errno and EAGAIN/EWOULDBLOCK
#include <cstring> // memcpy, memset
#include <optional>

namespace balloon{

constexpr int ID_NONE = 255;
constexpr int ID_Whole_TELEMETRY = 9;//私の考えが正しければこれが使われることはない
constexpr int ID_HK_TELEMETRY = 1;
constexpr int ID_GNSS_TELEMETRY = 2;
constexpr int ID_Elmo_TELEMETRY = 3; // "ess" データ
constexpr int ID_Relay_TELEMETRY = 4;
constexpr int ID_ER_RESPONSE = 5;   // "er" データ
constexpr int MAX_BUFFER_SIZE = 1500; // UDPパケットの最大サイズに合わせて適宜設定
static const long RECEIVE_TIMEOUT_MS = 10;

SocketTransceiver::SocketTransceiver(const std::string& ip, uint16_t port)
  : sock_fd_(-1) {
    serverIp_ = ip;
    port_ = port;
}

SocketTransceiver::~SocketTransceiver() {
    close_socket();
}

int SocketTransceiver::initialize(bool soul_bind) {
    if (serverIp_.empty() || port_ == 0) {
        std::cerr << "SocketTransceiver: Initialization error. Server IP or Port is not set." << std::endl;
        return -1;
    }
    return open_socket(soul_bind) ? 0 : -1;
}

// bool SocketTransceiver::open_socket(bool soul_bind) {
//     if (sock_fd_ != -1) {
//         std::cerr << "Already open." << std::endl;
//         return false;
//     }
//     sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sock_fd_ < 0) {
//         std::cerr << "Failed to create socket." << std::endl;
//         return false;
//     }
//     struct timeval tv; //タイムアウトの設定
//     tv.tv_sec = RECEIVE_TIMEOUT_MS / 1000;
//     tv.tv_usec = (RECEIVE_TIMEOUT_MS % 1000) * 1000; 
//     if (setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
//         std::cerr << "Failed to set socket timeout." << std::endl;
//         close(sock_fd_);
//         sock_fd_ = -1;
//         return false;
//     }
//     struct sockaddr_in local_addr;//受信アドレス
//     memset(&local_addr, 0, sizeof(local_addr));
//     local_addr.sin_family = AF_INET;
//     local_addr.sin_port = htons(port_); 
//     if (inet_pton(AF_INET, serverIp_.c_str(), &local_addr.sin_addr) <= 0) {
//         std::cerr << "Invalid local address or address not supported." << std::endl;
//         close(sock_fd_);
//         sock_fd_ = -1;
//         return false;
//     }
//     if (soul_bind) {
//     if (bind(sock_fd_, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
//         std::cerr << "Failed to bind socket to port " << port_ << ": " << strerror(errno) << std::endl;
//         close(sock_fd_);
//         sock_fd_ = -1;
//         return false;
//     }}
//     std::cout << "Successfully opened UDP socket to" << serverIp_ << ":" << port_ <<std::endl;
//     return true;
// }
bool SocketTransceiver::open_socket(bool soul_bind) {
    // すでに開いている場合は二重に開かない
    if (sock_fd_ != -1) {
        std::cerr << "Already open." << std::endl;
        return false;
    }
        
    // UDPソケットの作成
    sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd_ < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }

    // タイムアウトの設定
    struct timeval tv;
    tv.tv_sec = RECEIVE_TIMEOUT_MS / 1000;
    tv.tv_usec = (RECEIVE_TIMEOUT_MS % 1000) * 1000; 
    if (setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        std::cerr << "Failed to set socket timeout." << std::endl;
        close(sock_fd_);
        sock_fd_ = -1;
        return false;
    }

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port_); // 9998 または 9090
    
    // --- ここからが修正のキモ ---
    
    if (soul_bind) {
        // 【受信待ち（bind）モード】
        // 相手のIPを指定してbindするとエラーになるので、INADDR_ANY（自分の全IP）を使う
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
        
        if (bind(sock_fd_, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
            std::cerr << "Failed to bind socket to port " << port_ << ": " << strerror(errno) << std::endl;
            close(sock_fd_);
            sock_fd_ = -1;
            return false;
        }
        std::cout << "Successfully bound socket to port " << port_ << " (Ready to receive)" << std::endl;
    } else {
        // 【送信専用モード】
        // bindせずに、送り先（Macなど）のアドレスが正しいかチェックだけ行う
        if (inet_pton(AF_INET, serverIp_.c_str(), &local_addr.sin_addr) <= 0) {
            std::cerr << "Invalid local address or address not supported: " << serverIp_ << std::endl;
            close(sock_fd_);
            sock_fd_ = -1;
            return false;
        }
        std::cout << "Successfully opened UDP socket for sending to " << serverIp_ << ":" << port_ << std::endl;
    }

    return true;
}

void SocketTransceiver::close_socket() {    
    if (sock_fd_ != -1) {
        close(sock_fd_);
        sock_fd_ = -1; // ソケットを閉じたら無効にする
    }
    std::cout << "Socket: Closed." << std::endl;
}

int SocketTransceiver::sendBinary(const std::vector<uint8_t>& data){
    if (sock_fd_ < 0) {
        std::cerr << "Socket not open." << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, serverIp_.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address." << std::endl;
        return -1;
    }

    // sendto() で宛先を指定して送信
    ssize_t bytes_sent = ::sendto(sock_fd_, data.data(), data.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (bytes_sent < 0) {
        std::cerr << "Failed to send command." << std::endl;
        return -1;
    }
    
    if ((size_t)bytes_sent != data.size()) {// 全て送信できなかった場合
         std::cerr << "Warning: Only sent " << bytes_sent << " of " << data.size() << " bytes." << std::endl;
    }

    return static_cast<int>(bytes_sent);
}

int SocketTransceiver::sendASCII(std::string& command) {
    if (sock_fd_ < 0) {
        std::cerr << "Socket not open." << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, serverIp_.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address." << std::endl;
        return -1;
    }

    // sendto() で宛先を指定して送信
    ssize_t bytes_sent = ::sendto(sock_fd_, command.data(), command.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (bytes_sent < 0) {
        std::cerr << "Failed to send command." << std::endl;
        return -1;
    }
    
    if ((size_t)bytes_sent != command.size()) {// 全て送信できなかった場合
         std::cerr << "Warning: Only sent " << bytes_sent << " of " << command.size() << " bytes." << std::endl;
    }

    return static_cast<int>(bytes_sent);
}

int SocketTransceiver::receiveBinary() {
    if (sock_fd_ < 0) {
        std::cerr << "Socket not open." << std::endl;
        return -1;
    }
    uint8_t buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    
    ssize_t bytes_read = ::recvfrom(
        sock_fd_, 
        buffer, 
        MAX_BUFFER_SIZE, 
        0, 
        (struct sockaddr*)&sender_addr, 
        &addr_len
    );
    if (bytes_read <= 0) {
        // タイムアウト (0) やソケットエラー (-1) の値をそのまま返す
        return (int)bytes_read; 
    }

    last_received_raw_data_.assign(buffer, buffer + bytes_read);
    return (int)bytes_read; 
}
} // namespace balloon

// int SocketTransceiver::receiveTelemetry() {
//     if (sock_fd_ < 0) {
//         std::cerr << "Socket not open." << std::endl;
//         return ID_NONE;
//     }

//     uint8_t buffer[MAX_BUFFER_SIZE];
//     struct sockaddr_in sender_addr;
//     socklen_t addr_len = sizeof(sender_addr);
    
//     // ソケットからデータを受け取る
//     ssize_t bytes_read = ::recvfrom(
//         sock_fd_, 
//         buffer, 
//         MAX_BUFFER_SIZE, 
//         0, 
//         (struct sockaddr*)&sender_addr, 
//         &addr_len
//     );

//     if (bytes_read <= 0) {
//         return ID_NONE; // タイムアウトやエラーでデータなし
//     }

//     last_received_raw_data_.assign(buffer, buffer + bytes_read);
    
//     const size_t received_size = (size_t)bytes_read;

//     // -----------------------------------------------------------
//     // 識別ロジック (サイズとマジックナンバーによる識別)
//     // -----------------------------------------------------------

//     // A. サイズによる識別（最優先）
//     if (received_size == sizeof(ess0)) {
        
//         ess0 all_status;
//         // バッファから構造体へコピー
//         memcpy(&all_status, buffer, sizeof(ess1));
//         std::cout << "-> Identified ESS Telemetry (ID: " << ID_Whole_TELEMETRY << ") " << std::endl;
//         return ID_Whole_TELEMETRY; 

//         // 2. 確からしいフィールドをチェック (MFフィールドを例とする)
//         // **バイトオーダー変換を適用してから比較**
//         // uint16_t version = ntohs(elmo_status.MF); 
//         // if (version == 0xABCD) { // サーバー仕様で決まっている固定値
//         //     std::cout << "-> Identified ESS Telemetry (ID: " << ID_Elmo_TELEMETRY << ") " << std::endl;
//         //     return ID_Elmo_TELEMETRY; 
//         // }

//     }else if(received_size == sizeof(ess1)) {
        
//         ess1 elmo_status;
//         // バッファから構造体へコピー
//         memcpy(&elmo_status, buffer, sizeof(ess1));
//         std::cout << "-> Identified ESS Telemetry (ID: " << ID_Elmo_TELEMETRY << ") " << std::endl;
//         return ID_Elmo_TELEMETRY; 
//         // 2. 確からしいフィールドをチェック (MFフィールドを例とする)
//         // **バイトオーダー変換を適用してから比較**
//         // uint16_t version = ntohs(elmo_status.MF); 
//         // if (version == 0xABCD) { // サーバー仕様で決まっている固定値
//         //     std::cout << "-> Identified ESS Telemetry (ID: " << ID_Elmo_TELEMETRY << ") " << std::endl;
//         //     return ID_Elmo_TELEMETRY; 
//         // }

//     }else if (received_size == sizeof(ess2)) {
        
//         ess2 gnss_status;
//         // バッファから構造体へコピー
//         memcpy(&gnss_status, buffer, sizeof(ess2));
//         std::cout << "-> Identified ESS Telemetry (ID: " << ID_GNSS_TELEMETRY << ") " << std::endl;
//         return ID_GNSS_TELEMETRY;

//         // 2. 確からしいフィールドをチェック (MFフィールドを例とする)
//         // **バイトオーダー変換を適用してから比較**
//         // uint16_t version = ntohs(gnss_status.MF); 
//         // if (version == 0xABCD) { // サーバー仕様で決まっている固定値
//         //     std::cout << "-> Identified ESS Telemetry (ID: " << ID_GNSS_TELEMETRY << ") " << std::endl;
//         //     return ID_GNSS_TELEMETRY; 
//         // }

//     } else if (received_size == sizeof(er)) {
        
//         er optional_response;
//         // バッファから構造体へコピー
//         memcpy(&optional_response, buffer, sizeof(er));

//         std::cout << "-> Identified ER Response (ID: " << ID_ER_RESPONSE << ") " << std::endl;
//         return ID_ER_RESPONSE ; 
//     }
    
//     // -----------------------------------------------------------
//     // どのパターンにも一致しなかった場合
//     // -----------------------------------------------------------
//     std::cerr << "Unrecognized packet received (Size: " << received_size << " bytes)." << std::endl;
//     return ID_NONE; // 識別失敗
// }


