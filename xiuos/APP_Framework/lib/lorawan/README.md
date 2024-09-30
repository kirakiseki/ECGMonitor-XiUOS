# lorawan子模块调试说明

矽璓工业物联操作系统XiUOS目前支持lorawan相关开源库，通过子模块形式管理，该目录主要内容是包含**lora_radio_driver**、**lorawan_devicenode**、**lorawan_gateway_single_channel**等。

## 目录内容

```
xiuos/APP_Framework/lib/lorawan
    ├── README.md    
    ├── lora_radio_driver                   lora_radio驱动库
    ├── lorawan_devicenode                  lorawan节点协议栈
    ├── lorawan_gateway_single_channel      lorawan单通道网关协议栈
    ├── Kconfig                             lorawan Kconfig配置
    └── Makefile                            lorawan Makefile
```

## 使用

使用前执行以下操作：

```
# 下载代码
# 进入APP_Framework/lib/lorawan目录下载更新子模块
git submodule init
git submodule update APP_Framework/lib/lorawan/lora_radio_driver
git submodule update APP_Framework/lib/lorawan/lorawan_devicenode

# 进入 APP_Framework/lib/lorawan/Kconfig 配置，增加子模块source路径，从而编译时可找到相应lib的配置
menuconfig LIB_USING_LORAWAN_GATEWAY_SC
    bool "LoRaWan using lorawan_gateway for single channel(SX126x、SX127x) lib"
    default n
    select LIB_USING_LORA_RADIO

if LIB_USING_LORAWAN_GATEWAY_SC
    source "$APP_DIR/lib/lorawan/lorawan_gateway_single_channel/Kconfig"
endif

menuconfig LIB_USING_LORAWAN_ED_STACK
    bool "LoRaWan using lorawan_ed_stack lib"
    default n
    select LIB_USING_LORA_RADIO

if LIB_USING_LORAWAN_ED_STACK
    source "$APP_DIR/lib/lorawan/lorawan_devicenode/Kconfig"
endif

menuconfig LIB_USING_LORA_RADIO
    bool "LoRaWan using lora-radio-driver lib"
    default n

if LIB_USING_LORA_RADIO
    source "$APP_DIR/lib/lorawan/lora_radio_driver/Kconfig"
endif

#编译对应OS即可
```

