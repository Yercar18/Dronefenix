#include "SD_PROCESS.h"
#include "configuration.h"
#include <SD.h>   
//For sd support
#include <SPI.h>
#include <time.h>

const String defaultFileName = "datos", defaultFileExtension = ".csv";
int timezone = -5 * 3600;
int dst = 0;

File Archivo;

void SD_PROCESS::setFileCounter(int val){
     __defaultFileCounter = val;
}
void SD_PROCESS::setNumError(int val){
     __numError = val;
}
String SD_PROCESS::getTime()
{
      time_t now = time(nullptr);
      struct tm* p_tm = localtime(&now);
      __fecha = String(p_tm->tm_mday) + "/" +  String(p_tm->tm_mon + 1) + "/" + String(p_tm->tm_year + 1900) + " - " +  String(p_tm->tm_hour) + ":" + String(p_tm->tm_min) + ":" + String(p_tm->tm_sec);
      return __fecha;
}

void SD_PROCESS::inicializar(){
  
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  
  if (!SD.begin(chipSelect) & serDebug) {
    Serial.println("SD CARD Lecture failed");
  }
  
  //Busca el nombre de archivo
  String fileName = String(defaultFileName+__defaultFileCounter+defaultFileExtension);
  int contadorSDFiles = 0;
  
  while(SD.exists(fileName) & (contadorSDFiles<100)){
    __defaultFileCounter +=1;  
    fileName = String(defaultFileName+__defaultFileCounter+defaultFileExtension);
    Serial.println("EXISTE: "+ fileName);
    delay(__defaultFileCounter);
    contadorSDFiles += 1;
    }
    Serial.println("Numero de archivos: " + String(contadorSDFiles));
    if(contadorSDFiles >= 100)
    {
      __defaultFileCounter = 0;
      //Clean sd
      for(int i = 0; i<=100 ; i++)
      {
          __defaultFileCounter +=1; 
          fileName = defaultFileName+__defaultFileCounter+defaultFileExtension;
          SD.remove(fileName);
          Serial.println("Eliminado = " + String(fileName));
          delay(timeDelay/20);
      }
    }
    __fileNameAndExtension = fileName;
}

void SD_PROCESS::guardarInfo(String Data){
    Archivo = SD.open(__fileNameAndExtension, FILE_WRITE);  
    delay(minDelay);
    Archivo.println(Data);
    delay(minDelay);
    Archivo.close();
    if(serDebug) Serial.println("Data saved");
}

void SD_PROCESS::saveIntoLogMsg(String msg, int freeSpace, String WiFiStatus, String mqttStatus, bool isError)
{
  getTime(); //Actualizar la fecha
  msg.replace("\r", "");
  msg.replace("\n", "");
  msg.replace("\r\n", "");
  if(__numError!=0)
  {
    uint32_t freeSpace = 0;

    String encabezado = isError?"Error":"Log";
    String linea = encabezado  + "(" + __fecha  +  ")" + tabulador + "Mensaje: " + msg + tabulador + "Estado WiFi: " + WiFiStatus + tabulador + "mqttEstatus: " + mqttStatus + tabulador + "Consecutivo: " + String(__numError) + tabulador + "Maximo permitido: " + String(maxNumError) + tabulador + "FREE MEMORY: " + String(freeSpace);

     
    Archivo = SD.open(String("log.txt"), FILE_WRITE);
    delay(minDelay);
    Archivo.println(linea);
    delay(minDelay);
    Archivo.close();
      
    if(serDebug) Serial.println(linea);
    
    if(isError) __numError++;
    if(__numError>=maxNumError & isError)
    {
      __numError = 0;
      if(serDebug)  Serial.println("Reseting arduino");
      digitalWrite(arduinoResetPin, LOW);
      delay(timeDelay);
      digitalWrite(arduinoResetPin, HIGH);
      setup();
    } 
  }
  else
  {
    __numError++; //Only if error  == 0
  }
}


void SD_PROCESS::guardarEncabezados(){
  Archivo = SD.open(__fileNameAndExtension, FILE_WRITE);  
  if(serDebug) Serial.println("EL ARCHIVO EN LA SD SE LLAMARA: "+__fileNameAndExtension);
    

    Archivo.println("SEP="+sep);

    delay(minDelay);
    Archivo.print("Temperatura (ºC)");
    Archivo.print(sep);
    
    delay(minDelay);
    Archivo.print("Humedad (%)");
    Archivo.print(sep);

    delay(minDelay);
    Archivo.print("Presion atmosferica (mBar)");
    Archivo.print(sep);

    delay(minDelay);
    Archivo.print("Alcohol (ppm)");
    Archivo.print(sep);


    delay(minDelay);
    Archivo.print("TVOC (ppm)");
    Archivo.print(sep);

    
    delay(minDelay);
    Archivo.print("CO2 (ppb)");
    Archivo.print(sep);
    
    delay(minDelay);
    Archivo.print("Gas metano (ppm)");
    Archivo.print(sep);
    
    delay(minDelay);
    Archivo.print("latitud");
    Archivo.print(sep);  

    delay(minDelay);
    Archivo.print("longitud");
    Archivo.print(sep);
    
    delay(minDelay);
    Archivo.print("Fecha");
    Archivo.println(sep);
    
    delay(minDelay);
    Archivo.close();
}
