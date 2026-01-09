#!/usr/bin/env ruby
require 'anlnext'
$LOAD_PATH.push('/home/syn/balloon/onboard/build/rubyext')
require 'Balloon'

class MyApp < ANL::ANLApp
    def setup()

      chain Balloon::ReceiveCommand
      with_parameters(
        open_mode: 2,
        timeout_sec: 1, 
        save_command: true,
        num_command_per_file: 1000,
        chatter: 1,
        # 愛大Raspi
        EU_socket_serverIp: "192.168.100.201", #Eu
        EU_socket_port: 9998,
        # 地上系との通信手段の選択
        communication_type: "socket", # "serial" or "socket"
        serial_path: "/dev/ttyAMA0",
        OU_socket_serverIp: "192.168.1.120", #Ou
        OU_socket_port: 9090,
        chatter: 1, 
        binary_filename_base: "/home/syn/data/command/command"
        ) do |m|
          m.set_singleton(1)
      end

      chain Balloon::SPIManager
      with_parameters(spi_flags: (1<<5) + (1<<6) + (1<<7) + 3, baudrate: 1000000) do |m|
        m.set_singleton(0)
      end

      chain Balloon::GetRaspiStatus do |m|
          m.set_singleton(0)
          with_parameters(chatter: 1)
      end

      (1..18).each do |i|
      chain Balloon::GetGL860Data, "getGL860Data_#{i}"
      with_parameters(
        path: "/", 
        chatter: 0
      ) do |m|
        m.set_singleton(0) # SendTelemetryと同じParallel IDで動かす
        end
      end

      chain Balloon::ReceiveEUResponse do |m|
          m.set_singleton(0)
          with_parameters(chatter: 1)
      end

      chain Balloon::SendTelemetry
      with_parameters(
        # 愛大Raspi
        EU_socket_serverIp:"192.168.100.201",#Eu
        EU_socket_port: 9998,
        # 地上系との通信手段の選択
        communication_type: "socket", # "serial" or "socket"
        serial_path: "/dev/ttyAMA0",
        OU_socket_serverIp:"192.168.1.207", #mac
        OU_socket_port: 7070,
        GL860_Data_names:(1..18).map { |i| "getGL860Data_#{i}" },
        binary_filename_base: "/home/syn/data/telemetry/telemetry",
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

# a.modify do |m|
#   # Parallel 0 で動かすもの以外を OFF にする
#   # (例：0番はコマンド系だけにする場合)
#   main_modules.each do |mod|
#     m.get_parallel_module(0, mod).off rescue nil
#   end

#   # Parallel 1 で動かすもの以外を OFF にする
#   # (例：1番はデータ計測・保存系にする場合)
#   command_modules.each do |mod|
#     m.get_parallel_module(1, mod).off rescue nil
#   end
# end

# 実行
a.run(10000000, 10)


# exit_status = a.get_parallel_module(0, "ShutdownSystem").get_result_value("exit_status")
exit_status = 0
puts "exit_status: #{exit_status}"
exit exit_status
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
