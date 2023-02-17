#ifndef SENSOR_BSEC2_H_
#define SENSOR_BSEC2_H_

#include "SensorClass.h"


class SensorBSEC2 : public SensorClass {
public:
  SensorBSEC2() {}
  SensorBSEC2(uint8_t id) : SensorClass(id), _data() {}

  bool getNewDataFlag() { return _newDataFlag;}
  void setNewDataFlag(bool flag) { _newDataFlag = flag;}

  uint8_t gas_estimates0() {return _data.gas_estimates[0];}
  uint8_t gas_estimates1() {return _data.gas_estimates[1];}
  uint8_t gas_estimates2() {return _data.gas_estimates[2];}
  uint8_t gas_estimates3() {return _data.gas_estimates[3];}
  uint8_t accuracy() {return _data.accuracy;}


  void setData(SensorDataPacket &data)
  {
      if (_id == SENSOR_ID_BSEC2 ) {
          DataParser::parseBSEC2(data, _data);
          _newDataFlag = true;
      }
  }

  void setData(SensorLongDataPacket &data)
  {
  }

  String toString()
  {
    return _data.toString();
  }

private:
  DataBSEC2 _data;
  bool _newDataFlag;
};
#endif
