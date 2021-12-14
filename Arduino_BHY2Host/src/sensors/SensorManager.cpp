#include "SensorManager.h"
#include "SensorID.h"

SensorManager::SensorManager() :
  _sensors(),
  _sensorsLen(0)
{
}

void SensorManager::process(SensorLongDataPacket &data)
{
  for (int i = 0; i < _sensorsLen; i++) {
    if (data.sensorId == _sensors[i]->id()) {

      bool longSensor = false;

      for (int i = 0; i < NUM_LONG_SENSOR; i++) {
        if (LongSensorList[i].id == data.sensorId) {
          longSensor = true;
          break;
        }
      }

      if (longSensor) {
        _sensors[i]->setData(data);
      } else {
        // All the other sensors have short payloads
        SensorDataPacket shortData;
        memcpy(&shortData, &data, sizeof(SensorDataPacket));
        _sensors[i]->setData(shortData);
      }

      return;

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
