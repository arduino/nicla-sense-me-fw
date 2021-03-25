/* 12/23/2017 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 This basic sketch is to operate the BQ25120A battery charger
 https://www.ti.com/lit/ds/symlink/bq25120a.pdf?ts=1610608851953&ref_url=https%253A%252F%252Fwww.startpage.com%252F
 
 The sketch uses SDA/SCL pins of I2C0.
 Library may be used freely and without limit only with attribution.
 
  */
#include <mbed.h>
#include <I2C.h>
#include "BQ25120A.h"

BQ25120A::BQ25120A(){
}

I2C i2c0(I2C_SDA0, I2C_SCL0);


uint8_t BQ25120A::getStatus()
{
  uint8_t c = readByte(BQ25120A_ADDRESS, BQ25120A_STATUS); // Read PRODUCT_ID register for BQ25120A
  return c;
}

void BQ25120A::writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  char command[2];
  command[0] = subAddress;
  command[1] = data;
  i2c0.write(address << 1, command, 2);
}

uint8_t BQ25120A::readByte(uint8_t address, uint8_t subAddress)
{
  char response = 0xFF;
  int ret = i2c0.write(address << 1, (const char*)&subAddress, 1);
  ret = i2c0.read(address << 1, &response, 1);
  return response;
}