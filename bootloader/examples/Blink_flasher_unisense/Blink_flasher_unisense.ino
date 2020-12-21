/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"
#include "fw.h"

SPIFBlockDevice spif(P0_13, P0_14, P0_12, P0_5);

mbed::LittleFileSystem fs("fs");

FILE *file;

int timeout = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(921600);
  int err = fs.mount(&spif);

  file = fopen("/fs/ANNA_UPDATE.BIN", "wb");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(P0_19, OUTPUT);
  pinMode(P0_20, INPUT_PULLUP);
  fwrite(TestCRC_bin, TestCRC_bin_len, 1, file);
}

// the loop function runs over and over again forever
void loop() {
  if (digitalRead(P0_20)==LOW) {
    delay(5000);
    fclose(file);
    system_reset();
  }
  /*
  while (Serial.available()) {
    char c = Serial.read();
    fwrite(&c, 1, 1, file);
    timeout = millis();
  }
  */
  digitalWrite(P0_19, LOW);
  delay(1000);  
  digitalWrite(P0_19, HIGH);
  delay(1000);
}
