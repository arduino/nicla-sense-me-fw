#ifndef SENSOR_H_
#define SENSOR_H_

#include "sensors/DataParser.h"
#include "BoschSensortec.h"

class Sensor {
public:
  //Sensor();
  Sensor(uint8_t id);
  virtual ~Sensor();

  //void init(uint8_t id);
  uint8_t id();
  //void configure(uint8_t id, float rate, uint32_t latency);
  void configure(float rate, uint32_t latency);
  void disable();

  // void setCallback();
  virtual void setData(SensorDataPacket &data) = 0;
  virtual String toString() = 0;

protected:
  Sensor();

  uint8_t _id;
  bool _subscribed;
};

#endif
