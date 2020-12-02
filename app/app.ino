#include <Arduino.h>

#define ESLOV_MAX_LENGTH (255)

#define ESLOV_ADDRESS

int _rxIndex = 0;
uint8_t _rxBuffer[ESLOV_MAX_LENGTH] = {0};
bool _packetComplete = false;

// pack received data, or making classes for fifo events?
//void receivedEvent(uint8_t* pdata)
//{
  //struct __attribute__ ((packed)) FIFOEvent {
    //uint8_t eventId;
    //uint8_t plen;
  //} *eventHdr = (FIFOEvent*)pdata;
//}

void receiveEvent(int howMany)
{
  while(Wire.available()) 
  {
    _rxBuffer[_rxIndex++] = Wire.read(); 
    Serial.println(_rxBuffer[_rxIndex-1]);
  }
}

void setup()
{
  Wire.begin(ESLOV_ADDRESS);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  while(!Serial);
}

void loop()
{
  if (_packetComplete) {
    Serial.println("Packet received");
  }
}
