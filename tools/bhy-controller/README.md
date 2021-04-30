# bhy-controller commands

When a command is called without arguments, the possible subcommands and/or flags are printed

Example: 
```
./bhy  
```
will result in:
```
 A command is required
        sensor
                to control bhy sensors
        dfu
                to upload new firmware for nicla or bhy
        list
                list available serial ports
        webserver
                start a local webserver and open the webserver
```

List of useful commands:
```bash
# list available serial ports
./bhy list

# start the local webserver and open the main webpage
./bhy webserver

# read available sensor data
./bhy sensor read -p /dev/ttyACM2

# continuously read sensor data when available
./bhy sensor read -live -p /dev/ttyACM2

# configure sensor 10 with a sample rate of 1 Hz and latency of 0ms
./bhy sensor config -p /dev/ttyACM2 -sensor 10 -rate 1 -latency 0

# disable sensor 10
./bhy sensor config -p /dev/ttyACM2 -sensor 10 -rate 0 -latency 0

# update bhi firmware
./bhy dfu -t nicla -bin fw.bin -p /dev/ttyACM2

# update Nicla firmware
./bhy dfu -t bhi -bin fw.bin -p /dev/ttyACM2
```
