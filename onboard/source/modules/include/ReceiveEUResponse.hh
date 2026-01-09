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
	void inputGNSS(const ess2& gnss_status);
	void inputElmo(const ess1& elmo_status);
	void inputOption(const ess3& option);
  SendTelemetry* sender;
  ErrorManager* getErrorManager();
  bool hasNewData() const { return hasNewData_; }
  int getNewDataID() const { return newDataID_; }
  void clearNewDataFlag() { hasNewData_ = false; newDataID_ = 255; }
  
// getter of Elmo
  int MotorOnOff(){ return MO_; }
  int UnitMode(){ return UM_; }
  float MoterFault(){ return MF_;}
  int ErrorCode(){ return EC_; }
  int Position(){ return PX_; }
  float Velocity(){ return VX_; }
  float I_Qaxis(){ return IQ_; }
  float I_Daxis(){ return ID_; }
  float MaxCurrent(){ return MC_; }
  int BusVoltage(){ return BV_; }
  int TemperatureInfomation(){ return TI_; }
  float TorqueCommand(){ return TC_; }
  float JogVelocity(){ return JV_; }
  int PositionAbusolute(){ return PA_; }
  int PositionRelative(){ return PR_; }
  int modeflag(){ return ac_; }
  int enablefrag(){ return ef_; }
  int parameterset(){ return ps_; }
  int EUlastcommand(){return elc_;}
  // getter of GNSS
  float latitude(){ return la_; }
  float longitude(){ return lo_; }
  float height(){ return he_; }
  float yaw(){ return ya_; }
  float pitch(){ return pi_; }
  float roll(){ return ro_; }
  float temperature(){ return te_; }
// ninni no mojiretu
  std::string getOptionalStrings() const {return er_;}

private:
// Elmo
  int MO_ = 0;   // Motor On/OFF 状態 (1=モーター有効, 0=無効)
  int UM_ = 0;   // Unit Mode = 0,1,2,5
  float MF_ = 0;   // Motor Fault 異常停止要因（モーターが無効化された理由）
  int EC_ = 0;   // Error Code エラーコード
  int PX_ = 0;   // Position 現在位置 [cnt]
  float VX_ = 0;   // Velocity 現在速度 [cnt/sec]
  float IQ_ = 0;   // q-axis current (トルクを発生させる電流)
  float ID_ = 0;   // d-axis current (トルクに関わらない電流、通常0付近)
  float MC_ = 0;   // Max Current 設定されている最大電流 (q軸電流制限)
  int BV_ = 0;   // Bus Voltage ドライバ内部バス電圧 [V]
  int TI_ = 0;   // Temperature Information ドライバ温度 [°C]
  float TC_ = 0;   // Torque Command 指令トルク値
  float JV_ = 0;   // Jog Velocity 指令速度（Jog運転時）
  int PA_ = 0;   // Position Abusolute 絶対位置指令
  int PR_ = 0;   // Position Relative 相対位置指令値
  int ac_ = 0;   // mode flag 運転モード番号 (0~21)
  int ef_ = 0;   // enable flag 回転可能かどうか (0=回転不可, 1=回転可能)
  int ps_ = 0;  //パラメーターセットのセット番号？多分
  int elc_= 0;
// GNSS
  float la_ = 0;
  float lo_ = 0;  // longitude 経度 [deg]
  float he_ = 0;  // height    海抜高度 [m]
  float ya_ = 0;  // yaw       機体のヨー角 [deg]
  float pi_ = 0;  // pitch     機体のピッチ角 [deg]
  float ro_ = 0;  // roll      機体のロール角 [deg]
  float te_ = 0;  // temperature 気温 / センサ温度 [°C]
// optional
  std::string er_;
//socket通信用
  std::shared_ptr<SocketTransceiver> eu_;
  std::string serverIp_ = "127.0.0.1";
  uint16_t port_ = 9090;
  std::vector<uint8_t> raw_data_;
  SendTelemetry* sendTelemetry_ = nullptr;
  bool hasNewData_ = false;
  int newDataID_ = 255;

};
}/*namespase balloon*/
#endif