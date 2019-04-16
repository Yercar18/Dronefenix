#include "DHT_SENSOR.h"
#include "configuration.h"
#include "DHT.h"


DHT dht(DHTPIN, DHTTYPE);

void DHT_SENSOR::inicializar(){
  dht.begin();
}

double DHT_SENSOR::leerTemperatura(){
  return dht.readTemperature();
}

double DHT_SENSOR::leerHumedad(){
  return dht.readHumidity();
}
