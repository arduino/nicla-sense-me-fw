#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "thingProperties.h"

#include "Arduino_BHY2_HOST.h"
Sensor tempSensor(SENSOR_ID_TEMP);

int printTime = 0;

#define DEBUG false

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  while(!Serial) {}

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
 
#if DEBUG
  BHY2_HOST.debug(Serial);
#endif

  while (!BHY2_HOST.begin(BLE_BRIDGE)) {}
  Serial.println("NICLA device found!");

  tempSensor.configure(1, 0, BLE_BRIDGE);

  printTime = millis();
}

void loop() {
  BHY2_HOST.update();
  
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
