#include "BoschSensortec.h"
#include "BoschParser.h"

BoschSensortec::BoschSensortec() : 
  _savedConfig(NULL),
  _debug(NULL)
{
}

BoschSensortec::~BoschSensortec() 
{
}

void BoschSensortec::begin()
{
  bhy2_init(BHY2_SPI_INTERFACE, bhy2_spi_read, bhy2_spi_write, bhy2_delay_us, MAX_READ_WRITE_LEN, NULL, &_bhy2);
}

void BoschSensortec::configureSensor(SensorConfigurationPacket *config)
{
  bhy2_register_fifo_parse_callback(config->sensorId, BoschParser::parseData, NULL, &_bhy2);
  bhy2_update_virtual_sensor_list(&_bhy2);
  bhy2_set_virt_sensor_cfg(config->sensorId, config->sampleRate, config->latency, &_bhy2);
}

uint8_t BoschSensortec::availableSensorData()
{
  return _sensorQueue.size();
}

bool BoschSensortec::readSensorData(SensorDataPacket &data)
{
  return _sensorQueue.pop(data);
}

void BoschSensortec::addSensorData(const SensorDataPacket &sensorData)
{
  if (!_sensorQueue.full()) {
    _sensorQueue.push(sensorData);
  } else {
    // handle the queue by storing it in flash if full
  }
}

void BoschSensortec::update()
{
  if (get_interrupt_status()) {
    bhy2_get_and_process_fifo(_workBuffer, WORK_BUFFER_SIZE, &_bhy2);
  }
}

void BoschSensortec::debug(Stream &stream)
{
  _debug = &stream;
}


BoschSensortec sensortec;
