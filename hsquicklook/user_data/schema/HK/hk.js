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
      "last_command":{"type": "int" }
      // "Last_Command_name":{"type": "string"},
      // "Last_Command_Value":{"type": "int"}
    }
  },

  // 2. GNSS Status
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
    "section": "Strings",
    "contents": {
      "StringsResponse": { "type": "string" },
    }
  },

  // 5. Software Status & Error
  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "Whole",
    "period": 1,
    "section": "Software_Error",
    "contents": {
      "Last_Command_Index" :{"type": "int"},
      "Last_Command_Code" :{"type": "int"},
      "Command_Reject_Count" :{"type": "int"},
      "Software_Error_Code" :{"type": "int64_t"},
      "CRC":{"type": "int"},
      "Stop_Code" :{"type": "int64_t"},
      "MOTOR_FAULT_DETECTED": { "type": "string" },
      "MOTOR_ENCODER_ERROR": { "type": "string" },
      "MOTOR_COMUTATION_FAULT": { "type": "string" },
      "MOTOR_OVER_CURRENT": { "type": "string" },
      "EC_UNKNOWN_COMMAND": { "type": "string" },
      "EC_INDEX_OUTOFRANGE": { "type": "string" },
      "EC_PROGRAM_NOT_RUNNING": { "type": "string" },
      "INVALID_COMMAND": { "type": "string" },
      "SEND_TELEMETRY_COMMUNICATION_ERROR": { "type": "string" },
      "RECEIVE_EU_RESPONSE_INIT_ERROR": { "type": "string" }
    }
  },

  // 6. Packet Header Info
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
  }
];

  // // 3. Temperature (HK)
  // {
  //   "collection": "BACS",
  //   "directory": "Telemetry",
  //   "document": "Whole",
  //   "period": 1,
  //   "section": "Temperature",
  //   "contents": {
  //     "Pivot_Temperature": { "type": "int" },
  //     "StarCamera_Temperature": { "type": "int" },
  //     "Mirror_Temperature": { "type": "int" },
  //     "GNSS_Temperature_HK": { "type": "int" },
  //     "Calculator_Temperature": { "type": "int" },
  //     "Battery_Temperature": { "type": "int" },
  //     "Gyro_Temperature": { "type": "int" },
  //     "CMOS_Temperature": { "type": "int" }
  //   }
  // },

  // // 4. Voltage (HK)
  // {
  //   "collection": "BACS",
  //   "directory": "Telemetry",
  //   "document": "Whole",
  //   "period": 1,
  //   "section": "Voltage",
  //   "contents": {
  //     "PC_Voltage": { "type": "int" },
  //     "StarCamera_Voltage": { "type": "int" },
  //     "GNSS_Voltage": { "type": "int" },
  //     "Gyro_Voltage": { "type": "int" },
  //     "CMOS_Voltage": { "type": "int" },
  //     "Router_Voltage": { "type": "int" },
  //     "Heater_Voltage": { "type": "int" },
  //     "Pi_HK_Voltage": { "type": "int" },
  //     "Pivot_Voltage": { "type": "int" },
  //     "Hub_Voltage": { "type": "int" }
  //   }
  // },