#include <Arduino.h>
#include "src/BoschSensortec/BoschSensortec.h"
#include "src/EslovHandler.h"
#include "src/BLEHandler.h"


// test purpose function 
void injectSensorData()
{
  SensorDataPacket sensorData;
  sensorData.sensorId = random(10);
  sensorData.data = random(255);
  sensorData.size = 1;

  sensortec.addSensorData(sensorData);
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

  sensortec.begin();
  eslovHandler.begin();
  bleHandler.begin();
}

void loop()
{
  // test purpose: inject random sensor data
  static auto time = millis();
  if (millis() - time >= 200) {
    injectSensorData();
    time = millis();
  }
  
  bleHandler.update();
  sensortec.update();
}
