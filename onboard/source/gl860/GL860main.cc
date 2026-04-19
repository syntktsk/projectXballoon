#include "GL860main.hh"
#include <iostream>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

namespace balloon {

GL860main::GL860main() : sock_(-1) {}

GL860main::~GL860main() {
    finalize();
}

// bool GL860main::initialize(const std::string& ip, int port) {
//     sock_ = socket(AF_INET, SOCK_STREAM, 0);
//     if (sock_ < 0) {
//         std::cerr << "GL860: Failed to create socket. errno=" << errno << std::endl;
//         return false;
//     }
//     int opt = 1;
//     setsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
//     struct timeval tv;
//     tv.tv_sec = 2;
//     tv.tv_usec = 0;
//     setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
//     struct sockaddr_in addr;
//     memset(&addr, 0, sizeof(addr));
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(port);
//     addr.sin_addr.s_addr = inet_addr(ip.c_str());
//     if (::connect(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
//         std::cerr << "GL860main: Connection failed to " << ip << ":" << port << std::endl;
//         close(sock_);
//         sock_ = -1;
//         return false;
//     }
//     range_info_ =getRange();
//     if (range_info_.empty()){
//         std::cerr << "GL860main: range info is empty "<< std::endl;
//     }
//     return true;
// }
bool GL860main::initialize(const std::string& ip, int port) {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        std::cerr << "GL860: Failed to create socket." << std::endl;
        return false;
    }

    // --- お手本 (SerialCommunication) に合わせた実装 ---

    // 1. ソケットを非ブロッキングモードに設定（これで connect が待たなくなる）
    int flags = fcntl(sock_, F_GETFL, 0);
    if (fcntl(sock_, F_SETFL, flags | O_NONBLOCK) != 0) {
        std::cerr << "GL860main: fcntl failed" << std::endl;
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (::connect(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        if (errno == EINPROGRESS) {
            // --- ここで最大1秒だけ「接続完了」を待つ ---
            fd_set wset;
            FD_ZERO(&wset);
            FD_SET(sock_, &wset);

            struct timeval tv;
            tv.tv_sec = 1;  // 1秒待機
            tv.tv_usec = 0;

            // ソケットが書き込み可能になる（＝接続完了）のを待つ
            int ret = select(sock_ + 1, NULL, &wset, NULL, &tv);
            if (ret <= 0) { // タイムアウト(0) または エラー(-1)
                std::cerr << "GL860main: Connect timeout or error." << std::endl;
                close(sock_);
                sock_ = -1;
                return false;
            }

            // selectが成功しても、本当に繋がったか確認が必要
            int so_error;
            socklen_t len = sizeof(so_error);
            getsockopt(sock_, SOL_SOCKET, SO_ERROR, &so_error, &len);
            if (so_error != 0) {
                std::cerr << "GL860main: Connect failed with error: " << so_error << std::endl;
                close(sock_);
                sock_ = -1;
                return false;
            }
            // ここまで来れば接続成功！
        } else {
            std::cerr << "GL860main: Connect failed immediately." << std::endl;
            close(sock_);
            sock_ = -1;
            return false;
        }
    }
    fcntl(sock_, F_SETFL, flags);
    int opt = 1;
    setsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    range_info_ = getRange();
    return true;
}
void GL860main::finalize() {
    if (sock_ != -1) {
        close(sock_);
        sock_ = -1;
    }
}

std::string GL860main::sendAndReceive(std::string cmd) {
    if (sock_ < 0) return "";
    // コマンドにGL860指定の改行コード
    cmd += "\r\n";

    ssize_t sent_len = send(sock_, cmd.c_str(), cmd.length(), 0);
    if (sent_len != static_cast<ssize_t>(cmd.length())) {
        std::cerr << "GL860: Send error. Sent " << sent_len << "/" << cmd.length() << " bytes." << std::endl;
        return ""; 
    }

    std::string response = "";
    char buffer[BUFF_SIZE];
    while (true) {
        memset(buffer, 0, BUFF_SIZE);
        ssize_t len = recv(sock_, buffer, BUFF_SIZE - 1, 0);
        
        if (len < 0) {
            std::cerr << "GL860: Receive error (Timeout or Network error)." << std::endl;
            return ""; // エラーを報告
        }
        if (len == 0) {
            std::cerr << "GL860: Connection closed by device." << std::endl;
            finalize();
            return "";
        }
        response += std::string(buffer, len);       
        // 改行コードで一つのレスポンス
        if (response.find('\n') != std::string::npos) {
            break;
        }
    }    
    // 不要な改行コードを除去
    size_t pos;
    while((pos = response.find_first_of("\r\n")) != std::string::npos) {
        response.erase(pos, 1);
    }
    return response;
}

std::vector<uint8_t> GL860main::readBinary(int readBytes) {
    std::vector<uint8_t> data;
    if (sock_ < 0) return data;
    data.resize(readBytes);
    int totalRead = 0;
    // 指定バイト数に達するまでループ受信
    while (totalRead < readBytes) {
        ssize_t len = recv(sock_, reinterpret_cast<char*>(&data[totalRead]), readBytes - totalRead, 0);
        if (len <= 0) {
            std::cerr << "GL860: Binary read error at " << totalRead << "/" << readBytes << std::endl;
            data.resize(totalRead);
            break;
        }
        totalRead += len;
    }
    return data;
}

std::vector<uint8_t> GL860main::getRange() {
    std::vector<uint8_t> rangeList;
    rangeList.clear();

    for (int ch = 1; ch <= 20; ++ch) {
        uint8_t type_id = 0xFF; // 初期値（エラー）

        std::string range_res = sendAndReceive(":AMP:CH" + std::to_string(ch) + ":RANG?");
        std::string range_str = "";
        size_t last_space = range_res.find_last_of(' ');
        if (last_space != std::string::npos) {
            range_str = range_res.substr(last_space + 1);
        }
        for (const auto& info : RANGE_TABLE) {
            if (range_str == info.label) {
                type_id = info.range_id;
                break;
            }
        }
        if (type_id == 0xFF) {
            std::string temp_res = sendAndReceive(":AMP:CH" + std::to_string(ch) + ":TEMPR?");
            std::string temp_str = "";
            size_t t_last_space = temp_res.find_last_of(' ');
            
            if (t_last_space != std::string::npos) {
                temp_str = temp_res.substr(t_last_space + 1);
            }
            for (const auto& info : RANGE_TABLE) {
                if (temp_str == info.label) {
                    type_id = info.range_id;
                    break;
                }
            }
        }
        rangeList.push_back(static_cast<uint8_t>(ch));
        rangeList.push_back(type_id);
    }
    return rangeList;
}


} /* namespace balloon */