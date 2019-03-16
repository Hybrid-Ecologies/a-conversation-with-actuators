class ToolController < ApplicationController
  
  # def actuator
  #   @files = get_displays()
  #   @ports = get_ports()
  #   # render :json => @ports
  #   render :layout => "full_screen"
  # end
  def midi
    render :layout => "full_screen"
  end
  def annotator
    render :layout => "full_screen"
  end
  def statemachine
    @files = get_displays()
    @ports = get_ports()
    # render :json => @ports
    render :layout => "full_screen"
  end
  def cwa
    @files = get_displays()
    @ports = get_ports()
    @metadata = Device.data()
    # render :json => @metadata.actuators
    # render :json => get_displays()
    render :layout => "full_screen"
  end
  def aesthetic_actuation
    @files = get_displays()
    @ports = get_ports()
    @metadata = Device.data()
    # render :json => @metadata.actuators
    render :layout => "full_screen"
  end
  def system_control
    @files = get_displays()
    @ports = get_ports()
    # render :json => @ports
    render :layout => "full_screen"
  end
  def designer
    @files = get_displays()
    # render :json => @files
    render :layout => "full_screen"
  end
  def index
  	@files = get_displays()
  	render :layout => "full_screen"
  end


  def displays
    @files = get_displays()
    render :json => @files
  end



  

 def start_server
   # NOTE: currently doesn't work :(
   # dir = system('ruby ./ruby_scripts/ArduinoServer.rb &')
   dir = system("sh start_server.sh")
   render :json => {msg: "I started server", debug: dir}
 end

 # HELPER METHODS
 
  def get_ports
    ports = ["/dev/tty.usb*", "/dev/tty.AestheticAquarium-DevB", "/dev/tty.wc*", "/dev/cu.HC-06-DevB"] #"/dev/tty.HC*", 
    ports.map!{|p| Dir[p]}
    ports.flatten!
  end
end
