#include "Arduino_BHY2_HOST.h"

#include "EslovHandler.h"
#include "sensors/SensorManager.h"

Arduino_BHY2_HOST::Arduino_BHY2_HOST() :
  _passthrough(false)
{
}

Arduino_BHY2_HOST::~Arduino_BHY2_HOST()
{
}

bool Arduino_BHY2_HOST::begin(bool passthrough, bool niclaUsedAsShield)
{
  _passthrough = passthrough;
  if (niclaUsedAsShield) {
    eslovHandler.niclaAsShield();
  }
  return eslovHandler.begin(passthrough);
}

void Arduino_BHY2_HOST::update()
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

void Arduino_BHY2_HOST::configureSensor(SensorConfigurationPacket& config)
{
  eslovHandler.writeConfigPacket(config);
}

void Arduino_BHY2_HOST::configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency)
{
  SensorConfigurationPacket config;
  config.sensorId = sensorId;
  config.sampleRate = sampleRate;
  config.latency = latency;
  eslovHandler.writeConfigPacket(config);
}

uint8_t Arduino_BHY2_HOST::requestAck()
{
  return eslovHandler.requestPacketAck();
}

uint8_t Arduino_BHY2_HOST::availableSensorData()
{
  return eslovHandler.requestAvailableData();
}

bool Arduino_BHY2_HOST::readSensorData(SensorDataPacket &data)
{
  return eslovHandler.requestSensorData(data);
}

void Arduino_BHY2_HOST::parse(SensorDataPacket& data, DataXYZ& vector)
{
  DataParser::parse3DVector(data, vector);
}

void Arduino_BHY2_HOST::parse(SensorDataPacket& data, DataOrientation& vector)
{
  DataParser::parseEuler(data, vector);
}

void Arduino_BHY2_HOST::parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor)
{
  DataParser::parseEuler(data, vector, scaleFactor);
}

void Arduino_BHY2_HOST::debug(Stream &stream)
{
  eslovHandler.debug(stream);
}

Arduino_BHY2_HOST BHY2_HOST;
