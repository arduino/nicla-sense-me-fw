# Bosch BHY2 Sensor Library for Arduino

The Bosch BHY2 Sensor Library provides an interface with the [Bosch BHY2-Sensor-API](https://github.com/BoschSensortec/BHY2-Sensor-API) for communicating with the BHI260AP and a custom library for the BME688 sensors of the Nicla Sense ME.

## Features

- Easy access to data from Nicla Sense ME sensors
- Wrapper for [Bosch BHY2-Sensor-API](https://github.com/BoschSensortec/BHY2-Sensor-API)
- DFU (Device Firmware Update) of the ANNA-B112 and the BHI260AP
- All functionality avaliable over both ESLOV and BLE

## Usage

The ArduinoBLE library needs to be installed, for the BLE features to function.

For additional information on the Arduino_BHY2 library and how you can use it with the Nicla Sense ME, see the [Arduino Nicla Sense ME Cheat Sheet](https://docs.arduino.cc/tutorials/nicla-sense-me/cheat-sheet) page.

To use this library
```
#include <Arduino_BHY2.h>
```

## Examples

- [App](examples/App/App.ino) : Control Nicla Sense ME from an external device acting as a host via I2C or ESLOV
- [AppLowDelay](examples/AppLowDelay/AppLowDelay.ino) : Control Nicla Sense ME from an external device acting as a host via I2C or ESLOV with a lower delay
- [BHYFirmwareUpdate](examples/BHYFirmwareUpdate/BHYFirmwareUpdate.ino) : Update ANNA-B112 and BHI260 firmware
- [Fail_Safe_flasher](examples/Fail_Safe_flasher/Fail_Safe_flasher.ino) : Fail Safe flashing of a RGB blink sketch
- [ReadSensorConfiguration](examples/ReadSensorConfiguration/ReadSensorConfiguration.ino) : Read motion, temperature and gas data and send over Serial
- [ShowSensorList](examples/ShowSensorList/ShowSensorList.ino) : List sensors of the Nicla Sense ME board
- [Standalone](examples/Standalone/Standalone.ino) : Read motion, temperature and gas data and send over Serial
- [StandaloneFlashStorage](examples/StandaloneFlashStorage/StandaloneFlashStorage.ino) : Read motion, temperature and gas data and send over and save on Flash storage 

## License

See [LICENSE.txt](LICENSE.txt)