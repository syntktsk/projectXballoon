#ifndef GetGL860Data_H
#define GetGL860Data_H 1

#include <anlnext/BasicModule.hh>
#include <cstdint>
#include <string>
#include <vector>
#include "SendTelemetry.hh"
#include "GL860main.hh"

namespace balloon {

class SendTelemetry;
class GL860main;

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
  int getCapacity();
  float Value() const { return value_; }
  void interpretGL860(const std::string& data);
  void writeDataToFile();
  std::vector<int16_t> getGL860DataVec() {return latestAnalogData_;}
  
  std::string lastCommandGL860(){return lastCommandGL860_;}
  std::string lastReceivedOptionGL860() {return lastReceivedGL860_;} //singleton_selfがあんま分かってないヨ
  void setGL860Command(std::string v) { lastCommandGL860_ = v; }
  void setDoYouHaveGL860(bool v) { IhaveGL860_ = v; }
  void setLastReceivedOpyionGL860(std::string v){ lastReceivedGL860_ = v;}
  void sendGL860(std::string cmd){ lastReceivedGL860_ = GL860main_ -> sendAndReceive(cmd);}
  bool IhaveGL860(){return IhaveGL860_;}

  private:
  std::shared_ptr<GL860main> GL860main_  = nullptr;
  std::vector<int16_t> getGL860Vec_;
  std::vector<int16_t> latestAnalogData_;
  std::string RawData_;
  std::string tempFile_;
  std::string glTimeStr_;
  std::string path_;
  std::string filename_;

  uint64_t capacityFree_ = 0;
  uint64_t capacityAll_ = 0;

  SendTelemetry* sendTelemetry_ = nullptr;
  // GL860main* GL860main_ = nullptr;
  std::vector<uint8_t> range_info_;
  
  std::string ip_;
  int port_;
  std::string lastCommandGL860_="";
  std::string lastReceivedGL860_="";
  bool IhaveGL860_ = false;
  int DataID_ = 0;
  int numPerFile_ = 2*60*30;
  int currentFileID_ = 0;
  int lineCount_ = 0;
  int chatter_ = 0;
  float value_;
};

} /* namespace balloon */

#endif /*GetGL860Data_H*/