#ifndef SD_PROCESS_H
#define SD_PROCESS_H

#include "Arduino.h"

class SD_PROCESS {
  public:
    String inicializar();
    void guardarInfo(String fileNameAndExtension,String Data);
    void guardarEncabezados();
    void reportError(String msg);
    void setFileCounter(int val);
  private:
    int __defaultFileCounter;
};


#endif // SD_PROCESS_H
