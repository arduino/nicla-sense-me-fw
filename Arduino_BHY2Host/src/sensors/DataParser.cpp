#include "sensors/DataParser.h"

void DataParser::parse3DVector(SensorDataPacket& data, DataXYZ& vector) {
  vector.x = data.getInt16(0);
  vector.y = data.getInt16(2);
  vector.z = data.getInt16(4);
}

void DataParser::parseEuler(SensorDataPacket& data, DataOrientation& vector, float scaleFactor) {
  vector.heading = data.getInt16(0) * scaleFactor;
  vector.pitch = data.getInt16(2) * scaleFactor;
  vector.roll = data.getInt16(4) * scaleFactor;
}

void DataParser::parseEuler(SensorDataPacket& data, DataOrientation& vector) {
  parseEuler(data, vector, 1.f);
}

void DataParser::parseQuaternion(SensorDataPacket& data, DataQuaternion& vector) {
  vector.x = data.getInt16(0);
  vector.y = data.getInt16(2);
  vector.z = data.getInt16(4);
  vector.w = data.getInt16(6);
  vector.accuracy = data.getUint16(8);
}

void DataParser::parseData(SensorDataPacket& data, float& value, float scaleFactor, SensorPayload format) {
  uint8_t id = data.sensorId;
  switch (format) {
    case P8BITSIGNED:
      value = data.getInt8(0) * scaleFactor;
      break;
    case P8BITUNISIGNED:
      value = data.getUint8(0) * scaleFactor;
      break;
    case P16BITSIGNED:
      value = data.getInt16(0) * scaleFactor;
      break;
    case P16BITUNSIGNED:
      value = data.getUint16(0) * scaleFactor;
      break;
    case P24BITUNSIGNED:
      value = data.getUint24(0) * scaleFactor;
      break;
    case P32BITSIGNED:
      value = data.getInt32(0) * scaleFactor;
      break;
    case P32BITUNSIGNED:
      value = data.getUint32(0) * scaleFactor;
      break;
    case PEVENT:
      value = 1;
    default:
      break;
  }

}

void DataParser::parseActivity(SensorDataPacket& data, uint16_t value) {
  value = data.getUint16(0);
}