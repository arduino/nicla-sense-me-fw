#include "EslovHandler.h"

#define ESLOV_DELAY (1)

EslovHandler::EslovHandler() :
  _rxIndex(0),
  _rxBuffer(),
  _eslovState(ESLOV_AVAILABLE_SENSOR_STATE),
  _intPinAsserted(false),
  _intPinCleared(false),
  _dfuLedOn(false),
  _debug(NULL),
  _eslovIntPin(ESLOV_INT_PIN)
{
}

EslovHandler::~EslovHandler()
{
}

bool EslovHandler::begin(bool passthrough)
{
  pinMode(_eslovIntPin, INPUT);
  Wire.begin();
  Wire.setClock(400000);
  if (passthrough) {
    Serial.begin(115200);
  }

  return true;
}

void EslovHandler::update() 
{
  while (Serial.available()) {
    _rxBuffer[_rxIndex++] = Serial.read();

    if (_rxBuffer[0] == HOST_DFU_EXTERNAL_OPCODE || _rxBuffer[0] == HOST_DFU_INTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {

        if (!_dfuLedOn) {
          pinMode(LED_BUILTIN, OUTPUT);
          digitalWrite(LED_BUILTIN, HIGH);
          flushWire();
        }

        //Wait for Nicla to set _eslovIntPin HIGH, meaning that is ready to receive
        while(!digitalRead(_eslovIntPin)) {
          if (_debug) _debug->println("Waiting for Eslov Int pin to be released");
        }

        writeDfuPacket(_rxBuffer, sizeof(DFUPacket) + 1);

        if (digitalRead(_eslovIntPin)) {
          if (_debug) _debug->println("Eslov INT pin HIGH.");
        } else {
          if (_debug) _debug->println("Eslov INT pin STILL LOW");
          while(!digitalRead(_eslovIntPin)) {}
        }

        uint16_t index = _rxBuffer[2];

        uint8_t ack = 15;
        if (_debug) {
          // print ack received
          _debug->print("Packet received from Nicla. Index: ");
          _debug->println(index);
        }

        dump();

        _rxIndex = 0;
      
        Serial.write(ack);
      }

    } else if (_rxBuffer[0] == HOST_READ_SENSOR_OPCODE) {

      if (_debug) {
        _debug->print("received read sensor opcode\r\n");
      }
      uint8_t availableData = requestAvailableData();
      Serial.write(availableData);

      SensorDataPacket sensorData;
      while (availableData) {
        requestSensorData(sensorData);
        /*
        This delay is needed because the synchronization mechanism over the Eslov Int Pin
        may not apply for the requests from the host board to Nicla.
        It may happen that the onRequest callback on Nicla side is serviced after a certain delay.
        We need to add this delay of 10ms to avoid that a second request is issued before
        the first one is handled.
        */
        delay(10);
        Serial.write((uint8_t*)&sensorData, sizeof(SensorDataPacket));
        availableData--;
      }

      _rxIndex = 0;

    } else if (_rxBuffer[0] == HOST_READ_LONG_SENSOR_OPCODE) {

      if (_debug) {
        _debug->print("received read sensor opcode\r\n");
      }
      uint8_t availableData = requestAvailableLongData();
      Serial.write(availableData);

      SensorLongDataPacket sensorData;
      while (availableData) {
        //delay(ESLOV_DELAY);
        requestSensorLongData(sensorData);
        delay(10);
        Serial.write((uint8_t*)&sensorData, sizeof(SensorLongDataPacket));
        availableData--;
      }

      _rxIndex = 0;

    } else if (_rxBuffer[0] == HOST_CONFIG_SENSOR_OPCODE) {
      if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {

        SensorConfigurationPacket* config = (SensorConfigurationPacket*)&_rxBuffer[1];
        if (_debug) {
          _debug->print("received config: ");
          _debug->println(config->sensorId);
          _debug->println(config->sampleRate);
          _debug->println(config->latency);
          _debug->println();
        }
        writeConfigPacket(*config);

        uint8_t ack = requestPacketAck();

        if (_debug) {
          // print ack received
          _debug->print("Sent Ack: ");
          _debug->println(ack);
        }

        Serial.write(ack);

        _rxIndex = 0;
      }
      
    } else {
    if (_debug) {
      _debug->println("no opcode");
    }
      _rxIndex = 0;
    }
  }
  
}

void EslovHandler::writeDfuPacket(uint8_t *data, uint8_t length)
{
  uint8_t attempts = 0;
  uint8_t bytesToWrite = length;
  while(bytesToWrite && (attempts < 3)) {
    Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
    int ret = Wire.write(data, length);
    if (_debug){
      _debug->print("Write returned: ");
      _debug->print(ret);
      _debug->println();
    }
    /*
    endTransmission returns 0 if the number of bytes written
    *  is equal to the total length. Otherwise, a positive number is returned
    */
    bytesToWrite = Wire.endTransmission(true);
    attempts++;
  }
  if (*(data+1)) {
    //Last packet
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    _dfuLedOn = false;
    _intPinAsserted = false;
  }
}

void EslovHandler::writeStateChange(EslovState state)
{
  while(!digitalRead(_eslovIntPin)) {}
  uint8_t packet[2] = {ESLOV_SENSOR_STATE_OPCODE, state};
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write((uint8_t*)packet, sizeof(packet));
  Wire.endTransmission();
  _eslovState = state;
}

void EslovHandler::writeConfigPacket(SensorConfigurationPacket& config)
{
  while(!digitalRead(_eslovIntPin)) {}
  uint8_t packet[sizeof(SensorConfigurationPacket) + 1]; 
  packet[0] = ESLOV_SENSOR_CONFIG_OPCODE;
  memcpy(&packet[1], &config, sizeof(SensorConfigurationPacket));
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write(packet, sizeof(SensorConfigurationPacket) + 1);
  Wire.endTransmission();
}

uint8_t EslovHandler::requestPacketAck()
{
  while(!digitalRead(_eslovIntPin)) {}
  uint8_t ret = 0;
  while(!ret) {
    ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, 1);
    if (_debug){
      _debug->print("Request returned: ");
      _debug->println(ret);
    }
  }
  return Wire.read();
}

uint8_t EslovHandler::requestAvailableData() 
{
  writeStateChange(ESLOV_AVAILABLE_SENSOR_STATE);
  while(!digitalRead(_eslovIntPin)) {}
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, 1);
  if (!ret) return 0;
  return Wire.read();
}

uint8_t EslovHandler::requestAvailableLongData() 
{
  writeStateChange(ESLOV_AVAILABLE_LONG_SENSOR_STATE);
  while(!digitalRead(_eslovIntPin)) {}
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, 1);
  if (!ret) return 0;
  return Wire.read();
  delay(ESLOV_DELAY);
}

bool EslovHandler::requestSensorData(SensorDataPacket &sData)
{
  if (_eslovState != ESLOV_READ_SENSOR_STATE) {
    writeStateChange(ESLOV_READ_SENSOR_STATE);
  }
  while(!digitalRead(_eslovIntPin)) {}
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, sizeof(SensorDataPacket));
  if (!ret) return false;

  uint8_t *data = (uint8_t*)&sData;
  for (uint8_t i = 0; i < sizeof(SensorDataPacket); i++) {
    data[i] = Wire.read();
  }
  return true;
}

bool EslovHandler::requestSensorLongData(SensorLongDataPacket &sData)
{
  if (_eslovState != ESLOV_READ_LONG_SENSOR_STATE) {
    writeStateChange(ESLOV_READ_LONG_SENSOR_STATE);
  }
  while(!digitalRead(_eslovIntPin)) {}
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, sizeof(SensorLongDataPacket));
  if (!ret) return false;

  uint8_t *data = (uint8_t*)&sData;
  for (uint8_t i = 0; i < sizeof(SensorLongDataPacket); i++) {
    data[i] = Wire.read();
  }
  return true;
}

void EslovHandler::niclaAsShield()
{
  _eslovIntPin = I2C_INT_PIN;
}

void EslovHandler::flushWire()
{
  Wire.end();
  Wire.begin();
  Wire.setClock(400000);
}

void EslovHandler::debug(Stream &stream)
{
  _debug = &stream;
}

void EslovHandler::dump() 
{
  if (_debug) {
    _debug->print("received: ");
    _debug->println(_rxIndex);
    for (int i = 0; i < _rxIndex; i++) {
      _debug->print(_rxBuffer[i], HEX);
      _debug->print(", ");
    }
    _debug->println();
  }
}

EslovHandler eslovHandler;
