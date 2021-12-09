#ifndef SENSOR_CLASS_H_
#define SENSOR_CLASS_H_

#include "SensorID.h"
#include "DataParser.h"
#include "BoschSensortec.h"

class SensorClass {
public:
  __attribute__ ((error("Sensor requires an ID"))) SensorClass();
  SensorClass(uint8_t id);
  virtual ~SensorClass();

  uint8_t id();
  /* 
   * Sample rate: it indicates the frequency at which a sensor is sampled.
   *              It is expressed in Hz.
   * Latency: it indicates how much ms time a new value is retained in its fifo
   *          before a notification to the host is sent via interrupt.
   *          It is expressed in ms.
   */
  bool begin(float rate = 1000, uint32_t latency = 1);
  void configure(float rate, uint32_t latency);
  void end();

  virtual void setData(SensorDataPacket &data) = 0;
  virtual String toString() = 0;

protected:
  uint8_t _id;
  bool _subscribed;
};

#endif