package main

import (
	"encoding/binary"
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

var crc8bit byte
var spareBytes uint16
var Ack = (byte)(0x0F)
var Nack = (byte)(0x00)
var packSize = 64
var FullPackSize = packSize + 4

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func CRC8(buf []byte, last_pack bool) {

	buf_len := packSize

	if last_pack {
		buf_len = (int)(spareBytes)
	}

	for i := 0; i < buf_len; i++ {
		b := buf[i]
		crc8bit = crc8bit ^ b
	}

}

func main() {
	argCount := len(os.Args[1:])
	if argCount < 4 {
		fmt.Printf("Usage: ./fwUpdaterUnisense PATH_TO_BIN USB_PORT UPDATE_OPCODE DEBUG\n")
		fmt.Printf("                           PATH_TO_BIN: full path to Firmware Update Binary File\n")
		fmt.Printf("                           USB_PORT: USB port with 115200 baude rate connected to PC\n")
		fmt.Printf("                           UPDATE_OPCODE: 0 for ANNA fw update - 1 for BHY2 fw update\n")
		fmt.Printf("                           DEBUG: 1 print debug messages - 0 print only main messages\n")
		panic("Missing one or more input parameters!")
	}
	bin_path := os.Args[1]
	USBport := os.Args[2]
	oc, _ := strconv.Atoi(os.Args[3])
	if oc > 1 {
		panic("Invalid UPDATE_OPCODE parameter")
	}
	debug, _ := strconv.Atoi(os.Args[4])
	if debug > 1 {
		panic("Invalid DEBUG parameter")
	}

	oc16 := (uint16)(oc)
	opcode := make([]byte, 2)

	binary.LittleEndian.PutUint16(opcode, oc16)

	last := make([]byte, 1)
	idx := make([]byte, 2)

	mode := &serial.Mode{
		BaudRate: 115200,
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
	spareBytes = uint16(fw_size % ((int64)(packSize)))
	fmt.Printf("spareBytes: %d\n", spareBytes)

	crc8bit = 0

	for n := 0; n <= nChunks; n++ {

		if n < nChunks || spareBytes > 0 {
			//Read packSize bytes from the binary file
			_, err := f.Read(buf)
			check(err)

			//update the CRC at each chunk of packSize bytes read
			if n == nChunks {
				CRC8(buf, true)
			} else {
				CRC8(buf, false)
			}
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

		if debug == 1 {
			fmt.Printf("opcode: %d\n", opcode[1:])
			fmt.Printf("lastPack: %d\n", last)
			fmt.Printf("index: %d\n", index)
			for j := 0; j < packSize; j++ {
				fmt.Printf("%x, ", buf[j])
			}
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
			if debug == 1 {
				fmt.Printf("Packet sent to MKR\n")
			}

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
						if debug == 1 {
							fmt.Printf("ACK %x received!\n", ackBuf[0])
						}
						break
					}
					if ackBuf[0] == Nack { //Unisense did NOT correctly received the packet
						//keep ackReceived = false and resend
						if debug == 1 {
							fmt.Printf("NACK %x received!\n", ackBuf[0])
						}
						break
					}
					if debug == 1 {
						fmt.Printf("ERROR! Unknown ACK format: %x\n", ackBuf[0])
					}
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
