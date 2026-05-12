import sys
import os

def translate_dat_to_log(dat_filename):
    if not os.path.exists(dat_filename):
        print(f"Error: File '{dat_filename}' not found.")
        return

    base_name = os.path.splitext(dat_filename)[0]
    log_filename = base_name + ".log"

    try:
        with open(dat_filename, "rb") as dat_file:
            binary_data = dat_file.read()
            
        with open(log_filename, "w") as log_file:
            line_bytes = []
            for i in range(len(binary_data)):
                b = binary_data[i]
                line_bytes.append(f"{b:02x}")
                
                # 現在のバイトが a4 かつ、一つ前が c5 だった場合に改行
                if b == 0xa4 and i > 0 and binary_data[i-1] == 0xc5:
                    log_file.write(" ".join(line_bytes) + "\n")
                    line_bytes = []
            
            # 最後に改行コードがないデータが残っていれば書き出す
            if line_bytes:
                log_file.write(" ".join(line_bytes) + "\n")

        print(f"Success: Converted to '{log_filename}' (Delimited by c5 a4)")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python trans.py <filename>.dat")
    else:
        translate_dat_to_log(sys.argv[1])