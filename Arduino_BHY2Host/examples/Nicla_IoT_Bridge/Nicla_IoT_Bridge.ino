/* 
 * Upload this sketch to a Host board to use it as an I2C bridge between 
 * Nicla Sense ME and the Arduino Cloud. 
 * Nicla Sense ME board needs to run the basic App.ino sketch and to be plugged
 * as a shield on top of the Host board.
 * 
 * Before uploading this sketch to the Host board:
 * - add your Host device to Arduino Cloud
 * - setup temperature and seconds Things in Arduino Cloud
 * - get the THING_ID and copy it in thingProperties.h
 * - create a Dashboard in Arduino Cloud, like a live chart of the temperature
*/

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
