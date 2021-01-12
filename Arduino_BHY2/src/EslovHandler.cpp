#include "EslovHandler.h"

#include "DFUManager.h"
#include "BoschSensortec.h"

EslovHandler::EslovHandler() :
  _rxIndex(0),
  _rxBuffer(),
  _state(ESLOV_AVAILABLE_SENSOR_STATE)
{

}

EslovHandler::~EslovHandler()
{
}

void EslovHandler::begin()
{
  Wire.begin(ESLOV_DEFAULT_ADDRESS);                
  Wire.onReceive(EslovHandler::onReceive); 
  Wire.onRequest(EslovHandler::onRequest); 
}

// Ugly and.. does it work?
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

  } else if (_state == ESLOV_DFU_ACK_STATE) {
    uint8_t ack = dfuManager.acknowledgment();
    if (_debug) {
      _debug->print("Ack: ");
      _debug->println(ack);
    }
    Wire.write(ack);

    _state = ESLOV_AVAILABLE_SENSOR_STATE;
    
  }
}

void EslovHandler::receiveEvent(int length)
{
  if (_debug) {
    _debug->println("Wire Receive event.");
  }

  while(Wire.available())
  {
    _rxBuffer[_rxIndex++] = Wire.read(); 
    //Serial.println(_rxBuffer[_rxIndex-1]);
  }

  if (_debug && _rxIndex > 0) {
    _debug->print("Opcode = ");
    _debug->println(_rxBuffer[0]);
  }

  // Check if packet is complete depending on its opcode
  if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE) {
    if (_rxIndex == sizeof(DFUPacket) + 1) {
      dfuManager.processPacket(DFU_EXTERNAL, &_rxBuffer[1]);

      dump();

      _state = ESLOV_DFU_ACK_STATE;
    }

  } else if (_rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {
    if (_rxIndex == sizeof(DFUPacket) + 1) {
      dfuManager.processPacket(DFU_INTERNAL, &_rxBuffer[1]);

      dump();

      _state = ESLOV_DFU_ACK_STATE;
    }

  } else if (_rxBuffer[0] == ESLOV_SENSOR_CONFIG_OPCODE) {
    if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {
      SensorConfigurationPacket *config = (SensorConfigurationPacket*)&_rxBuffer[1];
      sensortec.configureSensor(config);

      dump();
    }

  } else if (_rxBuffer[0] == ESLOV_SENSOR_STATE_OPCODE) {
    if (_rxIndex == 2) {
      _state = (EslovState)_rxBuffer[1];

      dump();
    }

  } else {
    // Not valid opcode. Discarding packet
    if (_debug) {
      _debug->println("discard");
    }
  }

  if (_rxIndex == ESLOV_MAX_LENGTH) {
    if (_debug) {
      _debug->println("discard");
    }
    // Packet too long. Discarding it
  }

  _rxIndex = 0;

}

void EslovHandler::debug(Stream &stream)
{
  _debug = &stream;
  dfuManager.debug(stream);

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
