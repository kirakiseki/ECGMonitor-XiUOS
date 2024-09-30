# SCHNEIDER M241L通信测试

[TOC]

## 通信接线及参数设置

* 串口
* M241支持2路485串口，本次采用的是serial2。波特率：9600，数据位：8位，停止位：1位，校验：偶校验

## 存储区

- 存储区MW区。

## JSON配方设计

* 共测试BOOL，INT16共2种类型数据,以下为JSON文件解释。

  - ```json
    {
        "device_id": 1,                     //设备ID默认是1，此参数无效
        "device_name": "m241",              //设备名称，自定义
        "communication_type": 1,            //通讯协议类型 0是以太网，1是串口 
        "serial_config": {                  //串口配置
            "station": 1,                   //站号
            "baud_rate": 9600,              //波特率
            "data_bits": 8,                 //数据位
            "stop_bits": 1,                 //停止位
            "check_mode": 3                 //1无校验，2校验，3偶校验
        },
        "protocol_type": 3,                 //通讯协议，3代表modbus-tcp协议
        "read_period": 100,                 //交互周期ms
        "read_item_list": [
            {
                "value_name": "MW0",  //变量名称，自定义
                "value_type": 3,      //变量类型，BOOL = 1,INT8 = 2,INT16,INT32,UINT8,UINT16,UINT32,DOUBLE,FLOAT = 9
                "function_code": 3,   //功能码。3是读
            	"start_address": 0,   //起始地址
                "data_length": 1      //BOOL长度，默认是1，代表读取1个BOOL长度
            },
            {
                "value_name": "MW1",  //变量名称，自定义
                "value_type": 3,      //变量类型，BOOL = 1,INT8 = 2,INT16,INT32,UINT8,UINT16,UINT32,DOUBLE,FLOAT = 9
                "function_code": 3,   //功能码。3是读
            	"start_address": 1,   //起始地址偏移1位
                "data_length": 1      //BOOL长度，默认是1，代表读取1个BOOL长度
            }
        ]
    }
    ```

## 通信测试

 (1) 新增1个通信demo，命名为schneider_m241.c；

 (2) 复制样例代码程序到schneider_m241.c文件中；

 (3) void **ControlSCHNEIDERM241Test**(void) 更改函数名；

 (4) PRIV_SHELL_CMD_FUNCTION(**ControlM241Test**, **Schneider M241 Demo**, PRIV_SHELL_CMD_MAIN_ATTR);更改测试指令；

 (5) 剪裁配置完成后，用过烧写器下载至矽达通中，重启后完成测试。



 