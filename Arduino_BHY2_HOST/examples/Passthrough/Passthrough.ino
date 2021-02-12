/* 
 * This sketch allows to control unisense from a PC.
 * Upload this sketch on an arduino board connected to unisense 
 * through the eslov connector. Then connect the same arduino board
 * to your PC.
 * Now you can use the arduino-bhy tool, written in golang, 
 * to control unisense from either the PC command line or from a web page.
*/

#include "Arduino.h"
#include "Arduino_BHY2_HOST.h"

// Set DEBUG to true in order to enable debug print
#define DEBUG false

void setup()
{
#if DEBUG
  // When the passthrough is enabled, Serial is busy ->
  // so it cannot be used for debugging. Serial1 is used instead
  Serial1.begin(115200);
  BHY2_HOST.debug(Serial1);
#endif

  BHY2_HOST.begin(true);
}

void loop()
{
  BHY2_HOST.update();
}
