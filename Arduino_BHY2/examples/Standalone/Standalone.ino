/* 
 * This sketch shows how nicla can be used in standalone mode.
 * Without the need for an host, nicla can run sketches that 
 * are able to configure the bhi sensors and are able to read all 
 * the bhi sensors data.
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
SensorTemperature temp(SENSOR_ID_TEMP);

#define TEMP_SCALE_FACTOR (0.01)

void setup()
{
  Serial.begin(115200);

  BHY2.begin();

  accel.configure(1, 0);
  gyro.configure(1, 0);
  temp.configure(1, 0);
  temp.setFactor(TEMP_SCALE_FACTOR);
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
    Serial.println(String("temperature: ") + String(int(temp.value())));
  }
}
