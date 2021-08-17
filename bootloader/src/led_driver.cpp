/* 12/23/2017 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 This basic sketch is to operate the IS31FL3194 3-channel led driver
 http://www.issi.com/WW/pdf/IS31FL3194.pdf
 
 The sketch uses default SDA/SCL pins on the Ladybug development board 
 but should work with almost any Arduino-based board.
 Library may be used freely and without limit only with attribution.
 
  */
#include <mbed.h>
#include <I2C.h>
#include "IS31FL3194.h"

IS31FL3194::IS31FL3194(){
}

I2C i2c(I2C_SDA0, I2C_SCL0);

// Read the Chip ID register, this is a good test of communication
uint8_t IS31FL3194::getChipID() 
{
  uint8_t c = readByte(IS31FL3194_ADDRESS, IS31FL3194_PRODUCT_ID); // Read PRODUCT_ID register for IS31FL3194
  return c;
}


void IS31FL3194::reset()
{
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_RESET, 0xC5);
}


void IS31FL3194::powerDown()
{
  uint8_t d = readByte(IS31FL3194_ADDRESS, IS31FL3194_OP_CONFIG);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OP_CONFIG, d & ~(0x01)); //clear bit 0 to shut down
}


void IS31FL3194::powerUp()
{
  uint8_t d = readByte(IS31FL3194_ADDRESS, IS31FL3194_OP_CONFIG);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OP_CONFIG, d | 0x01); //set bit 0 to enable
}


void IS31FL3194::init()// configure rgb led function
{
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OP_CONFIG, 0x01);     // normal operation in current mode
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT_CONFIG, 0x07);    // enable all three ouputs
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_CURRENT_BAND, 0x00);  // 10 mA max current
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_HOLD_FUNCTION, 0x00); // hold function disable
}


void IS31FL3194::ledBlink(uint8_t color, uint32_t duration)
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
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT1, _out1 >> 4); //maximum current
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT2, _out2 >> 4);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT3, _out3 >> 4);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_COLOR_UPDATE, 0xC5); // write to color update register for changes to take effect
  ThisThread::sleep_for(duration);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT1, 0x00); //maximum current
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT2, 0x00);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_OUT3, 0x00);
  writeByte(IS31FL3194_ADDRESS, IS31FL3194_COLOR_UPDATE, 0xC5); // write to color update register for changes to take effect
}

void IS31FL3194::writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  char command[2];
  command[0] = subAddress;
  command[1] = data;
  i2c.write(address << 1, command, 2);
}

uint8_t IS31FL3194::readByte(uint8_t address, uint8_t subAddress)
{
  char response = 0xFF;
  int ret = i2c.write(address << 1, (const char*)&subAddress, 1);
  ret = i2c.read(address << 1, &response, 1);
  return response;
}