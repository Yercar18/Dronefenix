#include "PROCESS_DATA.h"
#include <ArduinoJson.h>
#include "configuration.h"

String PROCESS_DATA::ensamblarMensajeJSON(double temp, double hum, double presAlt, double alcoholPPM, double TVOC, double CO2, double Metano, double NH4, float latitud, float longitud, double fecha){
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

    
    root["D9"] =  latitud; 
    root["D10"] = longitud; 
    root["D11"] = fecha;
        
    char JSONmessageBuffer[260];
    root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    return JSONmessageBuffer;
}


double PROCESS_DATA::leerTemperatura()
{
  return stringToDouble(temp);
}
double PROCESS_DATA::leerHumedad()
{
  return stringToDouble(hum);
}
double PROCESS_DATA::leerPresionAtmosferica()
{
  return stringToDouble(presAt);
}
int PROCESS_DATA::leerTVOC()
{
  return stringToDouble(tvoc);
}
int PROCESS_DATA::leerAlcohol()
{
  return stringToDouble(alcohol);
}
int PROCESS_DATA::leerCO2()
{
  return stringToDouble(co2);
}
int PROCESS_DATA::leerMetano()
{
  return stringToDouble(metano);
}
int PROCESS_DATA::leerNH4()
{
  return stringToDouble(NH4);
}
float PROCESS_DATA::leerLatitud()
{
  return stringToDouble(latitud);
}
float PROCESS_DATA::leerLongitud()
{
  return stringToDouble(longitud);
}
double PROCESS_DATA::leerFecha()
{
  return stringToDouble(fecha);
}

String PROCESS_DATA::mensajeSDTabulado()
{
  return temp + tabulador + hum + tabulador + presAt + tabulador + alcohol + tabulador +tvoc + tabulador + co2 + tabulador + metano + tabulador + NH4 + tabulador + latitud + tabulador + longitud + tabulador + fecha;
}

bool PROCESS_DATA::procesarInformacion(String Data)
{
    //Elimniar espacios en blanco
    Data.trim();

    if(Data.length() >0)
    {
      int valuePointer = 0;
      String initCharacterReceived = getValueStr(Data,sep,valuePointer);
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
      
      String endCharacterReceived = getValueStr(Data,sep,valuePointer);
      
      if(initCharacterReceived == initialCharacter && endCharacterReceived == endCharacter)
      {
        if(serDebug) Serial.println("La informacion leida es valida");
        return true;
      }
      else
      {
        if(serDebug) Serial.print("Data invalida - ");
        if(serDebug) Serial.println("Caracter de inicio: " + initCharacterReceived + " caracter de fin: " + endCharacterReceived);
        return false;
      }
    }
    else
    {
      //Si no ha leido data
      return false;
    }
}

bool PROCESS_DATA::getValue(String data, char separator, int index)
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

String PROCESS_DATA::getValueStr(String data, char separator, int index)
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

double PROCESS_DATA::stringToDouble(String & str)  // <-- notice the "&"
{
  return atof( str.c_str() );
}
