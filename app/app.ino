#include <Arduino.h>
#include "Wire.h"
#include "ArduinoBLE.h"
#include "src/BoschSensortec/BoschSensortec.h"
#include "src/DataChannel.h"
#include "src/DFUChannel.h"

// ESLOV part
#define ESLOV_MAX_LENGTH (255)
#define ESLOV_ADDRESS
int _rxIndex = 0;
uint8_t _rxBuffer[ESLOV_MAX_LENGTH] = {0};
bool _packetComplete = false;
///////

// BLE part
BLEService dfuService("34c2e3b8-34aa-11eb-adc1-0242ac120002"); 
auto dfuInternalUuid = "34c2e3b9-34aa-11eb-adc1-0242ac120002";
auto dfuExternalUuid = "34c2e3ba-34aa-11eb-adc1-0242ac120002";
BLECharacteristic dfuInternalCharacteristic(dfuInternalUuid, BLEWrite, sizeof(DFUPacket));
BLECharacteristic dfuExternalCharacteristic(dfuExternalUuid, BLEWrite, sizeof(DFUPacket));
/////////////

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

void receiveEvent(int howMany)
{
  while(Wire.available()) 
  {
    _rxBuffer[_rxIndex++] = Wire.read(); 
    Serial.println(_rxBuffer[_rxIndex-1]);
  }
}

BoschSensortec sensortec;

// Initialize bosch sensortec
// Initialize and link channels 
void setup()
{
  sensortec.begin();

  // debug port
  Serial.begin(9600);           
  while(!Serial);

  // ESLOV part
  Wire.begin(ESLOV_ADDRESS);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  ///////////

  // BLE part
  BLE.begin();
  BLE.setLocalName("UNISENSE");
  BLE.setAdvertisedService(dfuService);
  dfuService.addCharacteristic(dfuInternalCharacteristic);
  dfuService.addCharacteristic(dfuExternalCharacteristic);
  BLE.addService(dfuService);
  BLE.advertise();
  dfuInternalCharacteristic.setEventHandler(BLEWritten, receivedInternalDFU);
  dfuExternalCharacteristic.setEventHandler(BLEWritten, receivedExternalDFU);
  ///////
}

void loop()
{
  BLE.poll();
  if (_packetComplete) {
    Serial.println("Packet received");
  }
}
