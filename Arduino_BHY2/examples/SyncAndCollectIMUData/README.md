# Overview
This example shows how to stream sensor data with very high data rate (up to 1600hz for both accel and gyro) through the USB Serial interface to the host (PC)

#prerequisite
## Host Side
To stream both accel and gyro data at high frequency (e.g.: 800hz or above),
the host PC needs to be able to support USB serial baud rate of 1Mbps or at least 921600bps which are widely available for regular PCs,
however, it is observed that on Mac OS based devices, the baud rate seems to be capped at 230400bps.

If only one sensor needs to be streamed, the clock rate on the host side could be relaxed by about a half.

## Device Side
The default device FW for the BHI260AP device supports sensor data output rate of up to 400hz, to support higher rates, the firmware needs to be updated,
and for the convenience of the users, a stock firmware ("NiclaSenseME-nobsx-1600hz-IMU-passthrough-flash.fw") for this purpose has been provided at the same 
folder with this README, please refer to the example sketch BHYFirmwareUpdate within the Arduino_BHY2 library for the steps of updating the BHI260AP firmware.

# The "tools" Folder
For reliable data transfer at high data rates, the data is encoded in base64 format, and to help convert the data (captured at the host side) back to its original format, some helper tools have been provided for the users' convenience.
Please refer to the README.md under the "tools" folder for more details.
