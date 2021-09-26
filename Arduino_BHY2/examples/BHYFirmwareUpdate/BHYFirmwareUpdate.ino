/*
 * Use this sketch to flash a binary (fw.h) that is used to update the on-flash firmware for BHI260AP on Nicla Sense.
 * Steps:
 * 1. Generate the "fw.h" desired (see steps below) and put it in the same folder as this sketch
 * 2. Upload (program) this sketch to the Nicla Sense Board and immediately open the Serial Monitor from the Arduino IDE
 * 3. Check the messages in the Serial Monitor and wait until "BHY FW Upload Done!" shows up
 * 4. Upload another sketch such as "Examples->Arduino_BHY2->Standalone" to the Nicla Sense Board and you are all set
 * 
 * Steps to generate the desired "fw.h"
 *  To use a different BHI260 FW binary, use the command below to generate the "fw.h"
 *  note: the FW binary should have a name pattern: xxx-flash.fw or xxx_flash.fw
 *  $ mv xxx-flash.fw BHI260AP_NiclaSenseME-flash.fw 
 *  $ echo const > fw.h && xxd -i BHI260AP_NiclaSenseME-flash.fw >> fw.h
 * 
 */

#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"
#include "fw.h" //echo const > fw.h && xxd -i BHI260AP_NiclaSenseME-flash.fw >> fw.h

#define BHY_DFU_FW_PATH "/fs/BHY_UPDATE.BIN"

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH);
mbed::LittleFileSystem fs("fs");
FILE *file;

#define fw_bin BHI260AP_NiclaSenseME_flash_fw
#define fw_bin_len BHI260AP_NiclaSenseME_flash_fw_len

void setup() {
  Serial.begin(115200);
  delay(3000);

  char crc = 0;
  for (int i = 0; i < fw_bin_len; i++) {
    crc = crc ^ fw_bin[i];
  }
  
  Serial.print("The computed CRC is 0x");
  Serial.println(crc, HEX);

  Serial.println("Writing BHY FW binary into SPIFlash...");

  int err = fs.mount(&spif);
  if (err) {
    err = fs.reformat(&spif);
    Serial.print("Error mounting file system: ");
    Serial.println(err);
  }

  file = fopen(BHY_DFU_FW_PATH, "wb");
  int ret = fwrite(fw_bin, fw_bin_len, 1, file);
  fwrite(&crc, 1, 1, file);
  delay(100);
  fclose(file);

  Serial.println("BHY FW Upload Done!");
}

void loop() {
  delay(100);
}
