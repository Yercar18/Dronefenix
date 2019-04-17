#include "SD_PROCESS.h"
#include "configuration.h"
#include <SD.h>   
//For sd support
#include <SPI.h>

const String defaultFileName = "datos", defaultFileExtension = ".csv";


File Archivo;

void SD_PROCESS::setFileCounter(int val){
     __defaultFileCounter = val;
}

String SD_PROCESS::inicializar(){
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
    return fileName;
}

void SD_PROCESS::guardarInfo(String fileNameAndExtension,String Data){
    Archivo = SD.open(fileNameAndExtension, FILE_WRITE);  
    delay(minDelay);
    Archivo.println(Data);
    delay(minDelay);
    Archivo.close();
    contadorDatos++;
   debugSerial("Data saved");
}

void SD_PROCESS::reportError(String msg)
{

  if(numError!=0)
  {
    uint32_t freeSpace = 0;
      
    String linea = ("****************Error detectado********************************");
    linea += "\r\n" + "Consecutivo: " + String(numError) + " ***" + "Maximo permitido: " + String(maxNumError) + " ***FREE MEMORY: " + String(freeSpace);
    
    Archivo = SD.open(String("log.txt"), FILE_WRITE);
    delay(minDelay);
    Archivo.println(linea);
    delay(minDelay);
    Serial.println("Log saved");

    delay(minDelay);
    Archivo.close();
    
    String linea2 = "Estado WiFi: " + String(WiFi.status()) + " ,mqttEstatus: " + String(mqttClient.connected());
    if(serDebug) Serial.println(linea2);
    if(serDebug) Serial.println(msg);
  
  
    if(serDebug) Serial.println("****************Error detectado********************************");
    if(serDebug) Serial.println(linea);
    if(serDebug)  Serial.println(linea2);
    if(serDebug)  Serial.println(msg);
    
    numError++;
    if(numError>=maxNumError)
    {
      if(serDebug)  Serial.println("Reseting arduino");
      digitalWrite(arduinoResetPin, LOW);
      delay(timeDelay);
      digitalWrite(arduinoResetPin, HIGH);
      setup();
    } 
  }
  else
  {
    numError++; //Only if error  == 0
  }
}


void SD_PROCESS::guardarEncabezados(){
  Archivo = SD.open(fileNameAndExtension, FILE_WRITE);  
  if(serDebug) Serial.println("EL ARCHIVO EN LA SD SE LLAMARA: "+fileNameAndExtension);
    

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
