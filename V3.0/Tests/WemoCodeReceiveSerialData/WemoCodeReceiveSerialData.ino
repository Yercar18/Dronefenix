
#include <SoftwareSerial.h>

#define BAUD_RATE 115200

SoftwareSerial arduinoSerial(14, 12, false, 256);

void setup() {
  Serial.begin(BAUD_RATE);
  arduinoSerial.begin(BAUD_RATE);

  Serial.println("\nSoftware serial test started");

  for (char ch = ' '; ch <= 'z'; ch++) {
    arduinoSerial.write(ch);
  }
  arduinoSerial.println("");

}

void loop() {
  while (arduinoSerial.available() > 0) {
    Serial.write(arduinoSerial.read());
    yield();
  }
  while (Serial.available() > 0) {
    arduinoSerial.write(Serial.read());
    yield();
  }

}
