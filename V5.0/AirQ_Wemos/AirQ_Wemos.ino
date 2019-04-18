#include "SERIAL_COMMUNICATION.h"
#include "configuration.h"

SERIAL_COMMUNICATION serial;

unsigned long oldTime;

void setup() {
    serial.inicializar();
    Serial.println("Saliendo del setup");
}


void loop() {
  while((millis()-oldTime)<=timeDelay){
    String informacion = serial.leerArduino();
    Serial.println(informacion);
  }
  if(serDebug) Serial.println("He salido del while");
  oldTime = millis();
}
