#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include "Arduino.h"
#include "sensors/SensorTypes.h"
#include "DFUTypes.h"
#include "ArduinoBLE.h"

class BLEHandler {
public:
  BLEHandler();
  virtual ~BLEHandler();

  bool begin();
  void update();
  void end();

  void writeConfigPacket(SensorConfigurationPacket& config);

private:
  friend class Arduino_BHY2Host;

  static void receivedSensorData(BLEDevice central, BLECharacteristic characteristic);
  bool connectToNicla();

  void debug(Stream &stream);
  Stream *_debug;
};

extern BLEHandler bleHandler;

#endif