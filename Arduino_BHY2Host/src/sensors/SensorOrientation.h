#ifndef SENSOR_ORIENTATION_H_
#define SENSOR_ORIENTATION_H_

#include "SensorClass.h"

class SensorOrientation : public SensorClass {
public:
  SensorOrientation() {} 
  SensorOrientation(uint8_t id) : SensorClass(id), _data(), _factor(0) {
    for (int i = 0; i < NUM_SUPPORTEND_SENSOR; i++) {
      if (SensorList[i].id == id) {
        _factor = SensorList[i].scaleFactor;
      }
    }
  }

  float heading() 
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

  float getFactor()
  {
    return _factor;
  }

  void setData(SensorDataPacket &data)
  {
    DataParser::parseEuler(data, _data, _factor);
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
