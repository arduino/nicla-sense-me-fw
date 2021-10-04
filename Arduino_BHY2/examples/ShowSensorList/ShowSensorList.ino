/* 
 * This sketch shows a list of virtual sensors supported on the Nicla Sense Board
 * see more details about the definition and specs of virtual sensors in the BHI260AP datasheet on Bosch Sensortec website
 
 * Dependencies: 
 * librares: Arduino_BHY2
*/


#include "Arduino.h"
#include "Arduino_BHY2.h"

void showSensorList() 
{
    Serial.println("\n");
    Serial.println("Virtual sensors supported on this board:");
    for (int id = 0; id < 255; id++) {
        if (BHY2.hasSensor(id)){
            Serial.println(id);
        }
    }

    Serial.println("end of the list!");
    Serial.flush();
}

void setup()
{
  Serial.begin(115200);
  BHY2.begin();
  
  showSensorList();
}

void loop()
{
  delay(1000);
}
