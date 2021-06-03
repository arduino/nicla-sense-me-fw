#ifndef SENSOR_MANAGER_H_
#define SENSOR_MANAGER_H_

#include "Sensor.h"

class SensorManager {
public:
  SensorManager();
  void process(SensorDataPacket &data);

  void subscribe(Sensor *sensor);
  void unsubscribe(Sensor *sensor);

private:
  Sensor* _sensors[10]; // array of 256 or list to handle unsubscription
  int _sensorsLen;
};

extern SensorManager sensorManager;

#endif
