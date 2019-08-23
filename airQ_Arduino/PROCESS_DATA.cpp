#include "PROCESS_DATA.h"
#include "configuration.h"

String PROCESS_DATA::procesarData(double temperatura, double humedad, double presionAtmosferica, int alcohol, int tvoc, int co2, int CH4, int NH4, float latitud, float longitud, double fecha)
{
  int deltaTemp = abs(__temperatura - temperatura);
  int deltaHum = abs(__humedad - humedad);
  int deltaPresAt = abs(__presionAtmosferica - presionAtmosferica);
  int deltaAlcohol = abs(__alcohol - alcohol);
  int deltaTVOC = abs(__tvoc - tvoc);
  int deltaCO2 = abs(__co2 - co2);
  int deltaCH4 = abs(__CH4 - CH4);
  int deltaNH4 = abs(__NH4 - NH4);
  unsigned long deltaTime = millis() - __oldTime;
  
  if(deltaTime >= limitTime)
  {
    //Enviar cada 5 minutos aun si no ha cambiado la medicion  
    __oldTime = millis();
    return String(initialCharacter) + sep +  String(isnan(temperatura)?0:temperatura) + sep +  String(isnan(humedad)?0:humedad) + sep + String(isnan(presionAtmosferica)?0:presionAtmosferica) + sep + String(alcohol) +  sep  +  String(isnan(tvoc)?0:tvoc) + sep  +  String(isnan(co2)?0:co2) + sep  + String(CH4)  + sep + String(isnan(NH4)?0:NH4) + sep  + String(isnan(latitud)?0:latitud, decPrecision)  + sep  + String(isnan(longitud)?0:longitud, decPrecision) + sep + String(fecha) + sep + String(endCharacter);   
  }
  else if((deltaTemp > limit) || (deltaHum > limit) || (deltaPresAt > limit) || (deltaAlcohol > limit) || (deltaTVOC > limit) || (deltaCO2 > limit) || (deltaCH4 > limit)|| (deltaNH4 > limit))
  {
    //Enviar en caso que haya cambiado la medicion
    return String(initialCharacter) + sep +  String(isnan(temperatura)?0:temperatura) + sep +  String(isnan(humedad)?0:humedad) + sep + String(isnan(presionAtmosferica)?0:presionAtmosferica) + sep + String(alcohol) +  sep  +  String(isnan(tvoc)?0:tvoc) + sep  +  String(isnan(co2)?0:co2) + sep  + String(CH4)  + sep + String(isnan(NH4)?0:NH4) + sep  + String(isnan(latitud)?0:latitud, decPrecision)  + sep  + String(isnan(longitud)?0:longitud, decPrecision) + sep + String(fecha) + sep + String(endCharacter);   
  }
  else
  {
    return "";
  }
}
