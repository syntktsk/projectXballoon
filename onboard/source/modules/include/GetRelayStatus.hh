/**
 * GetRelayStatus sample module of the ANL Next framework
 *
 * @author 
 * @date
 *
 */

#ifndef GetRelayStatus_H
#define GetRelayStatus_H 1

#include <anlnext/BasicModule.hh>
#include <cstdint>
#include "SendTelemetry.hh"

namespace balloon {

class SendTelemetry;

class GetRelayStatus : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetRelayStatus, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  GetRelayStatus();
  virtual ~GetRelayStatus();
protected:
  GetRelayStatus(const GetRelayStatus& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  int dummy(){return dummy_;}

private:
  std::string tempFile_;
  std::string path_;
  uint64_t capacityFree_ = 0;
  uint64_t capacityAll_ = 0;
  SendTelemetry* sendTelemetry_ = nullptr;
  int chatter_ = 0;
  int dummy_;
};

} /* namespace balloon */

#endif /* GetRaspiStatus_H */
