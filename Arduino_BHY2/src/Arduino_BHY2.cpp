#include "Arduino_BHY2.h"

#include "BoschSensortec.h"
#include "BoschParser.h"
#include "BLEHandler.h"
#include "EslovHandler.h"
#include "DFUManager.h"
#include <I2C.h>

mbed::I2C i2c0(I2C_SDA0, I2C_SCL0);

uint8_t BQ25120A_ADDRESS = 0x6A;

Arduino_BHY2::Arduino_BHY2() :
  _debug(NULL),
  start_time(0)
{
}

Arduino_BHY2::~Arduino_BHY2()
{
}

void Arduino_BHY2::ping_i2c0() {
  char response = 0xFF;
  int curr_time = millis();
  if ((curr_time - start_time) > 30000) {
    start_time = curr_time;
    //Read status reg
    int ret = i2c0.write(BQ25120A_ADDRESS << 1, 0, 1);
    ret = i2c0.read(BQ25120A_ADDRESS << 1, &response, 1);
  }
}

bool Arduino_BHY2::begin()
{
  i2c0.frequency(500000);
  start_time = millis();
  if (!sensortec.begin()) {
    return false;
  }
  if (!bleHandler.begin()) {
    return false;
  }
  if (!eslovHandler.begin()) {
    return false;
  }
  if (!dfuManager.begin()) {
    return false;
  }
  return true;
}

void Arduino_BHY2::update()
{
  sensortec.update();
  bleHandler.update();

  // While updating fw, detach the library from the sketch
  if (dfuManager.isPending()) {
    if (_debug) _debug->println("Start DFU procedure. Sketch execution is stopped.");
    // TODO: abort dfu
    while (dfuManager.isPending()) {
      ping_i2c0();
      bleHandler.update();
    }
    // Wait some time for acknowledgment retrieval
    auto timeRef = millis();
    while (millis() - timeRef < 1000) {
      bleHandler.update();
    }
    // Reboot after fw update
    if (_debug) _debug->println("DFU procedure terminated. Rebooting.");
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

bool Arduino_BHY2::hasSensor(uint8_t sensorId)
{
  return sensortec.hasSensor(sensorId);
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
  _debug = &stream;
  eslovHandler.debug(stream);
  BLEHandler::debug(stream);
  sensortec.debug(stream);
  dfuManager.debug(stream);
  BoschParser::debug(stream);
}

Arduino_BHY2 BHY2;
