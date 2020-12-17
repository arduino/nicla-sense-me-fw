#include <Wire.h>

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
}

char name[6] = {'p', 'o', 'l', 'd', 'o', '0'};
uint8_t len = 0;
uint8_t count = 0;

void loop()
{
  static auto time = millis();
  if (millis() - time >= 2000) {
    time = millis();

    Wire.beginTransmission(4);
    uint8_t resp = 1;
    Wire.write(resp);
    Wire.endTransmission();

    delay(100);

    Wire.requestFrom(4, 1);
    while (Wire.available()){
      char b = Wire.read();
      Serial.println(b);
    }

  }


#if 0
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(name[len++]);              // sends one byte  
  Wire.endTransmission();    // stop transmitting

  if (len == sizeof(name)) {
    name[len-1] = (count++) + '0';
    len = 0;
    if (count == 10) count = 0;
    delay(10000);
  }
#endif

  //delay(1000);
}

