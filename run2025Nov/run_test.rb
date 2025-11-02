#!/usr/bin/env ruby

require 'anlnext'
require 'Balloon'

class MyApp < ANL::ANLApp
  def setup()
    chain Balloon::MongoDBClient
    chain Balloon::WaitFor
    with_parameters(time: 500000)
    chain Balloon::TelemetryReceiver_UDP
    
  end
  attr_accessor :host_name, :database_name
end

a = MyApp.new
a.run(100000)