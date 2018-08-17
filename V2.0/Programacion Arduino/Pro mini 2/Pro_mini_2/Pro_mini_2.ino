//Librerias empleadas

#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SD.h>

//Variables del programa
/*Pines de la memoria micro SD
 * SS --> 10
 * MOSI -->11
 * MISO --> 12 
 * CLK --> 13
 */
#define SD_SS 10
#define ledStatus 13
#define dispCam A7
#define AirSensor A6
#define itemSeparator ',' //Item que separa los datos guardados en el .csv Por defecto es "," Comma Separated Value
#define timeDelay 50
#define maxPosString 11
#define decimals 7
#define inputLength 53 //Largo de la palabra recibida
#define aditionalCharLen 4 //Char que llega por defecto en el puerto serie como \r\n
#define startChar 'A' //Palabra de inicio "AXXXXZ"
#define endChar 'Z' //Palabra de final "AXXXXXZ"
#define gpsTxPin  8
#define gpsRxPin  9
#define baudRate 115200  
#define baudRateGPS 9600

boolean statusFlag, ledFlag;
int intentos = 0, intentosLecturaSerie = 0, i = 0;
int defaultFileCounter = 0;
float flat, flon,alt, AirQ;
String inputString;
String fecha ;
String outString1;String outString2;
String defaultFileName = "datos"; // nombre del archivo por defecto
String defaultFileExtension = ".csv";



//Variables especiales de la libreria del Gps - special variables of gps

static void smartdelay(unsigned long ms);
unsigned long age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;
  

//inicializacion de librerias globales

TinyGPS gps;
File Archivo;
SoftwareSerial gpsSerial(gpsTxPin, gpsRxPin);

void setup() {
  //inicializacion del puerto serie
  Serial.begin(baudRate); //El puerto serie nativo es empleado para recibir los datos
  gpsSerial.begin(baudRateGPS);
  //Declaracion de los pines
  pinMode(dispCam, INPUT);
  pinMode(AirSensor,INPUT);
  pinMode(ledStatus,OUTPUT);
  while(!inicializacion() && intentos<=5){
  //init
  }
}

void loop() {
    readProMini1();
    //interrogar al arduino pro mini 1
    boolean banderaDisparo = digitalRead(dispCam);
    readGPS();
    AirQ = readAirQual();
  
    if(banderaDisparo){
      //Publicar
    }
    outString1 =  ""; outString2="";
    outString1 += concatWord(String(fecha), 22);
    outString1 += concatWord(String(flat), 12);
    outString1 += concatWord(String(flon), 12);
    outString1 += concatWord(String(alt),7);
    outString2 += concatWord(String(AirQ), 8);
    outString2 += concatWord(String(banderaDisparo), 1);
    Serial.println("DEBUG OK");
    Serial.print(outString1);
    Serial.print(inputString);
    Serial.print(outString2);
    Serial.println(""); 
    Serial.println("OK");
  
    Serial.println("RECOMENZANDO");
}

String concatWord(String value , int len){
  int lenVal = value.length();
  String outWord="";
  if(lenVal<len){
    int iteraciones = len - lenVal - 1 ;
    for(int i= 0; i<=iteraciones;i++){
      outWord.concat(String(0));
    }
    outWord.concat(value);  
    return (outWord + itemSeparator) ;
  }else{
    return (value + itemSeparator) ;
  }
}
void saveInFile(String wordToWrite){
    Archivo.print(wordToWrite);
    Archivo.println(itemSeparator);
}
void inicioDeAlmacenamiento(String fileNameAndExtension){
  Archivo = SD.open(fileNameAndExtension, FILE_WRITE);  
  showError("EL ARCHIVO EN LA SD SE LLAMARA: "+fileNameAndExtension);
  if (Archivo) {
    
    Archivo.println("SEP="+itemSeparator);
    saveInFile("Fecha/hora de la muestra");   
    saveInFile("Latitud");
    saveInFile("Longitud");
    saveInFile("Altitud");
    saveInFile("Temperatura BMP_180 (ºC)");
    saveInFile("Temperatura DHT11 (ºC)");
    saveInFile("Temperatura DHT11 (ºF)");
    saveInFile("Temperatura CCS_811 (ºC)");
    saveInFile("Humedad relativa DHT11 (%)");
    saveInFile("Niveles de TVOC PPB");
    saveInFile("Nivel de CO2 PPM");
    saveInFile("Presion atmosferica");
    saveInFile("Calidad del aire (MQ-3)");
    saveInFile("Activacion desde el piloto automatico");
  }else {
    showError("SD CARD");
  }
  Archivo.close();
}

int readAirQual(){
  return analogRead(AirSensor);
}
void readProMini1(){
  statusFlag = false;
  char inputChar;
  int count = 0;
  inputString = "";
  while (Serial.available()>0) {
          inputChar = Serial.read();  //gets one byte from serial buffer
          inputString += inputChar; //makes the string readString  
          count += 1;   
          if(inputChar==endChar || inputChar == '\n') {
            inputString = inputString.substring(2,inputString.length()-2) + itemSeparator;// Este es el string de salida    
            Serial.println(inputString);
            return;
          }
  }
}
  
void readGPS(){
  sentences = 0, failed = 0;chars = 0;
  gps.f_get_position(&flat, &flon, &age);
  gps.stats(&chars, &sentences, &failed);
  if ((flat == 1000 && flon == 1000)  ) {
    showError("GPS: flat = 1000, flon = 1000");
  }
}

boolean inicializacion(){
  statusFlag = true;
  Serial.println("--------------------------------------------------");
  if (!SD.begin(SD_SS)) {
    showError("SD");
    statusFlag = statusFlag && false;
  }else{
      showError("SD init OK");
      statusFlag = statusFlag && true; 
      // deteccion de errores - intentar abrir el archivo para verificar que existe
      while(SD.exists(defaultFileName+defaultFileExtension)){
        showError("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
        //Serial.println(SD.exists(defaultFileName+defaultFileCounter+defaultFileExtension));  
        defaultFileCounter +=1;  
      }
  }
  readGPS();

 
}

void showError(String sensor){
  if(intentos>5) intentos = 0;
  ledFlag = !ledFlag; //Cambio de estado (intermitente)
    digitalWrite(ledStatus,!ledFlag);
    delay(timeDelay);
    intentos += 1;
    Serial.print("Errores: ");
    Serial.print(intentos);
    Serial.print(" ,Fallo el sensor: ");
    Serial.println(sensor);
}



// Funciones para uso unico del GPS , importante no tocar! - Agradecimiento a TinyGPS por estas funciones.



  static void smartdelay(unsigned long ms)
  {
    unsigned long start = millis();
    do
    {
      while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
      }
      if (analogRead(dispCam) > 800) {
        ms = millis() - start - 10; // garantizo que se interrumpe si hay disparo
      }

    } while (millis() - start < ms);


  }

