#include "CommandBuilder.hh"
#include <tuple>
#include <iostream>
#include <iomanip>

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
  code_map_["Software"]             = CommandProperty{100, 0};
  code_map_["Elmo"]                 = CommandProperty{200, 0};
  code_map_["ReadStatus"]           = CommandProperty{300, 0};
  code_map_["Parameter"]            = CommandProperty{400, 0};
  code_map_["Set"]                  = CommandProperty{500, 1};
  code_map_["Optional"]             = CommandProperty{600, 1};
  code_map_["Other"]                = CommandProperty{900, 0};
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

std::vector<uint8_t> CommandBuilder::make_byte_array(const std::string& name, const std::string& arg_string) const
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

  if (argnum != static_cast<int>(arg_string.size())) {
    throw CommandException("Invalid argument number");
  }
  // argのStringをByte列にしてPush
  getByte<std::string>(command, arg_string);
  
  const uint16_t crc = crc_calc(command);
  command.push_back((crc & 0xFF00u) >> 8);
  command.push_back((crc & 0x00FFu) >> 0);

  // termination word C5A4
  command.push_back(0xC5);
  command.push_back(0xA4);
  
  return command;
}

} /* namespace balloon */
