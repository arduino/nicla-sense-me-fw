#include "Arduino_BHY2.h"

#include "BoschSensortec.h"
#include "BoschParser.h"
#include "BLEHandler.h"
#include "EslovHandler.h"
#include "DFUManager.h"
#include "Wire.h"

#include "mbed.h"
#include "Nicla_System.h"

Arduino_BHY2::Arduino_BHY2() :
  _debug(NULL),
  _pingTime(0),
  _timeout(120000),
  _eslovActive(false),
  _startTime(0),
  _eslovIntPin(PIN_ESLOV_INT),
  _niclaConfig(NICLA_BLE_AND_I2C)
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
    nicla::readLDOreg();
  }
}

void Arduino_BHY2::checkEslovInt() {
  if (!digitalRead(_eslovIntPin)) {
    //Wait for MKR to clear Eslov Int pin
    while(!digitalRead(_eslovIntPin)) {}
    if (_debug) _debug->println("MKR released Eslov Int pin");

    //Change mode for Eslov Int pin
    //Eslov Int Pin will be used to synchronize Dfu via Eslov
    pinMode(_eslovIntPin, OUTPUT);

    eslovHandler.begin();
    //Eslov has been activated
    _eslovActive = true;
  }
}

void Arduino_BHY2::setLDOTimeout(int time) {
  _timeout = time;
}

bool Arduino_BHY2::begin(NiclaConfig config, NiclaWiring niclaConnection)
{
  _niclaConfig = config;

  if (niclaConnection == NICLA_AS_SHIELD) {
    _eslovIntPin = I2C_INT_PIN;
    eslovHandler.niclaAsShield();
  }

  pinMode(_eslovIntPin, INPUT);
  nicla::begin();
  _startTime = millis();
  nicla::enable3V3LDO();
  Wire1.setClock(500000);
  _pingTime = millis();
  if (!sensortec.begin()) {
    return false;
  }
  if (_niclaConfig & NICLA_BLE) {
    if (!bleHandler.begin()) {
      return false;
    }
  }
  if (!dfuManager.begin()) {
    return false;
  }

  if (_debug) {
    _debug->print("Eslov int pin: ");
    _debug->println(_eslovIntPin);
  }

  return true;
}

bool Arduino_BHY2::begin(NiclaSettings& settings)
{
  uint8_t niclaSettings = settings.getConfiguration();
  if ((niclaSettings & NICLA_I2C) && (niclaSettings & NICLA_BLE)) {
    _niclaConfig = NICLA_BLE_AND_I2C;
  }
  else if (niclaSettings & NICLA_I2C) {
    _niclaConfig = NICLA_I2C;
  }
  else {
    _niclaConfig = NICLA_BLE;
  }
  if (niclaSettings & NICLA_AS_SHIELD) {
    return begin(_niclaConfig, NICLA_AS_SHIELD);
  } else {
    return begin(_niclaConfig, NICLA_VIA_ESLOV);
  }
}

void Arduino_BHY2::update()
{
  pingI2C();

  if (_niclaConfig & NICLA_I2C) {
    if (!_eslovActive) {
      checkEslovInt();
    }
  }

  sensortec.update();
  if (_niclaConfig & NICLA_BLE) {
    bleHandler.update();
  }

  // While updating fw, detach the library from the sketch
  if (dfuManager.isPending()) {
    if (_debug) _debug->println("Start DFU procedure. Sketch execution is stopped.");
    while (dfuManager.isPending()) {
      if (_niclaConfig & NICLA_BLE) {
        if (dfuManager.dfuSource() == bleDFU && bleHandler.bleActive) {
          bleHandler.update();
        }
      }
      pingI2C();
    }
    // Wait some time for acknowledgment retrieval
    if (_niclaConfig & NICLA_BLE) {
      if (dfuManager.dfuSource() == bleDFU) {
        auto timeRef = millis();
        while (millis() - timeRef < 1000) {
          bleHandler.update();
        }
      }
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
  if (_niclaConfig & NICLA_BLE) {
    while (elapsed < ms) {
      bleHandler.poll(ms - elapsed);
      elapsed = millis() - start;
    }
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

void Arduino_BHY2::addSensorData(SensorDataPacket &sensorData)
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
  DataParser::parse3DVector(data, vector);
}

void Arduino_BHY2::parse(SensorDataPacket& data, DataOrientation& vector)
{
  DataParser::parseEuler(data, vector);
}

void Arduino_BHY2::parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor)
{
  DataParser::parseEuler(data, vector, scaleFactor);
}

void Arduino_BHY2::debug(Stream &stream)
{
  _debug = &stream;
  if (_niclaConfig & NICLA_I2C) {
    eslovHandler.debug(stream);
  }
  if (_niclaConfig & NICLA_BLE) {
    BLEHandler::debug(stream);
  }
  sensortec.debug(stream);
  dfuManager.debug(stream);
  BoschParser::debug(stream);
}

Arduino_BHY2 BHY2;
