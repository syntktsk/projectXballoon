HSQuickLook.main.schema = [
  // 1. Elmo Motor Status
  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "Whole",
    "period": 1,
    "section": "Elmo_Status",
    "contents": {
      "Motor_OnOff": { "type": "int" },
      "Unit_Mode": { "type": "int" },
      "Brake_OnOff": { "type": "int" },
      "Motor_Fault": { "type": "int" },
      "Error_Code": { "type": "int" },
      "Position_PX": { "type": "int" },
      "Velocity_VX": { "type": "float" },
      "I_Qaxis": { "type": "float" },
      "I_Daxis": { "type": "float" },
      "Max_Current": { "type": "float" },
      "Bus_Voltage": { "type": "int" },
      "Temperature_Info": { "type": "int" },
      "Torque_Command": { "type": "float" },
      "Jog_Velocity": { "type": "float" },
      "Position_Absolute": { "type": "int" },
      "Position_Relative": { "type": "int" },
      "Mode_Flag": { "type": "int" },
      "Enable_Flag": { "type": "int" },
      "Parameter_Set": { "type": "int" },
      "en": { "type": "int" },
      "az": { "type": "float" },
      "hi": { "type": "int" },
      "last_command":{"type": "string" }
    }
  },
  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "Whole",
    "period": 1,
    "section": "GNSS_Status",
    "contents": {
      "Latitude": { "type": "float" },
      "Longitude": { "type": "float" },
      "Height": { "type": "float" },
      "Yaw": { "type": "float" },
      "Pitch": { "type": "float" },
      "Roll": { "type": "float" },
      "Temperature": { "type": "float" }
    }
  },
  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "Whole",
    "period": 1,
    "section": "Temperature",
    "contents": {
      "Pivot_Temperature": { "type": "float" },
      "StarCamera_Temperature": { "type": "float" },
      "Mirror_Temperature": { "type": "float" },
      "GNSS_Temperature_HK": { "type": "float" },
      "Calculator_Temperature": { "type": "float" },
      "Battery_Temperature": { "type": "float" },
      "Gyro_Temperature": { "type": "float" },
      "CMOS_Temperature": { "type": "float" }
    }
  },
  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "Whole",
    "period": 1,
    "section": "Voltage",
    "contents": {
      "PC_Voltage": { "type": "float" },
      "StarCamera_Voltage": { "type": "float" },
      "GNSS_Voltage": { "type": "float" },
      "Gyro_Voltage": { "type": "float" },
      "CMOS_Voltage": { "type": "float" },
      "Router_Voltage": { "type": "float" },
      "Heater_Voltage": { "type": "float" },
      "Pi_HK_Voltage": { "type": "float" },
      "Pivot_Voltage": { "type": "float" },
      "Hub_Voltage": { "type": "float" }
    }
  },

  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "Whole",
    "period": 1,
    "section": "Header",
    "contents": {
      "Telemetry_Index": { "type": "int" },
      "Run_ID": { "type": "int" },
      "Time": { "type": "int" },
      "Time_us": { "type": "int" }
    }
  },
  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "Option",
    "period": 1,
    "section": "Status",
    "contents": {
      "er =": { "type": "string" },
    }
  },
  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "gl860",
    "period": 1,
    "section": "Strings",
    "contents": {
      "gl860_string": { "type": "string" },
      "gl860_lastCommand": { "type": "string" },
    }
  },
    {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "GPIO",
    "period": 1,
    "section": "Relay",
    "contents": {
      "GPIO_22": { "type": "string" },
      "GPIO_23": { "type": "string" },
      "GPIO_24": { "type": "string" },
    }
  },
];

