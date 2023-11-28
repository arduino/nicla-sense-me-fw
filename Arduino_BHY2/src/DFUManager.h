#ifndef DFU_MANAGER_H_
#define DFU_MANAGER_H_

#include "Arduino.h"

#include "SPIFBlockDevice.h"

#include "LittleFileSystem.h"

/**
 * @brief Enumerator for selecting device for DFU
 * 
 */
enum DFUType {
  DFU_INTERNAL,    /*!< ANNA-B112 */
  DFU_EXTERNAL     /*!< BHY2 */
};

/**
 * @brief Acknowledgment handler for the DFU
 * 
 */
enum DFUAckCode {
  DFUAck = 0x0F,   /*!< Acknowledgment */
  DFUNack = 0x00   /*!< Negative Acknowledgment */
};

/**
 * @brief Enumeration to define source of DFU data stream
 * 
 */
enum DFUSource {
  bleDFU,           /*!< BLE */
  eslovDFU          /*!< ESLOV */
};

/**
 * @brief Structure of a DFU data packet
 * 
 */
struct __attribute__((packed)) DFUPacket {
  uint8_t last: 1;          /*!< Bit to indicate last packet */
  union {
    uint16_t index: 15;     /*!< Current packet index */
    uint16_t remaining: 15; /*!< Remaining packet index */
  };
  uint8_t data[232];        /*!< 232 byte array to store data */
};

/**
 * @brief Class for handling Device Firmware Upgrade (DFU) of both the ANNA-B112 and the BHY2 chip
 * 
 */
class DFUManager {
public: 
  DFUManager();
  virtual ~DFUManager();

  /**
   * @brief Mount file system
   * 
   * @return true   File system mounted successfully 
   * @return false  Error in mounting file system
   */
  bool begin();
  /**
   * @brief Method for processing packet data
   * 
   * @param source  Selection of DFU data source
   * @param dfuType Choose between ANNA-B112 and BHY260
   * @param data    datastream
   */
  void processPacket(DFUSource source, DFUType dfuType, const uint8_t* data);
  /**
   * @brief Set _transferPending to False
   * 
   */
  void closeDfu();

  /**
   * @brief Return state of _transferPending 
   * 
   * @return true _transferPending is true
   * @return false _transferPending is false
   */
  bool isPending();
  DFUSource dfuSource();
  /**
   * @brief Reset DFUAckCode flag
   * 
   * @return uint8_t 
   */
  uint8_t acknowledgment();


private:
  static SPIFBlockDevice _bd;
  static mbed::LittleFileSystem _fs;
  FILE* _target;

  uint8_t _acknowledgment;
  bool _transferPending;
  DFUSource _dfuSource;

private:
  /**
   * @brief The Arduino_BHY2 class can access both private and public methods of DFUManager
   * 
   */
  friend class Arduino_BHY2;
  void debug(Stream &stream);
  Stream *_debug;
};

/**
 * @brief The DFUManager class can be externally linked to as dfuManager in your sketch
 * 
 */
extern DFUManager dfuManager;

#endif
