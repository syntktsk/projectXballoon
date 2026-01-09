include_directories(
  include
  ../core/include
  ../BME68x-Sensor-API/include
  ../ICM20948-linux-i2c/src
  ../ICM20948-linux-i2c/I2C/src
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${BOOST_INC_DIR}
  ${PIGPIO_INC_DIR}
  ${HSQUICKLOOK_INC_DIR}
  )

link_directories(
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  )

set(PX_MODULES
  src/SimpleLoop.cc
  src/ReceiveTelemetry.cc
  src/RunIDManager.cc
  src/ReadTelemetry.cc
  )

if(PX_USE_PIGPIO)
  set(PX_PIGPIO_MODULES
    src/SPIManager.cc
    src/GetEnvironmentalData.cc
    src/MeasureTemperatureWithRTDSensor.cc
    src/GetSlowADCData.cc
    )
endif(PX_USE_PIGPIO)

if(PX_USE_WAVEFORMS)
  set(PX_WAVEFORMS_MODULES
    src/AnalogDiscoveryManager.cc
    src/ControlHighVoltage.cc
    src/ReadWaveform.cc
    src/ReceiveCommand.cc
    src/SendTelemetry.cc
    )
endif(PX_USE_WAVEFORMS)

if(PX_USE_ICM20948)
  set(PX_ICM20948_MODULES
    src/MeasureAcceleration.cc
    )
endif(PX_USE_ICM20948)

if(PX_USE_RASPISYS)
  set(PX_RASPISYS_MODULES
    src/GetRaspiStatus.cc
    src/ShutdownSystem.cc
    )
endif(PX_USE_RASPISYS)

if(PX_USE_HSQUICKLOOK)
  set(PX_HSQUICKLOOK_MODULES
    src/PushToMongoDB.cc
  )
endif(PX_USE_HSQUICKLOOK)

if(PX_USE_ROOT)
  set(PX_ROOT_MODULES
    src/PlotWaveform.cc
    src/InterpretTelemetry.cc
  )
endif(PX_USE_ROOT)

add_library(PXModules SHARED
  ${PX_MODULES}
  ${PX_PIGPIO_MODULES}
  ${PX_WAVEFORMS_MODULES}
  ${PX_ICM20948_MODULES}
  ${PX_RASPISYS_MODULES}
  ${PX_HSQUICKLOOK_MODULES}
  ${PX_ROOT_MODULES}
  )

target_link_libraries(PXModules
  GBCore
  ${BME68x_LIB}
  ${ANLG4_LIB}
  ${ANLNEXT_LIB}
  ${ROOT_LIB}
  ${G4_LIB}
  ${PIGPIO_LIB_DAEMON}
  ${PIGPIO_LIB_RT}
  ${WAVEFORMS_LIB}
  ${ICM20948_LIB}
  ${MONGODB_LIB}
  ${HSQUICKLOOK_LIB}
  )

install(TARGETS PXModules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ProjectX
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)