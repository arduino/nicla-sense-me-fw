#ifndef DFU_TYPES_H_
#define DFU_TYPES_H_

#include "Arduino.h"

struct __attribute__((packed)) DFUPacket {
  uint8_t last: 1;
  union {
    uint16_t index: 15;
    uint16_t remaining: 15;
  };
  uint8_t data[232];
};

#endif
