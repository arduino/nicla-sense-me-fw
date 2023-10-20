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
  setup_interfaces(false, BHY2_SPI_INTERFACE);
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
    for (int i = 0; i < (int)sizeof(presentBuff); i++) {
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

int BoschSensortec::configureSensorRange(uint8_t id, uint16_t range)
{
  auto ret = bhy2_set_virt_sensor_range(id, range, &_bhy2);
  if (ret == BHY2_OK) {
    return 1;
  }
  return 0;
}

void BoschSensortec::getSensorConfiguration(uint8_t id, SensorConfig& virt_sensor_conf)
{
  bhy2_get_virt_sensor_cfg(id, &virt_sensor_conf, &_bhy2);
}

int8_t BoschSensortec::bhy2_setParameter(uint16_t param, const uint8_t *buffer, uint32_t length) {
  return bhy2_set_parameter(param, buffer, length, &_bhy2);
}

int8_t BoschSensortec::bhy2_getParameter(uint16_t param, uint8_t *buffer, uint32_t length, uint32_t *actual_len) {
  return bhy2_get_parameter(param, buffer, length, actual_len, &_bhy2);
}

void BoschSensortec::bhy2_bsec2_setConfigString(const uint8_t * buffer, uint32_t length) {
    const uint8_t BSEC2_CMD_ENA_WR[] = {0x01,0x00,0x00,0x00};
    const uint8_t BSEC2_CMD_WR_CFG[] = {0x00,0x00,0x00,0x00};
    const uint16_t BSEC2_CFG_PARAM_ID_1 = 0X0802;
    const uint16_t BSEC2_CFG_PARAM_ID_2 = 0X0801;

    const uint16_t BLOCK_SIZE = 4;
    uint16_t wr_cnt = length / BLOCK_SIZE;
    uint8_t remain[BLOCK_SIZE] = {0};
    uint16_t i=0;

    bhy2_setParameter(BSEC2_CFG_PARAM_ID_1, BSEC2_CMD_ENA_WR, sizeof(BSEC2_CMD_ENA_WR)/sizeof(BSEC2_CMD_ENA_WR[0]));

    for (i=0; i<wr_cnt; i++){
        bhy2_setParameter(BSEC2_CFG_PARAM_ID_2, &buffer[i*BLOCK_SIZE], BLOCK_SIZE);
    }

    if (length % BLOCK_SIZE != 0){
        memcpy(remain, &buffer[i*BLOCK_SIZE], length % BLOCK_SIZE);
        bhy2_setParameter(BSEC2_CFG_PARAM_ID_2, remain, BLOCK_SIZE);
    }

    bhy2_setParameter(BSEC2_CFG_PARAM_ID_1, BSEC2_CMD_WR_CFG, sizeof(BSEC2_CMD_ENA_WR)/sizeof(BSEC2_CMD_ENA_WR[0]));
}

void BoschSensortec::bhy2_bsec2_setHP(const uint8_t * hp_temp, uint8_t hp_temp_len, const uint8_t * hp_dur, uint8_t hp_dur_len) {
    const uint16_t BSEC2_CFG_PARAM_ID_1 = 0X0803;
    const uint16_t BSEC2_CFG_PARAM_ID_2 = 0X0804;

    bhy2_setParameter(BSEC2_CFG_PARAM_ID_1, hp_temp, hp_temp_len);
    bhy2_setParameter(BSEC2_CFG_PARAM_ID_2, hp_dur, hp_dur_len);
}

void BoschSensortec::bsecSetBoardTempOffset(float temp_offset)
{
    const uint16_t BSEC2_CFG_PARAM_ID_1 = 0X0805;
    uint8_t temp_buf[4];
    memcpy(temp_buf, (uint8_t*)&temp_offset, sizeof(temp_buf));
    bhy2_setParameter(BSEC2_CFG_PARAM_ID_1, temp_buf, sizeof(temp_buf));
}

uint8_t BoschSensortec::availableSensorData()
{
  return _sensorQueue.size();
}

uint8_t BoschSensortec::availableLongSensorData()
{
  return _longSensorQueue.size();
}

bool BoschSensortec::readSensorData(SensorDataPacket &data)
{
  return _sensorQueue.pop(data);
}

bool BoschSensortec::readLongSensorData(SensorLongDataPacket &data)
{
  return _longSensorQueue.pop(data);
}

void BoschSensortec::addSensorData(SensorDataPacket &sensorData)
{
  // Overwrites oldest data when fifo is full
  _sensorQueue.push(sensorData);
  // Alternative: handle the full queue by storing it in flash
  sensorManager.process(sensorData);
}

void BoschSensortec::addLongSensorData(SensorLongDataPacket &sensorData)
{
  // Overwrites oldest data when fifo is full
  _longSensorQueue.push(sensorData);
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
