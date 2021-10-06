#include "SensorManager.h"

SensorManager::SensorManager() :
  _sensors(),
  _sensorsLen(0)
{
}

void SensorManager::process(SensorDataPacket &data)
{
  for (int i = 0; i < _sensorsLen; i++) {
    if (data.sensorId == _sensors[i]->id()) {
      _sensors[i]->setData(data);
      return; // can more sensor objects use the same sensor id?
    }
  }
}

void SensorManager::subscribe(SensorClass *sensor)
{
  _sensors[_sensorsLen++] = sensor;
}

void SensorManager::unsubscribe(SensorClass *sensor)
{
  for (int i = 0; i < _sensorsLen; i++) {
    if (sensor->id() == _sensors[i]->id()) {
      _sensors[i] = _sensors[--_sensorsLen];
      _sensors[_sensorsLen] = NULL;
      return; // can more sensor objects use the same sensor id?
    }
  }
}

SensorManager sensorManager;
