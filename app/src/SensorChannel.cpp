#include "SensorChannel.h"

SensorChannelClass::SensorChannelClass()
{
}

SensorChannelClass::~SensorChannelClass()
{
}

uint8_t SensorChannelClass::processPacket(SensorPacketType type, const uint8_t* data)
{
  uint8_t returnValue = 1;

  if (type == SENSOR_CONFIG_PACKET) {
    // Send config to sensortec class
    SensorConfigurationPacket* packet = (SensorConfigurationPacket*) data;

  } else if (type == SENSOR_REQUEST_PACKET) {
    // Query sensors from sensortec class 
    // return number of data to be read
  }

  return returnValue;
}

uint8_t SensorChannelClass::getNextDataPacket(uint8_t* data)
{
  SensorDataPacket* packet = (SensorDataPacket*) data;
  // Retrieve sensor data from sensortec object and populate the packet

  return sizeof(SensorDataPacket);
}

SensorChannelClass SensorChannel;
