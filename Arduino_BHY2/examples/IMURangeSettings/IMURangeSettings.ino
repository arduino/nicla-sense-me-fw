/* 
 * IMURangeSettings
 * 
 * This sketch demonstrates how to configure the acceleration (SENSOR_ID_ACC) and gyroscope (SENSOR_ID_GYRO) range values.
 * In the setup function, the default ranges are printed to the serial for the acceleration and gravity (+/-8g).
 * Then, the range is modified to +/-4g, and is confirmed by printing the value of .range method to the Serial monitor. Finally,
 * the setup function prints the default range value for the gyroscope (+/-2000 dps) and then modifies this to +/-1000 dps.
 * 
 * Every second, the loop function prints out acceleration and gyroscope values to the Serial port.
 * 
 * 
*/

#include "Arduino_BHY2.h"

// declare 3D sensor instances
// default ADC range is -32768 to 32767 (16 bit)
SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
SensorXYZ gravity(SENSOR_ID_GRA);

void setup() {
  Serial.begin(9600);
  while(!Serial);

  BHY2.begin();

  accel.begin();
  gyro.begin();

  delay(1000);

  SensorConfig cfg = accel.getConfiguration();

  Serial.println("Default Range values:");
  Serial.println(String("range of accel: +/-") + cfg.range + String("g"));
  cfg = gravity.getConfiguration();
  Serial.println(String("range of gravity: +/-") + cfg.range + String("g"));

  accel.setRange(4);    //this sets the range of accel to +/-4g, 
  Serial.println("Modified Range values:");
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

void loop() {
  static auto printTime = millis();

  // Update function should be continuously polled
  BHY2.update();

  // print gyroscope/acceleration data once every second
  if (millis() - printTime >= 1000) {
    printTime = millis();
    Serial.print(String("acceleration (raw): ") + accel.toString());

    float accelX = ((float)accel.x() / 32768.0) *4;
    float accelY = ((float)accel.y() / 32768.0) *4;
    float accelZ = ((float)accel.z() / 32768.0) *4;
    String accelInG = String("X: ") + String(accelX) + String(" Y: ") + String(accelY) + String(" Z: ") + String(accelZ);

    Serial.println(String("acceleration (g): ") + accelInG);
    Serial.println(String("gyroscope: ") + gyro.toString());
  }
}
