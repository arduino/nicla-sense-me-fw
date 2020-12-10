#include <Arduino.h>
#include "src/BoschSensortec/BoschSensortec.h"
#include "src/EslovHandler.h"
#include "src/BLEHandler.h"

// Initialize bosch sensortec
// Initialize and link channels 
void setup()
{
  sensortec.begin();

  // debug port
  Serial.begin(9600);           
  while(!Serial);

  eslovHandler.begin();
  bleHandler.begin();
}

void loop()
{
  bleHandler.update();
  sensortec.update();
}
