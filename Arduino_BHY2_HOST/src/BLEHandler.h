#if 0
#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include "Arduino.h"
#include "ArduinoBLE.h"

class BLEHandler {
public:
  BLEHandler();
  virtual ~BLEHandler();

  void begin();
  void update();

  static void debug(Stream &stream);

private:
  static Stream *_debug;
};

extern BLEHandler bleHandler;

#endif

#endif
