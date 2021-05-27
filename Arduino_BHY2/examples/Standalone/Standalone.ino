/* 
 * This sketch shows how nicla can be used in standalone mode.
 * Without the need for an host, nicla can run sketches that 
 * are able to configure the bhi sensors and are able to read all 
 * the bhi sensors data.
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

#define ACCEL_ID (SENSOR_ID_ACC)
#define GYRO_ID (SENSOR_ID_GYRO)

SensorXYZ accel(ACCEL_ID);
SensorXYZ gyro(GYRO_ID);

void setup()
{
  Serial.begin(115200);

  BHY2.begin();

  accel.configure(1, 0);
  gyro.configure(1, 0);
}

void loop()
{
  static auto printTime = millis();
  static auto sampleTime = millis();

  // Update function should be continuously polled
  BHY2.update();

  if (millis() - sampleTime >= 1000) {
    sampleTime = millis();

    Serial.println(String("acceleration: ") + accel.toString());
    Serial.println(String("gyroscope: ") + gyro.toString());
  }
}
