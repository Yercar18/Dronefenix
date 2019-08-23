#include "SERIAL_COMMUNICATION.h"
#include "PROCESS_DATA.h"
#include "SD_PROCESS.h"
#include "WIFI_PROCESS.h"
#include "MEMORY_ADMINISTRATION.h"
#include "PINS.h"
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
MEMORY_ADMINISTRATION administracion;
PINS pinesIO;

static const int count_mqtt_server = 3;
static char* mqtt_server[count_mqtt_server] = { "mqtt.onmotica.com", "mqtt2.onmotica.com", "mqtt3.onmotica.com"};
char* __mqttServerConnected;
const int serverPort = 1883;
int serverConnectedIndex = 0;

unsigned long lastPublishedTime = 0;
unsigned long lastGetPetition = 0;


WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {

    //Inicializacion de los pines, memoria SD y WiFi
    pinesIO.inicializar();
    memoriaSD.setFileCounter(0);
    memoriaSD.setNumError(0);
    
    serial.inicializar();
    memoriaSD.inicializar();
    memoriaSD.guardarEncabezados();

    WiFiProcess.inicializar();

    //Guardo en el registro que el sistema inicio correctamente
    memoriaSD.saveIntoLogMsg("Encendido del sistema, WiFi OK", administracion.freeSpaceReportSerial() , WiFiProcess.wifiIsConnected()?"Conectado":"Desconectado", mqttIsConnected()?"Conectado":"Desconectado", false);

    String resultPetition = WiFiProcess.getPetition(URL);
    if(serDebug) Serial.println("Peticion get - " + resultPetition);
    
    setMQTTServer();

    procesamiento.setTimeToWait(procesamiento.generateRandom());
    memoriaSD.saveIntoLogMsg("Saliendo del setup - Resultado de la peticion: " + resultPetition, administracion.freeSpaceReportSerial() , WiFiProcess.wifiIsConnected()?"Conectado":"Desconectado", mqttIsConnected()?"Conectado":"Desconectado", false);
    
}


void loop() {
  
    if(!WiFiProcess.wifiIsConnected()) setup(); //Reinicio si no hay wifi
    if (!mqttIsConnected()) reconnect(); //Reconectar mqtt si perdio conexion

    
    String informacion = serial.leerArduino();
    if(procesamiento.procesarInformacion(informacion))
    {
      //Calcular y guardar la fecha
      String fecha = memoriaSD.getTime();
      procesamiento.setFecha(fecha);
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
      int CH4 = procesamiento.leerCH4();
      int NH4 = procesamiento.leerNH4();
      float latitud = procesamiento.leerLatitud();
      float longitud = procesamiento.leerLongitud();

      String json2MQTT = procesamiento.ensamblarMensajeJSON(temperatura, humedad, presionAmosferica, alcohol, tvoc, co2, CH4, NH4, latitud, longitud, fecha);

      if(procesamiento.SAVEJSON(json2MQTT))
      {
        procesamiento.setTimeToWait(procesamiento.generateRandom());
        sendMQTTMsgPacket(procesamiento.getIndex());
      }

    }
    
    if((millis() - lastPublishedTime)>maxTimeWithNoPublish)  memoriaSD.saveIntoLogMsg("Han pasado " + String(maxTimeWithNoPublish/60000) + " minutos sin enviar actualizaciones" , administracion.freeSpaceReportSerial() , WiFiProcess.wifiIsConnected()?"Conectado":"Desconectado", mqttIsConnected()?"Conectado":"Desconectado", true);   
    if((millis() -lastGetPetition)>maxTimeWithNoPublish) WiFiProcess.getPetition(URL); //Despertar al servidor haciendo una peticion cada media hora
    mqttClient.loop();
}
void sendMQTTMsgPacket(int countMsgToSend)
{
  int len = 260;
  char buf[len];
  procesamiento.resetMsgQeueCounter();

  for(int i=0; i<=countMsgToSend; i++)
  {
    String mensajeJson = procesamiento.getJSON(i);
    mensajeJson.toCharArray(buf, len);
    savePublishStatusMQTT(publicarInformacion(buf));
  }
}
void savePublishStatusMQTT(boolean result)
{
  if(result)
      { 
         unsigned long span = (millis() - lastPublishedTime)/1000;
         int ratio = span>60?span/60:span;
         String unidad = span>60?" minutos":" segundos";
         lastPublishedTime = millis();
         memoriaSD.saveIntoLogMsg("Mensaje publicado(" + String(__mqttServerConnected) + ") con exito - ratio (Tiempo transcurrido desde la anterior publicacion): " + String(ratio) + unidad , administracion.freeSpaceReportSerial() , WiFiProcess.wifiIsConnected()?"Conectado":"Desconectado", mqttIsConnected()?"Conectado":"Desconectado", false);   
      }
      else
      {
        memoriaSD.saveIntoLogMsg("Mensaje no publicado", administracion.freeSpaceReportSerial() , WiFiProcess.wifiIsConnected()?"Conectado":"Desconectado", mqttIsConnected()?"Conectado":"Desconectado", true);   
      }
}


boolean publicarInformacion(char JSON[260]){
   
    bool isPublished = false;
    int attemps = 0;

    while(!isPublished & attemps<=10)
    {
      if (mqttClient.publish(inTopic, JSON) == true) {
        
        if(serDebug) Serial.println("El mensaje se ha publciado correctamente");
        
        isPublished = true;
        if(serDebug) Serial.println("Publicado!!! :)");
        break;
      } else {
        attemps++;
        if(serDebug) Serial.println("No se ha podido publicar - intento " + String(attemps));
        isPublished = false;
        delay(minTime);
      }
    }
    delay(minTime);
    return isPublished;
}

void setMQTTServer()
{   
  if(serverConnectedIndex < count_mqtt_server-1)
  {
    serverConnectedIndex++;
  }
  else
  {
    serverConnectedIndex = 0;
  }
  __mqttServerConnected = mqtt_server[serverConnectedIndex];//"broker.mqtt-dashboard.com";
  if(serDebug) Serial.println("MQTT Server: " + String(__mqttServerConnected));
  mqttClient.setServer(__mqttServerConnected, serverPort);
  mqttClient.setCallback(callback);
}
bool mqttIsConnected()
{
  return mqttClient.connected();
}

void reconnect() {
  int attemps = 0;
  // Loop until we're reconnected
  while (!mqttIsConnected() && attemps<10) {
    attemps ++;
    if(serDebug) Serial.print("reconectando MQTT...");
    memoriaSD.saveIntoLogMsg("reconectando MQTT(" + String(__mqttServerConnected) + ")...", administracion.freeSpaceReportSerial() , WiFiProcess.wifiIsConnected()?"Conectado":"Desconectado", mqttIsConnected()?"Conectado":"Desconectado", false);   
    // Create a random client ID
    String clientId = obtenerIdCliente();
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      if(serDebug) Serial.println("Conectado con exito");
      memoriaSD.saveIntoLogMsg("Conectado con exito", administracion.freeSpaceReportSerial() , WiFiProcess.wifiIsConnected()?"Conectado":"Desconectado", mqttIsConnected()?"Conectado":"Desconectado", false);   
      // Once connected, publish an announcement...
      mqttClient.publish("testMQTT", "probando estacion metereologica");
      // ... and resubscribe
      mqttClient.subscribe(outTopic);
    } else {
      if(serDebug) Serial.print("fallo, rc=");
      if(serDebug) Serial.print(mqttClient.state());
      if(serDebug) Serial.println(" intentando nuevamente en 5 segundos");
      memoriaSD.saveIntoLogMsg("Fallo, rc = " + String(mqttClient.state()), administracion.freeSpaceReportSerial() , WiFiProcess.wifiIsConnected()?"Conectado":"Desconectado", mqttIsConnected()?"Conectado":"Desconectado", true);   
      // Wait 5 seconds before retrying
      setMQTTServer();
      delay(5000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  if(serDebug) Serial.println(topic);
  if(serDebug) Serial.println("Message arrived [");
  if(serDebug) Serial.println("] ");
  String strTopic = String(topic);
  String strOutTopic = String(outTopic);
  strOutTopic.trim();
  strTopic.trim();
  if(serDebug) Serial.println(strTopic.indexOf(strOutTopic));
  if(strTopic.indexOf(strOutTopic) > 0 || strTopic == strOutTopic)
  {
    String mensaje = "";
    for (int i = 0; i < length; i++) {
      if(serDebug)
      {
        //if(serDebug) Serial.print((char)payload[i]);
        mensaje +=  (char)payload[i];
      }
    }
    if(serDebug) Serial.println("");
  
    mensaje.trim();
    if(mensaje == "actualizar")
    {
      if(serDebug) Serial.println("enviando actualizacion");
      procesamiento.setTimeToWait(0.1);
    }
  }
}
String obtenerIdCliente()
{
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  return clientId;
}
