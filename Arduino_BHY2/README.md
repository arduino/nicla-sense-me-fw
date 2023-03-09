# Arduino_BHY2

The `Arduino_BHY2` class, which is incorporated into the sketch by the statement `#include Arduino_BHY2.h`, configures the communication between the Nicla microcontroller and the BHY family of onboard sensors. The `NiclaWiring` and `NiclaConfig` enumerations configures the operation mode (over ESLOV or connected as a shield) and communication protocol (I2C, BLE, both or standalone) respectively.

> **Note**
> If no wired connection is used, we can set the state of `NiclaWiring` to `false`. Also, a modified delay using the millis() function similar to the [BlinkWithoutDelay.ino](https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay) sketch is used.

Methods interacting with the sensor, such as `configureSensor()` and `readSensorData()`, calls methods defined by the `sensortec` class which are defined in `BoschSensortec.h`. The `sensortec` class itself calls methods developed by the [Bosch BHY2-Sensor-API](https://github.com/boschsensortec/BHY2-Sensor-API) library and mirrored in the `bosch` folder. Motion data is parsed from the `DataParser` class (defined in `sensors/DataParser.h`). The `debug()` method checks the state of `NiclaConfig` and then calls the related method to start `.debug(stream)` using `eslovHandler` or `BLEHandler` class, together with `sensortec`, `dfuManager` and `BoschParser`.

IMU sensor objects are defined as objects of `SensorXYZ`. IMU readings in quaternion format are defined in `SensorQuaternion.h`. Activity recognition (obtained from the on-chip AI processor) in `SensorActivity.h`. Pressure, temperature and gas values are defined in `Sensor.h`. The relevant IDs are defined in `SensorID.h`. A user-readable list of Sensor IDs can be found in the [Nicla Sense ME Cheatsheet](https://docs.arduino.cc/tutorials/nicla-sense-me/cheat-sheet#sensor-ids), together with example code.

A UML diagram of the main library classes are provided in the diagram below, provided as an editable SVG file.

![Arduino_BHY2 Library UML Diagram](./Arduino_BHY2.UML.drawio.svg)