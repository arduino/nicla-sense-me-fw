package sensor

import (
	"encoding/binary"
	"fmt"
	"log"
	"math"
	"time"

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

func (s SensorConfig) pack() []byte {
	// Prepare configuration packet to send
	op := []byte{eslovConfigOpcode}
	id := byte(s.id)
	rate := make([]byte, 4)
	binary.LittleEndian.PutUint32(rate, math.Float32bits(s.sampleRate))
	lat := make([]byte, 4)
	binary.LittleEndian.PutUint32(lat, s.latency)

	packet := append(op, id)
	packet = append(packet, rate...)
	packet = append(packet, lat...)

	return packet
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
	var dataPacket [sensorDataSize]byte
	for rxLen := 0; rxLen < sensorDataSize; {
		n, err := port.Read(buffer)
		errCheck(err)
		copy(dataPacket[rxLen:sensorDataSize], buffer[:n])
		rxLen = rxLen + n
	}
	// Print the received sensor data
	var data SensorData
	data.id = uint8(dataPacket[0])
	data.size = uint8(dataPacket[1])
	data.data = dataPacket[2:(2 + data.size)]
	parseData(&data)
}

func liveRead(port serial.Port) {
	for {
		singleRead(port, false)
		time.Sleep(300 * time.Millisecond)
	}
}

func singleRead(port serial.Port, printEnable bool) {
	// Send read opcode
	packet := []byte{eslovReadOpcode}
	n, err := port.Write(packet)
	errCheck(err)
	if printEnable {
		fmt.Printf("Sent %v bytes\n", n)
	}

	// Read bhy available data
	buff := make([]byte, 100)
	n, err = port.Read(buff)
	errCheck(err)

	// If no available data, just return
	availableData := int(buff[0])
	if n == 0 || availableData == 0 {
		if printEnable {
			fmt.Println("no available data")
		}
		return
	}
	// Else query all the available data
	if printEnable {
		fmt.Printf("available data: %d\n", availableData)
	}
	for availableData > 0 {
		readSensorData(buff, port)
		availableData--
	}
}

func Read(usbPort string, baudRate int, liveFlag bool) {
	port := openPort(usbPort, baudRate)
	defer port.Close()
	fmt.Printf("Connected - port: %s - baudrate: %d\n", usbPort, baudRate)

	if liveFlag {
		liveRead(port)
	} else {
		singleRead(port, true)
	}
}

func Configure(usbPort string, baudRate int, id int, rate float64, latency int) {
	port := openPort(usbPort, baudRate)
	defer port.Close()
	fmt.Printf("Connected - port: %s - baudrate: %d\n", usbPort, baudRate)

	s := &SensorConfig{
		id:         uint8(id),
		sampleRate: float32(rate),
		latency:    uint32(latency),
	}

	fmt.Printf("Sending configuration: sensor %d	rate %f	latency %d", s.id, s.sampleRate, s.latency)
	n, err := port.Write(s.pack())
	errCheck(err)
	fmt.Printf("Sent %v bytes\n", n)
}
