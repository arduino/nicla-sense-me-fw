package sensor

import (
	"fmt"
	"log"
	"go.bug.st/serial"
	"encoding/binary"
	"math"
)

type SensorData struct{
  id uint8
  size uint8 
  data uint64 
}

type SensorConfig struct{
  id uint8
  sampleRate float64 
  latency uint32
}

func errCheck(e error) {
	if e != nil {
		log.Fatal(e)
	}
}

func openPort(usbPort string, baudRate int) (serial.Port) {
	mode := &serial.Mode{
		BaudRate: baudRate,
		Parity:   serial.NoParity,
		DataBits: 8,
		StopBits: serial.OneStopBit,
	}
	port, err := serial.Open(usbPort, mode)
	errCheck(err)
	return port
}

func Read(usbPort string, baudRate int) {
	port := openPort(usbPort, baudRate)
	fmt.Printf("Connected - port: %s - baudrate: %d\n", usbPort, baudRate)

	// Send read opcode
	packet := []byte{2}
	n, err := port.Write(packet)
	errCheck(err)
	fmt.Printf("Sent %v bytes\n", n)

	// Read bhy available data
	buff := make([]byte, 100)
	n, err = port.Read(buff)
	errCheck(err)

	// If no available data, just return
	availableData := int(buff[0])
	if n == 0 || availableData == 0 {
		fmt.Println("no available data")
		port.Close()
		return
	}
	// Else query all the available data
	fmt.Printf("available data: %d\n", availableData)
	for availableData > 0 {
		n, err := port.Read(buff)
		errCheck(err)
		if n != 10 {
			fmt.Println("data not valid")
		} else {
			// Print the received sensor data 
			var data SensorData
			data.id = uint8(buff[0])
			data.size = uint8(buff[1])
			data.data = uint64(binary.LittleEndian.Uint64(buff[2:10]))
			fmt.Printf("sensor: %d	size: %d	data: %d\n", data.id, data.size, data.data)
		}
		availableData--
	}
	port.Close()
}

func Configure(usbPort string, baudRate int, id int, rate float64, latency int) {
	port := openPort(usbPort, baudRate)
	fmt.Printf("Connected - port: %s - baudrate: %d\n", usbPort, baudRate)

	// Fill a configuration struct just for clarity
	var config SensorConfig
	config.id = uint8(id)
	config.sampleRate = rate
	config.latency = uint32(latency)
	// Prepare configuration packet to send
	opCode := []byte{3}
	cId := []byte{config.id}
	cSample := make([]byte, 8)
	binary.LittleEndian.PutUint64(cSample[:], math.Float64bits(config.sampleRate))
	cLatency := make([]byte, 4)
	binary.LittleEndian.PutUint32(cLatency, config.latency)
	var packet []byte
	packet = append(opCode, cId...)
	packet = append(packet, cSample...)
	packet = append(packet, cLatency...)

	fmt.Printf("Sending configuration: sensor %d	rate %f	latency %d", config.id, config.sampleRate, config.latency)
	//fmt.Println(packet)
	n, err := port.Write(packet)
	errCheck(err)
	fmt.Printf("Sent %v bytes\n", n)
	
	port.Close()
}