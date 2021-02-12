/* 
 * This sketch shows how an arduino board can act as a host for unisense. 
 * An host board can configure the sensors of unisense and then read their values.
 * The host board should be connected to unisense through the eslov connector.
 * 
 * In this example, the orientation sensor is enabled and its
 * values are periodically read and then printed to the serial channel
*/

#include "Arduino.h"
#include "Arduino_BHY2_HOST.h"

#define ORI_ID (43)
#define ORI_SCALE_FACTOR (360.0f / 32768.0f)
DataOrientation orientation;

void configureSensors()
{
  SensorConfigurationPacket config;
  config.sampleRate = 1;
  config.latency = 0;

  config.sensorId = ORI_ID;
  BHY2_HOST.configureSensor(config);
}

void setup()
{
  Serial.begin(115200);
  while(!Serial);

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

      if (data.sensorId == ORI_ID) {
        Serial.print("Received orientation: ");
        BHY2_HOST.parse(data, orientation, ORI_SCALE_FACTOR);
      }
    }
  }

  if (millis() - printTime >= 200) {
    printTime = millis();

    Serial.print("Orientation values: ");
    Serial.println(orientation.toString());
  }


  delay(500);
}
