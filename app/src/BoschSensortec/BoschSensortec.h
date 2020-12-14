#ifndef BOSCH_SENSORTEC_H_
#define BOSCH_SENSORTEC_H_

#include "channel.h"
#include "SensorTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "bosch/bhy2.h"
#ifdef __cplusplus
}
#endif

#define SENSOR_QUEUE_SIZE   10
#define WORK_BUFFER_SIZE    2048

// This will use the BHY functions for configuring sensors and retrieving data
class BoschSensortec {
  public:
    BoschSensortec();
    virtual ~BoschSensortec();

    // sketch-side API
    void begin(); 
    bool hasNewData(); 
    void update();
    void configureSensor(SensorConfigurationPacket *config);

    // ANNA <-> BOSCH interface
    static void interruptHandler();
    static void retrieveData(const struct bhy2_fifo_parse_data_info *data, void *arg);
    void addNewData(const struct bhy2_fifo_parse_data_info *fifoData);

  private:
    bool _hasNewData;
    SensorDataPacket _sensorQueue[SENSOR_QUEUE_SIZE];
    uint8_t _sensorQueueIndex;
    uint8_t _workBuffer[WORK_BUFFER_SIZE];

    SensorConfigurationPacket* _savedConfig;
    
    struct bhy2_dev _bhy2;
};

extern BoschSensortec sensortec;

#endif
