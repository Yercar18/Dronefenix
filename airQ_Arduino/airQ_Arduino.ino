#include <MQUnifiedsensor.h>
#include "CCS811_SENSOR.h"
#include "BMP_SENSOR.h"
#include "DHT_SENSOR.h"
#include "GPS_SENSOR.h"
#include  "PROCESS_DATA.h"
#include "SERIAL_COMMUNICATION.h"
#include "configuration.h"

DHT_SENSOR DHTSensor;
CCS811_SENSOR CCSSensor;
BMP_SENSOR bmpSensor;
GPS_SENSOR GPSSensor;
PROCESS_DATA data;
SERIAL_COMMUNICATION serialportManager;
MQUnifiedsensor MQ4(MQ4Pin, 4);
MQUnifiedsensor MQ135(MQ135Pin, 135);

double alcohol,temperatura, humedad, presionAtmosferica, fecha;
int  tvoc, co2, CH4, NH4;
float latitud, longitud;

void setup() {
  // put your setup code here, to run once:
  serialportManager.inicializar();  
  CCSSensor.inicializar();
  DHTSensor.inicializar();
  if(serDebug) Serial.println(CCSSensor.inicializar());
  if(serDebug) Serial.println(bmpSensor.inicializar());
  MQ4.inicializar();
  MQ135.inicializar();
  GPSSensor.inicializar();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  CCSSensor.cicloCCS();
  GPSSensor.smartdelay(timeDelay);
  MQ4.update();  // Update data, the arduino will be read the voltaje in the analog pin
  MQ135.update(); // Update data, the arduino will be read the voltaje in the analog pin
  
  temperatura = DHTSensor.leerTemperatura();
  humedad = DHTSensor.leerHumedad();
  presionAtmosferica = bmpSensor.leerPresionAtmosferica();
  alcohol = MQ4.readSensor("Alcohol");
  tvoc =  CCSSensor.medirTVOC();
  co2 = CCSSensor.medirCO2();
  CH4 = MQ4.readSensor("CH4");
  NH4 =   MQ135.readSensor("NH4");
  latitud = GPSSensor.leerLatitud();
  longitud = GPSSensor.leerLongitud();
  fecha = GPSSensor.leerFecha();

  String lecturas = data.procesarData(temperatura, humedad, presionAtmosferica, alcohol, tvoc, co2, CH4, NH4, latitud, longitud, fecha);

  if(lecturas!= "" & CCSSensor.getFlagVal())
  {
    if(serDebug) Serial.println("Se ha enviado la informacion hacia el wemos");
    if(serDebug) Serial.println("Lecturas: " + lecturas);
    serialportManager.enviarWemos(lecturas);
    CCSSensor.resetFlag();
  }
}
