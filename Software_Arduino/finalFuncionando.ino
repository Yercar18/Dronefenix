/*
  ENSAMBLE DE UN DRONE BASADO EN UN SISTEMA SENSORIAL Y MULTIESPECTRAL PARA MEDIR VARIABLES  METEOROLÓGICOS
  Este programa permite generar un archivo excel en el cual
  se ven reflejados la temperatura , humedad , presion atmosferica
  y la geolocalizacion ( latitud , longitud en la cual se encuentra el dispositivo ).


   Materiales

  - SD CARD SHIELD
  - Arduino Mega Protoshield
  - Arduino Mega R3
  - Sensor CCS811

   Cableado
  Gps TX --> Arduino Mega RX 3
  Gps RX --> Arduino Mega TX 3
  BMP 180 conexion I2C , SDA and SCL.
  DHT 11 --> Pin 7 Arduino Mega
  ** El cableado de la sd shield deben modificarlo para que se acople al arduino mega pines 50,51,52  SD shield 11,12,13


  created 06 June 2016
  by Yerson Carrillo in collaboration with Miguel Angel Califa Urquiza
  modified 14 January 2017
  by Miguel Califa
  redesigned to redistribution

*/

//Importacion libreria - librarie import

#include <SPI.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include <SD.h>
#include "Adafruit_CCS811.h"

//Variables globales - globale variables
long consecutivo = 0;
float hum, ic, alt;
double temp, p;
String fecha ;
char status;
double T, P, p0, a;
float h, t, f, hif, hic;
int co2, tvoc, tempCCS;
const int ledWarning = 5 ; //** LED QUE INDICA ERRORES **//
const int dispCam = A0; //Pin que dispara la camara para tomar una foto , Pin that triggers the camera to take a picture.
const int gpsTxPin =  9;
const int gpsRxPin =  8;
Adafruit_CCS811 ccs;
#define DHTPIN 7



//Variables especiales de la libreria del Gps - special variables of gps

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

//inicializacion de librerias globales

TinyGPS gps;
File Archivo;
#define DHTTYPE DHT11   // DHT 11
SFE_BMP180 pressure;
DHT dht(DHTPIN, DHTTYPE);

void setup() {


  //Declaracion I/O de pines
  pinMode(dispCam, INPUT);
  pinMode(10, OUTPUT);
  pinMode(ledWarning, OUTPUT);


  //inicializacion de los puertos serie para comunicarse con los perifericos - computador - GPS
  Serial.begin(115200);
  Serial3.begin(9600);
  if (!ccs.begin()) {
    Serial.println("Failed to start sensor! Please check your wiring.");
    while (1);
  }

  //Aviso del encendido


  bienvenida();


  //incicializacion del modulo sd
  if (!SD.begin(10)) {
    Serial.println("INICIALIZACION DE SD FALLO!");
    while (1) {
      digitalWrite(ledWarning, HIGH);
      delay(500);
      digitalWrite(ledWarning, LOW);
      delay(500);
    }
    return;
  }

  //Aviso de que todo ha inicializado correctamente

  Serial.println("INIT OK "); // Esto lo puedes personalizar

  // deteccion de errores - intentar abrir el archivo para verificar que existe
  Archivo = SD.open("datos.csv", FILE_WRITE);
  if (Archivo) {
    //Archivo.write("Temperatura barometrico ; Humedad ; Presion absoluta ; Indice de calor ; latitud ; Logitud ; Altitud ; fecha;");
    Archivo.println(" ");

  } else {
    digitalWrite(ledWarning, HIGH);
    delay(500);
    digitalWrite(ledWarning, LOW);
    delay(500);
  }
  Archivo.close();

  // inicializacion del dht para medir temperatura y humedad relativa - prueba de errores en perifericos de medicion

  dht.begin();

  if (pressure.begin())
    Serial.println("BMP180 init success"); // aviso de la inicializacion exitosa del bmp 180
  else
  {

    Serial.println("BMP180 Error\n\n");
    while (1) {
      digitalWrite(ledWarning, HIGH);
      delay(250);
      digitalWrite(ledWarning, LOW);
      delay(250);

      // Pausa por siempre y alerta de fallo personalizada
    }
  }

inicioDeAlmacenamiento();
}
// final del setup , hasta aqui todos los perifericos estan armados y preparados para actuar en el momento que se necesiten

void loop()
{

  //este ciclo se repite n veces para grabar cada una de las mediciones

  //**VARIABLES DE USO EN LA EJECUCION DEL PROGRAMA**//
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  // Obtener las geolocalizaciones en las variables flat y flon

  gps.f_get_position(&flat, &flon, &age);
  gps.stats(&chars, &sentences, &failed);

  //validacion de las lecturas del gps , sino estas en un sitio cubierto por un satelite no puedo registrar los datos

  if ((flat == 1000 && flon == 1000)  ) { //flat!=1000 && flon!=1000 // CAMBIO PARA TEST EN SD


    Serial.println("Error leyendo el gps por que esta en una zona donde no recibe señal");
    for (int i = 0; i <= 2; i++) {

      digitalWrite(ledWarning, HIGH);
      delay(50);
      digitalWrite(ledWarning, LOW);
      delay(50);
    }
  }  else {
    digitalWrite(ledWarning, HIGH);
  }


  Serial.println();
  Serial.println();

  //el delay de siempre se reemplaza por un smart delay para realizar otras operaciones necesarias


  smartdelay(1000);
  if (analogRead(dispCam) > 800) {
    if (digitalRead(ledWarning) == 0) {
      digitalWrite(ledWarning, HIGH);
    } else {
      digitalWrite(ledWarning, LOW);
    }
    print_date(gps);
    print_float2(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);

    //LEO LOS DATOS
    leerDatos(flat, flon);
    //PUBLICO POR MEDIO DE LOS PERIFERICOS EXISTENTES(MONITOR SERIE)
    publicar(flat, flon);
    //ALMACENO EN MEMORIA SD

    almacenarDatos(flat, flon);

    Serial.println();
    Serial.println("Esperando GPS");
    Serial.println();
  }

}

//funciones de usuario independientes , invocadas durante la ejecucion del loop

void inicioDeAlmacenamiento(){
  Archivo = SD.open("datos.csv", FILE_WRITE);
  if (Archivo) {
    Archivo.print("------------------------------------------------ INICIO DE LA TOMA DE DATOS -------------------------------------------------");
    Archivo.println();
    
    Archivo.print("Temperatura");
    Archivo.print(";");

    Archivo.print("Humedad");
    Archivo.print(";");

    Archivo.print("Presion");
    Archivo.print(";");

    Archivo.print("Indice de calor");
    Archivo.print(";");


    Archivo.print("Latitud");
    Archivo.print(";");

    Archivo.print("Longitud");
    Archivo.print(";");

    Archivo.print("Altitud");
    Archivo.print(";");

    
    Archivo.print("Nivel de CO2 PPB");
    Archivo.print(";");
    
    Archivo.print("Niveles de TVOC PPB");
    Archivo.print(";");
    
    Archivo.print("Temperatura reportada por el CCS");
    Archivo.print(";");

  

    Archivo.print("Fecha/hora de la muestra");
    Archivo.println(";");


  }
}


void almacenarDatos(float flat2 , float flon2) {

  //** CONSTANTES temp,hum,p,ic; **///

  Archivo = SD.open("datos.csv", FILE_WRITE);
  if (Archivo) {
    Archivo.print(temp);
    Archivo.print(";");

    Archivo.print(hum);
    Archivo.print(";");

    Archivo.print(p);
    Archivo.print(";");

    Archivo.print(ic);
    Archivo.print(";");


    Archivo.print(flat2, 6);
    Archivo.print(";");

    Archivo.print(flon2, 6);
    Archivo.print(";");

    Archivo.print(alt);
    Archivo.print(";");

    
    Archivo.print(co2);
    Archivo.print(";");
    
    Archivo.print(tvoc);
    Archivo.print(";");
    
    Archivo.print(tempCCS);
    Archivo.print(";");

  

    Archivo.print(fecha);
    Archivo.println(";");




  } else {
    digitalWrite(ledWarning, HIGH);
    delay(500);
    digitalWrite(ledWarning, LOW);
    delay(500);
  }
  Archivo.close();

}




void leerDatos(float flat , float flon) {

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      //Serial.print("Esta es la temperatura actual: ");
      //Serial.print(T,2);
      //Serial.println("");

      //*** GUARDANDO EN LA VARIABLE GLOBAL ***//

      temp = T;

      //*** FIN DEL GUARDADO EN MEMORIA **//


      status = pressure.startPressure(3);

      if (status != 0)
      {
        delay(status);


        status = pressure.getPressure(P, T);

        if (status != 0)
        {

          //Serial.print("Esta es la presion absoluta medida por el bmp180 :  ");
          //Serial.print(P,2);
          //Serial.println("");

          //***ALMACENANDO EN VARIABLE GLOBAL **//
          p = P;

        }
        else {}
      }
      else {}
    }
    else {}
  }
  else {}

  leerTemperatura(flat, flon);
  leerCCS();

}
void leerCCS() {
  if (ccs.available()) {
    float temp = ccs.calculateTemperature();
    if (!ccs.readData()) {

      co2 = ccs.geteCO2();
      tvoc = ccs.getTVOC();
      tempCCS =  temp;

    }

  }
}
  void leerTemperatura(float flat , float flon)
  {



    h = dht.readHumidity();

    t = dht.readTemperature();

    f = dht.readTemperature(true);


    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("fallo leyendo el sensor DHT11!");
      return;
    }


    hif = dht.computeHeatIndex(f, h);

    hic = dht.computeHeatIndex(t, h, false);

    //**** ALMACENANDO EN MEMORIA ****///

    ic = hic;
    hum = h;


  }

  void publicar(float latitud, float longitud) {

    consecutivo++;

    Serial.print("****************** ");
    Serial.print(consecutivo);
    Serial.println(" ******************");

    Serial.print("Reporte de datos numero : ");
    Serial.println(consecutivo);

    Serial.print("Fecha actual internacional : ");
    Serial.println(fecha);

    Serial.print("latitud: ");
    Serial.println(latitud, 8);

    Serial.print("longitud: ");
    Serial.println(longitud, 8);


    //****LECTURA DE LA HUMEDAD **//

    Serial.print("Este es la temperatura actual  :   ");
    Serial.print(temp);
    Serial.println("  °C");


    h = dht.readHumidity();

    //****LECTURA DE LA HUMEDAD **//

    Serial.print("Este es la humedad actual  :   ");
    Serial.print(h);
    Serial.println(" %");

    ///** LECTURA DEL INDICE DE CALOR**//

    Serial.print("Este es el inidce de calor actual :   ");
    Serial.println(hic);



    Serial.print("Esta es la presion absoluta medida por el bmp180 :  ");
    Serial.print(P, 2);
    Serial.println("");

    Serial.print("Estas son las lecturas del CCS811:  ");

    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.print("ppm, TVOC: ");
    Serial.print(tvoc);
    Serial.print("ppb   Temp:");
    Serial.println(tempCCS);
    Serial.println("");


    Serial.print("-------------------");
    Serial.print("");
    Serial.print(consecutivo);
    Serial.println("--------------------");

    Serial.println();

    Serial.println();

  }
  void bienvenida() {
    Serial.println(); Serial.println(); Serial.println();
    Serial.print("ENSAMBLE DE UN DRONE BASADO EN UN SISTEMA SENSORIAL Y MULTIESPECTRAL PARA MEDIR VARIABLES  METEOROLÓGICOS v.1.0 ");
    Serial.println("Yerson Carrillo And Miguel Califa");
    Serial.println();
    Serial.println();
    Serial.print("Lic : 2016 ");

    Serial.println();

  }
  // Funciones para uso unico del GPS , importante no tocar! - Agradecimiento a TinyGPS por estas funciones.



  static void smartdelay(unsigned long ms)
  {
    unsigned long start = millis();
    do
    {
      while (Serial3.available()) {
        gps.encode(Serial3.read());
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
