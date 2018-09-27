/*
  ENSAMBLE DE UN DRONE BASADO EN UN SISTEMA SENSORIAL Y MULTIESPECTRAL PARA MEDIR VARIABLES  METEOROLÓGICOS
  Este programa permite generar un archivo excel en el cual
  se ven reflejados la temperatura , humedad , presion atmosferica
  y la geolocalizacion ( latitud , longitud en la cual se encuentra el dispositivo ).
   Materiales
  - Sensor CCS811
  - Sensor DHT11
  - Sensor BMP180
   Cableado
  Gps TX --> Arduino `Pro mini RX 7
  Gps RX --> Arduino Pro mini TX 8
  BMP 180 conexion I2C , SDA and SCL.
  DHT 11 --> Pin 7 Arduino Mega
  by Yerson Carrillo in collaboration with Miguel Angel Califa Urquiza
  Modified 14 January 2017
  by Miguel Califa
  redesigned to redistribution
  Modified 05 Febrary 2018
  Update to Github Repository
  Modified 27 September 2018
  Update pcb and software
*/
//Library
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <TinyGPS.h>
#include <DHT.h>
#include <Adafruit_CCS811.h>
#include <SFE_BMP180.h>

//Definitions
#define gpsRx 7
#define gpsTx 8
#define baudRateGps 9600
#define baudRateDebug 9600
#define dhtPin 9
#define dhtType DHT11
#define timeDelay 1000
//Def to send data
#define sep ','
#define decimalPlaces 4



//Ports and sensor def
SoftwareSerial gpsSerial(gpsRx,gpsTx);
DHT dht(dhtPin, dhtType);  
Adafruit_CCS811 ccs;
TinyGPS gps;
SFE_BMP180 pressure;

//variables
double  h,t,T,p,P,tBMP,co2,tvoc,tempCCS,hic,status,alt;
float lat,lon;
unsigned long age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;
String outStr;  

void setup(){
  if(!initSensors()){
    Serial.println("init failed");
  }
}
void loop(){
  readData();
  smartdelay(timeDelay);
}
void requestEvent()
{
  String data = getData();
  int str_len = data.length() + 1;  
  uint8_t Buffer[str_len];
  data.toCharArray(Buffer, str_len);
  Wire.write(Buffer,str_len);
}
void readData()
{
  h = dht.readHumidity();
   t = dht.readTemperature();
   hic = dht.computeHeatIndex(t, h, false);
   if (ccs.available()) {
    float temp = ccs.calculateTemperature();
    if (!ccs.readData()) {
      co2 = ccs.geteCO2();
      tvoc = ccs.getTVOC();
      tempCCS =  temp;
    }
  }
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      tBMP = T;
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          p = P;
        }
      }
    }
  }
  gps.f_get_position(&lat, &lon, &age);
  gps.stats(&chars, &sentences, &failed);
  alt = gps.f_altitude();
  // latitud,longitud,altitud,temperatura (DHT), temperatura(CCS), temperatura (BMP), humedad (DHT11), hic (DHT11), co2 (CCS811), tvoc(CCS811), presionAtmosferica (BMP180)
  outStr = String(lat,decimalPlaces) + sep +  String(lon,decimalPlaces) + sep + String(alt,decimalPlaces) + sep + String(t,decimalPlaces) + String(tempCCS,decimalPlaces)  + sep  + String(tBMP,decimalPlaces) +  sep  +  String(h,decimalPlaces) + sep  +  String(hic,decimalPlaces) + sep  + String(co2,decimalPlaces)  + sep  + String(tvoc,decimalPlaces)  + sep  + String(p,decimalPlaces) ;
}

String getData() {
  Serial.print("out string: ");
  Serial.println(outStr);
  return outStr;
}


bool initSensors(){
  if(!ccs.begin()){return false;}
  if(!pressure.begin()){return false;}  
  Wire.begin(8);
  Wire.onRequest(requestEvent); // register event
  dht.begin();
  Serial.begin(baudRateDebug);
  gpsSerial.begin(baudRateGps);
  return true;
}
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}
