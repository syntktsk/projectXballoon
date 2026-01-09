#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::SimpleLoop
    with_parameters(int_v: 2)
  end
end

a = MyApp.new
a.run(100, 1)
