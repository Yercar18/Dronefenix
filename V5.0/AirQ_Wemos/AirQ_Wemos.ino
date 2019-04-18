#include "SERIAL_COMMUNICATION.h"
#include "PROCESS_DATA.h"
#include "SD_PROCESS.h"
#include "WIFI_PROCESS.h"
#include "configuration.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>

SERIAL_COMMUNICATION serial;
PROCESS_DATA procesamiento;
SD_PROCESS memoriaSD;
WIFI_PROCESS WiFiProcess;

char* __mqttServerConnected;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {

    memoriaSD.setFileCounter(0);
    memoriaSD.setNumError(0);
    
    serial.inicializar();
    memoriaSD.inicializar();

    WiFiProcess.inicializar();
    setMQTTServer();
    
    if(serDebug) Serial.println("Saliendo del setup");
}


void loop() {
    String informacion = serial.leerArduino();
    if(procesamiento.procesarInformacion(informacion))
    {
      //Si la informacion es valida se debe proceder a guardar en la SD y enviar al MQTT server
      String lineaSDCard = procesamiento.mensajeSDTabulado();
      memoriaSD.guardarInfo(lineaSDCard);
      if(serDebug) Serial.println("Guardado en la sd: " + lineaSDCard);
      
      //Mensaje MQTT
      double temperatura = procesamiento.leerTemperatura();
      double humedad = procesamiento.leerHumedad();
      double presionAmosferica = procesamiento.leerPresionAtmosferica();
      int tvoc =  procesamiento.leerTVOC();
      int co2 = procesamiento.leerCO2();
      int alcohol = procesamiento.leerAlcohol();
      int metano = procesamiento.leerMetano();
      int NH4 = procesamiento.leerNH4();
      float latitud = procesamiento.leerLatitud();
      float longitud = procesamiento.leerLongitud();
      double fecha = procesamiento.leerFecha();

      String json2MQTT = procesamiento.ensamblarMensajeJSON(temperatura, humedad, presionAmosferica, alcohol, tvoc, co2, metano, NH4, latitud, longitud, fecha);
      int len = 260;
      char buf[len];
      json2MQTT.toCharArray(buf, len);
      publicarInformacion(buf);
      
      
      if(serDebug) Serial.println("json: " + json2MQTT);  
    }
}

boolean publicarInformacion(char JSON[260]){
   
    bool isPublished = false;
    int attemps = 0;

    while(!isPublished or attemps>=10)
    {
      if (mqttClient.publish(outTopic, JSON) == true) {
        
        if(serDebug) Serial.println("El mensaje se ha publciado correctamente");
        
        isPublished = true;
        if(serDebug) Serial.println("Publicado!!! :)");
        break;
      } else {
        if(serDebug) Serial.println("No se ha podido publicar");
        isPublished = false;
        delay(timeDelay*1.5);
      }
    }
    delay(minDelay*100);
    return isPublished;
}

void setMQTTServer()
{   
  __mqttServerConnected = "test.mosquitto.org";
  mqttClient.setServer(__mqttServerConnected, serverPort);
  mqttClient.connect(inTopic);
}
bool mqttIsConnected()
{
  return mqttClient.connected();
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message arrived [");
  if(serDebug) Serial.println(topic);
  if(serDebug) Serial.println("] ");
  for (int i = 0; i < length; i++) {
    if(serDebug)
    {
      Serial.print((char)payload[i]);
    }
  }
  if(serDebug) Serial.println("");
}
