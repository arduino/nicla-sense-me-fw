#ifndef DFU_CHANNEL_H_
#define DFU_CHANNEL_H_

#include "Arduino.h"
#include "FlashIAPBlockDevice.h"
#include "LittleFileSystem.h"

enum DFUType {
  DFU_INTERNAL,
  DFU_EXTERNAL 
};

struct __attribute__((packed)) DFUPacket {
  uint8_t last: 1;
  union {
    uint16_t index: 15;
    uint16_t remaining: 15;
  };
  uint8_t data[64];
};

class DFUChannelClass {
  public: 
    DFUChannelClass();
    ~DFUChannelClass();

    void setup();
    void processPacket(DFUType dfuType, const uint8_t* data);

  private:
    static FlashIAPBlockDevice _bd;
    static mbed::LittleFileSystem _fs;
    FILE* _target;
};

extern DFUChannelClass DFUChannel;



#endif
