#include "configuration.h"
#include "PINS.h"

void PINS::inicializar()
{
  pinMode(arduinoResetPin,OUTPUT);
  digitalWrite(arduinoResetPin, LOW);

  digitalWrite(arduinoResetPin, HIGH);
  delay(minDelay*2500);
}
