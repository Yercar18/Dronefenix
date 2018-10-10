
//#include <SoftwareSerial.h>

#include <SPI.h>
#include <SD.h>
// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
//#include <ESP8266WebServer.h>

#define timeDelay 1400
#define sep ','
#define ledWarning 13
#define BAUD_RATE 115200
#define chipSelect D4

const String defaultFileName = "datos";
const String defaultFileExtension = ".csv";
const String defaultWebName = "web";
const String defaultWebExtension = ".html";
const char* ssid = "DroneFenix_V3.00";
const char* password = "abcd1234";

//WebPage Settings

const String minWebPageStart = "<!DOCTYPE html><html><head><title>Drone Fenix V3.00</title><style type='text/css'>body{background-color: white;}.pure-table{border-collapse:collapse;border-spacing:0;empty-cells:show;border:1px solid #cbcbcb}.pure-table caption{color:#000;font:italic 85%/1 arial,sans-serif;padding:1em 0;text-align:center}.pure-table td,.pure-table th{border-left:1px solid #cbcbcb;border-width:0 0 0 1px;font-size:inherit;margin:0;overflow:visible;padding:.5em 1em}.pure-table td:first-child,.pure-table th:first-child{border-left-width:0}.pure-table thead{background-color:#e0e0e0;color:#000;text-align:left;vertical-align:bottom}.pure-table td{background-color:transparent}.pure-table-odd td,.pure-table-striped tr:nth-child(2n-1) td{background-color:#f2f2f2}.pure-table-bordered td{border-bottom:1px solid #cbcbcb}.pure-table-bordered tbody>tr:last-child>td{border-bottom-width:0}.pure-table-horizontal td,.pure-table-horizontal th{border-width:0 0 1px;border-bottom:1px solid #cbcbcb}.pure-table-horizontal tbody>tr:last-child>td{border-bottom-width:0}</style></head><body><div style='padding:30px;background-color: blue'><h1 style='color:white;text-align: center;'>ENSAMBLE DE UN DRONE BASADO EN UN SISTEMA SENSORIAL Y MULTIESPECTRAL PARA MEDIR VARIABLES METEOROLÓGICOS</h1></div><h2 style='#'>Desarrolladores:&nbsp&nbsp</h2><ul><li><p style='#'><a href='www.github.com/miguel56121'> Miguel Angel Califa</a> </p></li><li><p><a href='#'>Yersson Carrillo</a></p></li></ul><div><h2 style='clear:both'>Mediciones tomadas por la estacion:</h2><div align='center'><table class='pure-table' style='text-align: center'> <thead> <tr> <th># de muestra tomada</th> <th>Latitud</th><th>Longitud</th><th>Altitud</th><th>Temperatura</th><th>Temperatura CCS</th><th>Temperatura BMP</th><th>Humedad</th><th>Indice de calor</th><th>Dioxido de carbono</th><th>Polvo particulado</th><th>Presion</th> </tr></thead> <tbody>";
const String minWebPageEnd = "</tbody></table></div><h3 style='background-color:blue;margin:0;margin-top:3%'><div style='padding:1%'></div><a style='color:white' href='/downloadData'>Descargar informacion de la estacion metereologica</a></h3></div><footer style='margin=0;padding:500px;background-color: blue; width: 100%;height: 100%'></footer></body></html>";
const String startData = "<td>";
const String endData = "</td>";


String Data = "";
int defaultFileCounter = 0,contadorDatos = 0;
boolean newData = false, validData = false;
File Archivo;


SoftwareSerial arduinoSerial(D3, D2, false, 256);// SoftwareSerial arduinoSerial(14, 12, false, 256);
//ESP8266WebServer server(80);
// Web Server on port 80
WiFiServer server(80);

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
  //server.on("/", webPage);  
  server.begin();
  while(SD.exists(String(defaultFileName+defaultFileCounter+defaultFileExtension))){
    Serial.println("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
    Serial.println(SD.exists(String(defaultFileName+defaultFileCounter+defaultFileExtension)));  
    defaultFileCounter +=1;  
    delay(defaultFileCounter);
  }
  inicioDeAlmacenamiento(String(defaultFileName+defaultFileCounter+defaultFileExtension));// Esta funcion verifica la integridad del archivo y realiza la insecion de las caveceras
  //inicio de pagina web
  Archivo = SD.open(String(defaultWebName+defaultFileCounter+defaultWebExtension), FILE_WRITE);
  delay(1);
  Archivo.println(minWebPageStart);
  delay(1);
  Archivo.close();
}

void loop() {
    if(newData){
      ValidateAndShowData();
    }
    saveDataSD(Data);
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
  //while (Serial.available() > 0) {
    //arduinoSerial.write(Serial.read());
    //yield();
  //}
  //Serial.print("DataSerial: ");
  //Serial.println(Data);
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
    //WebUpdate
    Archivo = SD.open(String(defaultWebName+defaultFileCounter+defaultWebExtension), FILE_WRITE);
    delay(1);
    for(int i=0;i<=10;i++){
      Archivo.println(startData+getValueDouble(Data,sep,i)+endData);
      delay(1);  
    }
    Serial.println("Data_almacenada ");
    Archivo.close();
  }
  
  }
void smartDelay(int timeWait){
  unsigned long oldTime = millis();
  while(millis()-oldTime<=timeWait){
    //server.handleClient();
    WiFiClient client = server.available();
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
            if(!loadFromSdCard("/" + String(defaultWebName+defaultFileCounter+defaultWebExtension))){
              server.send(200, "text/html","Error loading page");
            }
            
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

bool loadFromSdCard(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) {
    path += "index.htm";
  }

  if (path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if (path.endsWith(".htm")) {
    dataType = "text/html";
  } else if (path.endsWith(".css")) {
    dataType = "text/css";
  } else if (path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (path.endsWith(".png")) {
    dataType = "image/png";
  } else if (path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (path.endsWith(".zip")) {
    dataType = "application/zip";
  }

  File dataFile = SD.open(path.c_str());
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile) {
    return false;
  }

  if (server.hasArg("download")) {
    dataType = "application/octet-stream";
  }

  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    Serial.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

