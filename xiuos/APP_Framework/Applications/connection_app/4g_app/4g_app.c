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
 * @file 4g_app.c
 * @brief support get data from and send data to 4g server
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022.12.12
 */

#include <stdio.h>
#include <string.h>
#include <transform.h>
#include <adapter.h>

static uint8_t adapter_4g_status = 0;
static pthread_t recv_4g_heart_task;
struct Adapter *adapter_4g;

static const uint8_t server_addr[] = "xxx.xxx.xxx.xxx";
static const uint8_t server_port[] = "xxx";

#define ADAPTER_4G_HEART "HEART"

int Adapter4GConnectFunction(struct Adapter *adapter, uint8_t reconnect)
{
    int ret = 0;
    int baud_rate = BAUD_RATE_115200;
    
    if (1 != reconnect) {
        ret = AdapterDeviceOpen(adapter);
        if (ret < 0) {
            goto out;
        }

        ret = AdapterDeviceControl(adapter, OPE_INT, &baud_rate);
        if (ret < 0) {
            goto out;
        }
    }

    ret = AdapterDeviceConnect(adapter, CLIENT, server_addr, server_port, IPV4);
    if (ret < 0) {
        goto out;
    }

out:
    if (ret < 0) {
        AdapterDeviceClose(adapter);
    }
    return ret;
}

void Adapter4gSend(uint8_t *send_data, size_t length)
{
    if (adapter_4g_status) {
        AdapterDeviceSend(adapter_4g, send_data, length);
    }
}

static void *Receive4gHeartTask(void* parameter)
{
    char recv_msg[16] = {0};
    ssize_t recv_length = 0;
    uint8_t net_status_cnt = 0;
    
    while (1) {

        SetTaskStatus(0x01);

        if (net_status_cnt > 5) {
            adapter_4g_status = 0;

            while (Adapter4GConnectFunction(adapter_4g, 1) < 0) {
                PrivTaskDelay(10000);
            }

            net_status_cnt = 0;
        }

        adapter_4g_status = 1;

        recv_length = AdapterDeviceRecv(adapter_4g, recv_msg, 6);
        if (recv_length > 0) {
            //if (0 == strcmp(recv_msg, ADAPTER_4G_HEART)) {
                net_status_cnt = 0;
            //}
        } else {
            printf("4G recv heart error re-recv cnt %d\n", net_status_cnt);
            net_status_cnt++;
        }
        memset(recv_msg, 0, sizeof(recv_msg));
    }
}

int Adapter4GActive(void)
{
    int ret = 0;
    adapter_4g = AdapterDeviceFindByName(ADAPTER_4G_NAME);

#ifdef ADAPTER_EC200T
    adapter_4g->socket.socket_id = 0;

    ret = Adapter4GConnectFunction(adapter_4g, 0);
    if (ret < 0) {
        printf("Adapter4GConnect failed %d\n", ret);
    }

    adapter_4g_status = 1;

    pthread_attr_t attr;
    attr.schedparam.sched_priority = 22;
    attr.stacksize = 2048;

    PrivTaskCreate(&recv_4g_heart_task, &attr, &Receive4gHeartTask, NULL);
    PrivTaskStartup(&recv_4g_heart_task);

#endif

    return ret;    
}


