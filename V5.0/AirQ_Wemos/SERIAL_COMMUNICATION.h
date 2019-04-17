#ifndef SERIAL_COMMUNICATION_H
#define SERIAL_COMMUNICATION_H

#include "Arduino.h"

class SERIAL_COMMUNICATION {
  public:
    void inicializar();
    boolean leerArduino();
    boolean compararUltimaLectura();
};


#endif // SERIAL_COMMUNICATION_H
