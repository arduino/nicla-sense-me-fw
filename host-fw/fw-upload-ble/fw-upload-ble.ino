#include "Arduino.h"
#include "ArduinoBLE.h"

//static uint8_t testFw[sizeof(DFUPacket)+1] = {
  //0x01, 0x00, 0x00, 0x00,
  //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
//};

void updateFw(BLEDevice peripheral);

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  auto ret = BLE.begin();
  Serial.println(ret);

  BLE.scanForName("UNISENSE");
}

void loop()
{
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.print("Found ");

    BLE.stopScan();

    updateFw(peripheral);

    BLE.scanForName("UNISENSE");
  }
}

void updateLoop(BLEDevice peripheral)
{
  while (peripheral.connected()) {




    uint8_t ack;

    if (dfuAckCharacteristic.valueUpdated()) { 
      ack = dfuAckCharacteristic.readValue();
    }
  }
}

void updateFw(BLEDevice peripheral)
{
  // Connect to peripheral
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // Discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic dfuAckCharacteristic = peripheral.characteristic("34c2e3b8-34aa-11eb-adc1-0242ac120002");
  BLECharacteristic dfuIntCharacteristic = peripheral.characteristic("34c2e3b9-34aa-11eb-adc1-0242ac120002");
  if ((!dfuAckCharacteristic) && (!dfuIntCharacteristic)) {
    Serial.println("Peripheral does not have characteristics!");
    peripheral.disconnect();
    return;
  }

  dfuAckCharacteristic.subscribe();

  updateLoop();

}
