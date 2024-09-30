/*
* Copyright (c) 2020 AIIT XUOS Lab
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
 * @file adapter_Modbus_rtu.c
 * @brief Implement the connection 4G adapter function
 * @version 1.1
 * @author AIIT XIUOS Lab
 * @date 2021.06.25
 */

#include <adapter.h>



static int AdapterModbusRtuRegister(struct Adapter *adapter)
{
    int ret = 0;

    strncpy(adapter->name, ADAPTER_MODBUS_RTU_NAME, NAME_NUM_MAX);

    adapter->net_protocol = PROTOCOL_NONE;//IP层协议类型，串口没有此层
    adapter->net_role = SLAVE;
    adapter->adapter_status = UNREGISTERED;

    ret = AdapterDeviceRegister(adapter);
    if (ret < 0) {
        printf("AdapterModbusRtu register error\n");
        return -1;
    }

    return ret;
}

int AdapterModbusRtuInit(void)
{
    int ret = 0;

    struct Adapter *adapter = PrivMalloc(sizeof(struct Adapter));
    if (!adapter) {
        PrivFree(adapter);
        return -1;
    }

    memset(adapter, 0, sizeof(struct Adapter));

    ret = AdapterModbusRtuRegister(adapter);
    if (ret < 0) {
        printf("AdapterModbusRtuInit register ModbusRtu adapter error\n");
        PrivFree(adapter);
        return -1;
    }

    return ret;
}

