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

enum EslovOpcode {
  ESLOV_DFU_INTERNAL_OPCODE,
  ESLOV_DFU_EXTERNAL_OPCODE,
  ESLOV_SENSOR_CONFIG_OPCODE,
  ESLOV_SENSOR_STATE_OPCODE
};

enum HostOpcode {
  HOST_DFU_INTERNAL_OPCODE = ESLOV_DFU_INTERNAL_OPCODE,
  HOST_DFU_EXTERNAL_OPCODE = ESLOV_DFU_EXTERNAL_OPCODE,
  HOST_READ_SENSOR_OPCODE,
  HOST_CONFIG_SENSOR_OPCODE
};

enum EslovState {
  ESLOV_AVAILABLE_SENSOR_STATE = 0x00,
  ESLOV_READ_SENSOR_STATE = 0x01,
  ESLOV_DFU_ACK_STATE = 0x02,
  ESLOV_SENSOR_ACK_STATE = 0x03

};

class EslovHandler {
public:
  EslovHandler();
  virtual ~EslovHandler();

  bool begin(bool passthrough);
  void update();

  void writeDfuPacket(uint8_t *data, uint8_t length);
  void writeStateChange(EslovState state);
  void writeConfigPacket(SensorConfigurationPacket& config);
  uint8_t requestPacketAck();
  uint8_t requestAvailableData() ;
  bool requestSensorData(SensorDataPacket &sData);
  void toggleEslovIntPin();

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
