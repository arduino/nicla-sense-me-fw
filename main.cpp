#include "mbed.h"
#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"

#define MOUNT_PATH           "fs"
#define ANNA_UPDATE_FILE_PATH   "/" MOUNT_PATH "/ANNA_UPDATE.BIN"

#define DEBUG_PRINTF(...) printf(__VA_ARGS__)

#define WRONG_OTA_BINARY 	(-1)
#define MOUNT_FAILED 		(-2)
#define NO_OTA_FILE 		(-3)
#define INIT_FAILED 		(-4)

#if !defined(POST_APPLICATION_ADDR)
#error "target.restrict_size must be set for your target in mbed_app.json"
#endif

//Pin order: MOSI, MISO, SCK, CS
SPIFBlockDevice spif(MBED_CONF_APP_SPIF_MOSI, MBED_CONF_APP_SPIF_MISO,
                     MBED_CONF_APP_SPIF_SCK, MBED_CONF_APP_SPIF_CS);

LittleFileSystem fs(MOUNT_PATH);
FlashIAP flash;

DigitalIn boot_rst_n(P0_20,PullUp);
Timer timer_rst_n;

int try_fail_safe(int timeout);
int try_update(void);
int apply_update(FILE *file, uint32_t address);

int try_fail_safe(int timeout) {
    //Try fail safe: check button state
    //if button pressed, start timer
    printf("Button pressed \r\n");
    int elapsed_time_ms = 0;
    timer_rst_n.start();

    while((elapsed_time_ms < timeout) && (boot_rst_n == 0)) {
        elapsed_time_ms = timer_rst_n.read_ms();
    }

    timer_rst_n.stop();
    timer_rst_n.reset();

    if (elapsed_time_ms < timeout) {
        //NO fail safe
        printf("Button for fail safe has been pulled down for less than 3 seconds. No fail safe! \r\n");
        return 0;
    } else {
        //Fail safe
        printf("Fail safe mode ON \r\n");
        return 1;
        //maybe retrieve a know firmware?
    }
}


int apply_update(FILE *file, uint32_t address)
{
    fseek(file, 0, SEEK_END);
    long len = ftell(file);

    if (len == 0) {
        //No valid firmware update found, the main application can start
        return 1;
    }

    printf("Firmware update found\r\n");

    len = len - 1;     //the last byte contains the CRC
    printf("Firmware size is %ld bytes\r\n", len);

    //read by chunks of 256 bytes
    uint8_t iterations = len/256;
    uint8_t spare_bytes = len%256;

    char buffer[256];
    char crc_file;

    fseek(file, len, SEEK_SET);
    //read CRC written at the end of the file
    fread(&crc_file, 1, 1, file);
    printf("CRC written in file is %x \r\n", crc_file);

    fseek(file, 0, SEEK_SET);

    char crc;
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

    if (crc!=crc_file) {
        printf("Wrong CRC! The computed CRC is %x, while it should be %x \r\n", crc, crc_file);
        printf("Press the button for at least 3 seconds to enter the Fail Safe mode \r\n");

        //wait for the button to be pressed
        while(boot_rst_n) {}

        return try_fail_safe(3000);

    } else {
        printf("Correct CRC=%x \r\n", crc);
    }

    fseek(file, 0, SEEK_SET);
  
    flash.init();

    const uint32_t page_size = flash.get_page_size();
    char *page_buffer = new char[page_size];
    uint32_t addr = address;
    uint32_t next_sector = addr + flash.get_sector_size(addr);
    bool sector_erased = false;
    size_t pages_flashed = 0;
    uint32_t percent_done = 0;
    while (true) {

        // Read data for this page
        memset(page_buffer, 0, sizeof(char) * page_size);
        int size_read = fread(page_buffer, 1, page_size, file);
        if (size_read <= 0) {
            break;
        }

        // Erase this page if it hasn't been erased
        if (!sector_erased) {
            flash.erase(addr, flash.get_sector_size(addr));
            sector_erased = true;
        }

        // Program page
        flash.program(page_buffer, addr, page_size);

        addr += page_size;
        if (addr >= next_sector) {
            next_sector = addr + flash.get_sector_size(addr);
            sector_erased = false;
        }

        if (++pages_flashed % 3 == 0) {
            uint32_t percent_done_new = ftell(file) * 100 / len;
            if (percent_done != percent_done_new) {
                percent_done = percent_done_new;
                printf("Flashed %3ld%%\r", percent_done);
            }
        }
    }
    printf("Flashed 100%%\r\n");

    delete[] page_buffer;

    flash.deinit();

    return 1;
}


int try_update()
{
    int ret_val = 0;

    int err = fs.mount(&spif);
    if (err) {
        DEBUG_PRINTF("Formatting file system...\n");
        err=fs.reformat(&spif);
        if (err) {
            DEBUG_PRINTF("Mount failed\n");
            return MOUNT_FAILED;
        }
    }


    FILE *file = fopen(ANNA_UPDATE_FILE_PATH, "rb");
    if (file != NULL) {

        ret_val = apply_update(file, POST_APPLICATION_ADDR);

        fclose(file);
        remove(ANNA_UPDATE_FILE_PATH);

    } else {
        printf("Starting application\r\n");
        mbed_start_application(POST_APPLICATION_ADDR);
    }

    fs.unmount();
    spif.deinit();

    if (ret_val) {
        printf("Starting application\r\n");
        mbed_start_application(POST_APPLICATION_ADDR);
    }
}


int main()
{
    if(!boot_rst_n) {
        if (try_fail_safe(3000)) {
            printf("Starting application\r\n");
            mbed_start_application(POST_APPLICATION_ADDR);
        }
    }

    try_update();
}
