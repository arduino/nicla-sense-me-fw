/* 
 * This sketch is used for collecting raw data of BME688, 
   and the data log after conversion with the helper tools can be used in Bosch Sensortec's AI Studio to train an algorithm 
   and generate the corresponding config string for the BSEC2 library which can be later used for gas type classification/scanning
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

SensorBSEC2Collector bsec2Collector(SENSOR_ID_BSEC2_COLLECTOR);

#define CONFIG_BSEC2_USE_DEAULT_HP 1

#if CONFIG_BSEC2_USE_DEAULT_HP
// Default Heater temperature and time base(Recommendation)
const uint16_t BSEC2HP_TEMP[] = {320, 100, 100, 100, 200, 200, 200, 320, 320, 320}; // HP-354 / 
const uint16_t BSEC2HP_DUR[] = {5, 2, 10, 30, 5, 5, 5, 5, 5, 5};  // the duration in steps of 140ms, 5 means 700ms, 2 means 280ms
#else
// customized Heater temperature and time base
const uint16_t BSEC2HP_TEMP[] = {100, 320, 320, 200, 200, 200, 320, 320, 320, 320}; // HP-321 / 
const uint16_t BSEC2HP_DUR[] = {43, 2, 2, 2, 21, 21, 2, 14, 14, 14};  // the duration in steps of 140ms, 5 means 700ms, 2 means 280ms
#endif

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  BHY2.begin();
  sensortec.bhy2_bsec2_setHP((uint8_t*)BSEC2HP_TEMP, sizeof(BSEC2HP_TEMP), (uint8_t*)BSEC2HP_DUR, sizeof(BSEC2HP_DUR)); 
  
  bsec2Collector.begin();
}

void loop()
{
  static auto last_index = 0;

  // Update function should be continuously polled
  BHY2.update();

  if (last_index != bsec2Collector.gas_index()) {
    last_index = bsec2Collector.gas_index();
    Serial.println(String((uint32_t)bsec2Collector.timestamp()) + " " 
              + String(bsec2Collector.temperature()) + " " 
              + String(bsec2Collector.pressure()) + " " 
              + String(bsec2Collector.humidity()) + " " 
              + String(bsec2Collector.gas()) + " " 
              + String(bsec2Collector.gas_index()) 
              );
  }
}
