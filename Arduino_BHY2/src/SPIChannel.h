#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "Arduino.h"

#define MAX_READ_WRITE_LEN (256)

int8_t bhy2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
int8_t bhy2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

void bhy2_delay_us(uint32_t us, void *private_data);

#endif
