#include "PROCESS_DATA.h"
#include "configuration.h"

String tempT, humT, presAtT, alcoholT,tvocT, co2T, metanoT, latitudT, longitudT, fechaT, Data = "", serialData, fileName, temp, hum, presAt, alcohol,tvoc, co2, metano, NH4,  latitud, longitud, fecha;

bool PROCESS_DATA::processData(String Data)
{
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
      Serial.println("La informacion leida es valida");
      return true;
    }
    else
    {
      Serial.println("Data invalida");
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
