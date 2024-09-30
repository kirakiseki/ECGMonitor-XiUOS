/**
 *******************************************************************************
 * @file  sdioc/sdioc_sd/source/main.c
 * @brief Main program of SDIOC SD card for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/**
* @file connect_sdio.c
* @brief support sdio function using bus driver framework on hc32f4a0 board
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-10-18
*/

/*************************************************
File name: connect_sdio.c
Description: support edu-arm32-board sd card configure and sdio bus register function
Others: 
History: 
1. Date: 2022-01-24
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board sdio configure, write and read
2. support edu-arm32-board sdio bus device and driver register
*************************************************/

#include <connect_sdio.h>

/* SD transfer mode */
#define SD_TRANS_MD_POLLING             (0U)
#define SD_TRANS_MD_INT                 (1U)
#define SD_TRANS_MD_DMA                 (2U)
/* Populate the following macro with an value, reference "SD transfer mode" */
#define SD_TRANS_MD                     (SD_TRANS_MD_POLLING)

/* SDIOC configuration define */
#define SDIOC_SD_UINT                   (CM_SDIOC1)
#define SDIOC_SD_CLK                    (FCG1_PERIPH_SDIOC1)

/* CD = PB13 */
#define SDIOC_CD_PORT                   (GPIO_PORT_B)
#define SDIOC_CD_PIN                    (GPIO_PIN_13)
/* CK = PC12 */
#define SDIOC_CK_PORT                   (GPIO_PORT_C)
#define SDIOC_CK_PIN                    (GPIO_PIN_12)
/* CMD = PD02 */
#define SDIOC_CMD_PORT                  (GPIO_PORT_D)
#define SDIOC_CMD_PIN                   (GPIO_PIN_02)
/* D0 = PB07 */
#define SDIOC_D0_PORT                   (GPIO_PORT_B)
#define SDIOC_D0_PIN                    (GPIO_PIN_07)
/* D1 = PA08 */
#define SDIOC_D1_PORT                   (GPIO_PORT_A)
#define SDIOC_D1_PIN                    (GPIO_PIN_08)
/* D2 = PC10 */
#define SDIOC_D2_PORT                   (GPIO_PORT_C)
#define SDIOC_D2_PIN                    (GPIO_PIN_10)
/* D3 = PB05 */
#define SDIOC_D3_PORT                   (GPIO_PORT_B)
#define SDIOC_D3_PIN                    (GPIO_PIN_05)

static stc_sd_handle_t gSdHandle;
static int sd_lock = -1;

static void SdCardConfig(void)
{
    /* Enable SDIOC clock */
    FCG_Fcg1PeriphClockCmd(SDIOC_SD_CLK, ENABLE);

    /* SDIOC pins configuration */
    stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(SDIOC_CD_PORT, SDIOC_CD_PIN, &stcGpioInit);

    GPIO_SetFunc(SDIOC_CK_PORT,  SDIOC_CK_PIN,  GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_CMD_PORT, SDIOC_CMD_PIN, GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_D0_PORT,  SDIOC_D0_PIN,  GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_D1_PORT,  SDIOC_D1_PIN,  GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_D2_PORT,  SDIOC_D2_PIN,  GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_D3_PORT,  SDIOC_D3_PIN,  GPIO_FUNC_9);

    /* Configure structure initialization */
    gSdHandle.SDIOCx                     = SDIOC_SD_UINT;
    gSdHandle.stcSdiocInit.u32Mode       = SDIOC_MD_SD;
    gSdHandle.stcSdiocInit.u8CardDetect  = SDIOC_CARD_DETECT_CD_PIN_LVL;
    gSdHandle.stcSdiocInit.u8SpeedMode   = SDIOC_SPEED_MD_HIGH;
    gSdHandle.stcSdiocInit.u8BusWidth    = SDIOC_BUS_WIDTH_4BIT;
    gSdHandle.stcSdiocInit.u16ClockDiv   = SDIOC_CLK_DIV2;
    gSdHandle.DMAx                       = NULL;
}

static uint32 SdioConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    if (configure_info->configure_cmd == OPER_BLK_GETGEOME) {
        NULL_PARAM_CHECK(configure_info->private_data);
        struct DeviceBlockArrange *args = (struct DeviceBlockArrange *)configure_info->private_data;

        args->size_perbank = gSdHandle.stcSdCardInfo.u32BlockSize;
        args->block_size = gSdHandle.stcSdCardInfo.u32BlockSize;
        args->bank_num = gSdHandle.stcSdCardInfo.u32BlockNum;
    }

    return EOK;
}

static uint32 SdioOpen(void *dev)
{
    NULL_PARAM_CHECK(dev);

    if(sd_lock >= 0) {
        KSemaphoreDelete(sd_lock);
    }
    sd_lock = KSemaphoreCreate(1);
    if (sd_lock < 0){
        return ERROR;
    }

    return EOK;
}

static uint32 SdioClose(void *dev)
{
    NULL_PARAM_CHECK(dev);

    KSemaphoreDelete(sd_lock);

    return EOK;
}

static uint32 SdioRead(void *dev, struct BusBlockReadParam *read_param)
{
    uint8 ret = EOK;
    uint32 sector = read_param->pos;
    uint32 block_num = read_param->size;
    uint8 *read_buffer = (uint8 *)read_param->buffer;

    KSemaphoreObtain(sd_lock, WAITING_FOREVER);

    if (LL_OK != SD_ReadBlocks(&gSdHandle, sector, block_num, read_buffer, 2000UL)) {
        KPrintf("Read data blocks failed.\r\n");
        return 0;
    }

    KSemaphoreAbandon(sd_lock);

    return read_param->size;
}

static uint32 SdioWrite(void *dev, struct BusBlockWriteParam *write_param)
{
    uint8 ret = EOK;
    uint32 sector = write_param->pos;
    uint32 block_num = write_param->size;
    uint8 *write_buffer = (uint8 *)write_param->buffer;

    KSemaphoreObtain(sd_lock, WAITING_FOREVER);

    if (LL_OK != SD_WriteBlocks(&gSdHandle, sector, block_num, write_buffer, 2000U)) {
        KPrintf("Write data blocks failed.\r\n");
        return 0;
    }

    KSemaphoreAbandon(sd_lock);

    return write_param->size;
}

static int SdioControl(struct HardwareDev *dev, struct HalDevBlockParam *block_param)
{
    NULL_PARAM_CHECK(dev);

    if (OPER_BLK_GETGEOME == block_param->cmd) {
        block_param->dev_block.size_perbank = gSdHandle.stcSdCardInfo.u32BlockSize;
        block_param->dev_block.block_size = gSdHandle.stcSdCardInfo.u32BlockSize;
        block_param->dev_block.bank_num = gSdHandle.stcSdCardInfo.u32BlockNum;
    }

    return EOK;
}

static struct SdioDevDone dev_done = 
{
    SdioOpen,
    SdioClose,
    SdioWrite,
    SdioRead,
};

#if defined(FS_VFS) && defined(MOUNT_SDCARD_FS)
#include <iot-vfs.h>

/**
 * @description: Mount SD card
 * @return 0
 */
static int MountSDCardFs(enum FilesystemType fs_type)
{
    if (MountFilesystem(SDIO_BUS_NAME, SDIO_DEVICE_NAME, SDIO_DRIVER_NAME, fs_type, "/") == 0)
        KPrintf("Sd card mount to '/'");
    else
        KPrintf("Sd card mount to '/' failed!");
    
    return 0;
}
#endif

static void SdCardAttach(void)
{
    static stc_sd_handle_t *sd_handle = &gSdHandle;
    
    KPrintf("\r\nCard inserted.\r\n");

    /* Reset and init SDIOC */
    if (LL_OK != SDIOC_SWReset(sd_handle->SDIOCx, SDIOC_SW_RST_ALL)) {
        KPrintf("Reset SDIOC failed!\r\n");
    } else if (LL_OK != SD_Init(sd_handle)) {
        KPrintf("SD card initialize failed!\r\n");
    } else {
        KPrintf("SD card init done!\r\n");
    }

#ifdef MOUNT_SDCARD_FS
    /*mount file system*/
    MountSDCardFs(MOUNT_SDCARD_FS_TYPE);
#endif
}

static void SdCardDetach(void)
{
    /*unmount file system*/
    KPrintf("\r\nCard detect extracted.\r\n");

#ifdef MOUNT_SDCARD_FS
    UnmountFileSystem("/");
#endif
}

static uint8 SdCardReadCd(void)
{
    en_pin_state_t sd_cd_state = GPIO_ReadInputPins(SDIOC_CD_PORT, SDIOC_CD_PIN);
    
    if (PIN_RESET == sd_cd_state) {
        return 0;
    } else {
        return 1;
    }
}

static void SdCardTask(void* parameter)
{
    static int sd_card_status = 0;

    while (1) {
        if (!SdCardReadCd()) {
            if (!sd_card_status) {
                SdCardAttach();
                sd_card_status = 1;
            }
        } else {
            if (sd_card_status) {
                SdCardDetach();
                sd_card_status = 0;
            }
        }
    }
}

#ifdef MOUNT_SDCARD
int MountSDCard()
{
    int sd_card_task = 0;
    sd_card_task = KTaskCreate("sd_card", SdCardTask, NONE,
                           SD_CARD_STACK_SIZE, 8);
    if(sd_card_task < 0) {		
		KPrintf("sd_card_task create failed ...%s %d.\n", __FUNCTION__,__LINE__);
		return ERROR;
	}

    StartupKTask(sd_card_task);

    return EOK;
}
#endif

int HwSdioInit(void)
{
    x_err_t ret = EOK;
    
    static struct SdioBus sdio_bus;
    static struct SdioDriver sdio_drv;
    static struct SdioHardwareDevice sdio_dev;

    memset(&sdio_bus, 0, sizeof(struct SdioBus));
    memset(&sdio_drv, 0, sizeof(struct SdioDriver));
    memset(&sdio_dev, 0, sizeof(struct SdioHardwareDevice));

    SdCardConfig();
    
    ret = SdioBusInit(&sdio_bus, SDIO_BUS_NAME);
    if (ret != EOK) {
        KPrintf("Sdio bus init error %d\n", ret);
        return ERROR;
    }

    ret = SdioDriverInit(&sdio_drv, SDIO_DRIVER_NAME);
    if (ret != EOK) {
        KPrintf("Sdio driver init error %d\n", ret);
        return ERROR;
    }
    ret = SdioDriverAttachToBus(SDIO_DRIVER_NAME, SDIO_BUS_NAME);
    if (ret != EOK) {
        KPrintf("Sdio driver attach error %d\n", ret);
        return ERROR;
    }

    sdio_dev.dev_done = &dev_done;
    sdio_dev.haldev.dev_block_control = SdioControl;
    ret = SdioDeviceRegister(&sdio_dev, SDIO_DEVICE_NAME);
    if (ret != EOK) {
        KPrintf("Sdio device register error %d\n", ret);
        return ERROR;
    }
    ret = SdioDeviceAttachToBus(SDIO_DEVICE_NAME, SDIO_BUS_NAME);
    if (ret != EOK) {
        KPrintf("Sdio device register error %d\n", ret);
        return ERROR;
    }

    return ret;
}
