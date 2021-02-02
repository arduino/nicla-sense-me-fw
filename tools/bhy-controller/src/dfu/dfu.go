package dfu

import (
	"encoding/binary"
	"fmt"
	"go.bug.st/serial"
	"log"
	"os"
)

/*Packet format:
- 1 byte: OPCODE
- 1 byte: LASTBYTE
- 2 bytes: LENGTH/REMAINING
- 64 bytes: DATA
*/

var crc8bit byte
var Ack = (byte)(0x0F)
var Nack = (byte)(0x00)
var packSize = 64
var FullPackSize = packSize + 4

func CRC8(buf []byte) {

	b1 := buf[0]
	b2 := buf[1]

	crc8bit = b1 ^ b2

	for i := 2; i < packSize; i++ {
		b1 = buf[i]
		crc8bit = crc8bit ^ b1
	}

}

func Upload(baud_rate int, oc int, USBport string, bin_path string) {
	oc16 := (uint16)(oc)
	opcode := make([]byte, 2)

	binary.LittleEndian.PutUint16(opcode, oc16)

	last := make([]byte, 1)
	idx := make([]byte, 2)

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

	//Open the firmware binary file and get the length
	f, err := os.Open(bin_path)
	check(err)
	fi, err := f.Stat()
	check(err)
	fw_size := fi.Size()
	fmt.Printf("The firware is %d bytes long\n", fw_size)

	buf := make([]byte, packSize)

	nChunks := int(fw_size / ((int64)(packSize)))
	fmt.Printf("nChunks: %d\n", nChunks)
	spareBytes := uint16(fw_size % ((int64)(packSize)))
	fmt.Printf("spareBytes: %d\n", spareBytes)

	for n := 0; n <= nChunks; n++ {

		if n < nChunks || spareBytes > 0 {
			//Read packSize bytes from the binary file
			_, err := f.Read(buf)
			check(err)

			//update the CRC at each chunk of packSize bytes read
			CRC8(buf)
		}

		index := (uint16)(n)

		if n == nChunks { //Last packet
			last[0] = 1
			//Add 8-bit CRC to len
			binary.LittleEndian.PutUint16(idx, (spareBytes + 1))
			index = spareBytes + 1
			//Add CRC to data
			buf[spareBytes] = crc8bit
		} else {
			last[0] = 0
			binary.LittleEndian.PutUint16(idx, index)
		}

		fmt.Printf("opcode: %d\n", opcode[1:])
		fmt.Printf("lastPack: %d\n", last)
		fmt.Printf("index: %d\n", index)
		for j := 0; j < packSize; j++ {
			fmt.Printf("%x, ", buf[j])
		}

		header := make([]byte, 3)
		header = append(last, idx[:2]...)
		opcodeHeader := make([]byte, 4)
		opcodeHeader = append(opcode[1:], header...)
		packet := make([]byte, FullPackSize)
		packet = append(opcodeHeader, buf...)

		ackReceived := false

		for ackReceived == false {
			_, err = port.Write(packet)
			check(err)
			fmt.Printf("Packet sent to MKR\n")

			ackBuf := make([]byte, 1)

			//wait response from Unisense
			for {
				bytesAck, err := port.Read(ackBuf)
				if err != nil {
					log.Fatal(err)
				}

				if bytesAck == 1 {
					if ackBuf[0] == Ack { //Unisense correctly received the packet
						ackReceived = true
						fmt.Printf("ACK %x received!\n", ackBuf[0])
						break
					}
					if ackBuf[0] == Nack { //Unisense did NOT correctly received the packet
						//keep ackReceived = false and resend
						fmt.Printf("NACK %x received!\n", ackBuf[0])
						break
					}
					fmt.Printf("ERROR! Unknown ACK format: %x\n", ackBuf[0])
					break
				}
			}

		}

		if n != nChunks {
			//Adjust pointer to fw binary file
			p := (int64)((n + 1) * packSize)
			_, err = f.Seek(p, 0)
			check(err)
		}
	}

	defer f.Close()

	fmt.Printf("FW sent!\n")

}

func check(e error) {
	if e != nil {
		panic(e)
	}
}
