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

#define LDO_ALWAYS_ON

#define MOUNT_PATH           "fs"
#define ANNA_UPDATE_FILE_PATH   "/" MOUNT_PATH "/ANNA_UPDATE.BIN"
#define FAIL_SAFE_FILE_PATH "/" MOUNT_PATH "/FAIL_SAFE.BIN"

#if defined(DEBUG)
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

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

DigitalOut cd(p25);

uint32_t regret;

int try_fail_safe();
int try_update(void);
int apply_update(FILE *file, uint32_t address);


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


int check_signature(FILE *file, long file_len)
{
    void *signature = NULL;
    char buffer[128];
    int nChunks = ceil(file_len/128);

    for (int j = 0; j < sizeof("NICLA"); j++) {

        for (int i = 0; i < nChunks; i++) {
            int file_ptr = j + i*128;
            fseek(file, 0, SEEK_SET);
            fseek(file, file_ptr, SEEK_SET);
            fread(buffer, 1, 128, file);
            signature = memmem(buffer, 128, "NICLA", sizeof("NICLA"));

            if (signature != NULL) {
                DEBUG_PRINTF("Signature check PASSED \r\n");
                return 1;
            }
        }

    }

    //Signature NOT found: do not run the current fw and try fail safe
    DEBUG_PRINTF("Nicla signature NOT found! \r\n");
    return 0;
}


int try_fail_safe()
{
    //Retrieve a know firmware from FAIL_SAFE.BIN file
    FILE *file_fail_safe = fopen(FAIL_SAFE_FILE_PATH, "rb");
    regret = NRF_POWER->GPREGRET2;
    if (file_fail_safe != NULL) {
        DEBUG_PRINTF("Fail safe file found \r\n");

        int safe_fw_found = apply_update(file_fail_safe, POST_APPLICATION_ADDR);

        fclose(file_fail_safe);

        if (safe_fw_found) {
            //Clear all flags
            NRF_POWER->GPREGRET2 = 0;
            DEBUG_PRINTF("Starting safe application\r\n");
            mbed_start_application(POST_APPLICATION_ADDR);
        } else {
            DEBUG_PRINTF("Deleting Fail Safe sketch. Please load a correct one.\r\n");
            remove(FAIL_SAFE_FILE_PATH);
        }

        return safe_fw_found;

    } else {
        DEBUG_PRINTF("No fail safe firmware found \r\n");
        regret = regret | 0x04;
        NRF_POWER->GPREGRET2 = regret;

        return 0;
    }
}


int apply_update(FILE *file, uint32_t address)
{
    long len = files.getFileLen(file);

    if (len == 0) {
        //No valid firmware update found, the main application can start
        return 1;
    }

    DEBUG_PRINTF("Firmware size is %ld bytes\r\n", len);

    int signature_found = check_signature(file, len);
    regret = NRF_POWER->GPREGRET2;
    if (!signature_found) {
        regret = regret | 0x01;
        NRF_POWER->GPREGRET2 = regret;

        return 0;
    }
    DEBUG_PRINTF("Signature found\r\n");
    //Clear error bit
    regret &= 0xFE;
    NRF_POWER->GPREGRET2 = regret;

    char crc_file = files.getFileCRC(file);

    fseek(file, 0, SEEK_SET);

    char crc = files.computeCRC(file);

    regret = NRF_POWER->GPREGRET2;
    if (crc!=crc_file) {
            DEBUG_PRINTF("Wrong CRC! The computed CRC is %x, while it should be %x \r\n", crc, crc_file);
            regret = regret | 0x02;
            NRF_POWER->GPREGRET2 = regret;
            return 0;
    }
    //Clear error flags
    regret &= 0xFC;
    NRF_POWER->GPREGRET2 = regret;
    DEBUG_PRINTF("Correct CRC=%x \r\n", crc);

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
    DEBUG_PRINTF("Flashed 100%%\r\n");

    delete[] page_buffer;

    flash.deinit();

    return 1;
}

void blink(uint8_t color, int duration = 1000)
{
    leds.reset();
    leds.init();
    leds.powerUp();

    for (int i = 0; i < 3; i++) {
        leds.ledBlink(color, duration);
        ThisThread::sleep_for(1000ms * duration / 1000);
    }
    leds.powerDown();
}


int try_update()
{
    int update = 0;

    FILE *file = fopen(ANNA_UPDATE_FILE_PATH, "rb");
    if (file != NULL) {
        DEBUG_PRINTF("ANNA_UPDATE_FILE found!\r\n");

        update = apply_update(file, POST_APPLICATION_ADDR);

        fclose(file);
        remove(ANNA_UPDATE_FILE_PATH);

        if (update) {
            DEBUG_PRINTF("Starting new application\r\n");
            blink(green, 200);
        } else {
            DEBUG_PRINTF("Unable to load the new application. Loading the previous one...\r\n");
            blink(red, 200);
        }

    } else {
        DEBUG_PRINTF("No ANNA_UPDATE_FILE found. Starting main application\r\n");
    }

    fs.unmount();
    spif.deinit();

    mbed_start_application(POST_APPLICATION_ADDR);
}


void enableShipMode()
{
    //    STATUS reg:
    //    | B7 | B6 |      B5     | B4 | B3 | B2 | B1 | B0 |
    //    | RO | RO | EN_SHIPMODE | RO | RO | RO | RO | RO |

    uint8_t status_reg = pmic.getStatus();
    DEBUG_PRINTF("Initial Status reg: %04x\n", status_reg);
    status_reg |= 0x20;
    DEBUG_PRINTF("Setting ship mode: %04x\n", status_reg);
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_STATUS, status_reg);
    status_reg = pmic.getStatus();
    DEBUG_PRINTF("Read back status reg: %04x\n", status_reg);

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
            DEBUG_PRINTF("Button didn't return to its initial state\r\n");
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
    fwupdate_bhi260();

    try_update();
}

int main()
{

    CoreDebug->DEMCR = 0;
    NRF_CLOCK->TRACECONFIG = 0;

    cd = 1;

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
#if defined(DEBUG) || defined(LDO_ALWAYS_ON)
    //Enable LDO
    uint8_t ldo_reg = 0xE4;
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL, ldo_reg);
    ldo_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL);
#else
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL, 0);
#endif


    //    PUSH-BUTTON CONTROL reg:
    //    | B7 | B6 |   B5  |    B4   |    B3   | B2 | B1 | B0 |
    //    | X  | X  | MRREC | MRRESET | MRRESET | X  | X  | X  |
    //    MRREC = 0 : enter Ship Mode after reset
    //    MRRESET = 00 : 5 s button press for reset
    uint8_t pb_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL);
    pb_reg &= 0xC7;
    pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL, pb_reg);
    pb_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_PUSH_BUTT_CTRL);
    DEBUG_PRINTF("Push button ctrl reg: %04x\n", pb_reg);

    DEBUG_PRINTF("Bootloader starting\r\n");

    if (NRF_UICR->PSELRESET[0] != 0 || NRF_UICR->PSELRESET[1] != 00) {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        DEBUG_PRINTF("NRF_UICR->PSELRESET[0]: %04x\n", NRF_UICR->PSELRESET[0]);
        NRF_UICR->PSELRESET[0] = 0x80000000;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        DEBUG_PRINTF("NRF_UICR->PSELRESET[1]: %04x\n", NRF_UICR->PSELRESET[1]);
        NRF_UICR->PSELRESET[1] = 0x80000000;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
    }

    mountFileSystem();

    FILE *file_fail_safe = fopen(FAIL_SAFE_FILE_PATH, "r");
    regret = NRF_POWER->GPREGRET2;
    if (file_fail_safe == NULL) {
        //No fail safe sketch available. Set error bit
        regret = regret | 0x04;
        NRF_POWER->GPREGRET2 = regret;
    } else {
        //Fail safe sketch available. Clean error bit
        regret &= 0xFB;
        NRF_POWER->GPREGRET2 = regret;
    }

    fclose(file_fail_safe);

    regret = NRF_POWER->GPREGRET2;
    DEBUG_PRINTF("NRF_POWER->GPREGRET2 = %04x\n", regret);

    if (boot_rst_n == 0) {

        int buttonTaps = selectOperation();
        switch (buttonTaps)
        {
        case 1:
            //Reset Anna
            DEBUG_PRINTF("Resetting Anna \r\n");
            break;

        case 2:
            //Enter Ship Mode
            DEBUG_PRINTF("Entering Ship Mode\r\n");
            blink(yellow);
            enableShipMode();
            while(1) {
                blink(red);
            }
            break;

        case 3:
            //Fail Safe
            DEBUG_PRINTF("Entering Fail Safe Mode\r\n");
            mountFileSystem();
            blink(green);
            try_fail_safe();
            break;

        default:
            break;
        }

    }

    blink(blue, 200);
    loadApp();

    // never reaches this
    while (1) {}
}
