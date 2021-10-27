#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "thingProperties.h"

#include "Arduino_BHY2Host.h"
Sensor tempSensor(SENSOR_ID_TEMP);

int printTime = 0;

#define DEBUG false

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  while(!Serial) {}
#ifndef TARGET_PORTENTA_H7
  Serial.println("Unsupported board!");
  while(1);
#endif

#if DEBUG
  BHY2.debug(Serial);
#endif

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  if (!ArduinoCloud.begin(ArduinoIoTPreferredConnection)) {
    Serial.println("ArduinoCloud.begin FAILED!");
  }

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  
  while(!ArduinoCloud.connected()) {
    ArduinoCloud.update();
    delay(10);
  }
  
  Serial.println("Configuring Nicla...");

  while (!BHY2Host.begin(false, NICLA_VIA_BLE)) {}
  Serial.println("NICLA device found!");

  tempSensor.begin();

  printTime = millis();
}

void loop() {
  BHY2Host.update(100);
  
  if (millis() - printTime > 1000) {
    printTime = millis();
    seconds = millis()/1000;
    temperature = tempSensor.value();

    Serial.print("Temp: ");
    Serial.println(temperature, 3);

  }

  ArduinoCloud.update(); 
  
}

void onTemperatureChange() {
}

void onSecondsChange() {
}
