#ifndef QLForProjectX_H
#define QLForProjectX_H 1

#include <anlnext/BasicModule.hh>
#include "SocketTransceiver.hh"

namespace balloon {

class MongoDBClient;

class QLForProjectX : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(QLForProjectX, 3.0);
public:
  QLForProjectX();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  MongoDBClient* mongodb_client_ = nullptr;
  SocketTransceiver transceiver_;
};

} /* namespace balloon */

#endif // QLForProjectX_H
