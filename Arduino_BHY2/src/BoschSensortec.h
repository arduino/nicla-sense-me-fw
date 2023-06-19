#ifndef BOSCH_SENSORTEC_H_
#define BOSCH_SENSORTEC_H_

#include "Arduino.h"
#include "mbed.h"

#include "BLEHandler.h"


#include "bosch/common/common.h"
#include "sensors/SensorTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "bosch/bhy2.h"
#ifdef __cplusplus
}
#endif

#if BHY2_ENABLE_BLE_BATCH
#define SENSOR_QUEUE_SIZE   (BLE_SENSOR_EVT_BATCH_CNT_MAX + 20)
#define WORK_BUFFER_SIZE    1024
#else
#define SENSOR_QUEUE_SIZE   10
#define WORK_BUFFER_SIZE    2048
#endif

//the long sensors are usually of much lower rate, hence smaller demand on buffer size
#define LONG_SENSOR_QUEUE_SIZE 5

#define MAX_READ_WRITE_LEN 256

/**
 * @brief Enumeration to check for correct short message delivery over ESLOV communication
 * 
 */
enum SensorAckCode {
  SensorAck = 0x0F,    /*!< Acknowledgement */
  SensorNack = 0x00    /*!< Negative Acknowledgement */
};

/**
 * @brief Class to communicate with BME688 Sensor
 * 
 */
class BoschSensortec {
public:
  BoschSensortec();
  virtual ~BoschSensortec();

  // sketch-side API
  /**
   * @brief Setup SPI interface between BHI260 and ANNA-B112
   * 
   * @return true successful initialisation of SPI interface 
   */
  bool begin();
  /**
   * @brief Update data on FIFO buffer
   * 
   */
  void update();
  /**
   * @brief Configure sensor. @see Arduino_BHY2::configureSensor() 
   * 
   * @param config Contains SensorID, SampleRate and latency
   */
  void configureSensor(SensorConfigurationPacket& config);
  /**
   * @brief Set range of the sensor @see Arduino_BHY2::configureSensor()
   * 
   * @param id    SensorID of selected virtual sensor
   * @param range Range for selected SensorID. See Table 79 in BHY260 datasheet 
   * @return int 1-> Success 0-> failure
   */
  int configureSensorRange(uint8_t id, uint16_t range);
  /**
   * @brief Read the configuration for a given virtual sensor
   * 
   * @param id                SensorID
   * @param virt_sensor_conf  Define sensitivity, range, latency and sample rate
   */
  void getSensorConfiguration(uint8_t id, SensorConfig& virt_sensor_conf);

  /**
   * @brief Print sensors to debug output
   * 
   */
  void printSensors();
  /**
   * @brief Check to see if sensor corresponding to SensorID is present.
   * 
   * @param sensorId SensorID
   * @return true Sensor is present
   */
  bool hasSensor(uint8_t sensorId);

  /**
   * @brief Return available size of avaliable sensor data
   * 
   * @return uint8_t size of avaliable sensor data
   */
  uint8_t availableSensorData();
  /**
   * @brief Return available size of avaliable long sensor data
   * 
   * @return uint8_t size of avaliable sensor long data
   */
  uint8_t availableLongSensorData();
  /**
   * @brief Read sensor data
   * 
   * @param data instance of SensorDataPacket containing sensorID (uint8_t), payload size (uint8_t) and data (uint8_t)
   * @return true data read from sensor successfully
   */
  bool readSensorData(SensorDataPacket &data);
  /**
   * @brief Read long sensor data
   * 
   * @param data instance of SensorDataPacket containing sensorID (uint8_t), payload size (uint8_t) and data (uint8_t)
   * @return true data read from sensor successfully
   */
  bool readLongSensorData(SensorLongDataPacket &data);

  // ANNA <-> BOSCH interface
  /**
   * @brief Handle FIFO of data queue. @see Arduino_BHY2::addSensorData()
   * 
   * @param sensorData Data packet from sensor
   */
  void addSensorData(SensorDataPacket &sensorData);
  /**
   * @brief Handle FIFO of data queue for long sensor data @see Arduino_BHY2::addSensorData()
   * 
   * @param sensorData Data packet from sensor
   */
  void addLongSensorData(SensorLongDataPacket &sensorData);
  /**
   * @brief Reset NACK flag
   * 
   * @return uint8_t 
   */
  uint8_t acknowledgment();

private:
  mbed::CircularBuffer<SensorDataPacket, SENSOR_QUEUE_SIZE, uint8_t> _sensorQueue;
  mbed::CircularBuffer<SensorLongDataPacket, LONG_SENSOR_QUEUE_SIZE, uint8_t> _longSensorQueue;

  uint8_t _workBuffer[WORK_BUFFER_SIZE];
  uint8_t _acknowledgment;

  struct bhy2_dev _bhy2;
  uint8_t _sensorsPresent[32];

private:
  /**
   * @brief The Arduino_BHY2 class can accces both private and public methods of BoschSensortec
   * 
   */
  friend class Arduino_BHY2;
  void debug(Stream &stream);
  Stream *_debug;
};

/**
  * @brief The BoschSensortec class can be externally linked to as sensortec in your sketch
  * 
  */
extern BoschSensortec sensortec;

#endif
