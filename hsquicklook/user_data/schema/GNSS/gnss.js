HSQuickLook.main.schema =
[
  {
      "collection": "BACS",
      "directory": "Telemetry",
      "document": "Whole",
      "period": 1,
      "section": "GNSS_Status",
      "contents": {
                "Latitude": {"type": "float"},
                "Longitude": {"type": "float"},
                "Height": {"type": "float"},
                "Yaw": {"type": "float"},
                "Pitch": {"type": "float"},
                "Roll": {"type": "float"},
                "Temperature": {"type": "float"}
        }
  },
  {
    "collection": "BACS",
    "directory": "Telemetry",
    "document": "Whole",
    "period": 1,
    "tableName": "Trend_graph",
    "section": "GNSS_Status",
    "contents": {
        "GNSS_Temperature": { "type": "trend-graph",
            "group": [
                {"source": "Temperature","conversion":convert_gnss_T, "options":{"legend": "temperature","color": "black"}},
            ],
            "options":{"xWidth": 1000,"yRange":[-40, 0]}
        },
        // "GNSS_pressure": { "type": "trend-graph",
        //     "group": [
        //         {"source": "GNSSpres","conversion":convert_gnss_P, "options":{"legend": "pressure","color": "black"}},
        //     ],
        //     "options":{"xWidth": 1000,"yRange":[-1, 1]}
        // },
    }
  }
];

function convert_gnss_T(d)  { return parseFloat(d); }
function convert_gnss_P(d)  
  { dd = parseFloat(d);
    return dd;
  }