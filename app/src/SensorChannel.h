#ifndef SENSOR_CHANNEL_H_
#define SENSOR_CHANNEL_H_

#include "Arduino.h"
#include "BoschSensortec/BoschSensortec.h"

// For future more generic use
struct __attribute__((packed)) SensorDataHeader {
  uint8_t sensorId;
  uint8_t length;
};

struct __attribute__((packed)) SensorDataPacket {
  uint8_t sensorId;
  uint8_t length;
  uint64_t data;
};

struct __attribute__((packed)) SensorConfigurationPacket {
  uint8_t sensorId;
  float sampleRate;
  uint32_t latency;
};

enum SensorPacketType {
  // Outcoming packets
  SENSOR_DATA_PACKET,
  // Incoming packets
  SENSOR_CONFIG_PACKET,
  SENSOR_REQUEST_PACKET
};

class SensorChannel {
  public: 
    SensorChannel();
    virtual ~SensorChannel();

    void begin();
    uint8_t processPacket(SensorPacketType type, const uint8_t* data);

    uint8_t getNextDataPacket(uint8_t* data);

  private:
};

extern SensorChannel sensorChannel;

#endif
