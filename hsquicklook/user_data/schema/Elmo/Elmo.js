HSQuickLook.main.schema =
    [
        {
			"collection": "BACS",
			"directory": "Telemetry",
			"document": "Whole",
			"period": 1,
			"section": "Elmo_Status",
			"contents": {
				"Motor_On/Off": {"type": "int"},
                "Unit_Mode": {"type": "int"},
                "Motor_Fault": {"type": "float"},
                "Error_Code": {"type": "int"},
                "Position_PX": {"type": "int"},
                "Velocity_VX": {"type": "float"},
                "I_Qaxis": {"type": "float"},
                "I_Daxis": {"type": "float"},
                "Max_Current": {"type": "float"},
                "Bus_Voltage": {"type": "int"},
                "Temperature_Info": {"type": "int"},
                "Torque_Command": {"type": "float"},
                "Jog_Velocity": {"type": "float"},
                "Position_Absolute": {"type": "int"},
                "Position_Relative": {"type": "int"},
                "Mode_Flag": {"type": "int"},
                "Enable_Flag": {"type": "int"},
                "Parameter_Set": {"type": "int"},
			}
    }
    ];
