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
#include <SoftwareSerial.h>

#include <TinyGPS.h>
#include <DHT.h>
#include <SFE_BMP180.h>
#include "SparkFunCCS811.h"

#define CCS811_ADDR 0x5A //Default I2C Address

//Definitions
#define gpsRx 6
#define gpsTx 7
#define baudRateGps 9600
#define baudRateDebug 115200
#define dhtPin 8
#define dhtType DHT11
#define timeDelay 30000
//Def to send data
#define sep ','
#define decimalPlaces 4
//I2C Address
#define master 1 //arduino pro mini
#define slave 8 //wemos D1
#define wRx 10
#define wTx 11
#define btnPin 5
//Sensores MQ
#define MQ135 A0
#define MQ4 A1



//Ports and sensor def
SoftwareSerial gpsSerial(gpsRx,gpsTx);
SoftwareSerial weMoSerial(wRx,wTx);
DHT dht(dhtPin, dhtType);  
CCS811 ccs(CCS811_ADDR);
TinyGPS gps;
SFE_BMP180 pressure;

//variables
int mq4Data,mq135Data,year;
double  h,t,T,p,P,tBMP,co2,tvoc,tempCCS,hic,status,alt,temp;
float lat,lon;
unsigned long age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;
String fecha,outStr;  
byte month, day, hour, minute, second, hundredths;


void setup(){
  if(!initSensors()){
    Serial.println("init failed");
  }
  Serial.println("INIT OK");
}
void loop(){
  if(!digitalRead(btnPin)){
    readData();
    Serial.println("read OK");
    sendData();
    showData();
    smartdelay(timeDelay);
  }else{
    smartdelay(300);
  }
  
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
  String Data = String(isnan(lat)?0:lat,decimalPlaces) + sep +  String(isnan(lon)?0:lon,decimalPlaces) + sep + String(isnan(alt)?0:alt,decimalPlaces) + sep + String(isnan(temp)?0:temp,decimalPlaces) +  sep  +  String(isnan(h)?0:h,decimalPlaces) + sep  +  String(isnan(hic)?0:hic,decimalPlaces) + sep  + String(isnan(co2)?0:co2,decimalPlaces)  + sep  + String(isnan(tvoc)?0:tvoc,decimalPlaces)  + sep  + String(isnan(p)?0:p,decimalPlaces) + sep + String(isnan(mq4Data)?0:mq4Data) +  sep + String(isnan(mq135Data)?0:mq135Data) + sep + fecha;
  weMoSerial.println(Data);
  Serial.print("Data writed:");
  Serial.println(Data);
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
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if(!(isnan(t) & isnan(tempCCS) & isnan(tBMP))){
    if(abs((tempCCS-tBMP)+(t-tBMP)+(tempCCS-t))<10){
      temp = (t + tempCCS + tBMP)/3;    
    }else{
      Serial.println("failed 3 meditions");
      Serial.print("ABS: ");Serial.println(abs(tempCCS-tBMP-t));
    }
  }else if(!isnan(t) & isnan(tempCCS)){
    if(abs(tempCCS-t)<10){
      temp = (t + tempCCS)/2;    
    }else{
      Serial.println("failed 2 meditions");      
      Serial.print("ABS: ");Serial.println(abs(tempCCS-t));
    }
  }else if(!isnan(t)){
    temp = t;
  }
  char sz[32];
  sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",month, day, year, hour, minute, second);
  if (!(age == TinyGPS::GPS_INVALID_AGE)){
      fecha = sz;
  }else{
      fecha = "01.01.2001 00:00";
  }
  ccs.setEnvironmentalData(h, t);
  mq135Data = analogRead(MQ135);
  mq4Data = analogRead(MQ4);
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
  pinMode(MQ135,INPUT);
  pinMode(MQ4,INPUT);
  pinMode(btnPin, INPUT_PULLUP);
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

