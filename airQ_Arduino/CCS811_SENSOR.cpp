#include <Wire.h>

#include "CCS811_SENSOR.h"
#include "SparkFunCCS811.h"
//#include "configuration.h"

CCS811 mySensor(0x5A);

String CCS811_SENSOR::inicializar(){
  Wire.begin();
  CCS811Core::status returnCode = mySensor.begin();
  
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println("CCS Reporta error");
    while (1); //Hang if there was a problem.
  }
  return "CCS811 OK";
  //return ccs.begin()?"CCS Ok":"CCS Init Fail";  
}
void CCS811_SENSOR::cicloCCS()
{
  if (mySensor.dataAvailable()) {
    mySensor.readAlgorithmResults();
    __co2 = mySensor.getCO2();
    __tvoc = mySensor.getTVOC();
    __flag = true;
  }
}
double CCS811_SENSOR::medirCO2(){
  return __co2;
}

double CCS811_SENSOR::medirTVOC(){
  return __tvoc;
}

bool CCS811_SENSOR::getFlagVal(){
  return __flag;
}
void CCS811_SENSOR::resetFlag(){
  __flag = false;
}
