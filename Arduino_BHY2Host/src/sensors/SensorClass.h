#ifndef SENSOR_CLASS_H_
#define SENSOR_CLASS_H_

#include "SensorID.h"
#include "DataParser.h"

enum NiclaWiring {
  NICLA_VIA_ESLOV = 0,
  NICLA_AS_SHIELD,
  NICLA_VIA_BLE
};

class SensorClass {
public:
  __attribute__ ((error("Sensor requires an ID"))) SensorClass();
  SensorClass(uint8_t id);
  virtual ~SensorClass();

  uint8_t id();
  bool begin(NiclaWiring wiring);
  bool begin(float rate = 1, uint32_t latency = 0, NiclaWiring wiring = NICLA_VIA_ESLOV);
  void configure(float rate, uint32_t latency);
  void end();

  virtual void setData(SensorDataPacket &data) = 0;
  virtual String toString() = 0;

protected:
  uint8_t _id;
  bool _subscribed;
  NiclaWiring _wiring;
};

#endif