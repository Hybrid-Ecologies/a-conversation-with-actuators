#include <Probe.h>
#include "Stage.h"
#include <Adafruit_DotStar.h>
#define BAUD 9600

Probe probe ("Theatre", Serial, BAUD);
StageManager sm (Serial);

void serial_setup(){
  while (!Serial);
  Serial.begin(BAUD); 
}
void pwm_send(){
  short k = Serial.parseInt();
  int voltage = Serial.parseInt();
  sm.pwm_send(k, voltage);
}
void switch_touch(){
  sm.switch_touch(Serial.parseInt());
}
void whats_on_the_dock(){
  short k = Serial.parseInt();
  sm.whats_on_the_dock(k);
}

void dock_color(){
  uint16_t dock_id = Serial.parseInt();
  int r = Serial.parseInt();
  int g = Serial.parseInt();
  int b = Serial.parseInt();
  sm.dock_color2(dock_id, r, g, b);
}

void setup() {
  serial_setup();
  probe.begin();
  sm.begin();
  probe.add_api('v', pwm_send);
  probe.add_api('s', switch_touch);
  probe.add_api('w', whats_on_the_dock);
  probe.add_api('l', dock_color);
}

void loop() {
  sm.process();
  probe.enable_api();
}




