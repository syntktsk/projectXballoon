// ess_client.c
//
//サーバー側
//                      ver 1.0       T. Yoshikawa  (2025.11.10)
//                   

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
// #include "elmo_rs_structure.h"
#include "eu_struct.h"

#define PORT 9998
#define BUFFER_SIZE 1024

int main() {

    //Elmoドライバから読み取った値が、愛媛大のラズパイ(サーバー側)に保存されている。
    ess1 data1 = {
        .MO = 0,      
        .UM = 1,
        .OB = 1,
        .MF = 0,
        .EC = 2,
        .PX = 3,
        .VX = 4.56,
        .IQ = 0.12,
        .ID = 9.87,
        .MC = 7.89,
        .BV = 11,
        .TI = 22,
        .TC = 0.0f,
        .JV = 0.0f,
        .PA = 0,
        .PR = 0,
        .ac = 0,
        .ef = 0,
        .ps = 0,
        .en = 1,
        .az = 32.5f,
        .hi = 245,
        .lc="THIS_IS_TEST."
    };

    //GNSSコンパスから読み取った値が、愛媛大のラズパイ(サーバー側)に保存されている。
    ess2 data2 = {
        .la = 1.0f,  // 緯度
        .lo = 2.0f,  // 経度
        .he = 3.0f,  // 海抜高度
        .ya = 4.0f,  // yaw
        .pi = 5.0f,  // pitch
        .ro = 6.0f,  // roll
        .te = 7.0f   // 温度
    };
    //fはfloat(4Bytes)型ということを表す


    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    //  UDPソケット作成
    sock = socket(AF_INET, SOCK_DGRAM, 0);//AF_INET:IPv4で、SOCK_DGRANでUDP通信
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    //SO_REUSEADDRでポート再利用を許可
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    //SO_REUSEADDR ポート番号を再利用できるようにしている。これがないと、連続でこのコードを実行するとき、
    //カーネルがそのportは使用中だと勘違いして「bind: Address already in use」って出てくることがある。


    //サーバ情報設定
    memset(&server_addr, 0, sizeof(server_addr));//server_addr配列を全部0で初期化
    server_addr.sin_family = AF_INET;//IPv4を使用
    server_addr.sin_port = htons(PORT);//送り先PORTを設定
    server_addr.sin_addr.s_addr = INADDR_ANY;//「inet_addr("192.168.100.211")」から変更した

    //バインド（ポートを開く）
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sock);
        exit(1);
    }

    printf("UDPでポート %d を待ち受けています...\n", PORT);


    //メインループ：受信を待機
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // クライアントからデータ受信
        int recv_len = recvfrom(sock, buffer, BUFFER_SIZE, 0,
                                (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom");
            break;
        }

        // クライアント情報を取得
        char client_ip[INET_ADDRSTRLEN];//clientのIPを取得
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));//clientのIPをバイナリにした
        printf("\n受信 from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        printf("受信データ: %s\n", buffer);


        // rs1がきたとき、ess1を返す
        if (strcmp(buffer, "rs1") == 0) {//bufferに入っている文字列が"rs"であるときに、ess構造体
            //clientにdata(ess変数型)を送る
            sendto(sock, &data1, sizeof(data1), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("構造体 ess1 を送信しました（%ld bytes）\n", sizeof(data1));
        }

        //rs2が来たとき、ess2を返す
        else if (strcmp(buffer, "rs2") == 0) {
            sendto(sock, &data2, sizeof(data2), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("構造体 ess2 を送信しました（%ld bytes）\n", sizeof(data2));
        }


        //rs0がきたとき、ess0を返す
        else if (strcmp(buffer, "rs0") == 0) {
            // data1 を送信
            sendto(sock, &data1, sizeof(data1), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("構造体 ess1 を送信しました（%ld bytes）\n", sizeof(data1));

            // data2 を送信
            sendto(sock, &data2, sizeof(data2), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("構造体 ess2 を送信しました（%ld bytes）\n", sizeof(data2));
        }

        //モータON/OFF　MO=1でモータON, MO=0でモータOFF
        else if (strncmp(buffer, "MO", 2) == 0) {
            // "MO" の後ろ（index 2 以降）を整数に変換
            int value = atoi(buffer + 2);

            // クライアントにエコー返信
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);

            // 判定
            if (value == 1) printf("モータ → ON（Enable）にしました\n");
            else if (value == 0) printf("モータ → OFF（Disable）にしました\n");
            else printf("MOコマンドの値が不正です（MO0 または MO1 を指定してください）\n");
        }


        else if (strncmp(buffer, "UM", 2) == 0) {
            // "MO" の後ろ（index 2 以降）を整数に変換
            int value = atoi(buffer + 2);

            // クライアントにエコー返信
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);

            // 判定
            if (value == 0) printf("MO=0です。\n");
            else if (value == 1) printf("UM=1にしました\n");
            else if (value == 2) printf("UM=2にしました\n");
            else if (value == 5) printf("UM=5にしました\n");
            else printf("UMの値が不正です。\n");
        }


        //ac0~ac21　のようにモード変更コマンドが来た時
        else if (strncmp(buffer, "ac", 2) == 0) {
            // 同じ文字列をそのまま返す
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、モードフラグを変更しました\n", buffer);
            }

        //モードを実際に変更するコマンド(ac0のダブルコマンド)
        else if (strcmp(buffer, "acex") == 0) {
            // 同じ文字列をそのまま返す
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、モードを変更しました\n", buffer);
        }

        //ps0~ps21　のようにパラメータ変更コマンドが来た時
        else if (strncmp(buffer, "ps", 2) == 0) {
            // 同じ文字列をそのまま返す
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、パラメータを変更しました\n", buffer);
            }

        //TC=1.2　のように電流変更コマンドが来た時
        else if (strncmp(buffer, "TC", 2) == 0) {
            // 同じ文字列をそのまま返す
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、電流目標を変更しました\n", buffer);
            }
        
        //JV=3633　のように電流変更コマンドが来た時
        else if (strncmp(buffer, "JV", 2) == 0) {
            // 同じ文字列をそのまま返す
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、速度目標を変更しました\n", buffer);
            }
        
        //PA=1000000　のように絶対位置変更コマンドが来た時
        else if (strncmp(buffer, "PA", 2) == 0) {
            // 同じ文字列をそのまま返す
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、絶対位置目標を変更しました\n", buffer);
            }

        //PR=5000000　のように絶対位置変更コマンドが来た時
        else if (strncmp(buffer, "PA", 2) == 0) {
            // 同じ文字列をそのまま返す
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("コマンド %s を受信し、相対位置目標を変更しました\n", buffer);
            //yoshikawaが実際にサーバ側を作るときは、数値の上限と下限を決めておくこと
            }

        else if (strncmp(buffer, "ec", 2) == 0) {
            // 同じ文字列をそのまま返す
            sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&client_addr, addr_len);
            printf("ワイルドカードコマンド %s を受信し、処理しました\n", buffer);
            }

        else {
            // rs 以外のコマンドが来たとき
            char reply[BUFFER_SIZE];

            // ここで文頭に「不明コマンド: 」を付ける
            snprintf(reply, sizeof(reply), "不明コマンド: %s", buffer);

            sendto(sock, reply, strlen(reply), 0,
                (struct sockaddr *)&client_addr, addr_len);
            printf("クライアントから不明コマンド '%s' を受け取りました\n", buffer);
        }

    }
    
    close(sock);
    return 0;
}
