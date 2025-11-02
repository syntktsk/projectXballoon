#include "TelemetryDefinition.hh"
#include <thread>
#include <chrono>
#include <fstream>
#include <cmath>
#include <cstring>
#include "BinaryFileManipulater.hh"

namespace balloon {

TelemetryDefinition::TelemetryDefinition()
{
  ypr_.resize(3);
  angrate_.resize(3);
  posLla_.resize(3);
  GNSSvelocity_.resize(3);
}

void TelemetryDefinition::generateTelemetry()
{
  clear();
  startCode_ = 0xEB905B6A;
  addValue<uint32_t>(startCode_);
  addValue<uint16_t>(static_cast<uint16_t>(telemetryType_));
  gettimeofday(&timeNow_, NULL);
  addValue<int32_t>(static_cast<int32_t>(timeNow_.tv_sec));
  addValue<int32_t>(static_cast<int32_t>(timeNow_.tv_usec));
  addValue<uint32_t>(telemetryIndex_);
  addValue<int32_t>(runID_);
  telemetryIndex_++;
  
  if (telemetryType_==static_cast<int>(TelemetryType::HK)) {
    generateTelemetryHK();
  }
  else if (telemetryType_==static_cast<int>(TelemetryType::Elmo)) {
    generateTelemetryElmo();
  }
  else if (telemetryType_==static_cast<int>(TelemetryType::GNSS)) {
    generateTelemetryGNSS();
  }
  else if (telemetryType_==static_cast<int>(TelemetryType::Relays)) {
    generateTelemetryRelays();
  }
  else {
    std::cerr << "telemetry type not set appropriately: telemetry_type = " << telemetryType_ << std::endl;
    return;
  }
  writeCRC16();
  stopCode_ = 0xC5A4D279;
  addValue<uint32_t>(stopCode_);
}

void TelemetryDefinition::generateTelemetryHK()
{
  writeRTDTemperature();  
}

void TelemetryDefinition::generateTelemetryElmo()
{
  addValue<uint8_t>(brakeStatus_);
  addValue<uint8_t>(moterStatus_);
  addValue<uint8_t>(unitMode_);
  addValue<int16_t>(posCount_);
  addValue<float>(ElmoVelocity_);
  addValue<float>(currentIQ_);
  addValue<float>(currentID_);
  addValue<float>(tgtAzDeg_);
  addValue<float>(tgtRaDeg_);
  addValue<float>(tgtDecDeg_);
  addValue<int32_t>(tgtCount_);
  addValue<uint8_t>(ctrlMode_);
  addValue<float>(pidKp_);
  addValue<float>(pidKi_);
  addValue<float>(pidKd_);
  addValue<uint8_t>(ElmoStatus_);

}

void TelemetryDefinition::generateTelemetryGNSS()
{
    addValue<int32_t>(seqNo_);
    addVector<float>(ypr_);
    addVector<float>(angrate_);
    addVector<double>(posLla_);
    addVector<float>(GNSSvelocity_);
    addValue<uint16_t>(insStatus_);
    addValue<float>(GNSStemp_);
    addValue<float>(GNSSpres_);
    addValue<int8_t>(currutc_.year);
    addValue<uint8_t>(currutc_.month);
    addValue<uint8_t>(currutc_.day);
    addValue<uint8_t>(currutc_.hour);
    addValue<uint8_t>(currutc_.minute);
    addValue<uint8_t>(currutc_.second);
    addValue<uint16_t>(currutc_.fracsec);
    addValue<uint16_t>(IMUsensSat_);
    addValue<uint8_t>(GNSSnumSats_);
}

void TelemetryDefinition::generateTelemetryRelays()
{
}

void TelemetryDefinition::writeRTDTemperature()
{
  const int buf_size = 1;
  const int n = RTDTemperatureADC_.size();
  std::vector<uint16_t> temperature(buf_size, 0);
  for (int i=0; i<n; i++) {
    if (i==buf_size) break;
    temperature[i] = RTDTemperatureADC_[i];
  }
  addVector<uint16_t>(temperature);
}

void TelemetryDefinition::writeCRC16()
{
  uint16_t crc = calcCRC16(telemetry_);
  addValue<uint16_t>(crc);
}

bool TelemetryDefinition::setTelemetry(const std::vector<uint8_t>& v)
{
  int n = v.size();
  if (n<10) {
    std::cerr << "Telemetry is too short!!: length = " << n << std::endl; 
    return false;
  }

  if (v[0]!=0xEB || v[1]!=0x90 || v[2]!=0x5B || v[3]!=0x6A) {
    std::cerr << "start code incorect" << std::endl;
    return false;
  }
  if (v[n-4]!=0xC5 || v[n-3]!=0xA4 || v[n-2]!=0xD2 || v[n-1]!=0x79) {
    std::cerr << "stop code incorrect" << std::endl;
    return false;
  }

  telemetry_ = v;
  
  // ＞＞＞＞＞＞＞＞＞ISAS実験の時だけコメントアウト＞＞＞＞＞＞＞＞＞＞
  uint16_t type = getValue<uint16_t>(4);
  const int hk_telemetry_len = 64;
  const int Elmo_telemetry_len = 36;
  const int GNSS_telemetry_len = 96;
  const int Relays_telemetry_len = 0;
  if (type==1) {
    if (n!=hk_telemetry_len) {
      std::cerr << "Telemetry HK: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }
  else if (type==2) {
    if (n!=Elmo_telemetry_len) {
      std::cerr << "Telemetry Status: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }
  else if (type==3) {
    if (n!=GNSS_telemetry_len) {
      std::cerr << "Telemetry Status: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }
  else if (type==4) {
    if (n!=Relays_telemetry_len) {
      std::cerr << "Telemetry Status: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }
  else {
    std::cerr << "Invalid telemetry type: type = " << type << std::endl;
  }

  std::vector<uint8_t> telem_without_fotter;
  for (int i=0; i<n-6; i++) {
    telem_without_fotter.push_back(telemetry_[i]);
  }

  uint16_t crc_calc = calcCRC16(telem_without_fotter);
  uint16_t crc_attached = getValue<uint16_t>(n-6);
  if (crc_calc != crc_attached) {
    if (type==2) {
      std::cerr << "CRC16 is not appropriate.\nBut for now, we process the telemetry as valid one since it is too long." << std::endl;
      std::cerr << "Telemetry is very long: size = " << v.size() << std::endl;
      return true;
    }
    if (type != 2) {
      std::cerr << "Invalid telemetry: CRC16 not appropriate" << std::endl;
      return false;
    }
  }
  // >>>>>>>>>>>>>>>ここまで>>>>>>>>>>>>>>>>>>>

  return true;
}

void TelemetryDefinition::interpret()
{
  startCode_ = getValue<uint32_t>(0);
  telemetryType_ = getValue<uint16_t>(4);
  timeNow_.tv_sec = getValue<int32_t>(6);
  timeNow_.tv_usec = getValue<int32_t>(10);
  telemetryIndex_ = getValue<uint32_t>(14);
  runID_ = getValue<int32_t>(18);
  if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::HK)) {
    interpretHK();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Elmo)) {
    interpretElmo();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::GNSS)) {
    interpretGNSS();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Relays)) {
    interpretRelays();
  }
  else {
    std::cerr << "could not interpret telemetry: telemetry_type = " << telemetryType_ << std::endl;
  }
}

void TelemetryDefinition::interpretHK()
{
  // getVector<int16_t>(22,1, RTDTemperatureADC_);
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
  
  crc_ = getValue<uint16_t>(24);
  stopCode_ = getValue<uint32_t>(26);
  
}

void TelemetryDefinition::interpretGNSS()
{
  seqNo_ = getValue<int32_t>(22);
  getVector<float>(26,3,ypr_);
  getVector<float>(38,3,angrate_);
  getVector<double>(54,3,posLla_);
  getVector<float>(78,3,GNSSvelocity_);
  insStatus_ = getValue<uint16_t>(92);
  GNSStemp_ = getValue<float>(94);
  GNSSpres_ = getValue<float>(98);
  currutc_.year = getValue<int8_t>(102);
  currutc_.month = getValue<uint8_t>(103);
  currutc_.day = getValue<uint8_t>(104);
  currutc_.hour = getValue<uint8_t>(105);
  currutc_.minute = getValue<uint8_t>(106);
  currutc_.second = getValue<uint8_t>(107);
  currutc_.fracsec = getValue<uint16_t>(108);
  IMUsensSat_ = getValue<uint16_t>(110);
  GNSSnumSats_ = getValue<uint8_t>(112);  

  crc_ = getValue<uint16_t>(113);
  stopCode_ = getValue<uint32_t>(115);
}

void TelemetryDefinition::interpretElmo()
{
  brakeStatus_ = getValue<uint8_t>(22);
  moterStatus_ = getValue<uint8_t>(23);
  unitMode_ = getValue<uint8_t>(24);
  posCount_ = getValue<int16_t>(25);
  ElmoVelocity_ = getValue<float>(29);
  currentIQ_ = getValue<float>(33);
  currentID_ = getValue<float>(37);
  tgtAzDeg_ = getValue<float>(41);
  tgtRaDeg_ = getValue<float>(45);
  tgtDecDeg_ = getValue<float>(49);
  tgtCount_ = getValue<int32_t>(53);
  ctrlMode_ = getValue<uint8_t>(57);
  pidKp_ = getValue<float>(58);
  pidKi_ = getValue<float>(62);
  pidKd_ = getValue<float>(66);
  ElmoStatus_ = getValue<uint8_t>(70);

  crc_ = getValue<uint16_t>(95);
  stopCode_ = getValue<uint32_t>(97);
}

void TelemetryDefinition::interpretRelays()
{
}


void TelemetryDefinition::clear()
{
  telemetry_.clear();
}

void TelemetryDefinition::writeFile(const std::string& filename, bool append)
{
  writeVectorToBinaryFile<uint8_t>(filename, append, telemetry_);
}


} /* namespace balloon */
