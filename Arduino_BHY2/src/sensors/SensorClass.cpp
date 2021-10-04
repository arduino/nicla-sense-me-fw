#include "SensorClass.h"
#include "SensorManager.h"

SensorClass::SensorClass() : 
  _id(0),
  _subscribed(false)
{
}

SensorClass::SensorClass(uint8_t id) : 
  _id(id),
  _subscribed(false)
{
}

SensorClass::~SensorClass()
{
  disable();
}

uint8_t SensorClass::id() 
{
  return _id;
}

void SensorClass::configure(float rate, uint32_t latency)
{
  SensorConfigurationPacket config {_id, rate, latency};
  sensortec.configureSensor(config);

  if (rate == 0 && _subscribed) {
    // unregister sensor
    sensorManager.unsubscribe(this);
    _subscribed = false;
  } else if (rate > 0 && !_subscribed) {
    // register sensor
    sensorManager.subscribe(this);
    _subscribed = true;
  }

}

void SensorClass::disable()
{
  configure(0, 0);
}