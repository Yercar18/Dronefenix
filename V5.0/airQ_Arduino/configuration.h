#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Arduino.h"

#define DHTPIN 8
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#define CCS811_ADDR 0x5A //Default I2C Address

#define MQ4Pin A1
#define MQ135Pin A0
#define arduino_sample_rate 1024
#define decPrecision 7

#define gpsRx 6
#define gpsTx 7
#define wRx 10
#define wTx 11
#define baudRateGps 9600
#define baudRateDebug 115200
#define serDebug true

#define initialCharacter "Start"
#define endCharacter "Stop"
#define sep ","

#define timeDelay 500
#define limit 10 //rango -- entre 6 y 12
#define limitTime 5*60*1000 // 5 minutos en milisegundos


#endif // CONFIGURATION_H
