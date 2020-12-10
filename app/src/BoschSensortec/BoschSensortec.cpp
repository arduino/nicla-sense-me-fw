#include "BoschSensortec.h"

BoschSensortec::BoschSensortec() : 
  _hasNewData(false), 
  _savedConfig(NULL) 
{
}

BoschSensortec::~BoschSensortec() 
{
}

void BoschSensortec::interruptHandler() 
{
  sensortec._hasNewData = true; 
}

void BoschSensortec::begin()
{
  bhy2_init(BHY2_SPI_INTERFACE, bhy2_spi_read, bhy2_spi_write, bhy2_delay_us, MAX_READ_WRITE_LEN, NULL, &_bhy2);
}

bool BoschSensortec::hasNewData()
{
  return _hasNewData;
}

void BoschSensortec::update()
{
  if (_hasNewData) {
    // Retrieve data and store it in the queue
    // also handle the queue by storing it in flash if full
    retrieveData();
  }
}


BoschSensortec sensortec;
