package dfu

import "encoding/binary"

const (
	Ack         = (byte)(0x0F)
	Nack        = (byte)(0x00)
	PayloadSize = 232
	PackSize    = PayloadSize + 4
)

type dfuPacket struct {
	opcode  uint8
	last    uint8
	index   uint16
	length  uint16
	payload []byte
}

func newDFUPacket(target string, i int, len int, p []byte) (d *dfuPacket) {
	d = &dfuPacket{
		index:   uint16(i),
		length:  uint16(len),
		payload: p,
	}
	d.setOpcode(target)
	return
}

func (d *dfuPacket) build() []byte {
	b := append([]byte{d.opcode}, byte(d.last))

	if d.last == 1 {
		len := make([]byte, 2)
		binary.LittleEndian.PutUint16(len, (d.length))
		b = append(b, len...)
	} else {
		idx := make([]byte, 2)
		binary.LittleEndian.PutUint16(idx, (d.index))
		b = append(b, idx...)
	}

	b = append(b, d.payload...)

	return b
}

func (d *dfuPacket) setPayload(p []byte) {
	d.payload = p
}

func (d *dfuPacket) setOpcode(target string) {
	oc := 0
	if target == "bhi" {
		oc = 1
	}
	d.opcode = uint8(oc)
}

func (d *dfuPacket) setLast(l int) {
	d.last = uint8(l)
}

func (d *dfuPacket) setIndex(i int) {
	d.index = uint16(i)
}

func (d *dfuPacket) setLength(l int) {
	d.length = uint16(l)
}
