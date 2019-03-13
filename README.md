# a-conversation-with-actuators
An open-source repository of software and hardware design files to create an actuation prototyping workstation.  


## Choreographer (Web Application)
Requirements:
* Rails 4
* Ruby 2.1.9 or higher

# Server (for all choreographers e.g. web app, midi controller, serial monitor)
Navigate to the server directory and run 
`ruby ArduinoServer.rb`
to start websocket communication between choreographer and theatre.
Default Port: 3015

