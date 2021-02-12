#ifndef ARDUINO_BHY2_HOST_H_
#define ARDUINO_BHY2_HOST_H_

#include "Arduino.h"
#include "SensorTypes.h"
#include "DataParser.h"

class Arduino_BHY2_HOST {
public:
  Arduino_BHY2_HOST();
  virtual ~Arduino_BHY2_HOST();

  // Necessary API. Update function should be continuously polled if PASSTHORUGH is ENABLED
  void begin(bool passthrough = false);
  void update();

  // Functions for controlling the BHY when PASSTHROUGH is DISABLED
  void configureSensor(SensorConfigurationPacket& config);
  void configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency);
  uint8_t availableSensorData();
  bool readSensorData(SensorDataPacket &data);

  void parse(SensorDataPacket& data, DataXYZ& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);

  void debug(Stream &stream);

private:
  bool _passthrough;
};

extern Arduino_BHY2_HOST BHY2_HOST;

#endif
