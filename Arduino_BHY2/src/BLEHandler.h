#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include "Arduino.h"
#include "ArduinoBLE.h"

#include "DFUChannel.h"

class BLEHandler {
public:
  BLEHandler();
  virtual ~BLEHandler();

  void begin();
  void update();

private:
  void processDFUPacket(DFUType dfuType, BLECharacteristic characteristic);
  static void receivedInternalDFU(BLEDevice central, BLECharacteristic characteristic);
  static void receivedExternalDFU(BLEDevice central, BLECharacteristic characteristic);
  static void receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic);
};

extern BLEHandler bleHandler;

#endif
