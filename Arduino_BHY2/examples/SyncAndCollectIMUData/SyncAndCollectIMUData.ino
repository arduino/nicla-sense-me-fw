/* 
 * This sketch shows how nicla can be used in standalone mode.
 * Without the need for an host, nicla can run sketches that 
 * are able to configure the bhi sensors and are able to read all 
 * the bhi sensors data.
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

class IMUDataSyncer;
class SensorMotion;

#define DEBUG 0

#if DEBUG
#include "MbedQDebug.h"
#else
void *nicla_dbg_info_ptr;
#endif

class IMUDataSyncer {
  public: 
  IMUDataSyncer() {
    int i;
    for (i = 0; i < sizeof(_seq)/sizeof(_seq[0]); i++) {
      _seq[i] = -1;
      _data[i] = NULL;
    }
  }

  virtual bool begin(int accelSampleRate, int gyroSampleRate) {
    int sampleRate = accelSampleRate > gyroSampleRate ? accelSampleRate : gyroSampleRate;
    
    if ((accelSampleRate > 0) && (gyroSampleRate > 0)) {
        
        if (accelSampleRate != gyroSampleRate) {
          _logId = 'u'; //data cannot be synced
          return false;
        } else {
          _logId = 'i';  
        }
    } else if (accelSampleRate > 0) {
        _logId = 'a';
    } else if (gyroSampleRate > 0){
        _logId = 'g';
    } else {
        _logId = 'n';
    }

    return true;
  }

  void onSensorDataUpdate(DataXYZ &data, int id) {
    _data[id] = &data;
    
    if ('i' == _logId) {
      //we assume id is always 0 (for accel) or 1 (for gyro)
      //when _seq[1 - id] is less than 0, it means the other sensor has not received any value yet
      if (_seq[1 - id] >= 0) {
        _seq[id] = (_seq[id]+1) % 10;
      } else {        
        _seq[id] = 0; //wait for another sensor to generate the 1st sample
      }

      if (_seq[id] == _seq[1 - id]) {
        //data synced well and ready for sending out
        #if DEBUG
        mbq_dbg_1(1);
        #endif

        #if 0 
        //enable this if you want to send the sequence number in each line, 
        //for highest ODR, we ignore this to save bandwidth
        Serial.print(_logId);                 
        Serial.print((char)(_seq[id]+'0')); Serial.print(',');
        #endif
        //accel data fields
        Serial.print(_data[0]->x); Serial.print(',');
        Serial.print(_data[0]->y); Serial.print(',');
        Serial.print(_data[0]->z); Serial.print(',');
        //gyro data fields
        Serial.print(_data[1]->x); Serial.print(',');
        Serial.print(_data[1]->y); Serial.print(',');
        Serial.print(_data[1]->z); Serial.println();

        #if DEBUG
        mbq_dbg_1(0);
        #endif
      }
    } else {
        _seq[id] = (_seq[id]+1) % 10;
        Serial.print(_logId);                 
        Serial.print(_seq[id]); Serial.print(',');
        Serial.print(_data[id]->x); Serial.print(',');
        Serial.print(_data[id]->y); Serial.print(',');
        Serial.print(_data[id]->z); Serial.println();        
    }
  }
  
  protected:
    char _logId;
    int8_t _seq[2];
    DataXYZ *_data[2];
};

class SensorMotion : public SensorClass {
  public:
    SensorMotion(uint8_t id) : SensorClass(id) {
      _id = (id / 10); //accel: 0, gyro: 1, mag: 2
      _dataSyncer = NULL;
    }
    
  void setDataSyncer(IMUDataSyncer &dataSyncer) {
      _dataSyncer = &dataSyncer;
  }

  void setData(SensorDataPacket &data) {
    DataParser::parse3DVector(data, _data);
    _dataSyncer->onSensorDataUpdate(_data, _id);
  }
  
  void setData(SensorLongDataPacket &data) {}
  
  String toString() {
    return _data.toString();
  }
  
  protected:
    DataXYZ _data;
    uint8_t _id;
    IMUDataSyncer *_dataSyncer;   
};


SensorMotion accel(1);  //sensor id 1: accel raw data passthrough
SensorMotion gyro(10);  //sensor id 10: gyro raw data passthrough
IMUDataSyncer imuDataSyncer;

#define IMU_DATA_RATE 1600



void testSerial()
{
  while (0) {
    Serial.print('*');
    //Serial.println();

    delay(1000);
  }
}

void setup()
{
  //Serial.begin(115200);
  Serial.begin(1000000);  //max br: 1Mbps for nRF52
  while(!Serial);
  
  BHY2.begin(NICLA_STANDALONE);

  testSerial();

  SensorConfig cfg = accel.getConfiguration();
  Serial.println(String("range of accel: +/-") + cfg.range + String("g"));
  accel.setRange(8);    //this sets the range of accel to +/-8g, 
  cfg = accel.getConfiguration();
  Serial.println(String("range of accel: +/-") + cfg.range + String("g"));

  imuDataSyncer.begin(IMU_DATA_RATE, IMU_DATA_RATE);
  accel.setDataSyncer(imuDataSyncer);
  gyro.setDataSyncer(imuDataSyncer);
  accel.begin(IMU_DATA_RATE);
  gyro.begin(IMU_DATA_RATE);
}

void loop()
{
  #if DEBUG
        mbq_dbg_0(1);
  #endif
  
  BHY2.update();

  #if DEBUG
        mbq_dbg_0(0);
  #endif
}
