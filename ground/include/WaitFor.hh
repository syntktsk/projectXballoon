/**
 * MyModule sample module of the ANL Next framework
 *
 * @author 
 * @date
 *
 */

#ifndef WaitFor_H
#define WaitFor_H 1

#include <anlnext/BasicModule.hh>

namespace balloon
{

class WaitFor : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(WaitFor, 2.0);
public:
  WaitFor();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  int wait_;
};

} /* namespace balloon */

#endif /* WaitFor_H */
