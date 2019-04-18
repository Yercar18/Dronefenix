#include "BMP_SENSOR.h"
#include "configuration.h"
#include <SFE_BMP180.h>


SFE_BMP180 pressure;

String BMP_SENSOR::inicializar(){
 return pressure.begin()?"BMP180 OK":"BMP180 fallo";
}

double BMP_SENSOR::leerPresionAtmosferica(){
  double T, status;
  double P;
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P, T);
      }
    }
  }
  return P;
}
