#include "Arduino_BHY2.h"

#include "BoschSensortec.h"
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
}

void Arduino_BHY2::configureSensor(SensorConfigurationPacket *config)
{
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

void Arduino_BHY2::debug(Stream &stream)
{
  eslovHandler.debug(stream);
}

Arduino_BHY2 BHY2;
