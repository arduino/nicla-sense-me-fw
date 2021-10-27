#include "Arduino_BHY2Host.h"

#ifdef __BHY2_HOST_BLE_SUPPORTED__

#include "BLEHandler.h"
#include "sensors/SensorManager.h"

auto sensorServiceUuid = '34c2e3bb-34aa-11eb-adc1-0242ac120002';
auto sensorDataUuid = "34c2e3bc-34aa-11eb-adc1-0242ac120002";
auto sensorConfigUuid = "34c2e3bd-34aa-11eb-adc1-0242ac120002";

BLECharacteristic configCharacteristic;
BLECharacteristic dataCharacteristic;
BLEDevice niclaPeripheral;

BLEHandler::BLEHandler() :
  _debug(NULL)
{
}

BLEHandler::~BLEHandler()
{
}

bool BLEHandler::begin()
{
  if (_debug) {
    _debug->print("Starting BLE...");
  }

  if (!BLE.begin()) {
    if (_debug) {
      _debug->println("ERROR!");
    }
    return false;
  }
  if (_debug) {
    _debug->println("OK!");
  }

  return connectToNicla();
}

bool BLEHandler::connectToNicla()
{
  // start scanning for peripheral
  BLE.scan();

  bool nicla_found = false;

  while(!nicla_found) {
    BLEDevice peripheral = BLE.available();

    if (peripheral.hasLocalName() && peripheral.localName() == "NICLA") {
      if (_debug) {
        _debug->println("Nicla peripheral found!");
      }
      // stop scanning
      BLE.stopScan();
      // Connect to peripheral
      if (!peripheral.connect()) {
        if (_debug) {
          _debug->println("Unable to connect to peripheral.");
        }
        end();
        return false;
      }
      if (_debug) {
        _debug->println("Successfully connected to peripheral.");
      }

      uint8_t retry = 0;
      // discover peripheral attributes
      while (!peripheral.discoverService("34c2e3bb-34aa-11eb-adc1-0242ac120002")) {
        if (_debug) {
          _debug->println("Unable to discover sensor service.");
        }
        if (retry++ > 3) {
          end();
          return false;
        }
      }
      if (_debug) {
        _debug->println("Sensor service discovered.");
      }

      // retrieve characteristics
      configCharacteristic = peripheral.characteristic(sensorConfigUuid);
      dataCharacteristic = peripheral.characteristic(sensorDataUuid);

      if (!configCharacteristic) {
        end();
        return false;
      } else if (!configCharacteristic.canWrite()) {
        end();
        return false;
      }
      if (_debug) {
        _debug->println("configCharacteristic found");
      }
      // subscribe to data characteristic
      if (!dataCharacteristic) {
        end();
        return false;
      } else if (!dataCharacteristic.canSubscribe()) {
        end();
        return false;
      } else if (!dataCharacteristic.subscribe()) {
        end();
        return false;
      }
      dataCharacteristic.setEventHandler(BLEWritten, receivedSensorData);
      if (_debug) {
        _debug->println("Subscribed to dataCharacteristic");
      }

      niclaPeripheral = peripheral;
      nicla_found = true;
    }
  }

  return true;
}

void BLEHandler::update() 
{
  BLE.poll();

  if (!niclaPeripheral.connected()) {
    if (_debug) {
      _debug->println("Peripheral disconnected");
    }
    begin();
  }

  //if (niclaPeripheral.connected()) {
    //if (dataCharacteristic.valueUpdated()) {
      /*
      uint8_t data[sizeof(SensorDataPacket)];
      dataCharacteristic.readValue(data, sizeof(SensorDataPacket));

      if (_debug) {
        _debug->print("Data packet received: ");
        for (int i=0; i<sizeof(SensorDataPacket); i++) {
          _debug->print(data[i], HEX);
          _debug->print(", ");
        }
        _debug->println();
      }

      SensorDataPacket sensorData;
      memcpy(&sensorData, &data, sizeof(SensorDataPacket));

      sensorManager.process(sensorData);
      */
    //}
    /*
  } else {
    if (_debug) {
      _debug->println("Peripheral disconnected");
    }
  }
  */
}

void BLEHandler::end()
{
  niclaPeripheral.disconnect();
  BLE.end();
}

void BLEHandler::writeConfigPacket(SensorConfigurationPacket& config)
{
  uint8_t packet[sizeof(SensorConfigurationPacket)]; 
  memcpy(&packet[0], &config, sizeof(SensorConfigurationPacket));
  if (_debug) {
    _debug->print("Config packet: ");
    for (int i=0; i<sizeof(SensorConfigurationPacket); i++) {
      _debug->print(packet[i], HEX);
      _debug->print(", ");
    }
    _debug->println();
  }
  configCharacteristic.writeValue(packet, sizeof(SensorConfigurationPacket));
}

void BLEHandler::receivedSensorData(BLEDevice central, BLECharacteristic characteristic)
{
  SensorDataPacket sensorData;
  characteristic.readValue(&sensorData, sizeof(sensorData));
  sensorManager.process(sensorData);
}

void BLEHandler::debug(Stream &stream)
{
  _debug = &stream;
}

BLEHandler bleHandler;

#endif //__BHY2_HOST_BLE_SUPPORTED__