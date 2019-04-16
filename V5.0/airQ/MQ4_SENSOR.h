#ifndef MQ4_SENSOR_H
#define MQ4_SENSOR_H

#include "Arduino.h"

class MQ4_SENSOR {
  public:
    void inicializar();
    double leerAlcohol();
    double leerMetano();
};


#endif // MQ4_SENSOR_H
