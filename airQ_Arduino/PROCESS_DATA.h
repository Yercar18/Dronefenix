#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include "Arduino.h"

class PROCESS_DATA {
  public:
    String procesarData(double temperatura, double humedad, double presionAtmosferica, int alcohol, int tvoc, int co2, int metano, int NH4, float latitud, float longitud, double fecha);
  private:
    double __temperatura, __humedad, __presionAtmosferica, __fecha;
    int __alcohol, __tvoc, __metano, __NH4, __co2;
    float __latitud, __longitud;
    unsigned long __oldTime;
};


#endif // DHT_SENSOR_H
