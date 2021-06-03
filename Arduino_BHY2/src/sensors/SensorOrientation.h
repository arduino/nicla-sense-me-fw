#ifndef SENSOR_ORIENTATION_H_
#define SENSOR_ORIENTATION_H_

#include "Sensor.h"

class SensorOrientation : public Sensor {
public:
  SensorOrientation() {} 
  SensorOrientation(uint8_t id) : Sensor(id), _data(), _factor(0.01098) {}

  float head() 
  { 
    return _data.heading; 
  }
  float pitch()
  {
    return _data.pitch;
  }
  float roll()
  {
    return _data.roll;
  }

  void setFactor(float factor)
  {
    _factor = factor;
  }
  void setData(SensorDataPacket &data)
  {
    DataParser::parse(data, _data, _factor);
  }

  String toString()
  {
    return _data.toString();
  }

private:
  float _factor;
  DataOrientation _data;
};

#endif
