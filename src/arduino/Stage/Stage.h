/*------------------------------------------------------------------------
  This file is part of the Adafruit Dot Star library.

  Adafruit Dot Star is free software: you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  Adafruit Dot Star is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with DotStar.  If not, see <http://www.gnu.org/licenses/>.
  ------------------------------------------------------------------------*/

#ifndef _STAGE_H_
#define _STAGE_H_

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_DotStar.h>
#include "Adafruit_MPR121.h"


#define VERBOSE_STAGE 0

// STAGE LIGHTS
#define STAGE_NUMPIXELS 30
#define STAGE_DATAPIN   2
#define STAGE_CLOCKPIN   3

// DOCK DEFINITION
#define NUM_OF_DOCKS 3
#define VERBOSE_STAGE 1

#define SELECT_MULTIPLE 1
#define TOGGLE 2

// STAGE_TYPES
#define NULL_TYPE 0
#define VOLT_TYPE 1
#define PWM_TYPE 2
#define DOTSTAR_I2C_TYPE 3
#define NEOPIXEL_I2C_TYPE 4
#define NUM_OF_STAGES 6
#define SAMPLES 12

typedef struct {
  int pin;
  int a;
  uint32_t color;
  int num_lights;
  int lights[12];
  short stage;
} dock;

typedef struct {
  int rid; // Unique resistive value
  char* name;
  uint32_t color;
  uint32_t selected_color;
  short type;
  short dock_id;
  bool selected;
} stage;



class StageManager {
    public:
        StageManager(Stream & port) : port_ (port){ }
        void begin();
        void process();
        bool active = false;
        int touch_mode = TOGGLE; // SELECT||TOGGLE
        void pwm_send(short k, int voltage);
        void switch_touch(short mode);
        short whats_on_the_dock(short k);
        
        void dock_color2(uint16_t dock_id, int r, int g, int b);
      
    private:
      Stream & port_; 
      Adafruit_DotStar* stage_lights;
      Adafruit_MPR121* cap;
      Adafruit_PWMServoDriver* pwm;

      dock docks[NUM_OF_DOCKS] = {
        {A0, 5, 0xFFFFFF, 9, {0, 1, 2, 3, 4, 5, 6, 21, 22}, 0}, //TOP
        {A1, 7, 0x00FF00, 7, {7, 8, 9, 10, 11, 12, 13}, 0}, //RIGHT
        {A2, 9, 0x0000FF, 7, {14, 15, 16, 17, 18, 19, 20}, 0} // LEFT
      };


      stage stages[NUM_OF_STAGES] ={
        {1024, "-", 0x000000, NULL_TYPE}, // black
        {899, "Vibromotor", 0xFF0000, 0xFFFFFF, VOLT_TYPE, false}, // red
        {233, "Buzzer", 0x00FF00, 0xFFFFFF, VOLT_TYPE, false}, // green
        {597, "AirPump", 0xFF00FF, 0xFFFFFF, PWM_TYPE, false}, // purple
        {0, "Stepper", 0xFF00FF, 0xFFFFFF, PWM_TYPE, false}, // red
        {30, "Heater", 0xFF00FF, 0xFFFFFF, VOLT_TYPE, false} // red
      };

      short which_stage(int pin_value);
      void scan_cap();
      void theatre_pulse(uint8_t pulse_times, float speed);
      void scan_docks();
      void startup_behavior();
      void dock_touch_handler(short k, bool value);
      void dock_change_handler(short k, short prev, short curr);
      void dock_color(uint16_t dock_id, uint32_t to);
      char report[100];
      uint16_t lasttouched = 0;
      uint16_t currtouched = 0;

      void pwm_on(short k);
      void pwm_off(short k);



};

#endif // _STAGE_H_
