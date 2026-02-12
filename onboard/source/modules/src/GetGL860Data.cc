#include "GetGL860Data.hh"
#include "GL860main.hh"
#include <sys/vfs.h>
#include <fstream>

using namespace anlnext;

namespace balloon {

GetGL860Data::GetGL860Data()
  : DataID_(0),
    currentFileID_(0),
    lineCount_(0)
    
{
  GL860main_= std::make_shared<GL860main>();
}

GetGL860Data::~GetGL860Data()= default;

enum GL860_SPECIAL_VALUE {
  PLUS_OVERFLOW  = 0x7fff, // 演算エラーデータ
  MEASUREMENT_OFF = 0x7ffe, // メジャメントOffデータ
  BURNOUT         = 0x7ffd, // バーンアウトデータ（断線）
  PLUS_FS_DATA    = 0x7ffc, // プラスFS（フルスケール）データ
  MINUS_FS_DATA   = -0x7fff // マイナスFS（フルスケール）データ
};

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}

ANLStatus GetGL860Data::mod_define()
{
  define_parameter("gl860_ip", &mod_class::ip_);
  define_parameter("gl860_port", &mod_class::port_);
  define_parameter("temperature_filename", &mod_class::tempFile_);
  define_parameter("path", &mod_class::path_);
  define_parameter("num_per_file", &mod_class::numPerFile_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus GetGL860Data::mod_initialize()
{
  // std::cout << "DEBUG: GL860 initialize Start" << std::endl;
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }
  // std::cout << "DEBUG:ここか？" << std::endl;
  bool a = GL860main_->initialize(ip_, port_);
  (void)a;
  range_info_ = GL860main_->RangeInfo();
  DataID_ = 0;
  // std::cout << "DEBUG: GL860 initialize Finish" << std::endl;
  return AS_OK;
}

ANLStatus GetGL860Data::mod_analyze()
{ 
  std::cout << "DEBUG: GL860 analyze Start" << std::endl;
  DataID_++;
  RawData_ = GL860main_->sendAndReceive(":MEAS:OUTP:ONECSV?");
  // RawData_ = GL860main_->RawData();
  // std::cout << "DEBUG: これか" << std::endl;
  interpretGL860(RawData_); 
  // std::cout << "DEBUG: これか" << std::endl;
  writeDataToFile();
  // std::cout << "DEBUG: これか" << std::endl;
  this->latestAnalogData_ = getGL860Vec_;
  // std::cout << "DEBUG: これか" << std::endl;
  return AS_OK;
}


ANLStatus GetGL860Data::mod_finalize()
{
  // std::cout << "DEBUG: GL860 Finalize Start" << std::endl;
  return AS_OK;
}

int GetGL860Data::getCapacity()
{
  struct statfs64 capacity;
  int rslt = statfs64(path_.c_str(), &capacity);
  if (rslt < 0) {
    std::cerr << "failed in get_freesize(). Code: " << rslt << std::endl;
    return -1;
  }
  capacityFree_ = capacity.f_bfree * capacity.f_bsize;
  capacityAll_ = capacity.f_blocks * capacity.f_bsize;
  return 0;
}

void GetGL860Data::interpretGL860(const std::string& data){
  // std::cout << "DEBUG: start interpretGL860" << std::endl;
  std::vector<std::string> analog_vec;
  // std::cout << "DEBUG: これか" << std::endl;
  getGL860Vec_.clear();
  analog_vec.clear();
  analog_vec = split(data , ',');
  std::cout << "DEBUG: analog_vec is "<< analog_vec.size() << std::endl;

  if (analog_vec.empty()) {
    std::cout << "DEBUG: analog_vec is empty, skipping..." << std::endl;
    return; 
  }
  uint8_t logic = 0;
  int ch_count = 0;
  // std::cout << "DEBUG: これか" << std::endl;
  for (const auto& item : analog_vec){
    if (item.empty()) continue;
    // std::cout << "DEBUG: これか" << std::endl;
    if (item.find('/') != std::string::npos || item.find(':') != std::string::npos) {
      // ファイル名に使えない文字 (/, :, スペース) を除去・置換する
      std::string clean_time = item;
      for (char &c : clean_time) {
        if (c == '/' || c == ':' || c == ' ') c = '_';
      }
      glTimeStr_ = clean_time;
      continue;
    }
    // std::cout << "DEBUG: これか" << std::endl;
    if (item.find_first_of("+-0123456789") != std::string::npos){
      float val = 0.0f;
      try {
        val = std::stof(item);
      }catch(...){
        continue;
      }
      float m = 1.0f;
      if (ch_count * 2 + 1 < (int)range_info_.size()) {
        uint8_t cid = range_info_[ch_count * 2 + 1]; // 安全な内側で取る
        for (const auto& info : RANGE_TABLE) {
          if (info.range_id == cid) {
            m = info.multiplier;
            break;
          }
        }
      } else {
      }
      // std::cout << "DEBUG: これか" << std::endl;
      getGL860Vec_ .push_back(static_cast<int16_t>(val * m));
      ch_count++;
      // std::cout << "DEBUG: これか" << std::endl;
    }else if (item[0] == 'L' || item[0] == 'H') {
      break;
    }
  }
  // std::cout << "DEBUG: Loop 抜けてる　これか" << std::endl;
  std::string alarm_out_str = analog_vec.back(); 
  uint8_t alarm_out_packed = 0;
  for (size_t i = 0; i < alarm_out_str.length() && i < 4; ++i) {
    if (alarm_out_str[i] == 'H') {
      alarm_out_packed |= (1 << i);
    }
  }
  // std::cout << "DEBUG: これか" << std::endl;
  getGL860Vec_.push_back(static_cast<int16_t>(alarm_out_packed));
  // std::cout << "DEBUG: これか" << std::endl;
  getGL860Vec_.insert(getGL860Vec_ .begin(), static_cast<int16_t>(ch_count));
  // std::cout << "DEBUG: interpretGL860 finished" << std::endl;
}

void GetGL860Data::writeDataToFile(){
  if (lineCount_ >= numPerFile_) {
    currentFileID_++;
    lineCount_ = 0;
  }
  std::string time_stamp = glTimeStr_.empty() ? "NoTime" : glTimeStr_;
  std::ostringstream run_id_sout, file_id_sout;
  run_id_sout << std::setfill('0') << std::right << std::setw(6) << DataID_;
  file_id_sout << std::setfill('0') << std::right << std::setw(6) << currentFileID_;
  const std::string filename = path_ + "/GL860_" + run_id_sout.str() + "_"+ time_stamp + "_id" + file_id_sout.str() + ".csv";
  std::ofstream ofs(filename, std::ios::app);
  if (ofs) {
    ofs << RawData_ << "\n";
    lineCount_++; // 書き込んだらカウントアップ
  }  
}
} /* namespace balloon */




