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

/*
void setFactor(float factor)
{
  _factor = factor;
}

void setData(SensorDataPacket &data)
{
  _format = SensorList[_id].payload;
  switch (_format) {
    case QUATERNION:
      DataParser::parseQuaternion(data, _quaternion_data, _factor);
      break;
    case 3DVECTOR:
      DataParser::parse3DVector(data, _xyz_data, _factor);
      break;
    case EULER:
      DataParser::parseEuler(data, _orientation_data, _factor);
      break;
    default:
      DataParser::parseData(data, _data, _factor);
      break;
  }
}

String toString()
{
  String str;

  switch (_format) {
    case QUATERNION:
      str = _quaternion_data.toString();
      break;
    case 3DVECTOR:
      str = _xyz_data.toString();
      break;
    case EULER:
      str = _orientation_data.toString();
      break;
    default:
      str = (String)("Data value: " + String(_value, 3)  + "\n");
      break;
  }
  return str;
}
*/