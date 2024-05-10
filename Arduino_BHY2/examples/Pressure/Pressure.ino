/*
 * This example shows how to use the access the pressure data and send it over serial.
 * 
 * Every 1 second, this sketch will send the pressure in hPa over serial.
 * SensorID 129 (SENSOR_ID_BARO) is read with the Sensor class from the BMP390.
 * 
 * Instructions:
 * 1. Upload this sketch to your Nicla Sense ME board.
 * 2. Open the Serial Monitor at a baud rate of 115200.
 * 3. The pressure will be printed to the serial monitor.
 * 
 * Initial author: @mcmchris
 */

#include "Arduino_BHY2.h"


unsigned long previousMillis = 0;  // will store last time the sensor was updated
const long interval = 1000;

Sensor pressure(SENSOR_ID_BARO);

void setup() {
  Serial.begin(9600);
  BHY2.begin();
  pressure.begin();
}

void loop() {
  BHY2.update();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println(String(pressure.value()) + " hPa");
  }
}