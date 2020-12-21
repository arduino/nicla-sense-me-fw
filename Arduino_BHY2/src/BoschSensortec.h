#ifndef BOSCH_SENSORTEC_H_
#define BOSCH_SENSORTEC_H_

#include "mbed.h"

#include "channel.h"
#include "SensorTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "bosch/bhy2.h"
#ifdef __cplusplus
}
#endif

#define SENSOR_QUEUE_SIZE   10
#define WORK_BUFFER_SIZE    2048

// This will use the BHY functions for configuring sensors and retrieving data
class BoschSensortec {
public:
  BoschSensortec();
  virtual ~BoschSensortec();

  // sketch-side API
  void begin(); 
  void update();
  void configureSensor(SensorConfigurationPacket *config);

  uint8_t availableSensorData();
  bool readSensorData(SensorDataPacket &data);

  // ANNA <-> BOSCH interface
  static void interruptHandler();
  static void parseBhyData(const struct bhy2_fifo_parse_data_info *data, void *arg);
  void addSensorData(const SensorDataPacket &sensorData);

private:
  bool _hasNewData;

  mbed::CircularBuffer<SensorDataPacket, SENSOR_QUEUE_SIZE, uint8_t> _sensorQueue;

  uint8_t _workBuffer[WORK_BUFFER_SIZE];

  SensorConfigurationPacket* _savedConfig;
  
  struct bhy2_dev _bhy2;
};

extern BoschSensortec sensortec;

#endif
