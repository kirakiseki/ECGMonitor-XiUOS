/*
* Copyright (c) 2022 AIIT XUOS Lab
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
 * @file control.h
 * @brief DEFINE code for control framework app
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-09-27
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <transform.h>
#include <list.h>


#ifdef __cplusplus
extern "C" {
#endif



struct ControlProtocol;
typedef struct ControlProtocol *ControlProtocolType;

struct ControlDone 
{
    int (*_open)(struct ControlProtocol *control_protocol);
    int (*_close)(struct ControlProtocol *control_protocol);
    int (*_read)(struct ControlProtocol *control_protocol, void *buf, size_t len);
    int (*_write)(struct ControlProtocol *control_protocol, const void *buf, size_t len);
    int (*_ioctl)(struct ControlProtocol *control_protocol, int cmd, void *args);
};

typedef enum
{
    PROTOCOL_S7 = 1,
    PROTOCOL_MODBUS_TCP,
    PROTOCOL_MODBUS_UART,
    PROTOCOL_OPC_UA,
    PROTOCOL_FINS,
    PROTOCOL_MELSEC_1E,
    PROTOCOL_MELSEC_3E_Q_L,
    PROTOCOL_MELSEC_3E_IQ_R,
    PROTOCOL_MELSEC_1C,
    PROTOCOL_MELSEC_3C,
    PROTOCOL_END
}ProtocolType;

typedef enum
{
    CONTROL_INIT,
    CONTROL_REGISTERED,
    CONTROL_UNREGISTERED,
}ProtocolStatus;

struct ControlDevice
{
    char *dev_name;
    int status;

    //to do
    void *dev_done;
};

struct ControlProtocol
{
    char *name;
    ProtocolType protocol_type;
    ProtocolStatus protocol_status;

    struct ControlRecipe *recipe;
    struct ControlDone *done;
    struct ControlDevice *device;
    
    void *args;

    sem_t sem;
    pthread_mutex_t lock;
};

/*Control Framework Protocol Init*/
int ControlFrameworkInit(void);

/*Control Framework Find certain Protocol*/
ControlProtocolType ControlProtocolFind(void);

/*Control Framework Protocol Open*/
int ControlProtocolOpen(struct ControlProtocol *control_protocol);

/*Control Framework Protocol Close*/
int ControlProtocolClose(struct ControlProtocol *control_protocol);

/*Control Framework Protocol Read*/
int ControlProtocolRead(struct ControlProtocol *control_protocol, void *buf, size_t len);

/*Control Framework Protocol Write*/
int ControlProtocolWrite(struct ControlProtocol *control_protocol, const void *buf, size_t len);

/*Control Framework Protocol Ioctl*/
int ControlProtocolIoctl(struct ControlProtocol *control_protocol, int cmd, void *args);

/*Control Framework new certain Protocol*/
ControlProtocolType control_protocol;

#ifdef __cplusplus
}
#endif

#endif

