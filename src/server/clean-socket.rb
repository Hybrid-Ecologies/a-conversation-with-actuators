require 'em-websocket'
require 'json'
require 'serialport'

EventMachine.run do
  @channel = EM::Channel.new
  
  # ESTABLISH CONNECTION TO SERIALPORT
  @baud = 9600
  @usb = "COM8"
  @sp = SerialPort.new(@usb, @baud, 8, 1, SerialPort::NONE)
  @sid = nil
  
  EM::defer do
    loop do
      data = @sp.readline("\n") 
      puts "IN: "+ data
      next if !data or data.to_s.size < 1
      @channel.push data
    end
  end
  
  # CONNECT TO WEBSOCKET
  ip="localhost"
  @port = 3013

  print "M: BINDING TO: #{ip} PORT #{@port}\n"
  
  EM::WebSocket.start(:host => ip, :port => @port) do |ws|
    ws.onopen{
      print "M: OPENED!\n"
      @sid = @channel.subscribe { |msg| ws.send msg }
      @channel.push "#{@sid} connected!"
    }
    ws.onclose{
      print "CLOSING\n"
      if @sid
        @channel.unsubscribe(@sid)
      end
    }
    # WHENEVER WEBSOCKET DETECTS MESSAGE, SEND CALL
    ws.onmessage do |msg|
        print "OUT:" + msg
          @sp.write(msg)
        end
    end
  end

  puts "M: SERVER STARTED"
end
