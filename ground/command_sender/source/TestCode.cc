/**
 * Serial Communication Test for BACS (1200bps persistence test)
 */

#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include "CommandSender.hh"
#include "CommandBuilder.hh"

using namespace balloon;

int main() {
    CommandBuilder builder;
    
    // 1. Staticインスタンスの作成（実質的なシングルトン）
    CommandSender sender; 

    std::cout << "--- BACS Serial Communication Test (1200bps) ---" << std::endl;

    // 2. シリアルポートの初期化
    sender.set_serial_port("/dev/cu.usbserial-BG03Q92N"); // cuデバイスを推奨
    if (!sender.open_serial_port()) {
        std::cerr << "Failed to open serial port." << std::endl;
        return 1;
    }
    
    // ポートが安定するまで待機（昨日の教訓）
    std::cout << "Port opened. Waiting for stability..." << std::endl;
    usleep(1000000); // 1秒

    // 3. テスト送信ループ
    std::string input;
    while (true) {
        std::cout << "\nEnter command name to send (or 'exit' to quit): ";
        std::cin >> input;

        if (input == "exit") break;

        try {
            // テスト用の引数（空、または必要に応じて追加）
            std::vector<std::string> args; 
            // コマンドのプロパティを確認して引数が必要ならダミーを入れる
            int req_args = builder.get_argnum(input);
            for(int i=0; i<req_args; ++i) args.push_back("0");

            std::vector<uint8_t> command_bits = builder.make_byte_array(input, args);

            std::cout << "Sending " << command_bits.size() << " bytes..." << std::endl;
            
            // 送信実行
            int rval = sender.send(command_bits);

            if (rval == static_cast<int>(command_bits.size())) {
                std::cout << "Success: Data pushed to OS buffer." << std::endl;
            } else {
                std::cerr << "Send Error!" << std::endl;
            }

            // 1200bpsの余韻（これがないと、連続送信時にバッファが溢れる可能性がある）
            usleep(500000); 

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    // 4. 最後にだけ Close する
    std::cout << "Closing port and exiting..." << std::endl;
    sender.close_serial_port();

    return 0;
}