#ifndef SENSOR_BSEC_H_
#define SENSOR_BSEC_H_

#include "SensorClass.h"


class SensorBSEC : public SensorClass {
public:
  SensorBSEC() {}
  SensorBSEC(uint8_t id) : SensorClass(id), _data() {}

/*
  BSEC sensor frames are:
    - 18 bytes for BSEC new format (SID=115 SENSOR_ID_BSEC_LEGACY)
    - 29 bytes for legacy format (SID=171 SENSOR_ID_BSEC)
  If the default size of SENSOR_DATA_FIXED_LENGTH is used (10 bytes), some fields of BSEC might be always 0.
  Enlarge SENSOR_DATA_FIXED_LENGTH to see all the fields of the BSEC sensor.
  For the new format (SID=115), if the compensated values (comp_t, comp_h, comp_g) are not important,
  keep SENSOR_DATA_FIXED_LENGTH to the default value (10) to save bandwidth.
*/
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
