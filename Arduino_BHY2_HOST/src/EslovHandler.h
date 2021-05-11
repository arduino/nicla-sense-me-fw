#ifndef ESLOV_HANDLER_H_
#define ESLOV_HANDLER_H_

#include "Arduino.h"
#include "SensorTypes.h"
#include "DFUTypes.h"
#include "Wire.h"

#define ESLOV_MAX_LENGTH      255
#define ESLOV_DEFAULT_ADDRESS 0x55

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
  ESLOV_DFU_ACK_STATE = 0x02
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
  uint8_t requestDfuPacketAck();
  uint8_t requestAvailableData() ;
  bool requestSensorData(SensorDataPacket &sData);

private:
  int _rxIndex;
  uint8_t _rxBuffer[ESLOV_MAX_LENGTH];

  EslovState _eslovState;

private:
  friend class Arduino_BHY2_HOST;
  void debug(Stream &stream);
  void dump();
  Stream *_debug;
};

extern EslovHandler eslovHandler;

#endif
