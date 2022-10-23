#ifndef SENSOR_H_
#define SENSOR_H_

#include "SensorClass.h"

class Sensor : public SensorClass {
public:
  Sensor() {} 
  Sensor(uint8_t id) : SensorClass(id), _value(0.), _factor(0) {
    for (int i = 0; i < NUM_SUPPORTEND_SENSOR; i++) {
      if (SensorList[i].id == id) {
        _factor = SensorList[i].scaleFactor;
        _format = SensorList[i].payload;
      }
    }
  }

  float value() 
  {
    if (_format == PEVENT) {
      if (_value > 0) {
        _value = 0;
        return 1;
      }
      return 0;
    }
    return _value;
  }

  void setFactor(float factor)
  {
    _factor = factor;
  }

  float getFactor()
  {
    return _factor;
  }

  void setData(SensorDataPacket &data)
  {
    DataParser::parseData(data, _value, _factor, _format);
  }

  void setData(SensorLongDataPacket &data) {}

  String toString()
  {
    if (_format == PEVENT) {
      if (value()) {
        return (String)("Event detected\n");
      }
      return (String)("");
    }
    return (String)("Data value: " + String(_value, 3)  + "\n");
  }

private:
  float _value;
  float _factor;
  SensorPayload _format;
};

#endif
