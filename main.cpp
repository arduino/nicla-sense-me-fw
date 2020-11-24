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

void apply_update(FILE *file, uint32_t address);

int main()
{
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
        printf("Firmware update found\r\n");

        apply_update(file, POST_APPLICATION_ADDR);

        fclose(file);
        remove(ANNA_UPDATE_FILE_PATH);
    } else {
        printf("No update found to apply\r\n");
    }

    fs.unmount();
    spif.deinit();

    printf("Starting application\r\n");

    mbed_start_application(POST_APPLICATION_ADDR);
}

void apply_update(FILE *file, uint32_t address)
{
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    printf("Firmware size is %ld bytes\r\n", len);
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
}
