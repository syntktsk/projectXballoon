#include "ReceiveCommand.hh"
#include <iostream>
#include <iomanip>

using namespace anlnext;

namespace balloon{

ReceiveCommand::ReceiveCommand()
  : baudrate_(B1200), openMode_(O_RDWR)
{
  serialPath_ = "/dev/null";
  binaryFilenameBase_ = "Command";
  comdef_ = std::make_shared<CommandDefinition>(); 
  buffer_.resize(bufferSize_);
}

ANLStatus ReceiveCommand::mod_define(){
// socketの確認
}

ANLStatus ReceiveCommand::mod_analyze(){
    Reader_.getCommand()
    std::string Command =  Reader_.StringCommand()
    std::cout <<"CommandTransfer: get command"<< Command << std::endl;

}


ANLStatus CommandTransfer::(){
}

}