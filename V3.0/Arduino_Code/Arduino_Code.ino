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
//#include <Wire.h>
#include <SoftwareSerial.h>
//#include <I2C_Anything.h>

#include <TinyGPS.h>
#include <DHT.h>
#include <SFE_BMP180.h>
#include "SparkFunCCS811.h"

#define CCS811_ADDR 0x5A //Default I2C Address

//Definitions
#define gpsRx 6
#define gpsTx 7
#define baudRateGps 9600
#define baudRateDebug 9600
#define dhtPin 8
#define dhtType DHT11
#define timeDelay 1000
//Def to send data
#define sep ','
#define decimalPlaces 4
//I2C Address
#define master 1 //arduino pro mini
#define slave 8 //wemos D1
#define wRx 10
#define wTx 11

//Ports and sensor def
SoftwareSerial gpsSerial(gpsRx,gpsTx);
SoftwareSerial weMoSerial(wRx,wTx);
DHT dht(dhtPin, dhtType);  
CCS811 ccs(CCS811_ADDR);
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
  Serial.println("INIT OK");
}
void loop(){
  readData();
  Serial.println("read OK");
  sendData();
  showData();
  smartdelay(timeDelay);
  //Serial.println("delay OK");
}
void showData(){
    Serial.println ("------------ Sended data -------------- ");
    Serial.println (isnan(lat)?0:lat);
    Serial.println (isnan(lon)?0:lon);
    Serial.println (isnan(alt)?0:alt);
    Serial.println (isnan(t)?0:t);
    Serial.println (isnan(tempCCS)?0:tempCCS);
    Serial.println (isnan(tBMP)?0:tBMP);
    Serial.println (isnan(h)?0:h);
    Serial.println (isnan(hic)?0:hic);
    Serial.println (isnan(co2)?0:co2);
    Serial.println (isnan(tvoc)?0:tvoc);
    Serial.println (isnan(p)?0:p);
}
void sendData(){
  //Wire.write(getData().c_str());
//  I2C_writeAnything (lat);
//  I2C_writeAnything (lon);
//  I2C_writeAnything ((short)alt);
//  I2C_writeAnything ((short)co2);
//  I2C_writeAnything ((short)tvoc);
//  I2C_writeAnything ((short)p);
//  I2C_writeAnything ((byte)t);
//  I2C_writeAnything ((byte)tempCCS);
//  I2C_writeAnything ((byte)tBMP);
//  I2C_writeAnything ((byte)h);
//  I2C_writeAnything ((byte)hic);
String Data = String(lat,decimalPlaces) + sep +  String(lon,decimalPlaces) + sep + String(alt,decimalPlaces) + sep + String(t,decimalPlaces) + String(tempCCS,decimalPlaces)  + sep  + String(tBMP,decimalPlaces) +  sep  +  String(h,decimalPlaces) + sep  +  String(hic,decimalPlaces) + sep  + String(co2,decimalPlaces)  + sep  + String(tvoc,decimalPlaces)  + sep  + String(p,decimalPlaces);
  weMoSerial.println(Data);
  Serial.println("Data writed");
}
void readData()
{
  h = dht.readHumidity();
   t = dht.readTemperature();
   hic = dht.computeHeatIndex(t, h, false);
    //float temp = ccs.calculateTemperature();
    if (ccs.dataAvailable()) {
      ccs.readAlgorithmResults();
      co2 = ccs.getCO2();
      tvoc = ccs.getTVOC();
      tempCCS =  t;
    
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
  //outStr = String(lat,decimalPlaces) + sep +  String(lon,decimalPlaces) + sep + String(alt,decimalPlaces) + sep + String(t,decimalPlaces) + String(tempCCS,decimalPlaces)  + sep  + String(tBMP,decimalPlaces) +  sep  +  String(h,decimalPlaces) + sep  +  String(hic,decimalPlaces) + sep  + String(co2,decimalPlaces)  + sep  + String(tvoc,decimalPlaces)  + sep  + String(p,decimalPlaces) ;
  ccs.setEnvironmentalData(h, t);
}

String getData() {
  Serial.print("out string: ");
  Serial.println(outStr);
  return outStr;
}


bool initSensors(){
  Serial.begin(baudRateDebug);
  Serial.println("INIT start");
  weMoSerial.begin(baudRateDebug);
  CCS811Core::status returnCode = ccs.begin();
  Serial.print("begin exited with: ");
  printDriverError( returnCode );
  if(!pressure.begin()){return false;}  
  dht.begin();
  gpsSerial.begin(baudRateGps);
  //Wire.begin(slave);
  //Wire.onReceive (sendData);
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
//to this function to see what the output was.
void printDriverError( CCS811Core::status errorCode )
{
  switch ( errorCode )
  {
    case CCS811Core::SENSOR_SUCCESS:
      Serial.print("SUCCESS");
      break;
    case CCS811Core::SENSOR_ID_ERROR:
      Serial.print("ID_ERROR");
      break;
    case CCS811Core::SENSOR_I2C_ERROR:
      Serial.print("I2C_ERROR");
      break;
    case CCS811Core::SENSOR_INTERNAL_ERROR:
      Serial.print("INTERNAL_ERROR");
      break;
    case CCS811Core::SENSOR_GENERIC_ERROR:
      Serial.print("GENERIC_ERROR");
      break;
    default:
      Serial.print("Unspecified error.");
  }
}

