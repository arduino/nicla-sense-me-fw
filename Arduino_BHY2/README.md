# Arduino_BHY2

A UML diagram of the main library clases are provided in the diagram below, provided as an editable SVG file.

![Arduino_BHY2 Library UML Diagram](./Arduino_BHY2.UML.drawio.svg)


The `EslovHandler` class configures the communication between the Nicla shield and the host controller, which also includes the I2C address. The `Wire` library is used to manage the I2C connection, while the I2C address is set to p24. The `DFUManager` class manages the device firmware upgrade (DFU) for the firmware on the Bosch BHY2 sensor family. The `BoschSensortec` class handles the communication between the ANNA-B112 microcontroller and the Bosch BHY2 sensors. Parsing of the sensor strings is done by the `BoschParser` class. 