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
  //randomSeed(analogRead(0));
  ///////

  // debug port
  Serial.begin(115200);

  BHY2.debug(Serial);
  BHY2.begin();

  // debug purpose
  //#define BHY2_SENSOR_ID_MAX                      UINT8_C(200)
  //SensorConfigurationPacket config;
  //config.sampleRate = 1;
  //config.latency = 0;
  //for (uint8_t i = 1; i < BHY2_SENSOR_ID_MAX; i++)
  //{
    //config.sensorId = i;
    //BHY2.configureSensor(&config);
  //}
}

void loop()
{
  // test purpose: inject random sensor data
  //static auto time = millis();
  //if (millis() - time >= 200) {
    //injectSensorData();
    //time = millis();
  //}
  
  BHY2.update();
}
