// elmo_rs_structure
//
//    阪大のraspiから愛媛大学のraspiにコマンドを送ると、
//    愛媛大学のraspiが阪大のraspiに「返事」を返す。その返事の変数や構造体を定義する
//
//                      ver 1.0       T. Yoshikawa  (2025.11.10)
//                      ver 2.0       S. Takatsuka  (2026.02.19)
#pragma once//一度だけ読み込む
// Elmoドライバ用構造体 4Bytes * 18 = 72Bytes
typedef struct {
    int   MO;   // Motor On/OFF 状態 (1=モーター有効, 0=無効)
    int   UM;   // Unit Mode = 0,1,2,5
    int   OB;
    float MF;   // Motor Fault 異常停止要因（モーターが無効化された理由）
    int   EC;   // Error Code エラーコード
    int   PX;   // Position 現在位置 [cnt]
    float VX;   // Velocity 現在速度 [cnt/sec]
    float IQ;   // q-axis current (トルクを発生させる電流)
    float ID;   // d-axis current (トルクに関わらない電流、通常0付近)
    float MC;   // Max Current 設定されている最大電流 (q軸電流制限)
    int   BV;   // Bus Voltage ドライバ内部バス電圧 [V]
    int   TI;   // Temperature Information ドライバ温度 [°C]
    float TC;   // Torque Command 指令トルク値
    float JV;   // Jog Velocity 指令速度（Jog運転時）
    int   PA;   // Position Abusolute 絶対位置指令
    int   PR;   // Position Relative 相対位置指令値
    int   ac;   // mode flag 運転モード番号 (0~21)
    int   ef;   // enable flag 回転可能かどうか (0=回転不可, 1=回転可能)
    int   ps;    // parameter set 使用中のパラメータセット番号
    int   en;
    float az;
    int   hi;
    char lc[16];
} ess1;

// GNSSコンパス用構造体 4Bytes * 7 = 28Bytes
typedef struct {
    float la;  // latitude  緯度 [deg]
    float lo;  // longitude 経度 [deg]
    float he;  // height    海抜高度 [m]
    float ya;  // yaw       機体のヨー角 [deg]
    float pi;  // pitch     機体のピッチ角 [deg]
    float ro;  // roll      機体のロール角 [deg]
    float te;  // temperature 気温 / センサ温度 [°C]
} ess2;
