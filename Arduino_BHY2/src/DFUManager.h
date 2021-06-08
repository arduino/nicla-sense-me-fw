#ifndef DFU_MANAGER_H_
#define DFU_MANAGER_H_

#include "Arduino.h"

#include "SPIFBlockDevice.h"

#include "LittleFileSystem.h"

enum DFUType {
  DFU_INTERNAL,
  DFU_EXTERNAL 
};

enum DFUAckCode {
  DFUAck = 0x0F,
  DFUNack = 0x00
};

enum DFUSource {
  bleDFU,
  eslovDFU
};

struct __attribute__((packed)) DFUPacket {
  uint8_t last: 1;
  union {
    uint16_t index: 15;
    uint16_t remaining: 15;
  };
  uint8_t data[232];
};

class DFUManager {
public: 
  DFUManager();
  virtual ~DFUManager();

  bool begin();
  void processPacket(DFUSource source, DFUType dfuType, const uint8_t* data);
  void closeDfu();

  bool isPending();
  DFUSource dfuSource();

  uint8_t acknowledgment();


private:
  static SPIFBlockDevice _bd;
  static mbed::LittleFileSystem _fs;
  FILE* _target;

  uint8_t _acknowledgment;
  bool _transferPending;
  DFUSource _dfuSource;

private:
  friend class Arduino_BHY2;
  void debug(Stream &stream);
  Stream *_debug;
};

extern DFUManager dfuManager;

#endif
