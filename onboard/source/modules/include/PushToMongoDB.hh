#ifndef PushToMongoDB_H
#define PushToMongoDB_H 1

#include <anlnext/BasicModule.hh>
#include <hsquicklook/MongoDBClient.hh>
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

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  void pushWholeTelemetry();
  void pushHKTelemetry();
  void pushWFTelemetry();
  void pushStatusTelemetry();
  void pushGNSSTelemetry();
  void pushElmoTelemetry();
  void pushRelayTelemetry();
  void pushOptionalTelemetry();
  void pushGL860OptionalTelemetry();
  void pushWaveformImage(const std::vector<std::string>& keys, const std::vector<std::string>& image_filenames);

private:
  hsquicklook::MongoDBClient* mongodbClient_ = nullptr;
  InterpretTelemetry* interpreter_ = nullptr;
  int chatter_ = 0;
  int elc_ ;
  std::string lc_string ;

}; /* namespace balloon */

}
#endif /* PushToMongoDB_H */
