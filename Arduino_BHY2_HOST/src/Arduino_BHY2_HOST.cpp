#include "Arduino_BHY2_HOST.h"

#include "EslovHandler.h"

Arduino_BHY2_HOST::Arduino_BHY2_HOST() :
  _passthrough(false)
{
}

Arduino_BHY2_HOST::~Arduino_BHY2_HOST()
{
}

void Arduino_BHY2_HOST::begin(bool passthrough)
{
  _passthrough = passthrough;
  eslovHandler.begin(passthrough);
}

void Arduino_BHY2_HOST::update()
{
  if (_passthrough){
    eslovHandler.update();
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
  DataParser::parse(data, vector);
}

void Arduino_BHY2_HOST::parse(SensorDataPacket& data, DataOrientation& vector)
{
  DataParser::parse(data, vector);
}

void Arduino_BHY2_HOST::parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor)
{
  DataParser::parse(data, vector, scaleFactor);
}

void Arduino_BHY2_HOST::debug(Stream &stream)
{
  eslovHandler.debug(stream);
}

Arduino_BHY2_HOST BHY2_HOST;
