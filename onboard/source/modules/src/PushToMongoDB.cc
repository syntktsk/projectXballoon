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
  {
    const std::string section_name = "Strings";
    auto section = bsoncxx::builder::stream::document{}
      << "StringsResponse"             << static_cast<std::string>(telemdef->OptionalStrings())
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
      << "Pivot_Temperature"        << static_cast<int>(telemdef->PivotTemp())
      << "StarCamera_Temperature"   << static_cast<int>(telemdef->StarCameraTemp())
      << "Mirror_Temperature"       << static_cast<int>(telemdef->MirrorTemp())
      << "GNSS_Temperature_HK"      << static_cast<int>(telemdef->GnssTemp_hk())
      << "Calculator_Temperature"   << static_cast<int>(telemdef->CulculatorTemp())
      << "Battery_Temperature"      << static_cast<int>(telemdef->BatteryTemp())
      << "Gyro_Temperature"         << static_cast<int>(telemdef->GyroTemp())
      << "CMOS_Temperature"         << static_cast<int>(telemdef->CmosTemp())

      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Voltage";
    auto section = bsoncxx::builder::stream::document{}
      << "PC_Voltage"               << static_cast<int>(telemdef->PcVolt())
      << "StarCamera_Voltage"       << static_cast<int>(telemdef->StarCameravolt())
      << "GNSS_Voltage"             << static_cast<int>(telemdef->GnssVolt())
      << "Gyro_Voltage"             << static_cast<int>(telemdef->GyroVolt())
      << "CMOS_Voltage"             << static_cast<int>(telemdef->CmosVolt())
      << "Router_Voltage"           << static_cast<int>(telemdef->RouterVolt())
      << "Heater_Voltage"           << static_cast<int>(telemdef->HeaterVolt())
      << "Pi_HK_Voltage"            << static_cast<int>(telemdef->Pi_hkVolt())
      << "Pivot_Voltage"            << static_cast<int>(telemdef->PivotVolt())
      << "Hub_Voltage"              << static_cast<int>(telemdef->HubVolt())
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
      << "Pivot_Temperature"        << static_cast<int>(telemdef->PivotTemp())
      << "StarCamera_Temperature"   << static_cast<int>(telemdef->StarCameraTemp())
      << "Mirror_Temperature"       << static_cast<int>(telemdef->MirrorTemp())
      << "GNSS_Temperature_HK"      << static_cast<int>(telemdef->GnssTemp_hk())
      << "Calculator_Temperature"   << static_cast<int>(telemdef->CulculatorTemp())
      << "Battery_Temperature"      << static_cast<int>(telemdef->BatteryTemp())
      << "Gyro_Temperature"         << static_cast<int>(telemdef->GyroTemp())
      << "CMOS_Temperature"         << static_cast<int>(telemdef->CmosTemp())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  {
    const std::string section_name = "Voltage";
    auto section = bsoncxx::builder::stream::document{}
      << "PC_Voltage"               << static_cast<int>(telemdef->PcVolt())
      << "StarCamera_Voltage"       << static_cast<int>(telemdef->StarCameravolt())
      << "GNSS_Voltage"             << static_cast<int>(telemdef->GnssVolt())
      << "Gyro_Voltage"             << static_cast<int>(telemdef->GyroVolt())
      << "CMOS_Voltage"             << static_cast<int>(telemdef->CmosVolt())
      << "Router_Voltage"           << static_cast<int>(telemdef->RouterVolt())
      << "Heater_Voltage"           << static_cast<int>(telemdef->HeaterVolt())
      << "Pi_HK_Voltage"            << static_cast<int>(telemdef->Pi_hkVolt())
      << "Pivot_Voltage"            << static_cast<int>(telemdef->PivotVolt())
      << "Hub_Voltage"              << static_cast<int>(telemdef->HubVolt())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }
  
  std::cout << "DEBUG: Mongo push SoftwareErrorCode: " <<telemdef->SoftwareErrorCode()<< std::endl;
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
    const std::string section_name = "Status";
    auto section = bsoncxx::builder::stream::document{}
      
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

  hsquicklook::DocumentBuilder builder("Telemetry", "Option");
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

} /* namespace balloon */
