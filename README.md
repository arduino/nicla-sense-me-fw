# Unisense
This repo contains everything needed to program Unisense and to control it from another device.

Table of Contents:
- [Repo structure](#repo-structure)
- [Main use cases](#main-use-cases)
  * [Unisense standalone mode](#unisense-standalone-mode)
  * [Control Unisense from an arduino board through eslov](#control-unisense-from-an-arduino-board-through-eslov)
  * [Control Unisense from a PC - web server](#control-unisense-from-a-pc---web-server)
  * [Control Unisense from a PC - command line tool](#control-unisense-from-a-pc---command-line-tool)
  * [Update Unisense firmware](#update-unisense-firmware)
  
## Repo structure

------------
[Arduino_BHY2](Arduino_BHY2) - is the library to be included when making sketches for Unisense. It can be used for configuring bhi's sensors and for retrieving bhi's sensors data either from a sketch running on Unisense (standalone mode)  or from an external device (thorugh eslov or BLE). 

In [examples](Arduino_BHY2/examples) there are already working examples for both scenarios:
- Standalone mode - [Standalone](Arduino_BHY2/examples/Standalone/Standalone.ino) exploits bhi's sensors directly from Unisense.
- Control from external device mode -  [App](Arduino_BHY2/examples/App/App.ino) waits for external stimuli, coming from eslov or BLE channels.
------------

[Arduino_BHY2_HOST](Arduino_BHY2_HOST) - is the library that an arduino board should include in order to control Uninsense through the eslov port.
  It can also act as a passthrough, to allow the control of Unisense from a PC.
  There is an example for both these use cases in [examples](Arduino_BHY2_HOST/examples):
  - Act as passthrough - [Passthrough](Arduino_BHY2_HOST/examples/Passthrough/Passthrough.ino)
  - Control Unisense directly - [Accelerometer](Arduino_BHY2_HOST/examples/Accelerometer/Accelerometer.ino)
  
------------

[bootloader](bootloader)  - contains the source code for Unisense's bootloader.

------------

[tools](tools/bhy-controller) - contains the source code of the command line tool to control Unisense from a PC.
  There is also a webserver mode that allows to control Unisense from a browser on your PC.

------------


## Main use cases

### Unisense standalone mode
In this use case, Unisense will be able to control its own sensors without the need for an external host.  
- Make a standalone sketch importing [Arduino_BHY2](Arduino_BHY2) library, follow this [example](Arduino_BHY2/examples/Standalone/Standalone.ino) to write the code.
- Select Unisense as the compilation target, then compile.
- Upload the `.bin` resulting file. (see [Update Unisense firmware](#update-unisense-firmware))


### Control Unisense from an arduino board through eslov
- Upload the [App](Arduino_BHY2/examples/App/App.ino) example to Unisense.
- Write a sketch for the arduino board acting as a host by importing [Arduino_BHY2_HOST](Arduino_BHY2_HOST) and following the [example](Arduino_BHY2_HOST/examples/Accelerometer/Accelerometer.ino).
- Upload the host sketch to the arduino board.
- Connect Unisense to the arduino board through the eslov connector.


### Control Unisense from a PC - web server 
A local web server has been implemented to manage Unisense from a browser. See [browser compatibility](https://developer.mozilla.org/en-US/docs/Web/API/Web_Bluetooth_API#browser_compatibility).

BLE is used instead of eslov, thus there is no need for an intermediary arduino board.
- Upload the [App](Arduino_BHY2/examples/App/App.ino) example to Unisense.
- Access the command line tool folder.
- Execute the following command `./bhy webserver` to start the local web server.
- Open the `http://localhost:8000/index.html` web page with a browser listed in the compatibility list.

There are two web pages
- `sensor.html` allows to configure the sensors and read their values in real time.
- `dfu.html` allows to update the firmware of either Unisense or bhi.


### Control Unisense from a PC - command line tool
The command line tool can be used for updating the firmware of Unisense or bhi, for managing Unisense's sensors and for reading their values in real time. Refer to [tools](tools/bhy-controller) for the list of available commands. 
Here the eslov protocol is employed, so an arduino board is needed between Unisense and the PC.
- Upload the [App](Arduino_BHY2/examples/App/App.ino) example to Unisense.
- Upload the [Passthrough](Arduino_BHY2_HOST/examples/Passthrough/Passthrough.ino) example to an arduino board.
- Connect the arduino board to your PC via USB.
- Connect the arduino board to Unisense through the eslov port.
- Access the command line tool folder
- Refer to [tools](tools/bhy-controller) for the list of available commands.


### Update Unisense firmware
The procedure to update the firmware is the same for both bhi and Unisense. You just need to specify the target during the procedure.
#### Update procedure through BLE:
- Follow these steps [Control Unisense from a PC - web server](#control-unisense-from-a-pc---web-server).
- Select _**DFU**_ in the page located at `http://localhost:8000/index.html` - or directly go to `http://localhost:8000/dfu.html`.
- Select the target (bhi or unisense).
- Select the .bin firmware file.
- Click on update and wait for the transfer to complete.
Then, Unisense will reset and the firmware will be updated.

#### Update procedure through ESLOV:
- Follow [Control Unisense from a PC - command line tool](#control-unisense-from-a-pc---command-line-tool)
- Execute the command `./bhy dfu -p YOUR_PORT -t TARGET -b BIN_FILE`
    * Replace `YOUR_PORT` with the serial port used by the arduino board
    * Replace `TARGET` with `unisense` or `bhi`
    * Replace `BIN_FILE` with the path of the binary file to bu uploaded
- Unisense will reset and the firmware will be updated
