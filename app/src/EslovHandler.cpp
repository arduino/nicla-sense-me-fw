#include "EslovHandler.h"

#include "DFUChannel.h"
#include "SensorChannel.h"

EslovHandler::EslovHandler() :
  _rxIndex(0),
  _rxBuffer()
{

}

EslovHandler::~EslovHandler()
{
}

void EslovHandler::begin()
{
  Wire.begin(ESLOV_DEFAULT_ADDRESS);                
  Wire.onReceive(wireCallback); 
}

// Ugly and.. does it work?
void EslovHandler::wireCallback(int howMany)
{
  eslovHandler.receiveEvent(howMany);
}

void EslovHandler::receiveEvent(int howMany)
{
  while(Wire.available()) 
  {
    _rxBuffer[_rxIndex++] = Wire.read(); 
    //Serial.println(_rxBuffer[_rxIndex-1]);

    // Check if packet is complete depending on its opcode
    if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        dfuChannel.processPacket(DFU_EXTERNAL, &_rxBuffer[1]);

        _rxIndex = 0;
      }

    } else if (_rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        dfuChannel.processPacket(DFU_INTERNAL, &_rxBuffer[1]);

        _rxIndex = 0;
      }

    } else if (_rxBuffer[0] == ESLOV_SENSOR_REQUEST_OPCODE) {
      uint8_t numAvailableData = sensorChannel.processPacket(SENSOR_REQUEST_PACKET, NULL);
      // return length of available data to ESLOV master

      _rxIndex = 0;

    } else if (_rxBuffer[0] == ESLOV_SENSOR_CONFIG_OPCODE) {
      if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {
        sensorChannel.processPacket(SENSOR_CONFIG_PACKET, &_rxBuffer[1]);

        _rxIndex = 0;
      }

    } else {
      // Not valid opcode. Discarding packet
      _rxIndex = 0;
    }

    if (_rxIndex == ESLOV_MAX_LENGTH) {
      // Packet too long. Discarding it
      _rxIndex = 0;
    }
  }
}

EslovHandler eslovHandler;
