# 三菱 FX2N通信测试

[TOC]

## 通信接线及参数设置

* 串口
  * FX2N自带8针圆口422，用于程序的下载。全系列不支持网口，且需购买串口拓展模块FX2N-485-BD用于通信测试。
  * 接线：RDA和SDA短接，引出A；RDB与SDB短接，引出B。
  * 串口模块支持MC-1C协议，通信速率：9600；数据位：7bit；停止位：1bit；校验：偶校验

## 存储区

- 存储区 I，Q，M，D区。

## 通信测试

-  共测试BOOL，INT16，FLOAT共三种类型数据。


-  测试M区及D区数据。



