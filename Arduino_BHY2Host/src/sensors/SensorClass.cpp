#include "SensorClass.h"
#include "SensorManager.h"
#include "Arduino_BHY2Host.h"
#include "EslovHandler.h"

SensorClass::SensorClass() : 
  _id(0),
  _subscribed(false),
  _wiring(NICLA_VIA_ESLOV)
{
}

SensorClass::SensorClass(uint8_t id) : 
  _id(id),
  _subscribed(false),
  _wiring(NICLA_VIA_ESLOV)
{
}

SensorClass::~SensorClass()
{
  end();
}

uint8_t SensorClass::id() 
{
  return _id;
}

bool SensorClass::begin(NiclaWiring wiring)
{
  _wiring = wiring;
  return begin(1,0,wiring);
}

bool SensorClass::begin(float rate, uint32_t latency, NiclaWiring wiring)
{
  configure(rate, latency);
  return true;
}

void SensorClass::configure(float rate, uint32_t latency)
{
  SensorConfigurationPacket config {_id, rate, latency};

  if (_wiring == NICLA_VIA_BLE) {
    BHY2Host.configureSensor(config);
  } else {
    eslovHandler.toggleEslovIntPin();
    uint8_t ack = 0;
    while(ack != 15) {
      BHY2Host.configureSensor(config);
      ack = BHY2Host.requestAck();
    }
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

void SensorClass::end()
{
  configure(0, 0);
}
