package main

import (
	"arduino/bhy/dfu"
	"arduino/bhy/sensor"
	"flag"
	"fmt"
	"go.bug.st/serial/enumerator"
	"log"
	"os"
)

func main() {
	// Check if a subcommand has been passed
	if len(os.Args) < 2 {
		commandError()
		return
	}
	// Parse the subcommand
	switch os.Args[1] {
	case "list":
		listCommand()
	case "dfu":
		dfuCommand()
	case "sensor":
		sensorCommand()
	default:
		commandError()
	}
}

func dfuCommand() {
	// dfu command is used for uploading firmware to unisense
	dfuFlags := flag.NewFlagSet("dfu", flag.ExitOnError)
	usbPort := dfuFlags.String("p", "", "usb port")
	baudRate := dfuFlags.Int("baud", 115200, "baud rate")
	opCode := dfuFlags.Int("o", 0, "op code")
	binPath := dfuFlags.String("bin", "", "binary path")

	dfuFlags.Parse(os.Args[2:])

	ret := dfuCheckFlags(*baudRate, *opCode, *usbPort, *binPath)
	if !ret {
		dfuFlags.PrintDefaults()
		return
	}

	dfu.Upload(*baudRate, *opCode, *usbPort, *binPath)
}

func sensorCommand() {
	// sensor subcommand requires an additional subcommand
	if len(os.Args) < 3 {
		sensorError()
		return
	}
	// parse the additional subcommand
	switch os.Args[2] {
	case "read":
		sensorReadCommand()
	case "config":
		sensorConfigureCommand()
	default:
		sensorError()
	}
}

func sensorReadCommand() {
	readFlags := flag.NewFlagSet("read", flag.ExitOnError)
	usbPort := readFlags.String("p", "", "usb port")
	baudRate := readFlags.Int("baud", 115200, "baud rate")

	readFlags.Parse(os.Args[3:])

	ret := sensorCheckFlags(*usbPort)
	if !ret {
		readFlags.PrintDefaults()
		return
	}

	sensor.Read(*usbPort, *baudRate)
}

func sensorConfigureCommand() {
	configureFlags := flag.NewFlagSet("configure", flag.ExitOnError)
	usbPort := configureFlags.String("p", "", "usb port")
	baudRate := configureFlags.Int("baud", 115200, "baud rate")
	sensorId := configureFlags.Int("sensor", 0, "Sensor ID to configure")
	sensorRate := configureFlags.Float64("rate", 0, "Rate of sample")
	sensorLatency := configureFlags.Float64("latency", 0, "Latency")

	configureFlags.Parse(os.Args[3:])

	ret := sensorCheckFlags(*usbPort)
	if !ret {
		configureFlags.PrintDefaults()
		return
	}

	sensor.Configure(*usbPort, *baudRate, *sensorId, *sensorRate, *sensorLatency)
}

func listCommand() {
	ports, err := enumerator.GetDetailedPortsList()
	if err != nil {
		log.Fatal(err)
	}
	if len(ports) == 0 {
		fmt.Println("No serial ports found!")
		return
	}
	for _, port := range ports {
		fmt.Printf("Found port: %s\n", port.Name)
		if port.IsUSB {
			fmt.Printf("   USB ID     %s:%s\n", port.VID, port.PID)
			fmt.Printf("   USB serial %s\n", port.SerialNumber)
		}
	}
}

func dfuCheckFlags(baudRate int, opCode int, usbPort string, binPath string) bool {
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

func sensorCheckFlags(usbPort string) bool {
	if usbPort == "" {
		fmt.Println("")
		fmt.Println(" missing -p parameter, provide a valid serial port")
		fmt.Println("")
		return false
	}
	return true
}

func commandError() {
	fmt.Println(" A command is required")
	fmt.Println("	sensor")
	fmt.Println("		to control bhy sensors")
	fmt.Println("	dfu")
	fmt.Println("		to upload new firmware for unisense or bhy")
	fmt.Println("	list")
	fmt.Println("		list available serial ports")
}

func sensorError() {
	fmt.Println(" A sensor subcommand is required")
	fmt.Println("	read")
	fmt.Println("		to read last sensor data")
	fmt.Println("	config")
	fmt.Println("		to configure a bhy sensor")
}
