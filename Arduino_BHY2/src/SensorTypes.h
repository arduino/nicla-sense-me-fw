#ifndef SENSOR_TYPES_H_
#define SENSOR_TYPES_H_

#include "stdint.h"

struct __attribute__((packed)) SensorDataPacket {
  uint8_t sensorId;
  uint8_t size;
  uint64_t data;
};

// The host can read sensor's values when it receives an interrupt.
// Alternatively, it can use the fifo flush command that sends all the fifo values and
// discard sensor's fifos 
struct __attribute__((packed)) SensorConfigurationPacket {
  uint8_t sensorId;
  // sample rate is used also to enable/disable the sensor
  // 0 for disable, else for enable
  //uint32_t sampleRate;
  float sampleRate;
  // latency indicates how much ms time a new value is retained in its fifo
  // before a notification to the host is sent via interrupt
  // expressed in 24 bit
  uint32_t latency;
};

#endif
