package util

import (
	"log"

	"go.bug.st/serial"
)

func OpenPort(usbPort string, baudRate int) serial.Port {
	mode := &serial.Mode{
		BaudRate: baudRate,
		Parity:   serial.NoParity,
		DataBits: 8,
		StopBits: serial.OneStopBit,
	}
	port, err := serial.Open(usbPort, mode)
	ErrCheck(err)
	return port
}

func ErrCheck(e error) {
	if e != nil {
		log.Fatal(e)
	}
}
