#include <Arduino.h>

#include "Wire.h"
#include "SensorTypes.h"

#define ESLOV_DEFAULT_ADDRESS 0x55

bool dataReceived = false;
SensorDataPacket sensorData;

enum EslovOpcode {
  ESLOV_DFU_INTERNAL_OPCODE,
  ESLOV_DFU_EXTERNAL_OPCODE,
  ESLOV_SENSOR_CONFIG_OPCODE,
  ESLOV_SENSOR_STATE_OPCODE
};

enum EslovState {
  ESLOV_AVAILABLE_SENSOR_STATE = 0x00,
  ESLOV_READ_SENSOR_STATE = 0x01
};

void setup()
{
  Serial.begin(115200);
  //while (!Serial);
  Wire.begin();                
}

void writeStateChange(EslovState state)
{
  delay(100);
  uint8_t packet[2] = {ESLOV_SENSOR_STATE_OPCODE, state};
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write((uint8_t*)packet, sizeof(packet));
  Wire.endTransmission();
  delay(100);
}

uint8_t requestAvailableData() 
{
  writeStateChange(ESLOV_AVAILABLE_SENSOR_STATE);
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, 1);
  if (!ret) return 0;
  return Wire.read();
  delay(100);
}

void requestSensorData(SensorDataPacket &sData)
{
  delay(100);
  uint8_t ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, sizeof(SensorDataPacket));
  if (!ret) return;

  uint8_t *data = (uint8_t*)&sData;
  for (uint8_t i = 0; i < sizeof(SensorDataPacket); i++) {
    data[i] = Wire.read();
  }
  delay(100);
}

void printSensorData(SensorDataPacket &sData) 
{
  Serial.println("Sensor data");
  Serial.print("Id: ");
  Serial.println(sData.sensorId);
  Serial.print("Data: ");
  Serial.println((uint8_t)sData.data);
  Serial.println();
}

void loop()
{
  static auto time = millis();

  if (millis() - time >= 3000) {
    time = millis();

    uint8_t availableData = requestAvailableData();
    writeStateChange(ESLOV_READ_SENSOR_STATE);

    while (availableData) {
      requestSensorData(sensorData);
      printSensorData(sensorData);
      availableData--;
    }

  }
}
