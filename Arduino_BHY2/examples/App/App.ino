/* 
 * Use this sketch if you want to control nicla from 
 * an external device acting as a host.
 * Here, nicla just reacts to external stimuli coming from
 * the eslov port or through BLE 
 * 
 * NOTE: Remember to choose your Nicla configuration! 
 * If Nicla is used as a Shield, provide the NICLA_AS_SHIELD parameter.
 * If you want to enable just one between I2C and BLE,
 * use NICLA_I2C or NICLA_BLE parameters.
 *
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
