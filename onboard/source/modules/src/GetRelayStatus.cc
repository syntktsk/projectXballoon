#include "GetRelayStatus.hh"
#include <sys/vfs.h>
#include <fstream>
#include <iostream>
#include <cstdlib> // rand用
#include <ctime>   // time用

using namespace anlnext;

namespace balloon {

GetRelayStatus::GetRelayStatus()
{
  tempFile_ = "/sys/class/thermal/thermal_zone0/temp";
  path_ = "/";
}

GetRelayStatus::~GetRelayStatus() = default;

ANLStatus GetRelayStatus::mod_define()
{
  define_parameter("temperature_filename", &mod_class::tempFile_);
  define_parameter("path", &mod_class::path_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus GetRelayStatus::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  return AS_OK;
}

ANLStatus GetRelayStatus::mod_analyze()
{
  std::srand(std::time(nullptr));
  dummy_ = std::rand() % 100;
  return AS_OK;
}


ANLStatus GetRelayStatus::mod_finalize()
{
  return AS_OK;
}

} /* namespace balloon */
