/* 
 * This sketch shows how an arduino board can act as a host for nicla. 
 * An host board can configure the sensors of nicla and then read their values.
 * The host board should be connected to nicla through the eslov connector.
 * 
 * In this example, the orientation sensor is enabled and its
 * values are periodically read and then printed to the serial channel
*/

#include "Arduino.h"
#include "Arduino_BHY2_HOST.h"

#define ORI_SCALE_FACTOR (360.0f / 32768.0f)
SensorOrientation ori(SENSOR_ID_ORI);

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  BHY2_HOST.begin();

  ori.setFactor(ORI_SCALE_FACTOR);
  ori.configure(1, 0);
}

void loop()
{
  static auto printTime = millis();
  BHY2_HOST.update();

  if (millis() - printTime >= 1000) {
    printTime = millis();
    Serial.println(String("Orientation values: ") + ori.toString());
  }
}
