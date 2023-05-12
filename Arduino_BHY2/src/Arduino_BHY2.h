#ifndef ARDUINO_BHY2_H_
#define ARDUINO_BHY2_H_

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

/**
 *  @brief Enumeration for defining wiring configuration over ESLOV or Shield.
 *  @ref Arduino_BHY2::begin()
 *
 *  @note For NICLA_AS_SHIELD configuration, see https://docs.arduino.cc/tutorials/nicla-sense-me/use-as-mkr-shield
 *
 */
enum NiclaWiring
{
  NICLA_VIA_ESLOV = 0x10, /*!< Indicates Nicla board is connected via the ESLOV header */
  NICLA_AS_SHIELD = 0x20  /*!< Indicates Nicla board is connected as a shield to a MKR board */
};

/**
 *  @brief Enumeration for defining I2C or BLE communication configuration.
 *  @ref Arduino_BHY2::begin()
 *
 *  @note ESLOV communication occurs over I2C
 */
enum NiclaConfig
{
  NICLA_I2C = 0x1,                           /*!< I2C and ESLOV */
  NICLA_BLE = 0x2,                           /*!< Bluetooth via ANNA-B112 module */
  NICLA_BLE_AND_I2C = NICLA_I2C | NICLA_BLE, /*!< Enable I2C and BLE simultaneously */
  NICLA_STANDALONE = 0x4                     /*!< Operate in standalone, without external data transfer */
};

/**
 * @brief Class for storing Nicla configuration state
 * 
 * @ref Arduino_BHY2::NiclaConfig Arduino_BHY2::NiclaWiring
 *
 */
class NiclaSettings
{
public:
  NiclaSettings(uint8_t conf1 = 0, uint8_t conf2 = 0, uint8_t conf3 = 0, uint8_t conf4 = 0)
  {
    conf = conf1 | conf2 | conf3 | conf4;
  }

  uint8_t getConfiguration() const
  {
    return conf;
  }

private:
  uint8_t conf = 0;
};

/**
 * @brief Main class for initialising communication with sensors on the Nicla Sense ME
 */
class Arduino_BHY2
{
public:
  Arduino_BHY2();
  virtual ~Arduino_BHY2();

  // Necessary API. Update function should be continuously polled
  /**
   * @brief Initialise communication with Nicla board based on configured communication method
   * 
   * @param config Configuration for set @ref NiclaConfig state
   * @param niclaConnection Configuration for set @ref NiclaWiring state
   *
   */
  bool begin(NiclaConfig config = NICLA_BLE_AND_I2C, NiclaWiring niclaConnection = NICLA_VIA_ESLOV);
  bool begin(NiclaSettings &settings);
  /**
   *  @brief  Pings I2C protocol and BHY sensor
   *
   *  @note BME688 sensor must be continuously polled to prevent sleep
   */
  void update(); // remove this to enforce a sleep
  /**
   *  @brief  Pings sensor and then sleep
   *
   *  @note Delay does not stall microcontroller to sleep. See https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay
   */
  void update(unsigned long ms); // Update and then sleep
  /**
   *   @brief delay method to be used instead of Arduino delay().
   */
  void delay(unsigned long ms); // to be used instead of arduino delay()

  // API for using the Bosch sensortec from sketch
  /**
   * @brief Set the sample rate and latency for a given sensorID.
   *
   * @param config Sensor configuration
   * 
   * @note For list of SensorID, see https://docs.arduino.cc/tutorials/nicla-sense-me/cheat-sheet#sensor-ids
   */
  void configureSensor(SensorConfigurationPacket &config);
  /**
   * @brief Set the sample rate and latency for a given sensorID.
   *
   * @param sensorId  Sensor ID for sensor
   * @param sampleRate Polling rate for sensor
   * @param latency   Latency in milliseconds
   *
   * @note For list of SensorID, see https://docs.arduino.cc/tutorials/nicla-sense-me/cheat-sheet#sensor-ids
   */
  void configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency);
  /**
   * @brief Handle FIFO of data queue. Eliminate oldest data when sensor queue is full.
   *
   * @param sensorData Data packet from sensor
   */
  void addSensorData(SensorDataPacket &sensorData);
  /**
   * @brief Handle FIFO of data queue. Eliminate oldest data when sensor queue is full.
   *
   * @param sensorData Data packet from sensor
   */
  void addLongSensorData(SensorLongDataPacket &sensorData);
  /**
   * @brief Return available sensor data within the FIFO buffer queue
   *
   * @return uint8_t The amount of data in bytes
   */
  uint8_t availableSensorData();
  /**
   * @brief Return available long sensor data within the FIFO buffer queue
   *
   * @return uint8_t The amount of data in bytes
   */
  uint8_t availableLongSensorData();
  /**
   * @brief Read sensor data from the top element of the queue
   *
   * @param data Structure including sensorID, sampleRate and latency
   */
  bool readSensorData(SensorDataPacket &data);
  /**
   * @brief Read long sensor data
   *
   * @param data Structure including sensorID, sampleRate and latency
   */
  bool readLongSensorData(SensorLongDataPacket &data);
  /**
   * @brief Check existence of a given sensorID
   *
   * @param sensorId Selected virtual sensor
   * @return true - Sensor is present
   * @return false - Sensor is not present
   */
  bool hasSensor(uint8_t sensorId);
  /**
   * @brief Parse XYZ Cartesian data
   *
   * @param data Data packet including SensorID
   * @param vector vector with XYZ
   */
  void parse(SensorDataPacket &data, DataXYZ &vector);
  /**
   * @brief Parse orientation
   *
   * @param data Data packet including SensorID
   * @param vector Vector with heading, pitch and roll
   */
  void parse(SensorDataPacket &data, DataOrientation &vector);
  /**
   * @brief Parse orientation with scale factor
   *
   * @param data Data packet including SensorID
   * @param vector Vector with heading, pitch and roll
   * @param scaleFactor scale factor for vector
   */
  void parse(SensorDataPacket &data, DataOrientation &vector, float scaleFactor);
  /**
   * @brief Define LDO regulator timeout time
   *
   * @param time in milliseconds. 120000 ms by default.
   */
  void setLDOTimeout(int time);
  /**
   * @brief Stream
   *
   * @param stream
   */
  void debug(Stream &stream);

private:
  Stream *_debug;

  void pingI2C();
  int _pingTime;
  int _timeout;
  int _startTime;

  PinName _eslovIntPin;

  NiclaConfig _niclaConfig;
};

/**
 * @brief The Arduino_BHY2 class can be externally linked to as BHY2 in your sketch
 *
 */
extern Arduino_BHY2 BHY2;

#endif
