import socket

# ホストとポート番号を設定
HOST = 'localhost'
PORT = 8080

# TCPソケットを作成
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    # ポートを再利用可能にする
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # 指定したホストとポートにバインド
    s.bind((HOST, PORT))
    # 接続待機
    s.listen()
    print(f"サーバーが {HOST}:{PORT} で待機中です...")

    # クライアントからの接続を受け付ける
    conn, addr = s.accept()
    with conn:
        print(f"クライアント {addr} が接続しました。")
        while True:
            # クライアントからデータを受信
            data = conn.recv(1024)
            if not data:
                break
            print(f"受信データ: {data.decode('utf-8')}")
            
print("サーバーを終了します。")