#include "eslov-controller.h"

void writeDfuPacket(uint8_t *data, uint8_t length)
{
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  int ret = Wire.write(data, length);
#if (DEBUG)
  Serial1.write("Write returned: ");
  Serial1.write(ret);
  Serial1.println();
#endif
  Wire.endTransmission(false);
}

void writeStateChange(EslovState state)
{
  delay(10);
  uint8_t packet[2] = {ESLOV_SENSOR_STATE_OPCODE, state};
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write((uint8_t*)packet, sizeof(packet));
  Wire.endTransmission();
  delay(10);
}

void writeConfigPacket(SensorConfigurationPacket* config)
{
  delay(10);
  uint8_t packet[sizeof(config) + 1]; 
  packet[0] = ESLOV_SENSOR_CONFIG_OPCODE;
  memcpy(&packet[1], config, sizeof(config));
  Wire.beginTransmission(ESLOV_DEFAULT_ADDRESS);
  Wire.write(packet, sizeof(packet));
  Wire.endTransmission();
  delay(10);
}

uint8_t requestDfuPacketAck()
{ 
  delay(10);
  uint8_t ret = 0;
  while(!ret) {
    ret = Wire.requestFrom(ESLOV_DEFAULT_ADDRESS, 1);
#if (DEBUG)
    Serial1.print("Request returned: ");
    Serial1.write(ret);
    Serial1.println();
#endif
  }
  return Wire.read();
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