#pragma once
#include "Arduino.h"
#include "Wire.h"
#include "SensorTypes.h"

#define ESLOV_DEFAULT_ADDRESS 0x55

#define ESLOV_DELAY (10)
#define ESLOV_DFU_CHUNK_SIZE (64)

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

struct __attribute__((packed)) DFUPacket {
  uint8_t last: 1;
  union {
    uint16_t index: 15;
    uint16_t remaining: 15;
  };
  uint8_t data[ESLOV_DFU_CHUNK_SIZE];
};

void writeDfuPacket(uint8_t *data, uint8_t length);
void writeStateChange(EslovState state);
void writeConfigPacket(SensorConfigurationPacket* config);
uint8_t requestDfuPacketAck();
uint8_t requestAvailableData() ;
void requestSensorData(SensorDataPacket &sData);
