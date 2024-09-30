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
 * @file lwip_ping_demo.c
 * @brief Demo for ping function
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2021.12.15
 */

#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_RAW

#include "ping.h"

#include "lwip/timeouts.h"
#include "lwip/init.h"
#include "netif/ethernet.h"
#include <shell.h>
#include "board.h"

#include <sys_arch.h>
#include "connect_ethernet.h"
#include <string.h>

ip4_addr_t ping_addr;

/******************************************************************************/
char arg_ip[20] = {192, 168, 130, 50};

void LwipPingTest(int argc, char *argv[])
{
    int result = 0;

    if (argc == 2) {
        printf("lw: [%s] ping %s\n", __func__, argv[1]);
        if(isdigit(argv[1][0]))
        {
            if(sscanf(argv[1], "%d.%d.%d.%d", &arg_ip[0], &arg_ip[1], &arg_ip[2], &arg_ip[3]) == EOF)
            {
                lw_notice("input wrong ip\n");
                return;
            }
        }
#if (LWIP_DHCP) && (PING_USE_SOCKETS)
        else
        {
            get_url_ip(argv[1]);
            return;
        }
#endif
    }

    printf("lw: [%s] argc %d\n", __func__, argc);

    IP4_ADDR(&ping_addr, arg_ip[0], arg_ip[1], arg_ip[2], arg_ip[3]);
    ping_init(&ping_addr);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_PARAM_NUM(2),
     ping, LwipPingTest, ping [IP] 10 times);

#endif
