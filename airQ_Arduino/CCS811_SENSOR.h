#ifndef CCS811_SENSOR_H
#define CCS811_SENSOR_H

#include "Arduino.h"

class CCS811_SENSOR {
  public:
    String inicializar();
    double medirCO2();
    double medirTVOC();
    void cicloCCS();
    void resetFlag();
    bool getFlagVal();
  private:
    double __tvoc;
    double __co2;
    boolean __flag;
};


#endif // CCS811_SENSOR_H
