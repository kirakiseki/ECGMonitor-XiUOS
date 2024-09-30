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
 * @file adapter_wifi.h
 * @brief Implement the connection wifi adapter function
 * @version 1.1
 * @author AIIT XUOS Lab
 * @date 2021.07.25
 */

#ifndef ADAPTER_WIFI_H
#define ADAPTER_WIFI_H

#define CONFIG_WIFI_RESET            (0)
#define CONFIG_WIFI_RESTORE          (1)
#define CONFIG_WIFI_BAUDRATE         (2)


#define SOCKET_PROTOCOL_TCP  (6)
#define SOCKET_PROTOCOL_UDP  (17)

struct WifiParam
{
    char wifi_ssid[128];
    char wifi_pwd[128];
};


#endif