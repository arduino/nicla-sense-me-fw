#ifndef SENSOR_ACTIVITY_H_
#define SENSOR_ACTIVITY_H_

#include "SensorClass.h"

struct ActivityBitMask{
  uint8_t bit;
  String activityMessage;
};

class SensorActivity : public SensorClass {
public:
  SensorActivity() {} 
  SensorActivity(uint8_t id) : SensorClass(id), _value(0) {}

  uint16_t value() 
  { 
    return _value; 
  }

  void setData(SensorDataPacket &data)
  {
    DataParser::parseActivity(data, _value);
  }

  String toString()
  {
    return getActivity();
  }

  String getActivity()
  {
    for (int i = 0; i < 16; i++) {
      uint16_t maskedVal = (_value & (0x0001 << i)) >> i;
      if (maskedVal) {
        return _activityArray[i].activityMessage;
      }
    }
  }

private:
  uint16_t _value;
  ActivityBitMask _activityArray[16] = {
  {0,  "Still activity ended"},
  {1,  "Walking activity ended"},
  {2,  "Running activity ended"},
  {3,  "On bicycle activity ended"},
  {4,  "In vehicle activity ended"},
  {5,  "Tilting activity ended"},
  {6,  "In vehicle still ended"},
  {7,  ""},
  {8,  "Still activity started"},
  {9,  "Walking activity started"},
  {10, "Running activity started"},
  {11, "On bicycle activity started"},
  {12, "IN vehicle activity started"},
  {13, "Tilting activity started"},
  {14, "In vehicle still started"},
  {15, ""}
  };
};

#endif
