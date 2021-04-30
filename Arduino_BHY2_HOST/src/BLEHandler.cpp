#if 0
#include "BLEHandler.h"

// Sensor Data channels
auto sensorServiceUuid = "34c2e3bb-34aa-11eb-adc1-0242ac120002";
BLEService sensorService; 
auto sensorDataUuid = "34c2e3bc-34aa-11eb-adc1-0242ac120002";
auto sensorConfigUuid = "34c2e3bd-34aa-11eb-adc1-0242ac120002";
BLECharacteristic sensorDataCharacteristic;
BLECharacteristic sensorConfigCharacteristic;

Stream* BLEHandler::_debug = NULL;

BLEHandler::BLEHandler()
{
}

BLEHandler::~BLEHandler()
{
}

void BLEHandler::begin()
{
  BLE.begin();

  BLE.scanForName("NICLA");
}

void BLEHandler::update()
{
  BLE.poll();

}

void BLEHandler::debug(Stream &stream)
{
  _debug = &stream;
  BLE.debug(stream);
}

BLEHandler bleHandler;

#endif
