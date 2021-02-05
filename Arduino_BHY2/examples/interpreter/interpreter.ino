#include "Arduino.h"
#include "Arduino_BHY2.h"
#include "interpreter.h"

void setup()
{
  Serial.begin(115200);

  //BHY2.debug(Serial);
  BHY2.begin();

  // Configure the gyroscope
  SensorConfigurationPacket config;
  config.sampleRate = 1;
  config.latency = 0;
  config.sensorId = 13;
  BHY2.configureSensor(&config);
}

int16_t extractInt16(uint64_t data, uint8_t offset) {
  uint16_t low = (data >> (offset * 8)) & 0x00FF;
  uint16_t high = (data >> ((offset + 1) * 8) ) & 0x0FF;
  int16_t value = (int16_t) (low | (high << 8));
  return value;
}

void parseXYZ(uint64_t data, DataXYZ& vector)
{
  vector.x = extractInt16(data, 0);
  vector.y = extractInt16(data, 2);
  vector.z = extractInt16(data, 4);
  Serial.print("value: ");
  Serial.print("x: ");
  Serial.print(vector.x);
  Serial.print("   y: ");
  Serial.print(vector.y);
  Serial.print("   z: ");
  Serial.println(vector.z);
}

void loop()
{
  static auto time = millis();
  if (millis() - time >= 200) {
    time = millis();

    if (BHY2.availableSensorData() > 0) {
      SensorDataPacket data;
      BHY2.readSensorData(data);

      Serial.println("received data from sensor: ");
      Serial.println(data.sensorId);
      DataXYZ value;
      parseXYZ(data.data, value);
      Serial.println();
    }
  }

  BHY2.update();
}


