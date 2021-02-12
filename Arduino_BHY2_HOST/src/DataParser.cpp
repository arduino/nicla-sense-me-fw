#include "DataParser.h"

void DataParser::parse(SensorDataPacket& data, DataXYZ& vector) {
  vector.x = data.getInt16(0);
  vector.y = data.getInt16(2);
  vector.z = data.getInt16(4);
}

void DataParser::parse(SensorDataPacket& data, DataOrientation& vector, float scaleFactor) {
  vector.heading = data.getInt16(0) * scaleFactor;
  vector.pitch = data.getInt16(2) * scaleFactor;
  vector.roll = data.getInt16(4) * scaleFactor;
}

void DataParser::parse(SensorDataPacket& data, DataOrientation& vector) {
  parse(data, vector, 1.f);
}

void DataParser::parse(SensorDataPacket& data, DataQuaternion& vector) {
  vector.x = data.getInt16(0);
  vector.y = data.getInt16(2);
  vector.z = data.getInt16(4);
  vector.w = data.getInt16(6);
  vector.accuracy = data.getUint16(8);
}
