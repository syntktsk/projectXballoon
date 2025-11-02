import socket
import time
import binascii
import numpy as np
import os

# ホストとポート番号を設定
HOST = 'localhost'
PORT = 8080

# ファイルパスとデータサイズを設定
BIN_PATH = '/Users/syn/Downloads/sample_data2/attitude_20250819164700_00001.dat'
DATA_CHUNK_SIZE = 96
# グローバル変数としてデータを格納
telemetry_data = b''
# グローバル変数として現在のデータインデックスを格納
data_index = 0

def load_telemetry_data():
    """バイナリデータを読み込み、グローバル変数に格納する"""
    global telemetry_data
    if os.path.exists(BIN_PATH):
        try:
            with open(BIN_PATH, 'rb') as f:
                telemetry_data = f.read()
            print(f"テレメトリデータをロードしました。全データサイズ: {len(telemetry_data)} バイト")
        except IOError as e:
            print(f"ファイルの読み込みに失敗しました: {e}")
            telemetry_data = b''
    else:
        print(f"指定されたファイルが見つかりません: {BIN_PATH}")
        telemetry_data = b''

def handle_command(byte_data):
    """
    受信したバイト列を解釈し、コマンドを識別する
    '2' (ASCII) コマンドを処理
    """
    print(f"受信したバイトデータ: {byte_data}")

    # ASCIIの '2' が含まれているかチェック
    if b'2' in byte_data:
        return "SUCCESS"
    else:
        try:
            decoded_data = byte_data.decode('utf-8')
            print(f"受信したコマンド (文字列デコード): '{decoded_data}'")
        except UnicodeDecodeError:
            print("警告: 受信したバイトはUTF-8でデコードできませんでした。")
        return "UNKNOWN"

def get_next_data_chunk():
    """
    データ全体の96バイトずつを順に返す
    データが尽きたらNoneを返す
    """
    global telemetry_data, data_index
    if data_index < len(telemetry_data):
        start = data_index
        end = data_index + DATA_CHUNK_SIZE
        chunk = telemetry_data[start:end]
        data_index = end
        return chunk
    else:
        # データが尽きたらインデックスをリセット
        data_index = 0
        return None

# TCPソケットを作成
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen()
    print(f"サーバーが {HOST}:{PORT} で待機中です...")

    load_telemetry_data() # サーバー起動時にデータを事前にロード

    conn, addr = s.accept()
    with conn:
        print(f"クライアント {addr} が接続しました。")
        while True:
            data = conn.recv(1024)
            if not data:
                break
            
            result = handle_command(data)
            print(f"結果: {result}")

            if result == "SUCCESS":
                chunk_to_send = get_next_data_chunk()
                if chunk_to_send:
                    print(f"コマンド '2' を受信しました。{len(chunk_to_send)} バイトのデータを送信します。")
                    conn.sendall(chunk_to_send)
                else:
                    response = "All data sent. Resetting."
                    print(response)
                    conn.sendall(response.encode('utf-8'))
                    
            else:
                response = "Unknown command or decode error."
                print(response)
                conn.sendall(response.encode('utf-8'))
            

print("サーバーを終了します。")