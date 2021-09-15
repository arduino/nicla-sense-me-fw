#include "BoschSensortec.h"
#include "BoschParser.h"
#include "sensors/SensorManager.h"

BoschSensortec::BoschSensortec() : 
  _acknowledgment(SensorNack),
  _debug(NULL)
{
}

BoschSensortec::~BoschSensortec() 
{
}

bool BoschSensortec::begin()
{
  auto ret = bhy2_init(BHY2_SPI_INTERFACE, bhy2_spi_read, bhy2_spi_write, bhy2_delay_us, MAX_READ_WRITE_LEN, NULL, &_bhy2);
  if (_debug) _debug->println(get_api_error(ret));
  if (ret != BHY2_OK) return false; 

  bhy2_soft_reset(&_bhy2);

  // Print bhi status 
  uint8_t stat = 0;
  //delay(1000);
  ret = bhy2_get_boot_status(&stat, &_bhy2);
  if (_debug) {
    _debug->println(get_api_error(ret));
    _debug->print("Boot status: ");
    _debug->println(stat, HEX);
  }

  ret = bhy2_boot_from_flash(&_bhy2);
  if (_debug) _debug->println(get_api_error(ret));
  if (ret != BHY2_OK) return false; 

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
  bhy2_get_virt_sensor_list(_sensorsPresent, &_bhy2);

  printSensors();

  return true;
}

void BoschSensortec::printSensors() {
  bool presentBuff[256];

  for (uint16_t i = 0; i < sizeof(_sensorsPresent); i++)
  {
      for (uint8_t j = 0; j < 8; j++)
      {
          presentBuff[i * 8 + j] = ((_sensorsPresent[i] >> j) & 0x01);
      }
  }

  if (_debug) {
    _debug->println("Present sensors: ");
    for (int i = 0; i < sizeof(presentBuff); i++) {
      if (presentBuff[i]) {
        _debug->print(i);
        _debug->print(" - ");
        _debug->print(get_sensor_name(i));
        _debug->println();
      }
    }
  }
}

bool BoschSensortec::hasSensor(uint8_t sensorId) {
  int i = sensorId / 8;
  int j = sensorId % 8;
  return ((_sensorsPresent[i] >> j) & 0x01) == 1;
}

void BoschSensortec::configureSensor(SensorConfigurationPacket& config)
{
  auto ret = bhy2_set_virt_sensor_cfg(config.sensorId, config.sampleRate, config.latency, &_bhy2);
  if (_debug) _debug->println(get_api_error(ret));
  if (ret == BHY2_OK) {
    _acknowledgment = SensorAck;
  } else {
    _acknowledgment = SensorNack;
  }
}

uint8_t BoschSensortec::availableSensorData()
{
  return _sensorQueue.size();
}

bool BoschSensortec::readSensorData(SensorDataPacket &data)
{
  return _sensorQueue.pop(data);
}

void BoschSensortec::addSensorData(SensorDataPacket &sensorData)
{
  // Overwrites oldest data when fifo is full 
  _sensorQueue.push(sensorData);
  // Alternative: handle the full queue by storing it in flash 
  sensorManager.process(sensorData);
}

// acknowledgment flag is reset when read
uint8_t BoschSensortec::acknowledgment()
{
  uint8_t ack = _acknowledgment;
  // Reset acknowledgment
  _acknowledgment = SensorNack;
  return ack;
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

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#if BHY2_CFG_DELEGATE_FIFO_PARSE_CB_INFO_MGMT
void bhy2_get_fifo_parse_callback_info_delegate(uint8_t sensor_id,
                                struct bhy2_fifo_parse_callback_table *info,
                                const struct bhy2_dev *dev)
{
    info->callback_ref = NULL;
    if (sensor_id < BHY2_SENSOR_ID_MAX) {
        info->callback = BoschParser::parseData;
    } else {
        switch (sensor_id) {
            case BHY2_SYS_ID_META_EVENT:
            case BHY2_SYS_ID_META_EVENT_WU:
                info->callback = BoschParser::parseMetaEvent;
                break;
            case BHY2_SYS_ID_DEBUG_MSG:
                info->callback = BoschParser::parseDebugMessage;
                break;
            default:
                info->callback = NULL;
        }
    }
}
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */


BoschSensortec sensortec;
