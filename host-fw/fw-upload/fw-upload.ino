#include "Arduino.h"
#include "Wire.h"

#define DEBUG false

#define ESLOV_DEFAULT_ADDRESS 0x55

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
  uint8_t data[8];
};

void writeStateChange(EslovState state)
{
  delay(100);
  uint8_t packet[2] = {ESLOV_SENSOR_STATE_OPCODE, state};
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write((uint8_t*)packet, sizeof(packet));
  Wire.endTransmission();
  delay(100);
}

void writeDfuPacket(uint8_t *data, uint8_t length)
{
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write(data, length);
  Wire.endTransmission();
  delay(100);
}

uint8_t requestDfuPacketAck()
{
  writeStateChange(ESLOV_DFU_ACK_STATE);
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, 1);
  if (!ret) return 0;
  return Wire.read();
  delay(100);
}


int _rxIndex;
uint8_t _rxBuffer[255];

void setup()
{
  Wire.begin();   
  Serial.begin(115200);  
  while (!Serial);           
  Serial1.begin(115200);
  while (!Serial1);
}

void loop()
{
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
}
