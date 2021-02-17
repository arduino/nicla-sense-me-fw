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
  auto ret = bhy2_init(BHY2_SPI_INTERFACE, bhy2_spi_read, bhy2_spi_write, bhy2_delay_us, MAX_READ_WRITE_LEN, NULL, &_bhy2);
  if (_debug) _debug->println(get_api_error(ret));

  // Print bhi status 
  uint8_t stat;
  ret = bhy2_get_boot_status(&stat, &_bhy2);
  if (_debug) {
    _debug->println(get_api_error(ret));
    _debug->print("Boot status: ");
    _debug->println(stat, HEX);
  }
  ret = bhy2_get_host_interrupt_ctrl(&stat, &_bhy2);
  if (_debug) {
    _debug->println(get_api_error(ret));
    _debug->print("Interrupt ctrl: ");
    _debug->println(stat, HEX);
  }
  ret = bhy2_get_host_intf_ctrl(&stat, &_bhy2);
  if (_debug) {
    _debug->println(get_api_error(ret));
    _debug->print("Interface ctrl: ");
    _debug->println(stat, HEX);
  }

  bhy2_register_fifo_parse_callback(BHY2_SYS_ID_META_EVENT, BoschParser::parseMetaEvent, NULL, &_bhy2);
  bhy2_register_fifo_parse_callback(BHY2_SYS_ID_META_EVENT_WU, BoschParser::parseMetaEvent, NULL, &_bhy2);
  bhy2_register_fifo_parse_callback(BHY2_SYS_ID_DEBUG_MSG, BoschParser::parseDebugMessage, NULL, &_bhy2);

  ret = bhy2_get_and_process_fifo(_workBuffer, WORK_BUFFER_SIZE, &_bhy2);
  if (_debug) _debug->println(get_api_error(ret));

  // All sensors' data are handled in the same generic way
  for (uint8_t i = 1; i < BHY2_SENSOR_ID_MAX; i++) {
    bhy2_register_fifo_parse_callback(i, BoschParser::parseData, NULL, &_bhy2);
  }

  bhy2_update_virtual_sensor_list(&_bhy2);
}

void BoschSensortec::configureSensor(SensorConfigurationPacket& config)
{
  auto ret = bhy2_set_virt_sensor_cfg(config.sensorId, config.sampleRate, config.latency, &_bhy2);
  if (_debug) _debug->println(get_api_error(ret));
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
  // Overwrites oldest data when fifo is full 
  _sensorQueue.push(sensorData);
  // Alternative: handle the full queue by storing it in flash 
}

void BoschSensortec::update()
{
  if (get_interrupt_status()) {
    auto ret = bhy2_get_and_process_fifo(_workBuffer, WORK_BUFFER_SIZE, &_bhy2);
    if (_debug) _debug->println(get_api_error(ret));
  }
}

void BoschSensortec::debug(Stream &stream)
{
  _debug = &stream;
}


BoschSensortec sensortec;
