#include "CCS811_SENSOR.h"
#include "DHT_SENSOR.h"
#include "MQ4_SENSOR.h"
#include "MQ135_SENSOR.h"
#include "GPS_SENSOR.h"

DHT_SENSOR DHTSensor;
CCS811_SENSOR CCSSensor;
MQ4_SENSOR MQ4Sensor;
MQ135_SENSOR MQ135Sensor;
GPS_SENSOR GPSSensor;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  DHTSensor.inicializar();
  Serial.println(CCSSensor.inicializar());
  MQ4Sensor.inicializar();
  MQ135Sensor.inicializar();
  GPSSensor.inicializar();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("************inicio****************");

  
  Serial.println(DHTSensor.leerTemperatura());
  Serial.println(DHTSensor.leerHumedad());

  Serial.println(CCSSensor.medirCO2());
  Serial.println(CCSSensor.medirTVOC());

  Serial.println(MQ4Sensor.leerAlcohol());
  Serial.println(MQ4Sensor.leerMetano());
  Serial.println(MQ135Sensor.leerNH4());
  
  Serial.println(GPSSensor.leerLatitud());
  Serial.println(GPSSensor.leerLongitud());
  Serial.println(GPSSensor.leerFecha());

  
  Serial.println("************Fin****************");
  GPSSensor.smartdelay(1000);

}
