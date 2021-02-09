#ifndef DATA_PARSER_H_
#define DATA_PARSER_H_

#include "Arduino.h"
#include "SensorTypes.h"

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

class DataParser {
public:
  static void parse(SensorDataPacket& data, DataXYZ& vector);
  static void parse(SensorDataPacket& data, DataOrientation& vector);
  static void parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);
};

#endif
