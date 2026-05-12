#ifndef ReceiveEUResponse_H
#define ReceiveEUResponse_H 1

#include <string>
#include <vector>
#include <queue>
#include <sys/select.h>
#include <sys/time.h>
#include "ErrorManager.hh"
#include "SendTelemetry.hh"
#include "eu_struct.hh"
#include <anlnext/BasicModule.hh>


namespace balloon{
class SocketTransceiver;

class ReceiveEUResponse : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(ReceiveEUResponse, 1.0); // これが必要

public:
  ReceiveEUResponse(); // 引数なしコンストラクタが必要
  virtual ~ReceiveEUResponse();

  // ANLNext標準のメソッド
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;
  void setSocket(std::shared_ptr<SocketTransceiver> socket) { eu_ = socket; }
  // void setAddress(std::string& ip, uint16_t port);
  int analysis();
  int interpretStruct(const std::vector<uint8_t>& bynary_struct);
	// void inputGNSS(const ess2& gnss_status);
	// void inputElmo(const ess1& elmo_status);
	// void inputOption(const ess3& option);
  SendTelemetry* sender;
  ErrorManager* getErrorManager();
  bool hasNewData() const { return hasNewData_; }
  int getNewDataID() const { return newDataID_; }
  void clearNewDataFlag() { hasNewData_ = false; newDataID_ = 255; }
  ess9 getWholeStatus() const { return last_all_status_; }
  ess1 getElmoStatus() const { return last_elmo_status_; }
  ess2 getGnssStatus() const { return last_gnss_status_; }
  ess3 getSensorsStatus() const { return last_sensors_status_; }
  bool reserveData();

// ninni no mojiretu
  std::string getOptionalStrings() const {return er_;}

private:

// optional
  std::string er_;
//socket通信用
  std::shared_ptr<SocketTransceiver> eu_;
  std::string serverIp_ = "127.0.0.1";
  uint16_t port_ = 9090;
  std::vector<uint8_t> raw_data_;
  std::string filepath_;
  SendTelemetry* sendTelemetry_ = nullptr;
  bool hasNewData_ = false;
  int newDataID_ = 255;
  ess9 last_all_status_;
  ess1 last_elmo_status_;
  ess2 last_gnss_status_;
  ess3 last_sensors_status_;
  ess8 last_option_;

  int chatter_;

};
}/*namespase balloon*/
#endif