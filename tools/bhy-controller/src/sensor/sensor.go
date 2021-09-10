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

const (
	Ack         = (byte)(0x0F)
	Nack        = (byte)(0x00)
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

func LoadSensors() {
	scheme = loadScheme()
	types = loadTypes()
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
    // Some sensors have a frame size bigger than 10 (such as Sensor id 171)
    // but the Arduino_BHY2 library only copies up to 10 bytes (SENSOR_DATA_FIXED_LENGTH in SensorTypes.h)
    // otherwise without this size check the tool would crash for such sensors
    if data.size > 10 {
        data.size = 10
    }

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

func sendConfig(port serial.Port, s *SensorConfig) {
	for ackReceived := false; ackReceived == false; {

		fmt.Printf("Sending configuration: sensor %d	rate %f	latency %d", s.id, s.sampleRate, s.latency)
		n, err := port.Write(s.pack())
		errCheck(err)
		fmt.Printf("Sent %v bytes\n", n)

		//wait ack from Nicla
		ackBuf := make([]byte, 1)
		for n := 0; n != 1; {
			n, err = port.Read(ackBuf)

			if n == 0 {
				fmt.Printf("Ack not received\n")
			} else if n > 1 {
				fmt.Printf("Ack expected but more than one byte received\n")
			}
		}

		ackReceived = (ackBuf[0] == Ack)
		if ackBuf[0] == Ack {
			fmt.Printf("Sensor configuration correctly sent!\n")
		}
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

	sendConfig(port, s)
}
