#include "DFUManager.h"

#if defined (TARGET_ANNA)
SPIFBlockDevice DFUManager::_bd(SPI_PSELMOSI0, SPI_PSELMISO0,
                     SPI_PSELSCK0, CS_FLASH);
#else
// half the flash (512KB) is dedicated as dfu temporary storage
FlashIAPBlockDevice DFUManager::_bd(0x80000, 0x80000);
#endif

mbed::LittleFileSystem DFUManager::_fs("fs");

DFUManager::DFUManager() :
  _target(NULL),
  _acknowledgment(DFUNack),
  _transferPending(false),
  _debug(NULL)
{
}

DFUManager::~DFUManager()
{
}

void DFUManager::begin()
{
  int err = _fs.mount(&_bd);
  if (err) {
    err = _fs.reformat(&_bd);
    if(_debug) {
      _debug->print("Error mounting file system: ");
      _debug->println(err);
    }
  }
}

void DFUManager::processPacket(DFUType dfuType, const uint8_t* data)
{
  DFUPacket* packet = (DFUPacket*)data;
  _transferPending = true;

  if (_debug) {
    _debug->print("packet: ");
    _debug->println(packet->index);
  }

  if (packet->index == 0) {
    if (dfuType == DFU_INTERNAL) {
      _target = fopen("/fs/ANNA_UPDATE.BIN", "wb");
    } else {
      _target = fopen("/fs/BHY_UPDATE.BIN", "wb");
    }

    if(_debug) {
      bool target_found = (_target != NULL);
      _debug->print("target_found = ");
      _debug->println(target_found);
    }
  }

  if (_target != NULL) {
    int ret = fwrite(&packet->data, packet->last ? packet->remaining : sizeof(packet->data), 1, _target);
    // Set the acknowledgment flag according to the write return value
    if(_debug) {
      _debug->print("ret: ");
      _debug->println(ret);
    }
    if (ret > 0) _acknowledgment = DFUAck;
    else _acknowledgment = DFUNack;
  }

  if (packet->last) {
    if(_debug) {
      _debug->print("Last packet received. Remaining: ");
      _debug->println(packet->remaining);
    }
    if (_acknowledgment == DFUAck) {
      fclose(_target);
      _target = NULL;
      _transferPending = false;
    }
  }
}

bool DFUManager::isPending()
{
  return _transferPending;
}

// acknowledgment flag is reset when read
uint8_t DFUManager::acknowledgment()
{
  uint8_t ack = _acknowledgment;
  // Reset acknowledgment
  _acknowledgment = DFUNack;
  return ack;
}

void DFUManager::debug(Stream &stream)
{
  _debug = &stream;
}

DFUManager dfuManager;
