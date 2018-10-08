
//#include <SoftwareSerial.h>

#include <SPI.h>
#include <SD.h>
// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>

#define timeDelay 1400
#define sep ','
#define ledWarning 13
#define BAUD_RATE 115200
#define chipSelect D4

String Data = "";
const char* ssid = "DroneFenix_V3.00";
const char* password = "abcd1234";
String defaultFileName = "datos"; // nombre del archivo por defecto
String defaultFileExtension = ".csv";
int defaultFileCounter = 0;
boolean newData = false;
File Archivo;


SoftwareSerial arduinoSerial(D3, D2, false, 256);// SoftwareSerial arduinoSerial(14, 12, false, 256);
ESP8266WebServer server(80);

void setup() {
  Serial.begin(BAUD_RATE);
  arduinoSerial.begin(BAUD_RATE);

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }

  Serial.println("\nSoftware serial test started");

  for (char ch = ' '; ch <= 'z'; ch++) {
    arduinoSerial.write(ch);
  }
  arduinoSerial.println("");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
  Serial.println("");
 IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
  // Starting the web server
  server.on("/", webPage);  
  server.begin();
  while(SD.exists(defaultFileName+defaultFileCounter+defaultFileExtension)){
    Serial.println("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
    Serial.println(SD.exists(defaultFileName+defaultFileCounter+defaultFileExtension));  
    defaultFileCounter +=1;  
  }
  inicioDeAlmacenamiento(defaultFileName+defaultFileCounter+defaultFileExtension);// Esta funcion verifica la integridad del archivo y realiza la insecion de las caveceras
}

void loop() {
    smartDelay(timeDelay);
    saveDataSD(Data); 
    if(newData){
      showData();
    }
}
void getDataFromArduino(){
  while (arduinoSerial.available() > 0) {
    char inByte = arduinoSerial.read();
    Data += inByte;
    Serial.write(inByte);
    newData = true;
    yield();
  }
  while (Serial.available() > 0) {
    arduinoSerial.write(Serial.read());
    //yield();
  }
  //Serial.print("DataSerial: ");
  //Serial.println(Data);
}
void saveDataSD(String Data){
  // String  var = getValue( StringVar, ',', 2); // if  a,4,D,r  would return D    
  if(getValue(Data, sep, 0)>=0 & getValue(Data, sep, 1)>=0 & getValue(Data, sep, 2)>=0 & getValue(Data, sep, 3)>=0 & getValue(Data, sep, 4)>=0 & getValue(Data, sep, 5)>=0 & getValue(Data, sep, 6)>=0 & getValue(Data, sep, 7)>=0 & getValue(Data, sep, 8)>=0 & getValue(Data, sep, 9)>=0 & getValue(Data, sep, 10)>=0 & Archivo & newData){
    Archivo = SD.open(defaultFileName+defaultFileCounter+defaultFileExtension, FILE_WRITE);
    Serial.println("LLegue");
    delay(1);
    Archivo.println(Data);
    Archivo.close();
    Serial.print("Data_SD: ");
    Serial.println(Data);
    Data = "";
    newData = false;
  }
  }
void smartDelay(int timeWait){
  unsigned long oldTime = millis();
  while(millis()-oldTime<=timeWait){
    server.handleClient();
    getDataFromArduino();
    delay(timeWait/10);
  }
}
void showData(){
    Serial.println("--------------DATA--------------");
  for(int i=0;i<=10;i++){
    Serial.println(getValue(Data, sep, 0));
  }
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
  Archivo.close();
}

// String  var = getValue( StringVar, ',', 2); // if  a,4,D,r  would return D    
double getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length();

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]).toDouble() : 0;
}  // END
