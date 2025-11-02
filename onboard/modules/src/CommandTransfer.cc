#include "CommandTransfer.hh"
#include <iostream>
#include <iomanip>

using namespace anlnext;

namespace balloon{

CommandTransfer::CommandTransfer(){
}

ANLStatus CommandTransfer::mod_define(){
// socketの確認
}

ANLStatus CommandTransfer::mod_analyze(){
    Receiver_.getCommand()
    std::string Command =  Receiver_.StringCommand()
    std::cout <<"CommandTransfer: get command"<< Command << std::endl;

}


ANLStatus CommandTransfer::(){
}

}