/* 
 * This sketch shows how nicla can be used in standalone mode.
 * Without the need for an host, nicla can run sketches that 
 * are able to configure the bhi sensors and are able to read all 
 * the bhi sensors data.
 */

#include "Arduino.h"
#include "Arduino_BHY2.h"

#include "base64.hpp" //remember to include the library "base64 by Densaugeo" from Library manager 

#define PDEBUG 0

#if PDEBUG
#include "MbedQDebug.h"
#else
void *nicla_dbg_info_ptr;
#endif


class IMUDataSyncer;
class SensorMotion;

enum {
    IMU_SENSOR_ID_ACC = 0,
    IMU_SENSOR_ID_GYR,
};

class IMUDataSyncer {
    public: 
        IMUDataSyncer() {
            uint8_t i;
            for (i = 0; i < sizeof(_seq)/sizeof(_seq[0]); i++) {
                _seq[i] = -1;
            }
        }

        //@param outputMetaInfo enable this if you want to send the sequence number in each line of output, 
        //when outputInBase64 is false, for highest ODR like 1600hz, ignore this to save bandwidth
        virtual bool begin(int accelSampleRate, int gyroSampleRate, bool outputInBase64 = false, bool outputMetaInfo = true) {
            //int sampleRate = accelSampleRate > gyroSampleRate ? accelSampleRate : gyroSampleRate;
            _outputInBase64 = outputInBase64;
            _outputMetaInfo = outputMetaInfo;
            if ((accelSampleRate > 0) && (gyroSampleRate > 0)) {
                if (accelSampleRate != gyroSampleRate) {
                    //'u' means unaliged data rate
                    _logId = 'u'; //for simplicity, we do not sync acc and gyro data when the rates are not the same
                    return false;
                } else {
                    _logId = 'i';  
                }
            } else if (accelSampleRate > 0) {
                _logId = 'a';
            } else if (gyroSampleRate > 0){
                _logId = 'g';
            } else {
                _logId = 'n';   //null
            }

            return true;
        }

        virtual void end() {
        }

        void onSensorDataUpdate(SensorDataPacket &data, uint8_t id) {
            const uint8_t IDS_ALL = (1<<IMU_SENSOR_ID_ACC) | (1<<IMU_SENSOR_ID_GYR);

            memcpy(&_dataPkt[id], &data, sizeof(SensorDataPacket));
            if (!_outputInBase64) {
                DataParser::parse3DVector(_dataPkt[id], _data[id]);
            }

            //we are logging data for the IMU (accel + gyro)
            if ('i' == _logId) {
                //we assume id is always 0 (for accel) or 1 (for gyro)
                //when _seq[1 - id] is less than 0, it means the other sensor has not received any value yet
                if (_seq[1 - id] >= 0) {
                    _seq[id] = (_seq[id]+1) % 10;
                } else {        
                    _seq[id] = 0; //this is to wait for another sensor to generate the 1st sample
                }

                if (_seq[id] == _seq[1 - id]) {
                    //data synced well and ready for sending out
                    outputData(IDS_ALL);
                }
            } else {
                //we are only collecting data for a single sensor
                _seq[id] = (_seq[id]+1) % 10;
                outputData(1<<id);
            }
        }

    private:
        void outputData(uint8_t ids) {
            const uint8_t idHasAcc = 1 << IMU_SENSOR_ID_ACC;
            const uint8_t idHasGyr = 1 << IMU_SENSOR_ID_GYR;
            int res;
            uint8_t id = 0;
#if PDEBUG
            mbq_dbg_1(1);
#endif
            if (_outputInBase64) {
                unsigned char rec_input[14] = "";
                char rec_out[21] = "";
                uint8_t idx = 0;
                if (_outputMetaInfo) {
                    id = ids & idHasAcc ? IMU_SENSOR_ID_ACC : IMU_SENSOR_ID_GYR;
                    rec_input[idx++] = _logId;
                    rec_input[idx++] = _seq[id];
                }

                if (ids & idHasAcc) {                    
                    memcpy(&rec_input[idx], &(_dataPkt[IMU_SENSOR_ID_ACC].data[0]), 6);   //accel data
                    idx += 6;
                    id = IMU_SENSOR_ID_ACC;
                }

                if (ids & idHasGyr) {                    
                    memcpy(&rec_input[idx], &(_dataPkt[IMU_SENSOR_ID_GYR].data[0]), 6);   //gyro data
                    idx += 6;
                    id = IMU_SENSOR_ID_GYR;
                }


                res = encode_base64(rec_input, idx, (unsigned char*)rec_out);
                (void)res;
                Serial.println(rec_out);
            } else {
                if (_outputMetaInfo) {
                    Serial.print(_logId);
                    id = (ids & idHasAcc) ? IMU_SENSOR_ID_ACC : IMU_SENSOR_ID_GYR;
                    Serial.print((char)(_seq[id]+'0'));
                }

                if (ids & idHasAcc) {
                    //accel data fields
                    id = IMU_SENSOR_ID_ACC;
                    Serial.print(','); Serial.print(_data[IMU_SENSOR_ID_ACC].x);
                    Serial.print(','); Serial.print(_data[IMU_SENSOR_ID_ACC].y);
                    Serial.print(','); Serial.print(_data[IMU_SENSOR_ID_ACC].z);
                }

                if (ids & idHasGyr) {
                    //gyro data fields
                    Serial.print(',');Serial.print(_data[IMU_SENSOR_ID_GYR].x); 
                    Serial.print(',');Serial.print(_data[IMU_SENSOR_ID_GYR].y); 
                    Serial.print(',');Serial.print(_data[IMU_SENSOR_ID_GYR].z);
                    id = IMU_SENSOR_ID_GYR;
                }


                Serial.println();
            }
#if PDEBUG
            mbq_dbg_1(0);
#endif

        }

    protected:
        char _logId;
        bool _outputInBase64;
        bool _outputMetaInfo;
        int8_t _seq[2];
        SensorDataPacket _dataPkt[2];
        DataXYZ _data[2];
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
            //DataParser::parse3DVector(data, _data);
            _dataSyncer->onSensorDataUpdate(data, _id);
        }

        void setData(SensorLongDataPacket &data) {}

        String toString() {
            //we are delegating the data parsing to the _dataSyncer
            return ("");
        }

    protected:
        //DataXYZ _data;
        uint8_t _id;
        IMUDataSyncer *_dataSyncer;
};


SensorMotion accel(1);  //sensor id 1: accel raw data passthrough
SensorMotion gyro(10);  //sensor id 10: gyro raw data passthrough
IMUDataSyncer imuDataSyncer;


#define DATA_RATE_ACC 1600
#define DATA_RATE_GYR 1600


void testSerial()
{
#if PDEBUG
    while (0) {
        mbq_dbg_1(1);
        Serial.println("0123456789");
        mbq_dbg_1(0);
        delay(10);
    }
#endif
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


    //imuDataSyncer.begin(DATA_RATE_ACC, DATA_RATE_GYR, false);
    imuDataSyncer.begin(DATA_RATE_ACC, DATA_RATE_GYR, true);
    accel.setDataSyncer(imuDataSyncer);
    gyro.setDataSyncer(imuDataSyncer);
    accel.begin(DATA_RATE_ACC);
    gyro.begin(DATA_RATE_GYR);
}

void loop()
{
#if PDEBUG
    mbq_dbg_0(1);
#endif

    BHY2.update();

#if PDEBUG
    mbq_dbg_0(0);
#endif
}
