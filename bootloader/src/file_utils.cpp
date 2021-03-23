#include "mbed.h"
#include "common.h"
#include "file_utils.h"

FileUtils::FileUtils(){}

long FileUtils::getFileLen(FILE *file) {

  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  fseek(file, 0, SEEK_SET);
  //Decrement len by 1 to remove the CRC from the count
  return len - 1;

}

char FileUtils::getFileCRC(FILE *file) {

  char crc_file;

  long len = getFileLen(file);
  fseek(file, len, SEEK_SET);
  fread(&crc_file, 1, 1, file);

  return crc_file;
}

char FileUtils::computeCRC(FILE *file) {
  char buffer[256];
  char crc;

  long len = getFileLen(file);

  //read by chunks of 256 bytes
  uint16_t iterations = floor(len/256);
  uint8_t spare_bytes = len%256;

  int buffer_index = 2;

  //copy the file content by chunks of 256 bytes into a buffer of chars
  for (int i = 0; i < iterations; i++) {
    //read 256 bytes into buffer
    fread(buffer, 1, 256, file);

    if (i==0) {
        //we are at the beginning of the file, so we must compute the first xor between bytes
        crc = buffer[0]^buffer[1];
    }

    for(int i=buffer_index; i<256; i++) {
        crc = crc^buffer[i];
    }

    buffer_index = 0;

  }

  if (spare_bytes) {
    //read the spare bytes, without the CRC
    fread(buffer, 1, spare_bytes, file);

    for(int i=0; i<spare_bytes; i++) {
        crc = crc^buffer[i];
    }
  }

  return crc;
}
