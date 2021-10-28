#include "arduino_secrets.h"

const char THING_ID[] = "";

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)

void onTemperatureChange();
void onSecondsChange();

float temperature;
int seconds;

void initProperties(){

  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(temperature, READWRITE, ON_CHANGE, onTemperatureChange);
  ArduinoCloud.addProperty(seconds, READWRITE, ON_CHANGE, onSecondsChange);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);