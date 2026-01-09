#include "ReadCommand.hh"
#include <vector>
#include <iostream>

namespace balloon{
ReadCommand::ReadCommand()
{
  code_map_["Get_Raspi_Status"]        = CommandProperty{100, 0};
  code_map_["Reset_Error"]             = CommandProperty{101, 0};
  code_map_["Exec_Shutdown"]           = CommandProperty{102, 0};
  code_map_["Exec_Reboot"]             = CommandProperty{103, 0};
  code_map_["Prepare_Shutdown"]        = CommandProperty{104, 0};
  code_map_["Prepare_Reboot"]          = CommandProperty{105, 0};
  code_map_["Exec_Software_Stop"]      = CommandProperty{198, 0};
  code_map_["Prepare_Software_Stop"]   = CommandProperty{199, 0};
  code_map_["Elmo_control"]            = CommandProperty{200, 0};
  code_map_["Get_All_Status"]          = CommandProperty{300, 0};
  code_map_["Get_HK_Status"]           = CommandProperty{301, 0};
  code_map_["Get_GNSS_Status"]         = CommandProperty{302, 0};
  code_map_["Get_Elmo_Status"]         = CommandProperty{303, 0};
  code_map_["Get_Relay_Status"]        = CommandProperty{304, 0};
  code_map_["Parameter_Set"]           = CommandProperty{400, 0};
  code_map_["Setting_TC[A]"]           = CommandProperty{501, 1};
  code_map_["Setting_JV[]"]            = CommandProperty{502, 1};
  code_map_["Setting_PA[]"]            = CommandProperty{503, 1};
  code_map_["Setting_PR[]"]            = CommandProperty{504, 1};
  code_map_["Optional_Command"]        = CommandProperty{600, 1};
  code_map_["Other"]                   = CommandProperty{900, 0};
}
 
std::string ReadCommand::getCommand(const std::vector<uint8_t>& cmd_){
    // 最初に切り離す
    ReadCommand::extract_command(cmd_);
    ReadCommand::getCommand_string(code_vec_, arg_vec_);
    return StringCommand_;
};


void ReadCommand::extract_command(const std::vector<uint8_t>& cmd_) {
    // コマンドの構造:Header2 + code2 + argc2 + 引数本体？？ + CRC2 + Footer2 = min10Byte
    const size_t MIN_CMD_SIZE = 10;
    if (cmd_.size() < MIN_CMD_SIZE) {
        throw std::runtime_error("Command size too small.");
    }
    // Header & Footer check
    if (cmd_[0] != 0xEB || cmd_[1] != 0x90) {
        throw std::runtime_error("Invalid command header (0xEB90 expected).");
    }
    if (cmd_[cmd_.size() - 2] != 0xC5 || cmd_[cmd_.size() - 1] != 0xA4) {
        std::cerr << "Warning: Invalid command terminator (0xC5A4 expected)." << std::endl;
    }

    const size_t PAYLOAD_END_INDEX = cmd_.size() - 4;
    std::vector<uint8_t> payload_for_crc;
    payload_for_crc.insert(payload_for_crc.end(), cmd_.begin(), cmd_.begin() + PAYLOAD_END_INDEX);
    
    uint16_t received_crc = (static_cast<uint16_t>(cmd_[PAYLOAD_END_INDEX]) << 8) | cmd_[PAYLOAD_END_INDEX + 1];

    // crcをチェック
    if (crc_calc(payload_for_crc) != received_crc) {
        throw std::runtime_error("CRC mismatch detected.");
    }else {
        code_vec_.clear();
    code_vec_.push_back(cmd_[2]);
    code_vec_.push_back(cmd_[3]);
    
    // 引数はindex6からスタート
    const size_t ARG_START_INDEX = 6;
    arg_vec_.clear();
    if (PAYLOAD_END_INDEX >= ARG_START_INDEX) {
        arg_vec_.insert(arg_vec_.end(), cmd_.begin() + ARG_START_INDEX, cmd_.begin() + PAYLOAD_END_INDEX);
    }   
    //arg_vec_は、引数の数(argnum)が0の場合は空
    }
}

void ReadCommand::getCommand_string(const std::vector<uint8_t>& code_vec, const std::vector<uint8_t>& arg_vec) {
       // EUに送る文字列に変換する
    code = getValue<uint16_t>(code_vec);
    std::string code_str = std::to_string(static_cast<int>(code));
    int code_int = static_cast<int>(code) % 100;
    std::string code_int_str = std::to_string(static_cast<int>(code_int));

    if (code >= 100 && code <= 199) {
        return;
    } else if (code >= 200 && code <= 299) {
        StringCommand_ = "ac" + code_int_str;
        return;
    } else if (code >= 300 && code <= 399) {
        StringCommand_ = "rs" + code_int_str;
        return;
    } else if (code >= 400 && code <= 499) {
        StringCommand_ = "ps" + code_int_str;
        return;
    } else if (code >= 500 && code <= 599) {
        // 1. まずバイナリを数値(floatやint)として復元する
        // 地上局側の送信型に合わせて float か int か選んでください
        int arg_val = getValue<int>(arg_vec);       
        // 2. 数値を文字列に変換する
        // atoiに合わせるなら整数にする必要があります
        std::string arg_str = std::to_string(static_cast<int>(arg_val));
        if (code == 501)      StringCommand_ = "TC" + arg_str;
        else if (code == 502) StringCommand_ = "JV" + arg_str;
        else if (code == 503) StringCommand_ = "PA" + arg_str;
        else if (code == 504) StringCommand_ = "PR" + arg_str;
    }else if (code == 600) {
        std::string arg_str = getValue<std::string>(arg_vec);
        StringCommand_ = "wc" + arg_str;
        std::cout << "Sending to EU by Read: " << StringCommand_ << std::endl;
        return;
    }else if (code >= 900 && code <= 999) {
        std::string arg_str = getValue<std::string>(arg_vec);
        // arg_int = getValue<uint16_t>(arg_vec);
        // std::string arg_str = std::to_string(static_cast<int>(arg_int));
        StringCommand_ = arg_str;
        return;
    } else {
        StringCommand_ = "[UNKNOWN] " + code_str;
        return;}
}

uint16_t ReadCommand::crc_calc(std::vector<uint8_t> byte_array)
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
}