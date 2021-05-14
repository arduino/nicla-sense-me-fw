#include "Arduino.h"

//#include "ArduinoBLE.h"
//#include "Arduino_BHY2.h"

#include "Arduino_BHY2.h"
#include "bosch/bhy2.h"
#include "bosch/common/common.h"

#define MAX_READ_WRITE_LEN 256

void setup() {
  //BLE.begin();

  //BLE.setLocalName("testpower");
  ////BLE.setAdvertisingInterval(0x4000);
  //BLE.advertise();

  //BHY2.begin();

  Serial.begin(115200);
  wakeBhy();

  delay(100);

  configureSensor(10, 1., 0);
}

void loop() {
  //BLE.poll(4000);
  delay(100);
  //BHY2.delay(4000);
  update();
}

auto _debug = &Serial;
//Stream* _debug = NULL;
struct bhy2_dev _bhy2;
#define WORK_BUFFER_SIZE 2048
uint8_t _workBuffer[WORK_BUFFER_SIZE];
uint8_t _sensorsPresent[32];

void update() {
  if (get_interrupt_status()) {
    auto ret = bhy2_get_and_process_fifo(_workBuffer, WORK_BUFFER_SIZE, &_bhy2);
    if (_debug) _debug->println(get_api_error(ret));
  }
}

void configureSensor(uint8_t id, float rate, uint32_t lat) {
  auto ret = bhy2_set_virt_sensor_cfg(id, rate, lat, &_bhy2);
  if (_debug) _debug->println(get_api_error(ret));
}

void wakeBhy() {
  auto ret = bhy2_init(BHY2_SPI_INTERFACE, bhy2_spi_read, bhy2_spi_write, bhy2_delay_us, MAX_READ_WRITE_LEN, NULL, &_bhy2);
  if (_debug) _debug->println(get_api_error(ret));

  bhy2_soft_reset(&_bhy2);

  // Print bhi status 
  uint8_t stat = 0;
  //delay(1000);
  ret = bhy2_get_boot_status(&stat, &_bhy2);
  if (_debug) {
    _debug->println(get_api_error(ret));
    _debug->print("Boot status: ");
    _debug->println(stat, HEX);
  }

  ret = bhy2_boot_from_flash(&_bhy2);
  if (_debug) _debug->println(get_api_error(ret));

  ret = bhy2_get_boot_status(&stat, &_bhy2);
  if (_debug) {
    _debug->println(get_api_error(ret));
    _debug->print("Boot status: ");
    _debug->println(stat, HEX);
  }

  ret = bhy2_get_host_interrupt_ctrl(&stat, &_bhy2);
  if (_debug) {
    _debug->println(get_api_error(ret));
    _debug->print("Interrupt ctrl: ");
    _debug->println(stat, HEX);
  }

  ret = bhy2_get_host_intf_ctrl(&stat, &_bhy2);
  if (_debug) {
    _debug->println(get_api_error(ret));
    _debug->print("Interface ctrl: ");
    _debug->println(stat, HEX);
  }

  bhy2_register_fifo_parse_callback(BHY2_SYS_ID_META_EVENT, parseMetaEvent, NULL, &_bhy2);
  bhy2_register_fifo_parse_callback(BHY2_SYS_ID_META_EVENT_WU, parseMetaEvent, NULL, &_bhy2);
  bhy2_register_fifo_parse_callback(BHY2_SYS_ID_DEBUG_MSG, parseDebugMessage, NULL, &_bhy2);

  ret = bhy2_get_and_process_fifo(_workBuffer, WORK_BUFFER_SIZE, &_bhy2);
  if (_debug) _debug->println(get_api_error(ret));

  // All sensors' data are handled in the same generic way
  for (uint8_t i = 1; i < BHY2_SENSOR_ID_MAX; i++) {
    bhy2_register_fifo_parse_callback(i, parseData, NULL, &_bhy2);
  }

  bhy2_update_virtual_sensor_list(&_bhy2);
  bhy2_get_virt_sensor_list(_sensorsPresent, &_bhy2);

  printSensors();

}

void printSensors() {
  bool presentBuff[256];

  for (uint16_t i = 0; i < sizeof(_sensorsPresent); i++)
  {
      for (uint8_t j = 0; j < 8; j++)
      {
          presentBuff[i * 8 + j] = ((_sensorsPresent[i] >> j) & 0x01);
      }
  }

  if (_debug) {
    _debug->println("Present sensors: ");
    for (int i = 0; i < sizeof(presentBuff); i++) {
      if (presentBuff[i]) {
        _debug->print(i);
        _debug->print(" - ");
        _debug->print(get_sensor_name(i));
        _debug->println();
      }
    }
  }
}

void convertTime(uint64_t time_ticks, uint32_t *s, uint32_t *ns)
{
    uint64_t timestamp = time_ticks; /* Store the last timestamp */

    timestamp = timestamp * 15625; /* timestamp is now in nanoseconds */
    *s = (uint32_t)(timestamp / UINT64_C(1000000000));
    *ns = (uint32_t)(timestamp - ((*s) * UINT64_C(1000000000)));
}


void parseData(const struct bhy2_fifo_parse_data_info *fifoData, void *arg)
{
  SensorDataPacket sensorData;
  sensorData.sensorId = fifoData->sensor_id;
  sensorData.size = (fifoData->data_size > sizeof(sensorData.data)) ? sizeof(sensorData.data) : fifoData->data_size;
  memcpy(&sensorData.data, fifoData->data_ptr, sensorData.size);

  if (_debug) {
    _debug->print("Sensor: ");
    _debug->print(sensorData.sensorId);
    _debug->print("  value: ");
    for (uint8_t i = 0; i < (fifoData->data_size - 1); i++)
    {
        _debug->print(sensorData.data[i], HEX);
    }
    _debug->print("  ");
    for (uint8_t i = 0; i < (fifoData->data_size - 1); i++)
    {
        _debug->print(fifoData->data_ptr[i], HEX);
    }
    _debug->println("");
  }
}


void parseMetaEvent(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
  uint8_t meta_event_type = callback_info->data_ptr[0];
  uint8_t byte1 = callback_info->data_ptr[1];
  uint8_t byte2 = callback_info->data_ptr[2];
  uint32_t s, ns;
  char *event_text;

  if (callback_info->sensor_id == BHY2_SYS_ID_META_EVENT)
  {
    event_text = "[META EVENT]";
  }
  else if (callback_info->sensor_id == BHY2_SYS_ID_META_EVENT_WU)
  {
    event_text = "[META EVENT WAKE UP]";
  }
  else
  {
    return;
  }

  convertTime(*callback_info->time_stamp, &s, &ns);

  struct parse_ref *parse_table = (struct parse_ref*)callback_ref;

  if (_debug) {

    switch (meta_event_type)
    {
      case BHY2_META_EVENT_FLUSH_COMPLETE:
        //printf("%s; T: %u.%09u; Flush complete for sensor id %u\r\n", event_text, s, ns, byte1);
        _debug->print(event_text); 
        _debug->print(" Flush complete for sensor id ");
        _debug->println(byte1);
        break;
      case BHY2_META_EVENT_SAMPLE_RATE_CHANGED:
        //printf("%s; T: %u.%09u; Sample rate changed for sensor id %u\r\n", event_text, s, ns, byte1);
        _debug->print(event_text); 
        _debug->print(" Sample rate changed for sensor id ");
        _debug->println(byte1);
        break;
      case BHY2_META_EVENT_POWER_MODE_CHANGED:
        //printf("%s; T: %u.%09u; Power mode changed for sensor id %u\r\n", event_text, s, ns, byte1);
        _debug->print(event_text); 
        _debug->print(" Power mode changed for sensor id ");
        _debug->println(byte1);
        break;
      case BHY2_META_EVENT_ALGORITHM_EVENTS:
        //printf("%s; T: %u.%09u; Algorithm event\r\n", event_text, s, ns);
        _debug->print(event_text); 
        _debug->println(" Algorithm event");
        break;
      case BHY2_META_EVENT_SENSOR_STATUS:
        //printf("%s; T: %u.%09u; Accuracy for sensor id %u changed to %u\r\n", event_text, s, ns, byte1, byte2);
        _debug->print(event_text); 
        _debug->print(" Accuracy for sensor id ");
        _debug->print(byte1);
        _debug->print(" changed to ");
        _debug->println(byte2);
        break;
      case BHY2_META_EVENT_BSX_DO_STEPS_MAIN:
        //printf("%s; T: %u.%09u; BSX event (do steps main)\r\n", event_text, s, ns);
        _debug->print(event_text); 
        _debug->println(" Algorithm event");
        break;
      case BHY2_META_EVENT_BSX_DO_STEPS_CALIB:
        //printf("%s; T: %u.%09u; BSX event (do steps calib)\r\n", event_text, s, ns);
        _debug->print(event_text); 
        _debug->println(" BSX event (do steps calib)");
        break;
      case BHY2_META_EVENT_BSX_GET_OUTPUT_SIGNAL:
        //printf("%s; T: %u.%09u; BSX event (get output signal)\r\n", event_text, s, ns);
        _debug->print(event_text); 
        _debug->println(" BSX event (get output signal)");
        break;
      case BHY2_META_EVENT_SENSOR_ERROR:
        //printf("%s; T: %u.%09u; Sensor id %u reported error 0x%02X\r\n", event_text, s, ns, byte1, byte2);
        _debug->print(event_text); 
        _debug->print(" Sensor id ");
        _debug->print(byte1);
        _debug->print(" reported error 0x ");
        _debug->println(byte2, HEX);
        break;
      case BHY2_META_EVENT_FIFO_OVERFLOW:
        //printf("%s; T: %u.%09u; FIFO overflow\r\n", event_text, s, ns);
        _debug->print(event_text); 
        _debug->println(" FIFO overflow");
        break;
      case BHY2_META_EVENT_DYNAMIC_RANGE_CHANGED:
        //printf("%s; T: %u.%09u; Dynamic range changed for sensor id %u\r\n", event_text, s, ns, byte1);
        _debug->print(event_text); 
        _debug->print(" Dynamic range changed for sensor id ");
        _debug->println(byte1);
        break;
      case BHY2_META_EVENT_FIFO_WATERMARK:
        //printf("%s; T: %u.%09u; FIFO watermark reached\r\n", event_text, s, ns);
        _debug->print(event_text); 
        _debug->println(" FIFO watermark reached");
        break;
      case BHY2_META_EVENT_INITIALIZED:
        //printf("%s; T: %u.%09u; Firmware initialized. Firmware version %u\r\n", event_text, s, ns,
              //((uint16_t )byte2 << 8) | byte1);
        _debug->print(event_text); 
        _debug->print(" Firmware initialized. Firmware version ");
        _debug->println(((uint16_t )byte2 << 8) | byte1);
        break;
      case BHY2_META_TRANSFER_CAUSE:
        //printf("%s; T: %u.%09u; Transfer cause for sensor id %u\r\n", event_text, s, ns, byte1);
        _debug->print(event_text); 
        _debug->print(" Transfer cause for sensor id ");
        _debug->println(byte1);
        break;
      case BHY2_META_EVENT_SENSOR_FRAMEWORK:
        //printf("%s; T: %u.%09u; Sensor framework event for sensor id %u\r\n", event_text, s, ns, byte1);
        _debug->print(event_text); 
        _debug->print(" Sensor framework event for sensor id ");
        _debug->println(byte1);
        break;
      case BHY2_META_EVENT_RESET:
        //printf("%s; T: %u.%09u; Reset event\r\n", event_text, s, ns);
        _debug->print(event_text); 
        _debug->println(" Reset event");
        break;
      case BHY2_META_EVENT_SPACER:
        break;
      default:
        //printf("%s; T: %u.%09u; Unknown meta event with id: %u\r\n", event_text, s, ns, meta_event_type);
        _debug->print(event_text); 
        _debug->print(" Unknown meta event with id: ");
        _debug->println(meta_event_type); 
        break;
    }
  }
}

void parseGeneric(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
  uint32_t s, ns;
  convertTime(*callback_info->time_stamp, &s, &ns);

  printf("SID: %u; T: %u.%09u; ", callback_info->sensor_id, s, ns);
  for (uint8_t i = 0; i < (callback_info->data_size - 1); i++)
  {
      printf("%X ", callback_info->data_ptr[i]);
  }
  printf("\r\n");
}

void parseDebugMessage(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
  uint32_t s, ns;
  convertTime(*callback_info->time_stamp, &s, &ns);
  //if (_debug) _debug->println("Debug message: ");
  if (_debug) {
    _debug->print("[DEBUG MSG]; flag: 0x");
    _debug->print(callback_info->data_ptr[0]);
    _debug->print(" data: ");
    _debug->println((char*)&callback_info->data_ptr[1]);
  }
  //printf("[DEBUG MSG]; T: %u.%09u; flag: 0x%x; data: %s\r\n",
        //s,
        //ns,
        //callback_info->data_ptr[0],
        //&callback_info->data_ptr[1]);
}

