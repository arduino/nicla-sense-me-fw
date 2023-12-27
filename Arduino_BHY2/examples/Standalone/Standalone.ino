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
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
SensorQuaternion rotation(SENSOR_ID_RV);

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  BHY2.begin();

  accel.begin();
  gyro.begin();
  temp.begin();
  gas.begin();
  rotation.begin();

  SensorConfig cfg = accel.getConfiguration();
  Serial.println(String("range of accel: +/-") + cfg.range + String("g"));
  accel.setRange(2);    //this sets the range of accel to +/-4g, 
  cfg = accel.getConfiguration();
  Serial.println(String("range of accel: +/-") + cfg.range + String("g"));

  cfg = gyro.getConfiguration();
  Serial.println(String("range of gyro: +/-") + cfg.range + String("dps"));  
  gyro.setRange(1000);    
  cfg = gyro.getConfiguration();
  Serial.println(String("range of gyro: +/-") + cfg.range + String("dps"));
}

void loop()
{
  static auto printTime = millis();

  // Update function should be continuously polled
  BHY2.update();

  if (millis() - printTime >= 1000) {
    printTime = millis();

    if(accel.dataAvailable()) {
      Serial.println(String("acceleration: ") + accel.toString());
      accel.clearDataAvailFlag();
    }

    if(gyro.dataAvailable()) {
      Serial.println(String("gyroscope: ") + gyro.toString());
      gyro.clearDataAvailFlag();
    }

    if (temp.dataAvailable()) {
      Serial.println(String("temperature: ") + String(temp.value(),3));
      temp.clearDataAvailFlag();
    }

    if (gas.dataAvailable()) {
      Serial.println(String("gas: ") + String(gas.value(),3));
      gas.clearDataAvailFlag();
    }

    if (rotation.dataAvailable()) {
      Serial.println(String("rotation: ") + rotation.toString());
      rotation.clearDataAvailFlag();
    }
  }
}
