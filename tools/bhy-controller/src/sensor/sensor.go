package sensor

import (
	"encoding/binary"
	"fmt"
	"log"
	"math"

	"go.bug.st/serial"
)

const (
	eslovReadOpcode   = 2
	eslovConfigOpcode = 3
	sensorDataSize    = 12
)

type SensorData struct {
	id   uint8
	size uint8
	data []byte
}

type SensorConfig struct {
	id         uint8
	sampleRate float32
	latency    uint32
}

func errCheck(e error) {
	if e != nil {
		log.Fatal(e)
	}
}

func openPort(usbPort string, baudRate int) serial.Port {
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

func readSensorData(buffer []byte, port serial.Port) {
	n, err := port.Read(buffer)
	errCheck(err)
	if n != sensorDataSize {
		fmt.Println("data not valid")
	} else {
		// Print the received sensor data
		var data SensorData
		data.id = uint8(buffer[0])
		data.size = uint8(buffer[1])
		//data.data = uint64(binary.LittleEndian.Uint64(buffer[2:10]))
		data.data = buffer[2:(2 + data.size)]
		//copy(data.data[:], buffer[2:(2+data.size)])
		fmt.Printf("sensor: %d	size: %d	data: ", data.id, data.size)
		fmt.Println(data.data)
	}
}

func Read(usbPort string, baudRate int) {
	port := openPort(usbPort, baudRate)
	defer port.Close()
	fmt.Printf("Connected - port: %s - baudrate: %d\n", usbPort, baudRate)

	// Send read opcode
	packet := []byte{eslovReadOpcode}
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
		return
	}
	// Else query all the available data
	fmt.Printf("available data: %d\n", availableData)
	for availableData > 0 {
		readSensorData(buff, port)
		availableData--
	}
}

func Configure(usbPort string, baudRate int, id int, rate float64, latency int) {
	port := openPort(usbPort, baudRate)
	defer port.Close()
	fmt.Printf("Connected - port: %s - baudrate: %d\n", usbPort, baudRate)

	// Fill a configuration struct just for clarity
	var config SensorConfig
	config.id = uint8(id)
	config.sampleRate = float32(rate)
	config.latency = uint32(latency)
	// Prepare configuration packet to send
	opCode := []byte{eslovConfigOpcode}
	cId := []byte{config.id}
	cSample := make([]byte, 4)
	binary.LittleEndian.PutUint32(cSample[:], math.Float32bits(config.sampleRate))
	cLatency := make([]byte, 4)
	binary.LittleEndian.PutUint32(cLatency, config.latency)
	var packet []byte
	packet = append(opCode, cId...)
	packet = append(packet, cSample...)
	packet = append(packet, cLatency...)

	fmt.Printf("Sending configuration: sensor %d	rate %f	latency %d", config.id, config.sampleRate, config.latency)
	n, err := port.Write(packet)
	errCheck(err)
	fmt.Printf("Sent %v bytes\n", n)
}
