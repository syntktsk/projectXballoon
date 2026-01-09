import socket
import sys
import struct # バイナリデータのデコードに使用

# --- 設定 ---
HOST = '127.0.0.1'  # ローカルホストでの待機
PORT = 9090       # C++クライアントと一致させるポート番号
BUFFER_SIZE = 4096 # 一度に受信する最大データサイズ (バイト)

def run_udp_server():
    # 1. ソケットの作成 (AF_INET: IPv4, SOCK_DGRAM: UDP)
    try:
        udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except socket.error as e:
        print(f"🚨 ソケット作成エラー: {e}")
        sys.exit(1)

    # 2. ソケットをアドレスにバインド (結びつけ)
    try:
        udp_socket.bind((HOST, PORT))
        print(f"✅ UDPサーバーを起動しました: {HOST}:{PORT}")
    except socket.error as e:
        print(f"🚨 バインドエラー: アドレスが既に使用されている可能性があります。 {e}")
        udp_socket.close()
        sys.exit(1)

    print("⏳ データグラムの受信を待機中です...")

    while True:
        try:
            # 3. データを受信 (data: バイト列, addr: 送信元アドレス)
            data, addr = udp_socket.recvfrom(BUFFER_SIZE)
            
            if data:
                n = len(data)
                print("\n===========================================")
                print(f"📥 受信元アドレス: {addr[0]}:{addr[1]}")
                print(f"📥 受信したバイト列 (長さ: {n} バイト):")
                print(data) 
                
                # --- コマンドフレームの解析処理 ---
                
                # 最低限の長さチェック: [EB90] + [CODE(2)] + [ARGC(2)] + [CRC(2)] + [C5A4] = 10バイト
                if n < 10:
                    print(f"⚠️ 受信バイト列が短すぎます (最低10バイト必要)。現在の長さ: {n}")
                    continue
                
                # バイナリデータはビッグエンディアン (>) を使用していると仮定
                
                # 1. ヘッダーとフッターの確認 (2バイトの符号なし整数としてデコード)
                start_code = struct.unpack('>H', data[0:2])[0]
                stop_code = struct.unpack('>H', data[-2:])[0]
                
                # 2. コマンドコード (2 bytes) と 引数カウント (2 bytes) の取得
                # インデックス 2 から 6 の 4バイトを '2H' (2つの uint16_t) としてデコード
                # code は index 2, argnum は index 4
                code, argnum = struct.unpack('>2H', data[2:6])
                
                # 3. CRC (2 bytes) の取得
                crc_attached = struct.unpack('>H', data[-4:-2])[0]

                # 4. 引数部分の抽出
                # Argはインデックス 6 から CRCの直前 (n-4) まで
                arg_start = 6
                arg_end = n - 4
                
                arg_string = "引数なし"
                if arg_end > arg_start:
                    arg_bytes = data[arg_start:arg_end]
                    
                    try:
                        # 0x00 (ヌル文字) を含む文字列としてデコード
                        arg_string_raw = arg_bytes.decode('utf-8')
                        
                        # C++側の16バイトパディングに対応するため、最初のヌル文字以降を削除
                        arg_string = arg_string_raw.split('\x00', 1)[0]
                        if not arg_string:
                             arg_string = "（引数データはあったが、ヌル文字のみ）"

                    except UnicodeDecodeError:
                        arg_string = "デコード失敗（非UTF-8データ）"

                print("\n========== コマンドフレーム解析結果 ==========")
                print(f"**ヘッダー** (0xEB90): 0x{start_code:X}")
                print(f"**コマンドID** (uint16): {code}")
                print(f"**引数カウント** (uint16): {argnum}")
                print(f"**CRC16** (uint16): 0x{crc_attached:X}")
                print(f"**フッター** (0xC5A4): 0x{stop_code:X}")
                print("----------------------------------")
                print(f"**引数文字列** (復元): {arg_string}")
                print("====================================")


            else:
                print("⚠️ データグラムが空でした。")

        except KeyboardInterrupt:
            print("\n🛑 ユーザーによってサーバーが停止されました。")
            break
        except Exception as e:
            print(f"🚨 予期せぬエラーが発生しました: {e}")
            break

    # サーバーソケットを閉じる
    udp_socket.close()
    print("サーバーを終了します。")

if __name__ == '__main__':
    run_udp_server()