#ifndef CCS811_SENSOR_H
#define CCS811_SENSOR_H

#include "Arduino.h"

class CCS811_SENSOR {
  public:
    String inicializar();
    double medirCO2();
    double medirTVOC();
};


#endif // CCS811_SENSOR_H
