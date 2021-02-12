#ifndef ARDUINO_BHY2_H_
#define ARDUINO_BHY2_H_

#include "Arduino.h"
#include "SensorTypes.h"
#include "DataParser.h"

class Arduino_BHY2 {
public:
  Arduino_BHY2();
  virtual ~Arduino_BHY2();

  // Necessary API. Update function should be continuously polled 
  void begin();
  void update();

  // API for using the bosch sensortec from sketch
  void configureSensor(SensorConfigurationPacket& config);
  void addSensorData(const SensorDataPacket &sensorData);
  uint8_t availableSensorData();
  bool readSensorData(SensorDataPacket &data);

  void parse(SensorDataPacket& data, DataXYZ& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);

  void debug(Stream &stream);
};

extern Arduino_BHY2 BHY2;

#endif
