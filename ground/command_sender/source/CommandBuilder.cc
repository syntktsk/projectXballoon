#include "CommandBuilder.hh"
#include <tuple>

namespace
{
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

} /* anonymous namespace */

namespace balloon
{

CommandBuilder::CommandBuilder()
{ 
  code_map_["Get_Raspi_Status"]        = CommandProperty{100, 0};
  code_map_["Reset_Error"]             = CommandProperty{101, 0};
  code_map_["Exec_Shutdown"]           = CommandProperty{102, 0};
  code_map_["Exec_Reboot"]             = CommandProperty{103, 0};
  code_map_["Prepare_Shutdown"]        = CommandProperty{104, 0};
  code_map_["Prepare_Reboot"]          = CommandProperty{105, 0};
  code_map_["Get_Relay_Status"]        = CommandProperty{106, 0};
  code_map_["Exec_Software_Stop"]      = CommandProperty{198, 0};
  code_map_["Prepare_Software_Stop"]   = CommandProperty{199, 0};
  code_map_["Elmo_control"]            = CommandProperty{200, 0};
  code_map_["Get_All_Status"]          = CommandProperty{300, 0};
  code_map_["Get_GNSS_Status"]         = CommandProperty{302, 0};
  code_map_["Get_Elmo_Status"]         = CommandProperty{301, 0};
  code_map_["Parameter_Set"]           = CommandProperty{400, 0};
  code_map_["Setting_TC[A]"]           = CommandProperty{501, 1};
  code_map_["Setting_JV[]"]            = CommandProperty{502, 1};
  code_map_["Setting_PA[]"]            = CommandProperty{503, 1};
  code_map_["Setting_PR[]"]            = CommandProperty{504, 1};
  code_map_["Setting_MO"]              = CommandProperty{505, 1};
  code_map_["Setting_UM"]              = CommandProperty{506, 1};
  code_map_["Optional_Command"]        = CommandProperty{599, 1};
  code_map_["Reset_gl860"]             = CommandProperty{600, 0};
  // code_map_["Reset_gl860"]             = CommandProperty{601, 0};
  code_map_["Stat_gl860"]              = CommandProperty{602, 0};
  code_map_["Optional_gl860"]          = CommandProperty{699, 1};
  code_map_["Other"]                   = CommandProperty{900, 0};
}

CommandProperty CommandBuilder::get_command_property(const std::string& name) const
{
  auto command = code_map_.find(name);
  if (command == code_map_.end()) {
    throw CommandException("Invalid command name");
  }
  
  return command->second;
}

uint16_t CommandBuilder::get_command_code(const std::string& name) const
{
  return get_command_property(name).code;
}

int CommandBuilder::get_argnum(const std::string& name) const
{
  return get_command_property(name).argnum;
}

std::vector<uint8_t> CommandBuilder::make_byte_array(const std::string& name, const std::vector<std::string>& arg_array) const
{
  std::vector<uint8_t> command;
  command.push_back(0xEB);
  command.push_back(0x90);

  const CommandProperty property = get_command_property(name);
  const uint16_t code = property.code;
  const int argnum = property.argnum;
  command.push_back((code & 0xFF00u) >> 8);
  command.push_back((code & 0x00FFu) >> 0);
  command.push_back((argnum & 0xFF00u) >> 8);
  command.push_back((argnum & 0x00FFu) >> 0);

  if (argnum != static_cast<int>(arg_array.size())) {
    throw CommandException("Invalid argument number");
  }
// --- 修正箇所：argnum > 0 の場合のみ引数処理を行う ---
  if (argnum > 0) {
    // 必要なサイズを16バイトに定義
    const size_t REQUIRED_SIZE = 16; 

    // 最初の引数 (statusgetterelmo) を取得
    const std::string& arg = arg_array[0]; 

    if (arg.size() > REQUIRED_SIZE) {
        throw CommandException("Argument size exceeds 16 bytes");
    }

    // --- 文字列をバイト列に変換 ---
    for (char c : arg) {
        command.push_back(static_cast<uint8_t>(c));
    }

    // --- ヌル文字 (\0) によるパディング処理 ---
    size_t padding_size = REQUIRED_SIZE - arg.size();
    for (size_t i = 0; i < padding_size; ++i) {
        command.push_back(static_cast<uint8_t>('\0')); // 0x00
    }
  }
  
  const uint16_t crc = crc_calc(command);
  command.push_back((crc & 0xFF00u) >> 8);
  command.push_back((crc & 0x00FFu) >> 0);

  // termination word C5A4
  command.push_back(0xC5);
  command.push_back(0xA4);
  
  return command;
}

} /* namespace balloon */
