#include "GPS_SENSOR.h"
#include "configuration.h"
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gpsSerial(gpsRx,gpsTx);
TinyGPSPlus gps;

void GPS_SENSOR::inicializar()
{
  gpsSerial.begin(baudRateGps);
}

double GPS_SENSOR::leerLatitud()
{
  return gps.location.lat();
}
double GPS_SENSOR::leerLongitud()
{
  return gps.location.lng();
}
double GPS_SENSOR::leerFecha()
{
  return gps.date.value();
}
void GPS_SENSOR::smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}
