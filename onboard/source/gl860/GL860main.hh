#ifndef GL860main_H
#define GL860main_H

#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

namespace balloon {

class GL860main {
public:
    GL860main();
    ~GL860main();

    bool initialize(const std::string& ip, int port);
    void finalize();
    std::string sendAndReceive(std::string cmd);
    std::vector<uint8_t> readBinary(int readBytes);
	std::string RawData() {return response;}
	std::vector<uint8_t> RangeInfo() { return range_info_; }
	std::vector<uint8_t> getRange();
	bool isConnected() const { return sock_ != -1; }

private:
    int sock_ = -1;
    static constexpr int BUFF_SIZE = 8192;
	std::string response;
    std::vector<uint8_t> range_info_{20, 0x0C};
};

struct RangeInfo {
    std::string label;
    int multiplier;
    uint8_t range_id;
};

// 電圧レンジの対応表
const std::vector<RangeInfo> RANGE_TABLE = {
    {"20MV", 1000000, 0x01}, 
    {"50MV", 400000, 0x02},
    {"100MV", 200000, 0x03},
    {"200MV", 100000, 0x04}, 
    {"500MV", 40000, 0x05}, 
    {"1V", 20000, 0x06},
    {"2V", 10000, 0x07},
    {"5V", 4000, 0x08}, 
	{"10V",2000, 0x09}, 
    {"20V", 1000, 0x0A},
    {"50V", 400, 0x0B},
    {"100V", 200, 0x0C},
    {"1-5V", 40000,0x0D}, 
    {"2000", 10, 0x11},
    {"500",  40, 0x12},
    {"100",  200, 0x13}
};

} /* namespace balloon */
#endif