#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "Arduino.h"

class MQTT_PROCESS {
  public:
    void inicializar();
    void publicarData(double temp, double hum, double presAlt, double alcoholPPM, double TVOC, double CO2, double Metano, double NH4, String latitud, String longitud, String fecha);
    void setMQTTServer();
    void getPetition();
};


#endif // DHT_SENSOR_H
