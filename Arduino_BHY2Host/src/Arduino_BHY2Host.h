#ifndef ARDUINO_BHY2HOST_H_
#define ARDUINO_BHY2HOST_H_

#include "Arduino.h"
#include "sensors/SensorTypes.h"
#include "sensors/DataParser.h"

#include "sensors/SensorClass.h"
#include "sensors/SensorOrientation.h"
#include "sensors/SensorXYZ.h"
#include "sensors/SensorQuaternion.h"
#include "sensors/SensorBSEC.h"
#include "sensors/SensorActivity.h"
#include "sensors/Sensor.h"

#include "sensors/SensorID.h"

#if defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_SAMD_MKRWIFI1010)
#define __BHY2_HOST_BLE_SUPPORTED__
#include "BLEHandler.h"
#endif


/** 
 *  @brief Enumeration for defining wiring configuration over ESLOV, Shield or BLE.
 * 
 *  For NICLA_AS_SHIELD configuration, see https://docs.arduino.cc/tutorials/nicla-sense-me/use-as-mkr-shield
 *  
 */
enum NiclaWiring {
  NICLA_VIA_ESLOV = 0,
  NICLA_AS_SHIELD,
  NICLA_VIA_BLE
};

/**
 * @brief Main class for initialising communication with sensors
*/
class Arduino_BHY2Host {
public:
  Arduino_BHY2Host();
  virtual ~Arduino_BHY2Host();

  // Necessary API. Update function should be continuously polled if PASSTHORUGH is ENABLED
  /**
   * @brief 
   * 
   * @param passthrough Enable Serial port at 115200 bps
   * @param niclaConnection Configuration for set @ref NiclaWiring state
   * @return true   Connection successful to the Nicla board
   * @return false  Connection unsuccessful to the Nicla board
   */
  bool begin(bool passthrough = false, NiclaWiring niclaConnection = NICLA_VIA_ESLOV);
  /**
   *  @brief  Pings sensor
   * 
   *  @note Sensor must be continuously polled to prevent sleep
   */
  void update();
  /**
   *  @brief  Pings sensor and then sleep
   * 
   *  @note Delay does not stall microcontroller to sleep. See https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay
   */
  void update(unsigned long ms); // Update and then sleep

  // Functions for controlling the BHY when PASSTHROUGH is DISABLED
  /**
   * @brief Poll Bosch method 
   * 
   * @param config Sensor configuration
   */
  void configureSensor(SensorConfigurationPacket& config);
    /**
   * @brief Setup virtual sensors on the BHI260
   * 
   * @param sensorId  Sensor ID for sensor
   * @param sampleRate Polling rate for sensor
   * @param latency   Latency in milliseconds
   * 
   * @note For list of SensorID, see src/SensorID.h
   */
  void configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency);
  /**
   * @brief Recieve acknowledgement from Nicla board over ESLOV
   * 
   * @return uint8_t Data read from I2C bus
   */
  uint8_t requestAck();
  /**
   * @brief Return available sensor data
   * 
   * @return uint8_t 
   */
  uint8_t availableSensorData();
  /**
   * @brief Return available long sensor data
   * 
   * @return uint8_t 
   */
  uint8_t availableSensorLongData();
  /**
   * @brief Read sensor data
   * 
   * @param data 
   * @return true 
   * @return false 
   */
  bool readSensorData(SensorDataPacket &data);
  /**
   * @brief Read long sensor data
   * 
   * @param data 
   * @return true 
   * @return false 
   */
  bool readSensorLongData(SensorLongDataPacket &data);
  /**
   * @brief Parse XYZ Cartesian data
   * 
   * @param data Data packet including SensorID
   * @param vector vector with XYZ
   */
  void parse(SensorDataPacket& data, DataXYZ& vector);
   /**
   * @brief Parse orientation
   * 
   * @param data Data packet including SensorID
   * @param vector Vector with heading, pitch and roll
   */
  void parse(SensorDataPacket& data, DataOrientation& vector);
   /**
   * @brief Parse orientation with scale factor
   * 
   * @param data Data packet including SensorID
   * @param vector Vector with heading, pitch and roll
   * @param scaleFactor scale factor for vector
   */
  void parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);

  NiclaWiring getNiclaConnection();

  void debug(Stream &stream);

private:
  bool _passthrough;
  NiclaWiring _wiring;
  Stream *_debug;
};

extern Arduino_BHY2Host BHY2Host;

#endif
