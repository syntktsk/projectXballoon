#include "PushToMongoDB.hh"

#include <cstdint>
#include <boost/format.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "DocumentBuilder.hh"
// #include "ErrorManager.hh"


using namespace anlnext;

namespace balloon {

PushToMongoDB::PushToMongoDB()
{
}

ANLStatus PushToMongoDB::mng_define()
{
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus PushToMongoDB::mng_initialize()
{
  get_module_NC("MongoDBClient", &mongodbClient_);
  mongodbClient_->createCappedCollection("ProjectX", 1*1024*1024);

  get_module_NC("InterpretTelemetry", &interpreter_);

  return AS_OK;
}

ANLStatus PushToMongoDB::mng_analyze()
{
  if (interpreter_->CurrentTelemetryType()==1) {
    pushHKTelemetry();
  }
  else if (interpreter_->CurrentTelemetryType()==2) {
    pushWFTelemetry();
  }
  else if (interpreter_->CurrentTelemetryType()==3) {
    pushStatusTelemetry();
  }
  return AS_OK;
}

} /* namespace balloon */
