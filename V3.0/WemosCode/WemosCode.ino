#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define timeDelay 2400
#define sep ','
#define ledWarning 13
#define BAUD_RATE 115200
#define chipSelect D4
#define buttonPin D8
#define RelayPin D7

#define test true //Este parametro debe estar en falso para estar en produccion

const String defaultFileName = "datos";
const String defaultFileExtension = ".csv";

//Configuraciones de red
const char* ssid = "MIGUELANGEL";
const char* password = "administrador5612";
const char* mqtt_server = "192.168.1.53";
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
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  arduinoSerial.println("");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setServer(mqtt_server, 1883);
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
    getDataFromArduino();
   if(newData){
     ValidateAndShowData();
   }
   if(validData & newData){
     if(client.connected() ){
        Data = getValueStr(Data,'\r',0);
        char msg[Data.length()];
        Data.toCharArray(msg,Data.length());
        client.publish(outTopic, msg);
        delay(timeDelay/10);
        Serial.print("Data published: ");Serial.print(Data);Serial.println(" <--- Hasta aqui");
        Data = "";newData = false;validData = false;
      }
    }
    smartDelay(timeDelay); 
}
void smartDelay(int timeWait){
  unsigned long oldTime = millis();
  while(millis()-oldTime<=timeWait){
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    if((digitalRead(buttonPin) || test)){
        saveDataSD(getValueStr(Data,'\r',0));
    }
    delay(timeDelay/10);
  }
}
void getDataFromArduino(){
  while (arduinoSerial.available() > 0) {
    char inByte = arduinoSerial.read();
    Data += inByte;
    newData = true; 
    validData = false; //to validate
    yield();
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
    Data = "";newData = false;validData = false;
    Serial.println("Data saved");
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

void inicioDeAlmacenamiento(String fileNameAndExtension){
  Archivo = SD.open(fileNameAndExtension, FILE_WRITE);  
  Serial.println("EL ARCHIVO EN LA SD SE LLAMARA: "+fileNameAndExtension);
    

    Archivo.println("SEP="+sep);
    
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
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "hello world");
      // ... and resubscribe
      //client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    yield();
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

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(RelayPin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(RelayPin, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}
