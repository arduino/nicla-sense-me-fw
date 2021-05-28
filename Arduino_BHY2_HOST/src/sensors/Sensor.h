#ifndef SENSOR_H_
#define SENSOR_H_

#include "sensors/SensorID.h"
#include "sensors/DataParser.h"

class Sensor {
public:
  __attribute__ ((error("Sensor requires an ID"))) Sensor();
  Sensor(uint8_t id);
  virtual ~Sensor();

  uint8_t id();
  void configure(float rate, uint32_t latency);
  void disable();

  virtual void setData(SensorDataPacket &data) = 0;
  virtual String toString() = 0;

protected:
  uint8_t _id;
  bool _subscribed;
};

#endif
