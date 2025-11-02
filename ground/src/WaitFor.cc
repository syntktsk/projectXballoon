#include "WaitFor.hh"
#include "unistd.h"

using namespace anlnext;

namespace balloon {

WaitFor::WaitFor()
  : wait_(5000000)//0.5s
{
}

ANLStatus WaitFor::mod_define()
{
  define_parameter("time", &mod_class::wait_);
  return AS_OK;
}

ANLStatus WaitFor::mod_analyze()
{
  usleep(wait_);
  return AS_OK;
}

} /* namespace balloon */
