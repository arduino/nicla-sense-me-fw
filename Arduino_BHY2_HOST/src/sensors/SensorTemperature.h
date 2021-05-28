#ifndef SENSOR_TEMPERATURE_H_
#define SENSOR_TEMPERATURE_H_

#include "Sensor.h"

class SensorTemperature : public Sensor {
public:
  SensorTemperature() {} 
  SensorTemperature(uint8_t id) : Sensor(id), _value(0.), _factor(0.01) {}

  float value() 
  { 
    return _value; 
  }

  void setFactor(float factor)
  {
    _factor = factor;
  }
  void setData(SensorDataPacket &data)
  {
    DataParser::parseTemperature(data, _value, _factor);
  }

  String toString()
  {
    return (String)("Temperature value: " + String(_value, 3)  + "\n");
  }

private:
  float _factor;
  float _value;
};

#endif
