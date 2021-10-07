#include "thingProperties.h"

#include "Arduino_BHY2Host.h"
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

Sensor tempSensor(SENSOR_ID_TEMP);

void setup() {
  Serial.begin(115200);  
  while(!Serial) {}

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  while(!ArduinoCloud.connected()) {
    ArduinoCloud.update();
    delay(10);
  }
  
  Serial.println("Configuring Nicla...");
#ifdef ARDUINO_ARCH_MBED
  BHY2Host.begin();
#else
  BHY2Host.begin(false, NICLA_AS_SHIELD);
#endif
  tempSensor.begin();
}

void loop() {
  static auto printTime = millis();
  BHY2Host.update();

  if (millis() - printTime > 1000) {
    printTime = millis();
    seconds = millis()/1000;
    temp = tempSensor.value();
    Serial.print("Temp: ");
    Serial.println(temp, 3);
  }
  
  ArduinoCloud.update();
}

void onTempChange() {
}


void onSecondsChange() {
}
