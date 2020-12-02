/*
  DFU example

  This sketch exposes a DFU characteristic which can be used to write a new firmware.
  Upload utility [here]


  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include "FlashIAPBlockDevice.h"
#include "LittleFileSystem.h"

// half the flash (512KB) is dedicated as dfu temporary storage
static FlashIAPBlockDevice bd(0x80000, 0x80000);
static mbed::LittleFileSystem fs("fs");

struct __attribute__((packed)) configurationPacket {
  uint8_t header;
  uint8_t len;
  uint8_t opcode;
  uint8_t* data;
};

struct __attribute__((packed)) configPacket {
  uint8_t operation;
  uint8_t data[64];
};

struct __attribute__((packed)) dfuPacket {
  uint8_t last: 1;
  union {
    uint16_t index: 15;
    uint16_t remaining: 15;
  };
  uint8_t data[64];
};

BLEService configService("34c2e3b8-34aa-11eb-adc1-0242ac120002"); // BLE LED Service

BLEStringCharacteristic configCharacteristic("34c2e3b8-34aa-11eb-adc1-0242ac120002", BLEWrite, 64 + sizeof(configurationPacket) + sizeof(dfuPacket));

void setup() {
  Serial.begin(9600);
  while (!Serial);

  int err = fs.mount(&bd);
  if (err) {
    err = fs.reformat(&bd);
  }

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("UNISENSE-DFU");
  BLE.setAdvertisedService(configService);

  // add the characteristic to the service
  configService.addCharacteristic(configCharacteristic);

  // add service
  BLE.addService(configService);

  // start advertising
  BLE.advertise();

  Serial.println("BLE DFU Test");
}

#define UNISENSE_CONFIG_HEADER  0xFC
#define UNISENSE_APP_CONFIG     0x01
#define UNISENSE_DFU_INTERNAL   0x02
#define UNISENSE_DFU_SENSOR     0x03

void configureApp(struct configPacket* config) {}

static FILE* target = NULL;
void writeDfuChunk(int where, struct dfuPacket* config) {
  if (config->index == 0) {
    if (where == UNISENSE_DFU_INTERNAL) {
      target = fopen("/fs/UPDATE.BIN", "wb");
    } else {
      target = fopen("/fs/BH104.BIN", "wb");
    }
  }
  if (target != NULL) {
      fwrite(config->data, config->last ? config->remaining : sizeof(config->data), 1, target);
  }
  if (config->last) {
    fclose(target);
    target = NULL;
    if (where == UNISENSE_DFU_INTERNAL) {
      // reboot
    } else {
      //apply bosch update
    }
  }
}

void parseConfig(struct configurationPacket * config) {
  if (config->header != UNISENSE_CONFIG_HEADER) return;
  switch (config->opcode) {
    case UNISENSE_APP_CONFIG:
      configureApp((struct configPacket*)config->data);
      break;
    case UNISENSE_DFU_INTERNAL:
      writeDfuChunk(UNISENSE_DFU_INTERNAL, (struct dfuPacket*)config->data);
      break;
    case UNISENSE_DFU_SENSOR:
      writeDfuChunk(UNISENSE_DFU_SENSOR, (struct dfuPacket*)config->data);
      break;
  }
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (configCharacteristic.written()) {
        String data = configCharacteristic.value();
        struct configurationPacket* config = (struct configurationPacket*)data.c_str();
        parseConfig(config);
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
