#include "Arduino_BHY2_HOST.h"

//#include "BLEHandler.h"
#include "EslovHandler.h"

Arduino_BHY2_HOST::Arduino_BHY2_HOST() :
  _passthrough(false)
{
}

Arduino_BHY2_HOST::~Arduino_BHY2_HOST()
{
}

void Arduino_BHY2_HOST::begin(bool passthrough)
{
  //bleHandler.begin();
  _passthrough = passthrough;
  eslovHandler.begin(passthrough);
}

void Arduino_BHY2_HOST::update()
{
  //bleHandler.update();
  if (_passthrough){
    eslovHandler.update();
  }
}

void Arduino_BHY2_HOST::configureSensor(SensorConfigurationPacket *config)
{
  eslovHandler.writeConfigPacket(config);
}

uint8_t Arduino_BHY2_HOST::availableSensorData()
{
  return eslovHandler.requestAvailableData();
}

bool Arduino_BHY2_HOST::readSensorData(SensorDataPacket &data)
{
  return eslovHandler.requestSensorData(data);
}

void Arduino_BHY2_HOST::debug(Stream &stream)
{
  eslovHandler.debug(stream);
  //BLEHandler::debug(stream);
}

Arduino_BHY2_HOST BHY2_HOST;
