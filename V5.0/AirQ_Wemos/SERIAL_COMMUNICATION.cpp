#include "SERIAL_COMMUNICATION.h"
#include "configuration.h"
#include <SoftwareSerial.h>

SoftwareSerial arduinoSerial(arduinoTXPin, arduinoRXPin, false, 256);// SoftwareSerial arduinoSerial(14, 12, false, 256);


void SERIAL_COMMUNICATION::inicializar(){
  arduinoSerial.begin(BAUD_RATE);
  if(serDebug)
  {
    Serial.begin(BAUD_RATE);
  }
}

boolean SERIAL_COMMUNICATION::leerArduino(){
  char inByte;
  arduinoSerial.listen();
  delay(minDelay*100); // Give it time to send a response or you'll get nothing!
  if (arduinoSerial.available() > 0) {
    Data = arduinoSerial.readStringUntil('/r');
    newData = true; 
  }
  else
  {
    newData = false;
  }
  return newData;
}

boolean SERIAL_COMMUNICATION::compararUltimaLectura(){
  return dht.readHumidity();
}
