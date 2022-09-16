/* 
 * This sketch shows how to get and set the range settings for the built-in IMU of the BHI260 sensor on Nicla Sense ME
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
SensorXYZ gravity(SENSOR_ID_GRA);

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  BHY2.begin();

  accel.begin();
  gyro.begin();


  SensorConfig cfg = accel.getConfiguration();
  Serial.println(String("range of accel: +/-") + cfg.range + String("g"));
  cfg = gravity.getConfiguration();
  Serial.println(String("range of gravity: +/-") + cfg.range + String("g"));

  accel.setRange(4);    //this sets the range of accel to +/-4g, 
  cfg = accel.getConfiguration();
  Serial.println(String("range of accel: +/-") + cfg.range + String("g"));

  cfg = gravity.getConfiguration();
  Serial.println(String("range of gravity: +/-") + cfg.range + String("g"));

  cfg = gyro.getConfiguration();
  Serial.println(String("range of gyro: +/-") + cfg.range + String("dps"));  
  gyro.setRange(1000);  //this sets the range of gyro to +/-1000dps, 
  cfg = gyro.getConfiguration();
  Serial.println(String("range of gyro: +/-") + cfg.range + String("dps"));
}

void loop()
{
  static auto printTime = millis();

  // Update function should be continuously polled
  BHY2.update();

  if (millis() - printTime >= 1000) {
    printTime = millis();
    Serial.println(String("acceleration: ") + accel.toString());
    Serial.println(String("gyroscope: ") + gyro.toString());
  }
}
