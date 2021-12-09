#ifndef ARDUINO_BHY2HOST_H_
#define ARDUINO_BHY2HOST_H_

#include "Arduino.h"
#include "sensors/SensorTypes.h"
#include "sensors/DataParser.h"

#include "sensors/SensorClass.h"
#include "sensors/SensorOrientation.h"
#include "sensors/SensorXYZ.h"
#include "sensors/SensorQuaternion.h"
#include "sensors/SensorBSEC.h"
#include "sensors/SensorActivity.h"
#include "sensors/Sensor.h"

#include "sensors/SensorID.h"

#if defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_SAMD_MKRWIFI1010)
#define __BHY2_HOST_BLE_SUPPORTED__
#include "BLEHandler.h"
#endif

enum NiclaWiring {
  NICLA_VIA_ESLOV = 0,
  NICLA_AS_SHIELD,
  NICLA_VIA_BLE
};

class Arduino_BHY2Host {
public:
  Arduino_BHY2Host();
  virtual ~Arduino_BHY2Host();

  // Necessary API. Update function should be continuously polled if PASSTHORUGH is ENABLED
  bool begin(bool passthrough = false, NiclaWiring niclaConnection = NICLA_VIA_ESLOV);
  void update();
  void update(unsigned long ms); // Update and then sleep

  // Functions for controlling the BHY when PASSTHROUGH is DISABLED
  void configureSensor(SensorConfigurationPacket& config);
  void configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency);
  uint8_t requestAck();
  uint8_t availableSensorData();
  bool readSensorData(SensorDataPacket &data);

  void parse(SensorDataPacket& data, DataXYZ& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);

  NiclaWiring getNiclaConnection();

  void debug(Stream &stream);

private:
  bool _passthrough;
  NiclaWiring _wiring;
  Stream *_debug;
};

extern Arduino_BHY2Host BHY2Host;

#endif
