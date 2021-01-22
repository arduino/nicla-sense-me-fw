/**
 * Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
 *
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file    common.h
 * @date    24 Mar 2020
 * @brief   Common header file for the BHI260/BHA260 examples
 *
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdbool.h>
#include <mbed.h>

#include "bosch/bhy2.h"

char *get_coines_error(int16_t rslt);
char *get_api_error(int8_t error_code);
char *get_sensor_error_text(uint8_t sensor_error);
char *get_sensor_name(uint8_t sensor_id);

void setup_interfaces(bool reset_power, enum bhy2_intf intf);
void close_interfaces(void);
int8_t bhy2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
int8_t bhy2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
int8_t bhy2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
int8_t bhy2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
void bhy2_delay_us(uint32_t us, void *private_data);
bool get_interrupt_status(void);

#endif /* _COMMON_H_ */
