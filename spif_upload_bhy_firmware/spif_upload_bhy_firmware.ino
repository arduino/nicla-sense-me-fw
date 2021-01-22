#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"
#include "fw.h"

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH);
mbed::LittleFileSystem fs("fs");
FILE *file;

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("Start");

  int err = fs.mount(&spif);
  if (err) {
    err = fs.reformat(&spif);
    Serial.print("Error mounting file system: ");
    Serial.println(err);
  }

  file = fopen("/fs/BHY_UPDATE.BIN", "wb");
  int ret = fwrite(bhy2_firmware_image, ___Bosch_SHUTTLE_BHI260_Unosense_fw_len, 1, file);
  delay(100);
  fclose(file);

  if (ret == 0) Serial.print("done");
  else Serial.print("error");
}

void loop() {
  delay(100);
}
