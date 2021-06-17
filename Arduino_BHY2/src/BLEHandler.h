#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include "Arduino.h"
#include "ArduinoBLE.h"

#include "DFUManager.h"

class BLEHandler {
public:
  BLEHandler();
  virtual ~BLEHandler();

  bool begin();
  void update();
  void poll(unsigned long timeout);
  void end();

  static void debug(Stream &stream);

  bool bleActive = false;

private:
  static Stream *_debug;

  bool _lastDfuPack;

  void processDFUPacket(DFUType dfuType, BLECharacteristic characteristic);

  static void receivedInternalDFU(BLEDevice central, BLECharacteristic characteristic);
  static void receivedExternalDFU(BLEDevice central, BLECharacteristic characteristic);
  static void receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic);
};

extern BLEHandler bleHandler;

#endif
