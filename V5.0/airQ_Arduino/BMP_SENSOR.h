#ifndef BMP_SENSOR_H
#define BMP_SENSOR_H

#include "Arduino.h"

class BMP_SENSOR {
  public:
    String inicializar();
    double leerPresionAtmosferica();
};


#endif // BMP_SENSOR_H
