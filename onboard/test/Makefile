# ==============================================================================
# 🎯 相対パス対応 Makefile
# ------------------------------------------------------------------------------
# 目的: ソースコードが別のディレクトリに分散している場合、相対パスで参照してビルドする。
# ==============================================================================

# ------------------------------------------------------------------------------
# 1. 変数定義
# ------------------------------------------------------------------------------

# ANL-Next のインストール場所を正確に指定してください！
ANL_ROOT = /Users/syn/ANLNext 

# 最終的に生成される Ruby 拡張ライブラリの名前
TARGET = Balloon

# ソースコードディレクトリのパス (Makefile から見た相対パス)
# 例: ソースが src/ にある場合
SRC_DIR = ../modules/src

# コンパイル対象の C++ ソースファイル (.cc)
# ※ 注意: ここではファイル名のみを記述し、パスの付与は後で行います。
SRCS_NAMES = \
	ReceiveCommand.cc \
	CommandDefinition.cc \
	SerialCommunication.cc \
	ShutdownSystem.cc \
	SocketTransceiver_UDP.cc \
	SendTelemetry.cc \
	RunIDManager.cc

# 相対パスを付けたソースファイルリスト
SRCS = $(addprefix $(SRC_DIR)/, $(SRCS_NAMES))

# 生成されるオブジェクトファイル (.o)
# オブジェクトファイルは Makefile と同じディレクトリに生成します
OBJS = $(SRCS_NAMES:.cc=.o) 

# コンパイラ
CXX = g++

# ------------------------------------------------------------------------------
# 2. コンパイラフラグとリンカフラグ
# ------------------------------------------------------------------------------
HEADER_DIR = ../module/include
# -I.: 現在のディレクトリと、ソースディレクトリをヘッダ検索パスに追加
# -I$(ANL_ROOT)/include: ANL-Next のヘッダ検索パス
CXXFLAGS = -c -Wall -std=c++17 \
	-I. \
	-I$(SRC_DIR) \  # 👈 💥 修正点: ソースディレクトリをインクルードパスに追加
	-I$(HEADER_DIR) \
	-I$(ANL_ROOT)/include \
	-I$(shell ruby -rrbconfig -e 'puts RbConfig::CONFIG["archdir"]') \
	-I$(shell ruby -rrbconfig -e 'puts RbConfig::CONFIG["rubyhdrdir"]') \
	-I$(shell ruby -rrbconfig -e 'puts RbConfig::CONFIG["sitearchdir"]')

# リンカフラグ
LDFLAGS = -shared \
	-L$(ANL_ROOT)/lib \
	-lanlnext

# ------------------------------------------------------------------------------
# 3. ターゲット定義 (実行手順)
# ------------------------------------------------------------------------------

all: install

# 最終ライブラリの生成
$(TARGET).so: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# .cc ファイルから .o ファイル (オブジェクトファイル) を生成するルール
# VPATH機能を使わずに、明示的にソースファイルを参照します。
# $<: 依存ファイル (例: src/ReceiveCommand.cc)
# $@: ターゲットファイル (例: ReceiveCommand.o)
%.o: $(SRC_DIR)/%.cc
	# 💥 修正点: コンパイル時に、相対パスを使ってソースファイルを指定
	$(CXX) $(CXXFLAGS) $< -o $@

# インストールターゲット
install: $(TARGET).so
	cp $(TARGET).so $(shell ruby -rrbconfig -e 'puts RbConfig::CONFIG["sitearchdir"]')/$(TARGET).so

# クリーンアップターゲット
clean:
	rm -f $(OBJS) $(TARGET).so