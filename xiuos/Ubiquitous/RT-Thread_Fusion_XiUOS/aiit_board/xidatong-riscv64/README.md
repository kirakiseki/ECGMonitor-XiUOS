# 矽达通-RISCV智能盒子说明



|         功能          | **引脚序号**<br />（并非原理图连接线序号） |    引脚定义    |
| :-------------------: | :----------------------------------------: | :------------: |
|   高速串口作为shell   |                     5                      |     ISP_RX     |
|                       |                     6                      |     ISP_TX     |
|         W5500         |                     9                      | SPI1_ENET_SCLK |
|                       |                     10                     | SPI1_ENET_MISO |
|                       |                     11                     | SPI1_ENET_MOSI |
|                       |                     12                     | SPI1_ENET_nCS  |
|                       |                     13                     | SPI1_ENET_nRST |
|                       |                     14                     |   ENET_nINT    |
| LCD-043MCU-TP   touch |                     15                     |    I2C_SDA     |
|                       |                     16                     |                |
|                       |                     17                     |    I2C_SCL     |
|        can模块        |                     18                     |   UART1_RXD    |
|                       |                     19                     |   UART1_TXD    |
|        4G模块         |                     20                     |   UART2_TXD    |
|                       |                     21                     |   UART2_RXD    |
|   CH376 = USB + TF    |                     22                     |   UART3_TXD    |
|                       |                     23                     |   UART3_RXD    |
|         CH438         |                     24                     |   CH438_ALE    |
|                       |                     25                     |   CH438_NWR    |
|                       |                     26                     |   CH438_NRD    |
|                       |                     27                     |    CH438_D0    |
|                       |                     28                     |    CH438_D1    |
|                       |                     29                     |    CH438_D2    |
|                       |                     30                     |    CH438_D3    |
|                       |                     31                     |    CH438_D4    |
|                       |                     32                     |    CH438_D5    |
|                       |                     33                     |    CH438_D6    |
|                       |                     34                     |    CH438_D7    |
|                       |                     35                     |   CH438_INT    |
|                       |                     36                     |    pTP_INT     |
|                       |                     37                     |    LCD_nRST    |
|                       |                     38                     |    LCD_SCLK    |
|                       |                     39                     |    LCD_MOSI    |
|                       |                     40                     |    LCD_MISO    |
|                       |                     41                     |    LCD_nCS     |
|                       |                     43                     |    CAN_nCFG    |
|                       |                     44                     |    E220_M0     |
|                       |                     45                     |    E220_M1     |
|                       |                     46                     |    E18_MODE    |
|                       |                     47                     |     LCD_BL     |

其中CH438 = ZIGBEE + LORA + BT + 3X*UART + 2X*485

## 编译说明
先下载源码，进入xiuos根目录：
    
    git submodule init
    git submodule update Ubiquitous/RT-Thread_Fusion_XiUOS/rt-thread
    git submodule update Ubiquitous/RT-Thread_Fusion_XiUOS/aiit_board/xidatong-riscv64/kendryte-sdk/kendryte-sdk-source
下载好相应的源码以及勘智官方K210 sdk到指定目录

下载risc-v的工具链，[下载地址](https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases)  
    
在`rtconfig.py`中将risc-v工具链的本地路径加入文档。

然后执行scons编译：  
    scons

也可以利用env工具编译，指定工具链路径

         set RTT_EXEC_PATH=D:\xpack-riscv-none-embed-gcc-8.2.0-3.1-win32-x64\xPack\RISC-V Embedded GCC\8.2.0-3.1\bin
         
         scons

