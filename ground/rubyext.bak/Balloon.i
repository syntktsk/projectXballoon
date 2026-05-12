%module Balloon
%{
#include <anlnext/BasicModule.hh>

// include headers of my modules
//#include "MyModule.hh"
//#include "MyMapModule.hh"
//#include "MyVectorModule.hh"
# include "MongoDBClient.hh" //追加した 
# include "TelemetryReceiver.hh"
#include "TelemetryReceiver_UDP.hh"
# include "WaitFor.hh"
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
	class TelemetryReceiver_UDP : public anlnext::BasicModule {};
	class WaitFor : public anlnext::BasicModule {};
}
//ここまで
