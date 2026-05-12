%module Balloon

%{
#include <anlnext/BasicModule.hh>
#include "MongoDBClient.hh"
#include "InterpretTelemetry.hh"
#include "PushToMongoDB.hh"
#include "TelemetryDefinition.hh"
#include "ReceiveTelemetry.hh"

using namespace balloon; 
%}

%import(module="anlnext/ANL") "anlnext/ruby/ANL.i"

namespace balloon {
    class ReceiveTelemetry : public anlnext::BasicModule {};
    class MongoDBClient : public anlnext::BasicModule {};
    class InterpretTelemetry : public anlnext::BasicModule {};
    class PushToMongoDB : public anlnext::BasicModule {};
    class TelemetryDefinition : public anlnext::BasicModule {};
}