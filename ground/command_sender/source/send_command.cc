#include <iostream>
#include <string>
#include "CommandSender.hh"
#include "CommandBuilder.hh"
#include "CommandSaver.hh"

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cout << "Usage: send_command <command name> <arg list>" << std::endl;
    return 0;
  }

  const std::string name(argv[1]);
  std::vector<std::string> arg_array;
  for (int i=2; i<argc; i++) {
    arg_array.push_back(argv[i]);
  }

  std::vector<uint8_t> command;
  balloon::CommandBuilder command_builder;
  try {
    command = command_builder.make_byte_array(name, arg_array);
  }
  catch (balloon::CommandException& e) {
    std::cout << "Command exception caught: " << e.print() << std::endl;
    return 1;
  }

  balloon::CommandSender sender;
  sender.set_serial_port("/dev/tty.usbserial-14410");
  if ( !sender.open_serial_port() ) {
    std::cout << "Serial port open error -> exit" << std::endl;
    return -1;
  }

  const int length_sent = sender.send(command);
  std::cout << "Length sent: " << length_sent << std::endl;

  balloon::write_command(command, name);

  sender.close_serial_port();
  
  return 0;
}
