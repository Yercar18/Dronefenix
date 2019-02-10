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

bool serDebug = true; //Para activar o desactivar el debug serial


int numError = 0; //Contador para saber cuantas veces ocurre un error
int maxNumError = 20; //Numero de errores maximo permitido antes de reiniciar el arduino


#define ledWarning led_builtin
#define BAUD_RATE 115200
#define chipSelect D4
#define timeDelay 5*60*1000 // 5 minutos * 60 segundos * 1000 milisegundos
#define minTime timeDelay/2 //Tiempo minimo para enviar una actualizacion
#define sep ','

const char* mqtt_server = "157.230.174.83";
const char* outTopic = "droneFenix/2/estacion1";
const char* inTopic = "droneFenix/2/estacion1IN";
const char* wiFiname = "AirQ_droneFenix/2/estacion1";
const int serverPort = 1883;
unsigned long lastMQTTConnectionAttempt, oldTime;

const String defaultFileName = "datos";
const String defaultFileExtension = ".csv";

String temp, hum, presAt, alcohol,tvoc, co2, metano, NH4,  latitud, longitud;
String fecha;
//temporales
String tempT, humT, presAtT, alcoholT,tvocT, co2T, metanoT, latitudT, longitudT, fechaT;
int contadorSDFiles = 0; //Esto es para calcular cuantos archivos hay en la sd

String Data = "", serialData;
int defaultFileCounter = 0,contadorDatos = 0, contadorFallas = 0, contadorInfoValida = 0;
boolean newData = false, validData = false;


File Archivo;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
SoftwareSerial arduinoSerial(D3, D2, false, 256);// SoftwareSerial arduinoSerial(14, 12, false, 256);

void setup() {
  numError = 0;
  if(serDebug)
  {
    Serial.begin(BAUD_RATE);
  }
  arduinoSerial.begin(BAUD_RATE);
  if (!SD.begin(chipSelect)) {
    reportError("SD CARD Lecture failed");
  }
  
  WiFiManager wifiManager;
  while (!wifiManager.autoConnect(wiFiname)) {
      reportError("Connection to hostname failed, restarting in 5 seconds");
      delay(5000);
  }
  mqttClient.setServer(mqtt_server, serverPort);
  mqttClient.setCallback(callback); 

   while(SD.exists(String(defaultFileName+defaultFileCounter+defaultFileExtension)) & contadorSDFiles<100){
    debugSerial("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
    defaultFileCounter +=1;  
    delay(defaultFileCounter);
    contadorSDFiles += 1;
    }
    debugSerial("Numero de archivos: " + String(contadorSDFiles));
    if(contadorSDFiles >= 100)
    {
      defaultFileCounter = 0;
      //Clean sd
      for(int i = 0; i<=100 ; i++)
      {
          defaultFileCounter +=1; 
          String nombre = defaultFileName+defaultFileCounter+defaultFileExtension;
          SD.remove(nombre);
          debugSerial("Eliminado = " + String(nombre));
      }
    }
    inicioDeAlmacenamiento(String(defaultFileName+defaultFileCounter+defaultFileExtension));// Esta funcion verifica la integridad del archivo y realiza la insecion de las caveceras
    delay(2000);
}


void loop() {
  smartDelay(timeDelay);
}

void debugSerialNCR(String msg)
{
  if(serDebug)
  {
    //Serial.listen();
    //delay(1000); // Give it time to send a response or you'll get nothing!
    Serial.print(msg);
  }
}
void debugSerial(String msg)
{
  if(serDebug)
  {
    //Serial.listen();
    //delay(1000); // Give it time to send a response or you'll get nothing!
    Serial.println(msg);
  }
}
void smartDelay(int timeWait){
  String fileName = String(defaultFileName+defaultFileCounter+defaultFileExtension);

  while((millis()-oldTime)<=timeWait){
    
    
    if (!mqttClient.connected()) {
      if(lastMQTTConnectionAttempt == 0 || millis() > lastMQTTConnectionAttempt + 3 * 60 * 1000) {
        lastMQTTConnectionAttempt = millis();
        
        reportError("Trying to connect to mqtt");
        if (mqttClient.connect("ESP8266Client")) {
          //mqttClient.set_callback(mqttCallback);
          char topic[50];
          //sprintf(topic, "%s/+/+", settings.mqttTopic);
          //mqttClient.subscribe(topic);
          mqttClient.subscribe(inTopic);
          debugSerial("MQTT connection OK");

          //TODO multiple relays
          //updateMQTT(0);
        } else {
          reportError("failed to reconnect MQTT");
        }
      }
    } else {
      mqttClient.loop();
    }

    if(!newData)
    {
      //debugSerial("Intentando obtener info del arduino");
      getDataFromArduino();
      //debugSerial("Fin de la info del arduino");
    
      Data = getValueStr(Data,'\r',0);
      //debugSerial("Data leida");
    }
    //T ..listo para enviar
    int valuePointer = 0;
    String initCharacter = getValueStr(Data,sep,valuePointer);
    valuePointer++;
    temp = getValueStr(Data,sep,valuePointer).toInt() > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    hum = getValueStr(Data,sep,valuePointer).toInt() > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    presAt = getValueStr(Data,sep,valuePointer).toInt() > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    alcohol = getValueStr(Data,sep,valuePointer).toInt() > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    tvoc = getValueStr(Data,sep,valuePointer).toInt() > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    co2 = getValueStr(Data,sep,valuePointer).toInt() > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    metano = getValueStr(Data,sep,valuePointer).toInt() > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    NH4 = abs(getValueStr(Data,sep,valuePointer).toInt()) > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;

    latitud = abs(getValueStr(Data,sep,valuePointer).toInt()) > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    longitud = abs(getValueStr(Data,sep,valuePointer).toInt()) > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    fecha = abs(getValueStr(Data,sep,valuePointer).toInt()) > 0 ? getValueStr(Data,sep,valuePointer):"0";
    valuePointer++;
    
    String endCharacter = getValueStr(Data,sep,valuePointer);
    
    if(initCharacter == "INIT" && endCharacter == "END")
    {
      if(contadorInfoValida <= 0 || contadorInfoValida >= 20)
      {
        debugSerial("La informacion leida es valida");
        contadorInfoValida = 1;
      }
      else
      {
        debugSerialNCR(".");
        contadorInfoValida ++;
      }
      //Si la informacion es integra se guarda.
      tempT = temp;
      humT = hum;
      presAtT = presAt;
      alcoholT = alcoholT;
      tvocT = tvoc;
      co2T = co2;
      metanoT = metano;
      latitudT = latitud;
      longitudT = longitud;
      fechaT = fecha;

      
      serialData = "";
      serialData += tempT + sep;
      serialData += humT + sep;
      serialData += presAtT + sep;
      serialData += alcoholT + sep;
      serialData += tvocT + sep;
      serialData += co2T + sep;
      serialData += metanoT + sep;
      
      serialData += latitudT + sep;
      serialData += longitudT + sep;
      serialData += fechaT + sep;
      
      serialData += "https://www.google.com/maps/@" + latitud + "," + longitud + ",15z";
      delay(5000);
    }
    else
    {
      if(contadorFallas <= 0 || contadorFallas >= 50)
      {        
        reportError("error leyendo el puerto serie, los caracteres no son los esperados");
        contadorFallas = 1;
        delay(3000);
      }
      else if(contadorFallas >0){
        debugSerialNCR(".");
        contadorFallas++;  
      }
    }
    

  }
    debugSerial("He salido del while");
    delay(10000);

      
      
      oldTime = millis();  
      debugSerial("Guardado la informacion y enviandola por MQTT:");
      debugSerial(serialData);
      
      debugSerial("Data :");
      debugSerial(Data);
      saveDataSD(fileName,serialData);
      
      publishData(tempT.toFloat(), humT.toFloat(), presAtT.toFloat(), alcoholT.toFloat(), tvocT.toFloat(), co2T.toFloat(), metanoT.toFloat(), NH4.toFloat(), latitud, longitud, fecha);
      Data = "";

      if(tempT == 0 && humT == 0)
      {
        reportError("Temperatura y humedad no pueden ser cero al mismo tiempo");
      }
      else if(tvocT == 0 && co2T == 0)
      {
       reportError("tvoc y co2 no pueden ser cero al mismo tiempo");
      }
      else if(presAtT == 0)
      {
       reportError("La presion atmosferica no puede ser cero");
      }

      newData = false;
    
    
    
}
void freeSpaceReportSerial()
{
      uint32_t freeSpace = system_get_free_heap_size();
      debugSerial("*************FREE MEMORY*******************");
      debugSerial(String(freeSpace));
}
void getDataFromArduino(){
  char inByte;
  arduinoSerial.listen();
  delay(1000); // Give it time to send a response or you'll get nothing!
  if (arduinoSerial.available() > 0) {
    Data = arduinoSerial.readStringUntil('/r');
    newData = true; 
  }
  else
  {
    newData = false;
  }
}
void saveDataSD(String fileNameAndExtension,String Data){
     Archivo = SD.open(fileNameAndExtension, FILE_WRITE);  
    delay(1);
    Archivo.println(Data);
    delay(1);
    Archivo.close();
    contadorDatos++;
   debugSerial("Data saved");
}

void reportError(String msg)
{
  
  uint32_t freeSpace = system_get_free_heap_size();
    
  saveLogSD("****************Error detectado********************************");
  String linea = "Consecutivo: " + String(numError) + " ***" + "Maximo permitido: " + String(maxNumError) + " ***FREE MEMORY: " + String(freeSpace);
  saveLogSD(linea);
  saveLogSD(msg);


  debugSerial("****************Error detectado********************************");
  debugSerial(linea);
  debugSerial(msg);
  
  numError++;
  if(numError>=maxNumError)
  {
    WiFi.disconnect();
    delay(500);
    ESP.restart();
  }
}

void saveLogSD(String Data){
    
    Archivo = SD.open(String("log.txt"), FILE_WRITE);
    delay(1);
    Archivo.println(Data);
    delay(1);
    contadorDatos++;
    debugSerial("Log saved");

    delay(1);
    Archivo.close();
      
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

void inicioDeAlmacenamiento(String fileNameAndExtension){
  Archivo = SD.open(fileNameAndExtension, FILE_WRITE);  
  debugSerial("EL ARCHIVO EN LA SD SE LLAMARA: "+fileNameAndExtension);
    

    Archivo.println("SEP="+sep);

    delay(1);
    Archivo.print("Temperatura (ºC)");
    Archivo.print(sep);
    
    delay(1);
    Archivo.print("Humedad (%)");
    Archivo.print(sep);

    delay(1);
    Archivo.print("Presion atmosferica (mBar)");
    Archivo.print(sep);

    delay(1);
    Archivo.print("Alcohol (ppm)");
    Archivo.print(sep);


    delay(1);
    Archivo.print("TVOC (ppm)");
    Archivo.print(sep);

    
    delay(1);
    Archivo.print("CO2 (ppb)");
    Archivo.print(sep);
    
    delay(1);
    Archivo.print("Gas metano (ppm)");
    Archivo.print(sep);
    
    delay(1);
    Archivo.print("latitud");
    Archivo.print(sep);  

    delay(1);
    Archivo.print("longitud");
    Archivo.print(sep);
    
    delay(1);
    Archivo.print("Fecha");
    Archivo.println(sep);
    
    delay(1);
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
    debugSerial("Sending message to MQTT topic..");
    debugSerial(JSONmessageBuffer);
    
    if (mqttClient.publish(outTopic, JSONmessageBuffer) == true) {
      debugSerial("Success sending message");
    } else {
      reportError("Error sending message");
    }
    delay(10000);
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
