#!/usr/bin/env ruby
require 'anlnext'
$LOAD_PATH.push('/home/bacs/balloon/onboard/build/rubyext')
require 'Balloon'

class MyApp < ANL::ANLApp
    def setup()

      chain Balloon::ReceiveCommand
      with_parameters(
        open_mode: 2,
        timeout_sec: 1, 
        save_command: true,
        num_command_per_file: 1000,
        chatter: 10,
        EU_socket_serverIp: "192.168.10.96", #Eu
        EU_socket_port: 9998,
        communication_type: "serial", # "serial" or "socket"
        serial_path: "/dev/ttyAMA5",
        baudrate: 1200.to_i,
        OU_socket_serverIp: "192.168.10.101", #Ou
        OU_socket_port: 9090,
        binary_filename_base: "/home/bacs/data/command/command"
        ) do |m|
          m.set_singleton(1)
      end

      # chain Balloon::SPIManager
      # with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000) do |m|
      #   m.set_singleton(0)
      # end

      chain Balloon::GetRaspiStatus do |m|
          m.set_singleton(0)
          with_parameters(chatter: 1)
      end

      chain Balloon::GetGL860Data
      with_parameters(
        path: "/home/bacs/data/telemetry/ras3", 
        chatter: 0,
        gl860_ip:"192.168.1.100",
        gl860_port:8023,
        num_per_file:1000
        ) do |m|
          m.set_singleton(0) # SendTelemetryと同じParallel IDで動かす
      end

      chain Balloon::ReceiveEUResponse do |m|
      with_parameters(filepath:"/home/bacs/data/telemetry/ras2")
          m.set_singleton(0)
          with_parameters(chatter: 1)
      end
      
      chain Balloon::RelayControl do |m|
      with_parameters(gpio_list:[23, 24])
        m.set_singleton(0)
      end

      chain Balloon::SendTelemetry
      with_parameters(
        EU_socket_serverIp:"192.168.10.96",#Eu
        EU_socket_port: 9998,
        communication_type: "serial", #地上との通信手段 "serial" or "socket"
        serial_path: "/dev/ttyAMA2",
        baudrate: 57600,
        OU_socket_serverIp:"192.168.96", #mac
        OU_socket_port: 7070,
        GL860_Data_names: ["GetGL860Data"],
        binary_filename_base: "/home/bacs/data/telemetry/telemetry",
        save_path:"/home/bacs/data/telemetry/ess/",
        chatter: 0
      ) do |m|
          m.set_singleton(0)
      end

      chain Balloon::RunIDManager do |m|
          m.set_singleton(0)
      end

      chain Balloon::ShutdownSystem do |m|
          m.set_singleton(0)
      end
    end
end



main_modules = [
  "SPIManager",
  "GetRaspiStatus",
  "GetGL860Data",
  "SendTelemetry",
  "RunIDManager",
  "ShutdownSystem"
]

command_modules = [
  "ReceiveCommand",
]

a = MyApp.new
a.num_parallels = 1 # 0番を通信/コマンド系、1番を計測/保存系にする

# 実行
a.run(10000000, 10)


# exit_status = a.get_parallel_module(0, "ShutdownSystem").get_result_value("exit_status")
exit_status = 0
puts "exit_status: #{exit_status}"
exit exit_status
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
