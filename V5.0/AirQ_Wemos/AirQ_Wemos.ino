#include "SERIAL_COMMUNICATION.h"
#include "configuration.h"

SERIAL_COMMUNICATION serial;

unsigned long oldTime;

void setup() {
    serial.inicializar();
}


void loop() {
  while((millis()-oldTime)<=timeDelay){
  
  }
  //if(serDebug) Serial.println("He salido del while");
  oldTime = millis();
}
