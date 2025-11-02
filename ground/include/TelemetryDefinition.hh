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
  Relays = 4
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
  void generateTelemetryHK();
  void generateTelemetryElmo();
  void generateTelemetryGNSS();
  void generateTelemetryRelays();
//   データ書き込むやつ。RTDは温度計で、環境データはGNSSとかかな今回の場合。CRCはコマンドちゃんと送れてるか数えるやつ
  void writeRTDTemperature();
  void writeEnvironmentalData();
  void writeCRC16();
  void clear();
  void writeFile(const std::string& filename, bool append);

  bool setTelemetry(const std::vector<uint8_t>& v);//   バイナリをテレメトリにセットするやつ？らしい
  void interpret();//   Setで受け取ったやつをそれぞれ構造体で解釈する関数
  void interpretHK();
  void interpretElmo();
  void interpretGNSS();
  void interpretRelays();
//   おまじない
  template<typename T> void addValue(T input);
  template<typename T> void addVector(std::vector<T>& input);
  template<typename T> T getValue(int index);
  template<typename T> void getVector(int index, int num, std::vector<T>& vec);

  const std::vector<uint8_t>& Telemetry() const { return telemetry_; }  

  // rtd
  void setRTDTemperatureADC(int index, int16_t v) { RTDTemperatureADC_[index] = v; }
  void resizeRTDTemperatureADC(int n) { RTDTemperatureADC_.resize(n); }
  std::vector<int16_t>& RTDTemperatureADC() { return RTDTemperatureADC_; }
  void setPivotTemp(uint16_t v) {PivotTemp_ = v;}



// setterForGNSS
  void setTelemetryType(uint16_t v) { telemetryType_ = v;}
  void setSeqNo(int32_t v){ seqNo_ = v;}
  void setYpr(const std::vector<float>& v) { ypr_ = v; } 
  void setAngrate(const std::vector<float>& v){ angrate_ = v;}
  void setPosLla(const std::vector<double>& v){ posLla_ = v;}
  void setVelocity(const std::vector<float>& v){ GNSSvelocity_ = v;}
  void setInsStatus(uint16_t v){insStatus_ = v;}
  void setGNSStemp(float v){GNSStemp_ = v;}
  void setGNSSpres(float v){GNSSpres_ = v;}
  void setCurrutc(const TUTC& v){currutc_ = v;}
  void setIMUsensSat(uint16_t v){IMUsensSat_ = v;}
  void setGNSSnumSats(uint8_t v){GNSSnumSats_ = v;}
// ForElmo
  void setBrakeStatus(uint8_t v){ brakeStatus_ = v;}
  void setMoterStatus(uint8_t v){ moterStatus_ = v;}
  void setUnitMode(uint8_t v){ unitMode_ = v;}
  void setPosCount(int16_t v){posCount_ = v;}
  void setElmoVelocity(float v){ElmoVelocity_ = v;}
  void setCurrentIQ(float v){currentIQ_ = v;}
  void setCurrentID(float v){currentID_ = v;}
  void setTgtAzDeg(float v){tgtAzDeg_ = v;}
  void setTgtRaDeg(float v){tgtRaDeg_ = v;}
  void setTgtDecDeg(float v){tgtDecDeg_ = v;}
  void setTgtCount(int32_t v){tgtCount_ = v;}
  void setCtrlMode(uint8_t v){ctrlMode_ = v;}
  void setPidKp(float v){pidKp_ = v;}
  void setPidKi(float v){pidKi_ = v;}
  void setPidKd(float v){pidKd_ = v;}
  void setElmoStatus(uint8_t v){ElmoStatus_ = v;}

// getter
  uint32_t StartCode() { return startCode_; }
  uint16_t TelemetryType() { return telemetryType_; }
  timeval TimeNow() { return timeNow_; }
  uint32_t TelemetryIndex() { return telemetryIndex_; }
  int32_t RunID() { return runID_; }
  uint16_t CRC() { return crc_; }
  uint32_t StopCode() { return stopCode_; }

// getter of HK で、温度だけやってもいいかも
  uint8_t PivotTemp(){ return PivotTemp_; }
  PivotTemp_ = getValue<int16_t>(22);
  StarCameraTemp_ = getValue<int16_t>(24);
  MirrorTemp_ = getValue<int16_t>();
  GnssTemp_hk_ = getValue<int16_t>();
  CulculatorTemp_ getValue<int16_t>();
  BatteryTemp_ = getValue<int16_t>();
  GyroTemp_ = getValue<int16_t>();
  CmosTemp_ = getValue<int16_t>();
  PcVolt_ = getValue<int16_t>();
  StarCameravolt_ = getValue<int16_t>();
  GnssVolt_ = getValue<int16_t>();
  GyroVolt_ = getValue<int16_t>();
  CmosVolt_ = getValue<int16_t>();
  RouterVolt_ = getValue<int16_t>();
  HeaterVolt_ = getValue<int16_t>();
  Pi-hkVolt_ = getValue<int16_t>();
  PivotVolt_ = getValue<int16_t>();
  HubVolt_ = getValue<int16_t>();
// getter of Elmo
  uint8_t brakeStatus(){ return brakeStatus_; }
  uint8_t moterStatus(){ return moterStatus_; }
  uint8_t unitMode(){ return unitMode_; }
  int16_t posCount(){return posCount_; }
  float ElmoVelocity(){ return ElmoVelocity_; }
  float currentIQ(){ return currentIQ_; }
  float currentID(){ return currentID_; }
  float tgtAzDeg(){ return tgtAzDeg_; }
  float tgtRaDeg(){ return tgtRaDeg_; }
  float tgtDecDeg(){ return tgtDecDeg_; }
  int32_t tgtCount(){ return tgtCount_; }
  uint8_t ctrlMode(){ return ctrlMode_; }
  float pidKp(){return pidKp_; }
  float pidKi(){ return pidKi_; }
  float pidKd(){ return pidKd_; }
  uint8_t ElmoStatus(){ return ElmoStatus_; }
  
// getter of GNSS
  int32_t SeqNo(){ return seqNo_; }
  const std::vector<float>& ypr() const { return ypr_; }
  uint16_t InsStatus(){ return insStatus_; }
  const std::vector<float>& Angrate() const { return angrate_; }
  const std::vector<double>& PosLla() const { return posLla_; }
  const std::vector<float>& GNSSVelocity() const { return GNSSvelocity_; }
  float GNSStemp() { return GNSStemp_; }
  float GNSSpres() { return GNSSpres_; }
  const TUTC& currutc() const { return currutc_; }
  uint16_t IMUsensSat() { return IMUsensSat_;}
  uint8_t GNSSnumSats() { return GNSSnumSats_; }

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

  // Elmo
  uint8_t brakeStatus_ = 0;
  uint8_t moterStatus_ = 0;
  uint8_t unitMode_ = 0;
  int16_t posCount_ = 0;
  float ElmoVelocity_ = 0;
  float currentIQ_ = 0;
  float currentID_ = 0;
  float tgtAzDeg_ = 0;
  float tgtRaDeg_ = 0;
  float tgtDecDeg_ = 0;
  int32_t tgtCount_ = 0;
  uint8_t ctrlMode_ = 0;
  float pidKp_ = 0;
  float pidKi_ = 0;
  float pidKd_ = 0;
  uint8_t ElmoStatus_ = 0;



  // GNSS
  int32_t seqNo_ = 0;
  std::vector<float> ypr_;
  std::vector<float> angrate_;
  std::vector<double>posLla_;
  std::vector<float> GNSSvelocity_;
  uint16_t insStatus_ = 0;
  float GNSStemp_ = 0;
  float GNSSpres_ = 0;
  TUTC currutc_;
  uint16_t IMUsensSat_ = 0;
  uint8_t GNSSnumSats_ = 0;


  // HK
  std::vector<int16_t> RTDTemperatureADC_;
  int16_t PivotTemp_ ;

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
  uint64_t v = static_cast<uint64_t>(input);
  for (int i=0; i<size; i++) {
    const int shift = 8 * (size-1-i);
    telemetry_.push_back(static_cast<uint8_t>((v>>shift) & 0xff));
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
  if (index+byte>n) {
    std::cerr << "TelemetryDefinition::getValue error: out of range" << std::endl;
    std::cerr << "telemetry_.size() = " << n << ", index = " << index << ", byte = " << byte << std::endl;
    return static_cast<T>(0);
  }
  if (byte > 8) {
    std::cerr << "TelemetryDefinition::getValue error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
    return static_cast<T>(0);
  }
  // こいつらでは上手くfloatとかdoubleを扱えなくて、それが気持ち悪いのね
  // uint64_t v = 0;
  // for (int i=0; i<byte; i++) {
  //   const int j = index + i;
  //   // const int shift = 8 * (byte-1-i);
  //   const int shift = 8 * i;//リトルエンディアン
  //   v |= (static_cast<uint64_t>(telemetry_[j]) << shift);
  // }
  T value;
   memcpy(&value, &telemetry_[index], sizeof(T));

  if (std::isnan(value)){
    value=0.0;
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