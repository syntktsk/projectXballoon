%module Balloon
%{
#include <anlnext/BasicModule.hh>

# include "MongoDBClient.hh" //追加した 
# include "TelemetryReceiver.hh"
# include "ReceiveCommand.hh"
# include "SPIManager.hh"
# include "GetRaspiStatus.hh"
# include "GetGL860Data.hh"
# include "SendTelemetry.hh"
# include "RelayControl.hh"
# include "RunIDManager.hh"
# include "ShutdownSystem.hh"
# include "InterpretTelemetry.hh"
// # include "WaitFor.hh"
# include "ReceiveEUResponse.hh"
%}

%import(module="anlnext/ANL") "anlnext/ruby/ANL.i"

// interface to my modules

//class MyModule : public anlnext::BasicModule {};
//class MyMapModule : public anlnext::BasicModule {};
//class MyVectorModule : public anlnext::BasicModule {};
//追加したよ
namespace balloon{
	class MongoDBClient : public anlnext::BasicModule {};
    class TelemetryReceiver : public anlnext::BasicModule {};
    class ReceiveCommand : public anlnext::BasicModule {};
    class GetRaspiStatus : public anlnext::BasicModule {};
    class SPIManager : public anlnext::BasicModule {};
    class GetGL860Data : public anlnext::BasicModule {};
    class SendTelemetry : public anlnext::BasicModule {};
    class RelayControl : public anlnext::BasicModule {};
    class RunIDManager : public anlnext::BasicModule {};
    class ShutdownSystem : public anlnext::BasicModule {};
    class InterpretTelemetry : public anlnext::BasicModule {};
    class ReceiveEUResponse : public anlnext::BasicModule {};
}
//ここまで
