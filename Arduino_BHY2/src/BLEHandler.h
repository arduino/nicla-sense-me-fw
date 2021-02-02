#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include "Arduino.h"
#include "ArduinoBLE.h"

#include "DFUManager.h"

class BLEHandler {
public:
  BLEHandler();
  virtual ~BLEHandler();

  void begin();
  void update();

  static void debug(Stream &stream);

private:
  static Stream *_debug;

  void DFUAcknowledgment();
  void processDFUPacket(DFUType dfuType, BLECharacteristic characteristic);
  static void receivedInternalDFU(BLEDevice central, BLECharacteristic characteristic);
  static void receivedExternalDFU(BLEDevice central, BLECharacteristic characteristic);
  static void receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic);
};

extern BLEHandler bleHandler;

#endif
