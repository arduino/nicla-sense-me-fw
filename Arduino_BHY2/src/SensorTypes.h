#ifndef SENSOR_TYPES_H_
#define SENSOR_TYPES_H_

#include "stdint.h"

#define SENSOR_DATA_FIXED_LENGTH (10)

struct __attribute__((packed)) SensorDataPacket {
  uint8_t sensorId;
  uint8_t size;
  uint8_t data[SENSOR_DATA_FIXED_LENGTH];
};

struct __attribute__((packed)) SensorConfigurationPacket {
  uint8_t sensorId;
  // sample rate is used also to enable/disable the sensor
  // 0 for disable, else for enable
  float sampleRate;
  // latency indicates how much ms time a new value is retained in its fifo
  // before a notification to the host is sent via interrupt
  // expressed in 24 bit
  uint32_t latency;
};

#endif
