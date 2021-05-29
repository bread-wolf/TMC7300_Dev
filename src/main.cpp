#include <Arduino.h>

#include"TMCSerial.hpp"
#include"TMC7300/TMC7300_Map.hpp"

TMCSerial TMC7300(Serial1, 115200, 0);

void setup() {
  TMC7300.begin();
}

void loop() {
  
}