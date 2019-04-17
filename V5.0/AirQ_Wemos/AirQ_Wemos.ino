#include "SERIAL_COMMUNICATION.h"

SERIAL_COMMUNICATION serial;

unsigned long oldTime;

void setup() {
  serial.inicializar();
 }


void loop() {
  while((millis()-oldTime)<=timeDelay){
      mqttClient.loop(); //Esperar el tiempo prudente
    }
    //debugSerial("He salido del while");
    oldTime = millis();
}
