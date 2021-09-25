#ifndef SENSOR_BSEC_H_
#define SENSOR_BSEC_H_

#include "SensorClass.h"

class SensorBSEC : public SensorClass {
public:
  SensorBSEC() {}
  SensorBSEC(uint8_t id) : SensorClass(id), _data() {}

  uint16_t iaq() {return _data.iaq;}
  uint16_t iaq_s() {return _data.iaq_s;}
  float b_voc_eq() {return _data.b_voc_eq;}
  uint32_t co2_eq() {return _data.co2_eq;}
  uint8_t accuracy() {return _data.accuracy;}
  float comp_t() {return _data.comp_t;}
  float comp_h() {return _data.comp_h;}
  uint32_t comp_g() {return _data.comp_g;}


  void setData(SensorDataPacket &data)
  {
      if (_id == SENSOR_ID_BSEC ) {
          DataParser::parseBSEC(data, _data);
      } else if (_id == SENSOR_ID_BSEC_LEGACY) {
          DataParser::parseBSECLegacy(data, _data);
      }
  }

  String toString()
  {
    return _data.toString();
  }

private:
  DataBSEC _data;
};
#endif
