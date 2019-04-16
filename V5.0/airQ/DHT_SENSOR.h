#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "Arduino.h"

class DHT_SENSOR {
  public:
    void inicializar();
    double leerTemperatura();
    double leerHumedad();
};


#endif // DHT_SENSOR_H
