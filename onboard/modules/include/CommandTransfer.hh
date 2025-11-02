#ifndef CommandSender_H
#define CommandSender_H 1

#include "SocketTransceiver_TCP.hh"
#include "SocketTransceiver_UDP.hh"
#include "../../tool/include/ReceiveCommand.hh"

namespace balloon{

class CommandSender{
public:

private:
    SocketTransceiver_TCP Socket_TCP_;
    SocketTransceiver_UDP Socket_UDP_;
    ReceiveCommand Receiver_;
    
}

}