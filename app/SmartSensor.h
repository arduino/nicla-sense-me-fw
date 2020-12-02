#ifndef SMART_SENSOR_H_
#define SMART_SENSOR_H_

// Properties of sensors
struct Sensor {
  uint8_t sensorId;
  // samle rate is used also to enable/disable the sensor
  // 0 for disable, else for enable
  //uint32_t sampleRate;
  float sampleRate;
  // how much ms time a new value is retained in its fifo
  // before a notification to the host is sent via interrupt
  // expressed in 24 bit
  uint32_t latency;
  // The host can read sensor's values when it receives an interrupt.
  // Alternatively, it can use the fifo flush command that sends all the fifo values and
  // discard sensor's fifos 
}

class SmartSensor {
  public:
    void interruptHandler() { _hasNewData = true; }
    bool hasNewData() { return _hasNewData;}

    void loop() 
    {
      if (_hasNewData()) {

      }
    }

  private:
    bool _hasNewData;
};


#endif
