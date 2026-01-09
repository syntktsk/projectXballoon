// elmo_control.h
//    Elmo社製ドライバーの制御のために使用
//                      ver 1.0       T. Yoshikawa  (2025.06.19)
//                      ver 1.1       H. Awaki      (2025.06.20)
//                   ☆構造体メンバーの名前の微修正とメンバーの追加
//                      ver 1.2       T. Yoshikawa  (2025.06.20)
//                   ☆ #define で ENCODER_COUNTS_PER_REV を追加
//                      ver 1.3       H. Awaki      (2025.06.23)
//                   ☆ 芳川製ELMOコマンドと文字列との対応を追加
//                      ver 1.4       T. Yoshikawa  (2025.06.24)
//                   ☆ ELMOコマンドの最後に付ける文字を\n→\rに変更。セミコロンを除去。 move_current_target[]を8bytesで宣言
//                      ver 1.5       T. Yoshikawa  (2025.11.11)
//                   ☆ ENCODER_COUNTS_PER_REVをRenishawのエンコーダように書き換えた


#pragma once

// Elmoのドライバの情報構造体
//bitのパディングが起きているため、bit fieldを使う必要がないかもしれない。　20Bytes
//20250619, 18:05 break_statusとmotor_statusはどちらも普通のunsigned charを使うことにする
typedef struct {
    unsigned char brake_status;    // 1 Byte   ブレーキのステータス格納 ON=1, OFF=0
    unsigned char motor_status;    // 1 Byte   モーターのステータス格納 ON=1, OFF=0
    unsigned char unit_mode;           // 1 Byte  UM unit_mode 1~5
    int position_count;                // 4 Bytes エンコーダ位置 (cnt) [-2^31 ~ 2^31-1]
    float velocity;                    // 4 Bytes エンコーダ速度 (cnt/sec) [-2e9 ~ 2e9]
    float current_IQ;                  // 4 Bytes 有効電流値 IQ[A]
    float current_ID;                  // 4 Bytes 無効電流値 ID[A]
} Elmo_Info;


// 制御パラメータ構造体 24Bytes
typedef struct {
    float target_az_deg;    // 4 Bytes 位置制御モード１(GNSS使用)目標位置[degree]
    float target_RA_deg;    // 4 Bytes 位置制御モード２(GNSS使用)目標位置の赤経[degree]
    float target_DEC_deg;    // 4 Bytes 位置制御モード２(GNSS使用)目標位置の赤緯[degree]
    int target_count;  // 4 Bytes 位置制御モード３(エンコーダー)目標位置[count]
    unsigned char control_mode;        // 1 Byte  control_modeが1のとき、GNSSコンパスと目標位置との差分をエンコーダの値に換算して指令を出す.2のときエンコーダの目標位置に移動する
    float pid_kp;                      // 4 Bytes P制御のゲイン
    float pid_ki;                      // 4 Bytes I制御のゲイン
    float pid_kd;                      // 4 Bytes D制御のゲイン
    int   status;                      // 0: OK, 1: BUSY, 2: ERROR 9: Newコマンド
    char  cmmd[20];                    // コマンド
//    char  mesg[100];                   // 最後に送信したコマンドに対する返信
} Control_Params;

// コマンドとの対応を追加 (H.Awaki, 2025.06.23)
// コマンドの最後に付ける文字を\n→\rに変更。セミコロンを除去 (T.Yoshikawa, 2025.06.24)
char motor_off[]="MO=0\r";    // ELMO モーターOFF
char motor_on[]="MO=1\r";     // ELMO モーターON
char brake_off[]="OL[1]=1\r"; // PIVOT ブレーキ OFF
char brake_on[]="OL[1]=0\r";  // PIVOTブレーキ ON

char torque_mode[]="UM=1\r";  // ELMOドライバー　トルクモード
char velocity_mode[]="UM=2\r";  // ELMOドライバー速度モード
char position_mode[]="UM=5\r";  // ELMOドライバー位置モード

//モーターやエンコーダーの情報を読み取るコマンド
char read_error_message[]="MF\r"; //エラーコード読み取り, MF=0で正常
char read_position_encoder[]="PX\r";  //encoderの位置読取り[cnt]
char read_velocity_encoder[]="VX\r";  //encoderの速度読取り[cnt/sec]
char read_current_max[]="MC\r"; //ドライバにかけることのできる最大電流を読取り[A]
char read_current_effective[]="IQ\r"; //有効電流の読取り[A]
char read_current_uneffective[]="ID\r"; //無効電流の読取り[A]
//char read_position_vn300[]="???\r"; //gsnnコンパス、vn300から位置を読み取るコマンド。ドライバでは実装不可能と思われる。


//ドライバの初期設定に使われるコマンド
char set_current_peak[]="PL[1]=10\r"; //最大電流[A]を設定。　PL[1]の値はMC以下にsetすること
char set_time_peak[]="PL[2]=10\r"; //電流値がPL[1]に張り付いて、PL[2]秒経過したら、電流値を連続電流値CL[1] [A]に下げる
char set_current_continuous[]="CL[1]=5\r"; //連続電流を設定 [A]
char set_current_threshold[]="CL[2]=30\r"; //モータースタックと判断するための電流閾値を設定[%]
char set_velocity_threshold[]="CL[3]=100\r"; //モータースタックと判断するための速度閾値を設定[cnt/sec]
char set_time_threshold[]="CL[4]=3000\r"; //モータースタックと判断するための時間閾値を設定[msec]

//モーターを回すには、この値を変更してモーターを制御をする。コマンドヘッダー＋数値
char move_current_target[64]="TC="; //モータードライバに4[A]を印加。モーターは即座に動き出す
char set_velocity_target[]="JV[1]="; //回転速度を格納 UM=2 , [cnt/sec]
char set_position_target_absolute[]="PA[1]="; //ターゲット位置を絶対位置として設定(UM=3, UM=5) [cnt]
char set_position_target_relative[]="PR[1]="; //ターゲット位置を相対位置として設定(UM=3, UM=5)[cnt]
char move_command[]="BG[1]\r";  //モータが即座に動き出す。　UM=2のとき速度ループ、UM=3のときステッパーモード、UM=5のとき位置ループ
char stop_command_safe[]="ST[1]\r"; //UM=2,3,5で現在実行中の動作を安全に減速、停止させるコマンド。UM=1の時は、トルク命令を0にする。


