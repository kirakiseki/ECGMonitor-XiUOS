# INOVANCE AM4011608TN通信测试

[TOC]

## 通信接线及参数设置

* 网口

  * CN3 EtherNET,Mosbus TCP协议，IP：192.168.250.50，Port：502

* 串口
  * CN1 RS485，AM401只支持一路串口，AM600可支持两路串口。AM401接线：pin1:485-，pin2:485+。波特率：9600，数据位：8位，停止位：1位，校验：偶校验

## 存储区

- 存储区 I，Q，M区。

## 通信测试

（1）共测试BOOL，INT16，INT32，FLOAT共四种类型数据。

（2）测试M区及Q区数据。

（3）M区数据测试，用功能码03，以字为单位读取。如读MX0.3,则读取MW0，然后按位拆解。如读MW100，则配方文件中起始地址则直接写100即可。如读MDx，则配方文件中起始地址应为2*x，这是汇川的地址编码规则决定，如MD200，则对应400。

（4）Q区数据测试，用功能码01，以位为单位读取。如读QX.Y，则配方文件中起始地址为X*8+Y；如读QW，QD等，则需按位进行读取后然后组合得到。