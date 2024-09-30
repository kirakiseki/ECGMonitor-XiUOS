# SIEMENS - S7-1512通信测试

[TOC]

## 通信接线及参数设置

* 本体以太网口

* S7协议，PLC IP：192.168.250.2，Port：102

## 存储区

- 存储区 Q，DB区。

## JSON配方设计

* 共测试BOOL，INT16共2种类型数据,以下为JSON文件解释。

  - ```json
    {
        "device_id": 1,                     //设备ID默认是1，此参数无效
        "device_name": "S7-1512",           //设备名称，自定义
        "communication_type": 0,            //通讯协议类型 0是以太网，1是串口 
        "socket_config": {                  //以太网配置
            "plc_ip": "192.168.250.2",      //PLC的IP地址
            "local_ip": "192.168.250.233",  //矽达通IP地址设定
            "gateway": "192.168.250.1",     //矽达通的网关地址设定
            "netmask": "255.255.255.0",     //矽达通子网掩码设定
            "port": 102                     //端口号设定
        },
        "protocol_type": 1,                 //通讯协议，5代表FINS协议
        "read_period": 100,                 //交互周期ms
        "read_item_list": [
            {
                "value_name": "布尔1",    //变量名称，自定义
                "value_type": 1,         //变量类型，BOOL = 1,INT8 = 2,INT16,INT32,UINT8,UINT16,UINT32,DOUBLE,FLOAT = 9
                "area": "Q",             //变量功能块
                "wordlen": "Bit",        //变量字长类型,有：Bit Byte Word DWord Real Counter Timer
                "db_number": 1,          //如为DB区，填写对应的DB块编号,如不是DB区，无效
                "start": 1,              //BOOL地址偏移位，采集变量地址是Q0.1   
                "amount": 1              //BOOL长度，默认是1，代表读取1个BOOL长度
            },
    		{ 
                "value_name": "整型",     //变量名称，自定义
                "value_type": 3,         //变量类型，BOOL = 1,INT8 = 2,INT16,INT32,UINT8,UINT16,UINT32,DOUBLE,FLOAT = 9
                "area": "DB",            //变量功能块
                "wordlen": "Word",       //变量字长类型,有：Bit Byte Word DWord Real Counter Timer
                "db_number": 18,         //如为DB区，即DB18块
                "start": 2,              //Word地址偏移位，采集变量地址是DB18.DBW2 
                "amount": 1              //Word长度，默认是1，代表读取1个WORD长度，2个字节
            },
            {
                "value_name": "浮点数",   //变量名称，自定义
                "value_type": 9,         //变量类型，BOOL = 1,INT8 = 2,INT16,INT32,UINT8,UINT16,UINT32,DOUBLE,FLOAT = 9
                "area": "DB",            //变量功能块
                "wordlen": "Real",       //变量字长类型,有：Bit Byte Word DWord Real Counter Timer
                "db_number": 18,         //如为DB区，即DB18块
                "start": 18,             //Real地址偏移位，采集变量地址是DB18.DBD18 
                "amount": 1              //Real长度，默认是1，代表读取1个Real长度，4个字节
            }
        ]
    }
    ```

## 通信测试

 (1) 新增1个通信demo，命名为simens_s7_1500.c；

 (2) 复制样例代码程序到simens_s7_1500.c文件中；

 (3) void **ControlS71500Test**(void)更改函数名；

 (4) PRIV_SHELL_CMD_FUNCTION(**ControlS71500Test, Siemens Plc S7_1512 Demo**, PRIV_SHELL_CMD_MAIN_ATTR)；更改测试指令；

  (5) 剪裁配置完成后，用过烧写器下载至矽达通中，重启后完成测试。



 