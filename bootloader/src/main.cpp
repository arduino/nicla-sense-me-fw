#define _GNU_SOURCE
#include <string.h>

#include "mbed.h"
#include "bhy_upload.h"
#include "SPIFBlockDevice.h"
#include "IS31FL3194.h"
#include "BQ25120A.h"
#include "LittleFileSystem.h"
#include "file_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MOUNT_PATH           "fs"
#define ANNA_UPDATE_FILE_PATH   "/" MOUNT_PATH "/ANNA_UPDATE.BIN"
#define FAIL_SAFE_FILE_PATH "/" MOUNT_PATH "/FAIL_SAFE.BIN"

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
                     MBED_CONF_APP_SPIF_SCK, MBED_CONF_APP_SPIF_CS, 16000000);

LittleFileSystem fs(MOUNT_PATH);
FlashIAP flash;

IS31FL3194 leds;

BQ25120A pmic;

FileUtils files;

DigitalIn boot_rst_n(BUTTON1);
Timer timer_rst_n;

int try_fail_safe();
int try_update(void);
int apply_update(FILE *file, uint32_t address, bool fail_safe);


int mountFileSystem()
{
    //Mount the file system
    int err = fs.mount(&spif);
    if (err) {
        DEBUG_PRINTF("Formatting file system...\n");
        err=fs.reformat(&spif);
        if (err) {
            DEBUG_PRINTF("Mount failed\n");
            return MOUNT_FAILED;
        }
    }
    return 0;
}


int check_signature(bool fail_safe)
{
    void *signature = NULL;
    signature = memmem((const char*)POST_APPLICATION_ADDR, POST_APPLICATION_SIZE, "NICLA", sizeof("NICLA"));

    if (signature != NULL) {
        //Signature found: fw can be executed
        printf("Signature check PASSED \r\n");
        printf("Starting application\r\n");
        mbed_start_application(POST_APPLICATION_ADDR);
    } else {
        //Signature NOT found: do not run the current fw and try fail safe
        printf("Nicla signature NOT found! \r\n");
        if (fail_safe) {
            return 0;
        } else {
            printf("Attempting Fail Safe... \r\n");
            if(try_fail_safe()) {
                printf("Starting safe application\r\n");
                mbed_start_application(POST_APPLICATION_ADDR);
            } else {
                return 0;
            }
        }
    }
}


int try_fail_safe()
{
    //Retrieve a know firmware from FAIL_SAFE.BIN file
    FILE *file_fail_safe = fopen(FAIL_SAFE_FILE_PATH, "rb");
    if (file_fail_safe != NULL) {
        printf("Fail safe file found \r\n");

        int safe_fw_found = apply_update(file_fail_safe, POST_APPLICATION_ADDR, true);

        fclose(file_fail_safe);

        return safe_fw_found;

    } else {
        printf("No fail safe firmware found \r\n");
        return 0;
    }
}


int apply_update(FILE *file, uint32_t address, bool fail_safe)
{
    long len = files.getFileLen(file);

    if (len == 0) {
        //No valid firmware update found, the main application can start
        return 1;
    }

    printf("Firmware size is %ld bytes\r\n", len);

    char crc_file = files.getFileCRC(file);

    fseek(file, 0, SEEK_SET);

    char crc = files.computeCRC(file);

    if (crc!=crc_file) {
        if (!fail_safe) {
            printf("Wrong CRC! The computed CRC is %x, while it should be %x \r\n", crc, crc_file);
            printf("Press the button for at least 3 seconds to enter the Fail Safe mode \r\n");

            //wait for the button to be pressed
            //while(*boot_rst_n) {}

            return try_fail_safe();
        } else {
            printf("ERROR! Wrong CRC in fail safe sketch \r\n");
            printf("The computed CRC is %x, while it should be %x \r\n", crc, crc_file);
            return 0;
        }

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
    int update = 0;

    FILE *file = fopen(ANNA_UPDATE_FILE_PATH, "rb");
    if (file != NULL) {

        update = apply_update(file, POST_APPLICATION_ADDR, false);

        fclose(file);
        remove(ANNA_UPDATE_FILE_PATH);

    } else {
        printf("No ANNA_UPDATE_FILE found. Starting main application\r\n");
        mbed_start_application(POST_APPLICATION_ADDR);
    }

    if (update) {
        check_signature(false);
    }
}

void blink(uint8_t color)
{
    leds.reset();
    leds.init();
    leds.powerUp();

    for (int i = 0; i < 3; i++) {
        leds.ledBlink(color, 1000);
        ThisThread::sleep_for(1s);
    }
}


void enableShipMode()
{
    //    STATUS reg:
    //    | B7 | B6 |      B5     | B4 | B3 | B2 | B1 | B0 |
    //    | RO | RO | EN_SHIPMODE | RO | RO | RO | RO | RO |

    uint8_t status_reg = pmic.getStatus();
    printf("Initial Status reg: %04x\n", status_reg);
    status_reg |= 0x20;
    printf("Setting ship mode: %04x\n", status_reg);
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_STATUS, status_reg);
    status_reg = pmic.getStatus();
    printf("Read back status reg: %04x\n", status_reg);

}

void debounce(int state)
{
    bool stable = false;

    int last_button_state = state;
    int last_change = timer_rst_n.read_ms();
    int debouncing_time = 1;
    while(!stable) {
        while((timer_rst_n.read_ms() - last_change) < debouncing_time ) {
            if (boot_rst_n != last_button_state) {
                last_button_state = boot_rst_n;
                last_change = timer_rst_n.read_ms();
            }
        }
        if (boot_rst_n == state) {
            stable = true;
        } else {
            printf("Button didn't return to its initial state\r\n");
        }
    }
}

int selectOperation()
{
    timer_rst_n.start();
    debounce(0);
    while(boot_rst_n == 0) {}
    debounce(1);
    int max_pressure_interval = 500;
    int last_time = timer_rst_n.read_ms();
    while(boot_rst_n == 1) {
        if ((timer_rst_n.read_ms() - last_time) > max_pressure_interval) {
            //Max time elapsed: button pressed just once
            return 1;
        }
    }
    debounce(0);
    while(boot_rst_n == 0) {}
    debounce(1);
    last_time = timer_rst_n.read_ms();
    while(boot_rst_n == 1) {
        if ((timer_rst_n.read_ms() - last_time) > max_pressure_interval) {
            //Max time elapsed: button pressed 2 times
            return 2;
        }
    }
    debounce(0);
    while(boot_rst_n == 0) {}
    debounce(1);
    last_time = timer_rst_n.read_ms();
    while(boot_rst_n == 1) {
        if ((timer_rst_n.read_ms() - last_time) > max_pressure_interval) {
            //Max time elapsed: button pressed 3 times
            return 3;
        }
    }

    return 0;
}

void loadApp()
{
    mountFileSystem();

    fwupdate_bhi260();

    try_update();

    fs.unmount();
    spif.deinit();
}

int main()
{

    CoreDebug->DEMCR = 0;
    NRF_CLOCK->TRACECONFIG = 0;

    /*
        LDO reg:
        |   B7   |  B6   |  B5   |  B4   |  B3   |  B2   | B1  | B0  |
        | EN_LDO | LDO_4 | LDO_3 | LDO_2 | LDO_1 | LDO_0 |  X  |  X  |

        Conversion function:
        LDO = 0.8V + LDO_CODE * 100mV

        - for LDO = 3.3V:
            - set LCO_CODE = 25 (0x19)
            - shift to lef by 2 positions: (0x19 << 2) = 0x64
            - set EN_LDO: 0xE4
        - for LDO = 1.8V:
            - set LCO_CODE = 10 (0x0A)
            - shift to lef by 2 positions: (0x0A << 2) = 0x28
            - set EN_LDO: 0xA8
    */
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL, 0);
    uint8_t ldo_reg = 0xE4;
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL, ldo_reg);
    ldo_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL);
    //printf("Ldo reg: %04x\n", ldo_reg);

    printf("Bootloader starting\r\n");

    //    PUSH-BUTTON CONTROL reg:
    //    | B7 | B6 |   B5  | B4 | B3 | B2 | B1 | B0 |
    //    | X  | X  | MRREC | X  | X  | X  | X  | X  |
    //    MRREC = 1 : enter Ship Mode after reset
    /*
    uint8_t pb_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL);
    printf("Initial push button reg: %04x\n", pb_reg);
    pb_reg |= 0x20;
    printf("Setting ship mode after reset: %04x\n", pb_reg);
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL, pb_reg);
    pb_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL);
    printf("Read back push button reg: %04x\n", pb_reg);
    */

    /*
    uint8_t pb_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL);
    printf("Initial push button reg: %04x\n", pb_reg);
    pb_reg |= 0x04;
    printf("Setting PG as voltage shifted push-button (input MR): %04x\n", pb_reg);
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL, pb_reg);
    pb_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL);
    printf("Read back push button reg: %04x\n", pb_reg);
    */

#if defined (CONFIG_GPIO_AS_PINRESET)
    printf("CONFIG_GPIO_AS_PINRESET defined\r\n");
#else
    printf("CONFIG_GPIO_AS_PINRESET NOT defined\r\n");
#endif

    if (boot_rst_n == 0) {

        int buttonTaps = selectOperation();
        switch (buttonTaps)
        {
        case 1:
            //Reset Anna
            printf("Resetting Anna \r\n");
            blink(blue);
            loadApp();
            break;

        case 2:
            //Enter Ship Mode
            printf("Entering Ship Mode\r\n");
            enableShipMode();
            blink(red);
            break;

        case 3:
            //Fail Safe
            printf("Entering Fail Safe Mode\r\n");
            mountFileSystem();
            blink(green);
            try_fail_safe();
            printf("Starting safe application\r\n");
            mbed_start_application(POST_APPLICATION_ADDR);
            break;
        
        default:
            break;
        }

    } else {

        blink(blue);

        loadApp();

    }
}
