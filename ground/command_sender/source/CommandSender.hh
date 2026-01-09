#ifndef COMMANDSENDER_H
#define COMMANDSENDER_H 1

#include <cstdint>
#include <vector>
#include <string>
#include <termios.h>

namespace balloon
{

class CommandSender
{
public:
  CommandSender();
  ~CommandSender() = default;
  int CommunicationType(){return CommunicationType_;}//1->Serial, 2->Socket
  
  void set_serial_port(const std::string& dev) { serial_port_ = dev; }
  std::string serial_port() const { return serial_port_; }
  bool open_serial_port();
  void close_serial_port();
  int send(const std::vector<uint8_t>& byte_array);

  int set_socket();
  bool open_socket();
  void close_socket();
  int sendBySocket(const std::vector<uint8_t>& byte_array);

private:
  std::string serial_port_;
  int fd_ = 0;
  struct termios tio_;

  int sock_fd_;
  std::string ServerIp_;
  int port_;

  int CommunicationType_;
};

} /* namespace balloon */

#endif /* COMMANDSENDER_H */
