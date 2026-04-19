cmake_minimum_required(VERSION 3.8)

### Initial definition of cmake variables
set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE PATH "install prefix")
set(CMAKE_BUILD_TYPE Release CACHE STRING "build type")
set(CMAKE_CXX_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "CXX_FLAGS for debug")
set(CMAKE_C_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "C_FLAGS for debug")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "CXX_FLAGS for release")
set(CMAKE_C_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "C_FLAGS for release")
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

### Definition of project
project(BACSBalloon)
set(RUBY_CONFIG_FILE_DIR "/usr/include/aarch64-linux-gnu/ruby-3.1.0" CACHE PATH "Ruby config dir")
set(CMAKE_CXX_STANDARD 17)
set(MY_LIBRARY BACSBalloon)
set(USE_RUBY ON)
message("-- Install destination prefix: ${CMAKE_INSTALL_PREFIX}")
list(APPEND CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# ### External libraries
# find_package(mongocxx REQUIRED)
# message("-- mongocxx_INCLUDE_DIRS: ${mongocxx_INCLUDE_DIRS}")
# message("-- mongocxx_LIBRARIES: ${mongocxx_LIBRARIES}")

# MongoDBのインクルードパスを手動で設定
set(MONGOCXX_INC_DIR "/usr/local/include/bsoncxx/v_noabi")
set(MONGOCXX_INC_DIR_2 "/usr/local/include/mongocxx/v_noabi")
set(MONGOCXX_LIB_DIR "/usr/local/lib")

### BOOST ###
find_package(Boost CONFIG 1.80.0)
set(BOOST_INC_DIR ${Boost_INCLUDE_DIRS})
set(BOOST_LIB_DIR ${Boost_LIBRARY_DIRS})
set(BOOST_LIB ${Boost_LIBRARIES})
message("-- BOOST_INC_DIR: ${BOOST_INC_DIR}")
message("-- BOOST_LIB_DIR: ${BOOST_LIB_DIR}")
message("-- BOOST_LIB: ${BOOST_LIB}")
### ANL ###
if(NOT DEFINED ANLNEXT_INSTALL)
  if(DEFINED ENV{ANLNEXT_INSTALL})
    set(ANLNEXT_INSTALL $ENV{ANLNEXT_INSTALL})
  else()
    set(ANLNEXT_INSTALL $ENV{HOME})
  endif()
endif(NOT DEFINED ANLNEXT_INSTALL)
set(ANLNEXT_INC_DIR ${ANLNEXT_INSTALL}/include)
set(ANLNEXT_LIB_DIR ${ANLNEXT_INSTALL}/lib)
set(ANLNEXT_LIB ANLNext)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ANLNEXT_LIB_DIR}/anlnext)
message("-- ANLNEXT_INSTALL = ${ANLNEXT_INSTALL}")

### HSQUICKLOOK ###
if(GB_USE_HSQUICKLOOK)
  find_path(HSQUICKLOOK_INC_DIR
    NAMES hsquicklook/DocumentBuilder.hh
    PATHS $ENV{HSQUICKLOOK_INSTALL}/analyzer/source/include $ENV{HOME}/include /usr/local/include)
  find_library(HSQUICKLOOK_LIB
    NAMES HSQuickLookAnalyzer
    PATHS $ENV{HOME}/lib $ENV{HSQUICKLOOK_INSTALL}/analyzer/lib /usr/local/lib )
  message("-- HSQUICKLOOK_INC_DIR: ${HSQUICKLOOK_INC_DIR}")
  message("-- HSQUICKLOOK_LIB: ${HSQUICKLOOK_LIB}")

  find_package(mongocxx REQUIRED)
  set(MONGODB_LIB mongo::mongocxx_shared)
  message("-- MONGODB_LIB: ${MONGODB_LIB}")
endif(GB_USE_HSQUICKLOOK)

include_directories(
  include
  ${ANLNEXT_INC_DIR}
  ${Boost_INCLUDE_DIRS}
  /usr/local/include/mongocxx/v_noabi
  /usr/local/include/bsoncxx/v_noabi
  /usr/local/include
)

link_directories(
  ${ANLNEXT_LIB_DIR}
  ${BOOST_LIB_DIR}
  ${MONGOCXX_LIB_DIR}
)

file(GLOB ANL_MODULES 
  "source/core/src/*.cc"
  "source/modules/src/*.cc"
  "source/gl860/*.cc"
)

# これで ANL_MODULES に TelemetryDefinition.cc などが入るはずです
add_library(${MY_LIBRARY} SHARED
    ${ANL_MODULES}
)

target_include_directories(${MY_LIBRARY} PUBLIC
  "source/core/include"        # これが必要！
  "source/modules/include"     # これも必要！
  "source/gl860"
  ${Boost_INCLUDE_DIRS}
  ${MONGOCXX_INC_DIR}
)

target_link_libraries(${MY_LIBRARY} PUBLIC
  -Wl,--no-as-needed
  /usr/lib/libpigpiod_if2.so
  /home/syn/lib/libHSQuickLookAnalyzer.so
  ${HSQUICKLOOK_LIB}
  ANLNext
  ${Boost_LIBRARIES}
  mongocxx
  bsoncxx
  rt
  -Wl,--as-needed
)

install(TARGETS ${MY_LIBRARY} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(USE_RUBY)
  add_subdirectory(rubyext)
endif(USE_RUBY)

if(USE_PYTHON)
  add_subdirectory(pyext)
endif(USE_PYTHON)