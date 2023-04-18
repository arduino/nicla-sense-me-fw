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
 * @brief Enumerator for ESLOV operational state
 * 
 */
enum EslovOpcode {
  ESLOV_DFU_INTERNAL_OPCODE,    /*!< ESLOV DFU ANNA-B112 */
  ESLOV_DFU_EXTERNAL_OPCODE,    /*!< ESLOV DFU BHY260 */
  ESLOV_SENSOR_CONFIG_OPCODE,   /*!< ESLOV Sensor Configuration */
  ESLOV_SENSOR_STATE_OPCODE     /*!< ESLOV Sensor State */
};


/**
 * @brief Enumeration for Host-Nicla configuration
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

class EslovHandler {
public:
  EslovHandler();
  virtual ~EslovHandler();

   /**
   * @brief Start I2C communication with Nicla Sense ME Board
   * 
   * @param passthrough Enable Serial port at 115200 bps
   * 
   * @return true   I2C communication initialised successfully. 
   * @return false  Failure in starting communication over ESLOV protocol.
   */
  bool begin(bool passthrough);
  /**
   * @brief When Serial port is avaliable, then based on the defined opcode write data 
   * 
   */
  void update();

  /**
   * @brief Send DFU data over ESLOV. On the last packet, the built-in LED is pulled down.
   * 
   * @param data    data to be send for DFU
   * @param length  total length of data to be sent
   */
  void writeDfuPacket(uint8_t *data, uint8_t length);
  /**
   * @brief Configure the Nicla device to run in a specific state
   * 
   * @param state Configuration passed to Nicla based on @ref EslovState enumerator
   */
  void writeStateChange(EslovState state);
  /**
   * @brief Specify which sensor to poll data from on the Nicla Sense ME
   * 
   * @param config Configuration for the Sensor including sensor ID, sample rate and latency
   */
  void writeConfigPacket(SensorConfigurationPacket& config);
  /**
   * @brief Recieve acknowledgement from Nicla board over ESLOV
   * 
   * @return uint8_t Data read from I2C bus
   */
  uint8_t requestPacketAck();
  /**
   * @brief Obtain data sent over ESLOV to the host board
   * 
   * @return uint8_t Data read from I2C bus
   */
  uint8_t requestAvailableData();
  /**
   * @brief Obtain long data sent over ESLOV to the host board
   * 
   * @return uint8_t Data read from I2C bus
   */
  uint8_t requestAvailableLongData();
  /**
   * @brief Request data from the Nicla board
   * 
   * @param sData 
   * @return true   Request successful
   * @return false  Request unsuccessful
   */
  bool requestSensorData(SensorDataPacket &sData);
  /**
   * @brief Request long data from the Nicla board
   * 
   * @param sData 
   * @return true  Request successful
   * @return false Request unsuccessful
   */
  bool requestSensorLongData(SensorLongDataPacket &sData);

protected:
  /**
   * @brief Nicla Sense ME is mounted as a Shield
   * 
   */
  void niclaAsShield();

private:
  int _rxIndex;
  uint8_t _rxBuffer[ESLOV_MAX_LENGTH];

  EslovState _eslovState;
  bool _intPinAsserted;
  bool _intPinCleared;
  bool _dfuLedOn;

private:
  /**
   * @brief The Arduino_BHY2Host class can accces both private and public methods of EslovHandler
   * 
   */
  friend class Arduino_BHY2Host;

  void flushWire();

  void debug(Stream &stream);
  void dump();
  Stream *_debug;

  uint8_t _eslovIntPin;
};

extern EslovHandler eslovHandler;

#endif
