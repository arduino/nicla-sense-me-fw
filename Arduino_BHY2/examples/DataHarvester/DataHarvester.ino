/*
    This sketch shows how the use the Nicla Sense ME to collect
    acceleration data at maximum speed, and how to send them to
    a companion app via UART.

    It uses a COBS-based, packed-oriented serial protocol to establish
    a reliable communication to the companion app, to receive operation
    commands and to send collected data.

    Requirements:
    - Nicla Sense ME
    - PacketSerial library https://github.com/bakercp/PacketSerial
    - Commander.py companion app
*/

#include <Nicla_System.h>
#include <Arduino_BHY2.h>

// Manage the communication with the companion app
#include <PacketSerial.h>
PacketSerial myPacketSerial;

auto capture { false };

// Send data as packed struct, ie. as the raw bytes
struct __attribute__((__packed__)) Data {
    float ts;
    float x;
    float y;
    float z;
};

SensorXYZ accel(SENSOR_ID_ACC_PASS);

void setup()
{
    nicla::begin();
    nicla::leds.begin();
    BHY2.begin();
    accel.begin();

    // Init the PacketSerial communication
    myPacketSerial.begin(115200);
    // Set the function for handling commands from the companion app
    myPacketSerial.setPacketHandler(&onPacketReceived);

    pinMode(LED_BUILTIN, OUTPUT);
    for (auto i = 0u; i < 10; i++) {
        nicla::leds.setColor(green);
        delay(25);
        nicla::leds.setColor(off);
        delay(25);
    }
}

void loop()
{
    // Update communication-channel and sensors
    myPacketSerial.update();
    BHY2.update();

    // Check for a receive buffer overflow (optional).
    if (myPacketSerial.overflow()) {
        for (auto i = 0u; i < 5; i++) {
            nicla::leds.setColor(green);
            delay(25);
            nicla::leds.setColor(off);
            delay(25);
        }
    }

    // Capture and send data as soon as we read it
    if (capture == true) {
        auto now = micros() / 1000.0;

        // Collect data from accel sensor
        Data data { now, accel.x(), accel.y(), accel.z() };
        constexpr size_t dataBufLen { sizeof(Data) };
        uint8_t dataBuf[dataBufLen] {};

        // Convert the Data struct to an array of bytes
        memcpy(dataBuf, reinterpret_cast<void*>(&data), dataBufLen);
        
        // Send data
        myPacketSerial.send(dataBuf, dataBufLen);
    }
}


// Parse commands from the companion app
void onPacketReceived(const uint8_t* buffer, size_t size)
{
    uint8_t tempBuffer[size];

    for (auto i = 0u; i < 2; i++) {
        nicla::leds.setColor(green);
        delay(25);
        nicla::leds.setColor(off);
        delay(25);
    }

    memcpy(tempBuffer, buffer, size);

    switch (tempBuffer[0]) {
    case 'R':
        nicla::leds.setColor(green);
        capture = true;
        break;
    case 'S':
        nicla::leds.setColor(off);
        capture = false;
        break;

    default:
        break;
    }
}
