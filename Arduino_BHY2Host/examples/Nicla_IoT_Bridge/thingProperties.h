#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"


const char THING_ID[] = "";

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)

void onTempChange();
void onSecondsChange();

float temp;
int seconds;

void initProperties(){

  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(temp, READWRITE, ON_CHANGE, onTempChange);
  ArduinoCloud.addProperty(seconds, READWRITE, ON_CHANGE, onSecondsChange);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
