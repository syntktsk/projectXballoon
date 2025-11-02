#include "QLForProjectX.hh"

#include <cstdint>
#include <cmath>
#include <ctime>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "MongoDBClient.hh"
#include "DocumentBuilder.hh"

#include "TelemetryDefinition.hh"
#include "SocketTransceiver.hh"

using namespace anlnext;

namespace balloon {

QLForProjectX::QLForProjectX()
{
}

ANLStatus QLForProjectX::mod_define()
{
  return AS_OK;
}

ANLStatus QLForProjectX::mod_initialize()
{
  get_module_NC("MongoDBClient", &mongodb_client_);
  mongodb_client_->createCappedCollection("main", 1*1024*1024);

  return AS_OK;
}

ANLStatus QLForProjectX::mod_analyze()
{
  TelemetryDefinition telemetry;

  std::vector<uint8_t> binary_data = transceiver_.get_binary_data();
  telemetry.setTelemetry(binary_data);

  DocumentBuilder builder("GNSS", "Status");
  time_t t(0); time(&t);
  const int64_t ti = static_cast<int64_t>(t)*64;
  builder.setTI(ti);
  builder.setTimeNow();

  {
    const std::string section_name = "GNSSData";
    auto section = bsoncxx::builder::stream::document{}
      << "SeqNo" << telemetry.SeqNo()
      << "Ypr" << bsoncxx::builder::stream::open_array
        << telemetry.ypr()[0] << telemetry.ypr()[1] << telemetry.ypr()[2]
      << bsoncxx::builder::stream::close_array
      << "Angrate" << bsoncxx::builder::stream::open_array
        << telemetry.Angrate()[0] << telemetry.Angrate()[1] << telemetry.Angrate()[2]
      << bsoncxx::builder::stream::close_array
      << "PosLla" << bsoncxx::builder::stream::open_array
        << telemetry.PosLla()[0] << telemetry.PosLla()[1] << telemetry.PosLla()[2]
      << bsoncxx::builder::stream::close_array
      << "Velocity" << bsoncxx::builder::stream::open_array
        << telemetry.GNSSVelocity()[0] << telemetry.GNSSVelocity()[1] << telemetry.GNSSVelocity()[2]
      << bsoncxx::builder::stream::close_array
      << "InsStatus" << telemetry.InsStatus()
      << "GNSStemp" << telemetry.GNSStemp()
      << "GNSSpres" << telemetry.GNSSpres()
      << "IMUsensSat" << telemetry.IMUsensSat()
      << "GNSSnumSats" << telemetry.GNSSnumSats()
      << "CurrentUTC" << bsoncxx::builder::stream::document{}
        << "year" << static_cast<int>(telemetry.currutc().year)
        << "month" << static_cast<int>(telemetry.currutc().month)
        << "day" << static_cast<int>(telemetry.currutc().day)
        << "hour" << static_cast<int>(telemetry.currutc().hour)
        << "minute" << static_cast<int>(telemetry.currutc().minute)
        << "second" << static_cast<int>(telemetry.currutc().second)
        << "fracsec" << static_cast<int>(telemetry.currutc().fracsec)
      << bsoncxx::builder::stream::finalize;
      
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodb_client_->push("gnss_status", doc);

  return AS_OK;
}

} /* namespace balloon */
