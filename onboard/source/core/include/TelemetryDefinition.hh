#ifndef TelemetryDefinition_H
#define TelemetryDefinition_H 1
#include "eu_struct.hh"
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <GL860main.hh>
#include "CRC16.hh"
#include "BinaryFileManipulater.hh"
#include <cstring>
#include <cmath>
/**
 * A class to define telemetry
 *
 * @author 
 * @date 
 */

namespace balloon {

enum class TelemetryType {
  HK = 1,
  GNSS = 2,
  Elmo = 3,
  Relays = 4,
  Optional = 5,
  GL860 = 6,
  Whole = 9,
};

typedef struct {
  int8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t fracsec;
} TUTC;

class TelemetryDefinition
{
public:
  TelemetryDefinition(); 
//  Telemetry作るやつ、全体・HK・Elmoモータ・GNSS位置情報・リレーでやってみる
  void generateTelemetry();
  // void generateTelemetryWhole();
  void generateTelemetryHK();
  void generateTelemetryElmo();
  void generateTelemetryGNSS();
  void generateTelemetryRelays();
  void generateTelemetryOption();
  void generateTelemetryGL860();
//   データ書き込むやつ。RTDは温度計で、環境データはGNSSとかかな今回の場合。CRCはコマンドちゃんと送れてるか数えるやつ
  void writeGL860value(std::vector<int16_t>& getGL860DataVec_);
  void writeGL860status(std::vector<uint8_t>& range_info_);
  void writeEnvironmentalData();
  void writeEUDataToFile(int id,bool app);
  void writeEss();
  void writeEr();
  void writeCRC16();
  void clear();
  void writeFile(const std::string& filename, bool append);

  bool setTelemetry(const std::vector<uint8_t>& v);//   バイナリをテレメトリにセットするやつ？らしい
  void interpret();//Setで受け取ったやつをそれぞれ構造体で解釈する関数
  void interpretHK();
  void interpretGL860option();
  void interpretGL860stat();
  void interpretElmo();
  void interpretGNSS();
  void interpretRelays();
  void interpretWhole();
  void interpretOption(int header_size);
// おまじない
  template<typename T> void addValue(T input);
  template<typename T> void addVector(std::vector<T>& input);
  template<typename T> T getValue(int index);
  template<typename T> void getVector(int index, int num, std::vector<T>& vec);
  const std::vector<uint8_t>& Telemetry() const { return telemetry_; }  
  // --- テレメトリサイズ取得用 Getter ---
  int hkSize()     const { return hkSize_; }
  int elmoSize()   const { return elmoSize_; }
  int GNSSSize()   const { return GNSSSize_; }
  int optionalSize() const { return optSize_; }
  int relaysSize() const { return relaysSize_; }
  int gl860Size() const { return gl860Size_; }
  // rtd
  void setGL860value(std::vector<int16_t> v){ getGL860DataVec_= v; }
  void setGL860option(std::string v){ gl860string_= v; }
  void setlastCommandGL860(std::string v){ lastCommandGL860_ = v;}
  void resizeGL860value(int n) { getGL860DataVec_.resize(n); }
  void setSDCapacity(uint64_t v) { SDCapacity_ = v; }
  void setRunID(int32_t v) { runID_ = v; }

  // --- ソフトウェア情報 ---
  void setLastCommandIndex(int v) { lastCommandIndex_ = v; }
  void setLastCommandCode(int v) { lastCommandCode_ = v; }
  void setCommandRejectCount(int v) { commandRejectCount_ = v; }
  void setSoftwareErrorCode(uint64_t v) { softwareErrorCode_ = v; }
  int LastCommandIndex() const { return lastCommandIndex_; }
  int LastCommandCode() const { return lastCommandCode_; }
  int CommandRejectCount() const { return commandRejectCount_; }
  uint64_t SoftwareErrorCode() const { return softwareErrorCode_; }
  std::vector<uint8_t> RelaysStatus() { return relaysStatus_; }


//setterForHK
  // --- 温度系関数 ---
  void setPivotTemp(uint16_t v)      { PivotTemp_ = v; }
  void setStarCameraTemp(uint16_t v) { StarCameraTemp_ = v; }
  void setMirrorTemp(uint16_t v)     { MirrorTemp_ = v; }
  void setGnssTemp_hk(uint16_t v)    { GnssTemp_hk_ = v; }
  void setCulculatorTemp(uint16_t v) { CulculatorTemp_ = v; } 
  void setBatteryTemp(uint16_t v)    { BatteryTemp_ = v; }
  void setGyroTemp(uint16_t v)       { GyroTemp_ = v; }
  void setCmosTemp(uint16_t v)       { CmosTemp_ = v; }
  // --- 電圧系関数 ---
  void setPcVolt(uint16_t v)         { PcVolt_ = v; }
  void setStarCameravolt(uint16_t v) { StarCameravolt_ = v; }
  void setGnssVolt(uint16_t v)       { GnssVolt_ = v; }
  void setGyroVolt(uint16_t v)       { GyroVolt_ = v; }
  void setCmosVolt(uint16_t v)       { CmosVolt_ = v; }
  void setRouterVolt(uint16_t v)     { RouterVolt_ = v; }
  void setHeaterVolt(uint16_t v)     { HeaterVolt_ = v; }
  void setPi_hkVolt(uint16_t v)      { Pi_hkVolt_ = v; }
  void setPivotVolt(uint16_t v)      { PivotVolt_ = v; }
  void setHubVolt(uint16_t v)        { HubVolt_ = v; }
  
  void setTelemetryType(uint16_t v) { telemetryType_ = v;}
// setterForGNSS
  void setGNSSData(const ess2& v) { gnss_status = v; }
  void setLatitude(float v){ la_ = v; }
  void setLongitude(float v){ lo_ = v; }
  void setHeight(float v){ he_ = v; }
  void setYaw(float v){ ya_ = v; }
  void setPitch(float v){ pi_ = v; }
  void setRoll(float v){ ro_ = v; }
  void setTemperature(float v){ te_ = v; }

// ForElmo　ここは変わる
  void setElmoData(const ess1& v) {elmo_status = v; }
  void setMotorOnOff(uint8_t v){ MO_ = v; }
  void setOnOffBrake(uint8_t v){ OB_ = v; }
  void setUnitMode(uint8_t v){ UM_ = v; }
  void setMoterFault(float v){ MF_ = v; }
  void setErrorCode(int32_t v){ EC_ = v; }
  void setPosition(int32_t v){ PX_ = v; }
  void setVelocity(float v){ VX_ = v; }
  void setI_Qaxis(float v){ IQ_ = v; }
  void setI_Daxis(float v){ ID_ = v; }
  void setMaxCurrent(float v){ MC_ = v; }
  void setBusVoltage(int32_t v){ BV_ = v; }
  void setTemperatureInfomation(int8_t v){ TI_ = v; }
  void setTorqueCommand(float v){ TC_ = v; }
  void setJogVelocity(float v){ JV_ = v; }
  void setPositionAbusolute(int32_t v){ PA_ = v; }
  void setPositionRelative(int32_t v){ PR_ = v; }
  void setModeflag(uint8_t v){ ac_ = v; }
  void setEnablefrag(u_int8_t v){ ef_ = v; }
  void setParameterset(uint8_t v){ ps_ = v; }
  void setEn(int32_t v){ en_ = v; }
  void setAz(float v){ az_ = v; }
  void setHi(int32_t v){ hi_ = v; }
  void setEUlastcommand(std::string v){ lc_ = v;}

// ninni no mojiretu
  void setOptionalStrings(std::string v){er_ = v;}

//Relay
  void setRelay(uint32_t v){relay_ = v;}

// getter
  uint32_t StartCode() { return startCode_; }
  uint16_t TelemetryType() { return telemetryType_; }
  timeval TimeNow() { return timeNow_; }
  uint32_t TelemetryIndex() { return telemetryIndex_; }
  int32_t RunID() { return runID_; }
  uint16_t CRC() { return crc_; }
  uint32_t StopCode() { return stopCode_; }
  int64_t SDCapacity() { return SDCapacity_; }

  // getter of HK で、温度だけやってもいいかも
  std::vector<int16_t>& getGL860DataVec() {return getGL860DataVec_; }
  std::string GL860option(){ return gl860string_; }
  std::vector<float>& gl860ground() {return gl860_ground_; }
  std::string lastCommandGL860(){return lastCommandGL860_;}
  std::vector<std::string> gl860logic(){ return gl860_logic_;}
  // --- 温度系 ---
  uint16_t PivotTemp(){ return PivotTemp_; }
  uint16_t StarCameraTemp() { return StarCameraTemp_; }
  uint16_t MirrorTemp()     { return MirrorTemp_; }
  uint16_t GnssTemp_hk()    { return GnssTemp_hk_; }
  uint16_t CulculatorTemp() { return CulculatorTemp_; } 
  uint16_t BatteryTemp()    { return BatteryTemp_; }
  uint16_t GyroTemp()       { return GyroTemp_; }
  uint16_t CmosTemp()       { return CmosTemp_; }
  // --- 電圧系 ---
  uint16_t PcVolt()         { return PcVolt_; }
  uint16_t StarCameravolt() { return StarCameravolt_; }
  uint16_t GnssVolt()       { return GnssVolt_; }
  uint16_t GyroVolt()       { return GyroVolt_; }
  uint16_t CmosVolt()       { return CmosVolt_; }
  uint16_t RouterVolt()     { return RouterVolt_; }
  uint16_t HeaterVolt()     { return HeaterVolt_; }
  uint16_t Pi_hkVolt()      { return Pi_hkVolt_; }
  uint16_t PivotVolt()      { return PivotVolt_; }
  uint16_t HubVolt()        { return HubVolt_; }

// getter of Elmo
  uint8_t MotorOnOff(){ return MO_; }
  uint8_t UnitMode(){ return UM_; }
  uint8_t OnOffBrake(){ return OB_; }
  float MoterFault(){ return MF_;}
  int ErrorCode(){ return EC_; }
  int Position(){ return PX_; }
  float Velocity(){ return VX_; }
  float I_Qaxis(){ return IQ_; }
  float I_Daxis(){ return ID_; }
  float MaxCurrent(){ return MC_; }
  int BusVoltage(){ return BV_; }
  int8_t TemperatureInfomation(){ return TI_; }
  float TorqueCommand(){ return TC_; }
  float JogVelocity(){ return JV_; }
  int PositionAbusolute(){ return PA_; }
  int PositionRelative(){ return PR_; }
  uint8_t modeflag(){ return ac_; }
  uint8_t enablefrag(){ return ef_; }
  uint8_t parameterset(){ return ps_; }
  int32_t en(){ return en_; }
  float az(){ return az_; }
  int32_t hi(){ return hi_; }
  std::string EUlastcommand(){return lc_;}

// getter of GNSS
  float latitude(){ return la_; }
  float longitude(){ return lo_; }
  float height(){ return he_; }
  float yaw(){ return ya_; }
  float pitch(){ return pi_; }
  float roll(){ return ro_; }
  float temperature(){ return te_; }

// ninni no mojiretu
  std::string OptionalStrings(){return er_;}

//Relay
  uint32_t Relay(){return relay_;}

private:
  std::vector<uint8_t> telemetry_;
  const int hkSize_ = 40+4;
  const int elmoSize_ = 83;
  const int GNSSSize_ = 28;
  const int optSize_ = 16;
  const int relaysSize_ = 4;
  const int gl860Size_ = 32*2;
  const float mul = 10000;
  // header
  uint32_t startCode_ = 0xEB905B6A;
  uint16_t telemetryType_ = 0;
  timeval timeNow_;
  uint32_t telemetryIndex_ = 0;
  int32_t runID_ = -1;
  // footer
  uint16_t crc_ = 0;
  uint32_t stopCode_ = 0xC5A4D279;

  uint32_t lastCommandIndex_; 
  uint16_t lastCommandCode_; 
  uint16_t commandRejectCount_; 
  uint64_t softwareErrorCode_;
  std::vector<uint8_t> relaysStatus_ = std::vector<uint8_t>(32, 0);
  
  uint64_t SDCapacity_ = 0;

  // HK
  std::vector<int16_t> getGL860DataVec_ = std::vector<int16_t>(22, 999);
  std::string gl860string_ = "";
  std::vector<float> gl860_ground_;
  std::vector<std::string> gl860_logic_ = {"0", "0", "0", "0"};
  std::string lastCommandGL860_ = "";
  std::vector<int> range_info_ = {
    4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000,
  };
  // --- 温度系 ---
  uint16_t PivotTemp_ = 0;
  uint16_t StarCameraTemp_= 0;
  uint16_t MirrorTemp_= 0;
  uint16_t GnssTemp_hk_= 0;
  uint16_t CulculatorTemp_= 0;
  uint16_t BatteryTemp_= 0;
  uint16_t GyroTemp_= 0;
  uint16_t CmosTemp_= 0;
  // --- 電圧系 ---
  uint16_t PcVolt_= 0;
  uint16_t StarCameravolt_= 0;
  uint16_t GnssVolt_= 0;
  uint16_t GyroVolt_= 0;
  uint16_t CmosVolt_= 0;
  uint16_t RouterVolt_= 0;
  uint16_t HeaterVolt_= 0;
  uint16_t Pi_hkVolt_= 0;
  uint16_t PivotVolt_= 0;
  uint16_t HubVolt_= 0;

  // Elmo
  ess1 elmo_status;
  uint8_t MO_ = 0;   // Motor On/OFF 状態 (1=モーター有効, 0=無効)
  uint8_t UM_ = 0;   // Unit Mode = 0,1,2,5
  uint8_t OB_ = 0;
  float MF_ = 0;   // Motor Fault 異常停止要因（モーターが無効化された理由）
  int EC_ = 0;   // Error Code エラーコード
  int PX_ = 0;   // Position 現在位置 [cnt]
  float VX_ = 0;   // Velocity 現在速度 [cnt/sec]
  float IQ_ = 0;   // q-axis current (トルクを発生させる電流)
  float ID_ = 0;   // d-axis current (トルクに関わらない電流、通常0付近)
  float MC_ = 0;   // Max Current 設定されている最大電流 (q軸電流制限)
  int BV_ = 0;   // Bus Voltage ドライバ内部バス電圧 [V]
  int8_t TI_ = 0;   // Temperature Information ドライバ温度 [°C]
  float TC_ = 0;   // Torque Command 指令トルク値
  float JV_ = 0;   // Jog Velocity 指令速度（Jog運転時）
  int PA_ = 0;   // Position Abusolute 絶対位置指令
  int PR_ = 0;   // Position Relative 相対位置指令値
  uint8_t ac_ = 0;   // mode flag 運転モード番号 (0~21)
  uint8_t ef_ = 0;   // enable flag 回転可能かどうか (0=回転不可, 1=回転可能)
  uint8_t ps_ = 0;  
  uint32_t  en_ = 0;
  float az_ = 0;
  uint32_t hi_ = 0;
  std::string lc_= "";

  ess2 gnss_status;
  float la_ = 0;
  float lo_ = 0;  // longitude 経度 [deg]
  float he_ = 0;  // height    海抜高度 [m]
  float ya_ = 0;  // yaw       機体のヨー角 [deg]
  float pi_ = 0;  // pitch     機体のピッチ角 [deg]
  float ro_ = 0;  // roll      機体のロール角 [deg]
  float te_ = 0;  // temperature 気温 / センサ温度 [°C]

// optional
  int header_size = 0;
  std::string er_ = "THIS_IS_TEST";
//Relay
  uint32_t relay_ = 0;

  int numZeroFill_ = 0;
  
// event rate
  double eventRate_ = 0.0;
  timeval prevTime_;
  uint32_t prevEventCount_ = 0;
};
template<typename T>
void TelemetryDefinition::addValue(T input)
{
  const int size = sizeof(T);
  uint64_t v = 0;

  std::memcpy(&v, &input, size);
  for (int i=0; i<size; i++) {
    const int shift = 8 * (size-1-i);
    telemetry_.push_back(static_cast<uint8_t>((v >> shift) & 0xff));
  }
}

template<typename T>
void TelemetryDefinition::addVector(std::vector<T>& input)
{
  const int n = input.size();
  for (int i=0; i<n; i++) {
    addValue(input[i]);
  }
}

template<typename T>
T TelemetryDefinition::getValue(int index)
{
  const int n = telemetry_.size();
  const int byte = sizeof(T);
  
  if (index + byte > n) return static_cast<T>(0);

  // 1. ターゲットの型 T と同じサイズのバッファを用意
  T value;
  uint8_t* p = reinterpret_cast<uint8_t*>(&value);

  // 2. ビッグエンディアン（通信）をリトルエンディアン（PC）に反転してコピー
  for (int i = 0; i < byte; i++) {
    p[byte - 1 - i] = telemetry_[index + i];
  }

  return value;
}

template<typename T>
void TelemetryDefinition::getVector(int index, int num, std::vector<T>& vec)
{
  const int n = telemetry_.size();
  const int byte = sizeof(T);
  if (index+byte*num > n) {
    std::cerr << "TelemetryDefinition::getVector error: out of range" << std::endl;
    std::cerr << "telemetry_.size() = " << n << ", index = " << index << ", byte = " << byte
    << ", num = " << num << std::endl;
    return;
  }
  if (byte > 8) {
    std::cerr << "TelemetryDefinition::getVector error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
    return;
  }
  vec.clear();
  for (int i=0; i<num; i++) {
    T v = getValue<T>(index);
    vec.push_back(v);
    index += byte;
  }
}


} /* namespace balloon */

#endif /* TelemetryDefinition_H */
