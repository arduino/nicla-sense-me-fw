#ifndef ESLOV_HANDLER_H_
#define ESLOV_HANDLER_H_

#include "Arduino.h"
#include "Wire.h"

#define ESLOV_MAX_LENGTH      255
#define ESLOV_DEFAULT_ADDRESS 0x55

enum EslovOpcode {
  ESLOV_DFU_INTERNAL_OPCODE,
  ESLOV_DFU_EXTERNAL_OPCODE,
  ESLOV_SENSOR_DATA_OPCODE,
  ESLOV_SENSOR_CONFIG_OPCODE,
  ESLOV_SENSOR_REQUEST_OPCODE
};


class EslovHandler {
  public:
    EslovHandler();
    ~EslovHandler();

    static void wireCallback(int howMany);
    void setup();

  private:
    void receiveEvent(int howMany);

    int _rxIndex = 0;
    uint8_t _rxBuffer[ESLOV_MAX_LENGTH] = {0};
};

extern EslovHandler eslovHandler;

#endif
