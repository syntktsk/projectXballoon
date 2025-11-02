#ifndef ReceiveCommand_H
#define ReceiveCommand_H 1

#include <iostream>
#include <cstdint>
#include <vector>
#include <map>
#include <string>

namespace balloon {

struct CommandProperty
{
  uint16_t code = 0;
  int argnum = 0;
};

class ReceiveCommand {
public:
    ReceiveCommand();
    ~ReceiveCommand();

    void getCommand(const std::vector<uint8_t>& cmd);
    void extract_command(const std::vector<uint8_t>& cmd);
    void getCommand_string(const std::vector<uint8_t>& code_vec, const std::vector<uint8_t>& arg_vec);
    std::string StringCommand() { return StringCommand_; }
    uint8_t OperationNo() { return OperationNo_ ; }
    template<typename T> T getValue(const std::vector<uint8_t>& arg);
    uint16_t crc_calc(std::vector<uint8_t> byte_array);

private:
    std::map<std::string, CommandProperty> code_map_;
    // std::map<CommandProperty, std::string> name_map_;
    std::vector<uint8_t> code_vec_;
    std::vector<uint8_t> arg_vec_;
    uint16_t code;
    float arg_float;
    uint16_t arg_int;
    std::string StringCommand_;
    uint8_t OperationNo_; //0:OUの処理　1:EUの引数なし　2:EUの引数あり
};

template<typename T>
T ReceiveCommand::getValue(const std::vector<uint8_t>& x)
{
  const int n = x.size();
  const int byte = sizeof(T);
  if (n == byte) {
    std::cerr << "ReceiveCommand::getValue error: out of range" << std::endl;
    return static_cast<T>(0);
  }
  T value;
   memcpy(&value, &x, sizeof(T));

  if (std::isnan(value)){
    value=0.0;
  }
  
  return value;
}

uint16_t crc_calc(std::vector<uint8_t> byte_array)
{
  uint16_t crc = 0;
  for (const uint8_t i: byte_array) {
    crc = crc ^ i;
    for (int j=0; j<8; j++) {
      if (crc & 1) {
        crc = (crc>>1)^0x8408;
      }
      else {
        crc >>= 1;
      }
    }
  }
  return crc;
}
}//namespace balloon
#endif // ReceiveCommand
