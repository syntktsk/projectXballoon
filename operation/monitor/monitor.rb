#!/usr/bin/env ruby

require 'anlnext'
$LOAD_PATH.push('/Users/syn/balloon/onboard/ground/build/rubyext')
require 'Balloon'
require 'HSQuickLook'

class MyApp < ANL::ANLApp
  def setup()
    chain HSQuickLook::MongoDBClient
    with_parameters(database: "BACS")
    #with_parameters(host: @host_name, database: @database_name, instantiation: false)
    chain Balloon::ReceiveTelemetry
    with_parameters(
          open_mode: 2, 
          timeout_sec: 1.0, 
          save_command: true,
          num_command_per_file: 1000,
          # 地上系との通信手段の選択
          communication_type: "socket", # "serial" or "socket"
          serial_path: "/tmp/tty.serial",
          OU_socket_serverIp:"192.168.10.101", #mac
          OU_socket_port: 7070,
          chatter: 1, 
          binary_filename_base: "/Users/syn/balloon/data/telemetry/telemetry",
          ) do |m|
            m.set_singleton(1)
        end
    chain Balloon::InterpretTelemetry
    with_parameters(save_telemetry: true, num_telem_per_file: 1000, chatter: 0, binary_filename_base: Dir.home + "/data/telemetry/telemetry")
    chain Balloon::PushToMongoDB
  end
  attr_accessor :serial_path
end

def get_serial_path()
  filename = Dir.home + "/settings/serial/telemetry_port.txt"
  ret = ""
  File.open(filename, mode="rt") do |f|
    f.each_line do |line|
      ret = line.strip
    end
  end
  return ret
end

a = MyApp.new
# a.serial_path = get_serial_path()

a.run(:all, 2)
