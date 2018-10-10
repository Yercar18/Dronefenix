#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define timeDelay 2000
#define sep ','
#define ledWarning 13
#define BAUD_RATE 115200
#define chipSelect D4
#define buttonPin D8

const String defaultFileName = "datos";
const String defaultFileExtension = ".csv";

//Configuraciones de red
const char* ssid = "MIGUELANGEL";
const char* password = "administrador5612";
const char* mqtt_server = "192.168.1.87";
const char* outTopic = "droneFenix/2/estacion1";

String Data = "";
int defaultFileCounter = 0,contadorDatos = 0;
boolean newData = false, validData = false;
File Archivo;


SoftwareSerial arduinoSerial(D3, D2, false, 256);// SoftwareSerial arduinoSerial(14, 12, false, 256);
WiFiClient espClient;
PubSubClient client(espClient);

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
  while(SD.exists(String(defaultFileName+defaultFileCounter+defaultFileExtension))){
    Serial.println("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
    Serial.println(SD.exists(String(defaultFileName+defaultFileCounter+defaultFileExtension)));  
    defaultFileCounter +=1;  
    delay(defaultFileCounter);
  }
  inicioDeAlmacenamiento(String(defaultFileName+defaultFileCounter+defaultFileExtension));// Esta funcion verifica la integridad del archivo y realiza la insecion de las caveceras
}

void loop() {
    if(newData){
      ValidateAndShowData();
    }
    if(digitalRead(buttonPin)){
      saveDataSD(Data);
    }
    smartDelay(timeDelay + contadorDatos); 
}
void getDataFromArduino(){
  while (arduinoSerial.available() > 0) {
    char inByte = arduinoSerial.read();
    Data += inByte;
    //Serial.write(inByte);
    newData = true; 
    validData = false; //to validate
    yield();
  }
}
void saveDataSD(String Data){
  // String  var = getValue( StringVar, ',', 2); // if  a,4,D,r  would return D    
  if(validData & newData){
    Archivo = SD.open(String(defaultFileName+defaultFileCounter+defaultFileExtension), FILE_WRITE);
    delay(1);
    Archivo.println(Data);
    delay(1);
    Archivo.close();
    contadorDatos++;
    Data = "";newData = false;validData = false;
  }
}
void smartDelay(int timeWait){
  unsigned long oldTime = millis();
  while(millis()-oldTime<=timeWait){
    //server.handleClient();
    char msg[Data.length()];
    Data.toCharArray(msg,Data.length());
    client.publish(outTopic, msg);
    getDataFromArduino();
    delay(timeWait/10);
  }
}
void ValidateAndShowData(){
  validData = false;
  for(int i=0;i<=10;i++){
    validData = getValue(Data, sep, i);
    delay(10);
  }
  if(!validData){Serial.println("Validacion: error");}
}
void webPage(){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(minWebPageStart);
            client.println(startData);
            client.println(Data);
            client.println(endData);
            client.println(minWebPageEnd);

            //client.println(,"Error loading page");
            //server.send(200, "text/plain",Data);
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
    //Archivo.print("------------------------------------------------ INICIO DE LA TOMA DE DATOS -------------------------------------------------");
    //Archivo.println();
    
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
bool getValue(String data, char separator, int index)
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
    return found > index ? data.substring(strIndex[0], strIndex[1]).toInt()>=0 : false;
}  // END


double getValueDouble(String data, char separator, int index)
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
    return found > index ? atof(data.substring(strIndex[0], strIndex[1]).c_str()) : 0;
}  // END

