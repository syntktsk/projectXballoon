// rs_client.c
//
//クライアント側
//                      ver 1.0       T. Yoshikawa  (2025.11.07)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>  // 時刻表示に必要

#include <sys/time.h>     // struct timeval


#include "elmo_rs_structure.h"
//構造体ess1とess2の定義に使用
//ess1はElmoドライバのstatusを格納
//ess2はGNSSコンパスのstatusを格納

#define SERVER_IP "192.168.100.211"    //本館5FにあるUbuntu計算機をSERVERとした
#define PORT 9998
#define BUFF 1024



//MFのエラー処理用
void print_motor_fault(int mf) {

    if (mf == 0) return; // エラーなし

    printf("\n ⚠ Motor Fault Detected ⚠ \n");

    if (mf & 0x01)
        printf("MF=0x01 エンコーダ信号異常\n");

    if (mf & 0x02)
        printf("MF=0x02 コミュテーション失敗（位相合わせができていない）\n");

    if (mf & 0x04)
        printf("MF=0x04 ホール信号不一致（Illegal Hall）\n");

    if (mf & 0x08)
        printf("MF=0x08 電流上限を超えました（Over Current / MC 限界）\n");

}


//ECエラーコード処理用
void print_error_code(int ec) {

    if (ec == 0) return; // エラー無し

    printf("\n ⚠ Error Code (EC) Detected ⚠ \n");

    if (ec == 1) printf("EC=1 : Reserved（特に意味はありません）\n");
    
    else if (ec == 2) printf("EC=2 : BAD_COMMAND → 存在しないコマンドを送信しました。\n");

    else if (ec == 3) printf("EC=3 : BAD_INDEX → パラメータ番号が範囲外です。\n");
 
    else if (ec == 5) printf("EC=5 : Reserved（特に意味はありません）\n");

    else if (ec == 6) printf("EC=6 : PROGRAM_NOT_RUNNING → 実行中でないプログラムにアクセスしました。\n");

}



//ドライバ情報 
void print_ess1(const ess1 *p) {
    printf("[recv] ess1:\n");
    printf("MO=%d UM=%d MF=%.2f EC=%d PX=%d VX=%.2f IQ=%.2f ID=%.2f MC=%.2f BV=%d TI=%d TC=%.2f JV=%.2f PA=%d PR=%d ac=%d ef=%d ps=%d\n",
        p->MO, p->UM, p->MF, p->EC, p->PX, p->VX, p->IQ, p->ID, p->MC,
        p->BV, p->TI, p->TC, p->JV, p->PA, p->PR, p->ac, p->ef, p->ps);

    
    // MFのエラー処理
    print_motor_fault((int)p->MF);
    // ECのエラー処理
    print_error_code((int)p->EC);

    printf("\n");
}

//VN300情報
void print_ess2(const ess2 *p) {
    printf("[recv] ess2:\n");
    printf("la=%f lo=%f he=%f ya=%f pi=%f ro=%f te=%f\n",
        p->la, p->lo, p->he, p->ya, p->pi, p->ro, p->te);
}





int main() {
    int sock;
    struct sockaddr_in server_addr, local_addr;
    char recv_buf[1024];  //一旦、クライアントPCの汎用バッファに受け取る。　構造体なら構造体用バッファに渡して、文字列なら文字列用バッファに渡す、要改善
    socklen_t local_len = sizeof(local_addr);
   
    ess1 data1; //受信用
    ess2 data2; //受信用

    // UDPソケット作成
    sock = socket(AF_INET, SOCK_DGRAM, 0);//AF_INET:IPv4で、SOCK_DGRANでUDP通信
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    // SO_REUSEADDR を設定（再起動時のエラー防止）
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    //SO_REUSEADDR ポート番号を再利用できるようにしている。これがないと、連続でこのコードを実行するとき、
    //カーネルがそのportは使用中だと勘違いして「bind: Address already in use」って出てくることがある。



    // サーバ情報を設定
    memset(&server_addr, 0, sizeof(server_addr));//server_addr配列を全部0で初期化
    server_addr.sin_family = AF_INET;//IPv4を使っていることを明示
    server_addr.sin_port = htons(PORT);//送り先PORTを設定
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);//inet_ptonでIPとかPORTをバイナリにしてserver_addr.sin_addrに格納している。


    // ===== メインループ（コマンドの入力を行うところ）=====
    while (1) {
        char send_buf[BUFF];  // 入力用バッファ(4Bytes) ※3文字まで
        printf("送信コマンドを入力してください > ");
        scanf("%16s", send_buf);  // 入力（スペース区切りの文字列を受け取る）

        // もし、"q"って文字を打ったら、サーバとの対話を終了する
        if (strcmp(send_buf, "q") == 0) {
            printf("通信を終了します。\n");
            break;
        }



    // サーバへ送信
    sendto(sock, send_buf, strlen(send_buf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // 自分のIPとポートを取得（UDPは送信後にポートが割り当てられるから、whileループの中に置かざるを得ない。。。）
    getsockname(sock, (struct sockaddr *)&local_addr, &local_len);//カーネルが自動的に割り当てた自分のIPとポート番号が local_addr に格納される→なんか0.0.0.0になる？？
    char local_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local_addr.sin_addr, local_ip, sizeof(local_ip));//取得したIPをバイナリ→数値変換
    int local_port = ntohs(local_addr.sin_port);
    printf("自分のIP: %s\n", local_ip);//自分のIPが　0.0.0.0になる。　WSL環境だから？？
    printf("送信元ポート: %d\n", local_port);
    printf("宛先: %s:%d\n", SERVER_IP, PORT);


    // 開始時刻を　yyyy/mm/dd 曜日 hh:mm）
    time_t start_time = time(NULL);
    struct tm *tm_info = localtime(&start_time);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y/%m/%d %a %H:%M", tm_info);
    printf("開始時刻: %s\n", time_str);
    //=====================================================



    // サーバから構造体を受信
    socklen_t addr_len = sizeof(server_addr);
    int recv_len = recvfrom(sock, recv_buf, sizeof(recv_buf) - 1, 0,(struct sockaddr *)&server_addr, &addr_len);//受信データをrecv_bufに格納する


    //recvfrom()に失敗すると、-1を返すため、その判定。
    if (recv_len < 0) {
        printf("⚠ サーバから応答がありません（通信できていません）\n");
        continue; // while に戻る（クライアントは生きている）
    }
    //タイムアウトも設定したいな。。


    //構造体処理
         // ---- rs0 / rs1 / rs2 はバイナリ受信する ----

        //rsを送ったときの処理
        if (strncmp(send_buf, "rs", 2) == 0) {//ここはよくないかも？　クライアントが打った文字じゃなくて、愛媛大学のRas(サーバ)から帰ってきたデータでif文を作りたい。 // "rs" の後ろを int に変換 int value = atoi(msg + 2);

                //rs1を打った時、ess1を受け取る
                if (strcmp(send_buf, "rs1") == 0) {
                    if (recv_len == sizeof(ess1)) {
                        print_ess1((ess1 *)recv_buf);
                    }
                    else printf("[ERROR] rs1 → 未知の受信サイズ\n");
                }

                //rs2を打った時、ess2を受け取る
                else if (strcmp(send_buf, "rs2") == 0) {
                    if (recv_len == sizeof(ess2)) {
                        print_ess2((ess2 *)recv_buf);
                    }
                    else printf("[ERROR] rs2 → 未知の受信サイズ\n");
                }

                //rs0を打った時、ess1とess2を受け取る
                else if (strcmp(send_buf, "rs0") == 0) {
                    if (recv_len == sizeof(ess1)) print_ess1((ess1 *)recv_buf);
                    recv_len = recvfrom(sock, recv_buf, sizeof(recv_buf), 0, NULL, NULL);//ess2の受け取り用に、recvfromしている
                    if (recv_len == sizeof(ess2)) print_ess2((ess2 *)recv_buf);
                }
        
                //rs○○　のように、rs0, rs1, rs2以外のコマンドを打った時の返信
                else printf("不明コマンド%s, rs0 or rs1 or rs2 の打ち間違いですか \n", recv_buf);
        }

        // ---- その他は文字列返信として処理 ----
        else {
            recv_buf[recv_len] = '\0';
            printf("[recv text] %s\n", recv_buf);
        }

    
    if (strcmp(send_buf, "help") == 0) {
        printf(
            "\n=== コマンド一覧（クライアント側から送信）===\n"
            "\n"
            "rs0 : ドライバ状態 (ess1) + GNSSコンパス状態 (ess2) を取得\n"
            "rs1 : ドライバ状態 (ess1) を取得\n"
            "rs2 : GNSSコンパス状態 (ess2) を取得\n"
            "\n"
            "MO0 / MO1 : モータ OFF / ON\n"
            "UM0 / UM1 / UM2 / UM5 : Unit Mode 設定\n"
            "\n"
            "acX   : モードフラグ変更 (例: ac0, ac1 ...)\n"
            "acex  : モード確定 (acX のダブルコマンド)\n"
            "psX   : パラメータセット変更\n"
            "\n"
            "TC=1.5 : トルク指令値変更\n"
            "JV=3600: 速度指令値変更\n"
            "PA=100000 : 絶対位置指令\n"
            "PR=500000 : 相対位置指令\n"
            "\n"
            "ecXXXX : ワイルドカードコマンド（15文字まで）\n"
            "\n"
            "help : このメニューを表示\n"
            "q    : 終了\n"
            "\n===========================================\n\n"
        );
        continue;  // help のときはループ先頭へ戻る
    }

    
    printf("==============================\n");

    }//whileループ終了

    close(sock);
    return 0;

}
