#include <Wire.h>

#include "CCS811_SENSOR.h"
#include "SparkFunCCS811.h"
#include "configuration.h"

CCS811 ccs(CCS811_ADDR);

String CCS811_SENSOR::inicializar(){
  return ccs.begin()?"CCS Ok":"CCS Init Fail";  
}
double CCS811_SENSOR::medirCO2(){
  double co2 = 0;
  if (ccs.dataAvailable()) {
    co2 = ccs.getCO2();
  }
  return co2;
}

double CCS811_SENSOR::medirTVOC(){
  double tvoc = 0;
  if (ccs.dataAvailable()) {
    tvoc = ccs.getTVOC();
  }
  return tvoc;
}
