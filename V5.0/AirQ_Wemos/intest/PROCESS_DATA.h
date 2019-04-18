#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include "Arduino.h"

class PROCESS_DATA {
  public:
    bool processData(String Data);
    bool getValue(String data, char separator, int index);
    String getValueStr(String data, char separator, int index);
    double stringToDouble(String & str);
    String tempT, humT, presAtT, alcoholT,tvocT, co2T, metanoT, latitudT, longitudT, fechaT, temp, hum, presAt, alcohol,tvoc, co2, metano, NH4,  latitud, longitud, fecha;
    
};


#endif // MEMORY_ADMINISTRATION_H
