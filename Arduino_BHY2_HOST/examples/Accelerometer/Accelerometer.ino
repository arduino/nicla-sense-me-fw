#include "Arduino.h"
#include "Arduino_BHY2_HOST.h"

#define ACCEL_ID (4)
DataXYZ accValue;

void configureSensors()
{
  SensorConfigurationPacket config;
  config.sampleRate = 1;
  config.latency = 0;

  config.sensorId = ACCEL_ID;
  BHY2_HOST.configureSensor(&config);
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
  static auto printTime = millis();
  static auto sampleTime = millis();

  if (millis() - sampleTime >= 200) {
    sampleTime = millis();

    if (BHY2_HOST.availableSensorData() > 0) {
      SensorDataPacket data;
      BHY2_HOST.readSensorData(data);

      if (data.sensorId == ACCEL_ID) {
        Serial.print("Received accel: ");
        BHY2_HOST.parse(data, accValue);
      }
    }
  }

  if (millis() - printTime >= 200) {
    printTime = millis();

    Serial.print("Acceleration values: ");
    Serial.println(accValue.toString());
  }


  delay(500);
}
