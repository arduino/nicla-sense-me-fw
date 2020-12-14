#include "BoschSensortec.h"

BoschSensortec::BoschSensortec() : 
  _hasNewData(false), 
  _sensorQueueIndex(0),
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

void BoschSensortec::configureSensor(SensorConfigurationPacket *config)
{
  bhy2_register_fifo_parse_callback(config->sensorId, retrieveData, NULL, &_bhy2);
  bhy2_update_virtual_sensor_list(&_bhy2);
  bhy2_set_virt_sensor_cfg(config->sensorId, config->sampleRate, config->latency, &_bhy2);
}

// Retrieve data and store it in the Sensortec's queue
void BoschSensortec::retrieveData(const struct bhy2_fifo_parse_data_info *fifoData, void *arg)
{
  sensortec.addNewData(fifoData);
}

void BoschSensortec::addNewData(const struct bhy2_fifo_parse_data_info *fifoData)
{
  if (_sensorQueueIndex < SENSOR_QUEUE_SIZE) {
    SensorDataPacket sensorData = _sensorQueue[_sensorQueueIndex++];

    sensorData.sensorId = fifoData->sensor_id;
    memcpy(&sensorData.data, fifoData->data_ptr, sizeof(fifoData->data_size));
    sensorData.size = fifoData->data_size;

  } else {
    // handle the queue by storing it in flash if full
  }
}

void BoschSensortec::update()
{
  if (_hasNewData) {
    bhy2_get_and_process_fifo(_workBuffer, WORK_BUFFER_SIZE, &_bhy2);
  }
}


BoschSensortec sensortec;
