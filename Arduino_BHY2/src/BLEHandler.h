#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include "Arduino.h"
#include "ArduinoBLE.h"

#include "DFUManager.h"
#include "sensors/SensorTypes.h"

#define BHY2_ENABLE_BLE_BATCH 1
const uint8_t BLE_SENSOR_EVT_BATCH_CNT_MAX = (244 / (SENSOR_DATA_FIXED_LENGTH + 2))/10*10;

/**
 * @brief Class for handling BLE communication for both sensor data transfer and DFU
 * 
 */
class BLEHandler {
public:
  BLEHandler();
  virtual ~BLEHandler();

  /**
   * @brief Set advertised local name and initialise BLE advertising for DFU and sensor data transfer
   * 
   * @return true   Successful initialisation of BLE device
   * @return false  Failure in initialisation of BLE device
   */
  bool begin();
  /**
   * @brief Poll data over bluetooth and read data to FIFO buffer.
   * 
   */
  void update();
  /**
   * @brief Poll BLE data
   * 
   * @param timeout timeout in ms, to wait for event.
   */
  void poll(unsigned long timeout);
  /**
   * @brief Close BLE connection
   * 
   */
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

  /**
   * @brief Method for reading and processing DFU packet. Prints size of DFU packet to debug output
   * 
   * @param dfuType         Selects device to update firmware. DFU_INTERNAL for the ANNA-B112 and DFU_EXTERNAL for the BHY2 sensor.
   * @param characteristic  Selected BLE characteristic
   */
  void processDFUPacket(DFUType dfuType, BLECharacteristic characteristic);

  /**
   * @brief Method for recieving BLE packet to ANNA-B112 module
   * 
   * @param central         Instance of BLEDevice
   * @param characteristic  Selected BLE characteristic
   */
  static void receivedInternalDFU(BLEDevice central, BLECharacteristic characteristic);
  /**
   * @brief Method for recieving BLE packet to BHI260AP sensor
   * 
   * @param central         Instance of BLEDevice
   * @param characteristic  Selected BLE characteristic
   */
  static void receivedExternalDFU(BLEDevice central, BLECharacteristic characteristic);
  /**
   * @brief Read sensor configuration data and output to debug
   * 
   * @param central         Instance of BLEDevice
   * @param characteristic  Selected BLE characteristic
   */
  static void receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic);
};

/**
 * @brief The BLEHandler class can be externally linked to as bleHandler in your sketch
 * 
 */
extern BLEHandler bleHandler;

#endif
