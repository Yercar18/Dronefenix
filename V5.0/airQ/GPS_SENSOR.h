#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include "Arduino.h"

class GPS_SENSOR {
  public:
      void inicializar();
      double leerLatitud();
      double leerLongitud();
      double leerFecha();
      void smartdelay(unsigned long ms);
};


#endif // GPS_SENSOR_H
