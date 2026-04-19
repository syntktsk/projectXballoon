#include "RelayControl.hh"
#include "pigpiod_if2.h"
#include <anlnext/ANLManager.hh>
#include <anlnext/BasicModule.hh>
#include <iostream>
using namespace anlnext;

namespace balloon{

RelayControl::RelayControl(){}

ANLStatus RelayControl::mod_define(){
  define_parameter("gpio_list", &mod_class::chipSelectArray_);
  return AS_OK;
}

ANLStatus RelayControl::mod_initialize(){
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  pi = pigpio_start(NULL, portNumber_.c_str());
  if (pi<0) {
    std::cerr << "pigpio daemon connection failed: pi =  " << pi << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::PIGPIO_START_ERROR);
    }
  }
  GPIOHandler_ = pi;
  chipSelect_ = 8;

  const int n = chipSelectArray_.size();
  for (int i=0; i<n; i++) {
    set_mode(pi, chipSelectArray_[i], PI_OUTPUT);
    set_pull_up_down(pi, chipSelectArray_[i], PI_PUD_UP);
    gpio_write(pi, chipSelectArray_[i], RELAY_OFF);/*High->流れる方ぽい*/
	int num = chipSelectArray_[i];
	if (num >= 0 && num <= 31) {
        relays_mask_ |= (1 << num);
    }
  }
  return AS_OK;
}

ANLStatus RelayControl::mod_analyze(){
	ReadRelayStatus();
	return AS_OK;
}

ANLStatus RelayControl::mod_finalize(){
  pigpio_stop(pi);
  return AS_OK;
}

void RelayControl::addChipSelect(int v)
{(void)v;}

void RelayControl::ReadRelayStatus(){
	if (pi >= 0) {
    uint32_t status = read_bank_1(pi);
    latestStatus_ = status;
  }else{
    std::cerr << "RelayControl: pigpio err" << std::endl;
  }
}
void RelayControl::AllOff() {
	if (pi >= 0 && relays_mask_ != 0) {
		clear_bank_1(pi, relays_mask_);
		std::cout << "RelayControl: All registered relays turned off." << std::endl;
	} else if (relays_mask_ == 0) {
		std::cout << "RelayControl: No relays registered in mask. Skipping." << std::endl;
	}
}

void RelayControl::RelayOn(int gpio){
  if (std::find(chipSelectArray_.begin(), chipSelectArray_.end(), gpio) == chipSelectArray_.end()) {
    std::cerr << "RelayControl: GPIO " << gpio << "is not contained gpio list." << std::endl;
    return;
  }	
  if (gpio >= 0 && pi >= 0){
	int result = gpio_write(pi, gpio, RELAY_ON);
	if (result != 0) {std::cerr << "RelayControl: false to change Relay, result = "<< result <<std::endl;
	}else{std::cout << "RelayControl: gpio "<< gpio << " was turned on." <<std::endl;}
  }	else {std::cerr << "RelayControl: pigpio status or gpio number is false." <<std::endl;}
}

void RelayControl::RelayOff(int gpio){
  if (std::find(chipSelectArray_.begin(), chipSelectArray_.end(), gpio) == chipSelectArray_.end()) {
    std::cerr << "RelayControl: GPIO " << gpio << " is not contained gpio list." << std::endl;
    return;
  }
  if (gpio >= 0 && pi >= 0){
	int result = gpio_write(pi, gpio, RELAY_OFF);
	if (result != 0) {std::cerr << "RelayControl: false to change Relay, result = "<< result <<std::endl;
	}else{std::cout << "RelayControl: gpio "<< gpio << " was turned off." <<std::endl;}
  }	else {std::cerr << "RelayControl: pigpio status or gpio number is false." <<std::endl;}
}

}/*namespace balloon*/