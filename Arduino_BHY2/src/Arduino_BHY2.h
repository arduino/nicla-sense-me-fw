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
#include "sensors/SensorBSEC2.h"
#include "sensors/SensorBSEC2Collector.h"
#include "sensors/SensorActivity.h"
#include "sensors/Sensor.h"

#include "sensors/SensorID.h"

/** 
 *  @brief Enumeration for defining wiring configuration between host board and Nicla client. We can select between an I2C connection over ESLOV (NICLA_VIA_ESLOV) or as a Shield for the MKR boards (NICAL_AS_SHIELD). 
 * 
 *  For NICLA_AS_SHIELD configuration, see https://docs.arduino.cc/tutorials/nicla-sense-me/use-as-mkr-shield
 *  
 */
enum NiclaWiring {
  NICLA_VIA_ESLOV = 0x10,                 /*!< Host connects to Nicla board via ESLOV */
  NICLA_AS_SHIELD = 0x20                  /*!< Host connects to Nicla board as a Shield */
};

/** 
 *  @brief Enumeration for defining I2C (wired) or BLE communication configuration between host board and Nicla client. Alternatively, the Nicla Sense ME can also run in a standalone configuration.  
 * 
 *  @note Both ESLOV and Shield should be considered as implementations of I2C communication (NICLA_I2C)
 * 
 */
enum NiclaConfig {
  NICLA_I2C = 0x1,                          /*!< I2C Configuration, relevant for both ESLOV and Shield configurations  */
  NICLA_BLE = 0x2,                          /*!< Bluetooth via the onboard ANNA-B112 module */
  NICLA_BLE_AND_I2C = NICLA_I2C | NICLA_BLE,/*!< Enable I2C (ESLOV/Shield) and BLE simultaneously */
  NICLA_STANDALONE = 0x4                    /*!< Operate in standalone, without external data transfer */
};

/**
 * @brief Class for configuring the host-client communication, based on the @ref NiclaConfig enumeration.
 * 
 * Example:
 * @code
 * NiclaSettings niclaSettings(NICLA_I2C);
 * @endcode
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
 * @brief Class to interface with the Bosch BHI260 sensor hub on the Nicla Sense ME.
 * Provides functionality for reading/configuring sensors based on sensor ID and accessing debug features. 
*/
class Arduino_BHY2 {

public:
  Arduino_BHY2();
  virtual ~Arduino_BHY2();

  // Necessary API. Update function should be continuously polled
  /**
   * @brief initialize the BHY2 functionality for the Nicla Sense ME, for a given @ref NiclaSettings configuration.
   * 
   * @note When called without input parameters, I2C and BLE are enabled by default. I2C communication is over ESLOV.
   * 
   * @param NiclaConfig Define communication configuration (NICLA_I2C, NICLA_BLE, NICLA_BLE_AND_I2C or NICLA_STANDALONE). @see NiclaConfig
   * @param NiclaWiring Defining I2C configuration (NICLA_VIA_ESLOV or NICLA_AS_SHIELD). @see NiclaWiring 
   * 
   * Configuring for operation as a Shield:
   * @code 
   * BHY2.begin(NICLA_I2C, NICLA_AS_SHIELD);
   * @endcode
   */
  bool begin(NiclaConfig config = NICLA_BLE_AND_I2C, NiclaWiring niclaConnection = NICLA_VIA_ESLOV);
  bool begin(NiclaSettings &settings);
  /**
   *  @brief Update sensor data by reading the FIFO buffer on the BHI260 and then pass it to a suitable parser. 
   * 
   *  @param ms (optional) Time (in milliseconds) to wait before returning data. 
   */
  void update(); // remove this to enforce a sleep
  void update(unsigned long ms); // Update and then sleep
  /**
  *   @brief Delay method to be used instead of Arduino delay().
  * 
  *   @note Unlike the standard Arduino delay, this modified delay method uses a function to check if the elapsed time has passed.
  * This does not stall the microprocessor. 
  * 
  *   @param ms Time (in milliseconds) to enforce delay.
  * 
  *   @code 
  *   BHY2.delay(100);
  *   @endcode
  */
  void delay(unsigned long ms); // to be used instead of arduino delay()

  // API for using the Bosch sensortec from sketch
  /**
   * @brief Configure a virtual sensor on the BHI260 to have a set sample rate (Hz) and latency (milliseconds)
   * This can be achieved 
   * 
   * @param config Instance of @see SensorConfigurationPacket class, with sensorID, sampleRate and latency
   * 
   * @code
   * #set virtual sensor SENSOR_ID_DEVICE_ORI_WU to have sample rate of 1 Hz and latency of 500 milliseconds
   * SensorConfigurationPacket config(70, 1, 500);
   * BHY2.configureSensor(config)
   * @endcode
   * 
   * @note Alternatively, we can directly configure the virtual sensor without creating a SensorConfigurationPacket class
   * 
   * @param sensorId  SensorID for virtual sensor
   * @param sampleRate Polling rate for sensor in Hz
   * @param latency   Latency in milliseconds 
   * 
   * @note For list of SensorID, see src/SensorID.h. Or see Table 79 in the <a href="https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bhi260ab-ds000.pdf">BHI260 datasheet</a>
   * 
   */
  void configureSensor(SensorConfigurationPacket& config);
  void configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency);
  /**
   * @brief Extract data from the FIFO buffer for a specific SensorID and save into an instance of the SensorDataPacket struct. Eliminate oldest data when sensor queue is full.
   * 
   * @param sensorData Struct containing SensorID (uint8_t), data payload size (uint8_t) and data corresponding to SensorID (uint8_t).
   */
  void addSensorData(SensorDataPacket &sensorData);
  /**
   * @brief Extract data from the FIFO buffer for a specific SensorID and save into an instance of the SensorLongDataPacket struct. Eliminate oldest data when sensor queue is full.
   *
   * @param sensorData Struct containing SensorID (uint8_t), long data payload size (uint8_t) and data corresponding to SensorID (uint8_t).
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
   * @brief Read long sensor data from the top element of the queue
   *
   * @param data Structure including sensorID, sampleRate and latency
   */
  bool readLongSensorData(SensorLongDataPacket &data);
  /**
   * @brief Check existence of a given sensorID
   *
   * @param sensorID Selected virtual sensor
   * @return True Sensor is present
   */
  bool hasSensor(uint8_t sensorId);
  /**
   * @brief Parse XYZ Cartesian data from a given data packet
   *
   * @param data data packet including SensorID
   * @param vector vector with XYZ
   */
  void parse(SensorDataPacket &data, DataXYZ &vector);
  /**
   * @brief Parse orientation from a given data packet
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
   * @brief Prints debug stream of multiple library components to the specified stream object. 
   *
   * @param Stream object that implements the Print() function. 
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
