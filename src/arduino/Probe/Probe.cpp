#include <Probe.h>

void Probe::begin () {
  port_.println("X: Probe v.1");
  port_.print("X: I am the ");
  port_.print(name_);
  port_.println(" API!");
  port_.print("X: Listening at ");
  port_.print(baud_);
  port_.println(" baud.");

  // Dynamic array of function pointers
  int capacity = 16;  // initial capacity
  int n = 0;          // initial size
  myFuncs = calloc(capacity, sizeof(functype));  // heap dynamic array
  myPrefixes = calloc(capacity, sizeof(char*));  // heap dynamic array
}


void Probe::findCommandEnd(){
  buffer = ' ';
  while(buffer != '\n'){
    if(port_.available() > 0){
      buffer = port_.read();
    } 
  }
}

void Probe::add_api(char msg, functype func){
  port_.print("X: Registering ");
  port_.print(pos_);
  port_.print(" ");
  port_.println(msg);
  myPrefixes[pos_] = msg;
  myFuncs[pos_] = *func;
  pos_++;
}


void Probe::api_call(char prefix){

   switch (prefix) {
    case 'p': 
      port_.println("X: COMMANDS IN MEMORY -->");
      for(int i = 0; i < 10; i++){
        port_.print("X: ");
        port_.print(i);
        port_.print(" ");
        port_.println(myPrefixes[i]);
        if(prefix == myPrefixes[i]){ myFuncs[i]();} 
      }
      // myFuncs[0]();
      findCommandEnd();
      break;
    default: 
      bool found = false;
      // SEARCH FOR FUNCTION
      for(int i = 0; i < 10; i++){
        if(prefix == myPrefixes[i]){ 
          myFuncs[i]();
          found = true;
        } 
      }
      if(!found){
        port_.print("X:");
        port_.print(" API command '");
        port_.print(prefix);
        port_.println("' does not exist!");
      }

      findCommandEnd(); 
      break;
  }
}
void Probe::enable_api(){
  if (port_.available() > 0) {
    prefix = port_.read();
    api_call(prefix);
  }
}
