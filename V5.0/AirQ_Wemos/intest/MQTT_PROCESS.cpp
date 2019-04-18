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


    
void MQTT_PROCESS::inicializar(){
  
  WiFiManager wifiManager;
  
  while (!wifiManager.autoConnect(wiFiname)) {
      if(serDebug) Serial.println("Connection to hostname failed, restarting in 5 seconds");
      delay(minDelay*50);
  }

   if(serDebug) Serial.println("MQTT Server: "  + String(__mqttServerConnected));
  
  
  
}

boolean MQTT_PROCESS::publicarData(double temp, double hum, double presAlt, double alcoholPPM, double TVOC, double CO2, double Metano, double NH4, double latitud, double longitud, double fecha){
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

    
    root["D9"] = latitud; 
    root["D10"] = longitud; 
    root["D11"] =  fecha;
        
    char JSONmessageBuffer[260];
    root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    if(serDebug) Serial.println("Mensaje listo para enviar por mqtt: ");
    if(serDebug) Serial.println(JSONmessageBuffer);

    bool isPublished = false;
    int attemps = 0;

    while(!isPublished or attemps>=10)
    {
      if (mqttClient.publish(outTopic, JSONmessageBuffer) == true) {
        
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

void MQTT_PROCESS::setMQTTServer()
{  
    unsigned long lastMQTTConnectionAttempt, oldTime;
    
  if(__consecutive<=count_mqtt_server - 1)
  {
    __consecutive+=1;
    if(serDebug) Serial.println("Proximo servidor: " + String(mqtt_server[__consecutive]));
  }
  else
  {
    __consecutive = 0;
    if(serDebug) Serial.println("He probado con todos los broker, volvere a comenzar a probar");
  }  
  __mqttServerConnected = mqtt_server[__consecutive];
  mqttClient.setServer(mqtt_server[__consecutive], serverPort);
  
  if(serDebug) Serial.println("New broker connected: " + __mqttServerConnected);
  
  mqttClient.setCallback(callback); 
}

void MQTT_PROCESS::getPetition()
{
    if(serDebug) Serial.println("Funcion peticion get - wifiStat: " + String(WiFi.status()));
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
 
    http.begin(URL);  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      //String payload = http.getString();   //Get the request response payload
      if(serDebug) Serial.println("Resultado de la peticion: ");
      if(serDebug) Serial.println(String(httpCode));
      //if(serDebug) Serial.println(payload);                     //Print the response payload
 
    }
 
    http.end();   //Close connection
 
  }
 delay(timeDelay/2);
}



bool MQTT_PROCESS::confirmIfMqttIsConnectedOrLoopMQTT()
{
  if(!WiFi.isConnected())
    {
      delay(timeDelay);
      reportError("WiFi is not connected");
      return false;
    }    
     else if (!mqttClient.connected()) {
      if(serDebug) Serial.println("MQTT no conectado");
      if(lastMQTTConnectionAttempt == 0 || millis() > lastMQTTConnectionAttempt + 3 * 1000) {
        lastMQTTConnectionAttempt = millis();
        
        while (!mqttClient.connect(inTopic)) {   
          setMQTTServer();
          reportError("Trying to connect to mqtt: " + String(mqtt_server[__consecutive]));  
          delay(minDelay*100);
          //sprintf(topic, "%s/+/+", settings.mqttTopic);
          //mqttClient.subscribe(topic);

          //TODO multiple relays
          //updateMQTT(0);
        }        
          mqttClient.subscribe(inTopic);
          if(serDebug) Serial.println("MQTT connection OK - " + String(mqtt_server[__consecutive]));
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

void MQTT_PROCESS::callback(char* topic, byte* payload, unsigned int length) {
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