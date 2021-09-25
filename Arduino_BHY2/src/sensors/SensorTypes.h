#ifndef SENSOR_TYPES_H_
#define SENSOR_TYPES_H_

#include "stdint.h"

#define SENSOR_DATA_FIXED_LENGTH (10)

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

struct __attribute__((packed)) SensorDataPacket {
  uint8_t sensorId;
  uint8_t size;
  uint8_t data[SENSOR_DATA_FIXED_LENGTH];

  float getFloat(uint8_t index) {
    float result = 0;
    uint8_t length = sizeof(result);
    if (index + length > SENSOR_DATA_FIXED_LENGTH) {
      //to safe guard against overflow
      length = SENSOR_DATA_FIXED_LENGTH > index ? SENSOR_DATA_FIXED_LENGTH - index : 0;
    }
    if (length > 0)
        memcpy(&result, &data[index], length);
    return result;
  }

  uint8_t getUint8(uint8_t index) {
    if (index >= SENSOR_DATA_FIXED_LENGTH) {
      return 0;
    }
    return data[index];
  }

  uint16_t getUint16(uint8_t index) {
    uint16_t result = 0;
    uint8_t length = sizeof(result);
    if (index + length > SENSOR_DATA_FIXED_LENGTH) {
      length = SENSOR_DATA_FIXED_LENGTH > index ? SENSOR_DATA_FIXED_LENGTH - index : 0;
    }
    if (length > 0)
        memcpy(&result, &data[index], length);
    return result;
  }

  uint32_t getUint24(uint8_t index) {
    uint32_t result = 0;
    uint8_t length = 3;
    if (index + length > SENSOR_DATA_FIXED_LENGTH) {
      length = SENSOR_DATA_FIXED_LENGTH > index ? SENSOR_DATA_FIXED_LENGTH - index : 0;
    }
    if (length > 0)
        memcpy(&result, &data[index], length);
    return result;
  }

  uint32_t getUint32(uint8_t index) {
    uint32_t result = 0;
    uint8_t length = sizeof(result);
    if (index + length > SENSOR_DATA_FIXED_LENGTH) {
      length = SENSOR_DATA_FIXED_LENGTH > index ? SENSOR_DATA_FIXED_LENGTH - index : 0;
    }
    if (length > 0)
        memcpy(&result, &data[index], length);
    return result;
  }

  int8_t getInt8(uint8_t index) {
    if (index >= SENSOR_DATA_FIXED_LENGTH) {
      return 0;
    }
    return data[index];
  }

  int16_t getInt16(uint8_t index) {
    int16_t result = 0;
    uint8_t length = sizeof(result);
    if (index + length > SENSOR_DATA_FIXED_LENGTH) {
      length = SENSOR_DATA_FIXED_LENGTH > index ? SENSOR_DATA_FIXED_LENGTH - index : 0;
    }
    if (length > 0)
        memcpy(&result, &data[index], length);
    return result;
  }

  int32_t getInt32(uint8_t index) {
    int32_t result = 0;
    uint8_t length = sizeof(result);
    if (index + length > SENSOR_DATA_FIXED_LENGTH) {
      length = SENSOR_DATA_FIXED_LENGTH > index ? SENSOR_DATA_FIXED_LENGTH - index : 0;
    }
    if (length > 0)
        memcpy(&result, &data[index], length);
    return result;
  }
};

#endif
