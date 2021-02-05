#pragma once

#include "Arduino.h"

class ParsedData {
public:
  uint64_t data;

  float getFloat(uint8_t index);

  uint8_t getUint8(uint8_t index);
  uint16_t getUint16(uint8_t index);
  uint32_t getUint32(uint8_t index);

  int8_t getInt8(uint8_t index);
  int16_t getInt16(uint8_t index);
  int32_t getInt32(uint8_t index);
};

struct DataXYZ
{
  int16_t x;
  int16_t y;
  int16_t z;
};

