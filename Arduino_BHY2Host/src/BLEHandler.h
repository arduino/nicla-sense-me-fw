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
  /**
   * @brief Initialise BLE for DFU and sensor data transfer and connect to Nicla device
   * 
   * @return true successful connection to Nicla over BLE
   * 
   */
  bool begin();
  /**
   * @brief Read data from the Nicla over BLE to the host device.
   * 
   */
  void update();
  /**
   * @brief Disconnected Nicla from host and close BLE connection 
   * 
   */
  void end();

  /**
   * @brief Write a configuration packet to the Nicla over BLE. First byte sets the opcode
   * 
   * @param config Instance of @see SensorConfigurationPacket class, with sensorID, sampleRate and latency
   */
  void writeConfigPacket(SensorConfigurationPacket& config);

private:
  friend class Arduino_BHY2Host;

  static void receivedSensorData(BLEDevice central, BLECharacteristic characteristic);
  static void receivedLongSensorData(BLEDevice central, BLECharacteristic characteristic);
  bool connectToNicla();

  void debug(Stream &stream);
  Stream *_debug;
};

extern BLEHandler bleHandler;

#endif