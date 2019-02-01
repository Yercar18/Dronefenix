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
#include <TinyGPS++.h>
#include <DHT.h>
#include <SFE_BMP180.h>
#include "SparkFunCCS811.h"
#include <MQ135.h>//Biblioteca de sensor de aire MQ135
#include <GAS_MQ4.h>//Biblioteca de sensor de aire MQ4


#define CCS811_ADDR 0x5A //Default I2C Address

//Definitions
#define gpsRx 6
#define gpsTx 7
#define baudRateGps 9600
#define baudRateDebug 115200
#define dhtPin 8
#define dhtType DHT11
#define timeDelay 1500
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
#define MQ135Pin A0
#define arduino_sample_rate 1024
#define decPrecision 7
#define MQ4Pin A1



//Ports and sensor def
SoftwareSerial gpsSerial(gpsRx,gpsTx);
SoftwareSerial weMoSerial(wRx,wTx);
DHT dht(dhtPin, dhtType);  
CCS811 ccs(CCS811_ADDR);
TinyGPSPlus gps;
SFE_BMP180 pressure;
GAS_MQ4 mq4Sensor(MQ4Pin);
//variables
double temp = 0, hum = 0, presAt = 0, alcohol = 0,tvoc = 0, co2 = 0, metano = 0, NH4 = 0, latitud = 0, longitud = 0;
String fecha;


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
  
}
void showData(){
    Serial.println ("------------ Sended data -------------- ");
    Serial.println (isnan(temp)?0:temp);
    Serial.println (isnan(hum)?0:hum);
    Serial.println (isnan(presAt)?0:presAt);
    Serial.println (alcohol);
    Serial.println (isnan(tvoc)?0:tvoc);
    Serial.println (isnan(co2)?0:co2);
    Serial.println (metano);    
    Serial.println (isnan(NH4)?0:NH4);
    Serial.println (isnan(latitud)?0:latitud, decPrecision);
    Serial.println (isnan(longitud)?0:longitud, decPrecision);
    Serial.println (fecha);
}
void sendData(){

  //MQ-4 sensor
  alcohol = abs(mq4Sensor.readAlcohol() < 0 ? 0:mq4Sensor.readAlcohol());
  
  //MQ-4 Sensor
  metano =  abs(mq4Sensor.readMetane() < 0 ? 0:mq4Sensor.readMetane());
  
  String Data = String(isnan(temp)?0:temp) + sep +  String(isnan(hum)?0:hum) + sep + String(isnan(presAt)?0:presAt) + sep + String(alcohol) +  sep  +  String(isnan(tvoc)?0:tvoc) + sep  +  String(isnan(co2)?0:co2) + sep  + String(metano)  + sep + String(isnan(NH4)?0:NH4) + sep  + String(isnan(latitud)?0:latitud, decPrecision)  + sep  + String(isnan(longitud)?0:longitud, decPrecision) + sep + String(fecha) ;
  weMoSerial.println(Data);
  Serial.print("Data writed:");
  Serial.println(Data);
}
void readData()
{
  //DHT11 sensor
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  //BMP-180 sensor
  double T, status;
  double P;
    status = pressure.startTemperature();
    if (status != 0)
    {
      delay(status);
      status = pressure.getTemperature(T);
      if (status != 0)
      {
        status = pressure.startPressure(3);
        if (status != 0)
        {
          delay(status);
          status = pressure.getPressure(P, T);
          if (status != 0)
          {
            presAt = P;
          }
        }
      }
    }

  Serial.print("Alcohol: ");
  Serial.println(alcohol);
  
  //CCS TVOC/CO2
  if (ccs.dataAvailable()) {
      ccs.readAlgorithmResults();
      co2 = ccs.getCO2();
      tvoc = ccs.getTVOC();
  }

  //MQ-135 Sensor  
  int adc = analogRead(MQ135Pin);
  NH4 = read_NH4(arduino_sample_rate ,adc);
  
  //gps
  latitud = gps.location.lat();
  longitud = gps.location.lng();
  fecha = gps.date.value();
}


bool initSensors(){
  Wire.begin();
  
  Serial.begin(baudRateDebug);
  Serial.println("INIT start");

  CCS811Core::status returnCode = ccs.begin();
  Serial.print("begin exited with: ");
  printDriverError( returnCode );
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println(".begin() returned with an error.");
    while (1); //Hang if there was a problem.
  } 
  
  weMoSerial.begin(baudRateDebug);

  
  if(!pressure.begin()){return false;} 
  
  dht.begin();
  gpsSerial.begin(baudRateGps);
  pinMode(MQ135Pin,INPUT);
  pinMode(MQ4Pin,INPUT);
  pinMode(btnPin, INPUT_PULLUP);
  //MQ-135 INIT AND CALIBRATE
  int avg = 0;
  for(int i=0; i<10;i++){
  avg += analogRead(MQ135Pin);
  delay(2000);
  }
  avg = avg/10;
  MQ135_init(arduino_sample_rate,avg);
  double R0 = mq4Sensor.calibrate();
  mq4Sensor.setR0(R0);
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
