/* 
 * Use this sketch if you want to control nicla from 
 * an external device acting as a host.
 * Here, nicla just reacts to external stimuli coming from
 * the eslov port or through BLE 
 * 
 * NOTE: if Nicla is used as a Shield on top of a MKR board,
 * please use BHY2.begin(NICLA_AS_SHIELD)
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

// Set DEBUG to true in order to enable debug print
#define DEBUG false

void setup()
{
#if DEBUG
  Serial.begin(115200);
  BHY2.debug(Serial);
#endif

  BHY2.begin();
}

void loop()
{
  // Update and then sleep
  BHY2.update(100);
}
