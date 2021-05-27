#ifndef ARDUINO_BHY2_H_
#define ARDUINO_BHY2_H_

#include "Arduino.h"
#include "sensors/SensorTypes.h"
#include "sensors/DataParser.h"

#include "sensors/Sensor.h"
#include "sensors/SensorOrientation.h"
#include "sensors/SensorXYZ.h"
#include "sensors/SensorQuaternion.h"

#include "sensors/SensorID.h"

class Arduino_BHY2 {
public:
  Arduino_BHY2();
  virtual ~Arduino_BHY2();

  // Necessary API. Update function should be continuously polled 
  bool begin();
  void update(); // remove this to enforce a sleep
  void update(unsigned long ms); // Update and then sleep
  void delay(unsigned long ms); // to be used instead of arduino delay()

  // API for using the bosch sensortec from sketch
  void configureSensor(SensorConfigurationPacket& config);
  void configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency);
  void addSensorData(SensorDataPacket &sensorData);
  uint8_t availableSensorData();
  bool readSensorData(SensorDataPacket &data);
  bool hasSensor(uint8_t sensorId);

  void parse(SensorDataPacket& data, DataXYZ& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);

  void checkEslovInt();
  void setLDOTimeout(int time);

  void debug(Stream &stream);

private:
  Stream *_debug;

  void pingI2C();
  int _pingTime;
  int _timeout;
  int _startTime;
  bool _timeoutExpired;
  bool _eslovActive;
};

extern Arduino_BHY2 BHY2;

#endif
