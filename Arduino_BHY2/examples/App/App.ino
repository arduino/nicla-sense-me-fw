/* 
 * Use this sketch if you want to control nicla from 
 * an external device acting as a host.
 * Here, nicla just reacts to external stimuli coming from
 * the eslov port or through BLE 
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

// Set DEBUG to true in order to enable debug print
#define DEBUG false

int start_time;

void setup()
{

  CoreDebug->DEMCR = 0;
  NRF_CLOCK->TRACECONFIG = 0;

#if DEBUG
  Serial.begin(115200);
  BHY2.debug(Serial);
#endif

  BHY2.begin();
}

void loop()
{
  BHY2.update();
  BHY2.ping_i2c0();
}
