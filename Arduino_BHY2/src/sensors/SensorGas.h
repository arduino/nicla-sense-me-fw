#ifndef SENSOR_GAS_H_
#define SENSOR_GAS_H_

#include "Sensor.h"

class SensorGas : public Sensor {
public:
  SensorGas() {} 
  SensorGas(uint8_t id) : Sensor(id), _value(0.), _factor(1.00) {}

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
    DataParser::parseGas(data, _value, _factor);
  }

  String toString()
  {
    return (String)("Gas value: " + String(_value, 3)  + "\n");
  }

private:
  float _factor;
  float _value;
};

#endif
