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

#define count_mqtt_server 3
#define mqtt_server[3] ["test.mosquitto.org", "iot.eclipse.org", "157.230.174.83"]
#define serverPort 1883

//Advance mosquitto NO TOUCH
#define MQTT_CALLBACK_FCTN std::function<void(char*, uint8_t*, unsigned int)> callback



#define ledWarning led_builtin
#define chipSelect D4
#define arduinoTXPin D3
#define arduinoRXPin D2
#define arduinoResetPin D1 //pin que comunica con el reset del arduino

#define timeDelay 4*60*1000 // 5 minutos * 60 segundos * 1000 milisegundos
#define minTime timeDelay/2 //Tiempo minimo para enviar una actualizacion
#define sep ',' //Item separador en los ar
#define minDelay 10 //delay min for try

#define maxNumError 10 //Numero maximo de errrores tolerable antes de causar un reinicio del arduino


#define URL "http://airq.dronefenix.a2hosted.com/bot"
#define banderaPeticionGet 6 //Maximo de publicaciones al broker antes de llamar a la pagina bot para despertar al hosting en caso que el iis haya apagado el servicio

#define initialCharacter "Inicio"
#define endCharacter "Final"
#define tabulador "\t"

#endif // CONFIGURATION_H
