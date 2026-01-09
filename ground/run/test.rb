#!/usr/bin/env ruby
require 'anlnext'
require 'Balloon'

class MyApp < ANL::ANLApp
    def setup()

        chain Balloon::CommandSender
        with_parameters(serial_path: "/dev/ttyAMA1", chatter: 0, binary_filename_base: "/home/grams/data/command/command") do |m|
            m.set_singleton(1)
        end

        chain Balloon::SPIManager
        with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000) do |m|
          m.set_singleton(0)
        end

        chain Balloon::GetRaspiStatus do |m|
            m.set_singleton(0)
        end

        # chain Balloon::AnalogDiscoveryManager do |m|
        #     m.set_singleton(0)
        # end

        chain Balloon::TelemetryReceiver
        with_parameters(
          serial_path: "/dev/ttyAMA0",
          MeasureTemperature_module_names: ["MeasureTemperatureWithRTDSensor_1",
                                            "MeasureTemperatureWithRTDSensor_2",
                                            "MeasureTemperatureWithRTDSensor_3",
                                            "MeasureTemperatureWithRTDSensor_4",
                                            "MeasureTemperatureWithRTDSensor_5"],
          GetEnvironmentalData_module_names: ["GetEnvironmentalData_1",
                                              "GetEnvironmentalData_2",
                                              "GetEnvironmentalData_3",
                                              "GetEnvironmentalData_4",
                                              "GetEnvironmentalData_5"],
          TPCHVController_module_name: "ControlHighVoltage_TPC",
          PMTHVController_module_name: "ControlHighVoltage_PMT",
          binary_filename_base: "/home/grams/data/telemetry/telemetry",
          chatter: 0
        ) do |m|
            m.set_singleton(0)
        end
    end
end




main_modules = ["SPIManager"]
for i in 1..5 do
  main_modules << "MeasureTemperatureWithRTDSensor_#{i}"
end

for i in 1..5 do
  main_modules << "GetEnvironmentalData_#{i}"
end

main_modules << "MeasureAcceleration" << "GetSlowADCData" << "GetRaspiStatus"
main_modules << "SendTelemetry" << "RunIDManager" << "ShutdownSystem"

command_modules = ["ReceiveCommand"]


a = MyApp.new


a.num_parallels = 2

a.modify do |m|
  main_modules.each do |mod|
    m.get_parallel_module(1, mod).off
  end
  command_modules.each do |mod|
    m.get_parallel_module(0, mod).off
  end
  daq_modules.each do |mod|
    m.get_parallel_module(0, mod).off
    m.get_parallel_module(1, mod).off
  end
  m.get_parallel_module(0, "GetEnvironmentalData_1").off
  m.get_parallel_module(0, "GetEnvironmentalData_2").off
  puts m.class
end

a.run(1000000000, 1)
exit_status = 1


exit_status = a.get_parallel_module(0, "ShutdownSystem").get_result_value("exit_status")


puts "exit_status: #{exit_status}"
exit exit_status

