#ifndef DATA_PARSER_H_
#define DATA_PARSER_H_

#include "Arduino.h"
#include "sensors/SensorTypes.h"
#include "SensorID.h"

struct DataXYZ {
  int16_t x;
  int16_t y;
  int16_t z;

  String toString() {
    return (String)("XYZ values - X: " + String(x)
                    + "   Y: " + String(y)
                    + "   Z: " + String(z) + "\n");
  }
};

struct DataOrientation {
  float heading;
  float pitch;
  float roll;

  String toString() {
    return (String)("Orientation values - heading: " + String(heading, 3) 
                    + "   pitch: " + String(pitch, 3) 
                    + "   roll: " + String(roll, 3) + "\n");
  }
};

struct DataQuaternion {
  int16_t x;
  int16_t y;
  int16_t z;
  int16_t w;
  uint16_t accuracy;

  String toString() {
    return (String)("Quaternion values - X: " + String(x)
                    + "   Y: " + String(y)
                    + "   Z: " + String(z) 
                    + "   W: " + String(w) 
                    + "   Accuracy: " + String(accuracy) 
                    + "\n");
  }
};

class DataParser {
public:
  static void parse3DVector(SensorDataPacket& data, DataXYZ& vector);
  static void parseEuler(SensorDataPacket& data, DataOrientation& vector);
  static void parseEuler(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);
  static void parseQuaternion(SensorDataPacket& data, DataQuaternion& vector);
  static void parseData(SensorDataPacket& data, float& value, float scaleFactor, SensorPayload format);
  static void parseActivity(SensorDataPacket& data, uint16_t value);
};

#endif
