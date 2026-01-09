#ifndef TelemetryReceiver_HH
#define TelemetryReceiver_HH 1

#include <vector>
#include <anlnext/BasicModule.hh>
#include "SocketTransceiver.hh"
// #include "CommandBuilder.hh"
// #include "DocumentBuilder.hh"
#include "MongoDBClient.hh"
#include "TelemetryDefinition.hh"
#include "SerialCommunication.hh"
#include "ErrorManager.hh"

namespace balloon{

class TelemetryReceiver : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(TelemetryReceiver, 1.0);
public:
  TelemetryReceiver();
  ~TelemetryReceiver() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  // --- 1. 相棒 (SendTelemetry) へのポインタとエラーマネージャー用の窓口 ---
  balloon::SendTelemetry* sendTelemetry_ = nullptr; 
  void* mongodb_client_ = nullptr;
  ErrorManager* getErrorManager(); // 宣言を忘れずに！
  
  std::ofstream outfile;

  std::vector<uint8_t> binary_command_;
  std::vector<uint8_t> ascii_command_;

  // 実行間隔を保持する変数（秒単位）
  double set_run_interval_time_ = 0.5; 
  // 最後に実行された時間を記録する変数
  std::chrono::steady_clock::time_point last_run_time_;

  std::shared_ptr<SocketTransceiver> ou_;
  std::shared_ptr<SerialCommunication> sc_;
  // std::shared_ptr<CommandDefinition> comdef_; 
  std::shared_ptr<TelemetryDefinition> td_;   // 142行目用
  
  std::string serialPath_;
  std::string binaryFilenameBase_;
  std::string serverIp_;
  std::string communicationType_;
  std::string OU_serverIp_;
  
  int port_;
  int OU_port_;
  int bufferSize_ = 2048;
  int timeoutSec_;
  int chatter_;
  int length_;
  mode_t openMode_;
  speed_t baudrate_;
  
  std::vector<uint8_t> buffer_;
};

}/* namespace balloon */

#endif /* TelemetryReceiver_H */