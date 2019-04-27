#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Arduino.h"

#define serDebug true //Para activar o desactivar el debug serial
#define BAUD_RATE 115200

//Mosquitto
//Server private onmotica and public bypass servers
//info: https://github.com/mqtt/mqtt.github.io/wiki/public_brokers
//static const char* mqtt_server[] = {"157.230.174.83", "test.mosquitto.org", "iot.eclipse.org", "broker.hivemq.com", "www.cloudmqtt.com", "mqtt.swifitch.cz", "mqtt.fluux.io", "console.solace.cloud"};

#define outTopic "droneFenix/2/estacion1"
#define inTopic "droneFenix/2/estacion1IN"
#define wiFiname "AirQ_droneFenix/2/estacion1"

#define ledWarning led_builtin
#define chipSelect D4
#define arduinoTXPin D3
#define arduinoRXPin D2
#define arduinoResetPin D1 //pin que comunica con el reset del arduino

#define timeDelay 5*60*1000 // 5 minutos * 60 segundos * 1000 milisegundos
#define minTime 100 //Tiempo minimo para re-enviar una actualizacion milisegundos
#define tabulador ',' //Item tabulador
#define minDelay 10 //delay min for try

#define maxTimeWithNoPublish 30*60*1000 //Si pasan mas de 30 minutos sin publicar es preocupante
#define maxNumError 500 //Numero maximo de errrores tolerable antes de causar un reinicio del arduino


#define URL "http://airq.onmotica.com/bot"
#define banderaPeticionGet 6 //Maximo de publicaciones al broker antes de llamar a la pagina bot para despertar al hosting en caso que el iis haya apagado el servicio

#define initialCharacter "Inicio"
#define endCharacter "Final"

#endif // CONFIGURATION_H
