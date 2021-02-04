#include "Arduino.h"
#include "ArduinoBLE.h"

#define DEBUG true

#define DFU_CHUNK_SIZE (64)

int _rxIndex = 0;
uint8_t _rxBuffer[255];
bool UNISENSE_available = false;

bool last = false;

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
  uint8_t data[DFU_CHUNK_SIZE];
};

static uint8_t testFw[sizeof(DFUPacket)] = {
  0x00, 0x00, 0x00,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

BLECharacteristic dfuAckCharacteristic;
BLECharacteristic dfuIntCharacteristic;
BLECharacteristic dfuExtCharacteristic;

BLEDevice peripheral;

void writeDfuPacket(uint8_t *data, uint8_t len)
{
  while (!peripheral.connected());

  //Send packet with BLE
  uint8_t write_ret = 0;
  //OPCODE must not be sent
  uint8_t oc = data[0];

  if (oc == ESLOV_DFU_INTERNAL_OPCODE) {
    write_ret = dfuIntCharacteristic.writeValue(&data[1], len);
  } else if (oc == ESLOV_DFU_EXTERNAL_OPCODE) {
    write_ret = dfuExtCharacteristic.writeValue(&data[1], len);
  }

#if (DEBUG)
  Serial1.print("writeValue returned: ");
  Serial1.println(write_ret);
#endif

}


uint8_t requestDfuPacketAck()
{
  while (!peripheral.connected());

  byte ack = 0;

  //Request ack
  int read_ret = dfuAckCharacteristic.readValue(ack);
  if (read_ret == 1) {
#if (DEBUG)
    Serial1.print("Ack received: ");
    Serial1.println(ack);
#endif
  } else {
#if (DEBUG)
    Serial1.print("Error reading ack! ReadValue returned: ");
    Serial1.println(read_ret);
#endif
  }

  return ack;

}


void setup()
{
  Serial.begin(115200);

#if (DEBUG)
  Serial1.begin(115200);
  while (!Serial1);
  Serial1.println("FW UPLOAD BLE SKETCH");
#endif

  BLE.noDebug();
  auto ret = BLE.begin();
#if (DEBUG)
  Serial1.println(ret);
#endif

  BLE.scanForName("UNISENSE");

  while (!UNISENSE_available) {

    // check if a peripheral has been discovered
    peripheral = BLE.available();

    if (peripheral) {
      UNISENSE_available = true;
#if (DEBUG)
      Serial1.println("Found ");
#endif

      BLE.stopScan();

      delay(1000);

      // Connect to peripheral
      if (peripheral.connect()) {
#if (DEBUG)
        Serial1.println("Connected");
#endif
      } else {
#if (DEBUG)
        Serial1.println("Failed to connect!");
        while (1);
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

      dfuAckCharacteristic = peripheral.characteristic("34c2e3be-34aa-11eb-adc1-0242ac120002");
      dfuIntCharacteristic = peripheral.characteristic("34c2e3b9-34aa-11eb-adc1-0242ac120002");
      dfuExtCharacteristic = peripheral.characteristic("34c2e3ba-34aa-11eb-adc1-0242ac120002");
      if ((!dfuAckCharacteristic) && (!dfuIntCharacteristic)) {
#if (DEBUG)
        Serial1.println("Peripheral does not have characteristics!");
#endif
        peripheral.disconnect();
        return;
      } else if (!dfuIntCharacteristic.canWrite()) {
#if (DEBUG)
        Serial1.println("Peripheral does not have a writable characteristic!");
#endif
        peripheral.disconnect();
        return;
      }

      dfuAckCharacteristic.subscribe();

      //updateFw(peripheral);

    }
  }

#if (DEBUG)
  Serial1.println("Setup completed");
#endif
}


void loop()
{
  while (Serial.available()) {
    _rxBuffer[_rxIndex++] = Serial.read();
    if (_rxIndex == sizeof(DFUPacket) + 1) {

      if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE || _rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {

        writeDfuPacket(_rxBuffer, sizeof(DFUPacket));

#if (DEBUG)
        Serial1.print("Received: ");
        for (int n = 0; n < _rxIndex; n++) {
          Serial1.print(_rxBuffer[n], HEX);
          Serial1.print(", ");
        }
        Serial1.println();
#endif

        uint8_t ack = requestDfuPacketAck();

        Serial.write(ack);
        _rxIndex = 0;

      }
    }
  }

}
