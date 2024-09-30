/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
/**
* @file ota.h
* @brief file ota.h
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2023-04-03
*/
#ifndef __OTA_DEF_H__
#define __OTA_DEF_H__

#include "flash_ops.h"

#define JUMP_FAILED_FLAG  0XABABABAB
#define JUMP_SUCCESS_FLAG 0XCDCDCDCD

typedef enum {
    OTA_STATUS_IDLE = 0,     // 空闲状态,没有进行OTA升级
    OTA_STATUS_READY,        // 准备状态,可以进行OTA升级
    OTA_STATUS_DOWNLOADING,  // 正在下载固件
    OTA_STATUS_DOWNLOADED,   // 固件下载完成
    OTA_STATUS_UPDATING,     // 正在进行OTA升级
    OTA_STATUS_BACKUP,       // 正在版本回退
    OTA_STATUS_ERROR,        // 出现错误,升级失败
} ota_status_t;


/* Flash分区中保存固件的属性描述 */
typedef struct {
    uint32_t size;              // 应用程序大小,记录分区固件的大小
    uint32_t crc32;             // 应用程序CRC32校验值,记录分区固件的crc32值
    uint32_t version;           // 应用程序版本号,记录分区固件的版本号
    uint32_t reserve;           // 保留字段
    uint8_t  description[128];  // 固件的描述信息,最多128个字符
} firmware_t;


/* OTA升级过程中的信息结构体 */
typedef struct {
    firmware_t os;                // XiUOS System分区属性信息
    firmware_t bak;               // Bakup分区属性信息
    firmware_t down;              // Download分区属性信息
    uint32_t status;              // 升级状态,取值来自于ota_status_t类型
    uint32_t lastjumpflag;        // bootloaer跳转失败的标志,bootloader里置0xABABABAB,跳转成功后在应用里置0xCDCDCDCD
    uint32_t reserve[2];          // 保留字段
    uint8_t  error_message[128];  // 错误信息,最多128个字符
} ota_info_t;

void app_clear_jumpflag(void);
void ota_entry(void);
#endif
