#include "TelemetryDefinition.hh"
#include <thread>
#include <chrono>
#include <fstream>
#include <cmath>
#include <tuple>
#include <cstring>
#include "BinaryFileManipulater.hh"
#include "CRC16.hh"

namespace balloon {

TelemetryDefinition::TelemetryDefinition()
{ 
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
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Whole)) {
    generateTelemetryElmo();
    generateTelemetryGNSS();
    generateTelemetryHK();

  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Optional)) {
    generateTelemetryOption();
  }
  else {
    std::cerr << "telemetry type not set appropriately: telemetry_type = " << telemetryType_ << std::endl;
    return;
  }
  writeCRC16();
  stopCode_ = 0xC5A4D279;
  addValue<uint32_t>(stopCode_);
}

void TelemetryDefinition::generateTelemetryWhole()
{
  addValue<int>(MO_);
  addValue<int>(UM_);
  addValue<float>(MF_);
  addValue<int>(EC_);
  addValue<int>(PX_);
  addValue<float>(VX_);
  addValue<float>(IQ_);
  addValue<float>(ID_);
  addValue<float>(MC_);
  addValue<int>(BV_);
  addValue<int>(TI_);
  addValue<float>(TC_);
  addValue<float>(JV_);
  addValue<int>(PA_);
  addValue<int>(PR_);
  addValue<int>(ac_);
  addValue<int>(ef_);
  addValue<int>(ps_);
  addValue<int>(elc_);
  addValue<float>(la_);
  addValue<float>(lo_);
  addValue<float>(he_);
  addValue<float>(ya_);
  addValue<float>(pi_);
  addValue<float>(ro_);
  addValue<float>(te_);

}

void TelemetryDefinition::generateTelemetryHK()
{
  addValue<int>(lastCommandIndex_);
  addValue<int>(lastCommandCode_);
  addValue<int>(commandRejectCount_);
  addValue<uint64_t>(softwareErrorCode_);
}
void TelemetryDefinition::generateTelemetryElmo()
{
  addValue<int>(MO_);
  addValue<int>(UM_);
  addValue<float>(MF_);
  addValue<int>(EC_);
  addValue<int>(PX_);
  addValue<float>(VX_);
  addValue<float>(IQ_);
  addValue<float>(ID_);
  addValue<float>(MC_);
  addValue<int>(BV_);
  addValue<int>(TI_);
  addValue<float>(TC_);
  addValue<float>(JV_);
  addValue<int>(PA_);
  addValue<int>(PR_);
  addValue<int>(ac_);
  addValue<int>(ef_);
  addValue<int>(ps_);
  addValue<int>(elc_);

}

void TelemetryDefinition::generateTelemetryGNSS()
{
  addValue<float>(la_);
  addValue<float>(lo_);
  addValue<float>(he_);
  addValue<float>(ya_);
  addValue<float>(pi_);
  addValue<float>(ro_);
  addValue<float>(te_);

}

void TelemetryDefinition::generateTelemetryRelays()
{
}

void TelemetryDefinition::generateTelemetryOption()
{ 
  const int size = er_.size();
  int pad_size = 16 - size;
  for (int i=0; i< size ; i++){ 
    char unit = er_[i];
    telemetry_.push_back(unit);
  }
  for (int j=0; j < pad_size ; j++){
    telemetry_.push_back('\0');
  }
}

void TelemetryDefinition::writeGL860value()
{
  const int buf_size = 18;
  const int n = getGL860DataVec_.size();
  std::vector<uint16_t> HKvalue(buf_size, 0);
  for (int i=0; i<n; i++) {
    if (i==buf_size) break;
    HKvalue[i] =getGL860DataVec_[i];
    addValue<int16_t>(HKvalue[i]);
  }
  int index = 0; // または、既存のインデックス変数を使用
  setPivotTemp(HKvalue[index++]);
  setStarCameraTemp(HKvalue[index++]);
  setMirrorTemp(HKvalue[index++]);
  setGnssTemp_hk(HKvalue[index++]);
  setCulculatorTemp(HKvalue[index++]);
  setBatteryTemp(HKvalue[index++]);
  setGyroTemp(HKvalue[index++]);
  setCmosTemp(HKvalue[index++]);
  setPcVolt(HKvalue[index++]);
  setStarCameravolt(HKvalue[index++]);
  setGnssVolt(HKvalue[index++]);
  setGyroVolt(HKvalue[index++]);
  setCmosVolt(HKvalue[index++]);
  setRouterVolt(HKvalue[index++]);
  setHeaterVolt(HKvalue[index++]);
  setPi_hkVolt(HKvalue[index++]);
  setPivotVolt(HKvalue[index++]);
  setHubVolt(HKvalue[index++]);
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
  
  uint16_t type = getValue<uint16_t>(4);
  const int whole_telemetry_len = 28+100+20+4;
  const int hk_telemetry_len = 28+72;
  const int Elmo_telemetry_len = 28+72;
  const int GNSS_telemetry_len = 28+28;
  const int Relays_telemetry_len = 28;
  const int  optional_telemetry_len=44;
  if (type==9) {
    if (n!=whole_telemetry_len) {
      std::cerr << "Telemetry Whole: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }else if (type==1) {
    if (n!=hk_telemetry_len) {
      std::cerr << "Telemetry HK: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }else if (type==3) {
    if (n!=Elmo_telemetry_len) {
      std::cerr << "Telemetry Elmo Status: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }  else if (type==2) {
    if (n!=GNSS_telemetry_len) {
      std::cerr << "Telemetry GNSS Status: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }
  else if (type==4) {
    if (n!=Relays_telemetry_len) {
      std::cerr << "Telemetry Relay Status: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }
  else if (type==5) {
    if (n!=optional_telemetry_len) {
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
  // これなんだ
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

  return true;
}

void TelemetryDefinition::interpret()
{
  startCode_ = getValue<uint32_t>(0);
  telemetryType_ = getValue<uint16_t>(4);
  timeNow_.tv_sec = getValue<int32_t>(8);
  timeNow_.tv_usec = getValue<int32_t>(12);
  telemetryIndex_ = getValue<uint32_t>(16);
  runID_ = getValue<int32_t>(20);
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
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Whole)) {
    interpretWhole();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Optional)) {
    interpretOption(22);
  }
  else {
    std::cerr << "could not interpret telemetry: telemetry_type = " << telemetryType_ << std::endl;
  }
}

void TelemetryDefinition::interpretWhole(){
  printf("\n!!! INTERPRET START !!! size=%d\n", (int)telemetry_.size());
  int i = 22; 
  MO_ = getValue<int>(i); i += 4;
  UM_ = getValue<int>(i); i += 4;
  MF_ = getValue<float>(i); i += 4;
  EC_ = getValue<int>(i); i += 4;
  PX_ = getValue<int>(i); i += 4;
  VX_ = getValue<float>(i); i += 4;
  IQ_ = getValue<float>(i); i += 4;
  ID_ = getValue<float>(i); i += 4;
  MC_ = getValue<float>(i); i += 4;
  BV_ = getValue<int>(i); i += 4;
  TI_ = getValue<int>(i); i += 4;
  TC_ = getValue<float>(i); i += 4;
  JV_ = getValue<float>(i); i += 4;
  PA_ = getValue<int>(i); i += 4;
  PR_ = getValue<int>(i); i += 4;
  ac_ = getValue<int>(i); i += 4;
  ef_ = getValue<int>(i); i += 4;
  ps_ = getValue<int>(i); i += 4;
  elc_= getValue<int>(i); i += 4;
  la_ = getValue<float>(i); i += 4;
  lo_ = getValue<float>(i); i += 4;
  he_ = getValue<float>(i); i += 4;
  ya_ = getValue<float>(i); i += 4;
  pi_ = getValue<float>(i); i += 4;
  ro_ = getValue<float>(i); i += 4;
  te_ = getValue<float>(i); i += 4;
  lastCommandIndex_=getValue<int>(i); i += 4;
  lastCommandCode_=getValue<int>(i); i += 4;
  commandRejectCount_=getValue<int>(i); i += 4;
  softwareErrorCode_=getValue<uint64_t>(i);
  printf("\n!!! software error code !!! value=%llu\n", softwareErrorCode_);
  int n = telemetry_.size();
  crc_ = getValue<uint16_t>(n - 6); 
  stopCode_ = getValue<uint32_t>(n - 4);
}

void TelemetryDefinition::interpretHK()
{
  // getVector<int16_t>(22,1, RTDTemperatureADC_);
  int i=24;
  PivotTemp_ = getValue<int16_t>(i);  i=i+sizeof(int16_t);
  StarCameraTemp_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  MirrorTemp_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  GnssTemp_hk_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  CulculatorTemp_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  BatteryTemp_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  GyroTemp_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  CmosTemp_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  PcVolt_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  StarCameravolt_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  GnssVolt_ = getValue<int16_t>(i);i = i + sizeof(int16_t);
  GyroVolt_ = getValue<int16_t>(i);i = i + sizeof(int16_t);
  CmosVolt_ = getValue<int16_t>(i);i = i + sizeof(int16_t);
  RouterVolt_ = getValue<int16_t>(i);i = i + sizeof(int16_t);
  HeaterVolt_ = getValue<int16_t>(i);i = i + sizeof(int16_t);
  Pi_hkVolt_ = getValue<int16_t>(i);i = i + sizeof(int16_t);
  PivotVolt_ = getValue<int16_t>(i);i = i + sizeof(int16_t);
  HubVolt_ = getValue<int16_t>(i);i = i + sizeof(int16_t);

  crc_ = getValue<uint16_t>(i);i = i + sizeof(uint16_t);
  stopCode_ = getValue<uint32_t>(i);
}

void TelemetryDefinition::interpretGNSS()
{
  int i=22;
  la_ = getValue<float>(i);i = i + sizeof(float);
  lo_ = getValue<float>(i);i = i + sizeof(float);
  he_ = getValue<float>(i);i = i + sizeof(float);
  ya_ = getValue<float>(i);i = i + sizeof(float);
  pi_ = getValue<float>(i);i = i + sizeof(float);
  ro_ = getValue<float>(i);i = i + sizeof(float);
  te_ = getValue<float>(i);i = i + sizeof(float);

  int n = telemetry_.size();
  crc_ = getValue<uint16_t>(n - 6); 
  stopCode_ = getValue<uint32_t>(n - 4);
}
void TelemetryDefinition::interpretElmo()
{
// Elmoの解釈作る
  int i=22;
  MO_ = getValue<int>(i);i = i + sizeof(int);
  UM_ = getValue<int>(i);i = i + sizeof(int);
  MF_ = getValue<float>(i);i = i + sizeof(float);
  EC_ = getValue<int>(i);i = i + sizeof(int);
  PX_ = getValue<int>(i);i = i + sizeof(int);
  VX_ = getValue<float>(i);i = i + sizeof(float);
  IQ_ = getValue<float>(i);i = i + sizeof(float);
  ID_ = getValue<float>(i);i = i + sizeof(float);
  MC_ = getValue<float>(i);i = i + sizeof(float);
  BV_ = getValue<int>(i);i = i + sizeof(int);
  TI_ = getValue<int>(i);i = i + sizeof(int);
  TC_ = getValue<float>(i);i = i + sizeof(float);
  JV_ = getValue<float>(i);i = i + sizeof(float);
  PA_ = getValue<int>(i);i = i + sizeof(int);
  PR_ = getValue<int>(i);i = i + sizeof(int);
  ac_ = getValue<int>(i);i = i + sizeof(int);
  ef_ = getValue<int>(i);i = i + sizeof(int);
  ps_ = getValue<int>(i);i = i + sizeof(int);
  elc_= getValue<int>(i);i = i + sizeof(int);

  int n = telemetry_.size();
  crc_ = getValue<uint16_t>(n - 6); 
  stopCode_ = getValue<uint32_t>(n - 4);
}

void TelemetryDefinition::interpretRelays()
{
}

void TelemetryDefinition::interpretOption(int header_size)
{
  // 1. データ位置と長さの定義
  const size_t HEADER_OFFSET = header_size;
  const size_t DATA_LENGTH = 16;

  // 3. 格納先の文字列 (er_) をクリア
  er_.clear();

  // 4. 該当部分 (22バイト目から16バイト分) をループで読み込み
  for (size_t i = 0; i < DATA_LENGTH; ++i) {
      
      // データを取得するインデックス
      size_t current_index = HEADER_OFFSET+ i;

      // 該当する1バイトの値を取得
      uint8_t byte_value = telemetry_[current_index];

      // ヌル文字 (\0) を検出したら、そこで文字列の終わりと判断し、ループを終了
      if (byte_value == '\0') {
          break;
      }

      // uint8_t を char にキャストし、er_ の末尾に追加
      // （これにより、er_に有効な文字のみが格納される）
      er_.push_back(static_cast<char>(byte_value));
  }
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