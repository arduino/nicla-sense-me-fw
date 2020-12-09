#include <Arduino.h>
#include "src/BoschSensortec/BoschSensortec.h"
#include "src/EslovHandler.h"
#include "src/BLEHandler.h"



#include "src/DFUChannel.h"
#include "src/SensorChannel.h"

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

  // ESLOV setup
  eslovHandler.setup();
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
}
