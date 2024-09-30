/*
 * Copyright (c) 2022 AIIT XUOS Lab
 * XiUOS is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *        http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/**
 * @file modbus_rtu_app.c
 * @brief support get data from and send data to Modbus rtu server
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022.12.12
 */
#include <stdio.h>
#include <string.h>
#include <transform.h>
#include <adapter.h>

#define ADAPTER_RTU_DATA_LENGTH 112
#define ADAPTER_RTU_TRANSFER_DATA_LENGTH ADAPTER_RTU_DATA_LENGTH + 16

uint8_t client_join_data[ADAPTER_RTU_TRANSFER_DATA_LENGTH];

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    return MB_ENOREG;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}


/******************Modbus RTU TEST*********************/
int AdapterModbusRtuTest(void)
{
    const char *send_msg = "Adapter_Modbus_Rtu Test";
    char recv_msg[256] = {0};
    int baud_rate = BAUD_RATE_115200;

    struct Adapter* adapter =  AdapterDeviceFindByName(ADAPTER_MODBUS_RTU_NAME);

    struct AdapterData priv_rtu_net;
    priv_rtu_net.len = ADAPTER_RTU_TRANSFER_DATA_LENGTH;
    priv_rtu_net.buffer = client_join_data;


    AdapterDeviceOpen(adapter);
    AdapterDeviceControl(adapter, OPE_INT, &baud_rate);

    AdapterDeviceJoin(adapter,(uint8_t *)&priv_rtu_net);

    

    eMBErrorCode    eStatus;

    eStatus = eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_EVEN );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

    for( ;; )
    {
        ( void )eMBPoll(  );//判断事件的发生，并执行对应的回调函数

        /* Here we simply count the number of poll cycles. */
        usRegInputBuf[0]++;
    }

    // while (1) {
    //     AdapterDeviceSend(adapter, send_msg, strlen(send_msg));
    //     AdapterDeviceRecv(adapter, recv_msg, 256);
    //     printf("4G recv msg %s\n", recv_msg);
    //     memset(recv_msg, 0, 256);
    // }


    return 0;    
}
PRIV_SHELL_CMD_FUNCTION(AdapterModbusRtuTest, a Modbus Rtu adpter sample, PRIV_SHELL_CMD_FUNC_ATTR);

