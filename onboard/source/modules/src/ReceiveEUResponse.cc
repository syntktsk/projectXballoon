#include "ReceiveEUResponse.hh"
#include "eu_struct.hh"
#include "SendTelemetry.hh" // エラーマネージャー呼び出しに必要
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring> // memcpy, memset
#include <stdexcept> // 例外処理用
#include <anlnext/ANLManager.hh>

namespace balloon {
constexpr int ID_NONE = 255;
constexpr int ID_Whole_TELEMETRY = 9;
constexpr int ID_HK_TELEMETRY = 1;
constexpr int ID_GNSS_TELEMETRY = 2;
constexpr int ID_Elmo_TELEMETRY = 3; // "ess" データ
constexpr int ID_Relay_TELEMETRY = 4;
constexpr int ID_ER_RESPONSE = 5;   // "er" データ
constexpr int MAX_BUFFER_SIZE = 1500; // UDPパケットの最大サイズに合わせて適宜設定
static const long RECEIVE_TIMEOUT_MS = 500;

ErrorManager* ReceiveEUResponse::getErrorManager() {
  if (sendTelemetry_ != nullptr) {
    return sendTelemetry_->getErrorManager();
  }
  return nullptr;
}

ReceiveEUResponse::ReceiveEUResponse()=default;
ReceiveEUResponse::~ReceiveEUResponse()=default;

anlnext::ANLStatus ReceiveEUResponse::mod_define()
{
  define_parameter("filepath", &mod_class::filepath_);
  return anlnext::AS_OK;
}


anlnext::ANLStatus ReceiveEUResponse::mod_initialize() {
  const std::string target = "SendTelemetry";
  if (exist_module(target)) {
    get_module_NC(target, &sendTelemetry_);
  }
  return anlnext::AS_OK;
}

anlnext::ANLStatus ReceiveEUResponse::mod_analyze() {
    if (eu_ == nullptr) return anlnext::AS_OK;

    while (true) {
        int byte_read = eu_->receiveBinary(); 
        if (byte_read <= 0) break; 
        this->interpretStruct(eu_->last_received_raw_data());
        std::cout << "ReceiveEUResponse: Data updated (" << byte_read << " bytes)." << std::endl;
    }
    return anlnext::AS_OK;
}


anlnext::ANLStatus ReceiveEUResponse::mod_finalize()
{
  return anlnext::AS_OK;
}


int ReceiveEUResponse::interpretStruct(const  std::vector<uint8_t>& bynary_struct){
	size_t received_size = bynary_struct.size();
  int id = ID_NONE;
  size_t WholeSize = sizeof(ess9);
  size_t ElmoSize = sizeof(ess1);
  size_t GNSSSize = sizeof(ess2);
  size_t OptionSize = sizeof(ess3);
	// A. サイズによる識別（最優先）
  if (received_size == WholeSize) {
    // バッファから構造体へコピー
    memcpy(&last_all_status_, bynary_struct.data(), WholeSize);
    last_gnss_status_ = last_all_status_.gnss;
    last_elmo_status_ = last_all_status_.elmo;
    std::cout << "-> Identified ESS Telemetry (ID: " << ID_Whole_TELEMETRY << ") " << std::endl;
    id = ID_Whole_TELEMETRY; 
  }else if(received_size == ElmoSize){
    // バッファから構造体へコピー
    memcpy(&last_elmo_status_, bynary_struct.data(), ElmoSize);
    std::cout << "-> Identified ESS Telemetry (ID: " << ID_Elmo_TELEMETRY << ") " << std::endl;
    id = ID_Elmo_TELEMETRY; 
  }else if (received_size == GNSSSize){
    // バッファから構造体へコピー
    memcpy(&last_gnss_status_, bynary_struct.data(), GNSSSize);
    std::cout << "-> Identified ESS Telemetry (ID: " << ID_GNSS_TELEMETRY << ") " << std::endl;
    id = ID_GNSS_TELEMETRY;
  } else if (received_size < OptionSize) {    
    // バッファから構造体へコピー
    memcpy(&last_option_, bynary_struct.data(), OptionSize);
    std::cout << "-> Identified ER Response (ID: " << ID_ER_RESPONSE << ") " << std::endl;
    er_ = last_option_.er;
    id = ID_ER_RESPONSE ; 
  }else{
    std::cerr << "Unrecognized packet received (Size: " << received_size << " bytes)." << std::endl;
    return ID_NONE; 
  }

  if (id == ID_ER_RESPONSE && id != ID_NONE) {
      hasNewData_ = true;
      newDataID_ = id;
  }
  return id;
}

// void ReceiveEUResponse::inputGNSS(const ess2& gnss_status){
//   // eu_struct.hhに従って構造体の値を各Private引数に振り分ける。2つ目の構造体
//   la_ = gnss_status.la; 
//   lo_ = gnss_status.lo;
//   he_ = gnss_status.he;
//   ya_ = gnss_status.ya;
//   pi_ = gnss_status.pi;
//   ro_ = gnss_status.ro;
//   te_ = gnss_status.te;

// }
// void ReceiveEUResponse::inputElmo(const ess1& elmo_status){
//   MO_ = elmo_status.MO;
//   UM_ = elmo_status.UM;
//   MF_ = elmo_status.MF;
//   EC_ = elmo_status.EC;
//   PX_ = elmo_status.PX;
//   VX_ = elmo_status.VX;
//   IQ_ = elmo_status.IQ;
//   ID_ = elmo_status.ID;
//   MC_ = elmo_status.MC;
//   BV_ = elmo_status.BV;
//   TI_ = elmo_status.TI;
//   TC_ = elmo_status.TC;
//   JV_ = elmo_status.JV;
//   PA_ = elmo_status.PA;
//   PR_ = elmo_status.PR;
//   ac_ = elmo_status.ac;
//   ef_ = elmo_status.ef;
//   ps_ = elmo_status.ps;
//   // elc_= elmo_status.lc;
//   std::cout << "LastCommandID:"<<elc_ << std::endl;
//   // eu_struct.hhに従って構造体の値を各Private引数に振り分ける。1つ目の構造体
//   // SendTelemetry* sender = SendTelemetry::singleton_self();
//   if (MF_ == 0) {
//     std::cout << "ReceiveEURes:MotorFault is None" << std::endl;
//   }else if (MF_ == 0x01) {
//     std::cerr << "Error in Motor::MF=0x01 signal of encoder is unusual." << std::endl;
//     sendTelemetry_->getErrorManager()->setError(ErrorType::MOTOR_ENCODER_ERROR);
//   }else if (MF_ == 0x02) {
//     std::cerr << "Error in Motor::MF=0x02 fault of comutation." << std::endl;
//     sendTelemetry_->getErrorManager()->setError(ErrorType::MOTOR_COMUTATION_FAULT);
//   }else if (MF_ == 0x04) {
//     std::cerr << "Error in Motor::MF=0x04 signal of hall is illigal." << std::endl;
//     sendTelemetry_->getErrorManager()->setError(ErrorType::MOTOR_ILLEGAL_HALL);
//   }else if (MF_ == 0x08) {
//     std::cerr << "Error in Motor::MF=0x08 current is over the limit." << std::endl;
//     sendTelemetry_->getErrorManager()->setError(ErrorType::MOTOR_OVER_CURRENT);
//   }

//   if (EC_ == 0) {
//     // ECに関連するエラービットを個別にクリアしていく
//     sendTelemetry_->getErrorManager()->clearError(ErrorType::EC_UNKNOWN_COMMAND);
//     sendTelemetry_->getErrorManager()->clearError(ErrorType::EC_INDEX_OUTOFRANGE);
//     sendTelemetry_->getErrorManager()->clearError(ErrorType::EC_PROGRAM_NOT_RUNNING);
//     sendTelemetry_->getErrorManager()->clearError(ErrorType::EC_RESERVE);
//     std::cout << "ERROR is None" << std::endl;;
//   }else if(EC_ ==2 ){
//     std::cerr << "Error in Elmo:: you send unknown command." << std::endl;
//     sendTelemetry_->getErrorManager()->setError(ErrorType::EC_UNKNOWN_COMMAND);
//   }else if(EC_ == 3){
//     std::cerr << "Error in ::you send index out of range." << std::endl;
//     sendTelemetry_->getErrorManager()->setError(ErrorType::EC_INDEX_OUTOFRANGE);
//   }else if(EC_ ==6 ){
//     std::cerr << "Error in ::you accessed illigal program." << std::endl;
//     sendTelemetry_->getErrorManager()->setError(ErrorType::EC_PROGRAM_NOT_RUNNING);
//   }else if(EC_ == 1 || EC_ == 5){
//     std::cerr << "Error in ::Please refer to the manual." << std::endl;
//     sendTelemetry_->getErrorManager()->setError(ErrorType::EC_RESERVE);
//   }else{
//     std::cout << "EC_ is other value: " << (int)EC_ << std::endl;
//   }}
// void ReceiveEUResponse::inputOption(const ess3& option){
  
// }
bool reserveData(std::string name){
  // 構造体のサイズから判別
  std::string type_of="";
  // 構造体をばらしてString型にする？
  // 保存
  return true;
}
}/*namespace balloon*/