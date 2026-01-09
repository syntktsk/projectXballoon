%module Balloon

%{
#include <anlnext/BasicModule.hh>

# include "MongoDBClient.hh" //追加した 
# include "TelemetryReceiver.hh"
# include "ReceiveCommand.hh"
# include "SPIManager.hh"
# include "GetRaspiStatus.hh"
# include "SendTelemetry.hh"
# include "RunIDManager.hh"
# include "ShutdownSystem.hh"
# include "InterpretTelemetry.hh"
// # include "WaitFor.hh"
%}

%import(module="anlnext.anlnextpy") "anlnext/python/anlnextpy.i"

// interface to my modules

namespace balloon{
    class MongoDBClient : public anlnext::BasicModule {};
    class TelemetryReceiver : public anlnext::BasicModule {};
    class ReceiveCommand : public anlnext::BasicModule {};
    class SPIManager : public anlnext::BasicModule {};
    class GetRaspiStatus : public anlnext::BasicModule {};
    class SendTelemetry : public anlnext::BasicModule {};
    class RunIDManager : public anlnext::BasicModule {};
    class ShutdownSystem : public anlnext::BasicModule {};
    class InterpretTelemetry : public anlnext::BasicModule {};
}

