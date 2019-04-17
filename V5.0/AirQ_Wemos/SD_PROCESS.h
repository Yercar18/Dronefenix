#ifndef SD_PROCESS_H
#define SD_PROCESS_H

#include "Arduino.h"

class SD_PROCESS {
  public:
    String inicializar();
    void guardarInfo(String fileNameAndExtension,String Data);
    void guardarEncabezados();
    void reportError(String msg, int freeSpace, String WiFiStatus, String mqttStatus);
    void setFileCounter(int val);
    void setNumError(int val);
  private:
    int __defaultFileCounter;
    int __numError;
};


#endif // SD_PROCESS_H
