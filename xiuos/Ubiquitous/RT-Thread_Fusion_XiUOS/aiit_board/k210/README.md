# K210最小系统板（Max bit）说明

## OV2640 menuconfig 配置：

​	More Driver-------->ov2640 driver （勾选） 保存即可



##  RW007 menuconfig 配置：

​	More Driver-------->rw007:SPI WIFI rw007 driver

​																 example driver port (not use example driver, porting by myself)  

​																(20000000)  SPI  MAX Hz

​    Board Drivers Config 

​															 Enable SPI1

​																				(27)  spi1 clk pin number                  

​                      														  (28)  spi1 d0 pin number                                                                         

​																			    (26)  spi1 d1 pin number 

​																				SPI1 Enable SS1（spi11 dev）-------->(8)  spi1 ss1 pin number											

​															 (spi11) the SPIDEV rw007 driver on                                                         

​                     										(7) rw007 int pin for rw007                                                                                  

​															 (6) rw007 rst pin for rw007

**SPI1 Enable SS1（spi11 dev）表示SPI1总线片选编号1 ，此时挂载在总线上设备名是spi11，所以 the SPIDEV rw007 driver on参数也要填写（spi11）**

## SD卡配置：

​	 Board Drivers Config-------->Enable SDCARD (spi1(ss0))  （勾选）保存即可		SPI1 Enable SS0（spi10 dev）-------->(29)  spi1 ss1 pin number 会默认配置

**SD卡和RW007共用一条spi硬件总线 ，其中片选设备sd卡为SPI1 Enable SS0（spi10 dev），RW007片选设备为SPI1 Enable SS1（spi11 dev）**



​												

**上述引脚根据电路实际而定,另外涉及到相关的Lwip wifi framwork等已经默认配置并匹配**

## 以下为引脚硬件的连接表

## RW007(SPI1 ) Kendryte Sipeed MAX bit io

| 引脚               | 作用      | 引脚序号 | RW007板子 |
| ------------------ | --------- | -------- | --------- |
| io 27(印丝标注SCK) | SPI1_SCK  |          | SCK       |
| io 26(印丝标注SO)  | SPI1_MISO |          | MISO      |
| io 28(印丝标注SI)  | SPI1_MOSI |          | MOSI      |
| io 8               | CS/BOOT1  |          | CS        |
| io 7               | INT/BUSY  |          | D9        |
| io 6               | RESET     |          | D8        |





## SD卡Kendryte Sipeed MAX bit io

| 引脚               | 作用      | 引脚序号 | RW007板子 |
| ------------------ | --------- | -------- | --------- |
| io 27(印丝标注SCK) | SPI1_SCK  |          | SCK       |
| io 26(印丝标注SO)  | SPI1_MISO |          | MISO      |
| io 28(印丝标注SI)  | SPI1_MOSI |          | MOSI      |
| io 29              | CS/BOOT1  |          | CS        |



**注意：BSP_SPI1_D0_PIN 10  d0也就是MOSI  ，sd卡可直接利用Max bit板载，无需重新接线。SD卡和Rw007设备共用一条SPI1总线**





## 编译说明
先下载源码，进入xiuos根目录：
    
    git submodule init
    git submodule update Ubiquitous/RT-Thread_Fusion_XiUOS/rt-thread
    git submodule update Ubiquitous/RT-Thread_Fusion_XiUOS/aiit_board/k210/kendryte-sdk/kendryte-sdk-source
下载好相应的源码以及勘智官方K210 sdk到指定目录

下载risc-v的工具链，[下载地址](https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases)  
    
在`rtconfig.py`中将risc-v工具链的本地路径加入文档。

然后执行scons编译：  
    scons

也可以利用env工具编译，指定工具链路径

         set RTT_EXEC_PATH=D:\xpack-riscv-none-embed-gcc-8.2.0-3.1-win32-x64\xPack\RISC-V Embedded GCC\8.2.0-3.1\bin
         
         scons

