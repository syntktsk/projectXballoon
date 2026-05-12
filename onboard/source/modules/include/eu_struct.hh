// elmo_rs_structure
//
//    阪大のraspiから愛媛大学のraspiにコマンドを送ると、
//    愛媛大学のraspiが阪大のraspiに「返事」を返す。その返事の変数や構造体を定義する
//
//                      ver 1.0       T. Yoshikawa  (2025.11.10)
//                      ver 2.0       S. Takatsuka  (2026.02.19)
#pragma once//一度だけ読み込む
#include <ostream>
// Elmoドライバ用構造体 4Bytes * 18 = 72Bytes
typedef struct ess1 {
    int   MO = 1;   // Motor On/OFF 状態 (1=モーター有効, 0=無効)
    int   UM = 0;   // Unit Mode = 0,1,2,5
    int   OB = 1;
    float MF = 3.5f;   // Motor Fault 異常停止要因（モーターが無効化された理由）
    int   EC = 1;   // Error Code エラーコード
    int   PX = 36;   // Position 現在位置 [cnt]
    float VX = 1.3f;   // Velocity 現在速度 [cnt/sec]
    float IQ = 3.2f;   // q-axis current (トルクを発生させる電流)
    float ID = 4.5f;   // d-axis current (トルクに関わらない電流、通常0付近)
    float MC = 6.4f;   // Max Current 設定されている最大電流 (q軸電流制限)
    int   BV = 0;   // Bus Voltage ドライバ内部バス電圧 [V]
    int   TI = 15;   // Temperature Information ドライバ温度 [°C]
    float TC = 1.45f;   // Torque Command 指令トルク値
    float JV = 2.21f;   // Jog Velocity 指令速度（Jog運転時）
    int   PA = 43256;   // Position Abusolute 絶対位置指令
    int   PR = 23453;   // Position Relative 相対位置指令値
    int   ac = 7;   // mode flag 運転モード番号 (0~21)
    int   ef = 0;   // enable flag 回転可能かどうか (0=回転不可, 1=回転可能)
    int   ps = 6;   // parameter set 使用中のパラメータセット番号
    int   en = 23;
    float az = 3.5f;
    int   hi = 2;
    char lc[16] = "THIS_IS_TEST";
    int16_t nc = 12345;
} ess1;

// GNSSコンパス用構造体 4Bytes * 7 = 28Bytes
typedef struct ess2{
    float la = 1;  // latitude  緯度 [deg]
    float lo = 2;  // longitude 経度 [deg]
    float he = 3;  // height    海抜高度 [m]
    float ya = 4;  // yaw       機体のヨー角 [deg]
    float pi = 5;  // pitch     機体のピッチ角 [deg]
    float ro = 6;  // roll      機体のロール角 [deg]
    float te = 7;  // temperature 気温 / センサ温度 [°C]
    float pr = 8;
    int16_t   ns = 12;
} ess2;

typedef struct ess3 {
    float te = 1;
    float hu = 2;
    float pr = 3;
    float xa = 4;
    float ya = 5;
    float za = 6;
    float ma = 7;
    float gy = 8;
}ess3;

typedef struct ess8 {
    char er[16] = "THIS_IS_TEST";
} ess8;

typedef struct ess9 {
    ess2 gnss;  // GNSSデータ一式 (28Bytes)
    ess1 elmo;  // Elmoデータ一式 (72Bytes)
    ess3 sensors;
} ess9;
// ess1 (Elmo) 用の出力ルール
inline std::ostream& operator<<(std::ostream& os, const ess1& s) {
    os << s.MO << "," << s.UM << "," << s.OB << ","<< s.MF << "," << s.EC << ","
       << s.PX << "," << s.VX << "," << s.IQ << "," << s.ID << ","
       << s.MC << "," << s.BV << "," << s.TI << "," << s.TC << ","
       << s.JV << "," << s.PA << "," << s.PR << "," << s.ac << ","
       << s.ef << "," << s.ps << "," << s.en << "," << s.az << ","<< s.hi << ","<< s.lc << ","<< s.nc;
    return os;
}

// ess2 (GNSS) 用の出力ルール
inline std::ostream& operator<<(std::ostream& os, const ess2& s) {
    os << s.la << "," << s.lo << "," << s.he << "," << s.ya << ","
       << s.pi << "," << s.ro << "," << s.te << ","<< s.pr << ","<<  s.ns;
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const ess3& s) {
    os << s.te << "," << s.hu << "," << s.pr << "," << s.xa << "," << s.ya << "," << s.za << "," << s.ma << "," << s.gy;
}
// ess9 (Whole) 用の出力ルール
inline std::ostream& operator<<(std::ostream& os, const ess9& s) {
    // 内部のgnssとelmoのルールを再利用する
    os << s.gnss << "," << s.elmo;
    return os;
}