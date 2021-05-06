package dfu

type crc8 struct {
	value uint8
}

func (c *crc8) reset() {
	c.value = uint8(0)
}

func (c *crc8) byte() byte {
	return byte(c.value)
}

func (c *crc8) compute(buf []byte, len int) {
	for i := 0; i < len; i++ {
		b := buf[i]
		c.value = c.value ^ b
	}
}

func CRC8(buf []byte, len int, crc uint8) uint8 {
	for i := 0; i < len; i++ {
		b := buf[i]
		crc = crc ^ b
	}
	return crc
}
