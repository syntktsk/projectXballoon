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

];
