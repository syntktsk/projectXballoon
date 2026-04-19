/**
 * Receive commands from ground.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */

#ifndef ReceiveCommand_H
#define ReceiveCommand_H 1

#include <anlnext/BasicModule.hh>
#include <queue>
#include <sys/select.h>
#include <sys/time.h>
#include "CommandDefinition.hh"
#include "SerialCommunication.hh"
#include "ShutdownSystem.hh"
#include "SocketTransceiver.hh"
#include "GetGL860Data.hh"
#include "RelayControl.hh"
#include "SendTelemetry.hh"
#include "RunIDManager.hh"
using namespace anlnext;

namespace balloon {

class ShutdownSystem;
class SendTelemetry;
class RunIDManager;
class SocketTransceiver;
class GetGL860Data;
class RelayControl;

class ReceiveCommand : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ReceiveCommand, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ReceiveCommand();
  virtual ~ReceiveCommand();
protected:
  ReceiveCommand(const ReceiveCommand& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  bool applyCommand();
  void writeCommandToFile(bool failed);

  std::shared_ptr<SocketTransceiver> getSocket() { return eu_; }


  uint16_t CommandCode() { return (singleton_self()->comdef_)->Code(); }
  uint32_t CommandIndex() { return singleton_self()->commandIndex_; }
  uint16_t CommandRejectCount() { return singleton_self()->commandRejectCount_; }

  

private:
  std::vector<uint8_t> buffer_;
  std::vector<uint8_t> command_;
  std::string StringCommand_;
  std::shared_ptr<CommandDefinition> comdef_ = nullptr;
  uint32_t commandIndex_ = 0;
  uint16_t commandRejectCount_ = 0;
  std::map<int, std::pair<int, int>> fileIDmp_;
  bool saveCommand_ = true;
  std::string binaryFilenameBase_ = "";
  int numCommandPerFile_ = 100;
  int chatter_ = 0;

  // access to other classes
  SendTelemetry* sendTelemetry_ = nullptr;
  ShutdownSystem* shutdownSystem_ = nullptr;
  RunIDManager* runIDManager_ = nullptr;
  GetGL860Data* getGL860Data_ = nullptr;
  RelayControl* relayControl_ = nullptr;
  ErrorManager* getErrorManager();

  //communication
  std::shared_ptr<SerialCommunication> sc_ = nullptr;
  std::shared_ptr<SocketTransceiver> st_ = nullptr;
  speed_t baudrate_;
  std::string serialPath_;
  mode_t openMode_ = O_RDWR;
  bool startReading_ = false;
  bool rs0_ = false;
  bool rs1_ = false;
  bool rs2_ = false;
  int timeoutSec_ = 2;
  constexpr static int bufferSize_ = 200;
  constexpr static int serialReadingTimems_ = 250;
  std::string EU_serverIp_;
  int EU_port_;
  std::string OU_serverIp_;
  int OU_port_;
  std::string communicationType_;
  std::string serverIp_; 
  int port_;
  std::string savePath_;
  std::shared_ptr<SocketTransceiver> eu_;
  std::shared_ptr<SocketTransceiver> ou_;
};

} /* namespace balloon */

#endif /* ReceiveCommand_H */
