#include "channel.h"

int8_t bhy2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
  return 0;
}

int8_t bhy2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
  return 0;
}

void bhy2_delay_us(uint32_t us, void *private_data)
{
  delay(1);
}
