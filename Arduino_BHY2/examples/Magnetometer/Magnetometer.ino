/*
 * This example shows how to use the access the magnetometer data and send it over serial.
 * 
 * Every 1 second, this sketch will send the heading of the magnetometer over serial.
 * by calculating the arctangent between the x and y axis and multiplied in a conversion
 * factor to convert the headings from radians to degrees.
 * 
 * Instructions:
 * 1. Upload this sketch to your Nicla Sense ME board.
 * 2. Open the Serial Monitor at a baud rate of 115200.
 * 3. The heading of the magnetometer will be printed to the serial monitor.
 * 
 * Initial author: @mcmchris
 */

#include "Arduino_BHY2.h"
#include "math.h"

SensorXYZ magnetometer(SENSOR_ID_MAG);

float heading = 0;
unsigned long previousMillis = 0;  // will store last time the sensor was updated
const long interval = 1000;

void setup() {
  Serial.begin(115200);
  BHY2.begin();
  magnetometer.begin();
}

void loop() {
  BHY2.update();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    heading = round(atan2(magnetometer.x(), magnetometer.y()) * 180.0 / PI);
    Serial.println(String(heading) + "º");
  }
}