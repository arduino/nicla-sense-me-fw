#include "BoschSensortec.h"

BoschSensortec::BoschSensortec() : 
  _hasNewData(false), 
  _sensorQueueFirst(0),
  _sensorQueueLast(0),
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

void BoschSensortec::configureSensor(SensorConfigurationPacket *config)
{
  bhy2_register_fifo_parse_callback(config->sensorId, parseBhyData, NULL, &_bhy2);
  bhy2_update_virtual_sensor_list(&_bhy2);
  bhy2_set_virt_sensor_cfg(config->sensorId, config->sampleRate, config->latency, &_bhy2);
}

uint8_t BoschSensortec::availableSensorData()
{
  return _sensorQueueLast - _sensorQueueFirst;
}

SensorDataPacket* BoschSensortec::readSensorData()
{
  if (_sensorQueueLast == _sensorQueueFirst) {
    //error: queue is empty
  }
  SensorDataPacket* sensorData = &_sensorQueue[_sensorQueueFirst++];
  if (_sensorQueueFirst == _sensorQueueLast) {
    _sensorQueueFirst = 0;
    _sensorQueueLast = 0;
  }
  return sensorData;
}

// Retrieve data and store it in the Sensortec's queue
void BoschSensortec::parseBhyData(const struct bhy2_fifo_parse_data_info *fifoData, void *arg)
{
  sensortec.addSensorData(fifoData);
}

void BoschSensortec::addSensorData(const struct bhy2_fifo_parse_data_info *fifoData)
{
  if (_sensorQueueLast < SENSOR_QUEUE_SIZE) {
    SensorDataPacket sensorData = _sensorQueue[_sensorQueueLast++];

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
