#ifndef ARDUINO_BHY2_H_
#define ARDUINO_BHY2_H_

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

/** 
 *  @brief Enumeration for defining wiring configuration over ESLOV or Shield.
 * 
 *  For NICLA_AS_SHIELD configuration, see https://docs.arduino.cc/tutorials/nicla-sense-me/use-as-mkr-shield
 *  
 */
enum NiclaWiring {
  NICLA_VIA_ESLOV = 0x10,
  NICLA_AS_SHIELD = 0x20
};

/** 
 *  @brief Enumeration for defining I2C or BLE communication configuration.
 * 
 *  
 *  @see Arduino_BHY2::begin()
 */
enum NiclaConfig {
  NICLA_I2C = 0x1,
  NICLA_BLE = 0x2,
  NICLA_BLE_AND_I2C = NICLA_I2C | NICLA_BLE,
  NICLA_STANDALONE = 0x4
};

class NiclaSettings {
public:
  NiclaSettings(uint8_t conf1 = 0, uint8_t conf2 = 0, uint8_t conf3 = 0, uint8_t conf4 = 0) {
    conf = conf1 | conf2 | conf3 | conf4;
  }

  uint8_t getConfiguration() const {
    return conf;
  }
private:
  uint8_t conf = 0;
};

class Arduino_BHY2 {
public:
  Arduino_BHY2();
  virtual ~Arduino_BHY2();

  // Necessary API. Update function should be continuously polled 
  /**
   * @param config Configuration for set @ref NiclaConfig state
   * @param niclaConnection Configuration for set @ref NiclaWiring state
   * 
  */
  bool begin(NiclaConfig config = NICLA_BLE_AND_I2C, NiclaWiring niclaConnection = NICLA_VIA_ESLOV);
  
  bool begin(NiclaSettings& settings);
  void update(); // remove this to enforce a sleep
  void update(unsigned long ms); // Update and then sleep
  void delay(unsigned long ms); // to be used instead of arduino delay()

  // API for using the bosch sensortec from sketch
  void configureSensor(SensorConfigurationPacket& config);
  void configureSensor(uint8_t sensorId, float sampleRate, uint32_t latency);
  void addSensorData(SensorDataPacket &sensorData);
  void addLongSensorData(SensorLongDataPacket &sensorData);
  uint8_t availableSensorData();
  uint8_t availableLongSensorData();
  bool readSensorData(SensorDataPacket &data);
  bool readLongSensorData(SensorLongDataPacket &data);
  bool hasSensor(uint8_t sensorId);

  void parse(SensorDataPacket& data, DataXYZ& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector);
  void parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);

  void setLDOTimeout(int time);

  void debug(Stream &stream);

private:
  Stream *_debug;

  void pingI2C();
  int _pingTime;
  int _timeout;
  int _startTime;

  PinName _eslovIntPin;

  NiclaConfig _niclaConfig;
};

extern Arduino_BHY2 BHY2;

#endif
