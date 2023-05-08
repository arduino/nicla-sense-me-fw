#define IS31FL3194_ADDRESS 0x53

#define IS31FL3194_OP_CONFIG            0x01
#define IS31FL3194_OUT_CONFIG           0x02
#define IS31FL3194_CURRENT_BAND         0x03
#define IS31FL3194_HOLD_FUNCTION        0x04

// Current Mode
#define IS31FL3194_OUT1                 0x10
#define IS31FL3194_OUT2                 0x21
#define IS31FL3194_OUT3                 0x32

#define IS31FL3194_COLOR_UPDATE         0x40

#define IS31FL3194_RESET                0x4F

// allowed colors
#define red     0
#define green   1
#define blue    2
#define yellow  3
#define magenta 4
#define cyan    5

#include <mbed.h>
#include <I2C.h>

mbed::I2C i2c(I2C_SDA0, I2C_SCL0);

uint8_t _out1 = 0;
uint8_t _out2 = 0;
uint8_t _out3 = 0;

void setup() {
  // reset
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_RESET, 0xC5);

  // init
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OP_CONFIG, 0x01);     // normal operation in current mode
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT_CONFIG, 0x07);    // enable all three ouputs
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_CURRENT_BAND, 0x00);  // 10 mA max current
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_HOLD_FUNCTION, 0x00); // hold function disable

  // powerUp
  uint8_t d = readByte(IS31FL3194_ADDRESS, IS31FL3194_OP_CONFIG);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OP_CONFIG, d | 0x01); //set bit 0 to enable

}

void loop() {
  //cycle through LED colour configurations
  ledBlink(green, 1000);    
  delay(1000);
  ledBlink(blue, 1000);     
  delay(1000);
  ledBlink(red, 1000);
  delay(1000);
  ledBlink(cyan, 1000);
  delay(1000);
  ledBlink(magenta, 1000);    
  delay(1000);
  ledBlink(yellow, 1000);     
  delay(1000);
}

/**
 * Blink a given color for a set duration in milliseconds. 
*/
void ledBlink(uint8_t color, uint32_t duration)
/**
 * Intensity is defined in 256 levels according to table 7 of the IS31FL3194 datasheet:
 * 0x00 -> 0
 * 0x20 -> I_max * (2^7) / 256 = I_max  * 1/2
 * 0xFF -> I_max * (2^7 + 2^6 + 2^5 + 2^4 + 2^3 + 2^2 + 2^1 + 2^0) / 255 ~= I_max
*/
{
  if(color == green) {
  _out1 = 0x00;
  _out2 = 0xFF;
  _out3 = 0x00;
  }

  if(color == blue) {
  _out1 = 0xFF;
  _out2 = 0x00;
  _out3 = 0x00;
  }

  if(color == red) {
  _out1 = 0x00;
  _out2 = 0x00;
  _out3 = 0xFF;
  }

  if(color == cyan) {
  _out1 = 0x20;
  _out2 = 0x20;
  _out3 = 0x00;
  }

  if(color == magenta) {
  _out1 = 0x20;
  _out2 = 0x00;
  _out3 = 0x20;
  }

  if(color == yellow) {
  _out1 = 0x00;
  _out2 = 0x20;
  _out3 = 0x20;
  }


  // set rgb led current
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT1, _out1); //maximum current
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT2, _out2);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT3, _out3);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_COLOR_UPDATE, 0xC5); // write to color update register for changes to take effect
  delay(duration);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT1, 0x00); //maximum current
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT2, 0x00);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT3, 0x00);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_COLOR_UPDATE, 0xC5); // write to color update register for changes to take effect
}

void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  char command[2];
  command[0] = subAddress;
  command[1] = data;
  i2c.write(address << 1, command, 2);
}


uint8_t readByte(uint8_t address, uint8_t subAddress)
{
  char response = 0xFF;
  int ret = i2c.write(address << 1, (const char*)&subAddress, 1);
  ret = i2c.read(address << 1, &response, 1);
  return response;
}
