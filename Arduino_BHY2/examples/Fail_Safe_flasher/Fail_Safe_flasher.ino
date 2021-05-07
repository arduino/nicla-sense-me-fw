/*
 * Use this sketch to flash a binary (fw.h) that can be used as Fail Safe for Nicla Sense.
 * Here a simple colorful blink for the RGB led is used.
 * 
 * To use a different binary, generate a new fw.h: 
 * xxd -i your_sketch.ino.bin >> fw.h
 * 
 */


#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"
#include "fw.h"

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH);
mbed::LittleFileSystem fs("fs");
FILE *file;

void setup() {
  Serial.begin(115200);
  delay(3000);

  char crc = 0;
  for (int i = 0; i < Fail_Safe_bin_len; i++) {
    crc = crc ^ Fail_Safe_bin[i];
  }
  Serial.print("The computed CRC is 0x");
  Serial.println(crc, HEX);

  Serial.println("Writing Fail Safe binary in SPIFlash...");

  int err = fs.mount(&spif);
  if (err) {
    err = fs.reformat(&spif);
    Serial.print("Error mounting file system: ");
    Serial.println(err);
  }

  file = fopen("/fs/FAIL_SAFE.BIN", "wb");
  int ret = fwrite(Fail_Safe_bin, Fail_Safe_bin_len, 1, file);
  fwrite(&crc, 1, 1, file);
  delay(100);
  fclose(file);

  Serial.print("Done!");
}

void loop() {
  delay(100);
}
