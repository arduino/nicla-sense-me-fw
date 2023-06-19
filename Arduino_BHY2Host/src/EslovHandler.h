#ifndef ESLOV_HANDLER_H_
#define ESLOV_HANDLER_H_

#include "Arduino.h"
#include "sensors/SensorTypes.h"
#include "DFUTypes.h"
#include "Wire.h"

#define ESLOV_MAX_LENGTH      255
#define ESLOV_DEFAULT_ADDRESS 0x55

#define ESLOV_INT_PIN (7)

#define I2C_INT_PIN (0)

/**
 * @brief Enumerator for ESLOV operational state. Defines what state the I2C protocol of the ESLOV device should be in. Used to set DFU (Device Firmware Update) mode in the ANNA-B112 and BHY260 of the Nicla Sense ME.
 * 
 */
enum EslovOpcode {
  ESLOV_DFU_INTERNAL_OPCODE,  /*!< ESLOV DFU ANNA-B112 */
  ESLOV_DFU_EXTERNAL_OPCODE,  /*!< ESLOV DFU BHY260 */
  ESLOV_SENSOR_CONFIG_OPCODE, /*!< ESLOV Sensor Configuration */
  ESLOV_SENSOR_STATE_OPCODE   /*!< ESLOV Sensor State */
};

/**
 * @brief Enumeration for the host device operational status. 
 * 
 */
enum HostOpcode {
  HOST_DFU_INTERNAL_OPCODE = ESLOV_DFU_INTERNAL_OPCODE,
  HOST_DFU_EXTERNAL_OPCODE = ESLOV_DFU_EXTERNAL_OPCODE,
  HOST_READ_SENSOR_OPCODE,
  HOST_CONFIG_SENSOR_OPCODE,
  HOST_READ_LONG_SENSOR_OPCODE
};

/**
 * @brief Enumeration for various states over ESLOV
 * 
 */
enum EslovState {
  ESLOV_AVAILABLE_SENSOR_STATE = 0x00,
  ESLOV_READ_SENSOR_STATE = 0x01,
  ESLOV_DFU_ACK_STATE = 0x02,
  ESLOV_SENSOR_ACK_STATE = 0x03,
  ESLOV_AVAILABLE_LONG_SENSOR_STATE = 0x04,
  ESLOV_READ_LONG_SENSOR_STATE = 0x05
};

/**
 * @brief Class to manage communication over ESLOV
 * 
 */
class EslovHandler {
public:
  EslovHandler();
  virtual ~EslovHandler();

  /**
   * @brief Start I2C communication over ESLOV between host board and Nicla
   * 
   * @return true I2C communication initialised successfully. 
   */
  bool begin(bool passthrough);
  /**
   * @brief Reads incoming data to the host board based on the opcode @see EslovState.
   * 
   */
  void update();
  /**
   * @brief Write a DFU (Device Firmware Update) packet to the Nicla Board over ESLOV. On the last packet, the built-in LED is pulled down.
   * 
   * @param data pointer to data to be uploaded to Nicla board as a byte array
   * @param length amount of data to be written to the Nicla in bytes (int)
   */
  void writeDfuPacket(uint8_t *data, uint8_t length);
  /**
   * @brief Waits for the ESLOV interrupt pin to be pulled high, then sends a packet with the @see ESLOV_SENSOR_STATE_OPCODE to over I2C
   * 
   * @param state State value sent to Nicla Sense ME based on @ref EslovState enumerator.
   */
  void writeStateChange(EslovState state);
  /**
   * @brief Write a configuration packet to the Nicla over ESLOV. First byte sets the opcode
   * 
   * @param config Instance of @see SensorConfigurationPacket class, with sensorID, sampleRate and latency
   */
  void writeConfigPacket(SensorConfigurationPacket& config);
  /**
   * @brief Requests an acknowledgment packet from the Nicla over ESLOV.
   * 
   * @return uint8_t acknowledge packet recieved from the Nicla over ESLOV
   */
  uint8_t requestPacketAck();
  /**
   * @brief Change state of Nicla to ESLOV_AVAILABLE_SENSOR_STATE and wait for the interrupt pin to go high. Then read the avaliable sensor data over I2C.
   * 
   * @return uint8_t Number of available sensor data packets.
   */
  uint8_t requestAvailableData();
  /**
   * @brief Change state of Nicla to ESLOV_AVAILABLE_SENSOR_STATE and wait for the interrupt pin to go high. Then read the avaliable long sensor data over I2C.
   * 
   * @return uint8_t Number of available long sensor data packets.
   */
  uint8_t requestAvailableLongData();
  /**
   * @brief Change state of Nicla to ESLOV_READ_SENSOR_STATE and wait for the interrupt pin to go high. Then read the avaliable sensor data over I2C.
   * 
   * @param sData data packet containing sensorID, payload size and data payload
   * @return true Successful request of sensor data
   */
  bool requestSensorData(SensorDataPacket &sData);
  /**
   * @brief Change state of Nicla to ESLOV_READ_SENSOR_STATE and wait for the interrupt pin to go high. Then read the avaliable long sensor data over I2C.
   * 
   * @param sData data packet containing sensorID, payload size and data payload
   * @return true Successful request of sensor data
   */
  bool requestSensorLongData(SensorLongDataPacket &sData);

protected:
  void niclaAsShield();

private:
  int _rxIndex;
  uint8_t _rxBuffer[ESLOV_MAX_LENGTH];

  EslovState _eslovState;
  bool _intPinAsserted;
  bool _intPinCleared;
  bool _dfuLedOn;

private:
  friend class Arduino_BHY2Host;

  void flushWire();

  void debug(Stream &stream);
  void dump();
  Stream *_debug;

  uint8_t _eslovIntPin;
};

extern EslovHandler eslovHandler;

#endif
