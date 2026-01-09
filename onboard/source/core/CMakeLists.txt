cmake_minimum_required(VERSION 3.0)

include_directories(
  include
  ../BME68x-Sensor-API/include
  ../ICM20948-linux-i2c/src
  ../ICM20948-linux-i2c/I2C/src
  ${ADD_INC_DIR}
  ${BOOST_INC_DIR}
  ${PIGPIO_INC_DIR}
  )

link_directories(
  ${BOOST_LIB_DIR}
  )

set(PX_CORE_CLASSES
  src/SerialCommunication.cc
  src/TelemetryDefinition.cc
  src/CommandDefinition.cc
  src/CRC16.cc
  src/ErrorManager.cc
  src/DateManager.cc
  )

if(PX_USE_PIGPIO)
  set(PX_PIGPIO_CORE_CLASSES
    src/BME680IO.cc
    src/MAX31865IO.cc
    src/SlowADCIO.cc
    )
endif(PX_USE_PIGPIO)

if(PX_USE_WAVEFORMS)
  set(PX_WAVEFORMS_CORE_CLASSES
    src/AnalogDiscoveryIO.cc
    src/DAQIO.cc
    )
endif(PX_USE_WAVEFORMS)

if(PX_USE_ICM20948)
  set(PX_ICM20948_CORE_CLASSES
    src/ICM20948IO.cc
  )
endif(PX_USE_ICM20948)

add_library(PXCore SHARED
  ${PX_CORE_CLASSES}
  ${PX_PIGPIO_CORE_CLASSES}
  ${PX_WAVEFORMS_CORE_CLASSES}
  ${PX_ICM20948_CORE_CLASSES}
  )

target_link_libraries(PXCore
  PXCore
  ${BME68x_LIB}
  ${ADD_LIB}
  ${BOOST_LIB}
  ${PIGPIO_LIB_DAEMON}
  ${PIGPIO_LIB_RT}
  ${WAVEFORMS_LIB}
  ${ICM20948_LIB}
  )

install(TARGETS PXCore LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ProjectX
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)