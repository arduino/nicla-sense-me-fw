#ifndef SENSOR_XYZ_H_
#define SENSOR_XYZ_H_

#include "SensorClass.h"

class SensorXYZ : public SensorClass {
public:
  SensorXYZ() {}
  SensorXYZ(uint8_t id) : SensorClass(id), _data() {}

  int16_t x() 
  { 
    return _data.x; 
  }
  int16_t y()
  {
    return _data.y;
  }
  int16_t z()
  {
    return _data.z;
  }

  void setData(SensorDataPacket &data)
  {
    DataParser::parse3DVector(data, _data);
  }

  String toString()
  {
    return _data.toString();
  }

private:
  DataXYZ _data;

};

#endif
