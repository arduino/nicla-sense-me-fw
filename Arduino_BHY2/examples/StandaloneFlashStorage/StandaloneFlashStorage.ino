/*
    This sketch shows how to use Nicla in standalone mode and how to save data
    the on-board 2MB SPI Flash using the LitteFS filesystem.

    Please, take care both of RAM and Flash usage at run-time.

    This example shows how to use MbedOS-native Storage APIs
    - https://os.mbed.com/docs/mbed-os/v6.15/apis/data-storage.html

    C/C++ storage APIs are supported too
    - C-based STDIO: https://en.cppreference.com/w/cpp/header/cstdio
    - Stream-based: https://en.cppreference.com/w/cpp/io
*/

#include <BlockDevice.h>
#include <Dir.h>
#include <File.h>
#include <FileSystem.h>
#include <LittleFileSystem.h>

// The name of the root of the filesystem
constexpr auto userRoot { "fs" };

// The SPIF Block Device
mbed::BlockDevice* spif;
// The LittleFS filesystem
mbed::LittleFileSystem fs { userRoot };

#include <Arduino_BHY2.h>

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
SensorQuaternion rotation(SENSOR_ID_RV);

void setup()
{
    Serial.begin(115200);

    for (const auto timeout = millis() + 2500; !Serial && millis() < timeout; delay(250))
        ;

    Serial.print("Loading the SPI Flash Storage and LittleFS filesystem...");

    // Get core-wide instance of SPIF Block Device
    spif = mbed::BlockDevice::get_default_instance();
    spif->init();

    // Mount the filesystem
    int err = fs.mount(spif);
    if (err) {
        err = fs.reformat(spif);
        Serial.print("Error mounting file system: ");
        Serial.println(err);
        while (true)
            ;
    }
    Serial.println(" done.");

    Serial.print("Initializing the sensors... ");
    BHY2.begin();

    accel.begin();
    gyro.begin();
    temp.begin();
    gas.begin();
    rotation.begin();
    Serial.println(" done.");
}

void loop()
{
    static auto printTime = millis();
    static auto storeTime = millis();
    static auto statsTime = millis();

    BHY2.update();

    // Store data from sensors to the SPI Flash Memory
    if (millis() - storeTime >= 1000) {
        storeTime = millis();
        storeData();
    }

    // List files on the Flash Memory and print contents
    if (millis() - printTime >= 5000) {
        printTime = millis();
        listDirsAlt();
    }

    // Retrieve and print Flash Memory stats
    if (millis() - statsTime >= 10000) {
        statsTime = millis();
        printStats();
    }
}

void listDirsAlt()
{
    String baseDirName = "/";
    baseDirName += userRoot;

    Serial.print("Listing file on ");
    Serial.print(baseDirName);
    Serial.println(" Filesystem");

    // Open the root of the filesystem
    mbed::Dir dir(&fs, "/");
    dirent ent;

    // Cycle through all the directory entries
    while ((dir.read(&ent)) > 0) {
        switch (ent.d_type) {
        case DT_DIR: {
            Serial.print("Directory ");
            Serial.println(ent.d_name);
            break;
        }
        case DT_REG: {
            Serial.print("Regular File ");
            Serial.print(ent.d_name);

            // Declare and open the file in read-only mode
            mbed::File file;
            auto ret = file.open(&fs, ent.d_name);
            if (ret) {
                Serial.println("Unable to open file");
                continue;
            }
            Serial.print(" [");
            Serial.print(file.size());
            Serial.println(" bytes]");

            if (file.size() > 0) {
                // Print file with an ad-hoc function. YMMV.
                printFile(file);

                // Empty file after reading all the content. YMMV.
                file.close();
                ret = file.open(&fs, ent.d_name, O_TRUNC);
                if (ret < 0)
                    Serial.println("Unable to truncate file");
            } else {
                // Remove file if empty. YMMV.
                file.close();
                fs.remove(ent.d_name);
            }

            break;
        }
        default: {
            Serial.print("Other ");
            break;
        }
        }
    }
}

void storeData()
{
    // Append data to file on LittleFS filesystem
    constexpr auto filename { "sensors.csv" };

    // Open in file in write mode.
    // Create if doesn't exists, otherwise open in append mode.
    mbed::File file;
    auto err = file.open(&fs, filename, O_WRONLY | O_CREAT | O_APPEND);
    if (err) {
        Serial.print("Error opening file for writing: ");
        Serial.println(err);
        return;
    }

    // Save sensors data as a CSV line
    auto csvLine = sensorsToCSVLine();

    auto ret = file.write(csvLine.c_str(), csvLine.length());
    if (ret != csvLine.length()) {
        Serial.print("Error writing data: ");
        Serial.println(ret);
    }
    file.close();
}

void printStats()
{
    struct statvfs stats { };
    fs.statvfs(userRoot, &stats);

    auto blockSize = stats.f_bsize;

    Serial.print("Total Space [Bytes]:  ");
    Serial.println(stats.f_blocks * blockSize);
    Serial.print("Free Space [Bytes]:   ");
    Serial.println(stats.f_bfree * blockSize);
    Serial.print("Used Space [Bytes]:   ");
    Serial.println((stats.f_blocks - stats.f_bfree) * blockSize);
    Serial.println();
}

String sensorsToCSVLine()
{
    String line;

    // Pre-allocate maxLine bytes for line
    constexpr size_t maxLine { 128 };
    line.reserve(maxLine);

    line = "";
    line += millis();
    line += ",";
    line += accel.x();
    line += ",";
    line += accel.y();
    line += ",";
    line += accel.z();
    line += ",";
    line += gyro.x();
    line += ",";
    line += gyro.y();
    line += ",";
    line += gyro.z();
    line += ",";
    line += temp.value();
    line += ",";
    line += gas.value();
    line += ",";
    line += rotation.x();
    line += ",";
    line += rotation.y();
    line += ",";
    line += rotation.z();
    line += ",";
    line += rotation.w();
    line += "\r\n";

    return line;
}

void printFile(mbed::File& file)
{
    // Read and print file len-bytes at time
    // to preserve RAM
    constexpr size_t len { 256 };

    size_t totalLen { file.size() };

    while (totalLen > 0) {
        char buf[len] {};

        auto read = file.read(buf, len);
        totalLen -= read;
        for (const auto& c : buf)
            Serial.print(c);
    }
    Serial.println();
}
