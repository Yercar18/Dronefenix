#include "MQ135_SENSOR.h"
#include "configuration.h"
#include <MQ135.h>//Biblioteca de sensor de aire MQ135

void MQ135_SENSOR::inicializar()
{
  pinMode(MQ135Pin,INPUT);
}

double MQ135_SENSOR::leerNH4()
{
  //MQ-135 Sensor  
  int adc = analogRead(MQ135Pin);
  return read_NH4(arduino_sample_rate ,adc);
}
