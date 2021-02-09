#include "Arduino.h"
#include "Arduino_BHY2_HOST.h"

void configureSensors()
{
  SensorConfigurationPacket config;
  config.sampleRate = 1;
  config.latency = 0;
  for (uint8_t i = 1; i < 20; i++)
  {
    config.sensorId = i;
    BHY2_HOST.configureSensor(&config);
  }
  Serial.println("configured");
}

void setup()
{
  // debug port
  Serial.begin(115200);
  while(!Serial);

  //BHY2_HOST.debug(Serial);
  BHY2_HOST.begin();

  configureSensors();
}

void loop()
{
  BHY2_HOST.update();

  auto availableData = BHY2_HOST.availableSensorData();
  Serial.print("Available data: ");
  Serial.println(availableData);

  while (availableData) {
    availableData--;

    SensorDataPacket data;
    BHY2_HOST.readSensorData(data);
    Serial.print("sensor: ");
    Serial.print(data.sensorId);
    Serial.print("  size: ");
    Serial.println(data.size);
  }

  delay(500);
}
