#include "Sensor.h"
#include "SensorManager.h"
#include "Arduino_BHY2_HOST.h"
#include "EslovHandler.h"

Sensor::Sensor() : 
  _id(0),
  _subscribed(false)
{
}

Sensor::Sensor(uint8_t id) : 
  _id(id),
  _subscribed(false)
{
}

Sensor::~Sensor()
{
  disable();
}

uint8_t Sensor::id() 
{
  return _id;
}

void Sensor::configure(float rate, uint32_t latency)
{
  eslovHandler.toggleEslovIntPin();
  SensorConfigurationPacket config {_id, rate, latency};

  uint8_t ack = 0;
  while(ack != 15) {
    BHY2_HOST.configureSensor(config);
    ack = BHY2_HOST.requestAck();
  }

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

void Sensor::disable()
{
  configure(0, 0);
}
