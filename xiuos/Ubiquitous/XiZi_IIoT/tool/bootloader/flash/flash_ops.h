/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
/**
* @file flash_ops.h
* @brief support flash function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2023-04-03
*/
#ifndef __FLASH_OPS_H__
#define __FLASH_OPS_H__


#include <flash.h>

typedef struct
{
    /* board init function*/
    void (*board_init)(void);
    void (*serial_init)(void);
    void (*print_string)(uint8_t *s);

    /* flash Driver operation */
    void (*flash_init)(void);
    void (*flash_deinit)(void);

    /* flash operation */
    status_t (*op_flash_erase)(uint32_t start_addr, uint32_t byte_cnt);
    status_t (*op_flash_write)(uint32_t start_addr, uint8_t *buf, uint32_t byte_cnt);
    status_t (*op_flash_read)(uint32_t addr, uint8_t *buf, uint32_t len);
    status_t (*op_flash_copy)(uint32_t srcAddr,uint32_t dstAddr, uint32_t imageSize);

    /* Burn the initialization version */
    int32_t (*download_by_serial)(const uint32_t addr);

    /* system operation */
    void(*op_reset)(void);
    void(*op_jump)(void);
    void(*op_delay)(uint32_t ms);
}mcuboot_t;

#endif