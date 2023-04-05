/*
   This sketch shows how to retrieve actual configuration
   - sample rate and latency – from sensors.
*/

#include "Arduino_BHY2.h"

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
SensorQuaternion rotation(SENSOR_ID_RV);

SensorConfig cfg;

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

    cfg = accel.getConfiguration();
    Serial.println(String("acceleration configuration - rate: ") + cfg.sample_rate + String("Hz - latency: ") + cfg.latency + String("ms - range: ") + cfg.range);

    cfg = gyro.getConfiguration();
    Serial.println(String("gyro configuration - rate: ") + cfg.sample_rate + String(" - latency: ") + cfg.latency + String("ms - range: ") + cfg.range);

    cfg = temp.getConfiguration();
    Serial.println(String("temperature configuration - rate: ") + cfg.sample_rate + String(" - latency: ") + cfg.latency + String("ms - range: ") + cfg.range);

    cfg = gas.getConfiguration();
    Serial.println(String("gas configuration - rate: ") + cfg.sample_rate + String(" - latency: ") + cfg.latency + String("ms - range: ") + cfg.range);

    cfg = rotation.getConfiguration();
    Serial.println(String("rotation configuration - rate: ") + cfg.sample_rate + String(" - latency: ") + cfg.latency + String("ms - range: ") + cfg.range);
    Serial.println();
  }
}