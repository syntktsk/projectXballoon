#ifndef TelemetryDefinition_H
#define TelemetryDefinition_H 1

#include <iostream>
#include <vector>
#include <sys/time.h>
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
  void generateTelemetryWhole();
  void generateTelemetryHK();
  void generateTelemetryElmo();//構造体→バイナリにするよ
  void generateTelemetryGNSS();
  void generateTelemetryRelays();
  void generateTelemetryOption();
//   データ書き込むやつ。RTDは温度計で、環境データはGNSSとかかな今回の場合。CRCはコマンドちゃんと送れてるか数えるやつ
  void writeGL860value();
  void writeEnvironmentalData();
  void writeEss();
  void writeEr();
  void writeCRC16();
  void clear();
  void writeFile(const std::string& filename, bool append);

  bool setTelemetry(const std::vector<uint8_t>& v);//   バイナリをテレメトリにセットするやつ？らしい
  void interpret();//   Setで受け取ったやつをそれぞれ構造体で解釈する関数
  void interpretHK();
  void interpretElmo();
  void interpretGNSS();
  void interpretRelays();
  void interpretWhole();
  void interpretOption(int header_size);
//   おまじない
  template<typename T> void addValue(T input);
  template<typename T> void addVector(std::vector<T>& input);
  template<typename T> T getValue(int index);
  template<typename T> T getValuel(int index);
  template<typename T> void getVector(int index, int num, std::vector<T>& vec);

  const std::vector<uint8_t>& Telemetry() const { return telemetry_; }  

  // rtd
  void setGL860value(int index, int16_t v) { getGL860DataVec_[index] = v; }
  void resizeGL860value(int n) { getGL860DataVec_.resize(n); }
  std::vector<int16_t>& getGL860DataVec() {return getGL860DataVec_; }
  
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
  // void setSeqNo(int32_t v){ seqNo_ = v;}
  // void setYpr(const std::vector<float>& v) { ypr_ = v; } 
  // void setAngrate(const std::vector<float>& v){ angrate_ = v;}
  // void setPosLla(const std::vector<double>& v){ posLla_ = v;}
  // void setVelocity(const std::vector<float>& v){ GNSSvelocity_ = v;}
  // void setInsStatus(uint16_t v){insStatus_ = v;}
  // void setGNSStemp(float v){GNSStemp_ = v;}
  // void setGNSSpres(float v){GNSSpres_ = v;}
  // void setCurrutc(const TUTC& v){currutc_ = v;}
  // void setIMUsensSat(uint16_t v){IMUsensSat_ = v;}
  // void setGNSSnumSats(uint8_t v){GNSSnumSats_ = v;}
  void setLatitude(float v){ la_ = v; }
  void setLongitude(float v){ lo_ = v; }
  void setHeight(float v){ he_ = v; }
  void setYaw(float v){ ya_ = v; }
  void setPitch(float v){ pi_ = v; }
  void setRoll(float v){ ro_ = v; }
  void setTemperature(float v){ te_ = v; }

// ForElmo　ここは変わる
  void setMotorOnOff(int v){ MO_ = v; }
  void setUnitMode(int v){ UM_ = v; }
  void setMoterFault(float v){ MF_ = v; }
  void setErrorCode(int v){ EC_ = v; }
  void setPosition(int v){ PX_ = v; }
  void setVelocity(float v){ VX_ = v; }
  void setI_Qaxis(float v){ IQ_ = v; }
  void setI_Daxis(float v){ ID_ = v; }
  void setMaxCurrent(float v){ MC_ = v; }
  void setBusVoltage(int v){ BV_ = v; }
  void setTemperatureInfomation(int v){ TI_ = v; }
  void setTorqueCommand(float v){ TC_ = v; }
  void setJogVelocity(float v){ JV_ = v; }
  void setPositionAbusolute(int v){ PA_ = v; }
  void setPositionRelative(int v){ PR_ = v; }
  void setModeflag(int v){ ac_ = v; }
  void setEnablefrag(int v){ ef_ = v; }
  void setParameterset(int v){ ps_ = v; }
  void setEUlastcommand(int v){ elc_ = v;}

// ninni no mojiretu
  void setOptionalStrings(std::string v){er_ = v;}

//Relay
  void setRelay_dummy(int v){dummy_data = v;}

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
  // int32_t SeqNo(){ return seqNo_; }
  // const std::vector<float>& ypr() const { return ypr_; }
  // uint16_t InsStatus(){ return insStatus_; }
  // const std::vector<float>& Angrate() const { return angrate_; }
  // const std::vector<double>& PosLla() const { return posLla_; }
  // const std::vector<float>& GNSSVelocity() const { return GNSSvelocity_; }
  // float GNSStemp() { return GNSStemp_; }
  // float GNSSpres() { return GNSSpres_; }
  // const TUTC& currutc() const { return currutc_; }
  // uint16_t IMUsensSat() { return IMUsensSat_;}
  // uint8_t GNSSnumSats() { return GNSSnumSats_; }
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
  int Relay_dummy(){return dummy_data;}

private:
  std::vector<uint8_t> telemetry_;
  // header
  uint32_t startCode_ = 0xEB905B6A;
  uint16_t telemetryType_ = 0;
  timeval timeNow_;
  uint32_t telemetryIndex_ = 0;
  int32_t runID_ = -1;
  // footer
  uint16_t crc_ = 0;
  uint32_t stopCode_ = 0xC5A4D279;

  int lastCommandIndex_; 
  int lastCommandCode_; 
  int commandRejectCount_; 
  uint64_t softwareErrorCode_;

  uint64_t SDCapacity_ = 0;

  // HK
  std::vector<int16_t> getGL860DataVec_;
  // --- 温度系 ---
  uint16_t PivotTemp_;
  uint16_t StarCameraTemp_;
  uint16_t MirrorTemp_;
  uint16_t GnssTemp_hk_;
  uint16_t CulculatorTemp_;
  uint16_t BatteryTemp_;
  uint16_t GyroTemp_;
  uint16_t CmosTemp_;
  // --- 電圧系 ---
  uint16_t PcVolt_;
  uint16_t StarCameravolt_;
  uint16_t GnssVolt_;
  uint16_t GyroVolt_;
  uint16_t CmosVolt_;
  uint16_t RouterVolt_;
  uint16_t HeaterVolt_;
  uint16_t Pi_hkVolt_;
  uint16_t PivotVolt_;
  uint16_t HubVolt_;

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
  int ps_ = 0;  
  int elc_= 0;

  // // GNSS
  // int32_t seqNo_ = 0;
  // std::vector<float> ypr_;
  // std::vector<float> angrate_;
  // std::vector<double>posLla_;
  // std::vector<float> GNSSvelocity_;
  // uint16_t insStatus_ = 0;
  // float GNSStemp_ = 0;
  // float GNSSpres_ = 0;
  // TUTC currutc_;
  // uint16_t IMUsensSat_ = 0;
  // uint8_t GNSSnumSats_ = 0;
// GNSS_v2
  float la_ = 0;
  float lo_ = 0;  // longitude 経度 [deg]
  float he_ = 0;  // height    海抜高度 [m]
  float ya_ = 0;  // yaw       機体のヨー角 [deg]
  float pi_ = 0;  // pitch     機体のピッチ角 [deg]
  float ro_ = 0;  // roll      機体のロール角 [deg]
  float te_ = 0;  // temperature 気温 / センサ温度 [°C]

// optional
  int header_size;
  std::string er_;
//Relay
  int dummy_data = 0;

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

  // 1. 型変換ではなく、メモリの中身をそのまま v にコピーする
  // これにより float のビットパターンがそのまま保持される
  std::memcpy(&v, &input, size);

  // 2. ビッグエンディアンとして push_back
  for (int i=0; i<size; i++) {
    // memcpyでvに入れた場合、PC（リトルエンディアン）では逆順になっているので
    // 送信順序を調整する（元のロジックに合わせるならこうなります）
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

// template<typename T>
// T TelemetryDefinition::getValue(int index)
// {
//   const int n = telemetry_.size();
//   const int byte = sizeof(T);
//   if (index+byte>n) {
//     std::cerr << "TelemetryDefinition::getValue error: out of range" << std::endl;
//     std::cerr << "telemetry_.size() = " << n << ", index = " << index << ", byte = " << byte << std::endl;
//     return static_cast<T>(0);
//   }
//   if (byte > 8) {
//     std::cerr << "TelemetryDefinition::getValue error: typename error" << std::endl;
//     std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
//     return static_cast<T>(0);
//   }
//   uint64_t v = 0;
//   for (int i = 0; i < byte; i++) {
//     const int j = index + i;
//     const int shift = 8 * (byte - 1 - i); 
//     v |= (static_cast<uint64_t>(telemetry_[j]) << shift);
//   }
//   T value;
//   memcpy(&value, &v, sizeof(T));

//   if (std::isnan(static_cast<double>(value))) {
//     value = static_cast<T>(0.0);
//   }
  
//   return value;
// }
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

  // 3. この時点で value の中身は、正しい符号(int)や指数(float)を持った状態になる
  
  // もし NaN チェックを残すなら float/double の時だけに限定する
  // if (std::is_floating_point<T>::value && std::isnan(static_cast<double>(value))) {
  //   return static_cast<T>(0);
  // }

  return value;
}

template<typename T>
T TelemetryDefinition::getValuel(int index)
{
  const int n = telemetry_.size();
  const int byte = sizeof(T);
  
  if (index + byte > n) {
    return static_cast<T>(0);
  }

  // バイト列を数値(v)に組み立てる
  uint64_t v = 0;
  for (int i = 0; i < byte; i++) {
    const int j = index + i;
    // --- ここを修正 ---
    // 最初のバイトを一番下の桁(0bitシフト)にする
    const int shift = 8 * i; 
    v |= (static_cast<uint64_t>(telemetry_[j]) << shift);
  }

  T value;
  memcpy(&value, &v, sizeof(T));

  // 前回のコンパイルエラー対策: 数値型の場合のみisnanチェック
  if constexpr (std::is_arithmetic_v<T>) {
    if (std::isnan(static_cast<double>(value))) {
      value = static_cast<T>(0.0);
    }
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