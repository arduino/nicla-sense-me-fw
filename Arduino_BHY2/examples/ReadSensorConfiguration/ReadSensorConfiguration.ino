/*
   This sketch shows how to retrieve actual configuration
   - sample rate and latency – from sensors.
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
SensorQuaternion rotation(SENSOR_ID_RV);

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  BHY2.begin();

  accel.begin();
  gyro.begin();
  temp.begin();
  gas.begin();
  rotation.begin();

}

void loop()
{
  static auto printTime = millis();

  // Update function should be continuously polled
  BHY2.update();

  if (millis() - printTime >= 1000) {
    printTime = millis();

    static float rate;
    static uint32_t latency;

    accel.readConfiguration(&rate, &latency);
    Serial.println(String("acceleration configuration - rate: ") + rate + String("Hz - latency: ") + latency + String("ms"));

    gyro.readConfiguration(&rate, &latency);
    Serial.println(String("gyro configuration - rate: ") + rate + String(" - latency: ") + latency + String("ms"));

    temp.readConfiguration(&rate, &latency);
    Serial.println(String("temperature configuration - rate: ") + rate + String(" - latency: ") + latency + String("ms"));

    gas.readConfiguration(&rate, &latency);
    Serial.println(String("gas configuration - rate: ") + rate + String(" - latency: ") + latency + String("ms"));

    rotation.readConfiguration(&rate, &latency);
    Serial.println(String("rotation configuration - rate: ") + rate + String(" - latency: ") + latency + String("ms"));
    Serial.println();
  }
}
