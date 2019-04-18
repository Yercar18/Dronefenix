#include "SERIAL_COMMUNICATION.h"ç
#include "PROCESS_DATA.h"
#include "SD_PROCESS.h"
#include "configuration.h"

SERIAL_COMMUNICATION serial;
PROCESS_DATA procesamiento;
SD_PROCESS memoriaSD;


void setup() {

    memoriaSD.setFileCounter(0);
    memoriaSD.setNumError(0);
    
    serial.inicializar();
    memoriaSD.inicializar();
    
    if(serDebug) Serial.println("Saliendo del setup");
}


void loop() {
    String informacion = serial.leerArduino();
    if(procesamiento.procesarInformacion(informacion))
    {
      //Si la informacion es valida se debe proceder a guardar en la SD y enviar al MQTT server
      String lineaSDCard = procesamiento.mensajeSDTabulado();
      memoriaSD.guardarInfo(lineaSDCard);
      if(serDebug) Serial.println("Guardado en la sd: " + lineaSDCard);
      
      //Mensaje MQTT
      double temperatura = procesamiento.leerTemperatura();
      double humedad = procesamiento.leerHumedad();
      double presionAmosferica = procesamiento.leerPresionAtmosferica();
      int tvoc =  procesamiento.leerTVOC();
      int co2 = procesamiento.leerCO2();
      int alcohol = procesamiento.leerAlcohol();
      int metano = procesamiento.leerMetano();
      int NH4 = procesamiento.leerNH4();
      float latitud = procesamiento.leerLatitud();
      float longitud = procesamiento.leerLongitud();
      double fecha = procesamiento.leerFecha();

      String json2MQTT = procesamiento.ensamblarMensajeJSON(temperatura, humedad, presionAmosferica, alcohol, tvoc, co2, metano, NH4, latitud, longitud, fecha);

      
      if(serDebug) Serial.println("json: " + json2MQTT);  
    }
}
