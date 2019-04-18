#include "CCS811_SENSOR.h"
#include "BMP_SENSOR.h"
#include "DHT_SENSOR.h"
#include "MQ4_SENSOR.h"
#include "MQ135_SENSOR.h"
#include "GPS_SENSOR.h"
#include  "PROCESS_DATA.h"
#include "SERIAL_COMMUNICATION.h"
#include "configuration.h"

DHT_SENSOR DHTSensor;
CCS811_SENSOR CCSSensor;
BMP_SENSOR bmpSensor;
MQ4_SENSOR MQ4Sensor;
MQ135_SENSOR MQ135Sensor;
GPS_SENSOR GPSSensor;
PROCESS_DATA data;
SERIAL_COMMUNICATION serialportManager;

double temperatura, humedad, presionAtmosferica, fecha;
int alcohol, tvoc, co2, metano, NH4;
float latitud, longitud;

void setup() {
  // put your setup code here, to run once:
  serialportManager.inicializar();  
  DHTSensor.inicializar();
  if(serDebug) Serial.println(CCSSensor.inicializar());
  if(serDebug) Serial.println(bmpSensor.inicializar());
  MQ4Sensor.inicializar();
  MQ135Sensor.inicializar();
  GPSSensor.inicializar();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  temperatura = DHTSensor.leerTemperatura();
  humedad = DHTSensor.leerHumedad();
  presionAtmosferica = bmpSensor.leerPresionAtmosferica();
  alcohol = MQ4Sensor.leerAlcohol();
  tvoc =  CCSSensor.medirTVOC();
  co2 = CCSSensor.medirCO2();
  metano = MQ4Sensor.leerMetano();
  NH4 =  MQ135Sensor.leerNH4();
  latitud = GPSSensor.leerLatitud();
  longitud = GPSSensor.leerLongitud();
  fecha = GPSSensor.leerFecha();
 
  String lecturas = data.procesarData(temperatura, humedad, presionAtmosferica, alcohol, tvoc, co2, metano, NH4, latitud, longitud, fecha);

  if(lecturas!= "")
  {
    if(serDebug) Serial.println("Se ha enviado la informacion hacia el wemos");
    if(serDebug) Serial.println("Lecturas: " + lecturas);
    serialportManager.enviarWemos(lecturas);
  }
  
  GPSSensor.smartdelay(timeDelay);

}
