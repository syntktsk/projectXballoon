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
    generateTelemetryRelays();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Optional)) {
    generateTelemetryOption();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::GL860)) {
    generateTelemetryGL860();
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
  addValue<uint32_t>(static_cast<uint32_t>(lastCommandIndex_));
  addValue<uint16_t>(static_cast<uint16_t>(lastCommandCode_));
  addValue<uint16_t>(static_cast<uint16_t>(commandRejectCount_));
  addValue<uint64_t>(softwareErrorCode_);
  writeGL860value(getGL860DataVec_);
}
void TelemetryDefinition::generateTelemetryElmo()
{
  addValue<uint8_t>(static_cast<uint8_t>(elmo_status.MO));
  addValue<uint8_t>(static_cast<uint8_t>(elmo_status.UM));
  addValue<uint8_t>(static_cast<uint8_t>(elmo_status.OB));
  // addValue<float>(elmo_status.MF);
  addValue<int32_t>(static_cast<int32_t>(elmo_status.MF * mul));
  // std::cout <<"DEBUG MF become :"<<static_cast<int32_t>(elmo_status.MF * mul)<<std::endl;
  addValue<int>(elmo_status.EC);
  addValue<int>(elmo_status.PX);
  addValue<int32_t>(static_cast<int32_t>(elmo_status.VX * mul));
  addValue<int32_t>(static_cast<int32_t>(elmo_status.IQ * mul));
  addValue<int32_t>(static_cast<int32_t>(elmo_status.ID * mul));
  addValue<int32_t>(static_cast<int32_t>(elmo_status.MC * mul));
  // addValue<float>(elmo_status.VX);
  // addValue<float>(elmo_status.IQ);
  // addValue<float>(elmo_status.ID);
  // addValue<float>(elmo_status.MC);
  addValue<int>(elmo_status.BV);
  addValue<int8_t>(static_cast<int8_t>(elmo_status.TI));
  addValue<int32_t>(static_cast<int32_t>(elmo_status.TC * mul));
  addValue<int32_t>(static_cast<int32_t>(elmo_status.JV * mul));
  // addValue<float>(elmo_status.TC);
  // addValue<float>(elmo_status.JV);
  addValue<int>(elmo_status.PA);
  addValue<int>(elmo_status.PR);
  addValue<uint8_t>(static_cast<uint8_t>(elmo_status.ac));
  addValue<uint8_t>(static_cast<uint8_t>(elmo_status.ef));
  addValue<uint8_t>(static_cast<uint8_t>(elmo_status.ps));
  addValue<int32_t>(static_cast<int32_t>(elmo_status.en));
  addValue<int32_t>(static_cast<int32_t>(elmo_status.az * mul));
  // addValue<float>(static_cast<float>(elmo_status.az));
  addValue<int32_t>(static_cast<int32_t>(elmo_status.hi));
  lc_ = elmo_status.lc;
  const int size = lc_.size();
  int pad_size = 16 - size;
  for (int i=0; i< size ; i++){ 
    char unit = lc_[i];
    telemetry_.push_back(unit);
  }
  for (int j=0; j < pad_size ; j++){
    telemetry_.push_back('\0');
  }
}

void TelemetryDefinition::generateTelemetryGNSS()
{
//   addValue<float>(gnss_status.la);
//   addValue<float>(gnss_status.lo);
//   addValue<float>(gnss_status.he);
//   addValue<float>(gnss_status.ya);
//   addValue<float>(gnss_status.pi);
//   addValue<float>(gnss_status.ro);
//   addValue<float>(gnss_status.te);
// // 緯度・経度・高度・姿勢などは範囲が広いため int32_t で処理
  addValue<int32_t>(static_cast<int32_t>(gnss_status.la * mul));
  addValue<int32_t>(static_cast<int32_t>(gnss_status.lo * mul));
  addValue<int32_t>(static_cast<int32_t>(gnss_status.he * mul));
  addValue<int32_t>(static_cast<int32_t>(gnss_status.ya * mul));
  addValue<int32_t>(static_cast<int32_t>(gnss_status.pi * mul));
  addValue<int32_t>(static_cast<int32_t>(gnss_status.ro * mul));
  addValue<int32_t>(static_cast<int32_t>(gnss_status.te * mul));
}

void TelemetryDefinition::generateTelemetryRelays()
{ 
  addValue<uint32_t>(relay_);
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

void TelemetryDefinition::generateTelemetryGL860()
{
  // res->comの順でペイロード64Byte
  const int MAX_LEN=32;
  int size = gl860string_.size();
  if (size > MAX_LEN) {
      size = MAX_LEN;
  }
  int pad_size = MAX_LEN - size;
  for (int i=0; i< size ; i++){ 
    char unit = gl860string_[i];
    telemetry_.push_back(unit);
  }
  for (int j=0; j < pad_size ; j++){
    telemetry_.push_back('\0');
  }

  const int MAX_CMD_LEN=32;
  int size_cmd = lastCommandGL860_.size();
  if (size_cmd > MAX_CMD_LEN) {
      size_cmd = MAX_CMD_LEN;
  }
  int pad_size_cmd = MAX_CMD_LEN - size_cmd;
  for (int i=0; i< size_cmd ; i++){ 
    char unit = lastCommandGL860_[i];
    telemetry_.push_back(unit);
  }
  for (int j=0; j < pad_size_cmd ; j++){
    telemetry_.push_back('\0');
  }
}

void TelemetryDefinition::writeGL860value(std::vector<int16_t>& getGL860DataVec_)
{ 
  const int buf_size = 22;
  const int n = getGL860DataVec_.size();
  std::vector<uint16_t> HKvalue(buf_size, 0);
  for (int i=0; i<n; i++) {
    if (i==buf_size) break;
    HKvalue[i] =getGL860DataVec_[i];
  }
  for (int i=0; i<buf_size; i++){
    addValue<int16_t>(HKvalue[i]);
  }

}

void TelemetryDefinition::writeGL860status(std::vector<uint8_t>& range_info_)
{ 
  const int buf_size = 41;
  const int n = range_info_.size();
  std::vector<uint16_t> HKstatus(buf_size, 0);
  for (int i=0; i<n; i++) {
    if (i==buf_size) break;
    HKstatus[i] =range_info_[i];
  }
  for (int i=0; i<buf_size; i++){
    addValue<int16_t>(HKstatus[i]);
  }
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
  const int whole_telemetry_len = 203;//28+GNSSSize_+elmoSize_+optSize_+hkSize_;
  const int hk_telemetry_len = 28+hkSize_;
  const int Elmo_telemetry_len = 28+elmoSize_;
  const int GNSS_telemetry_len = 28+GNSSSize_;
  const int Relays_telemetry_len = 28+relaysSize_;
  const int  optional_telemetry_len=28+optSize_;
  const int  gl860_telemetry_len=28+gl860Size_;
  if (type==9) {
    if (n!=whole_telemetry_len) {
      std::cerr << "Telemetry Whole: Telemetry length is not correct: n = " << n << ". It's should be "<< whole_telemetry_len << std::endl;
      telemetryType_ = 0;
      return false;
    }
  }else if (type==1) {
    if (n!=hk_telemetry_len) {
      std::cerr << "Telemetry HK: Telemetry length is not correct: n = " << n << std::endl;
      telemetryType_ = 0;
      return false;
    }
  }else if (type==3) {
    if (n!=Elmo_telemetry_len) {
      std::cerr << "Telemetry Elmo Status: Telemetry length is not correct: n = " << n << std::endl;
      telemetryType_ = 0;
      return false;
    }
  }  else if (type==2) {
    if (n!=GNSS_telemetry_len) {
      std::cerr << "Telemetry GNSS Status: Telemetry length is not correct: n = " << n << std::endl;
      telemetryType_ = 0;
      return false;
    }
  }
  else if (type==4) {
    if (n!=Relays_telemetry_len) {
      std::cerr << "Telemetry Relay Status: Telemetry length is not correct: n = " << n << std::endl;
      telemetryType_ = 0;
      return false;
    }
  }
  else if (type==5) {
    if (n!=optional_telemetry_len) {
      std::cerr << "Telemetry Status: Telemetry length is not correct: n = " << n << std::endl;
      telemetryType_ = 0;
      return false;
    }
  }
  else if (type==6) {
    if (n!=gl860_telemetry_len) {
      std::cerr << "Telemetry Status: Telemetry length is not correct: n = " << n << std::endl;
      telemetryType_ = 0;
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
{ std::cout << "koreyatteru???"<<std::endl;
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
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Whole)) {
    interpretWhole();
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::Optional)) {
    interpretOption(22);
  }
  else if (static_cast<int>(telemetryType_)==static_cast<int>(TelemetryType::GL860)) {
    interpretGL860option();
  }
  else {
    std::cerr << "could not interpret telemetry: telemetry_type = " << telemetryType_ << std::endl;
  }
}

void TelemetryDefinition::interpretWhole(){
  // printf("\n!!! INTERPRET START !!! size=%d\n", (int)telemetry_.size());
  int i = 22; 
  MO_ = getValue<uint8_t>(i);i = i + sizeof(MO_);
  UM_ = getValue<uint8_t>(i);i = i + sizeof(UM_);
  OB_ = getValue<uint8_t>(i);i = i + sizeof(OB_);
  MF_ = getValue<int32_t>(i)/mul; i += 4;
  // std::cout << "DEBUG: show each num EC ->"<< i <<std::endl;
  EC_ = getValue<int>(i);    i = i + sizeof(EC_);
  PX_ = getValue<int>(i);    i = i + sizeof(PX_);
  VX_ = getValue<int32_t>(i)/mul; i += 4;
  IQ_ = getValue<int32_t>(i)/mul; i += 4;
  // std::cout << "DEBUG: show each num  ID ->"<< i <<std::endl;
  ID_ = getValue<int32_t>(i)/mul; i += 4;
  MC_ = getValue<int32_t>(i)/mul; i += 4;
  BV_ = getValue<int>(i);    i = i + sizeof(BV_);
  TI_ = getValue<int8_t>(i); i = i + sizeof(TI_);
  // std::cout << "DEBUG: show each num TI->"<< i <<std::endl;
  TC_ = getValue<int32_t>(i)/mul; i += 4;
  JV_ = getValue<int32_t>(i)/mul; i += 4;
  PA_ = getValue<int>(i);    i = i + sizeof(PA_);
  PR_ = getValue<int>(i);    i = i + sizeof(PR_);
  // std::cout << "DEBUG: show each num AC->"<< i <<std::endl;
  ac_ = getValue<uint8_t>(i);i = i + sizeof(ac_);
  ef_ = getValue<uint8_t>(i);i = i + sizeof(ef_);
  ps_ = getValue<uint8_t>(i);i = i + sizeof(ps_);
  en_ = getValue<int32_t>(i);i = i + sizeof(en_);
  az_ = getValue<int32_t>(i)/mul; i += 4;
  hi_ = getValue<int32_t>(i);i = i + sizeof(hi_);
  const size_t DATA_LENGTH = 16;
  lc_.clear();
  for (size_t j = 0; j < DATA_LENGTH; ++j) {
    size_t current_index = i + j;
    uint8_t byte_value = telemetry_[current_index];

    if (byte_value == '\0') {
        break;
    }
    lc_.push_back(static_cast<char>(byte_value));
  }
  i = i + DATA_LENGTH;
  la_ = getValue<int32_t>(i)/mul; i += 4;
  lo_ = getValue<int32_t>(i)/mul; i += 4;
  he_ = getValue<int32_t>(i)/mul; i += 4;
  ya_ = getValue<int32_t>(i)/mul; i += 4;
  pi_ = getValue<int32_t>(i)/mul; i += 4;
  ro_ = getValue<int32_t>(i)/mul; i += 4;
  te_ = getValue<int32_t>(i)/mul; i += 4;
  // la_ = getValue<float>(i); i += 4;
  // lo_ = getValue<float>(i); i += 4;
  // he_ = getValue<float>(i); i += 4;
  // ya_ = getValue<float>(i); i += 4;
  // pi_ = getValue<float>(i); i += 4;
  // ro_ = getValue<float>(i); i += 4;
  // te_ = getValue<float>(i); i += 4;
  lastCommandIndex_=getValue<uint32_t>(i); i += sizeof(lastCommandIndex_); 
  lastCommandCode_=getValue<uint16_t>(i); i += sizeof(lastCommandCode_);
  commandRejectCount_=getValue<uint16_t>(i); i += sizeof(commandRejectCount_);
  softwareErrorCode_=getValue<uint64_t>(i);i += sizeof(softwareErrorCode_);
  int datanum_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  (void)datanum_;
  gl860_ground_.clear();
  for (int ch = 0; ch < 20; ++ch) {
    int16_t raw_val = getValue<int16_t>(i);
    i += sizeof(int16_t);
    float physical_val = static_cast<float>((raw_val) / range_info_[ch]);
    gl860_ground_.push_back(physical_val);
  }
  relay_ = getValue<uint32_t>(i);    i = i + sizeof(relay_);
  for (i=0; i< 32; i++){

  }
  int n = telemetry_.size();
  crc_ = getValue<uint16_t>(n - 6); 
  stopCode_ = getValue<uint32_t>(n - 4);
}

void TelemetryDefinition::interpretHK()
{
  int i=22;
  int datanum_ = getValue<int16_t>(i);  i = i + sizeof(int16_t);
  (void)datanum_;
  gl860_ground_.clear();
  for (int ch = 0; ch < 20; ++ch) {
    int16_t raw_val = getValue<int16_t>(i);
    i += sizeof(int16_t);
    float physical_val = static_cast<float>((raw_val) / range_info_[ch]);
    gl860_ground_.push_back(physical_val);
  }
  uint8_t packed_logic = getValue<uint16_t>(i);
  gl860_logic_.resize(4);
  for (int j = 0; j < 4; ++j) {
    if ((packed_logic & (1 << j)) != 0) {
        gl860_logic_[j] = "H";
    } else {
        gl860_logic_[j] = "L";
    }
  }
  crc_ = getValue<uint16_t>(i);i = i + sizeof(uint16_t);
  stopCode_ = getValue<uint32_t>(i);
}

void TelemetryDefinition::interpretGL860stat(){
  int i = 22;
  int n_total = 40;
  
  range_info_.clear();
  range_info_.resize(20, 1.0f); // 20ch分を1.0で初期化

  for (int j = 0; j < n_total; j += 2) {
    int8_t ch_id = getValue<int8_t>(i); i += 2; // 偶数項：CH番号 (1〜20)
    int8_t range_id = getValue<int8_t>(i); i += 2; // 奇数項：RangeID

    float m = 1.0f;
    for (const auto& info : RANGE_TABLE) {
      if (info.range_id == (uint8_t)range_id) {
        m = static_cast<float>(info.multiplier);
        break;
      }
    }
    int ch_idx = ch_id - 1; 
    if (ch_idx >= 0 && ch_idx < 20) {
      range_info_[ch_idx] = m;
    }
  }
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
  int i=22;
  MO_ = getValue<uint8_t>(i);i = i + sizeof(MO_);
  UM_ = getValue<uint8_t>(i);i = i + sizeof(UM_);
  MF_ = getValue<float>(i);  i = i + sizeof(MF_);
  EC_ = getValue<int>(i);    i = i + sizeof(EC_);
  PX_ = getValue<int>(i);    i = i + sizeof(PX_);
  VX_ = getValue<float>(i);  i = i + sizeof(VX_);
  IQ_ = getValue<float>(i);  i = i + sizeof(IQ_);
  ID_ = getValue<float>(i);  i = i + sizeof(ID_);
  MC_ = getValue<float>(i);  i = i + sizeof(MC_);
  BV_ = getValue<int>(i);    i = i + sizeof(BV_);
  TI_ = getValue<int8_t>(i); i = i + sizeof(TI_);
  TC_ = getValue<float>(i);  i = i + sizeof(TC_);
  JV_ = getValue<float>(i);  i = i + sizeof(JV_);
  PA_ = getValue<int>(i);    i = i + sizeof(PA_);
  PR_ = getValue<int>(i);    i = i + sizeof(PR_);
  ac_ = getValue<uint8_t>(i);i = i + sizeof(ac_);
  ef_ = getValue<uint8_t>(i);i = i + sizeof(ef_);
  ps_ = getValue<uint8_t>(i);i = i + sizeof(ps_);
  lc_= getValue<int>(i);     i = i + sizeof(lc_);

  int n = telemetry_.size();
  crc_ = getValue<uint16_t>(n - 6); 
  stopCode_ = getValue<uint32_t>(n - 4);
}

void TelemetryDefinition::interpretRelays()
{
  int i=22;
  relay_ = getValue<uint32_t>(i);i = i + sizeof(relay_);
  int j;
  for (j = 0; j < 32; j++) {
    relaysStatus_[j] = (relay_ >> j) & 1;
  }
  int n = telemetry_.size();
  crc_ = getValue<uint16_t>(n - 6); 
  stopCode_ = getValue<uint32_t>(n - 4);
}

void TelemetryDefinition::interpretOption(int header_size)
{
  const size_t HEADER_OFFSET = header_size;
  const size_t DATA_LENGTH = 16;
  er_.clear();
  for (size_t i = 0; i < DATA_LENGTH; ++i) {
      size_t current_index = HEADER_OFFSET+ i;
      uint8_t byte_value = telemetry_[current_index];

      if (byte_value == '\0') {
          break;
      }
      er_.push_back(static_cast<char>(byte_value));
  }
}
void TelemetryDefinition::interpretGL860option()
{
  // 1. データ位置と長さの定義
  const size_t HEADER_OFFSET = 22;
  const size_t RES_LENGTH = 32;
  const size_t CMD_LENGTH = 32;

  lastCommandGL860_.clear();
  gl860string_.clear();
  for (size_t i = 0; i < RES_LENGTH; ++i) {
    size_t current_index = HEADER_OFFSET+ i;

    uint8_t byte_value = telemetry_[current_index];
    if (byte_value == '\0') {
        break;
    }
    gl860string_.push_back(static_cast<char>(byte_value));
  }
  for (size_t i = 0; i < CMD_LENGTH; ++i) {
    size_t current_index = HEADER_OFFSET + RES_LENGTH + i;

    uint8_t byte_value = telemetry_[current_index];
    if (byte_value == '\0') {
        break;
    }
    lastCommandGL860_.push_back(static_cast<char>(byte_value));
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

}