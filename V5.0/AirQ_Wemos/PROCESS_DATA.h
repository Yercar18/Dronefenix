#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include "Arduino.h"

class PROCESS_DATA {
  public:
    bool procesarInformacion(String Data);
    bool getValue(String data, char separator, int index);
    String getValueStr(String data, char separator, int index);
    double stringToDouble(String & str);
    double leerTemperatura();
    double leerHumedad();
    double leerPresionAtmosferica();
    int leerTVOC();
    int leerCO2();
    int leerMetano();
    int leerAlcohol();
    int leerNH4();
    float leerLatitud();
    float leerLongitud();
    double leerFecha();
    String mensajeSDTabulado();
    String ensamblarMensajeJSON(double temp, double hum, double presAlt, double alcoholPPM, double TVOC, double CO2, double Metano, double NH4, float latitud, float longitud, double fecha);
  private:
    String temp, hum, presAt, alcohol,tvoc, co2, metano, NH4,  latitud, longitud, fecha;
    
};


#endif // MEMORY_ADMINISTRATION_H
