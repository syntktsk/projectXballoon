%module Balloon

%{
#include <anlnext/BasicModule.hh>

#include "MongoDBClient.hh"  
#include "TelemetryReceiver.hh"
#include "TelemetryReceiver_UDP.hh"
#include "Waitfor.hh"
%}

%import(module="anlnext.anlnextpy") "anlnext/python/anlnextpy.i"

// interface to my modules

namespace balloon{
        class MongoDBClient : public anlnext::BasicModule {};
        class TelemetryReceiver : public anlnext::BasicModule {};
        class TelemetryReceiver_UDP : public anlnext::BasicModule {};
        class WaitFor : public anlnext::BasicModule {};
}
