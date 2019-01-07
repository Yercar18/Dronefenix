#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
//For sd support
#include <SPI.h>
#include <SD.h>   


#define ledWarning led_builtin
#define BAUD_RATE 115200
#define chipSelect D4
#define timeDelay 5*60*1000 // 5 minutos * 60 segundos * 1000 milisegundos
#define minTime timeDelay/2 //Tiempo minimo para enviar una actualizacion
#define sep ','

const char* mqtt_server = "68.183.31.237";
const char* outTopic = "droneFenix/2/estacion1";
const char* inTopic = "droneFenix/2/estacion1IN";
const char* wiFiname = "AirQ_droneFenix/2/estacion1";
const int serverPort = 1883;

const String defaultFileName = "datos";
const String defaultFileExtension = ".csv";

String temp, hum, presAt, alcohol,tvoc, co2, metano, NH4,  latitud, longitud;
String fecha;
//temporales
String tempT, humT, presAtT, alcoholT,tvocT, co2T, metanoT;


String Data = "", serialData;
int defaultFileCounter = 0,contadorDatos = 0;
boolean newData = false, validData = false;

File Archivo;
WiFiClient espClient;
PubSubClient client(espClient);
SoftwareSerial arduinoSerial(D3, D2, false, 256);// SoftwareSerial arduinoSerial(14, 12, false, 256);

void setup() {
  Serial.begin(BAUD_RATE);
  arduinoSerial.begin(BAUD_RATE);
  if (!SD.begin(chipSelect)) {
    Serial.println("SD CARD Lecture failed");
  }
  
  WiFiManager wifiManager;
  wifiManager.autoConnect(wiFiname);
  client.setServer(mqtt_server, serverPort);
  client.setCallback(callback); 

   while(SD.exists(String(defaultFileName+defaultFileCounter+defaultFileExtension))){
    Serial.println("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
    Serial.println(SD.exists(String(defaultFileName+defaultFileCounter+defaultFileExtension)));  
    defaultFileCounter +=1;  
    delay(defaultFileCounter);
  }
    inicioDeAlmacenamiento(String(defaultFileName+defaultFileCounter+defaultFileExtension));// Esta funcion verifica la integridad del archivo y realiza la insecion de las caveceras

}

void loop() {
  smartDelay(timeDelay);
}
void smartDelay(int timeWait){
  unsigned long oldTime = millis();
  while(millis()-oldTime<=timeWait){
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  
    getDataFromArduino();
    
    Data = getValueStr(Data,'\r',0);

    //T .. Temporal
    tempT = getValueStr(Data,sep,0).toInt() > 0 ? getValueStr(Data,sep,0):"0";
    humT = getValueStr(Data,sep,1).toInt() > 0 ? getValueStr(Data,sep,1):"0";
    presAtT = getValueStr(Data,sep,2).toInt() > 0 ? getValueStr(Data,sep,2):"0";
    alcoholT = getValueStr(Data,sep,3).toInt() > 0 ? getValueStr(Data,sep,3):"0";
    tvocT = getValueStr(Data,sep,4).toInt() > 0 ? getValueStr(Data,sep,4):"0";
    co2T = getValueStr(Data,sep,5).toInt() > 0 ? getValueStr(Data,sep,5):"0";
    metanoT = getValueStr(Data,sep,6).toInt() > 0 ? getValueStr(Data,sep,6):"0";
    NH4 = abs(getValueStr(Data,sep,9).toInt()) > 0 ? getValueStr(Data,sep,7):"0";

    latitud = abs(getValueStr(Data,sep,7).toInt()) > 0 ? getValueStr(Data,sep,8):"0";
    longitud = abs(getValueStr(Data,sep,8).toInt()) > 0 ? getValueStr(Data,sep,9):"0";
    fecha = abs(getValueStr(Data,sep,9).toInt()) > 0 ? getValueStr(Data,sep,10):"0";
    
    serialData = "";
    serialData += tempT + sep;
    serialData += humT + sep;
    serialData += presAtT + sep;
    serialData += alcoholT + sep;
    serialData += tvocT + sep;
    serialData += co2T + sep;
    serialData += metanoT + sep;
    
    serialData += latitud + sep;
    serialData += longitud + sep;
    serialData += fecha + sep;
    
    serialData += "https://www.google.com/maps/@" + latitud + "," + longitud + ",15z";


    if(millis() - oldTime >= minTime  & (abs(temp.toInt()-tempT.toInt()) >= 1 ||  abs(hum.toInt()-humT.toInt()) >= 1 ||  abs(presAt.toInt()-presAtT.toInt()) >= 1 ||  abs(alcohol.toInt()-alcoholT.toInt()) >= 1 ||  abs(tvoc.toInt()-tvocT.toInt()) >= 1 ||  abs(co2.toInt()-co2T.toInt()) >= 1 ||  abs(metano.toInt()-metanoT.toInt()) >= 1))
    {
      Serial.print("Guardado la informacion por que las mediciones han cambiado: ");
      Serial.println(serialData);

      Serial.print("Data -- ");
      Serial.println(Data);
      saveDataSD(serialData);
      publishData(tempT.toFloat(), humT.toFloat(), presAtT.toFloat(), alcoholT.toFloat(), tvocT.toFloat(), co2T.toFloat(), metanoT.toFloat(), NH4.toFloat(), latitud, longitud, fecha);
      Data = "";

      temp = tempT;
      hum = humT;
      presAt = presAtT;
      alcohol = alcoholT;
      tvoc = tvoc;
      co2T = co2;
      metanoT = metano;

      break;
    }    
  }
}
void getDataFromArduino(){
  char inByte;
  if (arduinoSerial.available() > 0) {
    Data = arduinoSerial.readStringUntil('/r');
    newData = true; 
  }
}
void saveDataSD(String Data){
  if(validData & newData){
    Archivo = SD.open(String(defaultFileName+defaultFileCounter+defaultFileExtension), FILE_WRITE);
    delay(1);
    Archivo.println(Data);
    delay(1);
    Archivo.close();
    contadorDatos++;
    Serial.println("Data saved");
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void inicioDeAlmacenamiento(String fileNameAndExtension){
  Archivo = SD.open(fileNameAndExtension, FILE_WRITE);  
  Serial.println("EL ARCHIVO EN LA SD SE LLAMARA: "+fileNameAndExtension);
    

    Archivo.println("SEP="+sep);
    
    Archivo.print("Temperatura (ºC)");
    Archivo.print(sep);

    Archivo.print("Humedad (%)");
    Archivo.print(sep);

    Archivo.print("Presion atmosferica (mBar)");
    Archivo.print(sep);

    Archivo.print("Alcohol (ppm)");
    Archivo.print(sep);


    Archivo.print("TVOC (ppm)");
    Archivo.print(sep);

    
    Archivo.print("CO2 (ppb)");
    Archivo.print(sep);
    
    Archivo.print("Gas metano (ppm)");
    Archivo.print(sep);
    
    Archivo.print("latitud");
    Archivo.print(sep);  

    Archivo.print("longitud");
    Archivo.print(sep);
    
    Archivo.print("Fecha");
    Archivo.println(sep);
    
    Archivo.close();
}
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
}
String getValueStr(String data, char separator, int index)
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
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "0";
}
void publishData(double temp, double hum, double presAlt, double alcoholPPM, double TVOC, double CO2, double Metano, double NH4, String latitud, String longitud, String fecha){
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
    Serial.println("Sending message to MQTT topic..");
    Serial.println(JSONmessageBuffer);
    
    if (client.publish(outTopic, JSONmessageBuffer) == true) {
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }
    /*
    root.printTo(Data); //Almaceno el json generado en la variable Data
      Serial.print("El json es: "); Serial.println(Data);
      char msg[Data.length()];
      Data.toCharArray(msg,Data.length());
      
      client.publish(outTopic, msg);
      */
}

double stringToDouble(String & str)  // <-- notice the "&"
{
  return atof( str.c_str() );
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Testing");
      // ... and resubscribe

      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
