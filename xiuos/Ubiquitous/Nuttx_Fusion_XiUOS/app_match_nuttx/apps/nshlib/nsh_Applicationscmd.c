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
 * @file nsh_Applicationscmd.c
 * @brief nsh cmd function support
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.03.17
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <errno.h>
#include <debug.h>

#include "nsh.h"
#include "nsh_console.h"

extern int FrameworkInit(void);

/****************************************************************************
 * Name: cmd_Ch376
 ****************************************************************************/
#if defined(CONFIG_BSP_USING_CH376) && !defined(CONFIG_NSH_DISABLE_CH376)
extern void CH376Demo(void);
int cmd_Ch376(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    CH376Demo();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_w5500
 ****************************************************************************/
#if defined(CONFIG_BSP_USING_ENET) && !defined(CONFIG_NSH_DISABLE_W5500)
extern void w5500_test(void);
int cmd_w5500(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    w5500_test();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Touch
 ****************************************************************************/
#if defined(CONFIG_BSP_USING_TOUCH) && !defined(CONFIG_NSH_DISABLE_TOUCH)
extern void TestTouch(void);
int cmd_Touch(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    TestTouch();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Ch438
 ****************************************************************************/
#if defined(CONFIG_BSP_USING_CH438) && !defined(CONFIG_NSH_DISABLE_CH438)
extern void CH438Demo(void);
int cmd_Ch438(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    CH438Demo();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_lcd
 ****************************************************************************/
#if defined(CONFIG_USER_TEST_LCD) && !defined(CONFIG_NSH_DISABLE_USER_TEST_LCD)
extern void LcdDemo(void);
int cmd_Lcd(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, LCD!\n");
    LcdDemo();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Extsram
 ****************************************************************************/
#if defined(CONFIG_USER_TEST_SEMC) && !defined(CONFIG_NSH_DISABLE_USER_TEST_SEMC)
extern int ExtsramTest(void);
int cmd_Extsram(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, extra sdram!\n");
    ExtsramTest();
    return OK;
}
#endif

/****************************************************************************
 * Name: socket test
 ****************************************************************************/
#if defined(CONFIG_SOCKET_DEMO) && !defined(CONFIG_NSH_DISABLE_SOCKET_DEMO)
void UdpSocketRecvTest(int argc, char *argv[]);
int cmd_Udprecv(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, UDP receive!\n");
    UdpSocketRecvTest(argc, argv);
    return OK;
}

void UdpSocketSendTest(int argc, char *argv[]);
int cmd_Udpsend(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, UDP send!\n");
    UdpSocketSendTest(argc, argv);
    return OK;
}

void TcpSocketRecvTest(int argc, char *argv[]);
int cmd_Tcprecv(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, TCP receive!\n");
    TcpSocketRecvTest(argc, argv);
    return OK;
}

void TcpSocketSendTest(int argc, char *argv[]);
int cmd_Tcpsend(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, TCP send!\n");
    TcpSocketSendTest(argc, argv);
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Hcho1os
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_HCHO_TB600B_WQ_HCHO1OS) && !defined(CONFIG_NSH_DISABLE_HCHO_TB600B_WQ_HCHO1OS)
extern void HchoTb600bHcho1os(void);
int cmd_Hcho1os(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    HchoTb600bHcho1os();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Tvoc10
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_TVOC_TB600B_TVOC10) && !defined(CONFIG_NSH_DISABLE_TB600B_TVOC10)
extern void TvocTb600bTvoc10(void);
int cmd_Tvoc10(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    TvocTb600bTvoc10();
    return OK;
}
#endif


/****************************************************************************
 * Name: cmd_Iaq10
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_IAQ) && !defined(CONFIG_NSH_DISABLE_TB600B_IAQ10)
extern void IaqTb600bIaq10(void);
int cmd_Iaq10(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    IaqTb600bIaq10();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_As830
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_CH4_AS830) && !defined(CONFIG_NSH_DISABLE_CH4_AS830)
extern void Ch4As830(void);
int cmd_As830(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    Ch4As830();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Co2Zg09
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_CO2_ZG09) && !defined(CONFIG_NSH_DISABLE_CO2ZG09)
extern void Co2Zg09(void);
int cmd_Co2Zg09(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    Co2Zg09();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Co2g8s
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_CO2_G8S) && !defined(CONFIG_NSH_DISABLE_CO2G8S)
extern void Co2G8s(void);
int cmd_Co2G8S(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    Co2G8s();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Pm10Ps5308
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_PM1_0_PS5308) && !defined(CONFIG_NSH_DISABLE_PM1_0PS5308)
extern void Pm10Ps5308(void);
int cmd_Pm10Ps5308(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    Pm10Ps5308();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Pm25Ps5308
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_PM2_5_PS5308) && !defined(CONFIG_NSH_DISABLE_PM2_5PS5308)
extern void Pm25Ps5308(void);
int cmd_Pm25Ps5308(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    Pm25Ps5308();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_Pm10Ps5308
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_PM10_PS5308) && !defined(CONFIG_NSH_DISABLE_PM10PS5308)
extern void Pm100Ps5308(void);
int cmd_Pm100Ps5308(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    Pm100Ps5308();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_VoiceD124
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_VOICE_D124) && !defined(CONFIG_NSH_DISABLE_VOICED124)
extern void VoiceD124(void);
int cmd_VoiceD124(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    VoiceD124();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_TempHs300x
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_TEMPERATURE_HS300X) && !defined(CONFIG_NSH_DISABLE_TEMPHS300X)
extern void TempHs300x(void);
int cmd_TempHs300x(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    TempHs300x();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_HumiHs300x
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_HUMIDITY_HS300X) && !defined(CONFIG_NSH_DISABLE_HUMIHS300X)
extern void HumiHs300x(void);
int cmd_HumiHs300x(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    HumiHs300x();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_WindDirectionQsFx
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_WINDDIRECTION_QS_FX) && !defined(CONFIG_NSH_DISABLE_QS_FX)
extern void WindDirectionQsFx(void);
int cmd_WindDirectionQsFx(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    WindDirectionQsFx();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_WindSpeedQsFs
 ****************************************************************************/

#if defined(CONFIG_APPLICATION_SENSOR_WINDSPEED_QS_FS) && !defined(CONFIG_NSH_DISABLE_QS_FS)
extern void WindSpeedQsFs(void);
int cmd_WindSpeedQsFs(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    WindSpeedQsFs();
    return OK;
}
#endif

/****************************************************************************
 * Name: cmd_openzigbee
 ****************************************************************************/

#if defined(CONFIG_CONNECTION_ADAPTER_ZIGBEE) && !defined(CONFIG_NSH_DISABLE_OPENZIGBEE)
extern int OpenZigbee(void);
int cmd_openzigbee(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    OpenZigbee();
    return OK;
}
#endif


#if defined(CONFIG_CONNECTION_ADAPTER_ZIGBEE) && !defined(CONFIG_NSH_DISABLE_SENDZIGBEE)
extern int SendZigbee(int argc, char *argv[]);
int cmd_sendzigbee(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    SendZigbee(argc,argv);
    return OK;
}
#endif

#if defined(CONFIG_CONNECTION_ADAPTER_ZIGBEE) && !defined(CONFIG_NSH_DISABLE_RECVZIGBEE)
extern int RecvZigbee(void);
int cmd_recvzigbee(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    RecvZigbee();
    return OK;
}
#endif

#if (defined(CONFIG_ADAPTER_ESP07S_WIFI) || defined(CONFIG_ADAPTER_ESP8285_WIFI)) && !defined(CONFIG_NSH_DISABLE_ADAPTER_WIFI_TEST)
extern int AdapterWifiTestWithParam(int argc, char *argv[]);
int cmd_AdapterWifiTest(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    AdapterWifiTestWithParam(argc, argv);
    return OK;
}
#endif

#if (defined(CONFIG_ADAPTER_LORA_SX1278) || defined(CONFIG_ADAPTER_LORA_E220) || defined(CONFIG_ADAPTER_LORA_E22)) && \
    !defined(CONFIG_NSH_DISABLE_ADAPTER_LORATEST)
extern int AdapterLoraTest(void);
int cmd_AdapterLoraTest(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    AdapterLoraTest();
    return OK;
}
#endif

#if defined(CONFIG_ARCH_BOARD_EDU_RISCV64) && defined(CONFIG_K210_UART2)
extern void TestLora(void);
int cmd_TestLora(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    TestLora();
    return OK;
}
#endif

#if defined(CONFIG_ADAPTER_4G_EC200T) && !defined(CONFIG_NSH_DISABLE_ADAPTER_4GTEST)
extern int Adapter4GTest(void);
int cmd_Adapter4GTest(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    Adapter4GTest();
    return OK;
}
#endif

#if defined(CONFIG_ADAPTER_LORA_E220) && !defined(CONFIG_NSH_DISABLE_E220_LORA_RECEIVE)
void E220LoraReceive(void);
int cmd_E220LoraReceive(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    E220LoraReceive();
    return OK;
}
#endif

#if defined(CONFIG_ADAPTER_LORA_E220) && !defined(CONFIG_NSH_DISABLE_E220_LORA_SEND)
extern void E220LoraSend(int argc, char *argv[]);
int cmd_E220LoraSend(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    E220LoraSend(argc,argv);
    return OK;
}
#endif

#if defined(CONFIG_ADAPTER_LORA_E22) && !defined(CONFIG_NSH_DISABLE_E22_LORA_RECEIVE)
void E22LoraReceive(void);
int cmd_E22LoraReceive(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    E22LoraReceive();
    return OK;
}
#endif

#if defined(CONFIG_ADAPTER_LORA_E22) && !defined(CONFIG_NSH_DISABLE_E22_LORA_SEND)
extern void E22LoraSend(int argc, char *argv[]);
int cmd_E22LoraSend(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    E22LoraSend(argc,argv);
    return OK;
}
#endif

#if defined(CONFIG_ADAPTER_BLUETOOTH_HC08) && !defined(CONFIG_NSH_DISABLE_ADAPTER_BLUETOOTH_TEST)
extern int AdapterBlueToothTest(void);
int cmd_AdapterBlueToothTest(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    FrameworkInit();
    AdapterBlueToothTest();
    return OK;
}
#endif

#if defined(CONFIG_K210_FFT_TEST) && !defined(CONFIG_NSH_DISABLE_K210_FFT)
extern void nuttx_k210_fft_test(void);
int cmd_fft(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, fft!\n");
    nuttx_k210_fft_test();
    return OK;
}
#endif

#if defined(CONFIG_MUSL_LIBC) && !defined(CONFIG_NSH_DISABLE_MUSL_TEST)
extern void Testmusl(void);
int cmd_musl(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, test musl!\n");
    Testmusl();
    return OK;
}
#endif

#if defined(CONFIG_BSP_USING_CAN) && !defined(CONFIG_NSH_DISABLE_CAN_TEST)
extern void can_test(void);
int cmd_cantest(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
    nsh_output(vtbl, "Hello, world!\n");
    can_test();
    return OK;
}
#endif