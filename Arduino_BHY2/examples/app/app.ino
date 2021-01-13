#include "Arduino.h"
#include "Arduino_BHY2.h"

// test purpose function 
void injectSensorData()
{
  SensorDataPacket sensorData;
  sensorData.sensorId = random(10);
  sensorData.data = random(255);
  sensorData.size = 1;

  BHY2.addSensorData(sensorData);
}

// Initialize bosch sensortec
// Initialize and link channels 
void setup()
{
  // for test
  randomSeed(analogRead(0));
  ///////

  // debug port
  Serial.begin(115200);

  BHY2.debug(Serial);
  BHY2.begin();
}

void loop()
{
  // test purpose: inject random sensor data
  static auto time = millis();
  if (millis() - time >= 200) {
    injectSensorData();
    time = millis();
  }
  
  BHY2.update();
}
