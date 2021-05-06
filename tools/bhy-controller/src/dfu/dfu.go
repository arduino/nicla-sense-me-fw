package dfu

import (
	"arduino/bhy/util"
	"fmt"
	"log"
	"math"
	"os"

	"github.com/schollz/progressbar/v3"
	"go.bug.st/serial"
)

func sendPacket(port serial.Port, packet *dfuPacket) {
	for ackReceived := false; ackReceived == false; {
		log.Printf("Sending packet %d", packet.index)
		_, err := port.Write(packet.build())
		log.Println(packet.build())
		util.ErrCheck(err)

		//wait ack from Nicla
		ackBuf := make([]byte, 1)
		for n := 0; n != 1; {
			n, err = port.Read(ackBuf)
			util.ErrCheck(err)

			if n == 0 {
				log.Print("Ack not received")
			} else if n > 1 {
				log.Print("Ack expected but more than one byte received")
			}
		}

		ackReceived = (ackBuf[0] == Ack)
	}
}

func Upload(baud int, target string, usbPort string, bPath string, debug int) {
	// Open serial port
	port := util.OpenPort(usbPort, baud)

	//Open the firmware binary file and get the length
	f, err := os.Open(bPath)
	defer f.Close()
	util.ErrCheck(err)
	fi, err := f.Stat()
	util.ErrCheck(err)
	fwSize := fi.Size()
	fmt.Printf("The firmware is %d bytes long\n", fwSize)

	// consider fwSize + 1 for crc byte
	nChunks := int(math.Ceil(float64(fwSize+1) / float64(PayloadSize)))
	spareBytes := int(fwSize % int64(PayloadSize))
	bar := progressbar.Default(int64(nChunks))
	crc := uint8(0)
	buf := make([]byte, PayloadSize)

	for i := 0; i < nChunks; i++ {
		bar.Add(1)

		//Adjust pointer to fw binary file
		p := (int64)(i * PayloadSize)
		_, err = f.Seek(p, 0)
		util.ErrCheck(err)

		// Read payload from the binary file
		n, err := f.Read(buf)
		util.ErrCheck(err)
		crc = CRC8(buf, n, crc)

		packet := newDFUPacket(target, i, n, buf)
		// if this is the very last packet
		if i == (nChunks - 1) {
			packet.setLast(1)
			// assert n == spareBytes
			// append crc to this packet
			buf[spareBytes] = crc
			packet.setLength(spareBytes + 1)
		}
		sendPacket(port, packet)
	}

	bar.Finish()
	fmt.Printf("FW sent!\n")
}
