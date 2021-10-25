#include "Arduino_BHY2Host.h"

#include "EslovHandler.h"
#include "BLEHandler.h"
#include "sensors/SensorManager.h"

Arduino_BHY2Host::Arduino_BHY2Host() :
  _passthrough(false),
  _wiring(NICLA_VIA_ESLOV),
  _debug(NULL)
{
}

Arduino_BHY2Host::~Arduino_BHY2Host()
{
}

bool Arduino_BHY2Host::begin(bool passthrough, NiclaWiring niclaConnection)
{
  _passthrough = passthrough;
  _wiring = niclaConnection;
  if (niclaConnection == NICLA_VIA_BLE) {
    if (_debug) {
      _debug->println("NICLA_VIA_BLE selected");
    }
    return bleHandler.begin();
  }
  if (niclaConnection == NICLA_AS_SHIELD) {
    eslovHandler.niclaAsShield();
  }
  return eslovHandler.begin(passthrough);
}

void Arduino_BHY2Host::update()
{
  if (_wiring == NICLA_VIA_BLE) {
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

void Arduino_BHY2Host::update(unsigned long ms)
{
  update();
  delay(ms);
}

void Arduino_BHY2Host::configureSensor(SensorConfigurationPacket& config)
{
  if (_wiring == NICLA_VIA_BLE) {
    bleHandler.writeConfigPacket(config);
  } else {
    eslovHandler.writeConfigPacket(config);
  }
}

void Arduino_BHY2Host::configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency)
{
  SensorConfigurationPacket config;
  config.sensorId = sensorId;
  config.sampleRate = sampleRate;
  config.latency = latency;
  if (_wiring == NICLA_VIA_BLE) {
    bleHandler.writeConfigPacket(config);
  } else {
    eslovHandler.writeConfigPacket(config);
  }
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

NiclaWiring Arduino_BHY2Host::getNiclaConnection()
{
  return _wiring;
}

void Arduino_BHY2Host::debug(Stream &stream)
{
  _debug = &stream;
  eslovHandler.debug(stream);
  bleHandler.debug(stream);
}

Arduino_BHY2Host BHY2Host;
