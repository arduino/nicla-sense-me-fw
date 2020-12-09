#include <Arduino.h>
#include "Wire.h"
#include "ArduinoBLE.h"
#include "src/BoschSensortec/BoschSensortec.h"
#include "src/SensorChannel.h"
#include "src/DFUChannel.h"

// ESLOV config
#define ESLOV_MAX_LENGTH      255
#define ESLOV_DEFAULT_ADDRESS 0x55

enum EslovOpcode {
  ESLOV_DFU_INTERNAL_OPCODE,
  ESLOV_DFU_EXTERNAL_OPCODE,
  ESLOV_SENSOR_DATA_OPCODE,
  ESLOV_SENSOR_CONFIG_OPCODE,
  ESLOV_SENSOR_REQUEST_OPCODE
};

int _rxIndex = 0;
uint8_t _rxBuffer[ESLOV_MAX_LENGTH] = {0};
bool _packetComplete = false;
///////

// BLE config

// DFU channels
BLEService dfuService("34c2e3b8-34aa-11eb-adc1-0242ac120002"); 
auto dfuInternalUuid = "34c2e3b9-34aa-11eb-adc1-0242ac120002";
auto dfuExternalUuid = "34c2e3ba-34aa-11eb-adc1-0242ac120002";
BLECharacteristic dfuInternalCharacteristic(dfuInternalUuid, BLEWrite, sizeof(DFUPacket));
BLECharacteristic dfuExternalCharacteristic(dfuExternalUuid, BLEWrite, sizeof(DFUPacket));

// Sensor Data channels
BLEService sensorService("34c2e3bb-34aa-11eb-adc1-0242ac120002"); 
auto sensorDataUuid = "34c2e3bc-34aa-11eb-adc1-0242ac120002";
auto sensorConfigUuid = "34c2e3bd-34aa-11eb-adc1-0242ac120002";
BLECharacteristic sensorDataCharacteristic(sensorDataUuid, (BLERead | BLENotify), sizeof(SensorDataPacket));
BLECharacteristic sensorConfigCharacteristic(sensorConfigUuid, BLEWrite, sizeof(SensorConfigurationPacket));
////////

// BLE receiver

// DFU channel
void processDFUPacket(DFUType dfuType, BLECharacteristic characteristic) 
{
  uint8_t data[sizeof(DFUPacket)];
  characteristic.readValue(data, sizeof(data));
  DFUChannel.processPacket(dfuType, data);
}

void receivedInternalDFU(BLEDevice central, BLECharacteristic characteristic)
{
  processDFUPacket(DFU_INTERNAL, characteristic);
}

void receivedExternalDFU(BLEDevice central, BLECharacteristic characteristic)
{
  processDFUPacket(DFU_EXTERNAL, characteristic);
}

// Sensor channel
void receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic)
{
  uint8_t data[sizeof(SensorConfigurationPacket)];
  characteristic.readValue(data, sizeof(data));
  SensorChannel.processPacket(SENSOR_CONFIG_PACKET, data);
}

/////////////

// Eslov receiver
void receiveEvent(int howMany)
{
  while(Wire.available()) 
  {
    _rxBuffer[_rxIndex++] = Wire.read(); 
    //Serial.println(_rxBuffer[_rxIndex-1]);

    // Check if packet is complete depending on its opcode
    if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        DFUChannel.processPacket(DFU_EXTERNAL, &_rxBuffer[1]);

        _rxIndex = 0;
      }

    } else if (_rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {
        DFUChannel.processPacket(DFU_INTERNAL, &_rxBuffer[1]);

        _rxIndex = 0;
      }

    } else if (_rxBuffer[0] == ESLOV_SENSOR_REQUEST_OPCODE) {
      uint8_t numAvailableData = SensorChannel.processPacket(SENSOR_REQUEST_PACKET, NULL);
      // return length of available data to ESLOV master

      _rxIndex = 0;

    } else if (_rxBuffer[0] == ESLOV_SENSOR_CONFIG_OPCODE) {
      if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {
        SensorChannel.processPacket(SENSOR_CONFIG_PACKET, &_rxBuffer[1]);

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
////////

BoschSensortec sensortec;

// Initialize bosch sensortec
// Initialize and link channels 
void setup()
{
  sensortec.begin();

  // debug port
  Serial.begin(9600);           
  while(!Serial);

  // ESLOV receiver setup
  Wire.begin(ESLOV_DEFAULT_ADDRESS);                
  Wire.onReceive(receiveEvent); 
  ///////////

  // BLE receiver setup
  BLE.begin();
  BLE.setLocalName("UNISENSE");

  // DFU channel
  BLE.setAdvertisedService(dfuService);
  dfuService.addCharacteristic(dfuInternalCharacteristic);
  dfuService.addCharacteristic(dfuExternalCharacteristic);
  BLE.addService(dfuService);
  dfuInternalCharacteristic.setEventHandler(BLEWritten, receivedInternalDFU);
  dfuExternalCharacteristic.setEventHandler(BLEWritten, receivedExternalDFU);

  // Sensor channel
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(sensorConfigCharacteristic);
  BLE.addService(sensorService);
  sensorConfigCharacteristic.setEventHandler(BLEWritten, receivedInternalDFU);

  //
  BLE.advertise();
  ///////
}

void loop()
{
  BLE.poll();
  if (_packetComplete) {
    Serial.println("Packet received");
  }
}
