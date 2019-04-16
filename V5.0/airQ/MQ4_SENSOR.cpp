#include "MQ4_SENSOR.h"
#include "configuration.h"
#include <GAS_MQ4.h>//Biblioteca de sensor de aire MQ4

GAS_MQ4 mq4Sensor(MQ4Pin);

void MQ4_SENSOR::inicializar()
{
  pinMode(MQ4Pin,INPUT);
}

double MQ4_SENSOR::leerAlcohol(){
  return abs(mq4Sensor.readAlcohol() < 0 ? 0:mq4Sensor.readAlcohol());
}

double MQ4_SENSOR::leerMetano()
{
  return abs(mq4Sensor.readMetane() < 0 ? 0:mq4Sensor.readMetane());
}
