#include "Arduino.h"
#include <Wire.h>
#include "FlashIAPBlockDevice.h"
#include "FATFileSystem.h"

static FlashIAPBlockDevice bd(0x80000, 0x80000);
static mbed::FATFileSystem fs("fs");

char rxBuffer[6] = {0};
uint8_t rxLength = 0;

char nameUpdated[6] = {0};
bool isNameUpdated = false;

bool noName = true;

FILE* fp;

void flashSetup()
{
  int err = fs.mount(&bd);
  if (err) {
      err = fs.reformat(&bd);
  }
}

bool readName(char* name, uint8_t size)
{
  bool success = false;
  fp = fopen("/fs/name.txt", "rb");
  if (fp) {
    success = fread(name, 1, size, fp);
    if (success) {
      Serial.print("Read name: ");
      Serial.println(name);
    }
    fclose(fp);
  }
  return success;
}

bool writeName(char* name, uint8_t size)
{
  bool success = false;
  fp = fopen("/fs/name.txt", "wb");
  if (fp) {
    success = fwrite((void*)name, 1, size, fp);
    if (success) {
      Serial.print("Write name: ");
      Serial.println(name);
    }
  }
  fclose(fp);
  return success;
}

void setup()
{
  flashSetup();

  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  while(!Serial);

  delay(200);

  char name[6]; 
  auto ret = readName(name, 6);
  Serial.println(ret);

  if (!ret) {
    char newName[6] = {'p', 'o', 'l', 'd', 'o', '!'};
    writeName(newName, 6);
  }
  readName(name, 6);

  delay(10000);
}

void loop()
{
  delay(100);
  if (isNameUpdated) {
    auto ret = writeName(nameUpdated, 6);
    isNameUpdated = false;
    //Serial.print("writtten: ");
    //Serial.println(ret);

    char name[6]; 
    readName(name, 6);
  }
}

void receiveEvent(int howMany)
{
  while(Wire.available()) 
  {
    rxBuffer[rxLength++] = Wire.read(); 
    //Serial.println(rxBuffer[rxLength-1]);
    if (rxLength == sizeof(rxBuffer)) {
      isNameUpdated = true;
      rxLength = 0;
      memcpy(nameUpdated, rxBuffer, sizeof(rxBuffer));
    }
  }
}
