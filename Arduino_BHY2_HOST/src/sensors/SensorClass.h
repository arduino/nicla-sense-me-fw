#ifndef SENSOR_CLASS_H_
#define SENSOR_CLASS_H_

#include "SensorID.h"
#include "DataParser.h"

enum CommunicationProtocol {
  ESLOV = 0,
  BLE_BRIDGE   = 1
};

class SensorClass {
public:
  __attribute__ ((error("Sensor requires an ID"))) SensorClass();
  SensorClass(uint8_t id);
  virtual ~SensorClass();

  uint8_t id();
  void configure(float rate, uint32_t latency, CommunicationProtocol protocol = ESLOV);
  void disable();

  virtual void setData(SensorDataPacket &data) = 0;
  virtual String toString() = 0;

protected:
  uint8_t _id;
  bool _subscribed;
};

#endif