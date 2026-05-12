#ifndef RelayControl_H
#define RelayControl_H

#include <anlnext/BasicModule.hh>
#include "SendTelemetry.hh"
#include <cstdint>
#include <vector>
#include <string>

namespace balloon{

class RelayControl: public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(RelayControl, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  RelayControl();
  virtual ~RelayControl() = default;
protected:
  RelayControl(const RelayControl& r) = default;
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  anlnext::ANLStatus mod_analyze() override;

  void ReadRelayStatus();
  void setChipSelect(int v) { chipSelect_ = v; }
  void setGPIOHandler(int v) { GPIOHandler_ = v; }
  void addChipSelect(int v);
  int ChipSelect() { return chipSelect_; }
  int GPIOHandler() { return GPIOHandler_; }
  uint32_t LatestStatus(){ return latestStatus_; }
  std::vector<int> ChipSelectArray(){ return chipSelectArray_ ;}
  void RelayOn(int gpio);
  void RelayOff(int gpio);
  void AllOff();

private:
  int pi;
  uint32_t relays_mask_ = 0;
  int chipSelect_ = -1;
  int GPIOHandler_ = -1;
  int channel_ = 0;
  int baudrate_ = 100000;
  int Flags_ = (1<<5) + (1<<6);
  const std::string portNumber_ = "8888";
  std::vector<int> chipSelectArray_ = {22,23,24,27};/*pinの番号リスト*/
  SendTelemetry* sendTelemetry_ = nullptr;
  int chatter_ = 0;
  uint32_t latestStatus_ = 0;
  const int RELAY_ON  = 1; // 自分のリレーに合わせて決める
  const int RELAY_OFF = 0;
};
}/*namespace balloon*/ 
#endif