/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
* @file fsl_flash.c
* @brief support flexspi norflash function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2023-04-03
*/
 
#include <stdio.h>
#include "flash.h"
#include "MIMXRT1052.h"

uint8_t NorFlash_BUFFER[4096];         //4K buffer cache
uint8_t buffer[FLASH_PAGE_SIZE];       //256 bytes buffer cache
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static status_t Flexspi_Nor_Wait_Busy(uint32_t instance, uint32_t baseAddr);
static status_t Flexspi_Nor_Write_Enable(uint32_t instance, uint32_t baseAddr);
static void flexspi_clock_config(uint32_t instance, uint32_t freq, uint32_t sampleClkMode);
static void flexspi_clock_gate_enable(void);
static void flexspi_clock_gate_disable(void);
static status_t flexspi_get_clock(uint32_t instance, flexspi_clock_type_t type, uint32_t *freq);
static status_t flexspi_get_ticks(uint32_t *ticks, uint32_t intervalNs, uint32_t freq, uint32_t unit);
static status_t flexspi_configure_dll(uint32_t instance, flexspi_mem_config_t *config);
static status_t flexspi_config_mcr1(uint32_t instance, flexspi_mem_config_t *config);

static const lookuptable_t FlashLookupTable={
.ReadStatus_Seq=
{
    FSL_ROM_FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x04),
},
/* Write Enable */
.WriteEnable_Seq=
{
    FSL_ROM_FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP, FLEXSPI_1PAD, 0x00),
},
/* Erase Sector */
.EraseSector_Seq=
{
    FSL_ROM_FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x20, RADDR_SDR, FLEXSPI_1PAD, 0x18),
},
/* Page Program */
.PageProgram_Seq=
{
    FSL_ROM_FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x32, RADDR_SDR, FLEXSPI_1PAD, 0x18),
    FSL_ROM_FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_4PAD, 0x00, 0x00, 0x00, 0x00),
},
};


/*******************************************************************************
* 函 数 名: Flexspi_Nor_Wait_Busy
* 功能描述: 等待FlexSPI NOR Flash忙碌状态结束
* 形    参: instance:FlexSPI实例号
            baseAddr:开始读取的Flash地址(32bit)
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
static status_t Flexspi_Nor_Wait_Busy(uint32_t instance, uint32_t baseAddr)
{
    status_t status = kStatus_InvalidArgument;
    flexspi_xfer_t flashXfer;
    uint32_t statusDataBuffer;
    uint32_t busyMask;
    uint32_t busyPolarity;
    bool isBusy = false;
    
    flashXfer.operation = kFLEXSPIOperation_Read;
    flashXfer.seqNum = 1;
    busyMask = 1;
    busyPolarity = 0;
    flashXfer.seqId = NOR_CMD_LUT_SEQ_IDX_READSTATUS;
    flashXfer.isParallelModeEnable = false;
    flashXfer.baseAddress = baseAddr;
    flashXfer.rxBuffer = &statusDataBuffer;	
    flashXfer.rxSize = sizeof(statusDataBuffer);
    do
    {
        status = ROM_FLEXSPI_NorFlash_CommandXfer(instance, &flashXfer);
        // Busy bit is 0 if polarity is 1
        if(busyPolarity)
        {
            isBusy = (~statusDataBuffer) & busyMask;
        }
        else
        {
            isBusy = statusDataBuffer & busyMask;
        }
    }while (isBusy);

    return status;
}


/*******************************************************************************
* 函 数 名: Flexspi_Nor_Write_Enable
* 功能描述: 使能 FlexSPI NOR Flash的写入操作
* 形    参: instance:FlexSPI实例号
            baseAddr:开始读取的Flash地址(32bit)
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
static status_t Flexspi_Nor_Write_Enable(uint32_t instance, uint32_t baseAddr)
{
    status_t status = kStatus_InvalidArgument;
    flexspi_xfer_t flashXfer;
    
    flashXfer.operation = kFLEXSPIOperation_Command;
    flashXfer.seqNum = 1;
    flashXfer.seqId = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;
    flashXfer.isParallelModeEnable = false;
    flashXfer.baseAddress = baseAddr;
    
    status = ROM_FLEXSPI_NorFlash_CommandXfer(instance, &flashXfer);

    return status;
}


/*******************************************************************************
* 函 数 名: flexspi_clock_config
* 功能描述: 配置FlexSPI模块的时钟
* 形    参: instance:FlexSPI实例号
            freq:表示所需的FlexSPI时钟频率
            sampleClkMode:指定FlexSPI时钟的采样时钟模式,可以选择SDR或DDR模式
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
static void flexspi_clock_config(uint32_t instance, uint32_t freq, uint32_t sampleClkMode)
{
    uint32_t pfd480 = 0;
    uint32_t cscmr1 = 0;
    uint32_t frac = 0;
    uint32_t podf = 0;

    typedef struct _flexspi_clock_param
    {
        uint8_t frac;
        uint8_t podf;
    } flexspi_clock_param_t;

    const flexspi_clock_param_t k_sdr_clock_config[kFLEXSPISerialClk_200MHz + 1] = {
    //Reserved,  30MHz    50MHz    60MHz     75MHz   80MHz   100MHz   133MHz   166MHz   200MHz
        {0, 0}, {34, 8}, {22, 8}, {24, 6}, {30, 4}, {18, 6}, {14, 6}, {17, 4}, {26, 2}, {22, 2}
    };
    const flexspi_clock_param_t k_ddr_clock_config[kFLEXSPISerialClk_200MHz + 1] = {
    //Reserved,  30MHz    50MHz    60MHz     75MHz   80MHz   100MHz   133MHz   166MHz   200MHz
        {0, 0}, {24, 6}, {22, 4}, {12, 6}, {30, 2}, {18, 3}, {22, 2}, {33, 1}, {26, 1}, {22, 1}
    };

    do
    {
        if((sampleClkMode != kFLEXSPIClk_SDR) && (sampleClkMode != kFLEXSPIClk_DDR))
        {
            break;
        }

        pfd480 = CCM_ANALOG->PFD_480 & (~CCM_ANALOG_PFD_480_PFD0_FRAC_MASK);
        cscmr1 = CCM->CSCMR1 & (~CCM_CSCMR1_FLEXSPI_PODF_MASK);

        const flexspi_clock_param_t *flexspi_config_array = NULL;
        if(sampleClkMode == kFLEXSPIClk_SDR)
        {
            flexspi_config_array = &k_sdr_clock_config[0];
        }
        else
        {
            flexspi_config_array = &k_ddr_clock_config[0];
        }

        if(freq >= kFLEXSPISerialClk_30MHz)
        {
            if(freq > kFLEXSPISerialClk_200MHz)
            {
                freq = kFLEXSPISerialClk_30MHz;
            }

            frac = flexspi_config_array[freq].frac;
            podf = flexspi_config_array[freq].podf;

            pfd480 |= CCM_ANALOG_PFD_480_PFD0_FRAC(frac);
            cscmr1 |= CCM_CSCMR1_FLEXSPI_PODF(podf - 1);

            FLEXSPI->MCR0 |= FLEXSPI_MCR0_MDIS_MASK;
            flexspi_clock_gate_disable();

            if(pfd480 != CCM_ANALOG->PFD_480)
            {
                CCM_ANALOG->PFD_480 = pfd480;
            }
            if(cscmr1 != CCM->CSCMR1)
            {
                CCM->CSCMR1 = cscmr1;
            }
            flexspi_clock_gate_enable();
            FLEXSPI->MCR0 &= ~FLEXSPI_MCR0_MDIS_MASK;
        }
        else
        {
            //Do nothing
        }
    } while (0);
}


/*******************************************************************************
* 函 数 名: flexspi_clock_gate_enable
* 功能描述: 开启FlexSPI模块的时钟门控
* 形    参: 无
* 返 回 值: 无 
*******************************************************************************/
static void flexspi_clock_gate_enable(void)
{
    CCM->CCGR6 |= CCM_CCGR6_CG5_MASK;
}


/*******************************************************************************
* 函 数 名: flexspi_clock_gate_disable
* 功能描述: 关闭FlexSPI模块的时钟门控
* 形    参: 无
* 返 回 值: 无 
*******************************************************************************/
static void flexspi_clock_gate_disable(void)
{
    CCM->CCGR6 &= (uint32_t)~CCM_CCGR6_CG5_MASK;
}


/*******************************************************************************
* 函 数 名: flexspi_get_clock
* 功能描述: 获取FlexSPI时钟频率
* 形    参: instance:FlexSPI实例号
            type:所需时钟类型
            freq:用于存储获取到的时钟频率类型
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
static status_t flexspi_get_clock(uint32_t instance, flexspi_clock_type_t type, uint32_t *freq)
{
    uint32_t clockFrequency = 0;
    status_t status = kStatus_Success;

    uint32_t ahbBusDivider;
    uint32_t seralRootClkDivider;
    uint32_t arm_clock = SystemCoreClock;

    switch (type)
    {
        case kFlexSpiClock_CoreClock:
            clockFrequency = SystemCoreClock;
            break;
        case kFlexSpiClock_AhbClock:
        {
            // Note: In I.MXRT_512, actual AHB clock is IPG_CLOCK_ROOT
            ahbBusDivider = ((CCM->CBCDR & CCM_CBCDR_IPG_PODF_MASK) >> CCM_CBCDR_IPG_PODF_SHIFT) + 1;
            clockFrequency = arm_clock / ahbBusDivider;
        }
        break;
        case kFlexSpiClock_SerialRootClock:
        {
            uint32_t pfdFrac;
            uint32_t pfdClk;

            // FLEXPI CLK SEL
            uint32_t flexspi_clk_src =
                (CCM->CSCMR1 & CCM_CSCMR1_FLEXSPI_CLK_SEL_MASK) >> CCM_CSCMR1_FLEXSPI_CLK_SEL_SHIFT;

            // PLL_480_PFD0
            pfdFrac = (CCM_ANALOG->PFD_480 & CCM_ANALOG_PFD_480_PFD0_FRAC_MASK) >> CCM_ANALOG_PFD_480_PFD0_FRAC_SHIFT;
            pfdClk = (480000000UL) / pfdFrac * 18;

            seralRootClkDivider = ((CCM->CSCMR1 & CCM_CSCMR1_FLEXSPI_PODF_MASK) >> CCM_CSCMR1_FLEXSPI_PODF_SHIFT) + 1;

            clockFrequency = pfdClk / seralRootClkDivider;
        }
        break;
        default:
            status = kStatus_InvalidArgument;
            break;
    }
    *freq = clockFrequency;

    return status;
}


/*******************************************************************************
* 函 数 名: flexspi_get_ticks
* 功能描述: 计算FlexSPI时钟周期数
* 形    参: ticks:用于存储计算结果的指针，即所需的时钟周期数；
            intervalNs:所需的时间间隔，以纳秒（ns）为单位；
            freq:FlexSPI时钟频率，单位为MHz；
            unit:时钟周期单位,即计算结果中每个时钟周期代表的时间长度,以ns为单位
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
static status_t flexspi_get_ticks(uint32_t *ticks, uint32_t intervalNs, uint32_t freq, uint32_t unit)
{
    status_t status = kStatus_InvalidArgument;
    do
    {
        if((ticks == NULL) || (freq < 1) || (unit < 1))
        {
            break;
        }

        // Get clock cycle in terms of ns
        int32_t calculatedTicks;
        uint32_t cycleNs = FLEXSPI_FREQ_1GHz / freq;

        calculatedTicks = intervalNs / (cycleNs * unit);
        while(calculatedTicks * cycleNs * unit < intervalNs)
        {
            calculatedTicks++;
        }

        *ticks = calculatedTicks;

        status = kStatus_Success;

    } while (0);

    return status;
}


/*******************************************************************************
* 函 数 名: flexspi_configure_dll
* 功能描述: 用来配置FLEXSPI存储器的DLL(延迟锁存器)
* 形    参: instance:FLEXSPI实例号
            config:存储器配置信息，包括读取时钟源、数据有效时间等参数
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
static status_t flexspi_configure_dll(uint32_t instance, flexspi_mem_config_t *config)
{
    status_t status = kStatus_InvalidArgument;
    bool mdisConfigRequired;

    do
    {
        bool isUnifiedConfig = true;
        uint32_t flexspiRootClk;
        uint32_t flexspiDll[2];
        uint32_t dllValue;
        uint32_t temp;
        if(config->readSampleClkSrc > kFLEXSPIReadSampleClk_ExternalInputFromDqsPad)
        {
            break;
        }
        switch (config->readSampleClkSrc)
        {
            case kFLEXSPIReadSampleClk_LoopbackInternally:
            case kFLEXSPIReadSampleClk_LoopbackFromDqsPad:
            case kFLEXSPIReadSampleClk_LoopbackFromSckPad:
                isUnifiedConfig = true;
                break;
            case kFLEXSPIReadSampleClk_ExternalInputFromDqsPad:
                isUnifiedConfig = false;
                break;
            default: // Never reach here
                break;
        }

        if(isUnifiedConfig)
        {
            flexspiDll[0] = FLEXSPI_DLLCR_DEFAULT; // 1 fixed delay cells in DLL delay chain)
            flexspiDll[1] = FLEXSPI_DLLCR_DEFAULT; // 1 fixed delay cells in DLL delay chain)
        }
        else
        {
            flexspi_get_clock(instance, kFlexSpiClock_SerialRootClock, &flexspiRootClk);

            bool useDLL = false;

            //See FlexSPI Chapter for more details
            if((flexspiRootClk >= 100 * FREQ_1MHz) &&
                (!(config->controllerMiscOption & (1U << kFLEXSPIMiscOffset_UseValidTimeForAllFreq))))
            {
                useDLL = true;
            }
            if(useDLL)
            {
                flexspiDll[0] = FLEXSPI_DLLCR_DLLEN(1) | FLEXSPI_DLLCR_SLVDLYTARGET(0x0F);
                flexspiDll[1] = FLEXSPI_DLLCR_DLLEN(1) | FLEXSPI_DLLCR_SLVDLYTARGET(0x0F);
            }
            else
            {
                for(uint32_t i = 0; i < 2; i++)
                {
                    uint32_t dataValidTimeH = config->dataValidTime[i].delay_cells;
                    uint32_t dataValidTimeL = config->dataValidTime[i].time_100ps;
                    if(dataValidTimeH < 1)
                    {
                        // Convert the data valid time to n ps.
                        temp = dataValidTimeL * 100ul;
                        if(temp < 1)
                        {
                            uint32_t maxFreq = (166UL * 1000 * 1000);
                            bool is_ddr_enabled = (config->controllerMiscOption & 1<< kFLEXSPIMiscOffset_DdrModeEnable)?true:false;
                            /* For SDR mode, the delay cell configuration must ensure that the delay time is greater
                              than Half cycle of max  supported frequency*/
                            if(!is_ddr_enabled)
                            {
                                dllValue = FLEXSPI_FREQ_1GHz / maxFreq / 2 * 1000 / kFlexSpiDelayCellUnit_Min + 1;
                            }
                            /* For SDR mode, the delay cell configuration must ensure that the delay time is greater
                               than 1/4 cycle of max supported frequency */
                            else
                            {
                                dllValue = FLEXSPI_FREQ_1GHz / maxFreq / 4 * 1000 / kFlexSpiDelayCellUnit_Min + 1;
                            }
                        }
                        else
                        {
                            dllValue = temp / kFlexSpiDelayCellUnit_Min;
                            if(dllValue * kFlexSpiDelayCellUnit_Min < temp)
                            {
                                dllValue++;
                            }
                        }
                    }
                    else
                    {
                        dllValue = dataValidTimeH;
                    }
                    // Calculate maximum dll value;
                    temp = (FLEXSPI_DLLCR_OVRDVAL_MASK >> FLEXSPI_DLLCR_OVRDVAL_SHIFT);
                    if(dllValue > temp)
                    {
                        dllValue = temp;
                    }
                    flexspiDll[i] = FLEXSPI_DLLCR_OVRDEN(1) | FLEXSPI_DLLCR_OVRDVAL(dllValue);
                }
            }
        }

        if(FLEXSPI->MCR0 & FLEXSPI_MCR0_MDIS_MASK)
        {
            mdisConfigRequired = false;
        }
        else
        {
            mdisConfigRequired = true;
        }

        if(mdisConfigRequired)
        {
            FLEXSPI->MCR0 |= FLEXSPI_MCR0_MDIS_MASK;
        }

        FLEXSPI->DLLCR[0] = flexspiDll[0];
        FLEXSPI->DLLCR[1] = flexspiDll[1];

        if(mdisConfigRequired)
        {
            FLEXSPI->MCR0 &= (uint32_t)~FLEXSPI_MCR0_MDIS_MASK;
        }
        /* Wait at least 100 NOPs*/
        for(uint8_t delay = 100U; delay > 0U; delay--)
        {
            __NOP();
        }
        status = kStatus_Success;
    } while (0);

    return status;
}


/*******************************************************************************
* 函 数 名: flexspi_config_mcr1
* 功能描述: 配置FlexSPI模块的MCR1寄存器
* 形    参: instance:FLEXSPI实例号
            onfig指向FlexSPI存储器配置结构体的指针
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
static status_t flexspi_config_mcr1(uint32_t instance, flexspi_mem_config_t *config)
{
    uint32_t seqWaitTicks = 0xFFFFu;
    uint32_t ahbBusWaitTicks = 0xFFFFu;
    uint32_t serialRootClockFreq;
    uint32_t ahbBusClockFreq;

    if(config == NULL)
    {
        return kStatus_InvalidArgument;
    }

    flexspi_get_clock(instance, kFlexSpiClock_SerialRootClock, &serialRootClockFreq);
    flexspi_get_clock(instance, kFlexSpiClock_AhbClock, &ahbBusClockFreq);
    flexspi_get_ticks(&seqWaitTicks, FLEXSPI_WAIT_TIMEOUT_NS, serialRootClockFreq, 1024);
    flexspi_get_ticks(&ahbBusWaitTicks, FLEXSPI_WAIT_TIMEOUT_NS, ahbBusClockFreq, 1024);

    if(seqWaitTicks > 0xFFFF)
    {
        seqWaitTicks = 0xFFFF;
    }
    if(ahbBusWaitTicks > 0xFFFF)
    {
        ahbBusWaitTicks = 0xFFFF;
    }

    // Configure MCR1
    FLEXSPI->MCR1 = FLEXSPI_MCR1_SEQWAIT(seqWaitTicks) | FLEXSPI_MCR1_AHBBUSWAIT(ahbBusWaitTicks);

    return kStatus_Success;
}


/*******************************************************************************
* 函 数 名: FLASH_GetSectorSize
* 功能描述: 获取扇区大小
* 形    参: 无
* 返 回 值: 返回扇区大小,HYPER FLASH为64K字节,NOR FLASH为4K字节
*******************************************************************************/
uint32_t FLASH_GetSectorSize(void)
{
#ifndef HYPER_FLASH
    return 4096;//QSPI Flash Sector Size
#else
    return 256*1024UL;//Hyper Flash Sector Size
#endif
}


/*******************************************************************************
* 函 数 名: FLASH_GetProgramCmd
* 功能描述: 获取页大小
* 形    参: 无
* 返 回 值: 返回页大小,HYPER FLASH为512字节,NOR FLASH为256字节
*******************************************************************************/
uint32_t FLASH_GetProgramCmd(void)
{
    uint32_t Program_Unit;
#ifndef HYPER_FLASH
    Program_Unit = 256;//QSPI Flash Page Program
#else
    Program_Unit = 512;//Hyper Flash Page Program
#endif

    return Program_Unit;
}


/*******************************************************************************
* 函 数 名: FLASH_Init
* 功能描述: Flash接口初始化,需在进行Flash相关操作前进行调用
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void FLASH_Init(void)
{
    /* Update LUT Table for Status, Write Enable, Erase and Program */
    ROM_FLEXSPI_NorFlash_UpdateLut(0, NOR_CMD_LUT_SEQ_IDX_READSTATUS, (const uint32_t *)FlashLookupTable.ReadStatus_Seq, 10U);
    /* Use 30MHz Flexspi clock for safe operation */
    flexspi_clock_config(0, kFLEXSPISerialClk_30MHz, kFLEXSPIClk_DDR);
    extern flexspi_nor_config_t Qspiflash_config;
    flexspi_config_mcr1(0, &Qspiflash_config.memConfig);
    flexspi_configure_dll(0, &Qspiflash_config.memConfig);
    ROM_FLEXSPI_NorFlash_ClearCache(0);
}


/*******************************************************************************
* 函 数 名: FLASH_DeInit
* 功能描述: Flash接口反初始化，需在完成Flash相关操作后进行调用
* 形    参: 无
* 返 回 值: 无
*******************************************************************************/
void FLASH_DeInit(void)
{
    lookuptable_t clearlut;
    memset(&clearlut, 0, sizeof(lookuptable_t));
    ROM_FLEXSPI_NorFlash_UpdateLut(0, NOR_CMD_LUT_SEQ_IDX_READSTATUS, (const uint32_t *)FlashLookupTable.ReadStatus_Seq, 10U);
    /* Use 30MHz Flexspi clock for safe operation */
    flexspi_clock_config(0, kFLEXSPISerialClk_30MHz, kFLEXSPIClk_DDR);
}


/*******************************************************************************
* 函 数 名: FLASH_EraseSector
* 功能描述: 擦除一个Flash扇区
* 形    参: addr:擦除区域起始地址
* 返 回 值: None 
* 注    释: 擦除一个扇区的最少时间:30ms~200/400ms
*******************************************************************************/
uint8_t FLASH_EraseSector(uint32_t addr)
{
    status_t status;
    flexspi_xfer_t flashXfer;
    addr &= 0x0FFFFFFF;

    flashXfer.operation = kFLEXSPIOperation_Command;
    flashXfer.seqNum = 1;
    flashXfer.seqId = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;
    flashXfer.baseAddress = addr;
    flashXfer.isParallelModeEnable = false;

    __disable_irq();
    status = Flexspi_Nor_Write_Enable(0, addr);
    status = ROM_FLEXSPI_NorFlash_CommandXfer(0, &flashXfer);
    status = Flexspi_Nor_Wait_Busy(0, addr);
    ROM_FLEXSPI_NorFlash_ClearCache(0);
    __enable_irq();

    return status;
}


/*******************************************************************************
* 函 数 名: FLASH_WritePage
* 功能描述: 写Flash一个页
* 形    参: addr:写入区域起始地址
            buf:数据存储区
            len:要写入的字节数(最大256)
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
* 注    释: 在指定地址开始写入最大256字节的数据
*******************************************************************************/
uint8_t FLASH_WritePage(uint32_t addr, const uint32_t *buf, uint32_t len)
{
    status_t status;
    flexspi_xfer_t flashXfer;
    addr &= 0x0FFFFFFF;


    flashXfer.operation = kFLEXSPIOperation_Write;
    flashXfer.seqNum = 1;
    flashXfer.seqId = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM;
    flashXfer.baseAddress = addr;
    flashXfer.isParallelModeEnable = false;	
    flashXfer.txBuffer = (uint32_t *)buf;
    flashXfer.txSize = len;

    __disable_irq();
    status = Flexspi_Nor_Write_Enable(0, addr);
    status = ROM_FLEXSPI_NorFlash_CommandXfer(0, &flashXfer);
    status = Flexspi_Nor_Wait_Busy(0, addr);
    ROM_FLEXSPI_NorFlash_ClearCache(0);
    __enable_irq();

    return status;
}


/*******************************************************************************
* 函 数 名: FLASH_Read
* 功能描述: 读Flash内容
* 形    参: addr:读取区域起始地址
            buf:数据存储区
            len:要读取的字节数
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
status_t FLASH_Read(uint32_t addr, uint32_t *buf, uint32_t len)
{
    status_t status;
    flexspi_xfer_t flashXfer;

    addr &= 0x0FFFFFFF;

    flashXfer.operation = kFLEXSPIOperation_Read;
    flashXfer.seqNum = 1;
    flashXfer.seqId = NOR_CMD_LUT_SEQ_IDX_READ;
    flashXfer.baseAddress = addr;
    flashXfer.isParallelModeEnable = false;	
    flashXfer.rxBuffer = buf;
    flashXfer.rxSize = len;

    __disable_irq();
    ROM_FLEXSPI_NorFlash_ClearCache(0);
    status = ROM_FLEXSPI_NorFlash_CommandXfer(0, &flashXfer);
    __enable_irq();

    return status;
}


/*******************************************************************************
* 函 数 名: flash_erase
* 功能描述: 擦除Flash指定长度的空间
* 形    参: addr:擦除区域起始地址
            byte_cnt:要擦除的字节数,以4k字节为最小擦除单位
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码
* 注    释: 不满4k字节的，也需要擦除掉4k字节
*******************************************************************************/
status_t flash_erase(uint32_t start_addr, uint32_t byte_cnt)
{
    uint32_t addr;
    status_t status;

    addr = start_addr;
    while(addr < (byte_cnt + start_addr))
    {
        status = FLASH_EraseSector(addr);
        if(status != kStatus_Success)
        {
            return status;
        }
        addr += FLASH_GetSectorSize();
    }
    return status;
}


/*******************************************************************************
* 函 数 名: flash_write
* 功能描述: 在指定的flash起始地址写入指定长度的数据
* 形    参: addr:写入区域起始地址
            buf:数据存储区
            byte_cnt:要写入的字节数
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
status_t flash_write(uint32_t start_addr, uint8_t *buf, uint32_t byte_cnt)
{
    uint32_t size;
    status_t status;
    while(byte_cnt > 0)
    {
        size = byte_cnt > FLASH_PAGE_SIZE ? FLASH_PAGE_SIZE : byte_cnt;
        status = FLASH_WritePage(start_addr, (void *)buf, size);
        if(status != kStatus_Success) 
        {
            return status;
        }
        start_addr += size;
        buf += size;
        byte_cnt -= size;
    }

    return kStatus_Success;
}


/*******************************************************************************
* 函 数 名: flash_read
* 功能描述: 读Flash内容
* 形    参: addr:读取区域起始地址
            buf:数据存储区
            len:要读取的字节数
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
status_t flash_read(uint32_t addr, uint8_t *buf, uint32_t len)
{   
    /* For FlexSPI Memory ReadBack, use IP Command instead of AXI command for security */
    if((addr >= 0x60000000) && (addr < 0x61000000))
    {
        return FLASH_Read(addr, (void *)buf, len);
    }
        
    else
    {
        void* result = memcpy(buf, (void*)addr, len);
        if(result == NULL)
        {
            return (status_t)kStatus_Fail;  
        } 
        else 
        {
            return (status_t)kStatus_Success;  
        }
        
    }
}


/*******************************************************************************
* 函 数 名: flash_copy
* 功能描述: 实现flash数据在分区之间的拷贝
* 形    参: srcAddr:源flash的起始地址
            dstAddr:目标flash的起始地址;
            imageSize:要拷贝的flash空间大小,单位为字节
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
*******************************************************************************/
status_t flash_copy(uint32_t srcAddr,uint32_t dstAddr, uint32_t imageSize)
{   
    uint32_t PageNum, Remain, i;
    status_t status;

    if((srcAddr == dstAddr) || imageSize > APP_FLASH_SIZE)
    {
        return (status_t)kStatus_Fail;
    }

    status = flash_erase(dstAddr,imageSize);
    if(status != kStatus_Success)
    {
        KPrintf("Erase flash 0x%08x failure !\r\n",dstAddr);
        return status;
    }

    PageNum = imageSize/FLASH_PAGE_SIZE;
    Remain = imageSize%FLASH_PAGE_SIZE;

    for(i=0;i<PageNum;i++)
    {
        memset(buffer, 0, sizeof(buffer));
        status = flash_read(srcAddr + i*FLASH_PAGE_SIZE, buffer, sizeof(buffer));
        if(status != kStatus_Success)
        {
            KPrintf("Read flash 0x%08x failure !\r\n", srcAddr + i*FLASH_PAGE_SIZE);
            return status;
        }
        status = flash_write(dstAddr+ i*FLASH_PAGE_SIZE, buffer, FLASH_PAGE_SIZE);
        if(status != kStatus_Success)
        {
            KPrintf("Write flash 0x%08x failure !\r\n", dstAddr + i*FLASH_PAGE_SIZE);
            return status;
        }
    }

    if(Remain)
    {
        memset(buffer, 0, sizeof(buffer));
        status = flash_read(srcAddr + i*FLASH_PAGE_SIZE, buffer, Remain);
        if(status != kStatus_Success)
        {
            KPrintf("Read flash 0x%08x failure !\r\n", srcAddr + i*FLASH_PAGE_SIZE);
            return status;
        }
        status = flash_write(dstAddr+ i*FLASH_PAGE_SIZE, buffer, Remain);
        if(status != kStatus_Success)
        {
            KPrintf("Write flash 0x%08x failure !\r\n", dstAddr + i*FLASH_PAGE_SIZE);
            return status;
        }
    }

    return (status_t)kStatus_Success; 
}


/*******************************************************************************
* 函 数 名: NOR_FLASH_Erase
* 功能描述: 以扇区为擦除单位擦除Flash指定长度的空间,最终擦除的字节可能大于imageSize
* 形    参: addr:擦除区域起始地址
            imageSize:要擦除的字节数
* 返 回 值: None 
*******************************************************************************/
status_t NOR_FLASH_Erase(uint32_t app_base_addr,uint32_t imageSize)
{
    uint16_t i;
    uint32_t sectorNum = (imageSize%SECTOR_SIZE != 0)? (imageSize/SECTOR_SIZE + 1):(imageSize/SECTOR_SIZE);
    
    for(i=0;i<sectorNum;i++)
    {
       status_t status = FLASH_EraseSector(app_base_addr+i*SECTOR_SIZE);
    
        if (status != kStatus_Success)
        {
            KPrintf("Erase_Sector 0x%x faild!\r\n",i*SECTOR_SIZE);
            return status;
        }
    }
    return kStatus_Success;
}


/*******************************************************************************
* 函 数 名: NorFlash_Write_PageProgram
* 功能描述: 写入Flash指定长度的数据
* 形    参: pBuffer:数据存储区
            WriteAddr:写入区域起始地址
            NumByteToWrite:要写入的字节数(最大256)
* 返 回 值: 如果函数执行成功，状态值为 kStatus_Success，否则状态值为其他错误码 
* 注    释: 在指定地址开始写入最大256字节的数据
*******************************************************************************/
void NorFlash_Write_PageProgram(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint8_t temp_data[256] = {0xff};

    memcpy(temp_data,pBuffer,NumByteToWrite);
    
   status_t status = FLASH_WritePage(WriteAddr,(void *)temp_data,FLASH_PAGE_SIZE);
    if (status != kStatus_Success)
    {
        KPrintf("Write_PageProgram 0x%x faild!\r\n",WriteAddr);
    }
}


/*******************************************************************************
* 函 数 名: NorFlash_Write_NoCheck
* 功能描述: 无检验写入W25QXX从指定地址开始指定长度的数据
* 形    参: pBuffer:数据存储区
            WriteAddr:开始写入的地址(24bit)
            NumByteToWrite:要写入的字节数(最大65535)
* 返 回 值: 无
* 注    释: 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
            具有自动换页功能,在指定地址开始写入指定长度的数据,但是要确保地址不越界!
*******************************************************************************/
void NorFlash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint16_t pageRemain;	
    
    pageRemain = 256 - WriteAddr%256;//单页剩余的字节数
    
    if(NumByteToWrite <= pageRemain)
    {
        pageRemain = NumByteToWrite;//不大于256个字节
    }

    while(1)
    {
        NorFlash_Write_PageProgram(pBuffer,WriteAddr,pageRemain);
        if(NumByteToWrite == pageRemain)
        {
            break;//写入结束了	
        }
        else //NumByteToWrite>pageRemain
        {
            pBuffer += pageRemain;
            WriteAddr += pageRemain;

            NumByteToWrite -= pageRemain;//减去已经写入了的字节数
            if(NumByteToWrite > 256)
            {
                pageRemain = 256;//一次可以写入256个字节
            }
            else 
            {
                pageRemain = NumByteToWrite;//不够256个字节了
            }
        }
    }
}


/*******************************************************************************
* 函 数 名: NorFlash_Write
* 功能描述: 写入W25QXX在指定地址开始写入指定长度的数据 
* 形    参: pBuffer:数据存储区
            WriteAddr:开始写入的地址(24bit)
            NumByteToWrite:要写入的字节数(最大65535)  
* 返 回 值: None 
* 注    释: 该函数带擦除操作
*******************************************************************************/
void NorFlash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{ 
    uint32_t secPos;
    uint16_t secOff;
    uint16_t secRemain;
    uint16_t i;    
    uint8_t *NorFlash_BUF = 0;

    NorFlash_BUF = NorFlash_BUFFER;//RAM缓冲区4K

    WriteAddr &= 0x0FFFFFFF;
    
    secPos = WriteAddr/SECTOR_SIZE;//扇区地址
    secOff = WriteAddr%SECTOR_SIZE;//在扇区内的偏移
    secRemain = SECTOR_SIZE - secOff;//扇区剩余空间大小
    
    if(NumByteToWrite <= secRemain)
    {
        secRemain = NumByteToWrite;//不大于4096个字节
    }
    while(1) 
    {
        FLASH_Read(CHIP_FLAH_BASE + secPos*SECTOR_SIZE, (void *)NorFlash_BUF, SECTOR_SIZE);//读出整个扇区的内容
        for(i=0;i<secRemain;i++)//校验数据
        {
            if(NorFlash_BUF[secOff+i] != 0xFF)
            {
                break;//需要擦除
            }
        }
        if(i < secRemain)//需要擦除
        {
            FLASH_EraseSector(CHIP_FLAH_BASE + secPos*SECTOR_SIZE);
            for(i=0;i<secRemain;i++)//复制
            {
                NorFlash_BUF[i+secOff] = pBuffer[i];                       
            }
            NorFlash_Write_NoCheck(NorFlash_BUF,CHIP_FLAH_BASE + secPos*SECTOR_SIZE,SECTOR_SIZE);//写入整个扇区  
        }
        else
        {
            NorFlash_Write_NoCheck(pBuffer,CHIP_FLAH_BASE + WriteAddr,secRemain);//写已经擦除了的,直接写入扇区剩余区间. 		
        }
                           
        if(NumByteToWrite == secRemain)
        {
            break;//写入结束了
        }
        else//写入未结束
        {
            secPos++;//扇区地址增1
            secOff=0;//偏移位置为0

            pBuffer += secRemain;//指针偏移
            WriteAddr += secRemain;//写地址偏移
            NumByteToWrite -= secRemain;//字节数递减
            if(NumByteToWrite > SECTOR_SIZE)
            {
                secRemain = SECTOR_SIZE;//下一个扇区还是写不完	
            }
            else
            {
                secRemain = NumByteToWrite;//下一个扇区可以写完了	
            }
        }
    }
}


/*******************************************************************************
* 函 数 名: NOR_FLASH_Write
* 功能描述: 写入W25QXX在指定地址开始写入指定长度的数据 
* 形    参: FlashAddress:用于存储当前写入Flash地址的指针，写入过程中会移动
            Data:要写入数据存储区
            DataLength:要写入的字节数
* 返 回 值: 0 
*******************************************************************************/
#ifndef  USE_HIGHT_SPEED_TRANS
uint32_t NOR_FLASH_Write(uint32_t* FlashAddress, uint8_t* Data ,uint16_t DataLength)
{
    uint32_t WriteAddr;
    WriteAddr = *FlashAddress;
    NorFlash_Write(Data,WriteAddr,DataLength);
    *FlashAddress += DataLength;
    return 0;
}
#else
uint8_t packetNum = 0;
uint32_t dataLen = 0;
uint32_t WriteAddr;
uint8_t dataBuff[5*1024];
uint32_t NOR_FLASH_Write(uint32_t* FlashAddress, uint8_t* Data ,uint16_t DataLength,uint8_t doneFlag)
{
    if(!doneFlag)
    {  
        memcpy(&dataBuff[dataLen],Data,DataLength);
        dataLen += DataLength;
        packetNum ++;
        if(1 == packetNum)
        {
            WriteAddr = *FlashAddress;    		 
        }

        if(dataLen>=SECTOR_SIZE)
        {
            NorFlash_Write(dataBuff,WriteAddr,dataLen);
            packetNum = 0;
            dataLen = 0; 
        }
        *FlashAddress += DataLength;
    }
    else
    {
        NorFlash_Write(dataBuff,WriteAddr,dataLen);
        packetNum = 0;
        dataLen = 0; 
    }
   return (0);
}
#endif
