#include <SPI.h>
#include <SD.h>
#include <Wire.h>
// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>

// Replace with your network details
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";


//Definitions
#define SLAVE_ADDRESS 8 //Slave arduino ID
#define chipSelect 4 //D4
#define bytes 100
#define sep ','
#define timeDelay 500
#define ledWarning 13

//variables
int defaultFileCounter = 0;
char t[bytes]={};
File Archivo;
String defaultFileName = "datos"; // nombre del archivo por defecto
String defaultFileExtension = ".csv";


// Web Server on port 80
WiFiServer server(80);

void setup()
{
  if(initModule()){Serial.println("card initialized.");};
  pinMode(ledWarning,OUTPUT);
  digitalWrite(ledWarning,LOW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());

  while(SD.exists(defaultFileName+defaultFileCounter+defaultFileExtension)){
    Serial.println("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
    Serial.println(SD.exists(defaultFileName+defaultFileCounter+defaultFileExtension));  
    defaultFileCounter +=1;  
  }
  inicioDeAlmacenamiento(defaultFileName+defaultFileCounter+defaultFileExtension);// Esta funcion verifica la integridad del archivo y realiza la insecion de las caveceras

}

void loop()
{
  int countBytes = getDataFromArduino();
  String str(t);
  saveDataSD(t);
  webPage(t);
  delay(timeDelay);
}

void saveDataSD(String Data){
  File dataFile = SD.open(defaultFileName+defaultFileCounter+defaultFileExtension, FILE_WRITE);
  if (dataFile) {
    dataFile.println(Data);
    dataFile.close();
    Serial.println(Data);
  }
}
int getDataFromArduino(){
  Wire.requestFrom(SLAVE_ADDRESS, bytes);
  int i=0; //counter for each bite as it arrives
  while (Wire.available()) { 
    t[i] = Wire.read(); // every character that arrives it put in order in the empty array "t"
    i=i+1;
  }
  return i;
}
boolean initModule(){
  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return false;
  }
  Wire.begin ();
  return true;
}

void webPage(String Data){
  // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {
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
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
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
