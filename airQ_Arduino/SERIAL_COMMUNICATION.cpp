#include "SERIAL_COMMUNICATION.h"
#include "configuration.h"
#include <SoftwareSerial.h>

SoftwareSerial weMoSerial(wRx,wTx);


void SERIAL_COMMUNICATION::inicializar(){
  if(serDebug) Serial.begin(baudRateDebug);
  weMoSerial.begin(baudRateDebug);
}

void SERIAL_COMMUNICATION::enviarWemos(String mensaje){
  weMoSerial.println(mensaje);
}
