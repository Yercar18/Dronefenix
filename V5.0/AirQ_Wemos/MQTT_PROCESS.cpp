#include "MQTT_PROCESS.h"
#include "configuration.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <ESP8266HTTPClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

//Server private onmotica and public bypass servers
//info: https://github.com/mqtt/mqtt.github.io/wiki/public_brokers
//static const char* mqtt_server[] = {"157.230.174.83", "test.mosquitto.org", "iot.eclipse.org", "broker.hivemq.com", "www.cloudmqtt.com", "mqtt.swifitch.cz", "mqtt.fluux.io", "console.solace.cloud"};
static const int count_mqtt_server = 3;
static const char* mqtt_server[count_mqtt_server] = {"test.mosquitto.org", "iot.eclipse.org", "157.230.174.83"};
const int serverPort = 1883;

const char* outTopic = "droneFenix/2/estacion1";
const char* inTopic = "droneFenix/2/estacion1IN";
const char* wiFiname = "AirQ_droneFenix/2/estacion1";
unsigned long lastMQTTConnectionAttempt, oldTime;

void MQTT_PROCESS::inicializar(){
  
  WiFiManager wifiManager;
  
  while (!wifiManager.autoConnect(wiFiname)) {
      if(serDebug) Serial.println("Connection to hostname failed, restarting in 5 seconds");
      delay(minDelay*50);
  }

  if(serDebug) Serial.println("Numero de brokers: " + String(sizeof(mqtt_server)));
  
  for(int i = 0; i <= count_mqtt_server - 1; i++)
  {
    if(serDebug) Serial.println("Server" + String(i) +  ": " + String(mqtt_server[i]));
  }
  
  
}

void publicarData(double temp, double hum, double presAlt, double alcoholPPM, double TVOC, double CO2, double Metano, double NH4, String latitud, String longitud, String fecha){
    confirmIfMqttIsConnectedOrLoopMQTT();
    // Memory pool for JSON object tree.
    //
    // Inside the brackets, 200 is the size of the pool in bytes.
    // Don't forget to change this value to match your JSON document.
    // Use arduinojson.org/assistant to compute the capacity.
    DynamicJsonBuffer jsonBuffer(260);
    // StaticJsonBuffer allocates memory on the stack, it can be
    // replaced by DynamicJsonBuffer which allocates in the heap.
    //
    // DynamicJsonBuffer  jsonBuffer(200);
    
    // Create the root of the object tree.
    //
    // It's a reference to the JsonObject, the actual bytes are inside the
    // JsonBuffer with all the other nodes of the object tree.
    // Memory is freed when jsonBuffer goes out of scope.
    JsonObject& root = jsonBuffer.createObject();         
    root["D1"] = temp;  
    root["D2"] = hum; 
    root["D3"] = presAlt;  
    root["D4"] = alcoholPPM;  
    root["D5"] = TVOC;  
    root["D6"] = CO2; 
    root["D7"] = Metano;  
    root["D8"] = NH4;  

    
    root["D9"] = stringToDouble(latitud); 
    root["D10"] = stringToDouble(longitud); 
    root["D11"] = stringToDouble(fecha);
        
    char JSONmessageBuffer[260];
    root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    debugSerialNCR("Mensaje listo para enviar por mqtt: ");
    debugSerial(JSONmessageBuffer);

    bool isPublished = false;

    while(!isPublished)
    {
      if (mqttClient.publish(outTopic, JSONmessageBuffer) == true) {
        contadorPublicaciones++;
        
        debugSerial("El mensaje se ha publciado correctamente");
        debugSerial("Contador publciaciones: " +  String(contadorPublicaciones) + " , bandera para hacer un GET: " + String(banderaPeticionGet));
        
        if(contadorPublicaciones>=banderaPeticionGet)
        {
          contadorPublicaciones = 0;
          getPetition();
        }
        isPublished = true;
        debugSerial("Publicado!!! :)");
        break;
      } else {
        reportError("Error publicando el mensaje en el broker");
        debugSerial("result -- " + String(confirmIfMqttIsConnectedOrLoopMQTT()));
        delay(timeDelay*1.5);
      }
    }
    delay(minDelay*100);
    /*
    root.printTo(Data); //Almaceno el json generado en la variable Data
      Serial.print("El json es: "); Serial.println(Data);
      char msg[Data.length()];
      Data.toCharArray(msg,Data.length());
      
      client.publish(outTopic, msg);
      */
}

void MQTT_PROCESS::setMQTTServer()
{  
  if(consecutive<=count_mqtt_server - 1)
  {
    consecutive+=1;
    if(serDebug) Serial.println("Proximo servidor: " + String(mqtt_server[consecutive]));
  }
  else
  {
    consecutive = 0;
    if(serDebug) Serial.println("He probado con todos los broker, volvere a comenzar a probar");
  }  
  mqttClient.setServer(mqtt_server[consecutive], serverPort);
  
  if(serDebug) Serial.println("New broker connected: " + String(mqtt_server[consecutive]));
  
  mqttClient.setCallback(callback); 
}

void MQTT_PROCESS::getPetition()
{
    debugSerial("Funcion peticion get - wifiStat: " + String(WiFi.status()));
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
 
    http.begin(URL);  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      //String payload = http.getString();   //Get the request response payload
      debugSerial("Resultado de la peticion: ");
      debugSerial(String(httpCode));
      //debugSerial(payload);                     //Print the response payload
 
    }
 
    http.end();   //Close connection
 
  }
 delay(timeDelay/2);
}



bool confirmIfMqttIsConnectedOrLoopMQTT()
{
  if(!WiFi.isConnected())
    {
      delay(timeDelay);
      reportError("WiFi is not connected");
      return false;
    }    
     else if (!mqttClient.connected()) {
      debugSerial("MQTT no conectado");
      if(lastMQTTConnectionAttempt == 0 || millis() > lastMQTTConnectionAttempt + 3 * 1000) {
        lastMQTTConnectionAttempt = millis();
        
        while (!mqttClient.connect(inTopic)) {   
          setMQTTServer();
          reportError("Trying to connect to mqtt: " + String(mqtt_server[consecutive]));  
          delay(minDelay*100);
          //sprintf(topic, "%s/+/+", settings.mqttTopic);
          //mqttClient.subscribe(topic);

          //TODO multiple relays
          //updateMQTT(0);
        }        
          mqttClient.subscribe(inTopic);
          debugSerial("MQTT connection OK - " + String(mqtt_server[consecutive]));
          return true;
      }
      else
      {
        return false;
      }
    }
    else {
      mqttClient.loop();
      return true;
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  debugSerial("Message arrived [");
  debugSerial(topic);
  debugSerial("] ");
  for (int i = 0; i < length; i++) {
    if(serDebug)
    {
      Serial.print((char)payload[i]);
    }
  }
  debugSerial("");
}
