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

class BoschParser {
public:
  static void convertTime(uint64_t time_ticks, uint32_t *s, uint32_t *ns);
  static void parseData(const struct bhy2_fifo_parse_data_info *fifoData, void *arg);
  static void parseMetaEvent(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
  static void parseGeneric(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);
  static void parseDebugMessage(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref);

private:
  friend class Arduino_BHY2;
  static void debug(Stream &stream);
  static Stream *_debug;
};

#endif
