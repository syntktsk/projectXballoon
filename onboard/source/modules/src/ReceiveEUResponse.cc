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

  return anlnext::AS_OK;
}

// anlnext::ANLStatus ReceiveEUResponse::mod_initialize(){
// 	int ground_status = 0;
// 	socket_ = std::make_shared<SocketTransceiver>(serverIp_, port_);
// 	ground_status = socket_->initialize();
// 	if (ground_status != 0) {
// 		std::cerr << "Error in ReceiveEUResponse::mod_initialize: communication initialization failed." << std::endl;
// 		sendTelemetry_->getErrorManager()->setError(ErrorType::SEND_TELEMETRY_COMMUNICATION_ERROR); 
// 	}
// }

anlnext::ANLStatus ReceiveEUResponse::mod_initialize() {
  const std::string target = "SendTelemetry";
  if (exist_module(target)) {
    get_module_NC(target, &sendTelemetry_);
  }
  // eu_ = std::make_shared<SocketTransceiver>(serverIp_, port_);
  // int status = eu_->initialize();
  
  // if (status != 0) {
  //   std::cerr << "Error in ReceiveEUResponse: communication init failed." << std::endl;
  //   if (getErrorManager()) {
  //     getErrorManager()->setError(ErrorType::SEND_TELEMETRY_COMMUNICATION_ERROR); 
  //   }
  //   return anlnext::AS_ERROR;
  // }
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

// void ReceiveEUResponse::setSocket(std::string& ip, uint16_t port) {
//     serverIp_ = ip;
//     port_ = port;
// }

int ReceiveEUResponse::interpretStruct(const  std::vector<uint8_t>& bynary_struct){
	size_t received_size = bynary_struct.size();
  int id = ID_NONE;
	// A. サイズによる識別（最優先）
  if (received_size == sizeof(ess9)) {
    ess9 all_status;
    // バッファから構造体へコピー
    memcpy(&all_status, bynary_struct.data(), sizeof(ess9));
    std::cout << "-> Identified ESS Telemetry (ID: " << ID_Whole_TELEMETRY << ") " << std::endl;
    inputGNSS(all_status.gnss);
    inputElmo(all_status.elmo);
    id = ID_Whole_TELEMETRY; 
  }else if(received_size == sizeof(ess1)){
    ess1 elmo_status;
    // バッファから構造体へコピー
    memcpy(&elmo_status, bynary_struct.data(), sizeof(ess1));
    std::cout << "-> Identified ESS Telemetry (ID: " << ID_Elmo_TELEMETRY << ") " << std::endl;
    inputElmo(elmo_status);
    id = ID_Elmo_TELEMETRY; 
  }else if (received_size == sizeof(ess2)){
    ess2 gnss_status;
    // バッファから構造体へコピー
    memcpy(&gnss_status, bynary_struct.data(), sizeof(ess2));
    std::cout << "-> Identified ESS Telemetry (ID: " << ID_GNSS_TELEMETRY << ") " << std::endl;
    inputGNSS(gnss_status);
    id = ID_GNSS_TELEMETRY;
  } else if (received_size < sizeof(ess3)) {    
    ess3 option;
    // バッファから構造体へコピー
    memcpy(&option, bynary_struct.data(), sizeof(ess3));
    std::cout << "-> Identified ER Response (ID: " << ID_ER_RESPONSE << ") " << std::endl;
    inputOption(option);
    id = ID_ER_RESPONSE ; 
  }else{
    std::cerr << "Unrecognized packet received (Size: " << received_size << " bytes)." << std::endl;
    return ID_NONE; 
  }

  if (id != ID_NONE) {
      hasNewData_ = true;
      newDataID_ = id;
  }
  return id;
}

void ReceiveEUResponse::inputGNSS(const ess2& gnss_status){
// eu_struct.hhに従って構造体の値を各Private引数に振り分ける。2つ目の構造体
  la_ = gnss_status.la; 
  lo_ = gnss_status.lo;
  he_ = gnss_status.he;
  ya_ = gnss_status.ya;
  pi_ = gnss_status.pi;
  ro_ = gnss_status.ro;
  te_ = gnss_status.te;

}
void ReceiveEUResponse::inputElmo(const ess1& elmo_status){
  MO_ = elmo_status.MO;
  UM_ = elmo_status.UM;
  MF_ = elmo_status.MF;
  EC_ = elmo_status.EC;
  PX_ = elmo_status.PX;
  VX_ = elmo_status.VX;
  IQ_ = elmo_status.IQ;
  ID_ = elmo_status.ID;
  MC_ = elmo_status.MC;
  BV_ = elmo_status.BV;
  TI_ = elmo_status.TI;
  TC_ = elmo_status.TC;
  JV_ = elmo_status.JV;
  PA_ = elmo_status.PA;
  PR_ = elmo_status.PR;
  ac_ = elmo_status.ac;
  ef_ = elmo_status.ef;
  ps_ = elmo_status.ps;
  elc_= elmo_status.lc;
  std::cout << "LastCommandID:"<<elc_ << std::endl;
  // eu_struct.hhに従って構造体の値を各Private引数に振り分ける。1つ目の構造体
  // SendTelemetry* sender = SendTelemetry::singleton_self();
  if (MF_ == 0) {
    std::cout << "MotorFault is None" << std::endl;
  }else if (MF_ == 0x01) {
    std::cerr << "Error in Motor::MF=0x01 signal of encoder is unusual." << std::endl;
    sendTelemetry_->getErrorManager()->setError(ErrorType::MOTOR_ENCODER_ERROR);
  }else if (MF_ == 0x02) {
    std::cerr << "Error in Motor::MF=0x02 fault of comutation." << std::endl;
    sendTelemetry_->getErrorManager()->setError(ErrorType::MOTOR_COMUTATION_FAULT);
  }else if (MF_ == 0x04) {
    std::cerr << "Error in Motor::MF=0x04 signal of hall is illigal." << std::endl;
    sendTelemetry_->getErrorManager()->setError(ErrorType::MOTOR_ILLEGAL_HALL);
  }else if (MF_ == 0x08) {
    std::cerr << "Error in Motor::MF=0x08 current is over the limit." << std::endl;
    sendTelemetry_->getErrorManager()->setError(ErrorType::MOTOR_OVER_CURRENT);
  }

  if (EC_ == 0) {
    // ECに関連するエラービットを個別にクリアしていく
    sendTelemetry_->getErrorManager()->clearError(ErrorType::EC_UNKNOWN_COMMAND);
    sendTelemetry_->getErrorManager()->clearError(ErrorType::EC_INDEX_OUTOFRANGE);
    sendTelemetry_->getErrorManager()->clearError(ErrorType::EC_PROGRAM_NOT_RUNNING);
    sendTelemetry_->getErrorManager()->clearError(ErrorType::EC_RESERVE);
    std::cout << "ERROR is None" << std::endl;;
  }else if(EC_ ==2 ){
    std::cerr << "Error in Elmo:: you send unknown command." << std::endl;
    sendTelemetry_->getErrorManager()->setError(ErrorType::EC_UNKNOWN_COMMAND);
  }else if(EC_ == 3){
    std::cerr << "Error in ::you send index out of range." << std::endl;
    sendTelemetry_->getErrorManager()->setError(ErrorType::EC_INDEX_OUTOFRANGE);
  }else if(EC_ ==6 ){
    std::cerr << "Error in ::you accessed illigal program." << std::endl;
    sendTelemetry_->getErrorManager()->setError(ErrorType::EC_PROGRAM_NOT_RUNNING);
  }else if(EC_ == 1 || EC_ == 5){
    std::cerr << "Error in ::Please refer to the manual." << std::endl;
    sendTelemetry_->getErrorManager()->setError(ErrorType::EC_RESERVE);
  }else{
    std::cout << "EC_ is other value: " << (int)EC_ << std::endl;
  }
}
void ReceiveEUResponse::inputOption(const ess3& option){
  er_ = option.er;
}

}/*namespace balloon*/