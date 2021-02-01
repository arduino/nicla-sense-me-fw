#include "bhy-controller.h"
#include "eslov-controller.h"
#include "Arduino.h"

int _rxIndex = 0;
uint8_t _rxBuffer[255];

void setup()
{
  Wire.begin();   
  Serial.begin(115200);        

#if (DEBUG)
  Serial1.begin(115200);
delay(2000);
  Serial1.println("ok");
#endif
}

void loop()
{
  while (Serial.available()) {
    _rxBuffer[_rxIndex++] = Serial.read();

#if (DEBUG)
    Serial1.println(_rxBuffer[_rxIndex-1]);
    //Serial1.println(_rxBuffer[0]);
#endif

    if (_rxBuffer[0] == HOST_DFU_EXTERNAL_OPCODE || _rxBuffer[0] == HOST_DFU_INTERNAL_OPCODE) {
      if (_rxIndex == sizeof(DFUPacket) + 1) {

        writeDfuPacket(_rxBuffer, sizeof(DFUPacket) + 1);
        uint8_t ack = requestDfuPacketAck();

#if (DEBUG)
        { // dump rx buffer
          Serial1.print("Received: ");
          for(int n=0; n<_rxIndex; n++) {
            Serial1.print(_rxBuffer[n]);
            Serial1.print(", ");
          }
          Serial1.println();
        }
        { // print ack received
          Serial1.print("Sent Ack: ");
          Serial1.print(ack);
          Serial1.print(" back to PC");
          Serial1.println();
        }
#endif
        
        Serial.write(ack);
        _rxIndex = 0;
      }

    } else if (_rxBuffer[0] == HOST_READ_SENSOR_OPCODE) {

#if (DEBUG)
      Serial1.print("received read sensor opcode\r\n");
#endif
      uint8_t availableData = requestAvailableData();
      writeStateChange(ESLOV_READ_SENSOR_STATE);
      Serial.write(availableData);

      SensorDataPacket sensorData;
      while (availableData) {
        requestSensorData(sensorData);
        Serial.write((uint8_t*)&sensorData, sizeof(sensorData));
        availableData--;
      }


      _rxIndex = 0;

    } else if (_rxBuffer[0] == HOST_CONFIG_SENSOR_OPCODE) {
#if (DEBUG)
      Serial1.print("config");
#endif
      if (_rxIndex == sizeof(SensorConfigurationPacket) + 1) {
        SensorConfigurationPacket* config = (SensorConfigurationPacket*)&_rxBuffer[1];
        writeConfigPacket(config);

        _rxIndex = 0;
      }
      
    } else {
#if (DEBUG)
      Serial1.println("no opcode");
#endif
      _rxIndex = 0;
    }
  }
}
