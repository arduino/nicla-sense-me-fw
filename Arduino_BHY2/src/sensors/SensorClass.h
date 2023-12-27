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
  /*
   * parameter range:
   * for accelerometer, the range parameter is in the units of g (1g = ~9.80665 m/s^2)
   * valid values:
   * 2 (+/-2g), 4 (+/-4g), 8 (+/-8g), 16 (+/-16g),
   *
   * for gyroscope, the range parameter is in the units of dps (degrees per second)
   * valid values:
   *    125 (+/-125 dps)
   *    250 (+/-250 dps)
   *    500 (+/-500 dps)
   *    1000 (+/-1000 dps)
   *    2000 (+/-2000 dps)
   *
   * for other sensors, the range is defined based on the physical driver implementation
   * please note that: changing the range of one virtual sensor might affect the other virtual sensors which share the same underlying physical sensor, e.g.:
   * changing the accelerometer range will at the same time impact the sensing range for the gravity virtual sensor,
   * for more details please refer to the datasheet of BHI260AP, section "Change Sensor Dynamic Range"
   */
  int setRange(uint16_t range);
  const SensorConfig getConfiguration();
  void end();

  bool dataAvailable() {return _dataAvail;}
  bool clearDataAvailFlag(){_dataAvail = false;}


  virtual void setData(SensorDataPacket &data) = 0;
  virtual void setData(SensorLongDataPacket &data) = 0;
  virtual String toString() = 0;

protected:
  uint8_t _id;
  bool _subscribed;

  bool _dataAvail;

  void setDataAvailFlag() {_dataAvail = true;}

  friend class SensorManager;
};

#endif
