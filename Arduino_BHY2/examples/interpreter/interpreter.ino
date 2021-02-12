#include "Arduino.h"
#include "Arduino_BHY2.h"

#define ACCEL_ID (4)
DataXYZ accValue;

#define GYRO_ID (13)
DataXYZ gyroValue;

#define ORI_ID (43)
DataOrientation orientation;

void configureSensors() {
  SensorConfigurationPacket config;
  config.sampleRate = 1;
  config.latency = 0;

  config.sensorId = ACCEL_ID;
  BHY2.configureSensor(&config);

  config.sensorId = ORI_ID;
  BHY2.configureSensor(&config);

  config.sensorId = GYRO_ID;
  BHY2.configureSensor(&config);

  config.sensorId = 34;
  BHY2.configureSensor(&config);
}

void setup()
{
  Serial.begin(115200);

  BHY2.begin();

  configureSensors();
}

void loop()
{
  static auto printTime = millis();
  static auto sampleTime = millis();

  if (millis() - sampleTime >= 10) {
    sampleTime = millis();

    if (BHY2.availableSensorData() > 0) {
      SensorDataPacket data;
      BHY2.readSensorData(data);

      if (data.sensorId == ACCEL_ID) {
        Serial.print("Received accel: ");
        BHY2.parse(data, accValue);

      } else if (data.sensorId == GYRO_ID) {
        Serial.print("Received gyro: ");
        BHY2.parse(data, gyroValue);

      } else if (data.sensorId == ORI_ID) {
        Serial.println("orientation");
        BHY2.parse(data, orientation, 360.0f / 32768.0f);

      }
    }
  }

  if (millis() - printTime >= 200) {
    printTime = millis();

    Serial.println(orientation.toString());
    Serial.println(gyroValue.toString());
  }

  BHY2.update();
}


