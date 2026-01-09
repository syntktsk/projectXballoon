// ess_client.c
//
//サーバー側
//                      ver 1.0       T. Yoshikawa  (2025.11.10)
//                      ver 1.1		T.Yoshikawa (2025.12.08)
//    ☆実際にElmoドライバと通信できるように変更
//                      ver 2.0		T.Yoshikawa (2025.12.09)
//    ☆MOとUMコマンドが打てることを確認した。TCコマンドが打てないため、試行錯誤中
//                      ver 3.0		T.Yoshikawa (2025.12.15)
//    ☆TCコマンドをElmoが認識できるようにコードを書き換えた。

//                      ver 3.1		T.Yoshikawa (2025.12.22) 失敗
//    ☆10Hz以上でstatusを読み出すようにコードを書き換える（今回は、20Hzを目標とする）
//                      ver 3.2		T.Yoshikawa (2025.12.22)　失敗
//    ☆read statusは、カレントディレクトリのファイルに書いていくことにする。

//                      ver 3.3		T.Yoshikawa (2025.12.22)
//    ☆10Hz以上でstatusを読み出すようにコードを書き換える（今回は、20Hzを目標とする）
//                      ver 3.4		T.Yoshikawa (2025.12.22)
//    ☆1秒ごとのstatus読み出しと、クライアントによるコマンドを送ったりすることに成功した

//                      ver 4.1		T.Yoshikawa (2025.12.22)
//    ☆1秒ごとのstatus読み出しと、クライアントによるコマンドを送ったりすることに成功した
//                      ver 4.2		T.Yoshikawa (2025.12.24)
//    ☆lcコマンドと、rs3を追加

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>      // ★ O_RDWR, O_NOCTTY
#include <sys/types.h>
#include <sys/stat.h>

#include "elmo_rs_structure_v2p0.h"//socket通信用
#include"elmo_control.h" //serial通信用

#define PORT 9998//
#define BUFFER_SIZE 1024//
#define NANOSLEEP 5000 //usleep(NANOSLEEP); というコードによって、10ms待機させる
#define K 8.0 // ← チューニング用ゲイン
#define ENCODER_COUNTS_PER_REV 67108864 //エンコーダー RESA30リングとResoluterを360度回転させた時の数値(2^26)

// ===== 設定値 =====
#define SERIAL_DEVICE "/dev/ttyACM0"
#define SERIAL_BAUDRATE B115200

int init_udp_server(int port);//UDPでそのPORTに通信を開始する
int init_elmo_serial(const char *device, int baud_rate);//そのdevice ボーレートでシリアル通信を開始する。初期設定もここで行う
void finalize_elmo_serial(int serial_fd);//シリアル通信を閉じる

void rs_handle(int serial_fd, int sock,struct sockaddr_in *client_addr,socklen_t addr_len,const char *buffer,ess1 *data1,ess2 *data2);
void make_ess1_text(const ess1 *d, char *buf, size_t buf_size);



void elmo_update_ess1(int serial_fd, ess1 *st);
void gnss_dummy_update_ess2(ess2 *st);

double double_extract_number(const char *input);
int open_serial_port(const char *device, int baud_rate);// serialポートをopenする関数
void close_serial_port(int fd);// serialポートを閉じる関数
ssize_t write_serial(int fd, const char *data, size_t len);// fdにdataを書き込む関数
ssize_t read_serial(int fd, char *buffer, size_t len);// fdのバッファを読み取る関数
void print_send_command(char *command);// sendしたコマンドを表示する関数
void debug_hex(const char *label, const char *data, int length);// RAWデータを16進表示
void print_visible(const char *label, const char *data, int length);// ASCIIで表示
int send_elmo_command(int serial_fd, char *command, char *recv_buf, int buf_size);// Elmoにコマンドを送って、必要なら返事を受け取る関数
int send_elmo_command_wc(int serial_fd, char *command, char *recv_buf, int buf_size);

#include <stdint.h>
#include <time.h>

// 現在時刻（ナノ秒）を取得する関数
uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}


//int抽出関数
int extract_int(const char *s)
{
    while (*s) {
        if ((*s >= '0' && *s <= '9') || *s == '-' || *s == '+') {
            return (int)strtol(s, NULL, 10);
        }
        s++;
    }
    return 0;   // 数字が見つからなかった場合
}


float extract_float(const char *s)
{
    while (*s) {
        if ((*s >= '0' && *s <= '9') || *s == '-' || *s == '+' || *s == '.') {
            return (float)strtof(s, NULL);
        }
        s++;
    }
    return 0.0f;   // 数字が見つからなかった場合
}



#include <pthread.h>

typedef struct {
    int serial_fd;
    ess1 *data1;
    ess2 *data2;
} elmo_args;




/* =========================================
 * 現在時刻を Unix time [sec + nsec] で取得
 * 表示例: 1766404370.123456789
 * ========================================= */
typedef struct {
    time_t sec;
    long   nsec;
} time_sec_nsec_t;

static inline time_sec_nsec_t get_time_sec_nsec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    time_sec_nsec_t t;
    t.sec  = ts.tv_sec;
    t.nsec = ts.tv_nsec;
    return t;
}

void *elmo_logger(void *arg)
{
    elmo_args *args = (elmo_args *)arg;
    int serial_fd   = args->serial_fd;
    ess1 *data1     = args->data1;
    ess2 *data2     = args->data2;

    /* ===== ファイルを最初に1回だけ open（日時入り） ===== */
    char filename[256];

    /* 現在時刻を取得（ローカル時刻） */
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);

    /* ファイル名生成: YYYYMMDD_HHMM_SS_elmo_logger.csv */
    snprintf(filename, sizeof(filename),
            "%04d%02d%02d_%02d%02d_%02d_elmo_logger.csv",
            tm_now.tm_year + 1900,
            tm_now.tm_mon + 1,
            tm_now.tm_mday,
            tm_now.tm_hour,
            tm_now.tm_min,
            tm_now.tm_sec);

    /* open */
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        perror("fopen elmo_logger.csv");
        return NULL;
    }

    /* ===== ヘッダ（空ファイルなら書く） ===== */
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0) {
        fprintf(fp,
            "time_s,MO,UM,MF,EC,PX,VX,IQ,ID,MC,BV,TI,TC,JV,PA,PR,ac,ef,ps,la,lo,he,ya,pi,ro,te\n");
        fflush(fp);
    }

    /* ===== 50ms 周期（MONOTONIC想定） ===== */
    const uint64_t interval_ns = 20000000ull; // 20 ms
    //const uint64_t interval_ns = 500000000ull; // 500 ms
    //const uint64_t interval_ns = 5000000000ull; // 5000 ms
    uint64_t last_time = get_time_ns();


    /* ===== EC 切り替え設定 ===== */
    const int ec_values[] = {0, 1, 2, 4, 8};
    const int ec_count = sizeof(ec_values) / sizeof(ec_values[0]);
    int ec_index = 0;
    const uint64_t ec_interval_ns = 10ull * 1000 * 1000 * 1000; // 10秒
    uint64_t last_ec_time = get_time_ns();


    while (1) {
        uint64_t now = get_time_ns();

        if (now - last_time >= interval_ns) {
            last_time = now;

            char buf[128];

            /* ---- Elmo から取得 ---- */
            send_elmo_command(serial_fd, read_error_message, buf, sizeof(buf));
            data1->MF = extract_float(buf);

            send_elmo_command(serial_fd, read_position_encoder, buf, sizeof(buf));
            data1->PX = extract_int(buf);

            send_elmo_command(serial_fd, read_velocity_encoder, buf, sizeof(buf));
            data1->VX = extract_float(buf);

            send_elmo_command(serial_fd, read_current_effective, buf, sizeof(buf));
            data1->IQ = extract_float(buf);

            send_elmo_command(serial_fd, read_current_uneffective, buf, sizeof(buf));
            data1->ID = extract_float(buf);


                /* ===== 30秒ごとに EC を変更 ===== */
            if (now - last_ec_time >= ec_interval_ns) {
                last_ec_time = now;

                data1->EC = ec_values[ec_index];
                ec_index = (ec_index + 1) % ec_count;

                /* Elmo に EC を送る場合（例） */
                // send_elmo_set_ec(serial_fd, data1->EC);
            }


            /* ===== ログ出力（sec.nsec 完全精度） ===== */
            time_sec_nsec_t t = get_time_sec_nsec();

            fprintf(fp,
                "%ld.%09ld,%d,%d,%f,%d,%d,%.3f,%.5f,%.5f,%.2f,%d,%d,%.3f,%.3f,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f\n",
                t.sec,          // 秒
                t.nsec,         // ナノ秒（9桁固定）
                data1->MO,//%d
                data1->UM,//%d
                data1->MF,//%f
                data1->EC,//%d
                data1->PX,//%d
                data1->VX,//%.3f
                data1->IQ,//%5d
                data1->ID,//%5d
                data1->MC,//%.2f
                data1->BV,//%d
                data1->TI,//%d
                data1->TC,//%.3f
                data1->JV,//%.3f
                data1->PA,//%d
                data1->PR,//%d
                data1->ac,//%d
                data1->ef,//%d
                data1->ps,//%d
                data1->lc, //%d last command 追加
                data2->la,  // %f緯度
                data2->lo,  // 経度
                data2->he,  // 海抜高度
                data2->ya,  // yaw
                data2->pi,  // pitch
                data2->ro,  // roll
                data2->te   // 温度
            );

            fflush(fp);  // ★ 即時書き出し
        }

        usleep(1000);  // CPU負荷対策（1ms）
    }

    /* 実際は到達しないが形式上 */
    fclose(fp);
    return NULL;
}


int main() {

    int mode_flag = 0;//acを変更するためのフラグ。もっといい宣言方法があるかも。。

    //Elmoドライバから読み取った値が、愛媛大のラズパイ(サーバー側)に保存されている。
    ess1 data1 = {
        .MO = 0,      
        .UM = 1,
        .MF = 0.0f,
        .EC = 0,
        .PX = 0,
        .VX = 0.0f,
        .IQ = 0.0f,
        .ID = 0.0f,
        .MC = 20.0f,
        .BV = 0,
        .TI = 0,
        .TC = 0.0f,
        .JV = 0.0f,
        .PA = 0,
        .PR = 0,
        .ac = 0,
        .ef = 0,
        .ps = 0,
        .lc = 0,
    };

    //GNSSコンパスから読み取った値が、愛媛大のラズパイ(サーバー側)に保存されている。
    ess2 data2 = {
        .la = 0.0f,  // 緯度
        .lo = 0.0f,  // 経度
        .he = 0.0f,  // 海抜高度
        .ya = 0.0f,  // yaw
        .pi = 0.0f,  // pitch
        .ro = 0.0f,  // roll
        .te = 0.0f   // 温度
    };//fはfloat(4Bytes)型ということを表す

    //####################
    //サーバのUDP通信設定
    //###################
    int sock;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    sock = init_udp_server(PORT);
    if (sock < 0) {
        fprintf(stderr, "サーバ初期化に失敗しました\n");
        return 1;
    }//UDPセットアップ終了




    //################
    //serial通信設定
    //#################
    int serial_fd = init_elmo_serial(SERIAL_DEVICE, SERIAL_BAUDRATE);

    pthread_t thread;


    elmo_args args;
    args.serial_fd = serial_fd;
    args.data1 = &data1;
    args.data2 = &data2;   // ★ これを追加 ★

    if (pthread_create(&thread, NULL, elmo_logger, &args) != 0) {
        perror("pthread_create");
        return 1;
    }




    //#######################
    //メインループ：受信を待機
    //#######################
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        //クライアントのIPも静的にすると、より高速な通信ができるかも？！

        // クライアントからデータ受信
        int recv_len = recvfrom(sock, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom");
            break;
        }


        // クライアント情報を取得
        char client_ip[INET_ADDRSTRLEN];//clientのIPを取得
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));//clientのIPをバイナリにした
        printf("\n受信 from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        printf("受信データ: %s\n", buffer);

        //############################
        // ここでコマンドの系列を判定する。「rs系かどうか」を判定
        //##########################
        if (strncmp(buffer, "rs", 2) == 0) {
            rs_handle(serial_fd, sock, &client_addr, addr_len, buffer, &data1, &data2);
        }

        //#######################
        //Motor ON?OFF系列
        //########################
        //モータON/OFF　MO=1でモータON, MO=0でモータOFF
        else if (strncmp(buffer, "MO", 2) == 0) {
            // "MO" の後ろ（index 2 以降）を整数に変換
            int value = atoi(buffer + 3);
            // クライアントにエコー返信
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);

            // 判定
            if (value == 1){
                send_elmo_command(serial_fd, motor_on, NULL, 0); // motor ON
                data1.MO = 1;
                data1.lc = 71;
                printf("モータ → ON（Enable）にしました\n");
            }
            else if (value == 0){
                send_elmo_command(serial_fd, motor_off, NULL, 0);//motor をoffに
                data1.MO = 0;
                data1.lc = 70;
                printf("モータ → OFF（Disable）にしました\n");
            }
            else printf("MOコマンドの値が不正です（MO0 または MO1 を指定してください）\n");
        }


        //##########################
        //UNIT MODE 系列
        //##########################  ただし、mode変更は、motorがOFF(MO=0)のときしかできない
        else if (strncmp(buffer, "UM", 2) == 0) {
            // "UM" の後ろ（index 2 以降）を整数に変換
            int value = atoi(buffer + 3);

            // クライアントにエコー返信
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);

            // 判定
            //if (value == 0) printf("MO=0です。\n");
            if (value == 1){
                send_elmo_command(serial_fd, torque_mode, NULL, 0);//トルクモード(電流モード)にする
                data1.UM=1;
                data1.lc = 81;
                printf("UM=1にしました\n");
                }
            else if (value == 2){
                send_elmo_command(serial_fd, velocity_mode, NULL, 0);//速度モードにする
                data1.UM=2;
                data1.lc = 82;
                printf("UM=2にしました\n");
                }
            else if (value == 5){
                send_elmo_command(serial_fd, position_mode, NULL, 0);//位置モードにする
                data1.UM=5;
                data1.lc = 85;
                printf("UM=5にしました\n");
                }
            else printf("UMの値が不正です。\n");
        }

        //#############################################
        //TC=1.2　のように電流変更コマンドが来た時
        //#########################################
        else if (strncmp(buffer, "TC", 2) == 0) {
            printf("echo to client: [%s]\n", buffer);
            float tc = atof(buffer + 3);  // "TC=" を飛ばす
            printf("tc = %f\n", tc);
            snprintf(move_current_target, sizeof(move_current_target), "TC=%.2f\r", tc);
            send_elmo_command(serial_fd, move_current_target, NULL, 0); //モーターにmove_current_target[A]を印加。即座にモーターが動き出すはず
            //sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);//高塚さんとは構造体でやりとりするため、ここは不要
            data1.TC = tc; //ここが悪い気がしたから、一旦消去
            data1.lc = 401111111;//本番では固定小数点方式を使う？一旦固定する
            printf("コマンド %s を受信し、電流目標を変更しました\n", buffer);
        }


        //#########################################
        // ac0 ~ ac21 のようなモード変更コマンド(フラグを立てる)
        //########################################
        else if (strncmp(buffer, "ac", 2) == 0) {
            // "ac" の後ろの数字を取得
            int mode = atoi(buffer + 2);
            // 範囲チェック
            if (mode >= 0 && mode <= 21) {
                mode_flag = mode;
                // クライアントへエコーバック
                sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
                data1.ac=mode;

                //acの切り替え
                 char buf[16];
                snprintf(buf, sizeof(buf), "30%02d", mode);
                data1.lc = atoi(buf);

                printf("コマンド %s を受信：GLOBAL_mode_flag = %d に変更\n",buffer,mode_flag);
                }
            else{
                printf("無効な ac コマンド: %s（0～21のみ有効）\n", buffer);
                }
        }


        //#################################################
        //モードを実際に変更するコマンド(ac0のダブルコマンド)
        //###############################################
        else if (strcmp(buffer, "acex") == 0) {
            // ★ここで ess1 の ac を更新する
            if (mode_flag >= 0 && mode_flag <= 21) {
                data1.ac = mode_flag;//ess1のacの値を書き換え
                }  
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、モードを変更しました\n", buffer);
        }

        //##############################################
        //ps0~ps21　のようにパラメータ変更コマンドが来た時
        //###########################################
        else if (strncmp(buffer, "ps", 2) == 0) {
            // "ps" の後ろの数字を取得
            int param = atoi(buffer + 2);
            // 範囲チェック
            if (param >= 0 && param <= 21) {
                data1.ps = param;
                
                char buf[16];
                snprintf(buf, sizeof(buf), "30%02d", param);
                data1.lc = atoi(buf);

            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、パラメータを変更しました\n", buffer);
            }
        }

        
        //##########################################
        //JV=3633　のように速度変更コマンドが来た時
        //#############################################
        else if (strncmp(buffer, "JV", 2) == 0) {
                // 形式: "JV=1234" を想定
                float jv = atof(buffer + 3);   // "JV=" を飛ばす
                // ※ ここも速度の上限下限チェックを後で入れる
                data1.JV = jv;  // ★構造体へ保存（目標速度）
                data1.lc = 501111111;//本番では固定小数点方式を使う？一旦固定する

            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、速度目標を変更しました\n", buffer);
            }
        
        //#############################################
        //PA=1000000　のように絶対位置変更コマンドが来た時
        //##############################################
        else if (strncmp(buffer, "PA", 2) == 0) {
                long pa = strtol(buffer + 3, NULL, 10);  // "PA=" を飛ばす
                // 数値の上限と下限を決めておくこと
                data1.PA = (int)pa;  // ★構造体へ保存（絶対位置指令）
                data1.lc = 601111111;//本番では固定小数点方式を使う？一旦固定する
                sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
                printf("コマンド %s を受信し、絶対位置目標を変更しました\n", buffer);
                }

        //##################################################
        //PR=5000000　のように絶対位置変更コマンドが来た時
        //##################################################
        else if (strncmp(buffer, "PA=", 3) == 0) {
                // 形式: "PR=5000000"
                long pr = strtol(buffer + 3, NULL, 10);  // "PR=" を飛ばす
                // ★ ここも上限/下限チェックを入れる予定
                data1.PR = (int)pr;  // ★構造体へ保存（相対位置指令）
                sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
                data1.lc = 611111111;//本番では固定小数点方式を使う？一旦固定する
                printf("コマンド %s を受信し、相対位置目標を変更しました\n", buffer);
                //yoshikawaが実際にサーバ側を作るときは、数値の上限と下限を決めておくこと
                }

        //追加したけど、、
        else if (strncmp(buffer, "BG", 2) == 0) {
                sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
                send_elmo_command(serial_fd, move_command, NULL, 0); 
                data1.lc = 999;//本番では固定小数点方式を使う？一旦固定する
                printf("コマンド %s を受信し、動作を開始しました\n", buffer);
                //yoshikawaが実際にサーバ側を作るときは、数値の上限と下限を決めておくこと
                }

        //############################
        //elmoドライバに対する、ワイルドカードコマンド
        //###########################
        else if (strncmp(buffer, "wc=", 3) == 0) {//ここをwc=にしておく?
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);

            char *body = buffer + 3;  // 例: "MO=1"

            char cmd[256];
            snprintf(cmd, sizeof(cmd), "%s\r", body);  // Elmo用の送信コマンド（\r付き）

            char printer[256] = {0};
            // Elmoへ送信
            send_elmo_command_wc(serial_fd, cmd, printer, sizeof(printer));

            printf("wcコマンドで　%s を処理しました\n", printer);

            sendto(sock, printer, strlen(printer), 0,(struct sockaddr *)&client_addr, addr_len);

            data1.lc = 90;//本番では固定小数点方式を使う？一旦固定する

            // 表示用には \r を除いた文字列を出す
            //printf("ワイルドカードコマンド '%s' を処理しました（送信コマンド: '%s'）\n", buffer, body);
        }



        else {
            //以外のコマンドが来たとき
            char reply[BUFFER_SIZE];

            // ここで文頭に「不明コマンド: 」を付ける
            //snprintf(reply, sizeof(reply), "不明コマンド: %s", buffer);
            snprintf(reply, sizeof(reply),"不明コマンド: %.200s", buffer);


            sendto(sock, reply, strlen(reply), 0,
                (struct sockaddr *)&client_addr, addr_len);
            printf("クライアントから不明コマンド '%s' を受け取りました\n", buffer);
        }

    

    //もし、ctrl + C　が押されたら、そのループを終えた後に、モータをOFFにするっていう文章を作っておきたいな。
    }//ループ終了
    
    finalize_elmo_serial(serial_fd);//Elmoとのserial通信終了
    close(sock);//クライアントとのsocke通信終了


    pthread_join(thread, NULL);

    return 0;

}//main終了



//##########
//UDP通信
//############
int init_udp_server(int port)
{
    int sock;
    struct sockaddr_in server_addr;
    int opt = 1;

    // ソケット作成
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    // ポート再利用
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(sock);
        return -1;
    }

    // アドレス構造体の初期化
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // バインド
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    printf("UDPでポート %d を待ち受けています...\n", PORT);
    return sock;
}



//#################
//rs系列 ElmoドライバにstatusをReadしに行く系列
//################
void rs_handle(int serial_fd, int sock,struct sockaddr_in *client_addr,socklen_t addr_len,const char *buffer,ess1 *data1,ess2 *data2)
{
    // 改行文字を削る用に一応入れてる。
    char cmd[128];
    strncpy(cmd, buffer, sizeof(cmd)-1);
    cmd[sizeof(cmd)-1] = '\0';
    char *p = strpbrk(cmd, "\r\n");
    if (p) *p = '\0';

    // --- ここからは元の if/else をそのまま移植 ----
    char send_buf[1024];
    if (strcmp(cmd, "rs0") == 0) {
        //elmo_update_ess1(serial_fd, data1);// 🔴 Elmo から最新状態を取得
        make_ess1_text(data1, send_buf, sizeof(send_buf));
        sendto(sock, data1, sizeof(*data1), 0,(struct sockaddr *)client_addr, addr_len);
        printf("構造体 ess1 を送信しました（%ld bytes）\n", sizeof(*data1));

        gnss_dummy_update_ess2(data2);     // 🔵 今回はダミーで GNSS データを更新
        sendto(sock, data2, sizeof(*data2), 0,(struct sockaddr *)client_addr, addr_len);
        printf("構造体 ess2 を送信しました（%ld bytes）\n", sizeof(*data2));
    }
    else if (strcmp(cmd, "rs1") == 0) {
        make_ess1_text(data1, send_buf, sizeof(send_buf));
        sendto(sock, data1, sizeof(*data1), 0,(struct sockaddr *)client_addr, addr_len);
        data1->lc=2001;//コマンドコード
        printf("構造体 ess1 を送信しました（%ld bytes）\n", sizeof(*data1));
    }
    else if (strcmp(cmd, "rs2") == 0) {
        gnss_dummy_update_ess2(data2);     // 🔵 今回はダミーで GNSS データを更新
        sendto(sock, data2, sizeof(*data2), 0,(struct sockaddr *)client_addr, addr_len);
        data1->lc=2002;
        printf("構造体 ess2 を送信しました（%ld bytes）\n", sizeof(*data2));
    }
    else if (strcmp(cmd, "rs3") == 0) {
        //elmo_update_ess1(serial_fd, data1);// 🔴 Elmo から最新状態を取得
        make_ess1_text(data1, send_buf, sizeof(send_buf));
        sendto(sock, data1, sizeof(*data1), 0,(struct sockaddr *)client_addr, addr_len);
        printf("構造体 ess1 を送信しました（%ld bytes）\n", sizeof(*data1));

        gnss_dummy_update_ess2(data2);     // 🔵 今回はダミーで GNSS データを更新
        sendto(sock, data2, sizeof(*data2), 0,(struct sockaddr *)client_addr, addr_len);
        data1->lc=2003;
        printf("構造体 ess2 を送信しました（%ld bytes）\n", sizeof(*data2));
    }
    else {
        printf("未知の rs コマンド: %s\n", cmd);
    }
}


void make_ess1_text(const ess1 *d, char *buf, size_t buf_size)
{
    // snprintf を使うことでバッファオーバーフローを防ぐ
    snprintf(buf, buf_size,
        "MO\\n%d;"
        "UM\\n%d;"
        "MF\\n%f;"
        "EC\\n%d;"
        "PX\\n%d;"
        "VX\\n%.6f;"
        "IQ\\n%.6f;"
        "ID\\n%.6f;"
        "MC\\n%.6f;"
        "BV\\n%d;"
        "TI\\n%d;"
        "TC\\n%.6f;"
        "JV\\n%.6f;"
        "PA\\n%d;"
        "PR\\n%d;"
        "ac\\n%d;"
        "ef\\n%d;"
        "ps\\n%d;",
        d->MO, d->UM, d->MF, d->EC, d->PX, d->VX,
        d->IQ, d->ID, d->MC, d->BV, d->TI,
        d->TC, d->JV, d->PA, d->PR, d->ac, d->ef, d->ps
    );
}

//ECが0x00, 0x01, 0x02, 0x04, 0x08



//#######################################
// GNSSコンパス未実装のためのダミー更新関数
// 値は 1.0, 2.0, 3.0, ... の連番
//#########################################
void gnss_dummy_update_ess2(ess2 *st)
{
    static float la0 = 0.0f;
    static float lo0 = 0.0f;
    static float he0 = 0.0f;
    static float ya0 = 0.0f;
    static float pi0 = 0.0f;
    static float ro0 = 0.0f;
    static float ta0 = 0.0f;

    st->la = la0++;  // latitude
    st->lo = lo0++;  // longitude
    st->he = he0++;  // height
    st->ya = ya0++;  // yaw
    st->pi = pi0++;  // pitch
    st->ro = ro0++;  // roll
    st->te = ta0++;  // temperature
}


// ============================================
//  Elmo用 シリアル通信の開始処理
//  ・デバイスopen
//  ・ボーレート設定
//  ・Elmo初期コマンド（motor OFF → torque mode → 各種リード → motor ON）
// ============================================
int init_elmo_serial(const char *device, int baud_rate)
{
    int serial_fd = open_serial_port(device, baud_rate);
    if (serial_fd == -1) {
        return -1;
    }
    fflush(stdout);  // ログをすぐ出す

    // ここから Elmo 初期化シーケンス
    send_elmo_command(serial_fd, motor_off,          NULL, 0); // motor をOFFに
    send_elmo_command(serial_fd, torque_mode,        NULL, 0); // トルクモード(電流モード)
    send_elmo_command(serial_fd, read_current_max,   NULL, 0); // Max current を確認
    //send_elmo_command(serial_fd, set_current_peak,      NULL, 0);
    //send_elmo_command(serial_fd, set_time_peak,         NULL, 0);
    //send_elmo_command(serial_fd, set_current_continuous,NULL, 0);
    //send_elmo_command(serial_fd, set_current_threshold, NULL, 0);
    //send_elmo_command(serial_fd, set_velocity_threshold,NULL, 0);
    //send_elmo_command(serial_fd, set_time_threshold,    NULL, 0);
    send_elmo_command(serial_fd, read_error_message, NULL, 0); // エラーコード初期値を確認
    //send_elmo_command(serial_fd, motor_on,           NULL, 0); // motor ON

    return serial_fd;
}


// ============================================
//  Elmo用 シリアル通信の終了処理
//  ・motor OFF
//  ・バッファに残っている文字の読み出し
//  ・シリアルポート close
// ============================================
void finalize_elmo_serial(int serial_fd)
{
    // 念のため motor OFF
    send_elmo_command(serial_fd, motor_off, NULL, 0);
    // バッファに残っているデータを読む
    printf("=====まだバッファに文字が残っていたら、以下に示す。=====\n");

    char buffer[1024];
    ssize_t bytes_read = read_serial(serial_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Received: %s\n", buffer);
        }
    // シリアルポートを閉じる
    close_serial_port(serial_fd);
}




//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$Elmo sendとか

// "CL[1]=\n1.2;"
//20250708_T.Yoshikawa 追加　
// 制御に必要な数値部分だけ抽出　なんか、VLBIでは[]を除いてない気がする。
double double_extract_number(const char *input) {
    int pos = 0;
    int inside_bracket = 0;//flag
    int copy = 0;//flag
    char output[64] = {0};

    //終端文字「\0」が来るまで繰り返す。...ここはセミコロン「;」で区切ってもいいかもしれん
    for (int i = 0; input[i] != '\0'; i++) {
        char c = input[i];

        // [〜] に挟まれた部分を無視
        if (c == '[') {
            inside_bracket = 1;
            continue;
        }
        if (c == ']') {
            inside_bracket = 0;
            continue;
        }
        if (inside_bracket==1) continue;//inside_bracket==1,つまり[]で囲まれた部分にいる時は、以下の処理をせず、次のループに行く

        // 数値(0~9)と、ピリオド(.), マイナス(-), eとEだけ抽出
        //数値より前に、\nが入った場合は消去する。数値が来た後に、以下の文字「;」 or 「\n」 or「\r」が入った時点で、処理を終了
        if (c == '-' || c == '.' || (c >= '0' && c <= '9') || c == 'e' || c == 'E') {
            output[pos] = c;
            pos += 1;
            //output[pos++] = c; こうやって書くと、コンパイラによっては微妙に時短になるかも
            copy = 1;//一度でも、有効な文字(0~9 or . or - or e or E)が入ったら、copyフラグを1にする。以降は、処理終了文字「; or \r or \n」が来たら、処理を終える
        } else if (copy==1 && (c == ';' || c == '\n' || c == '\r')) {
            break;  // 数値部分終了
        }
    }
    //最後に終端文字入れる。欠落すると、他のメモリの数字読んでくるから気を付けて
    output[pos] = '\0';

    // outputの内容を数値にする
    double result = 0.0;
    if (copy==1) {
        result = atof(output);
    }

    return result;
}




//serialポートをopenする関数
int open_serial_port(const char *device, int baud_rate) {
	int fd = open(device, O_RDWR | O_NOCTTY);
	if (fd == -1) {
		perror("Unable to open serial port");
		return -1;
	}

	struct termios options;
	tcgetattr(fd, &options);

	cfsetispeed(&options, baud_rate);
	cfsetospeed(&options, baud_rate);

	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CREAD | CLOCAL;

	options.c_iflag &= ~(IXON | IXOFF | IXANY);
	options.c_lflag &= ~ICANON;
	options.c_lflag &= ~ECHO;
	options.c_lflag &= ~ECHONL;
	options.c_lflag &= ~ISIG;

	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 50;

	tcsetattr(fd, TCSANOW, &options);
	return fd;
}



//serialポートを閉じる関数
void close_serial_port(int fd) {
	close(fd);
}


//fdにdataを書き込む関数
ssize_t write_serial(int fd, const char *data, size_t len) {
	ssize_t bytes_written = write(fd, data, len);
	if (bytes_written == -1) {
		perror("Write failed");
	}
	return bytes_written;
}

//fdのバッファを読み取る関数
ssize_t read_serial(int fd, char *buffer, size_t len) {
	ssize_t bytes_read = read(fd, buffer, len);
	if (bytes_read == -1) {
		perror("Read failed");
	}
	return bytes_read;
}



// sendしたコマンドを表示する関数。制御文字を見せるように。
void print_send_command(char *command) {
    char buffer[256] = {0};
    int pos = 0;

    for (int i = 0; command[i] != '\0'; i++) {
        unsigned char c = command[i];
        if (c == '\r') {
            pos += sprintf(&buffer[pos], "\\r");
        } else if (c == '\n') {
            pos += sprintf(&buffer[pos], "\\n");
        } else if (c == '\t') {
            pos += sprintf(&buffer[pos], "\\t");
        } else if (c < 32 || c > 126) {
            pos += sprintf(&buffer[pos], "\\x%02X", c);
        } else {
            buffer[pos++] = c;
        }
    }

    //20251114 [SEND]が邪魔なため、いったん消去
    //printf("[SEND] %-12s\t", buffer);  // 左詰め10文字
    fflush(stdout);
}


// RAWデータを16進表示。　左詰め35文字
void debug_hex(const char *label, const char *data, int length) {
    char buffer[256] = {0};
    int pos = 0;
    for (int i = 0; i < length; i++) {
        pos += sprintf(&buffer[pos], "%02X ", (unsigned char)data[i]);
        }
    printf("%s %-35s\t", label, buffer);
}

// ASKIIで表示。左詰め15文字
void print_visible(const char *label, const char *data, int length) {
    char buffer[256] = {0};
    int pos = 0;

    for (int i = 0; i < length; i++) {
        unsigned char c = data[i];
        if (c == '\r') pos += sprintf(&buffer[pos], "\\r");
        else if (c == '\n') pos += sprintf(&buffer[pos], "\\n");
        else if (c < 32 || c > 126) pos += sprintf(&buffer[pos], "\\x%02X", c);
        else buffer[pos++] = c;
    }
    printf("%s %-40s\t", label, buffer);
    printf("\n");
}



//上の関数を改変。*recv_bufを使うことで、ドライバの返事を他の処理に使えるようにした。
int send_elmo_command(int serial_fd, char *command, char *recv_buf, int buf_size) {
    int flag = 0;
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s", command);  // コマンド整形

    //シリアル通信で書き込み
    ssize_t bytes_written = write(serial_fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
          //errorの時は、戻り値を-2など、他のerrorと被らない数値を入れた方がいい。
      	    flag = -2;
	    perror("writeエラー");
	    return flag;
    }

    //送ったコマンドを見る[SEND]
    //print_send_command(command);
    usleep(5000);

    //返事を見る
    if (recv_buf != NULL && buf_size > 0) {//メモリ領域が確保されているか、一応確認
        int bytes_read = read(serial_fd, recv_buf, buf_size - 1);//C配列は0始まりのため、buf_size - 1にしてること注意
        if (bytes_read > 0) {//ドライバから返事が来たら
            recv_buf[bytes_read] = '\0';//ドライバからの返事である「recv_buf」の最後に終端文字を入れる。
            //debug_hex("[RECV_RAW]", recv_buf, bytes_read);//16進数表示、見づらいから一旦comment out
            //print_visible("[RECV]", recv_buf, bytes_read);//RECVを表示
        } else {
	    //errorの時は、戻り値を-1などにした方が分かりやすい
	    flag = -1;
            recv_buf[0] = '\0';  // エラー時は空の文字列を返すように。
            printf("[RECV] (read error)\n");
	    return flag;
        }
    }
    return flag;
}


//上の関数を改変。*recv_bufを使うことで、ドライバの返事を他の処理に使えるようにした。
int send_elmo_command_wc(int serial_fd, char *command, char *recv_buf, int buf_size) {
    int flag = 0;
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s", command);  // コマンド整形

    printf("command=%s\n", command);

    //シリアル通信で書き込み
    ssize_t bytes_written = write(serial_fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
          //errorの時は、戻り値を-2など、他のerrorと被らない数値を入れた方がいい。
      	    flag = -2;
	    perror("writeエラー");
	    return flag;
    }

    //送ったコマンドを見る[SEND]
    print_send_command(command);
    usleep(5000);

    //返事を見る
    if (recv_buf != NULL && buf_size > 0) {//メモリ領域が確保されているか、一応確認
        int bytes_read = read(serial_fd, recv_buf, buf_size - 1);//C配列は0始まりのため、buf_size - 1にしてること注意
        if (bytes_read > 0) {//ドライバから返事が来たら
            recv_buf[bytes_read] = '\0';//ドライバからの返事である「recv_buf」の最後に終端文字を入れる。
            //debug_hex("[RECV_RAW]", recv_buf, bytes_read);//16進数表示、見づらいから一旦comment out
            print_visible("[RECV]", recv_buf, bytes_read);//RECVを表示
        } else {
	    //errorの時は、戻り値を-1などにした方が分かりやすい
	    flag = -1;
            recv_buf[0] = '\0';  // エラー時は空の文字列を返すように。
            printf("[RECV] (read error)\n");
	    return flag;
        }
    }
    return flag;
}
