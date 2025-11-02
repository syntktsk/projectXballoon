#ifndef BALLOON_PushToMongoDB_H
#define BALLOON_PushToMongoDB_H 1

#include <anlnext/BasicModule.hh>
#include "MongoDBClient.hh"
#include "InterpretTelemetry.hh"
#include "TelemetryDefinition.hh"


namespace balloon {

class MongoDBClient;
class InterpretTelemetry;

class PushToMongoDB : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(PushToMongoDB, 3.0);
public:
  PushToMongoDB();

  anlnext::ANLStatus mng_define();
  anlnext::ANLStatus mng_initialize();
  anlnext::ANLStatus mng_analyze();

  void pushHKTelemetry();
  void pushWFTelemetry();
  void pushStatusTelemetry();
  void pushWaveformImage(const std::vector<std::string>& keys, const std::vector<std::string>& image_filenames);

private:
  hsquicklook::MongoDBClient* mongodbClient_ = nullptr;
  // InterpretTelemetry* interpreter_ = nullptr;
  int chatter_ = 0;
};

} /* namespace balloon */

#endif /* BALLOON_PushToMongoDB_H */