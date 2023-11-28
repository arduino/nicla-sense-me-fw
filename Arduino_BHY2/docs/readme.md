# Bosch BHY2 Sensor Library for Arduino

This library allows the Nicla Sense ME to interface with the onboard 6-axis IMU (BHI260AP) and environmental sensor (BME688).
## Features

- Easy access to data from Nicla Sense ME sensors using sensor ID
- Wrapper for [Bosch BHY2-Sensor-API](https://github.com/BoschSensortec/BHY2-Sensor-API)
- DFU (Device Firmware Update) of the ANNA-B112 and the BHI260AP
- All functionality available over both ESLOV and BLE

## Usage

The ArduinoBLE library needs to be installed, for the BLE features to function.

To use this library, use this code at the top of your sketch
```
#include <Arduino_BHY2.h>
```

> **Note**
> If no wired connection is used, we can set the state of `NiclaWiring` to `false`. Also, a modified delay using the millis() function similar to the [BlinkWithoutDelay.ino](https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay) sketch is used.

Methods interacting with the sensor, such as `configureSensor()` and `readSensorData()`, calls methods defined by the `sensortec` class which are defined in `BoschSensortec.h`. The `sensortec` class itself calls methods developed by the [Bosch BHY2-Sensor-API](https://github.com/boschsensortec/BHY2-Sensor-API) library and mirrored in the `bosch` folder. Motion data is parsed from the `DataParser` class (defined in `sensors/DataParser.h`). The `debug()` method checks the state of `NiclaConfig` and then calls the related method to start `.debug(stream)` using `eslovHandler` or `BLEHandler` class, together with `sensortec`, `dfuManager` and `BoschParser`.

IMU sensor objects are defined as objects of `SensorXYZ`. IMU readings in quaternion format are defined in `SensorQuaternion.h`. Activity recognition (obtained from the on-chip AI processor) in `SensorActivity.h`. Pressure, temperature and gas values are defined in `Sensor.h`. The relevant IDs are defined in `SensorID.h`. 

A UML diagram of the main library classes are provided in the diagram below, provided as an editable SVG file.

![Arduino_BHY2 Library UML Diagram](./Arduino_BHY2.UML.drawio.svg)

For additional information on the Arduino_BHY2 library (including a list of Sensor IDs) and how you can use it with the Nicla Sense ME, see the [Arduino Nicla Sense ME Cheat Sheet](https://docs.arduino.cc/tutorials/nicla-sense-me/cheat-sheet) page.

## Examples

- [App](https://github.com/arduino-libraries/Arduino_BHY2/blob/main/examples/App/App.ino) : Control Nicla Sense ME from an external device acting as a host via I2C or ESLOV
- [AppLowDelay](https://github.com/arduino-libraries/Arduino_BHY2/blob/main/examples/AppLowDelay/AppLowDelay.ino) : Control Nicla Sense ME from an external device acting as a host via I2C or ESLOV with a lower delay
- [BHYFirmwareUpdate](https://github.com/arduino-libraries/Arduino_BHY2/tree/main/examples/BHYFirmwareUpdate) : Update BHI260 firmware to latest version. Can also be used to upload custom firmware binary.
- [Fail_Safe_flasher](https://github.com/arduino-libraries/Arduino_BHY2/blob/main/examples/Fail_Safe_flasher/Fail_Safe_flasher.ino) : Fail Safe flashing of a RGB blink sketch. Writes binary file to the SPI flash, then performs CRC to ensure successful writing of binary file to flash memory. 
- [ReadSensorConfiguration](https://github.com/arduino-libraries/Arduino_BHY2/blob/main/examples/ReadSensorConfiguration/ReadSensorConfiguration.ino) : Send sample rate, latency and range for accelerator, gyroscope, rotation, temperature and gas sensor over Serial
- [ShowSensorList](https://github.com/arduino-libraries/Arduino_BHY2/blob/main/examples/ShowSensorList/ShowSensorList.ino) : List sensors of the Nicla Sense ME board
- [Standalone](https://github.com/arduino-libraries/Arduino_BHY2/blob/main/examples/Standalone/Standalone.ino) : Read motion, temperature and gas data and send over Serial
- [StandaloneFlashStorage](https://github.com/arduino-libraries/Arduino_BHY2/blob/main/examples/StandaloneFlashStorage/StandaloneFlashStorage.ino) : Save accelerator, gryoscope, temperature, gas and rotation sensor values to the onboard SPI flash in a .CSV file.

## License

See [LICENSE.txt](LICENSE.txt)