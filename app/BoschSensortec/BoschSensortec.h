#ifndef BOSCH_SENSORTEC_H_
#define BOSCH_SENSORTEC_H_

#define SENSOR_QUEUE_SIZE (10)

struct SensorConfiguration {
//struct __attribute__((packed)) SensorConfiguration {
  uint8_t sensorId;
  // sample rate is used also to enable/disable the sensor
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
};

enum SensorDataType {
  TypeInteger,
  TypeUnsigned,
  TypeFloat
};

// Generic data structure
// Size is the effective number of bytes
struct SensorData {
//struct __attribute__((packed)) SensorData {
  SensorDataType type;
  uint64_t data;
  uint8_t size;
};

// This will use the BHY functions for configuring sensors and retrieving data
class BoschSensortec {
  public:
    BoschSensortec() : _hasNewData(false), _savedConfig(NULL) {}
    virtual ~BoschSensortec() {}

    // ANNA <-> BOSCH interface
    void interruptHandler() { _hasNewData = true; }
    void configureBosch() {}
    void retrieveData() {}

    // sketch-side API
    void begin() { configureBosch(); }
    bool hasNewData() { return _hasNewData;}
    void update() 
    {
      if (_hasNewData) {
        // Retrieve data and store it in the queue
        // also handle the queue by storing it in flash if full
        retrieveData();
      }
    }

  private:
    bool _hasNewData;
    SensorData _sensorQueue[SENSOR_QUEUE_SIZE];
    SensorConfiguration* _savedConfig;
};


#endif
