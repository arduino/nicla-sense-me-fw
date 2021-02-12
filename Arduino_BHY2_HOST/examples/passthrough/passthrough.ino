#include "Arduino.h"
#include "Arduino_BHY2_HOST.h"

void setup()
{
  // With the passthrough enabled Serial is busy ->
  // so it cannot be used for debugging. Serial1 is used instead
  Serial1.begin(115200);
  BHY2_HOST.debug(Serial1);

  BHY2_HOST.begin(true);
}

void loop()
{
  BHY2_HOST.update();
}
