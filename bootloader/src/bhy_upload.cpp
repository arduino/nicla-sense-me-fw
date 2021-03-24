#define _GNU_SOURCE
#include "bhy_upload.h"
#include <string.h>

#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"
#include "file_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define DEBUG_PRINTF(...) printf(__VA_ARGS__)

#define WRONG_OTA_BINARY 	(-1)
#define MOUNT_FAILED 		(-2)
#define NO_OTA_FILE 		(-3)
#define INIT_FAILED 		(-4)

#ifdef UPLOAD_FIRMWARE_TO_FLASH
//#include "bhi260/Bosch_SHUTTLE_BHI260-flash.fw.h"

/* No flash option for the BHA260 */
#else
//#include "bhi260/Bosch_SHUTTLE_BHI260.fw.h"

/* #include "bha260/Bosch_SHUTTLE_BHA260.fw.h" */
#endif

#if defined (PC)
#define MAX_READ_WRITE_LEN 44
#else
#define MAX_READ_WRITE_LEN 256
#endif


static void print_api_error(int8_t rslt, struct bhy2_dev *dev);
static int8_t upload_firmware(struct bhy2_dev *dev);


struct parse_ref parse_table;
DigitalOut* hostboot;

FileUtils bhyfile;

static void time_to_s_ns(uint64_t time_ticks, uint32_t *s, uint32_t *ns)
{
    uint64_t timestamp = time_ticks; /* Store the last timestamp */

    timestamp = timestamp * 15625; /* timestamp is now in nanoseconds */
    *s = (uint32_t)(timestamp / UINT64_C(1000000000));
    *ns = (uint32_t)(timestamp - ((*s) * UINT64_C(1000000000)));
}

/* BHY2_SYS_ID_META_EVENT, BHY2_SYS_ID_META_EVENT_WU */
void parse_meta_event(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint8_t meta_event_type = callback_info->data_ptr[0];
    uint8_t byte1 = callback_info->data_ptr[1];
    uint8_t byte2 = callback_info->data_ptr[2];
    uint32_t s, ns;
    char *event_text;

    if (callback_info->sensor_id == BHY2_SYS_ID_META_EVENT)
    {
        event_text = "[META EVENT]";
    }
    else if (callback_info->sensor_id == BHY2_SYS_ID_META_EVENT_WU)
    {
        event_text = "[META EVENT WAKE UP]";
    }
    else
    {
        return;
    }

    time_to_s_ns(*callback_info->time_stamp, &s, &ns);

    struct parse_ref *parse_table = (struct parse_ref*)callback_ref;

    switch (meta_event_type)
    {
        case BHY2_META_EVENT_FLUSH_COMPLETE:
            printf("%s; T: %u.%09u; Flush complete for sensor id %u\r\n", event_text, s, ns, byte1);
            break;
        case BHY2_META_EVENT_SAMPLE_RATE_CHANGED:
            printf("%s; T: %u.%09u; Sample rate changed for sensor id %u\r\n", event_text, s, ns, byte1);
            break;
        case BHY2_META_EVENT_POWER_MODE_CHANGED:
            printf("%s; T: %u.%09u; Power mode changed for sensor id %u\r\n", event_text, s, ns, byte1);
            break;
        case BHY2_META_EVENT_ALGORITHM_EVENTS:
            printf("%s; T: %u.%09u; Algorithm event\r\n", event_text, s, ns);
            break;
        case BHY2_META_EVENT_SENSOR_STATUS:
            printf("%s; T: %u.%09u; Accuracy for sensor id %u changed to %u\r\n", event_text, s, ns, byte1, byte2);
            if (parse_table)
            {
                parse_table->sensor[byte1].accuracy = byte2;
            }
            break;
        case BHY2_META_EVENT_BSX_DO_STEPS_MAIN:
            printf("%s; T: %u.%09u; BSX event (do steps main)\r\n", event_text, s, ns);
            break;
        case BHY2_META_EVENT_BSX_DO_STEPS_CALIB:
            printf("%s; T: %u.%09u; BSX event (do steps calib)\r\n", event_text, s, ns);
            break;
        case BHY2_META_EVENT_BSX_GET_OUTPUT_SIGNAL:
            printf("%s; T: %u.%09u; BSX event (get output signal)\r\n", event_text, s, ns);
            break;
        case BHY2_META_EVENT_SENSOR_ERROR:
            printf("%s; T: %u.%09u; Sensor id %u reported error 0x%02X\r\n", event_text, s, ns, byte1, byte2);
            break;
        case BHY2_META_EVENT_FIFO_OVERFLOW:
            printf("%s; T: %u.%09u; FIFO overflow\r\n", event_text, s, ns);
            break;
        case BHY2_META_EVENT_DYNAMIC_RANGE_CHANGED:
            printf("%s; T: %u.%09u; Dynamic range changed for sensor id %u\r\n", event_text, s, ns, byte1);
            break;
        case BHY2_META_EVENT_FIFO_WATERMARK:
            printf("%s; T: %u.%09u; FIFO watermark reached\r\n", event_text, s, ns);
            break;
        case BHY2_META_EVENT_INITIALIZED:
            printf("%s; T: %u.%09u; Firmware initialized. Firmware version %u\r\n", event_text, s, ns,
                 ((uint16_t )byte2 << 8) | byte1);
            break;
        case BHY2_META_TRANSFER_CAUSE:
            printf("%s; T: %u.%09u; Transfer cause for sensor id %u\r\n", event_text, s, ns, byte1);
            break;
        case BHY2_META_EVENT_SENSOR_FRAMEWORK:
            printf("%s; T: %u.%09u; Sensor framework event for sensor id %u\r\n", event_text, s, ns, byte1);
            break;
        case BHY2_META_EVENT_RESET:
            printf("%s; T: %u.%09u; Reset event\r\n", event_text, s, ns);
            break;
        case BHY2_META_EVENT_SPACER:
            break;
        default:
            printf("%s; T: %u.%09u; Unknown meta event with id: %u\r\n", event_text, s, ns, meta_event_type);
            break;
    }
}

void parse_generic(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint32_t s, ns;
    struct parse_ref *parse_table = (struct parse_ref*)callback_ref;

    time_to_s_ns(*callback_info->time_stamp, &s, &ns);

    printf("SID: %u; T: %u.%09u; ", callback_info->sensor_id, s, ns);
    for (uint8_t i = 0; i < (callback_info->data_size - 1); i++)
    {
        printf("%X ", callback_info->data_ptr[i]);
    }
    printf("\r\n");
}

void parse_debug_message(const struct bhy2_fifo_parse_data_info *callback_info, void *callback_ref)
{
    uint32_t s, ns;
    struct parse_ref *parse_table = (struct parse_ref*)callback_ref;

    time_to_s_ns(*callback_info->time_stamp, &s, &ns);

    printf("[DEBUG MSG]; T: %u.%09u; flag: 0x%x; data: %s\r\n",
         s,
         ns,
         callback_info->data_ptr[0],
         &callback_info->data_ptr[1]);
}

void install_callbacks(struct bhy2_dev* bhy2) {
    bhy2_set_host_intf_ctrl(BHY2_HIF_CTRL_ASYNC_STATUS_CHANNEL, bhy2);
    for (uint8_t i = 1; i < BHY2_SENSOR_ID_MAX; i++)
    {
        parse_table.sensor[i].scaling_factor = 1.0f;
        bhy2_register_fifo_parse_callback(i, parse_generic, &parse_table, bhy2);
    }
    bhy2_register_fifo_parse_callback(BHY2_SYS_ID_META_EVENT, parse_meta_event, &parse_table, bhy2);
    bhy2_register_fifo_parse_callback(BHY2_SYS_ID_META_EVENT_WU, parse_meta_event, &parse_table, bhy2);
    bhy2_register_fifo_parse_callback(BHY2_SYS_ID_DEBUG_MSG, parse_debug_message, &parse_table, bhy2);
}

/*
 * For use-cases where the whole firmware cannot be accessed
 * through single memory mapped location, for instance, insufficient
 * memory to hold the entire firmware on the host's RAM or Flash and
 * the firmware is stored on external memory like and SD card,
 * firmware updated over USB, BLE or another medium, the following
 * examples shows how to use the bhy2_upload_firmware_to_flash_partly
 * and bhy2_upload_firmware_to_ram_partly APIs
 */

long getUpdateFileSize();

int fwupdate_bhi260(void)
{
    uint8_t product_id = 0;
    uint16_t version = 0;
    int8_t rslt;
    struct bhy2_dev bhy2;
    uint8_t fifo_buffer[2048];

    uint8_t hintr_ctrl, hif_ctrl, boot_status;

    hostboot = new DigitalOut(BHI_HOSTBOOT);
    *hostboot = 0;
    ThisThread::sleep_for(5ms);

    DigitalOut* reset = new DigitalOut(RESET_BHI260);
    *reset = 0;
    ThisThread::sleep_for(5ms);
    *reset = 1;

    ThisThread::sleep_for(100ms);

    setup_interfaces(true, BHY2_SPI_INTERFACE); /* Perform a power on reset */

    rslt = bhy2_init(BHY2_SPI_INTERFACE, bhy2_spi_read, bhy2_spi_write, bhy2_delay_us, MAX_READ_WRITE_LEN, NULL, &bhy2);
    print_api_error(rslt, &bhy2);

    rslt = bhy2_get_product_id(&product_id, &bhy2);
    print_api_error(rslt, &bhy2);

    rslt = bhy2_get_product_id(&product_id, &bhy2);
    print_api_error(rslt, &bhy2);

    rslt = bhy2_soft_reset(&bhy2);
    print_api_error(rslt, &bhy2);

    /* Check for a valid product ID */
    if (product_id != BHY2_PRODUCT_ID)
    {
        printf("Product ID read %X. Expected %X\r\n", product_id, BHY2_PRODUCT_ID);
    }
    else
    {
        printf("BHI260/BHA260 found. Product ID read %X\r\n", product_id);
    }

    /* Check the interrupt pin and FIFO configurations. Disable status and debug */
    //hintr_ctrl = BHY2_ICTL_DISABLE_STATUS_FIFO | BHY2_ICTL_DISABLE_DEBUG;
    hintr_ctrl = 0;

    rslt = bhy2_set_host_interrupt_ctrl(hintr_ctrl, &bhy2);
    print_api_error(rslt, &bhy2);

    /* Configure the host interface */
    hif_ctrl = 0;
    rslt = bhy2_set_host_intf_ctrl(hif_ctrl, &bhy2);
    print_api_error(rslt, &bhy2);

    /* Check if the sensor is ready to load firmware */
    rslt = bhy2_get_boot_status(&boot_status, &bhy2);
    print_api_error(rslt, &bhy2);

    if (boot_status & BHY2_BST_HOST_INTERFACE_READY)
    {
        uint8_t sensor_error;
        int8_t temp_rslt;
        printf("Loading firmware. Boot status: %02X\r\n", boot_status);

        /* If loading firmware to flash, erase the relevant section */
#ifdef UPLOAD_FIRMWARE_TO_FLASH
        if (boot_status & BHY2_BST_FLASH_DETECTED)
        {
            uint32_t start_addr = BHY2_FLASH_SECTOR_START_ADDR;
            uint32_t end_addr = start_addr + getUpdateFileSize();
            printf("Flash detected. Erasing flash to upload firmware\r\n");

            rslt = bhy2_erase_flash(start_addr, end_addr, &bhy2);
            print_api_error(rslt, &bhy2);
        }
        else
        {
            printf("Flash not detected\r\n");

            return 0;
        }
#endif

        rslt = upload_firmware(&bhy2);
        temp_rslt = bhy2_get_error_value(&sensor_error, &bhy2);
        if (sensor_error)
        {
            printf("%s\r\n", get_sensor_error_text(sensor_error));
        }
        print_api_error(rslt, &bhy2);
        print_api_error(temp_rslt, &bhy2);

#ifdef UPLOAD_FIRMWARE_TO_FLASH
        printf("Booting from Flash.\r\n");
        rslt = bhy2_boot_from_flash(&bhy2);
#else
        printf("Booting from RAM.\r\n");
        rslt = bhy2_boot_from_ram(&bhy2);
#endif

        temp_rslt = bhy2_get_error_value(&sensor_error, &bhy2);
        if (sensor_error)
        {
            printf("%s\r\n", get_sensor_error_text(sensor_error));
        }
        print_api_error(rslt, &bhy2);
        print_api_error(temp_rslt, &bhy2);

        rslt = bhy2_get_kernel_version(&version, &bhy2);
        print_api_error(rslt, &bhy2);
        if ((rslt == BHY2_OK) && (version != 0))
        {
            install_callbacks(&bhy2);
            printf("Boot successful. Kernel version %u.\r\n", version);
            bhy2_update_virtual_sensor_list(&bhy2);
            for (uint16_t i = 0; i < 1000; i++) {
                bhy2_get_and_process_fifo(fifo_buffer, sizeof(fifo_buffer), &bhy2);
                bhy2.hif.delay_us(10000, NULL);
            }
            /*
            char* base = (char*)fifo_buffer;
            for (int i = 0; i < 100; i+=8) {
                printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", base[0], base[1], base[2], base[3],
                base[4], base[5], base[6], base[7]);
                base += 8;
            }
            */

        }
    }
    else
    {
        printf("Host interface not ready. Exiting. Boot status: %02X\r\n", boot_status);

        close_interfaces();

        return 0;
    }

    close_interfaces();

    return rslt;
}

static void print_api_error(int8_t rslt, struct bhy2_dev *dev)
{
    if (rslt != BHY2_OK)
    {
        printf("%s\r\n", get_api_error(rslt));
        //exit(0);
    }
}


long getUpdateFileSize() {
    FILE *file_bhy = fopen(BHY_UPDATE_FILE_PATH, "rb");

    if (file_bhy == NULL) {
        printf("No BHY UPDATE file found!");
        return 0;
    }

    long len_bhy = bhyfile.getFileLen(file_bhy);
    return len_bhy;
}

static int8_t upload_firmware(struct bhy2_dev *dev)
{
    int8_t rslt = BHY2_OK;

    FILE *file_bhy = fopen(BHY_UPDATE_FILE_PATH, "rb");

    long len_bhy = getUpdateFileSize();
    printf("BHY Firmware size is %ld bytes\r\n", len_bhy);

    if (len_bhy == 0) {
        return BHY2_E_NULL_PTR;
    }

    uint16_t iterations = len_bhy/256;
    printf("Iterations: %d \r\n", iterations);


    char crc_bhy_file = bhyfile.getFileCRC(file_bhy);
    printf("CRC written in BHY file is %x \r\n", crc_bhy_file);

    fseek(file_bhy, 0, SEEK_SET);

    char crc_bhy = bhyfile.computeCRC(file_bhy);

    if (crc_bhy!=crc_bhy_file) {
        printf("Wrong CRC! The computed CRC is %x, while it should be %x \r\n", crc_bhy, crc_bhy_file);
        //should return an error
        rslt = BHY2_E_NULL_PTR;
        return rslt;
    } else {
        printf("CRC check passed!\r\n");
    }

    fseek(file_bhy, 0, SEEK_SET);

    uint8_t bhy2_firmware_image[256];
    uint32_t incr = 256; /* Max command packet size */

    if ((incr % 4) != 0) /* Round off to higher 4 bytes */
    {
        incr = ((incr >> 2) + 1) << 2;
    }

    for (uint32_t i = 0; (i < len_bhy) && (rslt == BHY2_OK); i += incr)
    {
        //memset(bhy2_firmware_image, 0, 256);
        int size_read = fread(bhy2_firmware_image, 1, 256, file_bhy);
        if (incr > (len_bhy - i)) /* If last payload */
        {
            incr = len_bhy - i;
            if ((incr % 4) != 0) /* Round off to higher 4 bytes */
            {
                incr = ((incr >> 2) + 1) << 2;
            }
        }
#ifdef UPLOAD_FIRMWARE_TO_FLASH
        rslt = bhy2_upload_firmware_to_flash_partly(&bhy2_firmware_image[i], i, incr, dev);
#else
        rslt = bhy2_upload_firmware_to_ram_partly(bhy2_firmware_image, len_bhy, i, incr, dev);
#endif

        printf("%d%% complete\r", (i + incr) * 100 / len_bhy);
    }
    printf("%d%% complete\r\n", 100);



    fclose(file_bhy);

    return rslt;
}
