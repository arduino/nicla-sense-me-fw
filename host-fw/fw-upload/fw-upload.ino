#include "Arduino.h"
#include "Wire.h"

#define DEBUG false

#define ESLOV_DEFAULT_ADDRESS 0x55

#define ESLOV_DELAY (1)
#define ESLOV_DFU_CHUNK_SIZE (64)

enum EslovOpcode {
  ESLOV_DFU_INTERNAL_OPCODE,
  ESLOV_DFU_EXTERNAL_OPCODE,
  ESLOV_SENSOR_CONFIG_OPCODE,
  ESLOV_SENSOR_STATE_OPCODE
};

enum EslovState {
  ESLOV_AVAILABLE_SENSOR_STATE = 0x00,
  ESLOV_READ_SENSOR_STATE = 0x01,
  ESLOV_DFU_ACK_STATE = 0x02
};

struct __attribute__((packed)) DFUPacket {
  uint8_t last: 1;
  union {
    uint16_t index: 15;
    uint16_t remaining: 15;
  };
  uint8_t data[ESLOV_DFU_CHUNK_SIZE];
};

void writeStateChange(EslovState state)
{
  delay(ESLOV_DELAY);
  uint8_t packet[2] = {ESLOV_SENSOR_STATE_OPCODE, state};
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write((uint8_t*)packet, sizeof(packet));
  Wire.endTransmission();
  delay(ESLOV_DELAY);
}

void writeDfuPacket(uint8_t *data, uint8_t length)
{
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write(data, length);
  Wire.endTransmission();
  delay(ESLOV_DELAY);
}

uint8_t requestDfuPacketAck()
{
  writeStateChange(ESLOV_DFU_ACK_STATE);
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, 1);
  if (!ret) return 0;
  return Wire.read();
}

#if defined (ESLOV_DEBUG) && (ESLOV_DFU_CHUNK_SIZE == 64)
static uint8_t testFw[sizeof(DFUPacket)+1] = {
  0x01, 0x00, 0x00, 0x00,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};
#elif defined (ESLOV_DEBUG) && (ESLOV_DFU_CHUNK_SIZE == 8)
static uint8_t testFw[sizeof(DFUPacket)+1] = {
  0x01, 0x00, 0x00, 0x00,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
};
#endif

int _rxIndex;
uint8_t _rxBuffer[255];

void setup()
{
  Wire.begin();   
  Serial.begin(115200);  
  while (!Serial);           

#if !defined(ESLOV_DEBUG) 
  Serial1.begin(115200);
  while (!Serial1);
#endif
}

void loop()
{

#if defined(ESLOV_DEBUG) 
  // Test packet transfer
  writeDfuPacket(testFw, sizeof(testFw));
  uint8_t ack = requestDfuPacketAck();
  Serial.println(ack);
#else

  while (Serial1.available()) {
    _rxBuffer[_rxIndex++] = Serial1.read();
  }
  
  if (_rxIndex == sizeof(DFUPacket) + 1) {

    if(DEBUG){
      Serial.print("Received: ");
      for(int n=0; n<_rxIndex; n++) {
        Serial.print(_rxBuffer[n], HEX);
        Serial.print(", ");
      }
      Serial.println();
    }

    if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE || _rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {
        writeDfuPacket(_rxBuffer, sizeof(DFUPacket) + 1);

        uint8_t ack = requestDfuPacketAck();
        if(DEBUG){
          Serial.print("Sent Ack: ");
          Serial.print(ack, HEX);
          Serial.println(" back to PC");
        }
        
        Serial1.write(ack);

        _rxIndex = 0;

    }
  }

#endif
}
