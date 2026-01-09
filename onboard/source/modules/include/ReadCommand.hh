#ifndef ReadCommand_H
#define ReadCommand_H 1

#include <iostream>
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <cstring>  // memcpy のため
#include <cmath> 
#include <type_traits>

namespace balloon {

struct CommandProperty
{
  uint16_t code = 0;
  int argnum = 0;
};

class ReadCommand {
public:
    ReadCommand();
    ~ReadCommand();

    std::string getCommand(const std::vector<uint8_t>& cmd);
    void extract_command(const std::vector<uint8_t>& cmd);
    void getCommand_string(const std::vector<uint8_t>& code_vec, const std::vector<uint8_t>& arg_vec);
    std::string StringCommand() { return StringCommand_; }
    uint8_t OperationNo() { return OperationNo_ ; }
    template <typename T>
    T getValue(const std::vector<unsigned char>& x) {
        if constexpr (std::is_same_v<T, std::string>) {
            // 文字列(std::string)の場合：0x00が出るまでを文字として結合
            std::string s;
            for (auto byte : x) {
                if (byte == 0x00) break; 
                s += static_cast<char>(byte);
            }
            return s;
        } else {
            // 数値型(int, float等)の場合：今まで通りのバイナリコピー
            T value = 0;
            if (x.size() >= sizeof(T)) {
                memcpy(&value, x.data(), sizeof(T));
            }
            
            // 浮動小数点ならNaNチェックをする
            if constexpr (std::is_floating_point_v<T>) {
                if (std::isnan(value)) value = 0;
            }
            return value;
        }
    }
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


}//namespace balloon
#endif // ReadCommand
