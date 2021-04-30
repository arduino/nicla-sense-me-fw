#include "BLEHandler.h"

#include "BoschSensortec.h"

// DFU channels
BLEService dfuService("34c2e3b8-34aa-11eb-adc1-0242ac120002");
auto dfuAckUuid = "34c2e3be-34aa-11eb-adc1-0242ac120002";
auto dfuInternalUuid = "34c2e3b9-34aa-11eb-adc1-0242ac120002";
auto dfuExternalUuid = "34c2e3ba-34aa-11eb-adc1-0242ac120002";
BLECharacteristic dfuAckCharacteristic(dfuAckUuid, (BLERead | BLENotify), 1);
BLECharacteristic dfuInternalCharacteristic(dfuInternalUuid, BLEWrite, sizeof(DFUPacket), true);
BLECharacteristic dfuExternalCharacteristic(dfuExternalUuid, BLEWrite, sizeof(DFUPacket), true);

// Sensor Data channels
BLEService sensorService("34c2e3bb-34aa-11eb-adc1-0242ac120002"); 
auto sensorDataUuid = "34c2e3bc-34aa-11eb-adc1-0242ac120002";
auto sensorConfigUuid = "34c2e3bd-34aa-11eb-adc1-0242ac120002";
BLECharacteristic sensorDataCharacteristic(sensorDataUuid, (BLERead | BLENotify), sizeof(SensorDataPacket));
BLECharacteristic sensorConfigCharacteristic(sensorConfigUuid, BLEWrite, sizeof(SensorConfigurationPacket));

Stream* BLEHandler::_debug = NULL;

BLEHandler::BLEHandler()
{
}

BLEHandler::~BLEHandler()
{
}

void BLEHandler::writeDFUAcknowledgment()
{
  uint8_t ack = dfuManager.acknowledgment();
  dfuAckCharacteristic.writeValue(ack);
}

// DFU channel
void BLEHandler::processDFUPacket(DFUType dfuType, BLECharacteristic characteristic) 
{
  uint8_t data[sizeof(DFUPacket)];
  characteristic.readValue(data, sizeof(data));
  if (_debug) {
    _debug->print("Size of data: ");
    _debug->println(sizeof(data));
  }
  dfuManager.processPacket(dfuType, data);

  writeDFUAcknowledgment();
}

void BLEHandler::receivedInternalDFU(BLEDevice central, BLECharacteristic characteristic)
{
  if (_debug) {
    _debug->println("receivedInternalDFU");
  }
  bleHandler.processDFUPacket(DFU_INTERNAL, characteristic);
}

void BLEHandler::receivedExternalDFU(BLEDevice central, BLECharacteristic characteristic)
{
  bleHandler.processDFUPacket(DFU_EXTERNAL, characteristic);
}

// Sensor channel
void BLEHandler::receivedSensorConfig(BLEDevice central, BLECharacteristic characteristic)
{
  SensorConfigurationPacket data;
  characteristic.readValue(&data, sizeof(data));
  if (_debug) {
    _debug->println("configuration received: ");
    _debug->print("data: ");
    _debug->println(data.sensorId);
    _debug->println(data.sampleRate);
    _debug->println(data.latency);
  }
  sensortec.configureSensor(data);
}

void BLEHandler::begin()
{
  BLE.begin();
  BLE.setLocalName("NICLA");

  // DFU channel
  BLE.setAdvertisedService(dfuService);
  dfuService.addCharacteristic(dfuInternalCharacteristic);
  dfuService.addCharacteristic(dfuExternalCharacteristic);
  dfuService.addCharacteristic(dfuAckCharacteristic);
  BLE.addService(dfuService);
  dfuInternalCharacteristic.setEventHandler(BLEWritten, receivedInternalDFU);
  dfuExternalCharacteristic.setEventHandler(BLEWritten, receivedExternalDFU);

  // Sensor channel
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(sensorConfigCharacteristic);
  sensorService.addCharacteristic(sensorDataCharacteristic);
  BLE.addService(sensorService);
  sensorConfigCharacteristic.setEventHandler(BLEWritten, receivedSensorConfig);

  //
  BLE.advertise();
}

void BLEHandler::update()
{
  BLE.poll();

  // This check doesn't work with more than one client at the same time
  if (sensorDataCharacteristic.subscribed()) {

    // Simulate a request for reading new sensor data
    uint8_t availableData = sensortec.availableSensorData();
    while (availableData) {
      SensorDataPacket data;
      sensortec.readSensorData(data);
      sensorDataCharacteristic.writeValue(&data, sizeof(SensorDataPacket));
      --availableData;
    }

  }

}

void BLEHandler::debug(Stream &stream)
{
  _debug = &stream;
  BLE.debug(stream);
}

BLEHandler bleHandler;
