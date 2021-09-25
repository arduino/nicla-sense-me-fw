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
SensorBSEC *pbsec = NULL;

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  BHY2.begin();

  accel.configure(1, 0);
  gyro.configure(1, 0);
  temp.configure(1, 0);
  gas.configure(1,0);
  
  if (BHY2.hasSensor(SENSOR_ID_BSEC)) {
      pbsec = (new SensorBSEC(SENSOR_ID_BSEC));
  } else if (BHY2.hasSensor(SENSOR_ID_BSEC_LEGACY)) {
      pbsec = (new SensorBSEC(SENSOR_ID_BSEC_LEGACY));
  }
  if (pbsec != NULL) pbsec->configure(1,0);
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
    Serial.println(String("temperature: ") + String(temp.value(),3));
    Serial.println(String("gas: ") + String(gas.value(),3));
    
    // Note: some fields of BSEC printed below might be always 0 if 
    // SENSOR_DATA_FIXED_LENGTH (defined in "SensorTypes.h") is smaller than the actual frame size of the BSEC sensor, 
    // SENSOR_DATA_FIXED_LENGTH could be enlarged to the size of the BSEC sensor frame, which is 18 bytes for BSEC new format (SID=115) 
    // or 29 bytes for legacy format (SID=171 SENSOR_ID_BSEC)
    // for the new format (SID=115 SENSOR_ID_BSEC_LEGACY), if the compensated values (comp_t, comp_h, comp_g) are not important to you, you could keep 
    // SENSOR_DATA_FIXED_LENGTH to the default value which is 10, this will save bandwidth for other sensors
    if (pbsec != NULL) Serial.println(String("bsec: ") + pbsec->toString());
  }
}
