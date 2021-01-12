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
  _acknowledgment(DFUNack)
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
  }
}

void DFUManager::processPacket(DFUType dfuType, const uint8_t* data)
{
  DFUPacket* packet = (DFUPacket*)data;
  //Serial.print("packet: ");
  //Serial.println(packet->index);

  if (packet->index == 0) {
    if (dfuType == DFU_INTERNAL) {
      _target = fopen("/fs/ANNA_UPDATE.BIN", "wb");
    } else {
      _target = fopen("/fs/UPDATE.BIN", "wb");
    }
  }

  if (_target != NULL) {
    int ret = fwrite(&packet->data, packet->last ? packet->remaining : sizeof(packet->data), 1, _target);
    //Serial.println(ret);
    // Set the acknowledgment flag according to the write return value
    if (ret > 0) _acknowledgment = DFUAck;
    else _acknowledgment = DFUNack;
  }

  if (packet->last) {
    fclose(_target);
    _target = NULL;
    if (dfuType == DFU_INTERNAL) {
      // reboot
      //Serial.println("done");
    } else {
      //apply bosch update
      //Serial.println("done");
    }
  }
}

// acknowledgment flag is reset when read
uint8_t DFUManager::acknowledgment()
{
  uint8_t ack = _acknowledgment;
  // Reset acknowledgment
  _acknowledgment = DFUNack;
  return ack;
}

DFUManager dfuManager;
