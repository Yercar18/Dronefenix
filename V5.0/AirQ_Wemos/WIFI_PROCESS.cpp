#include "WIFI_PROCESS.h"
#include "configuration.h"
#include <WiFiManager.h>  
#include <ESP8266HTTPClient.h>

WiFiManager wifiManager;


    
void WIFI_PROCESS::inicializar(){
  while (!wifiManager.autoConnect(wiFiname)) {
      if(serDebug) Serial.println("Connection to hostname failed, restarting in 5 seconds");
      delay(minDelay*50);
  }
}



String WIFI_PROCESS::getPetition(String URL2Get)
{
    String payload;
    HTTPClient http;  //Declare an object of class HTTPClient
    http.begin(URL2Get);  //Specify request destination
    int httpCode = http.GET();                                                                 //Send the request
    if (httpCode > 0) { //Check the returning code 
      payload = http.getString();   //Get the request response payload
      if(serDebug) Serial.println("Resultado de la peticion: ");
      if(serDebug) Serial.println(String(httpCode));
      http.end();   //Close connection
  }
  delay(timeDelay/2);
  return payload;
}



boolean WIFI_PROCESS::wifiIsConnected()
{
  return WiFi.isConnected();
}
