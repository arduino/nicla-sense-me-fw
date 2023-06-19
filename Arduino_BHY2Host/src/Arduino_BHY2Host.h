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
 *  @brief Enumeration for defining wiring configuration between host board and Nicla client. We can select between an I2C connection over ESLOV (NICLA_VIA_ESLOV) or as a Shield for the MKR boards (NICAL_AS_SHIELD). 
 * 
 *  For NICLA_AS_SHIELD configuration, see https://docs.arduino.cc/tutorials/nicla-sense-me/use-as-mkr-shield
 *  
 */
enum NiclaWiring {
  NICLA_VIA_ESLOV = 0,      /*!< Host connects to Nicla board via ESLOV */
  NICLA_AS_SHIELD,          /*!< Host connects to Nicla board as a Shield */
  NICLA_VIA_BLE             /*!< Host connects to Nicla board via BLE */
};


/**
 * @brief Class to interface with the Bosch BHI260 sensor hub on the Nicla Sense ME.
 * Provides functionality for reading/configuring sensors based on sensor ID and accessing debug features. 
*/
class Arduino_BHY2Host {
public:
  Arduino_BHY2Host();
  virtual ~Arduino_BHY2Host();

  // Necessary API. Update function should be continuously polled if PASSTHORUGH is ENABLED
    /**
   * @brief Initialise the BHY2 functionality on the host board, for a given @ref NiclaWiring configuration.
   * 
   * @note When called without input parameters, I2C communication is over ESLOV by default.
   * 
   * @param passthrough Define passthrough state. Disabled by default
   * @param NiclaWiring Defining I2C configuration (NICLA_VIA_ESLOV, NICLA_AS_SHIELD or NICLA_VIA_BLE). @see NiclaWiring 
   * 
   * Configuring for operation as a Shield:
   * @code 
   * BHY2Host.begin(NICLA_VIA_BLE);
   * @endcode
   */
  bool begin(bool passthrough = false, NiclaWiring niclaConnection = NICLA_VIA_ESLOV);
    /**
   *  @brief Update sensor data by reading the FIFO buffer on the BHI260 and then pass it to a suitable parser. 
   * 
   *  @param ms (optional) Time (in milliseconds) to wait before returning data. 
   */
  void update();
  void update(unsigned long ms); // Update and then sleep

  // Functions for controlling the BHY when PASSTHROUGH is DISABLED
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
  uint8_t requestAck();
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
  uint8_t availableSensorLongData();
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
  bool readSensorLongData(SensorLongDataPacket &data);
  /**
   * @brief Parse XYZ Cartesian data from a given data packet
   *
   * @param data data packet including SensorID
   * @param vector vector with XYZ
   */
  void parse(SensorDataPacket& data, DataXYZ& vector);
  /**
   * @brief Parse orientation from a given data packet
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
