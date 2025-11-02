HSQuickLook.main.schema =
[
  {
      "collection": "gnss_status",
      "directory": "GNSS",
      "document": "Status",
      "period": 1,
      "section": "GNSSData",
      "contents": {
          "SeqNo": {"type": "int"},
          "Ypr": {"type": "float[]"},
          "Angrate": {"type": "float[]"},
          "PosLla": {"type": "double[]"},
          "Velocity": {"type": "float[]"},
          "InsStatus": {"type": "int"},
          "GNSStemp": {"type": "float"},
          "GNSSpres": {"type": "float"},
          "IMUsensSat": {"type": "int"},
          "GNSSnumSats": {"type": "int"},
          "TUTC": {"type": "int[]"},
        }
  },
  {
    "collection": "gnss_status",
    "directory": "GNSS",
    "document": "Status",
    "period": 1,
    "tableName": "Trend_graph",
    "section": "GNSSData",
    "contents": {
        "GNSS_Temperature": { "type": "trend-graph",
            "group": [
                {"source": "GNSStemp","conversion":convert_gnss_T, "options":{"legend": "temperature","color": "black"}},
            ],
            "options":{"xWidth": 1000,"yRange":[-40, 0]}
        },
        "GNSS_pressure": { "type": "trend-graph",
            "group": [
                {"source": "GNSSpres","conversion":convert_gnss_P, "options":{"legend": "pressure","color": "black"}},
            ],
            "options":{"xWidth": 1000,"yRange":[-1, 1]}
        },
    }
  }
];

function convert_gnss_T(d)  { return parseFloat(d); }
function convert_gnss_P(d)  
  { dd = parseFloat(d);
    return dd;
  }