#include <SoftwareSerial.h>
#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include "Adafruit_CCS811.h"

#define DHTTYPE DHT11   // DHT 11
#define DHTPIN  3
#define timeDelay 500
#define ledStatus 13
#define taraCCS811 400 //Cantidad de PPM considerado como umbral 0 o tara
#define serialProMini2_TX 8
#define serialProMini2_RX 9
#define sep ","
#define serialPrecision 2
#define baudRateBase 9600 //To pc
#define baudRateTransmit 9600  //To Pro mini 2


bool statusFlag = false; //Bandera usada en las funciones
bool ledFlag; //Bandera usada en los leds que flashean
int statusInt;
int co2, tvoc, tempCCS;
int intentos = 0;
double PresionNivelMar = 1013.25; //presion sobre el nivel del mar en mbar
double h,t,f,hif,hic;
double T, P, p0, a;

SoftwareSerial proMini2(serialProMini2_TX, serialProMini2_RX);
DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 pressure;
Adafruit_CCS811 ccs;

void setup() {
  pinMode(ledStatus,OUTPUT);
  // put your setup code here, to run once:
  while(!inicializacion() && intentos<5){
    //init
    ledFlag = !ledFlag; //Cambio de estado (intermitente)
    digitalWrite(ledStatus,!ledFlag);
    delay(timeDelay);
    intentos += 1;
  }
  digitalWrite(ledStatus,HIGH); //ALL OK
}

void loop() {
  readSensors();
  publishData();
  delay(timeDelay);
}
void publishData(){
  //Publicacion en el puerto de programacion
  Serial.print(T,serialPrecision);
  Serial.print(sep);
  Serial.print(t,serialPrecision);
  Serial.print(sep);
  Serial.print(tempCCS,serialPrecision);
  Serial.print(sep);
  Serial.print(h,serialPrecision);
  Serial.print(sep);
  Serial.print(f,serialPrecision);
  Serial.print(sep);
  Serial.print(tvoc,serialPrecision);
  Serial.print(sep);
  Serial.print(co2,serialPrecision);
  Serial.print(sep);
  Serial.print(co2,serialPrecision);
  Serial.print(sep);
  Serial.println(P,serialPrecision);
  
  //Publicacion en el puerto de comunicaciones con promini2
  /*
    proMini2.print(T,proMini2Precision);
    proMini2.print(sep);
    proMini2.print(t,proMini2Precision);
    proMini2.print(sep);
    proMini2.print(tempCCS,proMini2Precision);
    proMini2.print(sep);
    proMini2.print(h,proMini2Precision);
    proMini2.print(sep);
    proMini2.print(f,proMini2Precision);
    proMini2.print(sep);
    proMini2.print(tvoc,proMini2Precision);
    proMini2.print(sep);
    proMini2.print(co2,proMini2Precision);
    proMini2.print(sep);
    proMini2.print(P,proMini2Precision);
    proMini2.print(sep);
  */

}
void readSensors(){
  //Lectura del DHT11
    h = dht.readHumidity();
    t = dht.readTemperature();
    f = dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("fallo leyendo el sensor DHT11!");
      h = 0; t = 0; f = 0;
    }else{
      hif = dht.computeHeatIndex(f, h);
      hic = dht.computeHeatIndex(t, h, false);
    }   
  //Lectura del CCS_811
  if (ccs.available()) {
    float temp = ccs.calculateTemperature();
    if (!ccs.readData()) {
      co2 = ccs.geteCO2() - taraCCS811;
      tvoc = ccs.getTVOC();
      tempCCS =  temp;
    }
  }
  //Lectura del BMP180
  statusInt = pressure.startTemperature();
  if (statusInt != 0)
  {
    delay(statusInt);
    statusInt = pressure.getTemperature(T);
    if (statusInt != 0)
    {
      //Temperatura esta en la variable T
      statusInt = pressure.getPressure(P, T);
      if (statusInt != 0){
        //Presion esta en la variable P
      }
    }
 }  
}

boolean inicializacion(){
   Serial.println("--------------------------------------------------");
   statusFlag = true;
   // inicializacion del dht para medir temperatura y humedad relativa - prueba de errores en perifericos de medicion
    dht.begin();
    delay(1);
    //Comprobacion del DHT
    h = dht.readHumidity();
    t = dht.readTemperature();
    f = dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("fallo leyendo el sensor DHT11!");
      statusFlag = statusFlag && false;
    }else{
      Serial.println("DHT11 OK");
      statusFlag = statusFlag && true;
    }
    //Comprobacion del BMP180
    if (pressure.begin()){
      Serial.println("BMP180 OK");
      statusFlag = statusFlag && true;
    }else
    {
      Serial.println("BMP180 FAIL");
      statusFlag = statusFlag && false;
    }
    //Comprobacion del ccs811
    if (ccs.begin()) {
        Serial.println("inicializa el CCS_811");
        statusFlag = statusFlag && true;
    }else{
        Serial.println("CCS init Fail");
        statusFlag = statusFlag && false;
    }
    Serial.begin(baudRateBase);
    while (!Serial) {
      Serial.println("Serial inBoard OK");
      ; // wait for serial port to connect. Needed for native USB port only
    }
    proMini2.begin(baudRateTransmit);
    //temporal Mientras desarrollo
    //while (!proMini2) {
      Serial.println("Serial Pro Mini2 OK");
      ; // wait for serial port to connect. Needed for native USB port only
    //}
    Serial.println("in setup, "+intentos);
    return statusFlag;
}

