#include "EslovHandler.h"

#define ESLOV_DELAY (10)

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
  pinMode(_eslovIntPin, OUTPUT);
  digitalWrite(_eslovIntPin, LOW);

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

        toggleEslovIntPin();

        if (!_dfuLedOn) {
          pinMode(LED_BUILTIN, OUTPUT);
          digitalWrite(LED_BUILTIN, HIGH);
          flushWire();
        }

        pinMode(_eslovIntPin, INPUT);

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

        delay(ESLOV_DELAY);
      
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
        //delay(ESLOV_DELAY);
        requestSensorData(sensorData);
        delay(ESLOV_DELAY);
        Serial.write((uint8_t*)&sensorData, sizeof(sensorData));
        availableData--;
      }

      _rxIndex = 0;

    } else if (_rxBuffer[0] == HOST_CONFIG_SENSOR_OPCODE) {
      if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {

        toggleEslovIntPin();
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
  delay(ESLOV_DELAY);
  while(!digitalRead(_eslovIntPin)) {}
  uint8_t packet[2] = {ESLOV_SENSOR_STATE_OPCODE, state};
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write((uint8_t*)packet, sizeof(packet));
  Wire.endTransmission();
  delay(ESLOV_DELAY);
  _eslovState = state;
}

void EslovHandler::writeConfigPacket(SensorConfigurationPacket& config)
{
  delay(ESLOV_DELAY);
  uint8_t packet[sizeof(SensorConfigurationPacket) + 1]; 
  packet[0] = ESLOV_SENSOR_CONFIG_OPCODE;
  memcpy(&packet[1], &config, sizeof(SensorConfigurationPacket));
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write(packet, sizeof(SensorConfigurationPacket) + 1);
  Wire.endTransmission();
  delay(ESLOV_DELAY);
}

uint8_t EslovHandler::requestPacketAck()
{ 
  delay(ESLOV_DELAY);
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
  delay(ESLOV_DELAY);
}

bool EslovHandler::requestSensorData(SensorDataPacket &sData)
{
  if (_eslovState != ESLOV_READ_SENSOR_STATE) {
    writeStateChange(ESLOV_READ_SENSOR_STATE);
    while(!digitalRead(_eslovIntPin)) {}
  }
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, sizeof(SensorDataPacket));
  if (!ret) return false;

  uint8_t *data = (uint8_t*)&sData;
  for (uint8_t i = 0; i < sizeof(SensorDataPacket); i++) {
    data[i] = Wire.read();
  }
  return true;
}

void EslovHandler::toggleEslovIntPin()
{
  if (!_intPinAsserted) {
    // Indicates eslov presence
    pinMode(_eslovIntPin, OUTPUT);
    digitalWrite(_eslovIntPin, LOW);
    _intPinAsserted = true;
    if (_debug) {
      _debug->println("Eslov int LOW");
    }
    //Use 1 sec delay to let Nicla see the LOW pin and enable Eslov
    delay(500);

    digitalWrite(_eslovIntPin, HIGH);
    _intPinCleared = true;
    if (_debug) {
      _debug->println("Eslov int pin cleared");
    }
    delay(500);
  }
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
