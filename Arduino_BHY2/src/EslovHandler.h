#ifndef ESLOV_HANDLER_H_
#define ESLOV_HANDLER_H_

#include "Arduino.h"
#include "Wire.h"

#define ESLOV_MAX_LENGTH      255
#define ESLOV_DEFAULT_ADDRESS 0x55

#define I2C_INT_PIN (p24)

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
   * @brief Start I2C communication over ESLOV
   * 
   * @return true   I2C communication initialized successfully. 
   */
  bool begin();
  /**
   * @brief Close I2C communication over ESLOV
   * 
   */
  void end();

  /**
   * @brief Manage incoming I2C data based on EslovOpCode state with specified length
   * 
   * @param length 
   */
  static void onReceive(int length);
  /**
   * @brief Manage incoming I2C data based on EslovOpCode state
   * 
   */
  static void onRequest();

  bool eslovActive = false; /*!< Changes to true when ESLOV I2C communication established */

protected:
  /**
   * @brief Nicla Sense ME is mounted as a Shield
   * 
   */
  void niclaAsShield();

private:
  void receiveEvent(int length);
  void requestEvent();

  void eslovBusy();
  void eslovAvailable();

  int _rxIndex;
  uint8_t _rxBuffer[ESLOV_MAX_LENGTH];

  EslovState _state;

private:
  /**
   * @brief The Arduino_BHY2 class can access both private and public methods of EslovHandler
   * 
   */
  friend class Arduino_BHY2;
  void debug(Stream &stream);
  void dump();
  Stream *_debug;
  bool _lastDfuPack;

  PinName _eslovIntPin;
};

/**
 * @brief The EslovHandler class can be externally linked to as eslovHandler in your sketch
 * 
 */
extern EslovHandler eslovHandler;

#endif
