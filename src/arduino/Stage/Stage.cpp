#include <Stage.h>
const float pi=3.14;

// API

void StageManager::switch_touch(short mode){
  touch_mode = mode;
}
void StageManager::pwm_send(short k, int voltage){
  if(voltage == 0)
    pwm->setPWM(k, 4095, 0);
  else
    pwm->setPWM(k, 0, voltage);

  int b = map(voltage, 0, 4095, 0, 255);
  dock_color(k, stage_lights->Color(b, b, b));
}


// STAGE MANAGER BUSINESS
void StageManager::begin () {
  port_.println("X: StageManager activated");
  // PWM BEGIN
  pwm = new Adafruit_PWMServoDriver(0x43);
  pwm->begin();
  pwm->setPWMFreq(1600/2);  // 1600 is the maximum PWM frequency
  Wire.setClock(400000);
  
  //  LIGHT BEGIN
  stage_lights = new Adafruit_DotStar(STAGE_NUMPIXELS, STAGE_DATAPIN, STAGE_CLOCKPIN, DOTSTAR_BGR);
  stage_lights->begin();
  stage_lights->show();
  
  // CAP BEGIN
  cap= new Adafruit_MPR121();
  if (!cap->begin(0x5A)) {
    Serial.println("X: MPR121 not found, check wiring?");
    while (1);
  }

  // ID READ BEGIN
  for(int i = 0; i < NUM_OF_DOCKS; i++){
    pinMode(docks[i].pin, INPUT_PULLUP);  // set pull-up on analog pin 0 
  }
  startup_behavior();
  delay(1000);
}

void StageManager::dock_touch_handler(short k, bool up){
  stage* s = &stages[docks[k].stage];
  if(up){
    Serial.print("A: "); Serial.print(docks[k].stage); Serial.println(" UP");  
  }
  else{
    Serial.print("A: "); Serial.print(docks[k].stage); Serial.println(" DOWN");
  }

  switch(touch_mode){
    case TOGGLE:
      if(up) s-> selected = false;
      else s-> selected = true;
      break;
    case SELECT_MULTIPLE:
      if(!up) s-> selected = !s-> selected;
      break;
  }
  if(s-> selected){
    // pwm_on(k);
    // Serial.print("PWM\t");
    // Serial.print(k);
    // Serial.println(" ON");
    // dock_color(k, s-> selected_color);
  }
  else{
    // pwm_off(k);
    // dock_color(k, s-> color);
  }
  
}
void StageManager::dock_change_handler(short k, short prev, short curr){
  if (stages[curr].selected) dock_color(k, stages[curr].selected_color);
  else dock_color(k, stages[curr].color);
  docks[k].stage = curr;
  stages[prev].selected = false;
  cap->begin(0x5A);
  // pwm_off(k);
  Serial.print("D: "); Serial.print(k); Serial.print(" "); Serial.print(prev); Serial.print(" "); Serial.print(curr); Serial.print(" ");Serial.println(stages[curr].color, HEX);
}

void StageManager::scan_cap() {
  currtouched = cap->touched();

  for (short i=0; i<NUM_OF_DOCKS; i++) {
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) dock_touch_handler(i, false);
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) dock_touch_handler(i, true);    
  }
  lasttouched = currtouched;
}

void StageManager::scan_docks(){
  for(int k = 0; k < NUM_OF_DOCKS; k++){
    int samples = 0;
    for(int q = 0; q < SAMPLES; q++){
      samples += analogRead(docks[k].pin);
    }
    int dock_val = samples / SAMPLES;
    short stage_id = which_stage(dock_val);
   
    // snprintf(report, sizeof(report), "%6d: %6d \t", docks[k].pin, dock_val); 
    // Serial.print(report);

    if(docks[k].stage != stage_id){
      dock_change_handler(k, docks[k].stage, stage_id);
    }
  }
  // Serial.println();
  
}


void StageManager::process(){
    scan_docks();
    scan_cap();
} 

void StageManager::pwm_on(short k){
  pwm->setPWM(k, 0, 4095);
}
void StageManager::pwm_off(short k){
  pwm->setPWM(k, 4095, 0);
}  



void StageManager::startup_behavior(){
    // PULSE BLUE 6 TIMES
    theatre_pulse(3, 0.1);

    // FADE TO WHITE
    for(int j = 0; j < 256; j+=1){
        for(int i = 0; i < STAGE_NUMPIXELS; i++){
            stage_lights->setPixelColor(i, j, j, 255);
        }
        stage_lights->show();
    }
    stage_lights->show();
}

void StageManager::theatre_pulse(uint8_t pulse_times, float speed){
    float in, out;

    for (in = 0; in < 6.283 * pulse_times; in += speed){
        out = (int)(sin(in) * 127.5 + 127.5);
        uint32_t c = stage_lights->Color(0, 0, out);
        for(int k = 0; k < NUM_OF_DOCKS; k++) dock_color(k, c);
    }
}
void StageManager::dock_color2(uint16_t dock_id, int a, int b, int c){
  dock_color(dock_id, stage_lights->Color(a, b, c));
}
void StageManager::dock_color(uint16_t dock_id, uint32_t to){
    dock d = docks[dock_id];
    for(int j = 0; j < d.num_lights; j++){
        stage_lights->setPixelColor(d.lights[j], to);
    }
    stage_lights->show();
}

short StageManager::whats_on_the_dock(short k){
  short s = which_stage(docks[k].pin);
  Serial.print("D: "); Serial.print(k); Serial.print(" 0 "); Serial.print(s); Serial.print(" ");Serial.println(stages[s].color, HEX);
}
short StageManager::which_stage(int pin_value){
  int arg_min = -1; 
  int min = 1000000000000;
  
  for(int i = 0; i < NUM_OF_STAGES; i++){
    int diff = abs(stages[i].rid - pin_value);
    if(diff < min){
      min = diff;
      arg_min = i;
    }
  }
  return arg_min;
}