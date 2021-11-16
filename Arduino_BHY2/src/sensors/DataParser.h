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
  float x;
  float y;
  float z;
  float w;
  float accuracy;

  String toString() {
    return (String)("Quaternion values - X: " + String(x, 3)
                    + "   Y: " + String(y, 3)
                    + "   Z: " + String(z, 3)
                    + "   W: " + String(w, 3)
                    + "   Accuracy: " + String(accuracy, 3)
                    + "\n");
  }
};

struct DataBSEC {
  uint16_t  iaq;         //iaq value for regular use case
  uint16_t  iaq_s;       //iaq value for stationary use cases
  float     b_voc_eq;    //breath VOC equivalent (ppm)
  uint32_t  co2_eq;      //CO2 equivalent (ppm) [400,]
  float     comp_t;      //compensated temperature (celcius)
  float     comp_h;      //compensated humidity
  uint32_t  comp_g;      //compensated gas resistance (Ohms)
  uint8_t   accuracy;    //accuracy level: [0-3]

  String toString() {
    return (String)("BSEC output values - iaq: " + String(iaq)
                    + "   iaq_s: " + String(iaq_s)
                    + "   b_voc_eq: " + String(b_voc_eq, 2)
                    + "   co2_eq: " + String(co2_eq)
                    + "   accuracy: " + String(accuracy)
                    + "   comp_t: " + String(comp_t, 2)
                    + "   comp_h: " + String(comp_h, 2)
                    + "   comp_g: " + String(comp_g)
                    + "\n");
  }
};


class DataParser {
public:
  static void parse3DVector(SensorDataPacket& data, DataXYZ& vector);
  static void parseEuler(SensorDataPacket& data, DataOrientation& vector);
  static void parseEuler(SensorDataPacket& data, DataOrientation& vector, float scaleFactor);
  static void parseQuaternion(SensorDataPacket& data, DataQuaternion& vector, float scaleFactor);
  static void parseBSEC(SensorDataPacket& data, DataBSEC& vector);
  static void parseBSECLegacy(SensorDataPacket& data, DataBSEC& vector);
  static void parseData(SensorDataPacket& data, float& value, float scaleFactor, SensorPayload format);
  static void parseActivity(SensorDataPacket& data, uint16_t& value);
};

#endif
