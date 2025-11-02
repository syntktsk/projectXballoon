#ifndef TelemetryReceiver_UDP_HH
#define TelemetryReceiver_UDP_HH 1

#include <vector>
#include <anlnext/BasicModule.hh>
#include "SocketTransceiver_UDP.hh"
#include "CommandBuilder.hh"
#include "DocumentBuilder.hh"
#include "MongoDBClient.hh"
#include "TelemetryDefinition.hh"
#include "GNSSForISAS.hh"

namespace balloon{

class TelemetryReceiver_UDP : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(TelemetryReceiver_UDP, 1.0);
public:
  TelemetryReceiver_UDP();
  ~TelemetryReceiver_UDP() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  std::string serverIp_;
  int port_;
  TelemetryDefinition telemetry_;
  SocketTransceiver_UDP transceiver_;
  CommandBuilder command_builder_;
  GNSSforISAS forISAS_;
  MongoDBClient* mongodb_client_;
  std::ofstream outfile;

  std::vector<uint8_t> binary_command_;
  std::vector<uint8_t> ascii_command_;

  // 実行間隔を保持する変数（秒単位）
  double set_run_interval_time_ = 0.5; 
  // 最後に実行された時間を記録する変数
  std::chrono::steady_clock::time_point last_run_time_;
};

}/* namespace balloon */

#endif /* TelemetryReceiver_UDP_H */