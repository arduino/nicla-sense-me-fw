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

void DataParser::parseQuaternion(SensorDataPacket& data, DataQuaternion& vector, float scaleFactor) {
  vector.x = data.getInt16(0) * scaleFactor;
  vector.y = data.getInt16(2) * scaleFactor;
  vector.z = data.getInt16(4) * scaleFactor;
  vector.w = data.getInt16(6) * scaleFactor;
  vector.accuracy = data.getUint16(8) * scaleFactor;
}

void DataParser::parseBSEC(SensorDataPacket& data, DataBSEC& vector) {
  const float SCALE_BSEC_BVOC_EQ = 0.01f;
  const float SCALE_BSEC_COMP_T = 1.0f / 256;
  const float SCALE_BSEC_COMP_H = 1.0f / 500;

  vector.iaq = data.getUint16(0);
  vector.iaq_s = data.getUint16(2);
  vector.b_voc_eq = data.getUint16(4) * SCALE_BSEC_BVOC_EQ;  //b-VOC-eq in the FIFO frame is scaled up by 100
  vector.co2_eq = data.getUint24(6);
  vector.accuracy = data.getUint8(9);
  vector.comp_t = data.getInt16(10) * SCALE_BSEC_COMP_T;
  vector.comp_h = data.getUint16(12) * SCALE_BSEC_COMP_H;
  vector.comp_g = (uint32_t)(data.getFloat(14));
}

void DataParser::parseBSECLegacy(SensorDataPacket& data, DataBSEC& vector) {
  vector.comp_t = data.getFloat(0);
  vector.comp_h = data.getFloat(4);
  //note that: SENSOR_DATA_FIXED_LENGTH is defined as 10 by default,
  //so all the fields below are 0 unless it's redefined to 29 and above
  vector.comp_g = (uint32_t)(data.getFloat(8));
  vector.iaq = (uint16_t)(data.getFloat(12));
  vector.iaq_s = (uint16_t)(data.getFloat(16));
  vector.co2_eq = (uint32_t)data.getFloat(20);
  vector.b_voc_eq = data.getFloat(24);
  vector.accuracy = data.getUint8(28);
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

void DataParser::parseActivity(SensorDataPacket& data, uint16_t& value) {
  value = data.getUint16(0);
}
