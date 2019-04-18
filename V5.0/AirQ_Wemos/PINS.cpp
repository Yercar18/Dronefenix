#include "configuration.h"
#include "PINS.h"

void PINS::inicializar()
{
  pinMode(arduinoResetPin,OUTPUT);
  digitalWrite(arduinoResetPin, LOW);
  delay(100);
  digitalWrite(arduinoResetPin, HIGH);
  delay(minDelay);
}
