#include "Arduino_BHY2Host.h"

#include "EslovHandler.h"
#include "sensors/SensorManager.h"

Arduino_BHY2Host::Arduino_BHY2Host() :
  _passthrough(false)
{
}

Arduino_BHY2Host::~Arduino_BHY2Host()
{
}

bool Arduino_BHY2Host::begin(bool passthrough, NiclaWiring niclaConnection)
{
  _passthrough = passthrough;
  if (niclaConnection == NICLA_AS_SHIELD) {
    eslovHandler.niclaAsShield();
  }
  return eslovHandler.begin(passthrough);
}

void Arduino_BHY2Host::update()
{
  if (_passthrough){
    eslovHandler.update();
  } else {
    while (availableSensorData() > 0) {
      SensorDataPacket data;
      readSensorData(data);
      sensorManager.process(data);
    }
  }

}

void Arduino_BHY2Host::configureSensor(SensorConfigurationPacket& config)
{
  eslovHandler.writeConfigPacket(config);
}

void Arduino_BHY2Host::configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency)
{
  SensorConfigurationPacket config;
  config.sensorId = sensorId;
  config.sampleRate = sampleRate;
  config.latency = latency;
  eslovHandler.writeConfigPacket(config);
}

uint8_t Arduino_BHY2Host::requestAck()
{
  return eslovHandler.requestPacketAck();
}

uint8_t Arduino_BHY2Host::availableSensorData()
{
  return eslovHandler.requestAvailableData();
}

bool Arduino_BHY2Host::readSensorData(SensorDataPacket &data)
{
  return eslovHandler.requestSensorData(data);
}

void Arduino_BHY2Host::parse(SensorDataPacket& data, DataXYZ& vector)
{
  DataParser::parse3DVector(data, vector);
}

void Arduino_BHY2Host::parse(SensorDataPacket& data, DataOrientation& vector)
{
  DataParser::parseEuler(data, vector);
}

void Arduino_BHY2Host::parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor)
{
  DataParser::parseEuler(data, vector, scaleFactor);
}

void Arduino_BHY2Host::debug(Stream &stream)
{
  eslovHandler.debug(stream);
}

Arduino_BHY2Host BHY2_HOST;
