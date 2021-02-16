#include "Arduino_BHY2.h"

#include "BoschSensortec.h"
#include "BoschParser.h"
#include "BLEHandler.h"
#include "EslovHandler.h"
#include "DFUManager.h"

Arduino_BHY2::Arduino_BHY2()
{
}

Arduino_BHY2::~Arduino_BHY2()
{
}

void Arduino_BHY2::begin()
{
  sensortec.begin();
  bleHandler.begin();
  eslovHandler.begin();
  dfuManager.begin();
}

void Arduino_BHY2::update()
{
  sensortec.update();
  bleHandler.update();

  // While updating fw, detach the library from the sketch
  if (dfuManager.isPending()) {
    // TODO: abort dfu
    while (dfuManager.isPending()) {
      bleHandler.update();
    }
    // Wait some time for acknowledgment retrieval
    auto timeRef = millis();
    while (millis() - timeRef < 1000) {
      bleHandler.update();
    }
    // Reboot after fw update
    NVIC_SystemReset();
  }
}

void Arduino_BHY2::configureSensor(SensorConfigurationPacket& config)
{
  sensortec.configureSensor(config);
}

void Arduino_BHY2::configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency)
{
  SensorConfigurationPacket config;
  config.sensorId = sensorId;
  config.sampleRate = sampleRate;
  config.latency = latency;
  sensortec.configureSensor(config);
}

void Arduino_BHY2::addSensorData(const SensorDataPacket &sensorData)
{
  sensortec.addSensorData(sensorData);
}

uint8_t Arduino_BHY2::availableSensorData()
{
  return sensortec.availableSensorData();
}

bool Arduino_BHY2::readSensorData(SensorDataPacket &data)
{
  return sensortec.readSensorData(data);
}

void Arduino_BHY2::parse(SensorDataPacket& data, DataXYZ& vector)
{
  DataParser::parse(data, vector);
}

void Arduino_BHY2::parse(SensorDataPacket& data, DataOrientation& vector)
{
  DataParser::parse(data, vector);
}

void Arduino_BHY2::parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor)
{
  DataParser::parse(data, vector, scaleFactor);
}

void Arduino_BHY2::debug(Stream &stream)
{
  eslovHandler.debug(stream);
  BLEHandler::debug(stream);
  sensortec.debug(stream);
  dfuManager.debug(stream);
  BoschParser::debug(stream);
}

Arduino_BHY2 BHY2;
