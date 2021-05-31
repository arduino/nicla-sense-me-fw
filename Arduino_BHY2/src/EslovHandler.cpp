#include "EslovHandler.h"

#include "DFUManager.h"
#include "BoschSensortec.h"

EslovHandler::EslovHandler() :
  _rxIndex(0),
  _rxBuffer(),
  _state(ESLOV_AVAILABLE_SENSOR_STATE),
  _debug(NULL),
  _lastDfuPack(false)
{
}

EslovHandler::~EslovHandler()
{
}

bool EslovHandler::begin()
{
  Wire.begin(ESLOV_DEFAULT_ADDRESS);
  eslovActive = true;
  Wire.onReceive(EslovHandler::onReceive); 
  Wire.onRequest(EslovHandler::onRequest);
  return true;
}

void EslovHandler::onReceive(int length)
{
  eslovHandler.receiveEvent(length);
}

void EslovHandler::onRequest()
{
  eslovHandler.requestEvent();
}

void EslovHandler::requestEvent()
{
  if (_debug) {
    _debug->print("Wire Request event. State: ");
    _debug->println(_state);
  }

  if (_state == ESLOV_AVAILABLE_SENSOR_STATE) {
    uint8_t availableData = sensortec.availableSensorData();
    Wire.write(availableData);

  } else if (_state == ESLOV_READ_SENSOR_STATE) {
    SensorDataPacket data;
    sensortec.readSensorData(data);
    Wire.write((uint8_t*)&data, sizeof(SensorDataPacket));
    if (_debug) {
      _debug->print("data: ");
      _debug->println(data.sensorId);
      _debug->println(data.size);
    }

  } else if (_state == ESLOV_SENSOR_ACK_STATE) {
    uint8_t ack = sensortec.acknowledgment();
    if (_debug) {
      _debug->print("Ack: ");
      _debug->println(ack);
    }
    Wire.write(ack);
  }
}

void EslovHandler::end()
{
  eslovActive = false;
  Wire.end();
}

void EslovHandler::receiveEvent(int length)
{
  if (_debug) {
    _debug->println("Wire Receive event.");
  }

  while(Wire.available())
  {
    digitalWrite(p19, LOW);
    _rxBuffer[_rxIndex++] = Wire.read(); 

    // Check if packet is complete depending on its opcode
    if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        dfuManager.processPacket(eslovDFU, DFU_EXTERNAL, &_rxBuffer[1]);

        dump();

        _rxIndex = 0;

        //Release INT pin
        digitalWrite(p19, HIGH);
      }

    } else if (_rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        digitalWrite(p19, LOW);
        dfuManager.processPacket(eslovDFU, DFU_INTERNAL, &_rxBuffer[1]);

        dump();

        _rxIndex = 0;

        //Release INT pin
        digitalWrite(p19, HIGH);
      }

    } else if (_rxBuffer[0] == ESLOV_SENSOR_CONFIG_OPCODE) {
      if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {
        SensorConfigurationPacket *config = (SensorConfigurationPacket*)&_rxBuffer[1];
        sensortec.configureSensor(*config);

        _state = ESLOV_SENSOR_ACK_STATE;

        dump();
        _rxIndex = 0;
      }

    } else if (_rxBuffer[0] == ESLOV_SENSOR_STATE_OPCODE) {
      if (_rxIndex == 2) {
        _state = (EslovState)_rxBuffer[1];

        dump();
        _rxIndex = 0;
      }

    } else {
      // Not valid opcode. Discarding packet
      if (_debug) {
        _debug->println("discard");
      }

      _rxIndex = 0;
    }

    if (_rxIndex == ESLOV_MAX_LENGTH) {
      if (_debug) {
        _debug->println("discard");
      }
      // Packet too long. Discarding it

      _rxIndex = 0;
    }
  }
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
