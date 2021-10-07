/* 
 * This sketch shows how an arduino board can act as a host for nicla. 
 * An host board can configure the sensors of nicla and then read their values.
 * The host board should be connected to nicla through the eslov connector.
 * 
 * In this example, the accelerometer sensor is enabled and its
 * values are periodically read and then printed to the serial channel
 * 
 * NOTE: if Nicla is used as a Shield on top of a MKR board,
 * please use BHY2Host.begin(false, NICLA_AS_SHIELD)
*/

#include "Arduino.h"
#include "Arduino_BHY2Host.h"

SensorXYZ accel(SENSOR_ID_ACC);

void setup()
{
  // debug port
  Serial.begin(115200);
  while(!Serial);

  BHY2Host.begin();

  accel.begin();
}

void loop()
{
  static auto printTime = millis();
  BHY2Host.update();

  if (millis() - printTime >= 1000) {
    printTime = millis();
    Serial.println(String("Acceleration values: ") + accel.toString());
  }
}
