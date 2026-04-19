#include "PushToMongoDB.hh"

#include <cstdint>
#include <string>
#include <boost/format.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <hsquicklook/DocumentBuilder.hh>
#include "ErrorManager.hh"

using namespace anlnext;

namespace balloon {

PushToMongoDB::PushToMongoDB()
{
}

ANLStatus PushToMongoDB::mod_define()
{
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus PushToMongoDB::mod_initialize()
{
  get_module_NC("MongoDBClient", &mongodbClient_);
  mongodbClient_->createCappedCollection("BACS", 1*1024*1024);

  get_module_NC("InterpretTelemetry", &interpreter_);

  return AS_OK;
}

ANLStatus PushToMongoDB::mod_analyze()
{
  // std::cout <<"PushToMongo:kokomadekiterukaa~~~"<<interpreter_->CurrentTelemetryType()<<std::endl;
  if (interpreter_->CurrentTelemetryType()==9) {
    pushWholeTelemetry();
  }
  else if (interpreter_->CurrentTelemetryType()==1) {
    pushHKTelemetry();
  }
  else if (interpreter_->CurrentTelemetryType()==2) {
    pushGNSSTelemetry();
  }
  else if (interpreter_->CurrentTelemetryType()==3) {
    pushElmoTelemetry();
  }
  else if (interpreter_->CurrentTelemetryType()==4) {
    pushRelayTelemetry();
  }
  else if (interpreter_->CurrentTelemetryType()==5) {
    pushOptionalTelemetry();
  }
  else if (interpreter_->CurrentTelemetryType()==6) {
    pushGL860OptionalTelemetry();
  }else {
    return AS_OK;
  }
  return AS_OK;
}

void PushToMongoDB::pushWholeTelemetry()
{
  TelemetryDefinition* telemdef = interpreter_->Telemdef();

  hsquicklook::DocumentBuilder builder("Telemetry", "Whole");
  builder.setTI(telemdef->TimeNow().tv_sec*64 + telemdef->TimeNow().tv_usec*64*1E-6);
  builder.setTimeNow();
  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start_Code"      << static_cast<int64_t>(telemdef->StartCode())
      << "Telemetry_Type"  << static_cast<int>(telemdef->TelemetryType())
      << "Time"            << static_cast<int>((telemdef->TimeNow()).tv_sec)
      << "Time_us"         << static_cast<int>((telemdef->TimeNow()).tv_usec)
      << "Telemetry_Index" << static_cast<int>(telemdef->TelemetryIndex())
      << "Run_ID"          << static_cast<int>(telemdef->RunID())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Elmo_Status";
    auto section = bsoncxx::builder::stream::document{}
      << "Motor_OnOff"         << telemdef->MotorOnOff()
      << "Unit_Mode"            << telemdef->UnitMode()
      << "Brale_OnOff"            << telemdef->OnOffBrake()
      << "Motor_Fault"          << telemdef->MoterFault()
      << "Error_Code"           << telemdef->ErrorCode()
      << "Position_PX"          << telemdef->Position()
      << "Velocity_VX"          << telemdef->Velocity()
      << "I_Qaxis"              << telemdef->I_Qaxis()
      << "I_Daxis"              << telemdef->I_Daxis()
      << "Max_Current"          << telemdef->MaxCurrent()
      << "Bus_Voltage"          << telemdef->BusVoltage()
      << "Temperature_Info"     << telemdef->TemperatureInfomation()
      << "Torque_Command"       << telemdef->TorqueCommand()
      << "Jog_Velocity"         << telemdef->JogVelocity()
      << "Position_Absolute"    << telemdef->PositionAbusolute()
      << "Position_Relative"    << telemdef->PositionRelative()
      << "Mode_Flag"            << telemdef->modeflag()
      << "Enable_Flag"          << telemdef->enablefrag()
      << "Parameter_Set"        << telemdef->parameterset()
      << "en"                   << telemdef->en()
      << "az"                   << telemdef->az()
      << "hi"                   << telemdef->hi()
      << "last_command"         << telemdef->EUlastcommand()
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "GNSS_Status";
    auto section = bsoncxx::builder::stream::document{}
      << "Latitude"             << telemdef->latitude()
      << "Longitude"            << telemdef->longitude()
      << "Height"               << telemdef->height()
      << "Yaw"                  << telemdef->yaw()
      << "Pitch"                << telemdef->pitch()
      << "Roll"                 << telemdef->roll()
      << "Temperature"          << telemdef->temperature()
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  // {
  //   const std::string section_name = "Strings";
  //   auto section = bsoncxx::builder::stream::document{}
  //     << "StringsResponse"             << static_cast<std::string>(telemdef->OptionalStrings())
  //   << bsoncxx::builder::stream::finalize;
  //   builder.addSection(section_name, section);
  // }
  {
    const std::string section_name = "Software";
    auto section = bsoncxx::builder::stream::document{}
      << "Last_Command_Index"   << static_cast<int>(telemdef->LastCommandIndex())
      << "Last_Command_Code"    << static_cast<int>(telemdef->LastCommandCode())
      << "Command_Reject_Count" << static_cast<int>(telemdef->CommandRejectCount())
      << "Software_Error_Code"  << static_cast<int64_t>(telemdef->SoftwareErrorCode())
      << "CRC"                  << static_cast<int>(telemdef->CRC())
      << "Stop_Code"            << static_cast<int64_t>(telemdef->StopCode())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Temperature";
    auto section = bsoncxx::builder::stream::document{}
      << "Pivot_Temperature"        << telemdef->gl860ground()[0]
      << "StarCamera_Temperature"   << telemdef->gl860ground()[1]
      << "Mirror_Temperature"       << telemdef->gl860ground()[2]
      << "GNSS_Temperature_HK"      << telemdef->gl860ground()[3]
      << "Calculator_Temperature"   << telemdef->gl860ground()[4]
      << "Battery_Temperature"      << telemdef->gl860ground()[5]
      << "Gyro_Temperature"         << telemdef->gl860ground()[6]
      << "CMOS_Temperature"         << telemdef->gl860ground()[7]
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Voltage";
    auto section = bsoncxx::builder::stream::document{}
      << "PC_Voltage"               << telemdef->gl860ground()[8]
      << "StarCamera_Voltage"       << telemdef->gl860ground()[9]
      << "GNSS_Voltage"             << telemdef->gl860ground()[10]
      << "Gyro_Voltage"             << telemdef->gl860ground()[11]
      << "CMOS_Voltage"             << telemdef->gl860ground()[12]
      << "Router_Voltage"           << telemdef->gl860ground()[13]
      << "Heater_Voltage"           << telemdef->gl860ground()[14]
      << "Pi_HK_Voltage"            << telemdef->gl860ground()[15]
      << "Pivot_Voltage"            << telemdef->gl860ground()[16]
      << "Hub_Voltage"              << telemdef->gl860ground()[17]
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Logic";
    auto section = bsoncxx::builder::stream::document{}
      << "logic1"               << telemdef->gl860logic()[0]
      << "logic2"               << telemdef->gl860logic()[1]
      << "logic3"               << telemdef->gl860logic()[2]
      << "logic4"               << telemdef->gl860logic()[3]
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  { 
    const std::string section_name = "Software_Error";
    auto section_stream = bsoncxx::builder::stream::document{};
    uint64_t error_code = telemdef->SoftwareErrorCode();
    for (int i=0; i<64; i++) {
      const std::string error_name = ErrorManager::bitToStr(i);
      if (error_name=="") {
        continue;
      }
      if ((error_code>>i)&1) {
        section_stream << error_name << "Error";
      }
      else {
        section_stream << error_name << "OK";
      }
    }
    auto section = section_stream << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  auto doc = builder.generate();
  mongodbClient_->push("BACS", doc);
}

void PushToMongoDB::pushHKTelemetry()
{
  TelemetryDefinition* telemdef = interpreter_->Telemdef();
  if (telemdef == nullptr) {
    std::cout << "DEBUG: telemdef is NULL in pushWholeTelemetry!" << std::endl;
    return; // NULLならこれ以上進むと落ちるので、関数を抜ける
  }
  hsquicklook::DocumentBuilder builder("Telemetry", "HK");
  builder.setTI(telemdef->TimeNow().tv_sec*64 + telemdef->TimeNow().tv_usec*64*1E-6);
  builder.setTimeNow();

  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start_Code"      << static_cast<int64_t>(telemdef->StartCode())
      << "Telemetry_Type"  << static_cast<int>(telemdef->TelemetryType())
      << "Time"            << static_cast<int>((telemdef->TimeNow()).tv_sec)
      << "Time_us"         << static_cast<int>((telemdef->TimeNow()).tv_usec)
      << "Telemetry_Index" << static_cast<int>(telemdef->TelemetryIndex())
      << "Run_ID"          << static_cast<int>(telemdef->RunID())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "Software";
    auto section = bsoncxx::builder::stream::document{}
      << "Last_Command_Index"   << static_cast<int>(telemdef->LastCommandIndex())
      << "Last_Command_Code"    << static_cast<int>(telemdef->LastCommandCode())
      << "Command_Reject_Count" << static_cast<int>(telemdef->CommandRejectCount())
      << "Software_Error_Code"  << static_cast<int64_t>(telemdef->SoftwareErrorCode())
      << "CRC"                  << static_cast<int>(telemdef->CRC())
      << "Stop_Code"            << static_cast<int64_t>(telemdef->StopCode())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Temperature";
    auto section = bsoncxx::builder::stream::document{}
      << "Pivot_Temperature"        << telemdef->gl860ground()[0]
      << "StarCamera_Temperature"   << telemdef->gl860ground()[1]
      << "Mirror_Temperature"       << telemdef->gl860ground()[2]
      << "GNSS_Temperature_HK"      << telemdef->gl860ground()[3]
      << "Calculator_Temperature"   << telemdef->gl860ground()[4]
      << "Battery_Temperature"      << telemdef->gl860ground()[5]
      << "Gyro_Temperature"         << telemdef->gl860ground()[6]
      << "CMOS_Temperature"         << telemdef->gl860ground()[7]

      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Voltage";
    auto section = bsoncxx::builder::stream::document{}
      << "PC_Voltage"               << telemdef->gl860ground()[8]
      << "StarCamera_Voltage"       << telemdef->gl860ground()[9]
      << "GNSS_Voltage"             << telemdef->gl860ground()[10]
      << "Gyro_Voltage"             << telemdef->gl860ground()[11]
      << "CMOS_Voltage"             << telemdef->gl860ground()[12]
      << "Router_Voltage"           << telemdef->gl860ground()[13]
      << "Heater_Voltage"           << telemdef->gl860ground()[14]
      << "Pi_HK_Voltage"            << telemdef->gl860ground()[15]
      << "Pivot_Voltage"            << telemdef->gl860ground()[16]
      << "Hub_Voltage"              << telemdef->gl860ground()[17]
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Logic";
    auto section = bsoncxx::builder::stream::document{}
      << "logic1"               << telemdef->gl860logic()[0]
      << "logic2"               << telemdef->gl860logic()[1]
      << "logic3"               << telemdef->gl860logic()[2]
      << "logic4"               << telemdef->gl860logic()[3]
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Software_Error";
    auto section_stream = bsoncxx::builder::stream::document{};
    uint64_t error_code = telemdef->SoftwareErrorCode();
    for (int i=0; i<64; i++) {
      const std::string error_name = ErrorManager::bitToStr(i);
      if (error_name=="") {
        continue;
      }
      if ((error_code>>i)&1) {
        section_stream << error_name << "Error";
      }
      else {
        section_stream << error_name << "OK";
      }
    }
    auto section = section_stream << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodbClient_->push("BACS", doc);

}

void PushToMongoDB::pushGNSSTelemetry()
{
  TelemetryDefinition* telemdef = interpreter_->Telemdef();

  hsquicklook::DocumentBuilder builder("Telemetry", "GNSS");
  builder.setTI(telemdef->TimeNow().tv_sec*64 + telemdef->TimeNow().tv_usec*64*1E-6);
  builder.setTimeNow();

  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start_Code"      << static_cast<int64_t>(telemdef->StartCode())
      << "Telemetry_Type"  << static_cast<int>(telemdef->TelemetryType())
      << "Time"            << static_cast<int>((telemdef->TimeNow()).tv_sec)
      << "Time_us"         << static_cast<int>((telemdef->TimeNow()).tv_usec)
      << "Telemetry_Index" << static_cast<int>(telemdef->TelemetryIndex())
      << "Run_ID"          << static_cast<int>(telemdef->RunID())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "GNSS_Status";
    auto section = bsoncxx::builder::stream::document{}
      << "Latitude"             << telemdef->latitude()
      << "Longitude"            << telemdef->longitude()
      << "Height"               << telemdef->height()
      << "Yaw"                  << telemdef->yaw()
      << "Pitch"                << telemdef->pitch()
      << "Roll"                 << telemdef->roll()
      << "Temperature"          << telemdef->temperature()
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "Footer";
    auto section = bsoncxx::builder::stream::document{}
      << "CRC"                  << static_cast<int>(telemdef->CRC())
      << "Stop_Code"            << static_cast<int64_t>(telemdef->StopCode())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodbClient_->push("BACS", doc);

}

void PushToMongoDB::pushElmoTelemetry()
{
  TelemetryDefinition* telemdef = interpreter_->Telemdef();

  hsquicklook::DocumentBuilder builder("Telemetry", "Elmo");
  builder.setTI(telemdef->TimeNow().tv_sec*64 + telemdef->TimeNow().tv_usec*64*1E-6);
  builder.setTimeNow();

  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start_Code"      << static_cast<int64_t>(telemdef->StartCode())
      << "Telemetry_Type"  << static_cast<int>(telemdef->TelemetryType())
      << "Time"            << static_cast<int>((telemdef->TimeNow()).tv_sec)
      << "Time_us"         << static_cast<int>((telemdef->TimeNow()).tv_usec)
      << "Telemetry_Index" << static_cast<int>(telemdef->TelemetryIndex())
      << "Run_ID"          << static_cast<int>(telemdef->RunID())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "Elmo_Status";
    auto section = bsoncxx::builder::stream::document{}
      << "Motor_On/Off"         << telemdef->MotorOnOff()
      << "Unit_Mode"            << telemdef->UnitMode()
      << "Motor_Fault"          << telemdef->MoterFault()
      << "Error_Code"           << telemdef->ErrorCode()
      << "Position_PX"          << telemdef->Position()
      << "Velocity_VX"          << telemdef->Velocity()
      << "I_Qaxis"              << telemdef->I_Qaxis()
      << "I_Daxis"              << telemdef->I_Daxis()
      << "Max_Current"          << telemdef->MaxCurrent()
      << "Bus_Voltage"          << telemdef->BusVoltage()
      << "Temperature_Info"     << telemdef->TemperatureInfomation()
      << "Torque_Command"       << telemdef->TorqueCommand()
      << "Jog_Velocity"         << telemdef->JogVelocity()
      << "Position_Absolute"    << telemdef->PositionAbusolute()
      << "Position_Relative"    << telemdef->PositionRelative()
      << "Mode_Flag"            << telemdef->modeflag()
      << "Enable_Flag"          << telemdef->enablefrag()
      << "Parameter_Set"        << telemdef->parameterset()
      << "Last_Command_Code"    << telemdef->EUlastcommand()
      // int elc_ = telemdef->EUlastcommand()
      // std::string lc_string = "Some command"
      // int lc_value=0000;
      // << "Last_Command_name"         << lc_string
      // << "Last_Command_Value"        << lc_value
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "Footer";
    auto section = bsoncxx::builder::stream::document{}
      << "CRC"                  << static_cast<int>(telemdef->CRC())
      << "Stop_Code"            << static_cast<int64_t>(telemdef->StopCode())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodbClient_->push("BACS", doc);

}

void PushToMongoDB::pushRelayTelemetry()
{
  TelemetryDefinition* telemdef = interpreter_->Telemdef();

  hsquicklook::DocumentBuilder builder("Telemetry", "Relay");
  builder.setTI(telemdef->TimeNow().tv_sec*64 + telemdef->TimeNow().tv_usec*64*1E-6);
  builder.setTimeNow();

  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start_Code"      << static_cast<int64_t>(telemdef->StartCode())
      << "Telemetry_Type"  << static_cast<int>(telemdef->TelemetryType())
      << "Time"            << static_cast<int>((telemdef->TimeNow()).tv_sec)
      << "Time_us"         << static_cast<int>((telemdef->TimeNow()).tv_usec)
      << "Telemetry_Index" << static_cast<int>(telemdef->TelemetryIndex())
      << "Run_ID"          << static_cast<int>(telemdef->RunID())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Relay";
    auto section = bsoncxx::builder::stream::document{}
      << "GPIO_22" << (telemdef->RelaysStatus()[22])
      << "GPIO_23" << (telemdef->RelaysStatus()[23])
      << "GPIO_24" << (telemdef->RelaysStatus()[24])
      // 必要なやつだけ抜き出しで
    << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Footer";
    auto section = bsoncxx::builder::stream::document{}
      << "CRC"                  << static_cast<int>(telemdef->CRC())
      << "Stop_Code"            << static_cast<int64_t>(telemdef->StopCode())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodbClient_->push("BACS", doc);

}  

void PushToMongoDB::pushOptionalTelemetry()
{
  TelemetryDefinition* telemdef = interpreter_->Telemdef();

  hsquicklook::DocumentBuilder builder("Telemetry", "GPIO");
  builder.setTI(telemdef->TimeNow().tv_sec*64 + telemdef->TimeNow().tv_usec*64*1E-6);
  builder.setTimeNow();

  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start_Code"      << static_cast<int64_t>(telemdef->StartCode())
      << "Telemetry_Type"  << static_cast<int>(telemdef->TelemetryType())
      << "Time"            << static_cast<int>((telemdef->TimeNow()).tv_sec)
      << "Time_us"         << static_cast<int>((telemdef->TimeNow()).tv_usec)
      << "Telemetry_Index" << static_cast<int>(telemdef->TelemetryIndex())
      << "Run_ID"          << static_cast<int>(telemdef->RunID())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Status";
    auto section = bsoncxx::builder::stream::document{}
      << "er ="             << static_cast<std::string>(telemdef->OptionalStrings())
    << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Footer";
    auto section = bsoncxx::builder::stream::document{}
      << "CRC"                  << static_cast<int>(telemdef->CRC())
      << "Stop_Code"            << static_cast<int64_t>(telemdef->StopCode())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodbClient_->push("BACS", doc);

}  

void PushToMongoDB::pushGL860OptionalTelemetry(){
  TelemetryDefinition* telemdef = interpreter_->Telemdef();

  hsquicklook::DocumentBuilder builder("Telemetry", "GL860");
  builder.setTI(telemdef->TimeNow().tv_sec*64 + telemdef->TimeNow().tv_usec*64*1E-6);
  builder.setTimeNow();

  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start_Code"      << static_cast<int64_t>(telemdef->StartCode())
      << "Telemetry_Type"  << static_cast<int>(telemdef->TelemetryType())
      << "Time"            << static_cast<int>((telemdef->TimeNow()).tv_sec)
      << "Time_us"         << static_cast<int>((telemdef->TimeNow()).tv_usec)
      << "Telemetry_Index" << static_cast<int>(telemdef->TelemetryIndex())
      << "Run_ID"          << static_cast<int>(telemdef->RunID())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Strings";
    auto section = bsoncxx::builder::stream::document{}
      << "gl860_string"        << static_cast<std::string>(telemdef->GL860option())
      << "gl860_lastCommand"   << static_cast<std::string>(telemdef->lastCommandGL860())
    << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Footer";
    auto section = bsoncxx::builder::stream::document{}
      << "CRC"                  << static_cast<int>(telemdef->CRC())
      << "Stop_Code"            << static_cast<int64_t>(telemdef->StopCode())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodbClient_->push("BACS", doc);

}
} /* namespace balloon */
