/*
 * This example shows how to use the Nicla Sense ME library to detect activity and send it over serial.
 * 
 * Every 1 second, this sketch will send the latest activity detected via the BHI260AP sensor to the serial port.
 * Without any additional training, it is possible to detect Still, Walking, Running and Tilting activities
 * A full description of supported activities is available in Table 93 of the BHI260AP datasheet.
 * 
 * Instructions:
 * 1. Upload this sketch to your Nicla Sense ME board.
 * 2. Open the Serial Monitor at a baud rate of 115200.
 * 3. Observe the detected activity, by moving the Nicla Sense ME board.
 * 
 * Initial author: @mcmchris
 */

#include "Arduino_BHY2.h"

SensorActivity active(SENSOR_ID_AR);

unsigned long previousMillis = 0;  // will store last time the sensor was updated
const long interval = 1000;


void setup() {
  Serial.begin(115200);
  BHY2.begin();
  active.begin();
}

void loop() {
  BHY2.update();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println(String("Activity info: ") + active.toString());
  }
}