/**
* @file board.h
* @brief define imxrt1176-sbc-board init configure and start-up function
* @version 1.0
* @author AIIT XUOS Lab
* @date 2021-05-28
*/

/*************************************************
File name: board.h
Description: define imxrt1176-sbc board init function and struct
Others:
History:
1. Date: 2022-08-19
Author: AIIT XUOS Lab
Modification:
1. define imxrt-board InitBoardHardware
2. define imxrt-board heap struct
*************************************************/

#ifndef __BOARD_H__
#define __BOARD_H__

#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "fsl_enet.h"
#include "clock_config.h"
#include "pin_mux.h"
#include <xizi.h>
#include <arch_interrupt.h>

#define BOARD_NET_COUNT (2)
/* MAC address configuration. */
#define configMAC_ADDR {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}
#define configMAC_ADDR_ETH1 {0x02, 0x12, 0x13, 0x10, 0x15, 0x12}

/*! @brief The ENET0 PHY address. */
#define BOARD_ENET0_PHY_ADDRESS (0x02U) /* Phy address of enet port 0. */

/*! @brief The ENET1 PHY address. */
#define BOARD_ENET1_PHY_ADDRESS (0x07U) /* Phy address of enet port 1. */

#define BOARD_FLASH_SIZE    (0x1000000U)

extern int heap_start;
extern int heap_end;
#define HEAP_BEGIN          (&heap_start)
#define HEAP_END            (&heap_end)
#define HEAP_SIZE           ((uint32_t)HEAP_END - (uint32_t)HEAP_BEGIN)

void InitBoardHardware(void);

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "IMXRT1050"

#define NVIC_PRIORITYGROUP_0         0x00000007U /*!< 0 bits for pre-emption priority
                                                      4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         0x00000006U /*!< 1 bits for pre-emption priority
                                                      3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         0x00000005U /*!< 2 bits for pre-emption priority
                                                      2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         0x00000004U /*!< 3 bits for pre-emption priority
                                                      1 bits for subpriority */
#define NVIC_PRIORITYGROUP_4         0x00000003U /*!< 4 bits for pre-emption priority*/



#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void BOARD_ConfigMPU(void);
void BOARD_InitPins(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
