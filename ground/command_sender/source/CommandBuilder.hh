#ifndef COMMANDBUILDER_H
#define COMMANDBUILDER_H 1

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

  std::vector<uint8_t> make_byte_array(const std::string& name, const std::vector<std::string>& arg_array) const;

private:
  std::map<std::string, CommandProperty> code_map_;
};

} /* namespace balloon */

#endif /* COMMANDBUILDER_H */
