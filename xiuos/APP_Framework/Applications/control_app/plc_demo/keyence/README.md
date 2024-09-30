# keyence通信测试

[TOC]

## 通信接线及参数设置

* 网口

  *Mosbus TCP协议，IP：192.168.250.40，Port：502

## 存储区

- 存储区ZF区。

## JSON配方设计

* 共测试INT16共1种类型数据,以下为JSON文件解释。

  - ```json
    {
        "device_id": 1,                     //设备ID默认是1，此参数无效
        "device_name": "KV8000",            //设备名称，自定义
        "communication_type": 0,            //通讯协议类型 0是以太网，1是串口 
        "socket_config": {                  //以太网配置
            "plc_ip": "192.168.250.40",     //PLC的IP地址
            "local_ip": "192.168.250.233",  //矽达通IP地址设定
            "gateway": "192.168.250.1",     //矽达通的网关地址设定
            "netmask": "255.255.255.0",     //矽达通子网掩码设定
            "port":502                      //端口号设定
            },
        "protocol_type": 3,                 //通讯协议，3代表modbus-tcp协议
        "read_period": 100,                 //交互周期ms
        "read_item_list": [
            {
                "value_name": "ZF0",  //变量名称，自定义
                "value_type": 3,      //变量类型，BOOL = 1,INT8 = 2,INT16,INT32,UINT8,UINT16,UINT32,DOUBLE,FLOAT = 9
                "function_code": 3,   //功能码。3是读
            	"start_address": 0,   //起始地址
                "data_length": 1      //BOOL长度，默认是1，代表读取1个BOOL长度
            },
            {
                "value_name": "ZF2",  //变量名称，自定义
                "value_type": 3,      //变量类型，BOOL = 1,INT8 = 2,INT16,INT32,UINT8,UINT16,UINT32,DOUBLE,FLOAT = 9
                "function_code": 3,   //功能码。3/6是读
            	"start_address": 2,   //起始地址偏移1位
                "data_length": 1      //BOOL长度，默认是1，代表读取1个BOOL长度
            }
        ]
    }
    ```

## 通信测试

 (1) 新增1个通信demo，命名为keyence_KV8000.c；

 (2) 复制样例代码程序到keyence_KV8000.c文件中；

 (3) void **ControlKV8000Test**(void) 更改函数名；

 (4) PRIV_SHELL_CMD_FUNCTION(**ControKV8000Test**, keyence plc KV8000 Demo**, PRIV_SHELL_CMD_MAIN_ATTR);更改测试指令；

 (5) 剪裁配置完成后，用过烧写器下载至矽达通中，重启后完成测试。



 