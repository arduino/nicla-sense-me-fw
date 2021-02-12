/* 
 * This sketch shows how unisense can be used in standalone mode.
 * Without the need for an host, unisense can run sketches that 
 * are able to configure the bhi sensors and are able to read all 
 * the bhi sensors data.
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

#define ACCEL_ID (4)
DataXYZ accValue;

#define GYRO_ID (13)
DataXYZ gyroValue;

void configureSensors() {
  SensorConfigurationPacket config;
  config.sampleRate = 1;
  config.latency = 0;

  config.sensorId = ACCEL_ID;
  BHY2.configureSensor(config);

  config.sensorId = GYRO_ID;
  BHY2.configureSensor(config);
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

  // Update function should be continuously polled
  BHY2.update();

  // Retrieve new data at each 10 ms
  if (millis() - sampleTime >= 10) {
    sampleTime = millis();

    if (BHY2.availableSensorData() > 0) {
      SensorDataPacket data;
      BHY2.readSensorData(data);

      if (data.sensorId == ACCEL_ID) {
        Serial.println("Received accel");
        BHY2.parse(data, accValue);

      } else if (data.sensorId == GYRO_ID) {
        Serial.println("Received gyro");
        BHY2.parse(data, gyroValue);

      }
    }
  }

  // Print the last sensor data at each 200 ms
  if (millis() - printTime >= 200) {
    printTime = millis();

    Serial.print("Accelerometer values: ");
    Serial.println(accValue.toString());

    Serial.print("Orientation values: ");
    Serial.println(gyroValue.toString());
  }
}


