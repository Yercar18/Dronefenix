#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include "Arduino.h"

class PROCESS_DATA {
  public:
    void setFecha(String fechaIn);
    bool procesarInformacion(String Data);
    bool getValue(String data, char tabuladorarator, int index);
    String getValueStr(String data, char tabuladorarator, int index);
    double stringToDouble(String & str);
    double leerTemperatura();
    double leerHumedad();
    double leerPresionAtmosferica();
    int leerTVOC();
    int leerCO2();
    int leerCH4();
    int leerAlcohol();
    int leerNH4();
    float leerLatitud();
    float leerLongitud();
    double leerFecha();
    String mensajeSDTabulado();
    boolean SAVEJSON(String JSON);
    String getJSON(int index);
    void resetMsgQeueCounter();
    void setTimeToWait(int time);
    int generateRandom();
    int getIndex();
    String ensamblarMensajeJSON(int temp, int hum, int presAlt, int alcoholPPM, int TVOC, int CO2, int CH4, int NH4, float latitud, float longitud, String fecha);
  private:
    String temp, hum, presAt, alcohol,tvoc, co2, CH4, NH4,  latitud, longitud, fecha;
    String msqQeue[60];
    int msqQeueCounter = 1;
    float timeWait;
    unsigned long oldTimeSended;
    
};


#endif // MEMORY_ADMINISTRATION_H
