#ifndef SENSOR_QUATERNION_H_
#define SENSOR_QUATERNION_H_

#include "SensorClass.h"

class SensorQuaternion : public SensorClass {
public:
  SensorQuaternion() {} 
  SensorQuaternion(uint8_t id) : SensorClass(id), _data(), _factor(0.000061035) {}

  float x() 
  { 
    return _data.x; 
  }
  float y()
  {
    return _data.y;
  }
  float z()
  {
    return _data.z;
  }
  float w()
  {
    return _data.w;
  }
  float accuracy()
  {
    return _data.accuracy;
  }

  float getFactor()
  {
    return _factor;
  }

  void setData(SensorDataPacket &data)
  {
    DataParser::parseQuaternion(data, _data, _factor);
  }

  String toString()
  {
    return _data.toString();
  }

private:
  float _factor;
  DataQuaternion _data;
};

#endif
