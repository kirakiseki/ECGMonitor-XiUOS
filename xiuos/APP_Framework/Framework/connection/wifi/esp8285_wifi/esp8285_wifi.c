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
 * @file esp07_wifi.c
 * @brief Implement the connection wifi adapter function, using ESP8285 device
 * @version 1.1
 * @author AIIT XUOS Lab
 * @date 2022.04.08
 */

#include <adapter.h>
#include <at_agent.h>
#include "../adapter_wifi.h"
#include <stdlib.h>

#define LEN_PARA_BUF 128

static int Esp8285WifiSetDown(struct Adapter *adapter_at);

/**
 * @description: check AT startup status
 * @param at_agent - wifi device agent pointer
 * @return success: EOK
 */
static int Esp8285WifiTestAtCmd(ATAgentType at_agent)
{
    int ret = 0;

    ret = AtCmdConfigAndCheck(at_agent, "ATE0\r\n", "OK"); ///< close echo function
    if(ret < 0) {
        printf("%s %d cmd[ATE0] config failed!\n",__func__,__LINE__);
        ret = -1;
    }

    PrivTaskDelay(2000);
    ret = AtCmdConfigAndCheck(at_agent, "AT\r\n", "OK");
    if(ret < 0) {
        printf("%s %d cmd[AT] config failed!\n",__func__,__LINE__);
        ret = -1;
    }
    return ret;
}

static int Esp8285UartOpen(struct Adapter *adapter)
{
    if (NULL == adapter) {
        return -1;
    }

    /* Open device in read-write mode */
    adapter->fd = PrivOpen(ADAPTER_ESP8285_DRIVER, O_RDWR);
    if (adapter->fd < 0) {
        printf("Esp8285WifiOpen get serial %s fd error\n", ADAPTER_ESP8285_DRIVER);
        return -1;
    }
    /* set serial config, serial_baud_rate = 115200 */

    struct SerialDataCfg cfg;
    memset(&cfg, 0 ,sizeof(struct SerialDataCfg));

    cfg.serial_baud_rate = BAUD_RATE_115200;
    cfg.serial_data_bits = DATA_BITS_8;
    cfg.serial_stop_bits = STOP_BITS_1;
    cfg.serial_parity_mode = PARITY_NONE;
    cfg.serial_bit_order = BIT_ORDER_LSB;
    cfg.serial_invert_mode = NRZ_NORMAL;
    cfg.serial_buffer_size = SERIAL_RB_BUFSZ;
    cfg.is_ext_uart = 0;
#ifdef ADAPTER_ESP8285_DRIVER_EXT_PORT
    cfg.is_ext_uart = 1;
    cfg.ext_uart_no = ADAPTER_ESP8285_DRIVER_EXT_PORT;
    cfg.port_configure = PORT_CFG_INIT;
#endif

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = SERIAL_TYPE;
    ioctl_cfg.args = &cfg;

    PrivIoctl(adapter->fd, OPE_INT, &ioctl_cfg);
    PrivTaskDelay(1000);

    printf("esp8285 uart config ready\n");
    return 0;
}

/**
 * @description: Open wifi
 * @param adapter - wifi device pointer
 * @return success: EOK, failure: ENOMEMORY
 */
static int Esp8285WifiOpen(struct Adapter *adapter)
{
    /*step1: open esp8285 serial port*/
    Esp8285UartOpen(adapter);

    /*step2: init AT agent*/
    if (!adapter->agent) {
        char *agent_name = "wifi_uart_client";
        if (EOK != InitATAgent(agent_name, adapter->fd, 512)) {
            printf("at agent init failed !\n");
            return -1;
        }
        ATAgentType at_agent = GetATAgent(agent_name);

        adapter->agent = at_agent;
    }

    AtSetReplyEndChar(adapter->agent,'O','K');

    ADAPTER_DEBUG("Esp8285Wifi open done\n");

    return 0;
}

/**
 * @description: Close wifi
 * @param adapter - wifi device pointer
 * @return success: EOK
 */
static int Esp8285WifiClose(struct Adapter *adapter)
{
    Esp8285WifiSetDown(adapter);
    PrivClose(adapter->fd);
    return 0;
}

/**
 * @description: send data to adapter
 * @param adapter - wifi device pointer
 * @param data - data buffer
 * @param data - data length
 * @return success: EOK
 */
static int Esp8285WifiSend(struct Adapter *adapter, const void *data, size_t len)
{
    x_err_t result = EOK;
    if (adapter->agent) {
        EntmSend(adapter->agent, (const char *)data, len);
    }else {
        printf("Esp8285WifiSend can not find agent!\n");
	}

    return result;
}

/**
 * @description: receive data from adapter
 * @param adapter - wifi device pointer
 * @param data - data buffer
 * @param data - data length
 * @return success: EOK
 */
static int Esp8285WifiReceive(struct Adapter *adapter, void *rev_buffer, size_t buffer_len)
{
    x_err_t result = EOK;
    printf("esp8285 receive waiting ... \n");

    if (adapter->agent) {
        return EntmRecv(adapter->agent, (char *)rev_buffer, buffer_len, 40);
    } else {
        printf("Esp8285WifiReceive can not find agent!\n");
	}

    return result;
}

/**
 * @description: connnect wifi to internet
 * @param adapter - wifi device pointer
 * @return success: EOK
 */
static int Esp8285WifiSetUp(struct Adapter *adapter)
{
    char cmd[LEN_PARA_BUF];
    int ret = 0;
    char *result = NULL;

    struct WifiParam *param = (struct WifiParam *)adapter->adapter_param;
    struct ATAgent *agent = adapter->agent;

    PrivTaskDelay(2000);

    if(Esp8285WifiTestAtCmd(agent) < 0)
    {
        printf("wifi at cmd startup failed.\n");
        return -1;
    }
    PrivTaskDelay(2000);
    /* config as softAP+station mode */
    ret = AtCmdConfigAndCheck(agent, "AT+CWMODE=3\r\n", "OK");
    if(ret < 0) {
        printf("%s %d cmd[AT+CWMODE=3] config failed!\n",__func__,__LINE__);
        return -1;
    }
    PrivTaskDelay(2000);
    /* connect the router */
    memset(cmd,0,sizeof(cmd));
    strcat(cmd,"AT+CWJAP=");
    strcat(cmd,"\"");
    strcat(cmd,param->wifi_ssid);

    strcat(cmd,"\"");
    strcat(cmd,",");
    strcat(cmd,"\"");
    strcat(cmd,param->wifi_pwd);

    strcat(cmd,"\"");
    strcat(cmd,"\r\n");

    ret = AtCmdConfigAndCheck(agent, cmd, "OK");
    if(ret < 0) {
        printf("%s %d cmd[%s] connect[%s] failed!\n",__func__,__LINE__,cmd,param->wifi_ssid);
        return -1;
    }

    /* check the wifi ip address */
    ATReplyType reply = CreateATReply(256);
    if (NULL == reply) {
        printf("%s %d at_create_resp failed!\n",__func__,__LINE__);
        return -1;
    }
    ret = ATOrderSend(agent, REPLY_TIME_OUT, reply, "AT+CIFSR\r\n");
    if(ret < 0){
        printf("%s %d ATOrderSend AT+CIFSR failed.\n",__func__,__LINE__);
        ret = -1;
        goto __exit;
    }

    result = GetReplyText(reply);
    if (!result) {
        printf("%s %d get reply failed.\n",__func__,__LINE__);
        ret = -1;
        goto __exit;
    }
    printf("[%s]\n", result);

__exit:
    DeleteATReply(reply);

    return ret;
}

/**
 * @description: disconnnect wifi from internet
 * @param adapter - wifi device pointer
 * @return success: EOK
 */
static int Esp8285WifiSetDown(struct Adapter *adapter)
{
    ATOrderSend(adapter->agent, REPLY_TIME_OUT, NULL, "AT+RESTORE\r\n");
    PrivTaskDelay(2000);

    return 0;
}

/**
 * @description: set wifi ip/gateway/netmask address(in sta mode)
 * @param adapter - wifi device pointer
 * @param ip - ip address
 * @param gateway - gateway address
 * @param netmask - netmask address
 * @return success: EOK, failure: ENOMEMORY
 */
static int Esp8285WifiSetAddr(struct Adapter *adapter, const char *ip, const char *gateway, const char *netmask)
{
    int ret = 0;
    char cmd[LEN_PARA_BUF];

    /* e.g. AT+CIPSTA_DEF="192.168.6.100","192.168.6.1","255.255.255.0" */
    memset(cmd,0,sizeof(cmd));
    strncpy(cmd,"AT+CIPAP_DEF=",strlen(" AT+CIPAP_DEF="));
    strcat(cmd,"\"");
    strcat(cmd,ip);
    strcat(cmd,"\"");
    strcat(cmd,",");
    strcat(cmd,"\"");
    strcat(cmd,gateway);
    strcat(cmd,"\"");
    strcat(cmd,",");
    strcat(cmd,"\"");
    strcat(cmd,netmask);
    strcat(cmd,"\"");
    strcat(cmd,"\r\n");

    ret = AtCmdConfigAndCheck(adapter->agent, cmd, "OK");
    if(ret < 0) {
        printf("%s %d cmd[%s] config ip failed!\n",__func__,__LINE__,cmd);
        return -1;
    }

    return 0;
}

/**
 * @description: wifi ping function
 * @param adapter - wifi device pointer
 * @param destination - domain name or ip address
 * @return success: EOK, failure: ENOMEMORY
 */
static int Esp8285WifiPing(struct Adapter *adapter, const char *destination)
{
    char cmd[LEN_PARA_BUF];
    int ret = 0;

    memset(cmd,0,sizeof(cmd));
    strncpy(cmd,"AT+PING=",strlen("AT+PING="));
    strcat(cmd,"\"");
    strcat(cmd,destination);
    strcat(cmd,"\"");
    strcat(cmd,"\r\n");

    ret = AtCmdConfigAndCheck(adapter->agent, cmd, "OK"); ///< config as softAP+station mode
    if(ret < 0) {
        printf("%s %d ping [%s] failed!\n",__func__,__LINE__,destination);
        return -1;
    }

    printf("ping [%s] ok\n", destination);

    return 0;
}

/**
 * @description: display wifi network configuration
 * @param adapter - wifi device pointer
 * @return success: EOK, failure: ENOMEMORY
 */
static int Esp8285WifiNetstat(struct Adapter *adapter)
{
    int ret = 0;
    char *result = NULL;

    /* check the wifi ip address */
    ATReplyType reply = CreateATReply(256);
    if (NULL == reply) {
        printf("%s %d at_create_resp failed!\n",__func__,__LINE__);
        return -1;
    }
    ret = ATOrderSend(adapter->agent, REPLY_TIME_OUT, reply, "AT+CIFSR\r\n");
    if(ret < 0){
        printf("%s %d ATOrderSend AT+CIFSR failed.\n",__func__,__LINE__);
        ret = -1;
        goto __exit;
    }

    result = GetReplyText(reply);
    if (!result) {
        printf("%s %d get reply failed.\n",__func__,__LINE__);
        ret = -1;
        goto __exit;
    }
    printf("[%s]\n", result);

__exit:
    DeleteATReply(reply);

    return ret;
}

/**
 * @description: wifi connect function
 * @param adapter - wifi device pointer
 * @param net_role - net role, CLIENT or SERVER
 * @param ip - ip address
 * @param port - port num
 * @param ip_type - ip type, IPV4 or IPV6
 * @return success: 0, failure: -1
 */
static int Esp8285WifiConnect(struct Adapter *adapter, enum NetRoleType net_role, const char *ip, const char *port, enum IpType ip_type)
{
    int ret = EOK;
    char cmd[LEN_PARA_BUF];
    struct ATAgent *agent = adapter->agent;

    memset(cmd,0,sizeof(cmd));
    if(adapter->socket.protocal == SOCKET_PROTOCOL_TCP && net_role == CLIENT) //esp8285 as tcp client to connect server
    {
        //e.g. AT+CIPSTART="TCP","192.168.3.116",8080 protocol, server IP and port
        strncpy(cmd,"AT+CIPSTART=",strlen("AT+CIPSTART="));
        strcat(cmd,"\"");
        strcat(cmd,"TCP");
        strcat(cmd,"\"");
        strcat(cmd, ",");
        strcat(cmd,"\"");
        strcat(cmd, ip);
        strcat(cmd, "\"");
        strcat(cmd, ",");
        strcat(cmd, port);
        strcat(cmd,"\r\n");

        ret = AtCmdConfigAndCheck(agent, cmd, "OK");
        if(ret < 0) {
            printf("%s %d tcp connect [%s] failed!\n",__func__,__LINE__,ip);
            return -1;
        }
    }
    else if(adapter->socket.protocal == SOCKET_PROTOCOL_UDP)
    {
        //e.g. AT+CIPSTART="UDP","192.168.3.116",8080,2233,0  UDP protocol, server IP, port,local port,udp mode
        strncpy(cmd,"AT+CIPSTART=",strlen("AT+CIPSTART="));
        strcat(cmd,"\"");
        strcat(cmd,"UDP");
        strcat(cmd,"\"");
        strcat(cmd, ",");
        strcat(cmd,"\"");
        strcat(cmd, ip);
        strcat(cmd, "\"");
        strcat(cmd, ",");
        strcat(cmd, port);
        strcat(cmd, ",");
        strcat(cmd, "2233"); ///< local port
        strcat(cmd, ",");
        strcat(cmd, "0"); ///< udp transparent transmission mode must be 0
        strcat(cmd,"\r\n");

        ret = AtCmdConfigAndCheck(agent, cmd, "OK");
        if(ret < 0) {
            printf("%s %d udp connect [%s] failed!\n",__func__,__LINE__,ip);
            return -1;
        }
    }

    ret = AtCmdConfigAndCheck(agent, "AT+CIPMODE=1\r\n", "OK"); ///< config as transparent transmission
    if(ret < 0) {
        printf("%s %d cmd[%s] config as transparent transmission failed!\n",__func__,__LINE__,cmd);
        return -1;
    }
    ATOrderSend(agent, REPLY_TIME_OUT, NULL, "AT+CIPSEND\r\n");

    printf("[%s] connection config as transparent transmission\n",adapter->socket.protocal == SOCKET_PROTOCOL_UDP ? "udp" : "tcp");
    adapter->net_role = net_role;

    return 0;
}

/**
 * @description: wifi disconnect function
 * @param adapter - wifi device pointer
 * @return success: 0, failure: -1
 */
static int Esp8285WifiDisconnect(struct Adapter *adapter)
{
    int ret = EOK;
    char cmd[LEN_PARA_BUF];
    struct ATAgent *agent = adapter->agent;
    memset(cmd,0,sizeof(cmd));

    /* step1: stop transparent transmission mode */
    ATOrderSend(agent, REPLY_TIME_OUT, NULL, "+++\r\n");

    /* step2: exit transparent transmission mode */
    ret = AtCmdConfigAndCheck(agent, "AT+CIPMODE=0\r\n", "OK");
    if(ret < 0) {
        printf("%s %d cmd[AT+CIPMODE=0] exit failed!\n",__func__,__LINE__);
        return -1;
    }

    /* step3: disconnect */
    ret = AtCmdConfigAndCheck(agent, "AT+CIPCLOSE\r\n", "OK");
    if(ret < 0) {
        printf("%s %d cmd [AT+CIPCLOSE] disconnect failed!\n",__func__,__LINE__);
        return -1;
    }

    return 0;
}

static int Esp8285WifiIoctl(struct Adapter *adapter, int cmd, void *args)
{
    int32_t ret = 0;
    char baud_str[LEN_PARA_BUF];
    struct SerialDataCfg cfg;
    char at_cmd[LEN_PARA_BUF];
    uint32_t baud_rate = 0 ;

    switch (cmd)
    {
        case CONFIG_WIFI_RESET: /* reset wifi */
            ATOrderSend(adapter->agent, REPLY_TIME_OUT, NULL, "AT+RST\r\n");
            break;
        case CONFIG_WIFI_RESTORE: /* resore wifi */
            ATOrderSend(adapter->agent, REPLY_TIME_OUT, NULL, "AT+RESTORE\r\n");
            break;
        case CONFIG_WIFI_BAUDRATE:
            /* step1: config mcu uart*/
            baud_rate = *((uint32_t *)args);

            memset(at_cmd, 0, sizeof(at_cmd));
            memset(baud_str, 0, sizeof(baud_str));
            memset(&cfg, 0 ,sizeof(struct SerialDataCfg));

            cfg.serial_baud_rate = baud_rate;
            cfg.serial_data_bits = DATA_BITS_8;
            cfg.serial_stop_bits = STOP_BITS_1;
            cfg.serial_parity_mode = PARITY_NONE;
            cfg.serial_bit_order = BIT_ORDER_LSB;
            cfg.serial_invert_mode = NRZ_NORMAL;
            cfg.serial_buffer_size = SERIAL_RB_BUFSZ;
            cfg.is_ext_uart = 0;
#ifdef ADAPTER_ESP8285_DRIVER_EXT_PORT
            cfg.is_ext_uart = 1;
            cfg.ext_uart_no         = ADAPTER_ESP8285_DRIVER_EXT_PORT;
            cfg.port_configure      = PORT_CFG_INIT;
#endif

            struct PrivIoctlCfg ioctl_cfg;
            ioctl_cfg.ioctl_driver_type = SERIAL_TYPE;
            ioctl_cfg.args = &cfg;

            PrivIoctl(adapter->fd, OPE_INT, &ioctl_cfg);

            /* step2: config wifi uart*/
            itoa(baud_rate, baud_str, 10);

            strncpy(at_cmd, "AT+UART_DEF=", strlen("AT+UART_DEF="));
            strcat(at_cmd, baud_str);
            strcat(at_cmd, ",");
            strcat(at_cmd, "8");
            strcat(at_cmd, ",");
            strcat(at_cmd, "1");
            strcat(at_cmd, ",");
            strcat(at_cmd, "0");
            strcat(at_cmd, ",");
            strcat(at_cmd, "3");
            strcat(at_cmd,"\r\n");

            ret = AtCmdConfigAndCheck(adapter->agent, at_cmd, "OK");
            if(ret < 0) {
                printf("%s %d cmd [%s] config uart failed!\n",__func__,__LINE__,at_cmd);
                ret = -1;
            }

            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

static const struct IpProtocolDone esp8285_wifi_done =
{
    .open = Esp8285WifiOpen,
    .close =  Esp8285WifiClose,
    .ioctl = Esp8285WifiIoctl,
    .setup = Esp8285WifiSetUp,
    .setdown = Esp8285WifiSetDown,
    .setaddr = Esp8285WifiSetAddr,
    .setdns = NULL,
    .setdhcp = NULL,
    .ping = Esp8285WifiPing,
    .netstat = Esp8285WifiNetstat,
    .connect = Esp8285WifiConnect,
    .send = Esp8285WifiSend,
    .recv = Esp8285WifiReceive,
    .disconnect = Esp8285WifiDisconnect,
};

/**
 * @description: Register wifi device esp8285
 * @return success: EOK, failure: ERROR
 */
AdapterProductInfoType Esp8285WifiAttach(struct Adapter *adapter)
{
    struct AdapterProductInfo *product_info = PrivMalloc(sizeof(struct AdapterProductInfo));
    if (!product_info)
    {
        printf("Esp8285WifiAttach Attach malloc product_info error\n");
        PrivFree(product_info);
        return NULL;
    }

    strncpy(product_info->model_name, ADAPTER_WIFI_ESP8285, strlen(ADAPTER_WIFI_ESP8285));

    product_info->model_done = (void *)&esp8285_wifi_done;

    return product_info;
}
