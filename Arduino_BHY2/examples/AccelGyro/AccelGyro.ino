/*
 * This example shows how to use the access the IMU data and plot it in real time.
 * 
 * Every 50 milliseconds, this sketch will send the x,y and z accelerometer (SensorID 4) and
 * gyroscope (SensorID 22) values over serial from the BHI260AP six axis IMU.
 * These six values are visually displayed with the Serial Plotter in the Arduino IDE. 
 * 
 * Instructions:
 * 1. Upload this sketch to your Nicla Sense ME board.
 * 2. Open the Serial Plotter at a baud rate of 115200.
 * 3. The three axis values for both the accelerometer and gyroscope will be printed to the Serial Plotter.
 * 4. Optionally, you can change the visibility of each data by clicking on the legend.
 * 
 * Initial author: @mcmchris
 */

#include "Arduino_BHY2.h"

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);


void setup() {
  Serial.begin(115200);
  BHY2.begin();
  accel.begin();
  gyro.begin();
}

void loop() {
  static auto printTime = millis();

  // Update function should be continuously polled
  BHY2.update();

  if (millis() - printTime >= 50) {
    printTime = millis();

    // Accelerometer values
    Serial.print("acc_X:");
    Serial.print(accel.x());
    Serial.print(",");
    Serial.print("acc_Y:");
    Serial.print(accel.y());
    Serial.print(",");
    Serial.print("acc_Z:");
    Serial.print(accel.z());
    Serial.print(",");

    // Gyroscope values
    Serial.print("gyro_X:");
    Serial.print(gyro.x());
    Serial.print(",");
    Serial.print("gyro_Y:");
    Serial.print(gyro.y());
    Serial.print(",");
    Serial.print("gyro_Z:");
    Serial.println(gyro.z());
  }
}