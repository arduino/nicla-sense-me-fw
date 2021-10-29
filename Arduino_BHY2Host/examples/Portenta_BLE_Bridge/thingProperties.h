#include "arduino_secrets.h"

const char THING_ID[] = "";

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)

float temperature;
int seconds;

void initProperties(){

  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(temperature, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(seconds, READ, 1 * SECONDS, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);