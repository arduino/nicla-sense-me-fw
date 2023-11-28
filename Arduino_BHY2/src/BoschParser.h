#ifndef BOSCH_PARSER_H_
#define BOSCH_PARSER_H_

#include "Arduino.h"
#include "mbed.h"

#include "bosch/common/common.h"
#include "sensors/SensorTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "bosch/bhy2.h"
#ifdef __cplusplus
}
#endif

/**
 * @brief Class for parsing data obtained from the Bosch BHY2 libraries
 * 
 */
class BoschParser {
public:
  /**
  * @brief Convert ticks of the 64 MHz oscillator to time
  * 
  * @param time_ticks Timestamp of the 64 MHz oscillator in ticks
  * @param s          Timestamp in seconds
  * @param ns         Timestamp in nanoseconds
  */
  static void convertTime(uint64_t time_ticks, uint32_t *s, uint32_t *ns);
  /**
   * @brief Data payload is stored in FIFO buffer
   * 
   * @param fifoData Pointer to data to be parsed
   * @param arg      Reserved for future use.
   */
  static void parseData(const struct bhy2_fifo_parse_data_info *fifoData, void *arg);
  /**
   * @brief Parse Meta Events from the Bosch BHI260 Sensor
   * 
   * @param callback_info Pointer containing information to be parsed
   * @param callback_ref Reserved for future use
   */
  static void parseMetaEvent(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
  /**
   * @brief Extracts the timestamp information and prints it with the sensor ID. Also prints contents of callback_info.
   * 
   * @param callback_info Pointer containing information to be parsed
   * @param callback_ref  Reserved for future use
   */
  static void parseGeneric(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
  /**
   * @brief Parse debug message
   * 
   * @param callback_info Pointer containing information to be parsed
   * @param callback_ref  Reserved for future use
   */
  static void parseDebugMessage(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);

private:
  /**
   * @brief The Arduino_BHY2 class can access both private and public methods of BoschParser
   * 
   */
  friend class Arduino_BHY2;
  static void debug(Stream &stream);
  static Stream *_debug;
};

#endif
