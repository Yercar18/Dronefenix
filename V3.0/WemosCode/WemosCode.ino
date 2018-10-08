#include <SPI.h>
#include <SD.h>
// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>

#define master 1
#define slave 8
// Replace with your network details
const char* ssid = "DroneFenix_V3.00";
const char* password = "abcd1234";

//Variables que van a ser leidas del arduino
short p,P,tvoc,co2,alt;
byte tempCCS,t,T,h,tBMP,hic;
float lat=0,lon=0;


//Definitions
#define SLAVE_ADDRESS 8 //Slave arduino ID
#define chipSelect D4 //D4
#define bytes 100
#define sep ','
#define timeDelay 1400
#define ledWarning 13
#define decimalPlaces 4
#define aTx 12
#define aRx 14

//variables
int defaultFileCounter = 0;
File Archivo;
String defaultFileName = "datos"; // nombre del archivo por defecto
String defaultFileExtension = ".csv";
String Data = "";


// Web Server on port 80
ESP8266WebServer server(80);
SoftwareSerial arduinoSerial(aRx,aTx, false, 256);

void setup()
{
  if(initModule()){Serial.println("card initialized.");};
  arduinoSerial.begin(115200);
  pinMode(ledWarning,OUTPUT);
  digitalWrite(ledWarning,LOW);
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
  Serial.println("");
 IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
  // Starting the web server
  server.on("/", webPage);  
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  while(SD.exists(defaultFileName+defaultFileCounter+defaultFileExtension)){
    Serial.println("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
    Serial.println(SD.exists(defaultFileName+defaultFileCounter+defaultFileExtension));  
    defaultFileCounter +=1;  
  }
  inicioDeAlmacenamiento(defaultFileName+defaultFileCounter+defaultFileExtension);// Esta funcion verifica la integridad del archivo y realiza la insecion de las caveceras

}

void loop()
{
  //getDataFromArduino();
  //showData();
  saveDataSD(Data);
  smartDelay(timeDelay);
}
void smartDelay(int timeWait){
  unsigned long oldTime = millis();
  while(millis()-oldTime<=timeWait){
    server.handleClient();
    getDataFromArduino();
  }
}

void saveDataSD(String Data){
  File dataFile = SD.open(defaultFileName+defaultFileCounter+defaultFileExtension, FILE_WRITE);
  if (dataFile) {
    dataFile.println(Data);
    dataFile.close();
    Serial.println(Data);
  }
}
void getDataFromArduino(){
    while (arduinoSerial.available() > 0) {
      Serial.write(arduinoSerial.read());
      yield();
    }
}

String getData() {
  Serial.print("out string: ");
  Serial.println(Data);
  return Data;
}

void showData(){
  Serial.println ("------------ Received data -------------- ");
    Serial.println (isnan(lat)?0:lat);
    Serial.println (isnan(lon)?0:lon);
    Serial.println (isnan(alt)?0:alt);
    Serial.println (isnan(co2)?0:co2);
    Serial.println (isnan(tvoc)?0:tvoc);
    Serial.println (isnan(p)?0:p);
    Serial.println (isnan(t)?0:t);
    Serial.println (isnan(tempCCS)?0:tempCCS);
    Serial.println (isnan(tBMP)?0:tBMP);
    Serial.println (isnan(h)?0:h);
    Serial.println (isnan(hic)?0:hic);
}
boolean initModule(){
  Serial.begin(115200);
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return false;
  }
  
  return true;
}

void webPage(){
            server.send(200, "text/plain",Data);
            /*
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // your actual web page that displays temperature
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body><h1>ESP8266 - Data from station</h1><h3>Temperature in Celsius: ");
            client.println(Data);
            client.println("*C</h3><h3>Raw data: ");
            client.println(Data);
            client.println("*F</h3></body></html>");  
            */
}
void inicioDeAlmacenamiento(String fileNameAndExtension){
  Archivo = SD.open(fileNameAndExtension, FILE_WRITE);  
  Serial.println("EL ARCHIVO EN LA SD SE LLAMARA: "+fileNameAndExtension);
  if (Archivo) {
    // latitud,longitud,altitud,temperatura (DHT), temperatura(CCS), temperatura (BMP), humedad (DHT11), hic (DHT11), co2 (CCS811), tvoc(CCS811), presionAtmosferica (BMP180)

    Archivo.println("SEP="+sep);
    Archivo.print("------------------------------------------------ INICIO DE LA TOMA DE DATOS -------------------------------------------------");
    Archivo.println();
    
    Archivo.print("Latitud");
    Archivo.print(sep);

    Archivo.print("Longitud");
    Archivo.print(sep);

    Archivo.print("Altitud");
    Archivo.print(sep);

    Archivo.print("Temperatura (DHT11)");
    Archivo.print(sep);


    Archivo.print("Temperatura (CCS811)");
    Archivo.print(sep);

    Archivo.print("Temperatura (BMP180)");
    Archivo.print(sep);

    Archivo.print("humedad (DHT11)");
    Archivo.print(sep);

    
    Archivo.print("hic (DHT11)");
    Archivo.print(sep);
    
    Archivo.print("co2 (CCS811)");
    Archivo.print(sep);
    
    Archivo.print("tvoc(CCS811)");
    Archivo.print(sep);  

    Archivo.print("presionAtmosferica (BMP180)");
    Archivo.println(sep);


  }else {
    digitalWrite(ledWarning, HIGH);
    delay(500);
    digitalWrite(ledWarning, LOW);
    delay(500);
  }
  Archivo.close();
}
