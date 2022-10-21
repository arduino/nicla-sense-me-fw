#ifndef BOSCH_SENSORTEC_H_
#define BOSCH_SENSORTEC_H_

#include "Arduino.h"
#include "mbed.h"

#include "BLEHandler.h"


#include "bosch/common/common.h"
#include "sensors/SensorTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "bosch/bhy2.h"
#ifdef __cplusplus
}
#endif

#if BHY2_ENABLE_BLE_BATCH
#define SENSOR_QUEUE_SIZE   (BLE_SENSOR_EVT_BATCH_CNT_MAX + 20)
#define WORK_BUFFER_SIZE    1024
#else
#define SENSOR_QUEUE_SIZE   10
#define WORK_BUFFER_SIZE    2048
#endif

#define MAX_READ_WRITE_LEN 256

enum SensorAckCode {
  SensorAck = 0x0F,
  SensorNack = 0x00
};

class BoschSensortec {
public:
  BoschSensortec();
  virtual ~BoschSensortec();

  // sketch-side API
  bool begin(); 
  void update();
  void configureSensor(SensorConfigurationPacket& config);
  int configureSensorRange(uint8_t id, uint16_t range);
  void getSensorConfiguration(uint8_t id, SensorConfig& virt_sensor_conf);

  void printSensors();
  bool hasSensor(uint8_t sensorId);

  uint8_t availableSensorData();
  uint8_t availableLongSensorData();
  bool readSensorData(SensorDataPacket &data);
  bool readLongSensorData(SensorLongDataPacket &data);

  // ANNA <-> BOSCH interface
  void addSensorData(SensorDataPacket &sensorData);
  void addLongSensorData(SensorLongDataPacket &sensorData);

  uint8_t acknowledgment();

private:
  mbed::CircularBuffer<SensorDataPacket, SENSOR_QUEUE_SIZE, uint8_t> _sensorQueue;
  mbed::CircularBuffer<SensorLongDataPacket, SENSOR_QUEUE_SIZE, uint8_t> _longSensorQueue;

  uint8_t _workBuffer[WORK_BUFFER_SIZE];
  uint8_t _acknowledgment;
  
  struct bhy2_dev _bhy2;
  uint8_t _sensorsPresent[32];

private:
  friend class Arduino_BHY2;
  void debug(Stream &stream);
  Stream *_debug;
};

extern BoschSensortec sensortec;

#endif
