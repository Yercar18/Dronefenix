#ifndef SD_PROCESS_H
#define SD_PROCESS_H

#include "Arduino.h"

class SD_PROCESS {
  public:
    void inicializar();
    void guardarInfo(String Data);
    void guardarEncabezados();
    void saveIntoLogMsg(String msg, int freeSpace, String WiFiStatus, String mqttStatus, bool isError);
    void setFileCounter(int val);
    void setNumError(int val);
    String getTime();
  private:
    int __defaultFileCounter;
    int __numError;
    String __fileNameAndExtension;
    String __fecha;
};


#endif // SD_PROCESS_H
