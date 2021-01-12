#include "Arduino.h"
#include "Wire.h"

#define DEBUG false

#define ESLOV_DEFAULT_ADDRESS 0x55

#define ESLOV_DELAY (10)
#define ESLOV_DFU_CHUNK_SIZE (64)

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

void writeDfuPacket(uint8_t *data, uint8_t length)
{
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  int ret = Wire.write(data, length);
  if(DEBUG){
    Serial1.write("Write returned: ");
    Serial1.write(ret);
    Serial1.println();
  }
  Wire.endTransmission();
  delay(ESLOV_DELAY);
}

uint8_t requestDfuPacketAck()
{  
  uint8_t ret = 0;
  while(!ret) {
    ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, 1);
    if(DEBUG){
      Serial1.print("Request returned: ");
      Serial1.write(ret);
      Serial1.println();
    }
  }
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

  if(DEBUG){
    Serial1.begin(115200);
    while (!Serial1);
  }
}



void loop()
{

#if defined(ESLOV_DEBUG) 
  // Test packet transfer
  writeDfuPacket(testFw, sizeof(testFw));
  uint8_t ack = requestDfuPacketAck();
  Serial1.println(ack);
#else

  while (Serial.available()) {
    _rxBuffer[_rxIndex++] = Serial.read();
  }
  
  if (_rxIndex == sizeof(DFUPacket) + 1) {

    if(DEBUG){
      Serial1.write("Received: ");
      for(int n=0; n<_rxIndex; n++) {
        Serial1.write(_rxBuffer[n]);
        Serial1.write(", ");
      }
      Serial1.println();
    }

    if (_rxBuffer[0] == ESLOV_DFU_EXTERNAL_OPCODE || _rxBuffer[0] == ESLOV_DFU_INTERNAL_OPCODE) {
        writeDfuPacket(_rxBuffer, sizeof(DFUPacket) + 1);

        uint8_t ack = requestDfuPacketAck();
        if(DEBUG){
          Serial1.write("Sent Ack: ");
          Serial1.write(ack);
          Serial1.write(" back to PC");
          Serial1.println();
        }
        
        Serial.write(ack);

        _rxIndex = 0;

    }
  }

#endif
}