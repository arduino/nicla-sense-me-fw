#include "EslovHandler.h"

#include "DFUManager.h"
#include "BoschSensortec.h"

EslovHandler::EslovHandler() :
  _rxIndex(0),
  _rxBuffer(),
  _state(ESLOV_AVAILABLE_SENSOR_STATE),
  _debug(NULL),
  _lastDfuPack(false),
  _eslovIntPin(PIN_ESLOV_INT)
{
}

EslovHandler::~EslovHandler()
{
}

bool EslovHandler::begin()
{
  eslovBusy();
  Wire.begin(ESLOV_DEFAULT_ADDRESS);
  eslovActive = true;
  Wire.onReceive(EslovHandler::onReceive); 
  Wire.onRequest(EslovHandler::onRequest);
  eslovAvailable();
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

void EslovHandler::eslovBusy()
{
  //Set Eslov INT pin
  digitalWrite(_eslovIntPin, LOW);
}

void EslovHandler::eslovAvailable()
{
  //Release Eslov INT pin
  digitalWrite(_eslovIntPin, HIGH);
}

void EslovHandler::requestEvent()
{
  eslovBusy();

  if (_debug) {
    _debug->print("Wire Request event. State: ");
    if (_state == ESLOV_AVAILABLE_SENSOR_STATE) {
      _debug->println("ESLOV_AVAILABLE_SENSOR_STATE");
    }
    else if (_state == ESLOV_READ_SENSOR_STATE) {
      _debug->println("ESLOV_READ_SENSOR_STATE");
    }
    else if (_state == ESLOV_SENSOR_ACK_STATE) {
      _debug->println("ESLOV_SENSOR_ACK_STATE");
    }
    else {
      _debug->println(_state);
    }
  }

  if (_state == ESLOV_AVAILABLE_SENSOR_STATE) {
    uint8_t availableData = sensortec.availableSensorData();
    Wire.write(availableData);

  } else if (_state == ESLOV_AVAILABLE_LONG_SENSOR_STATE) {
    uint8_t availableData = sensortec.availableLongSensorData();
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

  } else if (_state == ESLOV_READ_LONG_SENSOR_STATE) {
    SensorLongDataPacket data;
    sensortec.readLongSensorData(data);

    Wire.write((uint8_t*)&data, sizeof(SensorLongDataPacket));
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

  eslovAvailable();
}

void EslovHandler::end()
{
  eslovActive = false;
  Wire.end();
}

void EslovHandler::niclaAsShield()
{
  _eslovIntPin = I2C_INT_PIN;
}

void EslovHandler::receiveEvent(int length)
{
  if (_debug) {
    _debug->println("Wire Receive event.");
  }

  while(Wire.available())
  {
    eslovBusy();
    _rxBuffer[_rxIndex++] = Wire.read(); 

    // Check if packet is complete depending on its opcode
    if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        if (_debug) {
          _debug->print("Opcode: ");
          _debug->println("ESLOV_DFU_EXTERNAL_OPCODE");
        }
        dfuManager.processPacket(eslovDFU, DFU_EXTERNAL, &_rxBuffer[1]);

        dump();

        _rxIndex = 0;

        //Last packet
        if (_rxBuffer[1]) {
          dfuManager.closeDfu();
        }

        eslovAvailable();
      }

    } else if (_rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        if (_debug) {
          _debug->print("Opcode: ");
          _debug->println("ESLOV_DFU_INTERNAL_OPCODE");
        }
        dfuManager.processPacket(eslovDFU, DFU_INTERNAL, &_rxBuffer[1]);

        dump();

        _rxIndex = 0;

        //Last packet
        if (_rxBuffer[1]) {
          dfuManager.closeDfu();
        }

        eslovAvailable();
      }

    } else if (_rxBuffer[0] == ESLOV_SENSOR_CONFIG_OPCODE) {
      if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {
        if (_debug) {
          _debug->print("Opcode: ");
          _debug->println("ESLOV_SENSOR_CONFIG_OPCODE");
        }
        SensorConfigurationPacket *config = (SensorConfigurationPacket*)&_rxBuffer[1];
        sensortec.configureSensor(*config);

        _state = ESLOV_SENSOR_ACK_STATE;

        dump();
        _rxIndex = 0;

        eslovAvailable();
      }

    } else if (_rxBuffer[0] == ESLOV_SENSOR_STATE_OPCODE) {
      if (_rxIndex == 2) {
        _state = (EslovState)_rxBuffer[1];
        if (_debug) {
          _debug->print("Opcode: ");
          _debug->println("ESLOV_SENSOR_STATE_OPCODE");
        }

        dump();
        _rxIndex = 0;
      }

      eslovAvailable();

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
    _debug->print("received:   ");
    _debug->print("size: ");
    _debug->println(_rxIndex);
    _debug->print("   packet: ");
    for (int i = 0; i < _rxIndex; i++) {
      _debug->print(_rxBuffer[i], HEX);
      _debug->print(", ");
    }
    _debug->println();
  }
}

EslovHandler eslovHandler;
