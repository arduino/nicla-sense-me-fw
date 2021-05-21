#include "Arduino_BHY2.h"

#include "BoschSensortec.h"
#include "BoschParser.h"
#include "BLEHandler.h"
#include "EslovHandler.h"
#include "DFUManager.h"
#include <I2C.h>

#include "mbed.h"
#include "Nicla_System.h"

mbed::I2C I2C(I2C_SDA0, I2C_SCL0);

mbed::DigitalIn eslovInt(p19);

Arduino_BHY2::Arduino_BHY2() :
  _debug(NULL),
  _pingTime(0),
  _timeout(60000),
  _timeoutExpired(false),
  _eslovActive(false)
{
}

Arduino_BHY2::~Arduino_BHY2()
{
}

void Arduino_BHY2::pingI2C() {
  char response = 0xFF;
  int currTime = millis();
  if ((currTime - _pingTime) > 30000) {
    _pingTime = currTime;
    //Read status reg
    int ret = I2C.write(BQ25120A_ADDRESS << 1, 0, 1);
    ret = I2C.read(BQ25120A_ADDRESS << 1, &response, 1);
  }
}

void Arduino_BHY2::checkEslovInt() {
  if (millis() - _startTime < _timeout) {
    //Timeout didn't expire yet
    if (!eslovInt) {
      //Eslov has been activated
      _eslovActive = true;
    }
  } else {
    //Timeout expired
    _timeoutExpired = true;
    disableLDO();
  }
}

void Arduino_BHY2::setLDOTimeout(int time) {
  _timeout = time;
}

bool Arduino_BHY2::begin()
{
  I2C.frequency(500000);
  _pingTime = millis();
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

  if (eslovInt) {
    //Eslov is NOT active
    _startTime = millis();
  } else {
    //Eslov is already active
    _eslovActive = true;
  }

  return true;
}

void Arduino_BHY2::update()
{
  pingI2C();

  if (!_timeoutExpired && !_eslovActive) {
    checkEslovInt();
  }

  sensortec.update();
  bleHandler.update();

  // While updating fw, detach the library from the sketch
  if (dfuManager.isPending()) {
    if (_debug) _debug->println("Start DFU procedure. Sketch execution is stopped.");
    // TODO: abort dfu
    while (dfuManager.isPending()) {
      pingI2C();
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

// Update and then sleep
void Arduino_BHY2::update(unsigned long ms)
{
  update();
  delay(ms);
}

void Arduino_BHY2::delay(unsigned long ms) 
{
  unsigned long start = millis();
  unsigned long elapsed = 0;
  while (elapsed < ms) {
    bleHandler.poll(ms - elapsed);
    elapsed = millis() - start;
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
