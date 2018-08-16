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
#define itemSeparator ","; //Item que separa los datos guardados en el .csv Por defecto es "," Comma Separated Value
#define timeDelay 500
#define maxPosString 11
#define decimals 7
#define inputLength 53 //Largo de la palabra recibida

boolean statusFlag, ledFlag;
int intentos = 0, i = 0;
int defaultFileCounter = 0;
float flat, flon,alt, AirQ;
String inputString, outString = "", tempString;
String fecha ;
char inputChar[inputLength];

String defaultFileName = "datos"; // nombre del archivo por defecto
String defaultFileExtension = ".csv";

const int gpsTxPin =  8;
const int gpsRxPin =  9;


//Variables especiales de la libreria del Gps - special variables of gps

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);
unsigned long age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;
  

//inicializacion de librerias globales

TinyGPS gps;
File Archivo;
SoftwareSerial gpsSerial(gpsTxPin, gpsRxPin);
SoftwareSerial debugSerial(7,6);

void setup() {
  //inicializacion del puerto serie
  Serial.begin(9600); //El puerto serie nativo es empleado para recibir los datos
  gpsSerial.begin(9600);
  debugSerial.begin(9600);
  //Declaracion de los pines
  pinMode(dispCam, INPUT);
  pinMode(AirSensor,INPUT);
  pinMode(ledStatus,OUTPUT);
  while(!inicializacion() && intentos<=5){
  //init
  }
}

void loop() {
  //interrogar al arduino pro mini 1
  readGPS();
  AirQ = readAirQual();
  readProMini1();
  outString = "";
  outString += itemSeparator;
  outString += AirQ;
  outString += itemSeparator;
  outString += flat;
  outString += tempString;
  outString += itemSeparator;
  outString +=  flon;
  outString += itemSeparator;
  outString += alt;
  debugSerial.write(inputChar);
  publishToWifiModule(outString);
  if(digitalRead(dispCam)){
    //Publicar
    
  }
}
String charToString(char* inCharString){
  i = 0; tempString="";
  while (i != inputLength) 
  {
    tempString = inCharString[i];
    i += 1;
  }
  return tempString;
}
void publishToWifiModule(String publishString){
  //Se envia por el puerto serie normal
  debugSerial.println(publishString);
}
int readAirQual(){
  return analogRead(AirSensor);
}
void readProMini1(){
  Serial.readBytes(inputChar,inputLength);
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
  debugSerial.println("--------------------------------------------------");
  if (!SD.begin(SD_SS)) {
    showError("SD");
    statusFlag = statusFlag && false;
  }else{
      Serial.print("SD init OK");
      statusFlag = statusFlag && true; 
      // deteccion de errores - intentar abrir el archivo para verificar que existe
      while(SD.exists(defaultFileName+defaultFileExtension)){
        Serial.println("EXISTE: "+defaultFileName+defaultFileCounter+defaultFileExtension);
        Serial.println(SD.exists(defaultFileName+defaultFileCounter+defaultFileExtension));  
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
    debugSerial.print("Errores: ");
    debugSerial.print(intentos);
    debugSerial.print(" ,Fallo el sensor: ");
    debugSerial.println(sensor);
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


  static void print_float(float val, float invalid, int len, int prec)
  {
    if (val == invalid)
    {
      while (len-- > 1)
        Serial.print('*');
      //Serial.print(';');
    }
    else
    {
      Serial.print(val, prec);
      int vi = abs((int)val);
      int flen = prec + (val < 0.0 ? 2 : 1); // . and -
      flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
      for (int i = 1; i > 1; ++i)
        Serial.print(' ');
    }
    smartdelay(0);
    Serial.print(';');
  }

  static void print_float2(float val, float invalid, int len, int prec)
  {
    if (val == invalid)
    {
      while (len-- > 1)
        Serial.print('*');
      //Serial.print(';');
    }
    else
    {
      //Serial.print(val, prec);
      alt = val ; //**GUARDO LA altitud **//
      int vi = abs((int)val);
      int flen = prec + (val < 0.0 ? 2 : 1); // . and -
      flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
      for (int i = 1; i > 1; ++i)
        Serial.print(' ');
    }
    smartdelay(0);
    //Serial.print(';');
  }
  static void print_int(unsigned long val, unsigned long invalid, int len)
  {
    char sz[32];
    if (val == invalid)
      strcpy(sz, "*******");
    else
      sprintf(sz, "%ld", val);
    sz[len] = 0;
    for (int i = strlen(sz); i < len; ++i)
      sz[i] = ';';
    if (len > 0)
      sz[len - 1] = ';';
    Serial.print(';');
    smartdelay(0);
  }

  static void print_date(TinyGPS & gps)
  {
    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long age;
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
    if (age == TinyGPS::GPS_INVALID_AGE)
      Serial.print("********** ********");
    else
    {
      char sz[32];
      sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
              month, day, year, hour, minute, second);
      //Serial.print("la fecha de hoy es :   ");
      //Serial.print(sz);
      //Serial.println();
      fecha = sz;//**GUARDO EN MEMORIA LA FECHA
    }
    //print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
    smartdelay(0);
  }

  static void print_str(const char *str, int len)
  {
    int slen = strlen(str);
    for (int i = 0; i < len; ++i)
      Serial.print(i < slen ? str[i] : ';');
    smartdelay(0);
  }
