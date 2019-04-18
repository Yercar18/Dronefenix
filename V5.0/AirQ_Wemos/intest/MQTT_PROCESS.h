#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "Arduino.h"

class MQTT_PROCESS {
  public:
    void inicializar();
    void setMQTTServer();
    void getPetition();
    boolean confirmIfMqttIsConnectedOrLoopMQTT();
    void callback(char* topic, byte* payload, unsigned int length);
    bool publicarData(double temp, double hum, double presAlt, double alcoholPPM, double TVOC, double CO2, double Metano, double NH4, double latitud, double longitud, double fecha);
 private:
    String __mqttServerConnected;
    int __consecutive;

};


#endif // DHT_SENSOR_H
