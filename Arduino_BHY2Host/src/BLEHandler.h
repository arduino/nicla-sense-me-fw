#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include "Arduino.h"
#include "sensors/SensorTypes.h"
#include "DFUTypes.h"
#include "ArduinoBLE.h"

/**
 * @brief Class for handling BLE communication for both sensor data transfer and DFU
 * 
 */

class BLEHandler {
public:
  BLEHandler();
  virtual ~BLEHandler();

  /**
   * @brief Start BLE connection to the Nicla board
   * 
   * @return true Connection successful to the Nicla board
   * @return false Connection unsuccessful to the Nicla board
   */
  bool begin();
  /**
   * @brief Poll data over Bluetooth 
   * 
   */
  void update();
  /**
   * @brief Disconnect BLE connection between host and Nicla board
   * 
   */
  void end();
  /**
   * @brief Specify which sensor to poll data from on the Nicla Sense ME
   * 
   * @param config Configuration for the Sensor including sensor ID, sample rate and latency
   */
  void writeConfigPacket(SensorConfigurationPacket& config);

private:
  /**
   * @brief The Arduino_BHY2Host class can accces both private and public methods of BLEHandler
   * 
   */
  friend class Arduino_BHY2Host;

  static void receivedSensorData(BLEDevice central, BLECharacteristic characteristic);
  static void receivedLongSensorData(BLEDevice central, BLECharacteristic characteristic);
  bool connectToNicla();

  void debug(Stream &stream);
  Stream *_debug;
};

extern BLEHandler bleHandler;

#endif