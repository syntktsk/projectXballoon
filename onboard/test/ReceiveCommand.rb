#!/usr/bin/env ruby
require 'anlnext'
require 'Balloon'

class MyApp < ANL::ANLApp
    def setup()
        chain Balloon::ReceiveCommand
            with_parameters(
            # 愛大Raspi
            EU_socket_serverIp: "192.168.10.119", 
            EU_socket_port: 8080,
            # 地上系との通信手段の選択
            communication_type: "socket", # "serial" or "socket"
            serial_path: "/dev/ttyAMA0",
            OU_socket_serverIp: "192.168.10.119", 
            OU_socket_port: 8080,
            chatter: 0, 
            ) do |m|
                m.set_singleton(1)
            end
        end
end

a = MyApp.new
a.run(1000, 1)
