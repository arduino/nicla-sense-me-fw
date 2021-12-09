#include "SensorManager.h"

SensorManager::SensorManager() :
  _sensors(),
  _sensorsLen(0)
{
}

void SensorManager::process(SensorLongDataPacket &data)
{
  for (int i = 0; i < _sensorsLen; i++) {
    if (data.sensorId == _sensors[i]->id()) {
      if (_sensors[i]->id() == 115 || _sensors[i]->id() == 171) {
        // BSEC sensor is the only one with long payload
        _sensors[i]->setData(data);
      } else {
        // All the other sensors have short payloads
        SensorDataPacket shortData;
        memcpy(&shortData, &data, sizeof(SensorDataPacket));
        _sensors[i]->setData(shortData);
      }
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
