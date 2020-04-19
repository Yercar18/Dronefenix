#include <MQUnifiedsensor.h>
/************************Hardware Related Macros************************************/
#define         Board                   ("Arduino pro mini")
/***********************Software Related Macros************************************/
#define         Type                    ("MQ-4") //MQ4
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10) // For arduino UNO/MEGA/NANO
#define         RatioMQ4CleanAir        (4.4) //RS / R0 = 60 ppm 
#define         RatioMQ135CleanAir      (3.6)//RS / R0 = 3.6 ppm  
/*****************************Globals***********************************************/
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
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ4Pin, Type);
MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ135Pin, Type);

double alcohol, fecha;
int  CH4, NH4;
float latitud, longitud;

void setup() {
  // put your setup code here, to run once:
  serialportManager.inicializar();  
  CCSSensor.inicializar();
  DHTSensor.inicializar();
  if(serDebug) Serial.println(CCSSensor.inicializar());
  if(serDebug) Serial.println(bmpSensor.inicializar());
  MQ4.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  
  MQ4.init(); 
  MQ135.init(); 
  
  /* 
    //If the RL value is different from 10K please assign your RL value with the following method:
    MQ4.setRL(10);
  */
  
  /* 
    //If the RL value is different from 10K please assign your RL value with the following method:
    MQ135.setRL(10);
  */

  /*****************************  MQ CAlibration ********************************************/ 
  // Explanation: 
  // In this routine the sensor will measure the resistance of the sensor supposing before was pre-heated
  // and now is on clean air (Calibration conditions), and it will setup R0 value.
  // We recomend execute this routine only on setup or on the laboratory and save on the eeprom of your arduino
  // This routine not need to execute to every restart, you can load your R0 if you know the value
  // Acknowledgements: https://jayconsystems.com/blog/understanding-a-gas-sensor
  Serial.print("Calibrating please wait.");
  float calcR0 = 0, calcR02 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ4.update(); // Update data, the arduino will be read the voltage on the analog pin
    MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
    
    calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    calcR02 += MQ135.calibrate(RatioMQ135CleanAir);
    
    Serial.print(".");
  }
  MQ4.setR0(calcR0/10);
  MQ135.setR0(calcR02/10);
  Serial.println("  done!.");
  
  //if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  //if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 

  
  GPSSensor.inicializar();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  CCSSensor.cicloCCS();
  GPSSensor.smartdelay(timeDelay);
  MQ4.update();  // Update data, the arduino will be read the voltaje in the analog pin
  MQ135.update(); // Update data, the arduino will be read the voltaje in the analog pin
  
  MQ4.setA(60000000000); MQ4.setB(-14.01); // Configurate the ecuation values to get Alcohol concentration
  alcohol = MQ4.readSensor();
  MQ4.setA(1012.7); MQ4.setB(-2.786); // Configurate the ecuation values to get CH4 concentration
  CH4 = MQ4.readSensor();
  MQ135.setA(102.2 ); MQ135.setB(-2.473); // Configurate the ecuation values to get NH4 concentration
  NH4 =   MQ135.readSensor();
  String lecturas = data.procesarData(
    DHTSensor.leerTemperatura()
  , DHTSensor.leerHumedad()
  , bmpSensor.leerPresionAtmosferica()
  , alcohol
  , CCSSensor.medirTVOC()
  , CCSSensor.medirCO2()
  , CH4
  , NH4
  , GPSSensor.leerLatitud(), GPSSensor.leerLongitud(),
  GPSSensor.leerFecha());

  if(lecturas!= "" & CCSSensor.getFlagVal())
  {
    if(serDebug) Serial.println("Se ha enviado la informacion hacia el wemos");
    if(serDebug) Serial.print("Lecturas: "); Serial.println(lecturas);
    serialportManager.enviarWemos(lecturas);
    CCSSensor.resetFlag();
  }
  else
  {
    if(serDebug) Serial.print("Error de lectura - No se ha enviado al wemos: "); Serial.println(lecturas);
  }
}
