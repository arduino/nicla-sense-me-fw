/* 
 * Use this sketch if you want to control nicla from 
 * an external device acting as a host.
 * Here, nicla just reacts to external stimuli coming from
 * the eslov port or through BLE 
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

// Set DEBUG to true in order to enable debug print
#define DEBUG false

void setup()
{
#if DEBUG
  Serial.begin(115200);
  BHY2.debug(Serial);
#endif

  enable3V3LDO();

  BHY2.begin();
}

void loop()
{
  BHY2.update();
}
