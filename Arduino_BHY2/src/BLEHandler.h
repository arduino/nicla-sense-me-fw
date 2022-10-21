#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include "Arduino.h"
#include "ArduinoBLE.h"

#include "DFUManager.h"
#include "sensors/SensorTypes.h"

#define BHY2_ENABLE_BLE_BATCH 1
const uint8_t BLE_SENSOR_EVT_BATCH_CNT_MAX = (244 / (SENSOR_DATA_FIXED_LENGTH + 2))/10*10;

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

#if BHY2_ENABLE_BLE_BATCH
  static SensorDataPacket *_dataBatch;
  static uint8_t           _idxBatch;
#endif

  bool _lastDfuPack;

  void processDFUPacket(DFUType dfuType, BLECharacteristic characteristic);

  static void receivedInternalDFU(BLEDevice central, BLECharacteristic characteristic);
  static void receivedExternalDFU(BLEDevice central, BLECharacteristic characteristic);
  static void receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic);
};

extern BLEHandler bleHandler;

#endif
