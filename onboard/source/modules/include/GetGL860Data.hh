#ifndef GetGL860Data_H
#define GetGL860Data_H 1

#include <anlnext/BasicModule.hh>
#include <cstdint>
#include "SendTelemetry.hh"

namespace balloon {

class SendTelemetry;

class GetGL860Data : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetGL860Data, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  GetGL860Data();
  virtual ~GetGL860Data();
protected:
  GetGL860Data(const GetGL860Data& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  int getCapacity();  // この一行を追加！ 
  float Value() const { return value_; }

  private:
  std::string tempFile_;
  std::string path_;
  uint64_t capacityFree_ = 0;
  uint64_t capacityAll_ = 0;
  SendTelemetry* sendTelemetry_ = nullptr;
  int chatter_ = 0;
  float value_;

};

} /* namespace balloon */

#endif /*GetGL860Data_H*/