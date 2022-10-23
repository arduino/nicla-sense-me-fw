#include "BLEHandler.h"

#include "sensors/SensorID.h"

#include "BoschSensortec.h"

// DFU channels
BLEService dfuService("34c2e3b8-34aa-11eb-adc1-0242ac120002");
auto dfuInternalUuid = "34c2e3b9-34aa-11eb-adc1-0242ac120002";
auto dfuExternalUuid = "34c2e3ba-34aa-11eb-adc1-0242ac120002";
BLECharacteristic dfuInternalCharacteristic(dfuInternalUuid, BLEWrite, sizeof(DFUPacket), true);
BLECharacteristic dfuExternalCharacteristic(dfuExternalUuid, BLEWrite, sizeof(DFUPacket), true);

// Sensor Data channels
BLEService sensorService("34c2e3bb-34aa-11eb-adc1-0242ac120002");
auto sensorDataUuid = "34c2e3bc-34aa-11eb-adc1-0242ac120002";
auto sensorLongDataUuid = "34c2e3be-34aa-11eb-adc1-0242ac120002";
auto sensorConfigUuid = "34c2e3bd-34aa-11eb-adc1-0242ac120002";
#if BHY2_ENABLE_BLE_BATCH
const uint8_t CHARACTERISTIC_SIZE_SENSOR_DATA_MAX = BLE_SENSOR_EVT_BATCH_CNT_MAX * sizeof(SensorDataPacket);
uint8_t BLEHandler::_idxBatch = 0;
SensorDataPacket * BLEHandler::_dataBatch = NULL;
BLECharacteristic sensorDataCharacteristic(sensorDataUuid, (BLERead | BLENotify), CHARACTERISTIC_SIZE_SENSOR_DATA_MAX);
#else
BLECharacteristic sensorDataCharacteristic(sensorDataUuid, (BLERead | BLENotify), sizeof(SensorDataPacket));
#endif
BLECharacteristic sensorLongDataCharacteristic(sensorLongDataUuid, (BLERead | BLENotify), sizeof(SensorLongDataPacket));
BLECharacteristic sensorConfigCharacteristic(sensorConfigUuid, BLEWrite, sizeof(SensorConfigurationPacket));

Stream* BLEHandler::_debug = NULL;

BLEHandler::BLEHandler() : _lastDfuPack(false)
{
}

BLEHandler::~BLEHandler()
{
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
  dfuManager.processPacket(bleDFU, dfuType, data);

  if (data[0]) {
    //Last packet
    _lastDfuPack = true;
    dfuManager.closeDfu();
  }
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

#if BHY2_ENABLE_BLE_BATCH
  uint16_t batch;
  bool longSensor = false;

  for (int i = 0; i < NUM_LONG_SENSOR; i++) {
      if (LongSensorList[i].id == data.sensorId) {
          longSensor = true;
          break;
      }
  }

  if (!longSensor) {
      if (data.latency > 0) {
          batch = ((uint16_t)((data.latency * data.sampleRate) / 1000)) + 1;
          //to simplify things:
          //if any sensor requires batching, enable batching for all
          if (batch > 1) {
              if (NULL == _dataBatch) {
                  _dataBatch = new SensorDataPacket[BLE_SENSOR_EVT_BATCH_CNT_MAX];
                  if (_debug) {
                      _debug->print("batch buffer allocated:");
                      _debug->println((long)_dataBatch, HEX);
                  }
                  printf("batch buffer:%p\n", _dataBatch);
              }
          }
      }
  }
#endif

  sensortec.configureSensor(data);
}

bool BLEHandler::begin()
{
  if (!BLE.begin()) {
    return false;
  }
  bleActive = true;
  BLE.setLocalName("NICLA");

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
  sensorService.addCharacteristic(sensorDataCharacteristic);
  sensorService.addCharacteristic(sensorLongDataCharacteristic);
  BLE.addService(sensorService);
  sensorConfigCharacteristic.setEventHandler(BLEWritten, receivedSensorConfig);

  //
  BLE.advertise();
  return true;
}

void BLEHandler::update()
{
  BLE.poll();

  // This check doesn't work with more than one client at the same time
  if (sensorDataCharacteristic.subscribed()) {

    // Simulate a request for reading new sensor data
    uint8_t availableData = sensortec.availableSensorData();
    while (availableData) {
#if BHY2_ENABLE_BLE_BATCH
        if (NULL != _dataBatch) {
            sensortec.readSensorData(_dataBatch[_idxBatch]);
            //we use this byte (unused anyway) as an sequence number
            _dataBatch[_idxBatch].data[SENSOR_DATA_FIXED_LENGTH-1] = _idxBatch;
            _idxBatch++;
            if (BLE_SENSOR_EVT_BATCH_CNT_MAX == _idxBatch) {
                sensorDataCharacteristic.writeValue(_dataBatch, CHARACTERISTIC_SIZE_SENSOR_DATA_MAX);
                _idxBatch = 0;
            }
            --availableData;
        } else {
            //since the buffer is not available, we just send the data directly
            SensorDataPacket data;
            sensortec.readSensorData(data);
            sensorDataCharacteristic.writeValue(&data, sizeof(SensorDataPacket));
            --availableData;
        }
#else
        SensorDataPacket data;
        sensortec.readSensorData(data);
        sensorDataCharacteristic.writeValue(&data, sizeof(SensorDataPacket));
        --availableData;
#endif
    }

  } else {
#if BHY2_ENABLE_BLE_BATCH
      if (NULL != _dataBatch) {
          delete[] _dataBatch;
          _dataBatch = NULL;
          if (_debug) {
              _debug->println("batch buffer released");
          }
          printf("batch buffer released\n");
      }
#endif
  }

  if (sensorLongDataCharacteristic.subscribed()) {

    uint8_t availableLongData = sensortec.availableLongSensorData();
    while (availableLongData) {
      SensorLongDataPacket data;
      sensortec.readLongSensorData(data);
      sensorLongDataCharacteristic.writeValue(&data, sizeof(SensorLongDataPacket));
      --availableLongData;
    }

  }

}

void BLEHandler::poll(unsigned long timeout)
{
  BLE.poll(timeout);
}


void BLEHandler::end()
{
  bleActive = false;
  BLE.end();
}

void BLEHandler::debug(Stream &stream)
{
  _debug = &stream;
  BLE.debug(stream);
}

BLEHandler bleHandler;
