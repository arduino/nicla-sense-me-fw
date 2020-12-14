#include "SensorChannel.h"

SensorChannel::SensorChannel()
{
}

SensorChannel::~SensorChannel()
{
}

uint8_t SensorChannel::processPacket(SensorPacketType type, const uint8_t* data)
{
  uint8_t returnValue = 1;

  // This check could be done by the ble/eslov receiver. In such case, bypass this class

  if (type == SENSOR_CONFIG_PACKET) {
    // Send config to sensortec class
    SensorConfigurationPacket* packet = (SensorConfigurationPacket*) data;
    sensortec.configureSensor(packet);

  } else if (type == SENSOR_REQUEST_PACKET) {
    // Query sensors from sensortec class 
    // return number of data to be read
    returnValue = sensortec.availableSensorData();
  }

  return returnValue;
}

// SensorChannel could bypassed
SensorDataPacket* SensorChannel::readSensorData()
{
  return sensortec.readSensorData();
}

SensorChannel sensorChannel;
