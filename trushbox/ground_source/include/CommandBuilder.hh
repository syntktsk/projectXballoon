#ifndef BALLOON_COMMANDBUILDER_H
#define BALLOON_COMMANDBUILDER_H 1

#include <cstdint>
#include <vector>
#include <map>
#include <string>

namespace balloon
{

struct CommandException
{
  CommandException(const std::string& info) { info_ = info; }
  std::string print() const { return info_; }
  
private:
  std::string info_;
};

struct CommandProperty
{
  uint16_t code = 0;
  int argnum = 0;
};

class CommandBuilder
{
public:
  CommandBuilder();
  ~CommandBuilder() = default;

  CommandProperty get_command_property(const std::string& name) const;
  uint16_t get_command_code(const std::string& name) const;
  int get_argnum(const std::string& name) const;
  template<typename T> void getByte(std::vector<uint8_t>& command, const T& value) const;

  std::vector<uint8_t> make_byte_array(const std::string& name, const std::string& arg_string) const;
  std::vector<uint8_t> make_ascii(const std::string& name, const std::vector<int32_t>& arg_array) const;

private:
  std::map<std::string, CommandProperty> code_map_;
};


template<typename T>
void CommandBuilder::getByte(std::vector<uint8_t>& command, const T& value) const
{
    const size_t byte_size = sizeof(T);
    const uint8_t* byte_ptr = reinterpret_cast<const uint8_t*>(&value);

    for (size_t i = 0; i < byte_size; ++i) {
        command.push_back(byte_ptr[i]);
    }
}

} /* namespace balloon */

#endif /* BALLOON_COMMANDBUILDER_H */
