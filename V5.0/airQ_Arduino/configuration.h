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
#define baudRateGps 9600


#endif // CONFIGURATION_H
