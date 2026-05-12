/**
 * Command sending app for GRAMS Balloon at Taiki
 *
 * @author Hirokazu Odaka, Shota Arai
 *
 * @date 2023-03-30 | Hirokazu Odaka | prototyping
 */

// --- 必要な標準C++ライブラリのインクルード ---
#include <vector>        // std::vector を使用
#include <string>        // std::string を使用
#include <sstream>       // std::stringstream を使用 (文字列ストリーム、パース用)
#include <istream>       // 入力ストリームの基本機能
#include <fstream>       // std::ifstream を使用 (ファイル入出力)
#include <iomanip>       // std::setw, std::setfill など書式設定用
#include <anlnext/CLIUtility.hh> // ANLNEXTフレームワークのCLI (コマンドラインインターフェース) ユーティリティ (ReadLineなど)
#include "CommandSender.hh"    // コマンドをシリアルポートに送信するクラス
#include "CommandBuilder.hh"   // コマンド名と引数からバイナリデータ（バイト配列）を構築するクラス
#include "CommandSaver.hh" 
#define SERIAL_PORT "/dev/cu.usbserial-FTRTKBUS"

using namespace balloon; 
// 関数宣言
void print_command(const std::vector<std::vector<std::string>>& commands, int run_index);
std::vector<std::vector<std::string>> read_command_plan(const std::string &filename);
void run_command_sequence(const std::vector<std::vector<std::string>> &commands, balloon::CommandSender &sender);
void send_command(const std::vector<std::vector<std::string>> &commands, int run_index, balloon::CommandSender &sender);

const int DISPLAY_NUMBER_PREVIOUS = 10; // 実行の行より前
const int DISPLAY_NUMBER_ADVANCE = 30;  // 実行の行より後
const int DISPLAY_SIZE = 30;

int main(int argc, char **argv)
{
  CommandSender sender; 

  std::cout << "--- BACS Serial Communication Test (1200bps) ---" << std::endl;
  // 2. シリアルポートの初期化
  sender.set_serial_port(SERIAL_PORT);
  if (!sender.open_serial_port()) {
      std::cerr << "Failed to open serial port." << std::endl;
      return 1;
  }
  // ポートが安定するまで待機
  std::cout << "Port opened. Waiting for stability..." << std::endl;
  usleep(1000000); // 1秒

  // 実行時引数の数をチェック
  if (argc != 2) {
    std::cout << "Usage: CommandPlanRunner <command-plan>" << std::endl;
    return 1; 
  }
  const std::string filename(argv[1]); 
  
  const std::vector<std::vector<std::string>> commands = read_command_plan(filename);

  run_command_sequence(commands, sender);


  return 0;
}

// --- 現在のコマンドとその前後を表示する関数 ---
void print_command(const std::vector<std::vector<std::string>>& commands, int run_index)
{  
  const int total_lines = static_cast<int>(commands.size());
  // ターミナルに区切り線とヘッダーを出力
  std::cout << "\n\n#############################################"
            << "\nlines command" << std::endl;
            
  // 画面のスペースを埋めるための空行を出力
  for (int i = 0; i < (DISPLAY_SIZE - total_lines)/2; i++) {
    std::cout << "\n";
  }

  // --- 実行前のコマンド（過去のコマンド）を表示 ---
  for (int i = 0; i < run_index ; i++) {
    // 行番号を出力（3桁、0埋め）
    std::cout << std::setfill('0') << std::right << std::setw(3) <<  i << std::setfill(' ') << "   ";
    
    // コメント行の場合、背景色を黒に設定 (\x1b[40m はターミナルのエスケープシーケンス)
    if (commands[i][0][0] == '#') {
      std::cout << "\x1b[40m";
    }
    // コマンドの各要素（コマンド名、引数）を出力
    for (int j = 0; j < commands[i].size(); j++) {
      std::cout << commands[i][j] << " ";
    }
    // コメント行の背景色を元に戻す
    if (commands[i][0][0] == '#') {
      std::cout << "\x1b[49m";
    }
    std::cout << std::endl;
  }

  // --- 現在実行対象のコマンド行を表示（緑色でハイライト） ---
  std::cout << std::setfill('0') << std::right << std::setw(3) << run_index << std::setfill(' ') << "   ";
  std::cout << "\x1b[42m"; // 背景色を緑に設定
  for (int i = 0; i < commands[run_index].size(); i++) {
    std::cout << commands[run_index][i] << " ";
  }
  std::cout << "\x1b[49m" // 背景色をリセット
            << " <=" << std::endl; // 現在行であることを示すマーカー

  // --- 実行後のコマンド（未来のコマンド）を表示 ---
  for (int i = 1; run_index + i < total_lines; i++) {
    // 行番号を出力
    std::cout << std::setfill('0') << std::right << std::setw(3) << run_index + i << std::setfill(' ') << "   ";
    // コマンドの各要素を出力
    for (int j = 0; j < commands[run_index + i].size(); j++) {
      std::cout << commands[run_index + i][j] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "\x1b[41m" // 背景色を赤に設定
            << "EOF"
            << "\x1b[49m"; // 背景色をリセット


  // ターミナルに区切り線を出力
  std::cout << "#############################################\n\n"
            << std::endl;
}

// --- コマンドプランファイルを読み込み、検証する関数 ---
std::vector<std::vector<std::string>> read_command_plan(const std::string &filename)
{
  CommandBuilder builder; // コマンドのプロパティ（引数の数など）をチェックするヘルパークラス
  std::vector<std::vector<std::string>> commands; // 読み込まれた全コマンドを格納するメインの配列
  
  // ファイルを開く (RAIIのunique_ptrを使用)
  std::unique_ptr<std::ifstream> ifs = std::make_unique<std::ifstream>(filename, std::ios_base::in);
  std::string line;
  
  // ファイルを一行ずつ読み込むループ
  while (std::getline(*ifs, line)) {
    std::string temp;
    std::stringstream linestream{line}; // 各行をストリームに変換
    std::vector<std::string> com_args; // コマンド名と引数を格納する配列
    
    // スペース区切りで要素を抽出するループ
    while (std::getline(linestream, temp, ' ')) {
      com_args.push_back(temp); // 各要素を com_args に追加
    }
    
    // --- 構文チェック ---
    if (com_args.size() == 0) {
      // 空行の場合、便宜的にマーカー（#####）を追加して配列のサイズを保つ
      com_args.push_back("#####");
      // continue; // 元のコードにはcontinueのコメントアウトあり
    }
    else if ((com_args[0][0] != '#')) { // コメント行でない場合のみ検証を実行
      try {
        // コマンド名のプロパティ（引数の数など）を builder でチェック
        builder.get_command_property(com_args[0]);
      }
      catch (CommandException &e) {
        // 未定義のコマンド名の場合、例外を捕捉しエラーメッセージを出して終了
        std::cout << "run_command_plan:Command exception caught: " << e.print() << " in " << com_args[0] << " <- Exit" << std::endl;
        exit(1);
      }
    }
    // 検証が通った行を commands リストの末尾に追加
    commands.push_back(com_args);
  }
  ifs->close(); // ファイルを閉じる
  return commands; // コマンドリストを返す
}

// --- コマンド実行の制御ループ ---
void run_command_sequence(const std::vector<std::vector<std::string>>& commands,CommandSender &sender)
{
  anlnext::ReadLine reader; // ターミナルからユーザー入力を受け付けるためのオブジェクト
  int run_index = 0; // 現在実行対象となっているコマンドの行番号 (インデックス)
  
  // 入力補完の候補を設定 (Tabキーで候補が表示される)
  const std::vector<std::string> completion_candidate = {"send", "back", "skip", "exit", "num"};
  reader.set_completion_candidates(completion_candidate);
  
  // 無限ループ (ユーザーが exit するか、コマンドの終端に達するまで続く)
  while (true) {
    // 1. ファイルの終端に達した場合、ループを終了
    if (run_index >= static_cast<int>(commands.size())) {
      break;
    }
    // 2. 現在行が空行の場合、次の行へスキップ
    if (commands[run_index].size() == 0) {
      run_index++;
      continue;
    }
    // 3. 現在行がコメント行の場合、次の行へスキップ
    if (commands[run_index][0][0] == '#') {
      run_index++;
      continue;
    }
    
    // --- ユーザーとの対話 ---
    
    // 現在のコマンドとその前後をターミナルに表示
    print_command(commands, run_index); 
    
    // ユーザーからの入力を待機 ("INPUT> ")
    const int count = reader.read("INPUT> ");
    std::cout << std::endl;
    const std::string line = reader.str(); // 入力された文字列を取得
    
    if (line == "send") {
      // 'send' が入力された場合、現在のコマンドを実行
      send_command(commands, run_index, sender);
      // run_index++; // 実行後、次の行に進む
      sender.close_serial_port();
      sender.set_serial_port(SERIAL_PORT);
      if (!sender.open_serial_port()) {
          std::cerr << "Failed to open serial port." << std::endl;
          break;
      }
      continue;
    }
    else if (line == "exit") {
      // 'exit' が入力された場合、プログラムを終了
      sender.close_serial_port();
      std::cout << "Exiting the command sender." << std::endl;
      break;
    }
    else if (line == "back") {
      // 'back' が入力された場合、実行可能な前のコマンドに戻る
      int i = run_index;
      while (1) {
        if (i <= 0) { // ファイルの先頭を超えた場合
          std::cout << "Can't back anymore" << std::endl;
          break;
        }
        i--; // インデックスを戻す
        if (commands[i].size() == 0) { // 空行をスキップ
          continue;
        }
        if (commands[i][0][0] == '#') { // コメント行をスキップ
          continue;
        }
        run_index = i; // 実行可能な行に戻る
        break;
      }
      continue;
    }
    else if (!line.empty() && std::all_of(line.begin(), line.end(), ::isdigit)) {
      // 'goto' が入力された場合、行番号の入力を促す
      // std::cout << "Which line do you want to go?" << std::endl;
      // reader.read("INPUT> ");
      // const std::string destination_str = reader.str();
      const int destination = std::stoi(line); // 行番号を整数に変換
      
      // 飛び先が不正な値でないかチェック
      if (destination >= commands.size() || destination < 0) {
        std::cout << "Invalid value" << std::endl;
        continue;
      }
      // 飛び先が空行でないかチェック
      if (commands[destination].size() == 0) {
        std::cout << "This line is blank" << std::endl;
        continue;
      }
      // 飛び先がコメントでないかチェック
      else if (commands[destination][0][0] == '#') {
        std::cout << "This line is comment" << std::endl;
        continue;
      }
      // チェックを通過した場合、実行インデックスを更新
      else {
        std::cout << "Go to line " << destination << std::endl;
        run_index = destination;
        continue;
      }
    }
    else if (line == "skip") {
      // 'skip' が入力された場合、現在の行を飛ばす
      std::cout << "Skipped line " << run_index << std::endl;
      run_index++;
      continue;
    }
    else {
      // どのコマンドにも該当しない場合、エラーを出力
      std::cout << "Error: invalid input." << std::endl;
      continue;
    }
  }
}

// --- コマンドをシリアルポートに送信する関数 ---
void send_command(const std::vector<std::vector<std::string>> &commands, int run_index,CommandSender &sender)
{
  CommandBuilder builder; // バイト配列構築用のヘルパークラス
  std::vector<std::string> args; // 引数を格納する配列
  bool use_default_args = false;
  std::vector<std::string> default_args_strings;

  // 1. コマンド名を取得し、必要な引数の数 (required_args) を取得する
  const std::string command_name = commands[run_index][0];
  const int required_args = builder.get_argnum(command_name);

  const int file_args_count = commands[run_index].size() - 1;

  if (file_args_count > 0) {
      for (int i = 1; i < static_cast<int>(commands[run_index].size()); i++) {
      default_args_strings.push_back(commands[run_index][i]);
      }
      std::cout << "Found " << file_args_count << " arguments in command plan: ";
      for (const auto& arg_str : default_args_strings) {
          std::cout << "[" << arg_str << "] ";
      }
      std::cout << "\nDo you want to use these as default arguments? (y/n)> ";
      
      anlnext::ReadLine reader;
      reader.read(""); 
      std::string response = reader.str();
      
      if (response == "y" || response == "Y") {
          use_default_args = true;
          std::cout << "Using default arguments." << std::endl;
          for (const auto& arg_str : default_args_strings) {
          args.push_back(arg_str); 
          }
      } else {
          std::cout << "Ignoring file arguments. Input required." << std::endl;
      }
  }
  int current_args_count = args.size();
  
  if (current_args_count < required_args) {
    anlnext::ReadLine reader;
    std::cout << "Required arguments: " << required_args << ", Current count: " << current_args_count << std::endl;
    
    // 不足している引数 (i は、これから入力すべき引数のインデックス)
    for (int i = current_args_count; i < required_args; ++i) {
        std::cout << "[input arg for " << command_name << " (Arg " << i + 1 << ")]> ";
        reader.read(""); 
        std::string arg_line = reader.str();
        
        args.push_back(arg_line);
    }
  } else if (current_args_count > required_args) {
      // デフォルト引数が多すぎた場合
      std::cout << "Warning: Too many arguments found (" << current_args_count << "). Using only the first " << required_args << "." << std::endl;
      // (切り詰めるかどうかの判断は、後続の builder.make_byte_array に委ねる)
  }
  
  std::vector<uint8_t> command_bits = builder.make_byte_array(commands[run_index][0], args);
  
  if (sender.CommunicationType() == 1){
    
    int rval = sender.send(command_bits);
    if (rval != static_cast<int>(command_bits.size())) {
      std::cout << "Send Error" << std::endl;
    }
    else {
      std::cout << "Command " << commands[run_index][0] << " sent." << std::endl;
      write_command(command_bits, commands[run_index][0]); 
    }
  }else if(sender.CommunicationType() == 2){
    
    if (!sender.open_socket()) {
    std::cout << "Socket open error -> Skip" << std::endl;
    return;
    }

    int rval = sender.sendBySocket(command_bits);

    if (rval != static_cast<int>(command_bits.size())) {
      std::cout << "Send Error" << std::endl;
    }
    else {
      std::cout << "Command " << commands[run_index][0] << " sent." << std::endl;
      write_command(command_bits, commands[run_index][0]); 
    }
    sender.close_socket();
  }else{
    std::cout << "You should choose Communication Type" << std::endl;
    return;
  }
}
