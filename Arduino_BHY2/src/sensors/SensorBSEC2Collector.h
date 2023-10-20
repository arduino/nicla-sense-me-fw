#ifndef SENSOR_BSEC2COLLECTOR_H_
#define SENSOR_BSEC2COLLECTOR_H_

#include "SensorClass.h"


class SensorBSEC2Collector : public SensorClass {
public:
  SensorBSEC2Collector() {}
  SensorBSEC2Collector(uint8_t id) : SensorClass(id), _data() {}


  uint64_t timestamp() {return _data.timestamp;}
  float temperature() {return _data.raw_temp;}
  float pressure() {return _data.raw_pressure;}
  float humidity() {return _data.raw_hum;}
  float gas() {return _data.raw_gas;}
  uint8_t gas_index() {return _data.gas_index;}

  void setData(SensorDataPacket &data)
  {
  }

  void setData(SensorLongDataPacket &data)
  {
      if (_id == SENSOR_ID_BSEC2_COLLECTOR ) {
          DataParser::parseBSEC2Collector(data, _data);
      }
  }

  String toString()
  {
    return _data.toString();
  }

private:
  DataBSEC2Collector _data;
};
#endif
