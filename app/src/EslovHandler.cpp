#include "EslovHandler.h"

#include "DFUChannel.h"
#include "BoschSensortec/BoschSensortec.h"

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

  if (_state == ESLOV_AVAILABLE_SENSOR_STATE) {
    uint8_t availableData = sensortec.availableSensorData();
    Wire.write(availableData);

  } else if (_state == ESLOV_READ_SENSOR_STATE) {
    SensorDataPacket data;
    sensortec.readSensorData(data);
    Wire.write((uint8_t*)&data, sizeof(SensorDataPacket));
  }
}

void EslovHandler::receiveEvent(int length)
{
  while(Wire.available()) 
  {
    _rxBuffer[_rxIndex++] = Wire.read(); 
    //Serial.println(_rxBuffer[_rxIndex-1]);

    // Check if packet is complete depending on its opcode
    if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        dfuChannel.processPacket(DFU_EXTERNAL, &_rxBuffer[1]);

        dump();
        _rxIndex = 0;
      }

    } else if (_rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        dfuChannel.processPacket(DFU_INTERNAL, &_rxBuffer[1]);

        dump();
        _rxIndex = 0;
      }

    } else if (_rxBuffer[0] == ESLOV_SENSOR_CONFIG_OPCODE) {
      if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {
        SensorConfigurationPacket *config = (SensorConfigurationPacket*)&_rxBuffer[1];
        sensortec.configureSensor(config);

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
      Serial.println("discard");
      _rxIndex = 0;
    }

    if (_rxIndex == ESLOV_MAX_LENGTH) {
      Serial.println("discard");
      // Packet too long. Discarding it
      _rxIndex = 0;
    }
  }
}

void EslovHandler::dump() 
{
  Serial.print("received: ");
  for (int i = 0; i < _rxIndex; i++) {
    Serial.print(_rxBuffer[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
}

EslovHandler eslovHandler;
