#ifndef ARDUINO_BHY2_HOST_H_
#define ARDUINO_BHY2_HOST_H_

#include "Arduino.h"
#include "sensors/SensorTypes.h"
#include "sensors/DataParser.h"

#include "sensors/SensorClass.h"
#include "sensors/SensorOrientation.h"
#include "sensors/SensorXYZ.h"
#include "sensors/SensorQuaternion.h"
#include "sensors/SensorActivity.h"
#include "sensors/Sensor.h"

#include "sensors/SensorID.h"

enum NiclaWiring {
  NICLA_VIA_ESLOV = 0,
  NICLA_AS_SHIELD
};

class Arduino_BHY2Host {
public:
  Arduino_BHY2Host();
  virtual ~Arduino_BHY2Host();

  // Necessary API. Update function should be continuously polled if PASSTHORUGH is ENABLED
  bool begin(bool passthrough = false, NiclaWiring niclaConnection = NICLA_VIA_ESLOV);
  void update();

  // Functions for controlling the BHY when PASSTHROUGH is DISABLED
  void configureSensor(SensorConfigurationPacket& config);
  void configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency);
  uint8_t requestAck();
  uint8_t availableSensorData();
  bool readSensorData(SensorDataPacket &data);

  void parse(SensorDataPacket& data, DataXYZ& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);

  void debug(Stream &stream);

private:
  bool _passthrough;
};

extern Arduino_BHY2Host BHY2_HOST;

#endif
