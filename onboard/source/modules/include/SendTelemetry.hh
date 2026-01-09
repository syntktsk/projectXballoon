/**
 * Regulate high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 */


#ifndef SendTelemetry_H
#define SendTelemetry_H 1

#include <anlnext/BasicModule.hh>
#include "TelemetryDefinition.hh"
#include "ErrorManager.hh"
// #include "MeasureTemperatureWithRTDSensor.hh"
#include "GetRaspiStatus.hh"
// #include "GetEnvironmentalData.hh"
#include "SerialCommunication.hh"
#include "SocketTransceiver.hh"
#include "ReceiveCommand.hh"
#include "RunIDManager.hh"
#include "GetRelayStatus.hh"
#include "ReceiveEUResponse.hh"

namespace balloon {

class GetRaspiStatus;
class GetRelayStatus;
class ReceiveCommand;
class RunIDManager;
class ReceiveEUResponse;
class GetGL860Data;

class SendTelemetry : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SendTelemetry, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  SendTelemetry();
  virtual ~SendTelemetry();
protected:
  SendTelemetry(const SendTelemetry& r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  void Sender();
  void inputInfo();
  void inputDetectorInfo();
  void inputHKVesselInfo();
  void inputSoftwareInfo();
  void inputStatusInfo();
  void inputEUInfo();
  void writeTelemetryToFile(bool failed);
  void setTelemetryType(int v) { singleton_self()->telemetryType_ = v; }
  int TelemetryType() { return singleton_self()->telemetryType_; }
  ErrorManager* getErrorManager() { return (singleton_self()->errorManager_).get(); }

private:
  std::shared_ptr<TelemetryDefinition> telemdef_ = nullptr;
  int telemetryType_ = 1;
  std::shared_ptr<ErrorManager> errorManager_ = nullptr;
  std::map<int, std::pair<int, int>> fileIDmp_;
  bool saveTelemetry_ = true;
  std::string binaryFilenameBase_ = "";

  int numTelemPerFile_ = 1000;
  static constexpr int sleepms_ = 500;
  int wfDivisionCounter_ = 0;
  int chatter_ = 0;

  void inputEUInfo_Elmo();
  void inputEUInfo_GNSS();
  void inputEUInfo_Option();
  void inputRelayInfo();
  void inputOptionalInfo();
  // access to other classes
  GetRaspiStatus* getRaspiStatus_ = nullptr;
  GetRelayStatus* getRelayStatus_ = nullptr;
  ReceiveCommand* receiveCommand_ = nullptr;
  RunIDManager* runIDManager_ = nullptr;
  ReceiveEUResponse* receiveEUResponse_ = nullptr;
  GetGL860Data* getGL860Data_ = nullptr;

  std::vector<std::string> getGL860DataNames_;
  std::vector<GetGL860Data*> getGL860DataVec_;

  std::string EU_serverIp_;
  int EU_port_;                                          
  std::string OU_serverIp_;                            
  int OU_port_;
  std::string communicationType_;                   

  std::chrono::steady_clock::time_point lastRegularTelemetryTime_; // 前回 Type 9 を送信した時間
  static constexpr std::chrono::milliseconds TELEMETRY_INTERVAL{500}; // 500ms (0.5秒)
  static constexpr int ID_NONE = 255;
  static constexpr int ID_HK_TELEMETRY = 1;
  static constexpr int ID_GNSS_TELEMETRY = 2; 
  static constexpr int ID_Elmo_TELEMETRY = 3; 
  static constexpr int ID_ER_RESPONSE = 5; 
  static constexpr int ID_Whole_TELEMETRY = 9;

  // communication
  std::shared_ptr<SerialCommunication> sc_;
  std::shared_ptr<SocketTransceiver> ou_;
  std::shared_ptr<SocketTransceiver> eu_;
  std::string serialPath_;
  speed_t baudrate_ = B9600;
  mode_t openMode_ = O_RDWR;

};

} /* namespace balloon */

#endif /* SendTelemetry_H */
