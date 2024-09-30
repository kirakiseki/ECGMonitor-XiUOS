/*
 * Copyright (c) 2021 AIIT XUOS Lab
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
 * @file lwip_config_demo.c
 * @brief Demo for ping function
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2021.12.15
 */

#include <board.h>
#include <sys_arch.h>
#include <shell.h>
#include <sys.h>
#include <string.h>

/******************************************************************************/
uint8_t enet_id = 0;
static void LwipSetIPTask(void *param)
{
    uint8_t enet_port = *(uint8_t *)param; ///< test enet port 
    printf("lw: [%s] config netport id[%d]\n", __func__, enet_port);
    // lwip_config_net(enet_port, lwip_ipaddr, lwip_netmask, lwip_gwaddr);
    lwip_config_tcp(enet_port, lwip_ipaddr, lwip_netmask, lwip_gwaddr);
}

void LwipSetIPTest(int argc, char *argv[])
{
    if(argc >= 4)
    {
        printf("lw: [%s] ip %s mask %s gw %s netport %s\n", __func__, argv[1], argv[2], argv[3], argv[4]);
        sscanf(argv[1], "%d.%d.%d.%d", &lwip_ipaddr[0], &lwip_ipaddr[1], &lwip_ipaddr[2], &lwip_ipaddr[3]);
        sscanf(argv[2], "%d.%d.%d.%d", &lwip_netmask[0], &lwip_netmask[1], &lwip_netmask[2], &lwip_netmask[3]);
        sscanf(argv[3], "%d.%d.%d.%d", &lwip_gwaddr[0], &lwip_gwaddr[1], &lwip_gwaddr[2], &lwip_gwaddr[3]);
        sscanf(argv[4], "%d", &enet_id);

        if(0 == enet_id)
        {
            printf("save eth0 info\n");
            memcpy(lwip_eth0_ipaddr, lwip_ipaddr, 20);
            memcpy(lwip_eth0_netmask, lwip_netmask, 20);
            memcpy(lwip_eth0_gwaddr, lwip_gwaddr, 20);
        }
        else if(1 == enet_id)
        {
            printf("save eth1 info\n");
            memcpy(lwip_eth1_ipaddr, lwip_ipaddr, 20);
            memcpy(lwip_eth1_netmask, lwip_netmask, 20);
            memcpy(lwip_eth1_gwaddr, lwip_gwaddr, 20);
        }
    }
    else if(argc == 2)
    {
        printf("lw: [%s] set eth0 ipaddr %s \n", __func__, argv[1]);
        sscanf(argv[1], "%d.%d.%d.%d", &lwip_ipaddr[0], &lwip_ipaddr[1], &lwip_ipaddr[2], &lwip_ipaddr[3]);
        memcpy(lwip_eth0_ipaddr, lwip_ipaddr, strlen(lwip_ipaddr));
    }
    // sys_thread_new("SET ip address", LwipSetIPTask, &enet_id, LWIP_TASK_STACK_SIZE, LWIP_DEMO_TASK_PRIO);
    LwipSetIPTask(&enet_id);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_PARAM_NUM(5),
     setip, LwipSetIPTest, setip [IP] [Netmask] [Gateway] [port]);


void LwipShowIPTask(int argc, char *argv[])
{
#ifdef configMAC_ADDR
    char mac_addr0[] = configMAC_ADDR;
#endif

    lw_notice("\r\n************************************************\r\n");
    lw_notice(" Network Configuration\r\n");
    lw_notice("************************************************\r\n");
    lw_notice(" ETH0 IPv4 Address   : %u.%u.%u.%u\r\n", ((u8_t *)&lwip_eth0_ipaddr)[0], ((u8_t *)&lwip_eth0_ipaddr)[1],
        ((u8_t *)&lwip_eth0_ipaddr)[2], ((u8_t *)&lwip_eth0_ipaddr)[3]);
    lw_notice(" ETH0 IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&lwip_eth0_netmask)[0], ((u8_t *)&lwip_eth0_netmask)[1],
        ((u8_t *)&lwip_eth0_netmask)[2], ((u8_t *)&lwip_eth0_netmask)[3]);
    lw_notice(" ETH0 IPv4 Gateway   : %u.%u.%u.%u\r\n", ((u8_t *)&lwip_gwaddr)[0], ((u8_t *)&lwip_eth0_gwaddr)[1],
        ((u8_t *)&lwip_eth0_gwaddr)[2], ((u8_t *)&lwip_eth0_gwaddr)[3]);
#ifdef configMAC_ADDR
    lw_notice(" ETH0 MAC Address    : %x:%x:%x:%x:%x:%x\r\n", mac_addr0[0], mac_addr0[1], mac_addr0[2],
        mac_addr0[3], mac_addr0[4], mac_addr0[5]);
#endif
#ifdef BOARD_NET_COUNT
    if(BOARD_NET_COUNT > 1)
    {
        char mac_addr1[] = configMAC_ADDR_ETH1;
        lw_notice("\r\n");
        lw_notice(" ETH1 IPv4 Address   : %u.%u.%u.%u\r\n", ((u8_t *)&lwip_eth1_ipaddr)[0], ((u8_t *)&lwip_eth1_ipaddr)[1],
            ((u8_t *)&lwip_eth1_ipaddr)[2], ((u8_t *)&lwip_eth1_ipaddr)[3]);
        lw_notice(" ETH1 IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&lwip_eth1_netmask)[0], ((u8_t *)&lwip_eth1_netmask)[1],
            ((u8_t *)&lwip_eth1_netmask)[2], ((u8_t *)&lwip_eth1_netmask)[3]);
        lw_notice(" ETH1 IPv4 Gateway   : %u.%u.%u.%u\r\n", ((u8_t *)&lwip_eth1_gwaddr)[0], ((u8_t *)&lwip_eth1_gwaddr)[1],
            ((u8_t *)&lwip_eth1_gwaddr)[2], ((u8_t *)&lwip_eth1_gwaddr)[3]);
        lw_notice(" ETH1 MAC Address    : %x:%x:%x:%x:%x:%x\r\n", mac_addr1[0], mac_addr1[1], mac_addr1[2],
            mac_addr1[3], mac_addr1[4], mac_addr1[5]);
    }
#endif
    lw_notice("************************************************\r\n");
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_PARAM_NUM(0),
     showip, LwipShowIPTask, GetIp [IP] [Netmask] [Gateway]);

