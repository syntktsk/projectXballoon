#ifndef BALLOON_COMMANDSENDER_H
#define BALLOON_COMMANDSENDER_H 1

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "CommandBuilder.hh" // コマンド構築クラスのヘッダーをインクルード
#include <thread>


namespace balloon
{

class CommandSender
{
public:
  CommandSender(const std::string& serverIp, int port);
  ~CommandSender() = default;


  std::string serverIp() const { return serverIp_; }
  bool open_socket();
  void close_socket();
  int send(const std::vector<uint8_t>& byte_array);

  std::vector<uint8_t> CommandBuild(const std::string& name, const std::vector<int32_t>& arg_array);

private:
  std::string serverIp_;
  int port_= 0;
  int sock_fd_;

  CommandBuilder commandBuilder_;  
};

} /* namespace balloon */

#endif /* BALLOON_COMMANDSENDER_H */