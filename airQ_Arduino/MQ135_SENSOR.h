#ifndef MQ135_SENSOR_H
#define MQ135_SENSOR_H

#include "Arduino.h"

class MQ135_SENSOR {
  public:
      void inicializar();
      double leerNH4();
};


#endif // MQ135_SENSOR_H
