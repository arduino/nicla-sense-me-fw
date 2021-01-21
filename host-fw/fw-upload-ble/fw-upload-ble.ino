#include "Arduino.h"
#include "ArduinoBLE.h"

#define DEBUG true

#define ESLOV_DFU_CHUNK_SIZE (64)

int _rxIndex;
uint8_t _rxBuffer[255];

bool last = false;
uint8_t ack = 0;

enum EslovOpcode {
  ESLOV_DFU_INTERNAL_OPCODE,
  ESLOV_DFU_EXTERNAL_OPCODE,
  ESLOV_SENSOR_CONFIG_OPCODE,
  ESLOV_SENSOR_STATE_OPCODE
};

struct __attribute__((packed)) DFUPacket {
  uint8_t last: 1;
  union {
    uint16_t index: 15;
    uint16_t remaining: 15;
  };
  uint8_t data[ESLOV_DFU_CHUNK_SIZE];
};

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

//void updateFw(BLEDevice peripheral);

//void updateLoop(BLEDevice peripheral);

BLECharacteristic dfuAckCharacteristic;
BLECharacteristic dfuIntCharacteristic;
BLECharacteristic dfuExtCharacteristic;

void setup()
{
  Serial.begin(115200);

#if (DEBUG)
  Serial1.begin(115200);
  Serial1.println("FW UPLOAD BLE SKETCH");
#endif

  auto ret = BLE.begin();
#if (DEBUG)
  Serial1.println(ret);
#endif

  BLE.scanForName("UNISENSE");
}

void loop()
{
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
#if (DEBUG)
    Serial1.println("Found ");
#endif

    BLE.stopScan();

    // Connect to peripheral
    if (peripheral.connect()) {
#if (DEBUG)
    Serial1.println("Connected");
#endif
    } else {
#if (DEBUG)
    Serial1.println("Failed to connect!");
#endif
    }

    // Discover peripheral attributes
    if (peripheral.discoverAttributes()) {
#if (DEBUG)
      Serial1.println("Attributes discovered");
#endif
    } else {
#if (DEBUG)
      Serial1.println("Attribute discovery failed!");
#endif
      peripheral.disconnect();
    }

    dfuAckCharacteristic = peripheral.characteristic("34c2e3b8-34aa-11eb-adc1-0242ac120002");
    dfuIntCharacteristic = peripheral.characteristic("34c2e3b9-34aa-11eb-adc1-0242ac120002");
    dfuExtCharacteristic = peripheral.characteristic("34c2e3ba-34aa-11eb-adc1-0242ac120002");
    if ((!dfuAckCharacteristic) && (!dfuIntCharacteristic)) {
#if (DEBUG)
      Serial1.println("Peripheral does not have characteristics!");
#endif
      peripheral.disconnect();
      return;
    }

    dfuAckCharacteristic.subscribe();

    updateFw(peripheral);

    BLE.scanForName("UNISENSE");
  }
}

void dumpPacket(uint8_t ack)
{ // dump rx buffer
  Serial1.write("Received: ");
  for (int n = 0; n < _rxIndex; n++) {
    Serial1.write(_rxBuffer[n]);
    Serial1.write(", ");
  }
  Serial1.println();
}

uint8_t getDFUPack()
{
  while (Serial.available()) {
    _rxBuffer[_rxIndex++] = Serial.read();
    if (_rxIndex == sizeof(DFUPacket) + 1) {
      uint8_t opcode = _rxBuffer[0];
      if (_rxBuffer[1] == 1) {
        last = true;
      }
      return opcode;
    }
  }
}


void updateFw(BLEDevice peripheral)
{
  while (1) {
    //read from Serial
    uint8_t oc = getDFUPack();

    while (!peripheral.connected());

#if DEBUG
    Serial1.println("Peripheral connected!");
#endif

    //Send packet with BLE
    uint8_t bytes_written = 0;
    //OPCODE must not be sent
    if (oc == ESLOV_DFU_INTERNAL_OPCODE) {
      bytes_written = dfuIntCharacteristic.writeValue(_rxBuffer + 1, sizeof(DFUPacket));
    } else if (oc == ESLOV_DFU_EXTERNAL_OPCODE) {
      bytes_written = dfuExtCharacteristic.writeValue(_rxBuffer + 1, sizeof(DFUPacket));
    }

#if (DEBUG)
    Serial1.print(bytes_written);
    Serial1.println("bytes written via BLE");
    dumpPacket(ack);
#endif

    //Request ack
    if (dfuAckCharacteristic.valueUpdated()) { //o while?
      if (dfuAckCharacteristic.readValue(&ack, 1) == 1) {
#if (DEBUG)
        Serial1.print("Ack received: ");
        Serial1.println(ack);
#endif
      } else {
#if (DEBUG)
        Serial1.println("Error reading ack");
#endif
      }
    }

    //Send ack back to PC
    Serial.write(ack);
    _rxIndex = 0;

    if (last) {
      break;
    }


  }
}
