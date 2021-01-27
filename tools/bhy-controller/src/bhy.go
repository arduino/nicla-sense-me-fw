package main

import (
	"flag"
	"fmt"
	"os"
	"arduino/bhy/dfu"
)

func main() {
	// commands subcommands and flags definition
	// dfu command is used for uploading firmware to unisense
	dfuCommand := flag.NewFlagSet("dfu", flag.ExitOnError)
	// upload flags
	baudRate := dfuCommand.Int("baud", 115200, "baud rate")
	opCode := dfuCommand.Int("o", 0, "op code")
	usbPort := dfuCommand.String("p", "", "usb port")
	binPath := dfuCommand.String("bin", "", "binary path")

	// sensor command is used to control bhy sensors
	sensorCommand := flag.NewFlagSet("sensor", flag.ExitOnError)
	// sensor subcommands
	readCommand := flag.NewFlagSet("read", flag.ExitOnError)
	configureCommand := flag.NewFlagSet("configure", flag.ExitOnError)
	// sensor configuration flags
	sensorId := configureCommand.Int("sensor", 0, "Sensor ID to configure")
	sensorRate := configureCommand.Float64("rate", 0, "Rate of sample")
	sensorLatency := configureCommand.Float64("latency", 0, "Latency")

	// Check if a subcommand has been passed
	if len(os.Args) < 2 {
		commandError()
	}

	// Parse the subcommand
	switch os.Args[1] {
	case "dfu":
		dfuCommand.Parse(os.Args[2:])
		ret := dfuCheck(*baudRate, *opCode, *usbPort, *binPath)
		if !ret {
			dfuCommand.PrintDefaults()
			os.Exit(1)
		}
		dfu.Upload(*baudRate, *opCode, *usbPort, *binPath)

	case "sensor":
		// sensor subcommand requires an additional subcommand
		if len(os.Args) < 3 {
			sensorError()
		}
		// parse the additional subcommand
		switch os.Args[2] {
		case "read":
			readCommand.Parse(os.Args[3:])

		case "configure":
			// Check that at least one flag is passed - only sensor id is mandatory
			if len(os.Args) < 4 { 
				configureCommand.PrintDefaults()
				os.Exit(1)
			}
			configureCommand.Parse(os.Args[3:])
			sensor(*sensorId, *sensorRate, *sensorLatency)

		default:
			sensorCommand.PrintDefaults()
			sensorError()
		}

	default:
		commandError()
		os.Exit(1)
	}
}

func dfuCheck(baudRate int, opCode int, usbPort string, binPath string) bool {
	if binPath == "" {
		fmt.Println("")
		fmt.Println(" missing -bin parameter, provide a valid binary path")
		fmt.Println("")
		return false
	} else {
		if _, err := os.Stat(binPath); os.IsNotExist(err) {
			fmt.Println("")
			fmt.Println("-bin parameter is not valid, the binary provided is NOT ACCESSIBLE")
			fmt.Println("")
			return false
		}
	}
	if usbPort == "" {
		fmt.Println("")
		fmt.Println(" missing -p parameter, provide a valid serial port")
		fmt.Println("")
		return false
	}
	return true
}

func sensor(id int, lat float64, rate float64) {

}

func commandError() {
	fmt.Println(" A command is required")
	fmt.Println("	sensor")
	fmt.Println("		to control bhy sensors")
	fmt.Println("	dfu")
	fmt.Println("		to upload new firmware for unisense or bhy")
	os.Exit(1)
}

func sensorError() {
	fmt.Println(" A sensor subcommand is required")
	fmt.Println("	read")
	fmt.Println("		to read last sensor data")
	fmt.Println("	config")
	fmt.Println("		to configure a bhy sensor")
	os.Exit(1)
}
