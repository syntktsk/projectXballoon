cmake_minimum_required(VERSION 3.8)
project(ReceiveCommandOnly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE PATH "install prefix")

# --- 1. ???? (???????) ---
set(ONBOARD_ROOT "/home/syn/balloon/onboard")
set(ANLNEXT_INSTALL "/home/syn/ANLNext")???

# --- 2. ???????? ---
include_directories(
  "${ONBOARD_ROOT}/source/core/include"
  "${ONBOARD_ROOT}/source/modules/include"
  "/home/syn/include"
  "/usr/include/aarch64-linux-gnu/ruby-3.1.0" # Ruby????
)

set(SRCS
  "${ONBOARD_ROOT}/source/modules/src/ReceiveCommand.cc"
  "${ONBOARD_ROOT}/source/core/src/SerialCommunication.cc"
  "${ONBOARD_ROOT}/source/core/src/CommandDefinition.cc"
  "${ONBOARD_ROOT}/source/modules/src/ShutdownSystem.cc"
  "${ONBOARD_ROOT}/source/modules/src/RunIDManager.cc"
  #"${ONBOARD_ROOT}/source/core/src/CoreModule.cc"
)

add_library(Balloon SHARED ${SRCS})

target_link_libraries(Balloon PUBLIC
  "/home/syn/lib/libANLNext.so"
  rt
  pthread
)
set_target_properties(Balloon PROPERTIES PREFIX "")