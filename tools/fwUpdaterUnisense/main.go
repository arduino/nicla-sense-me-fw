package main

import (
	"fmt"
	"log"
	"os"
	"strconv"

	"go.bug.st/serial"
)

/*Packet format:
- 1 byte: OPCODE
- 1 byte: LASTBYTE
- 2 bytes: LENGTH/REMAINING
- 64 bytes: DATA
*/
type packet_s struct {
	opcode   byte
	lastPack byte
	index    uint16
	data     []byte
}

var crc8bit byte
var Ack = (byte)(0x0F)
var Nack = (byte)(0x00)

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func CRC8(buf []byte) {

	b1 := buf[0]
	b2 := buf[1]

	crc8bit = b1 ^ b2

	for i := 2; i < 64; i++ {
		b1 = buf[i]
		crc8bit = crc8bit ^ b1
	}

}

func main() {
	bin_path := os.Args[1]
	USBport := os.Args[2]
	baud_rate, _ := strconv.Atoi(os.Args[3])
	oc, _ := strconv.Atoi(os.Args[4])
	opcode := byte(oc)

	mode := &serial.Mode{
		BaudRate: baud_rate,
		Parity:   serial.NoParity,
		DataBits: 8,
		StopBits: serial.OneStopBit,
	}

	port, err := serial.Open(USBport, mode)
	if err != nil {
		log.Fatal(err)
	}

	//Opcode will be unique for all the packets
	packet := packet_s{opcode: opcode}

	//Open the firmware binary file and get the length
	f, err := os.Open(bin_path)
	check(err)
	fi, err := f.Stat()
	check(err)
	fw_size := fi.Size()
	fmt.Printf("The firware is %d bytes long\n", fw_size)

	buf := make([]byte, 64)

	nChunks := int(fw_size / 64)
	spareBytes := uint16(fw_size % 64)

	for n := 0; n <= nChunks; n++ {

		if n < nChunks || spareBytes > 0 {
			//Read 64 bytes from the binary file
			_, err := f.Read(buf)
			check(err)

			//update the CRC at each chunk of 64 bytes read
			CRC8(buf)
		}

		if n == nChunks { //Last packet
			packet.lastPack = 1
			//Add 8-bit CRC to len
			packet.index = spareBytes + 1
			//Add CRC to data
			buf[spareBytes] = crc8bit
		} else {
			packet.lastPack = 0
			packet.index = (uint16)(n)
		}

		packet.data = buf[:64]

		ackReceived := false

		for ackReceived == false {
			//Send packet
			_, err = port.Write(packet.data)
			if err != nil {
				log.Fatal(err)
			}

			ackBuf := make([]byte, 1)

			//wait response from Unisense
			for {
				bytesAck, err := port.Read(ackBuf)
				if err != nil {
					log.Fatal(err)
				}

				if bytesAck == 0 {
					continue
				}

				if bytesAck == 1 {
					if ackBuf[0] == Ack { //Unisense correctly received the packet
						ackReceived = true
						break
					}
					if ackBuf[0] == Nack { //Unisense did NOT correctly received the packet
						//keep ackReceived = false and resend
						break
					}
					fmt.Printf("ERROR! Unknows ACK format: %x\n", ackBuf[0])
					break
				} else {
					fmt.Printf("ERROR! Received %d bytes from Unisense. Only 1 was expected\n", bytesAck)
					break
				}
			}

		}

		if n != nChunks {
			//Adjust pointer to fw binary file
			p := (int64)((n+1)*64 + 1)
			_, err = f.Seek(p, 0)
			check(err)
		}
	}

	defer f.Close()

	fmt.Printf("FW sent!\n")

}
