# CMake の最低限必要なバージョンを指定
cmake_minimum_required(VERSION 3.10)

# プロジェクト名を定義
# これが生成されるライブラリ（GBModules）のルートプロジェクト名となります
project(ProjectX)

# ------------------------------------------------------------------------------
# 1. 変数定義とパス設定
# ------------------------------------------------------------------------------

# ANL-Next のインストールディレクトリ (以前特定したパスを使用)
set(ANLNEXT_ROOT "/Users/syn/ANLNext" CACHE PATH "ANL-Next Installation Directory")

# 共有ライブラリの名前
set(LIBRARY_NAME Modules)

# ReceiveCommand.cc が依存する他の C++ ファイル名リストを定義 (コア依存ファイル)
set(CORE_DEFS
    CommandDefinition.cc
    SerialCommunication.cc
    ShutdownSystem.cc
    SocketTransceiver_UDP.cc
    SendTelemetry.cc
    RunIDManager.cc
)

# ------------------------------------------------------------------------------
# 2. インクルードディレクトリの指定 (ヘッダファイルの場所)
# ------------------------------------------------------------------------------

# include_directories(
#   include                   # CMAKELists.txtがある場所のinclude/
#   ../core/include           # 👈 修正: core/include ディレクトリを追加
#   ${ANLNEXT_INC_DIR}        # 👈 ANL-Nextのヘッダパスを追加
#   ...
# )

include_directories(
  # 💥 自作モジュールのヘッダパス: /User/syn/balloon/onboard/source/modules/include (このファイルから見て相対パスで設定)
  ../source/modules/include
  # 💥 Coreモジュールのヘッダパス: /User/syn/balloon/onboard/source/core/include
  ../source/core/include
  # ANL-Nextのヘッダパス (CMAKEで標準的な変数に変換)
  ${ANLNEXT_ROOT}/include
  /opt/homebrew/include
)

# ------------------------------------------------------------------------------
# 3. コンパイル対象ソースファイルの設定
# ------------------------------------------------------------------------------

# ReceiveCommandモジュールと、それが依存するすべての自作ファイルをリスト化
set(ALL_MODULE_SRCS
  # 💥 必須モジュール
  ../source/modules/src/ReceiveCommand.cc
  
  # 💥 ReceiveCommand が内部で使っている依存ファイル群 (Coreモジュール/Utilityモジュールなど)
  ../source/modules/src/${CORE_DEFS}
  ../source/core/src/CoreModule.cc # Coreの主要な実装ファイルなど、必要に応じて追加
)

# ------------------------------------------------------------------------------
# 4. ライブラリのビルドとリンク
# ------------------------------------------------------------------------------

# 共有ライブラリ (例: Modules.so) を作成
add_library(${LIBRARY_NAME} SHARED
  ${ALL_MODULE_SRCS}
)

# リンクする外部ライブラリの指定
target_link_libraries(${LIBRARY_NAME}
  # 💥 ANL-Next とリンク
  # ${ANLNEXT_LIB_DIR} 変数が存在しないため、直接パスでリンクを指定
  ${ANLNEXT_ROOT}/lib/libANLNext.dylib
  
  # 💥 その他のシステムライブラリ (SerialCommunication や SocketTransceiver に必要とされることが多い)
  # Linux環境の場合: -lrt, -lpthread, -ldlなどが必要になることがあります。
  # macOS環境を想定し、-lpthread は省略。
)

# ------------------------------------------------------------------------------
# 5. インストール
# ------------------------------------------------------------------------------

# ビルドしたライブラリを実行環境のパス（通常は lib ディレクトリ）にインストール
install(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION lib)

# ヘッダファイルもインストール (オプション)
install(DIRECTORY ../modules/include/ DESTINATION include/balloon
  FILES_MATCHING PATTERN "*.hh"
)