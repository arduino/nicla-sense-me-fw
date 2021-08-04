#include "Arduino_BHY2_HOST.h"

#include "EslovHandler.h"
#include "BLEHandler.h"
#include "sensors/SensorManager.h"

Arduino_BHY2_HOST::Arduino_BHY2_HOST() :
  _passthrough(false),
  _protocol(ESLOV)
{
}

Arduino_BHY2_HOST::~Arduino_BHY2_HOST()
{
}

bool Arduino_BHY2_HOST::begin(CommunicationProtocol protocol)
{
  _protocol = protocol;
  return begin(false);
}

bool Arduino_BHY2_HOST::begin(bool passthrough)
{
  _passthrough = passthrough;
  if (_protocol == BLE_BRIDGE) {
    return bleHandler.begin();
  }
  return eslovHandler.begin(passthrough);
}

void Arduino_BHY2_HOST::update()
{
  if (_protocol == BLE_BRIDGE) {
    bleHandler.update();
  } else {
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

}

void Arduino_BHY2_HOST::configureSensor(SensorConfigurationPacket& config)
{
  if (_protocol == BLE_BRIDGE) {
    bleHandler.writeConfigPacket(config);
  } else {
    eslovHandler.writeConfigPacket(config);
  }
}

void Arduino_BHY2_HOST::configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency)
{
  SensorConfigurationPacket config;
  config.sensorId = sensorId;
  config.sampleRate = sampleRate;
  config.latency = latency;
  if (_protocol == ESLOV) {
    eslovHandler.writeConfigPacket(config);
  } else {
    bleHandler.writeConfigPacket(config);
  }
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
  BLEHandler::debug(stream);
}

Arduino_BHY2_HOST BHY2_HOST;
