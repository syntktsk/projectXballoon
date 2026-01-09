#ifndef ReceiveCommand_H
#define ReceiveCommand_H 1

#include "SocketTransceiver_TCP.hh"
#include "SocketTransceiver_UDP.hh"
#include "../../tool/include/ReadCommand.hh"

namespace balloon{

class ReceiveCommand{
public:

private:
    SocketTransceiver_TCP Socket_TCP_;
    SocketTransceiver_UDP Socket_UDP_;
    ReadCommand Reader_;
}

}