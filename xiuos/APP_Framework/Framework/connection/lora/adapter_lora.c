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
 * @file adapter_lora.c
 * @brief Implement the connection lora adapter function
 * @version 1.1
 * @author AIIT XUOS Lab
 * @date 2021.10.20
 */

#include <adapter.h>

#ifdef ADAPTER_SX1278
extern AdapterProductInfoType Sx1278Attach(struct Adapter *adapter);
#endif

#ifdef ADAPTER_E220
extern AdapterProductInfoType E220Attach(struct Adapter *adapter);
#endif

#ifdef ADAPTER_E22
extern AdapterProductInfoType E22Attach(struct Adapter *adapter);
#endif

//#define CLIENT_UPDATE_MODE
#define GATEWAY_CMD_MODE

//Client num index 1-ADAPTER_LORA_CLIENT_NUM
//#define ADAPTER_LORA_CLIENT_NUM 20

//LORA single transfer data max size 128 bytes: data format 16 bytes and user data 112 bytes
#define ADAPTER_LORA_DATA_LENGTH 112
#define ADAPTER_LORA_TRANSFER_DATA_LENGTH ADAPTER_LORA_DATA_LENGTH + 16

#define ADAPTER_LORA_DATA_HEAD            0x3C
#define ADAPTER_LORA_NET_PANID            0x0102
#define ADAPTER_LORA_DATA_TYPE_JOIN       0x0A
#define ADAPTER_LORA_DATA_TYPE_QUIT       0x0B
#define ADAPTER_LORA_DATA_TYPE_JOIN_REPLY 0x0C
#define ADAPTER_LORA_DATA_TYPE_QUIT_REPLY 0x0D
#define ADAPTER_LORA_DATA_TYPE_USERDATA   0x0E
#define ADAPTER_LORA_DATA_TYPE_CMD        0x0F
#define ADAPTER_LORA_DATA_END             0x5A

#define ADAPTER_LORA_RECEIVE_ERROR_CNT    1

#define DEFAULT_SEM_TIMEOUT     10

//need to change status if the lora client wants to quit the net when timeout or a certain event
//eg.can also use sem to trigger quit function
static int g_adapter_lora_quit_flag = 0;

enum ClientState
{
    CLIENT_DISCONNECT = 0,
    CLIENT_CONNECT,
    CLIENT_DEFAULT,
};

enum DataType
{
    LORA_CLIENT_ACCESS = 0,
    LORA_GATEWAY_REPLY,
    LORA_USER_DATA,
};

enum LoraGatewayState {
    LORA_STATE_IDLE = 0,
    LORA_JOIN_NET,
    LORA_QUIT_NET,
    LORA_RECV_DATA,
};

struct LoraGatewayParam 
{
    uint8_t gateway_id;
    uint16_t panid;
    uint8_t client_id[ADAPTER_LORA_CLIENT_NUM];
    int client_num;
    int receive_data_sem;
};

struct LoraClientParam 
{
    uint8_t client_id;
    uint16_t panid;
    enum ClientState client_state;

    uint8_t gateway_id;
};

/*LoraDataFormat:
**flame_head : 0x3C3C
**length     : sizeof(struct LoraDataFormat)
**panid      : 0x0102
**client_id  : 0x01、0x02、0x03...
**gateway_id : 0x11、0x12、0x13...
**data_type  : 0x0A(join net, client->gateway)、0x0B(join net reply, gateway->client)、
               0x0C(user data, client->gateway)、0x0D(user data cmd, gateway->client)
**data       : user data
**flame_end  : 0x5A5A
*/
struct LoraDataFormat
{
    uint16_t flame_head;
    uint16_t flame_index;
    uint32_t length;
    uint16_t panid;
    uint8_t client_id;
    uint8_t gateway_id;

    uint16_t data_type;
    uint8_t data[ADAPTER_LORA_DATA_LENGTH];

    uint16_t flame_end;
};

uint8_t lora_recv_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH];
struct LoraDataFormat client_recv_data_format[ADAPTER_LORA_CLIENT_NUM];

static sem_t gateway_recv_data_sem;
struct LoraDataFormat gateway_recv_data_format;

static int recv_error_cnt = 0;

/*******************LORA MESH FUNCTION***********************/

/**
 * @description: create CRC16 data
 * @param data input data buffer
 * @param length input data buffer minus check code
 */
static uint16_t LoraCrc16(uint8_t *data, uint16_t length)
{
    int j;
    uint16_t crc_data = 0xFFFF;
    
    while (length--) {
        crc_data ^= *data++;
        for( j = 0 ; j < 8 ; j ++) {
            if(crc_data & 0x01) /* LSB(b0)=1 */
                crc_data = crc_data >> 1 ^ 0xA001;
            else
                crc_data = crc_data >> 1;
        }
    }
    
    return crc_data;
}

/**
 * @description: CRC16 check
 * @param data input data buffer
 * @param length input data buffer minus check code
 */
static int LoraCrc16Check(uint8_t *data, uint16_t length)
{
    uint16_t crc_data;
    uint16_t input_data = (((uint16_t)data[length - 1] << 8) & 0xFF00) | ((uint16_t)data[length - 2] & 0x00FF);

    crc_data = LoraCrc16(data, length - 2);

    if (crc_data == input_data)
        return 0;
    else
        return -1;
}

/**
 * @description: Lora Gateway reply connect request to Client
 * @param adapter Lora adapter pointer
 * @param gateway_recv_data Lora Client connect data
 */
static int LoraGatewayReply(struct Adapter *adapter, struct LoraDataFormat *gateway_recv_data)
{
    int i;
    int client_join_flag = 0;
    uint16_t gateway_data_type;
    uint32_t gateway_reply_length = ADAPTER_LORA_TRANSFER_DATA_LENGTH;
    struct LoraGatewayParam *gateway = (struct LoraGatewayParam *)adapter->adapter_param;

    uint8_t gateway_reply_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH];

    memset(gateway_reply_data, 0, ADAPTER_LORA_TRANSFER_DATA_LENGTH);

    if (ADAPTER_LORA_DATA_TYPE_JOIN == gateway_recv_data->data_type) {
        
        for (i = 0; i < gateway->client_num; i ++) {
            if (gateway_recv_data->client_id == gateway->client_id[i]) {
                printf("Lora client_%d 0x%x has already join net 0x%x\n", i, gateway_recv_data->client_id, gateway->gateway_id);
                client_join_flag = 1;
                break;
            }
        }
        
        if (!client_join_flag) {
            if (gateway->client_num > ADAPTER_LORA_CLIENT_NUM) {
                printf("Lora gateway only support %u(max) client\n", ADAPTER_LORA_CLIENT_NUM);
                gateway->client_num = 0;
            }
            gateway->client_id[gateway->client_num] = gateway_recv_data->client_id;
            gateway->client_num ++;
        }

        gateway_data_type = ADAPTER_LORA_DATA_TYPE_JOIN_REPLY;

        gateway_reply_data[0] = ADAPTER_LORA_DATA_HEAD;
        gateway_reply_data[1] = ADAPTER_LORA_DATA_HEAD;
        gateway_reply_data[2] = 0;
        gateway_reply_data[3] = 0;
        gateway_reply_data[4] = (gateway_reply_length >> 24) & 0xFF;
        gateway_reply_data[5] = (gateway_reply_length >> 16) & 0xFF;
        gateway_reply_data[6] = (gateway_reply_length >> 8) & 0xFF;
        gateway_reply_data[7] = gateway_reply_length & 0xFF;
        gateway_reply_data[8] = (gateway->panid >> 8) & 0xFF;
        gateway_reply_data[9] = gateway->panid & 0xFF;
        gateway_reply_data[10] = gateway_recv_data->client_id;
        gateway_reply_data[11] = gateway->gateway_id;
        gateway_reply_data[12] = (gateway_data_type >> 8) & 0xFF;
        gateway_reply_data[13] = gateway_data_type & 0xFF;

        gateway_reply_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 2] = ADAPTER_LORA_DATA_END;
        gateway_reply_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 1] = ADAPTER_LORA_DATA_END;

        if (AdapterDeviceSend(adapter, gateway_reply_data, ADAPTER_LORA_TRANSFER_DATA_LENGTH) < 0) {
            return -1;
        }

        printf("Lora gateway 0x%x accept client 0x%x <join request>\n", gateway->gateway_id, gateway_recv_data->client_id);
    } else if (ADAPTER_LORA_DATA_TYPE_QUIT == gateway_recv_data->data_type) {
        for (i = 0; i < gateway->client_num; i ++) {
            if (gateway->client_id[i] == gateway_recv_data->client_id) {
                gateway->client_id[i] = 0;
                gateway->client_num --;
                break;
            }
        }

        gateway_data_type = ADAPTER_LORA_DATA_TYPE_QUIT_REPLY;

        gateway_reply_data[0] = ADAPTER_LORA_DATA_HEAD;
        gateway_reply_data[1] = ADAPTER_LORA_DATA_HEAD;
        gateway_reply_data[2] = 0;
        gateway_reply_data[3] = 0;
        gateway_reply_data[4] = (gateway_reply_length >> 24) & 0xFF;
        gateway_reply_data[5] = (gateway_reply_length >> 16) & 0xFF;
        gateway_reply_data[6] = (gateway_reply_length >> 8) & 0xFF;
        gateway_reply_data[7] = gateway_reply_length & 0xFF;
        gateway_reply_data[8] = (gateway->panid >> 8) & 0xFF;
        gateway_reply_data[9] = gateway->panid & 0xFF;
        gateway_reply_data[10] = gateway_recv_data->client_id;
        gateway_reply_data[11] = gateway->gateway_id;
        gateway_reply_data[12] = (gateway_data_type >> 8) & 0xFF;
        gateway_reply_data[13] = gateway_data_type & 0xFF;

        gateway_reply_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 2] = ADAPTER_LORA_DATA_END;
        gateway_reply_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 1] = ADAPTER_LORA_DATA_END;

        if (AdapterDeviceSend(adapter, gateway_reply_data, ADAPTER_LORA_TRANSFER_DATA_LENGTH) < 0) {
            return -1;
        }

        printf("Lora gateway 0x%x accept client 0x%x <quit request>\n", gateway->gateway_id, gateway_recv_data->client_id);
    }

    return 0;
}

/**
 * @description: Lora Gateway send cmd to Client
 * @param adapter Lora adapter pointer
 * @param client_id Lora Client id
 * @param cmd Lora cmd
 */
static int LoraGatewaySendCmd(struct Adapter *adapter, uint8_t client_id, uint16_t cmd)
{
    struct LoraGatewayParam *gateway = (struct LoraGatewayParam *)adapter->adapter_param;
    uint16_t gateway_cmd_type;
    uint32_t gateway_cmd_length = ADAPTER_LORA_TRANSFER_DATA_LENGTH;
    uint8_t gateway_cmd_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH];

    memset(gateway_cmd_data, 0, sizeof(struct LoraDataFormat));

    gateway_cmd_type = cmd;

    gateway_cmd_data[0] = ADAPTER_LORA_DATA_HEAD;
    gateway_cmd_data[1] = ADAPTER_LORA_DATA_HEAD;
    gateway_cmd_data[2] = 0;
    gateway_cmd_data[3] = 0;
    gateway_cmd_data[4] = (gateway_cmd_length >> 24) & 0xFF;
    gateway_cmd_data[5] = (gateway_cmd_length >> 16) & 0xFF;
    gateway_cmd_data[6] = (gateway_cmd_length >> 8) & 0xFF;
    gateway_cmd_data[7] = gateway_cmd_length & 0xFF;
    gateway_cmd_data[8] = (gateway->panid >> 8) & 0xFF;
    gateway_cmd_data[9] = gateway->panid & 0xFF;
    gateway_cmd_data[10] = client_id;
    gateway_cmd_data[11] = gateway->gateway_id;
    gateway_cmd_data[12] = (gateway_cmd_type >> 8) & 0xFF;
    gateway_cmd_data[13] = gateway_cmd_type & 0xFF;

    gateway_cmd_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 2] = ADAPTER_LORA_DATA_END;
    gateway_cmd_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 1] = ADAPTER_LORA_DATA_END;
    
    if (AdapterDeviceSend(adapter, gateway_cmd_data, ADAPTER_LORA_TRANSFER_DATA_LENGTH) < 0) {
        return -1;
    }

    return 0;
}

/**
 * @description: Lora Gateway handle the user data from the client
 * @param adapter Lora adapter pointer
 * @param gateway_recv_data Lora Client user data
 */
static int LoraGatewayHandleData(struct Adapter *adapter, struct LoraDataFormat *gateway_recv_data)
{    
    /*User needs to handle client data depends on the requirement*/
    printf("Lora Gateway receive Client %d data:\n", gateway_recv_data->client_id);
    printf("%s\n", gateway_recv_data->data);

    PrivSemaphoreAbandon(&gateway_recv_data_sem);
    return 0;
}

/**
 * @description: Lora Client update status after join the net
 * @param adapter Lora adapter pointer
 * @param client_recv_data Lora Client recv data from Lora Gateway
 */
static int LoraClientUpdate(struct Adapter *adapter, struct LoraDataFormat *client_recv_data)
{
    struct LoraClientParam *client = (struct LoraClientParam *)adapter->adapter_param;

    if (ADAPTER_LORA_DATA_TYPE_JOIN_REPLY == client_recv_data->data_type) {
        client->gateway_id = client_recv_data->gateway_id;
        client->panid = client_recv_data->panid;
        client->client_state = CLIENT_CONNECT;
        printf("LoraClientUpdate client join panid 0x%x successfully\n", client->panid);
    } else if (ADAPTER_LORA_DATA_TYPE_QUIT_REPLY == client_recv_data->data_type) {
        client->gateway_id = 0;
        client->panid = 0;
        client->client_state = CLIENT_DISCONNECT;
        printf("LoraClientUpdate client quit panid 0x%x successfully\n", client->panid);
    }

    return 0;
}

/**
 * @description: Lora Client send the user data to the gateway
 * @param adapter Lora adapter pointer
 * @param send_buf Lora Client send user data buf
 * @param length user data length (max size is ADAPTER_LORA_DATA_LENGTH)
 * @param send_index user data index, max size ADAPTER_LORA_DATA_LENGTH single index
 */
static int LoraClientSendData(struct Adapter *adapter, void *send_buf, int length, uint16_t send_index)
{
    struct LoraClientParam *client = (struct LoraClientParam *)adapter->adapter_param;

    if (client->client_state != CLIENT_CONNECT) {
        printf("Lora client %d do not connect to Lora gateway\n", client->client_id);
        return 0;
    }

    if (length > ADAPTER_LORA_DATA_LENGTH) {
        printf("Lora client %d send data %d larger than max %d \n", client->client_id, length, ADAPTER_LORA_DATA_LENGTH);
        return 0;
    }

    uint16_t client_user_type;
    uint32_t client_user_length = ADAPTER_LORA_TRANSFER_DATA_LENGTH;
    uint8_t client_user_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH];

    memset(client_user_data, 0, ADAPTER_LORA_TRANSFER_DATA_LENGTH);

    client_user_type = ADAPTER_LORA_DATA_TYPE_USERDATA;

    client_user_data[0] = ADAPTER_LORA_DATA_HEAD;
    client_user_data[1] = ADAPTER_LORA_DATA_HEAD;
    client_user_data[2] = (send_index >> 8) & 0xFF;
    client_user_data[3] = send_index & 0xFF;
    client_user_data[4] = (client_user_length >> 24) & 0xFF;
    client_user_data[5] = (client_user_length >> 16) & 0xFF;
    client_user_data[6] = (client_user_length >> 8) & 0xFF;
    client_user_data[7] = client_user_length & 0xFF;
    client_user_data[8] = (client->panid >> 8) & 0xFF;
    client_user_data[9] = client->panid & 0xFF;
    client_user_data[10] = client->client_id;
    client_user_data[11] = client->gateway_id;
    client_user_data[12] = (client_user_type >> 8) & 0xFF;
    client_user_data[13] = client_user_type & 0xFF;

    memcpy((uint8_t *)(client_user_data + 14), (uint8_t *)send_buf, length);

    client_user_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 2] = ADAPTER_LORA_DATA_END;
    client_user_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 1] = ADAPTER_LORA_DATA_END;
    
    if (AdapterDeviceSend(adapter, client_user_data, ADAPTER_LORA_TRANSFER_DATA_LENGTH) < 0) {
        return -1;
    }

    return 0;
}

/**
 * @description: Lora Gateway receive data analyzing
 * @param adapter Lora adapter pointer
 */
static int LoraGateWayDataAnalyze(struct Adapter *adapter)
{
    int ret = 0;

    if (ADAPTER_LORA_NET_PANID == gateway_recv_data_format.panid) {

        printf("%s: gateway_recv_data\n", __func__);
        printf("head 0x%x length %d panid 0x%x data_type 0x%x client_id 0x%x gateway_id 0x%x end 0x%x\n",
            gateway_recv_data_format.flame_head, gateway_recv_data_format.length, gateway_recv_data_format.panid, gateway_recv_data_format.data_type,
            gateway_recv_data_format.client_id, gateway_recv_data_format.gateway_id, gateway_recv_data_format.flame_end);

        switch (gateway_recv_data_format.data_type)
        {
        case ADAPTER_LORA_DATA_TYPE_JOIN : 
        case ADAPTER_LORA_DATA_TYPE_QUIT :
            ret = LoraGatewayReply(adapter, &gateway_recv_data_format);
            break;
        case ADAPTER_LORA_DATA_TYPE_USERDATA :
            ret = LoraGatewayHandleData(adapter, &gateway_recv_data_format);
            break;
        default:
            break;
        }
    } else {
        ret = -1;
    }

    return ret;
}

/**
 * @description: Lora Client receive data analyzing
 * @param adapter Lora adapter pointer
 * @param send_buf Lora Client send user data buf
 * @param length user data length (max size is ADAPTER_LORA_DATA_LENGTH)
 * @param index user data index, max size ADAPTER_LORA_DATA_LENGTH single index
 */
static int LoraClientDataAnalyze(struct Adapter *adapter, void *send_buf, int length, uint16_t index)
{
    int ret = 0;
    uint8_t client_id = adapter->net_role_id;

    struct timespec abstime;
    abstime.tv_sec = DEFAULT_SEM_TIMEOUT;

    ret = PrivSemaphoreObtainWait(&adapter->sem, &abstime);
    if (0 == ret) {
        //only handle this client_id information from gateway
        if ((client_recv_data_format[client_id - 1].client_id == adapter->net_role_id) && 
            (ADAPTER_LORA_NET_PANID == client_recv_data_format[client_id - 1].panid)) {

            printf("%s: 0x%x client_recv_data\n", __func__, client_recv_data_format[client_id - 1].client_id);
            printf("head 0x%x length %d panid 0x%x data_type 0x%x client_id 0x%x gateway_id 0x%x end 0x%x\n",
                client_recv_data_format[client_id - 1].flame_head, client_recv_data_format[client_id - 1].length, client_recv_data_format[client_id - 1].panid, client_recv_data_format[client_id - 1].data_type,
                client_recv_data_format[client_id - 1].client_id, client_recv_data_format[client_id - 1].gateway_id, client_recv_data_format[client_id - 1].flame_end);

            switch (client_recv_data_format[client_id - 1].data_type)
            {
            case ADAPTER_LORA_DATA_TYPE_JOIN_REPLY : 
            case ADAPTER_LORA_DATA_TYPE_QUIT_REPLY :
                ret = LoraClientUpdate(adapter, &client_recv_data_format[client_id - 1]);
                break;
            case ADAPTER_LORA_DATA_TYPE_CMD :
                if (send_buf) {
                    ret = LoraClientSendData(adapter, send_buf, length, index);
                }
                break;
            default:
                break;
            }

            //Client operation done
            memset(&client_recv_data_format[client_id - 1], 0 , sizeof(struct LoraDataFormat));
        }
    }

    return ret;
}

/**
 * @description: Lora Client join net function
 * @param adapter Lora adapter pointer
 * @param panid Lora net panid
 */
static int LoraClientJoinNet(struct Adapter *adapter, unsigned short panid)
{
    struct AdapterData priv_lora_net;

    uint16_t client_join_type;
    uint32_t client_join_length = ADAPTER_LORA_TRANSFER_DATA_LENGTH;
    uint8_t client_join_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH];

    memset(client_join_data, 0, ADAPTER_LORA_TRANSFER_DATA_LENGTH);

    client_join_type = ADAPTER_LORA_DATA_TYPE_JOIN;

    client_join_data[0] = ADAPTER_LORA_DATA_HEAD;
    client_join_data[1] = ADAPTER_LORA_DATA_HEAD;
    client_join_data[2] = 0;
    client_join_data[3] = 0;
    client_join_data[4] = (client_join_length >> 24) & 0xFF;
    client_join_data[5] = (client_join_length >> 16) & 0xFF;
    client_join_data[6] = (client_join_length >> 8) & 0xFF;
    client_join_data[7] = client_join_length & 0xFF;
    client_join_data[8] = (panid >> 8) & 0xFF;
    client_join_data[9] = panid & 0xFF;
    client_join_data[10] = adapter->net_role_id & 0xFF;
    client_join_data[11] = 0;
    client_join_data[12] = (client_join_type >> 8) & 0xFF;
    client_join_data[13] = client_join_type & 0xFF;

    client_join_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 2] = ADAPTER_LORA_DATA_END;
    client_join_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 1] = ADAPTER_LORA_DATA_END;

    priv_lora_net.len = ADAPTER_LORA_TRANSFER_DATA_LENGTH;
    priv_lora_net.buffer = client_join_data;

    if (AdapterDeviceJoin(adapter, (uint8_t *)&priv_lora_net) < 0) {
        return -1;
    }

    printf("%s:client_join_data panid 0x%x client_id 0x%x\n", __func__, panid, adapter->net_role_id);

    return 0;
}

/**
 * @description: Lora Client quit net function
 * @param adapter Lora adapter pointer
 * @param panid Lora net panid
 */
static int LoraClientQuitNet(struct Adapter *adapter, unsigned short panid)
{
    struct AdapterData priv_lora_net;

    uint16_t client_quit_type;
    uint32_t client_quit_length = ADAPTER_LORA_TRANSFER_DATA_LENGTH;
    uint8_t client_quit_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH];

    memset(client_quit_data, 0, ADAPTER_LORA_TRANSFER_DATA_LENGTH);

    client_quit_type = ADAPTER_LORA_DATA_TYPE_QUIT;

    client_quit_data[0] = ADAPTER_LORA_DATA_HEAD;
    client_quit_data[1] = ADAPTER_LORA_DATA_HEAD;
    client_quit_data[2] = 0;
    client_quit_data[3] = 0;
    client_quit_data[4] = (client_quit_length >> 24) & 0xFF;
    client_quit_data[5] = (client_quit_length >> 16) & 0xFF;
    client_quit_data[6] = (client_quit_length >> 8) & 0xFF;
    client_quit_data[7] = client_quit_length & 0xFF;
    client_quit_data[8] = (panid >> 8) & 0xFF;
    client_quit_data[9] = panid & 0xFF;
    client_quit_data[10] = adapter->net_role_id & 0xFF;
    client_quit_data[11] = 0;
    client_quit_data[12] = (client_quit_type >> 8) & 0xFF;
    client_quit_data[13] = client_quit_type & 0xFF;

    client_quit_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 2] = ADAPTER_LORA_DATA_END;
    client_quit_data[ADAPTER_LORA_TRANSFER_DATA_LENGTH - 1] = ADAPTER_LORA_DATA_END;

    priv_lora_net.len = ADAPTER_LORA_TRANSFER_DATA_LENGTH;
    priv_lora_net.buffer = client_quit_data;

    if (AdapterDeviceDisconnect(adapter, (uint8_t *)&priv_lora_net) < 0) {
        return -1;
    }

    printf("%s:client_quit_data panid 0x%x client_id 0x%x\n", __func__, panid, adapter->net_role_id);

    return 0;
}

/**
 * @description: Lora receive data check
 * @param adapter Lora adapter pointer
 * @param recv_data receive data buffer
 * @param length receive data length
 */
static int LoraReceiveDataCheck(struct Adapter *adapter, uint8_t *recv_data, uint16_t length)
{
    int ret;
    uint8_t client_id;

    if ((ADAPTER_LORA_DATA_HEAD == recv_data[0]) && (ADAPTER_LORA_DATA_HEAD == recv_data[1]) &&
        (ADAPTER_LORA_DATA_END == recv_data[length - 1]) && (ADAPTER_LORA_DATA_END == recv_data[length - 2])) {

#ifdef AS_LORA_GATEWAY_ROLE
        gateway_recv_data_format.flame_head = ((recv_data[0] << 8) & 0xFF00) | recv_data[1];
        gateway_recv_data_format.flame_index = ((recv_data[2] << 8) & 0xFF00) | recv_data[3];
        gateway_recv_data_format.length = ((recv_data[4] << 24) & 0xFF000000) | ((recv_data[5] << 16) & 0xFF0000) |
                                    ((recv_data[6] << 8) & 0xFF00) | (recv_data[7] & 0xFF);
        gateway_recv_data_format.panid = ((recv_data[8] << 8) & 0xFF00) | recv_data[9];
        gateway_recv_data_format.client_id = recv_data[10];
        gateway_recv_data_format.gateway_id = recv_data[11];
        gateway_recv_data_format.data_type = ((recv_data[12] << 8) & 0xFF00) | recv_data[13];

        gateway_recv_data_format.flame_end = ((recv_data[length - 2] << 8) & 0xFF00) | recv_data[length - 1];

        memcpy(gateway_recv_data_format.data, (uint8_t *)(recv_data + 14), ADAPTER_LORA_DATA_LENGTH);

        ret = LoraGateWayDataAnalyze(adapter);

        return ret;
#else
        client_id = recv_data[10];

        if (client_id == adapter->net_role_id) {
            printf("client_id 0x%x recv data\n", client_id);
            client_recv_data_format[client_id - 1].flame_head = ((recv_data[0] << 8) & 0xFF00) | recv_data[1];
            client_recv_data_format[client_id - 1].flame_index = ((recv_data[2] << 8) & 0xFF00) | recv_data[3];
            client_recv_data_format[client_id - 1].length = ((recv_data[4] << 24) & 0xFF000000) | ((recv_data[5] << 16) & 0xFF0000) |
                                        ((recv_data[6] << 8) & 0xFF00) | (recv_data[7] & 0xFF);
            client_recv_data_format[client_id - 1].panid = ((recv_data[8] << 8) & 0xFF00) | recv_data[9];
            client_recv_data_format[client_id - 1].client_id = recv_data[10];
            client_recv_data_format[client_id - 1].gateway_id = recv_data[11];
            client_recv_data_format[client_id - 1].data_type = ((recv_data[12] << 8) & 0xFF00) | recv_data[13];

            client_recv_data_format[client_id - 1].flame_end = ((recv_data[length - 2] << 8) & 0xFF00) | recv_data[length - 1];

            memcpy(client_recv_data_format[client_id - 1].data, (uint8_t *)(recv_data + 14), ADAPTER_LORA_DATA_LENGTH);
            return 0;
        }
#endif
    }

    return -1;
}

/**
 * @description: Lora data receive task
 * @param parameter - Lora adapter pointer
 */
static void *LoraReceiveTask(void *parameter)
{
    int ret = 0;
    struct Adapter *lora_adapter = (struct Adapter *)parameter;

    while (1) {
        memset(lora_recv_data, 0, ADAPTER_LORA_TRANSFER_DATA_LENGTH);
        
        ret = AdapterDeviceRecv(lora_adapter, lora_recv_data, ADAPTER_LORA_TRANSFER_DATA_LENGTH);
        if (ret <= 0) {
            printf("AdapterDeviceRecv error.Just return\n");
            recv_error_cnt++;
            if (recv_error_cnt > ADAPTER_LORA_RECEIVE_ERROR_CNT) {
                recv_error_cnt = 0;
#ifdef AS_LORA_GATEWAY_ROLE
                PrivSemaphoreAbandon(&gateway_recv_data_sem);
#endif
            }
            continue;
        }

        ret = LoraReceiveDataCheck(lora_adapter, lora_recv_data, ADAPTER_LORA_TRANSFER_DATA_LENGTH);
        if (ret < 0) {
            continue;
        }

        PrivSemaphoreAbandon(&lora_adapter->sem);
    }

    return 0;
}

/**
 * @description: Lora Gateway Process function
 * @param lora_adapter Lora adapter pointer
 * @param gateway Lora gateway pointer
 */
void LoraGatewayProcess(struct Adapter *lora_adapter, struct LoraGatewayParam *gateway)
{
    int i, ret = 0;
    struct timespec abstime;
    abstime.tv_sec = DEFAULT_SEM_TIMEOUT;

#ifdef GATEWAY_CMD_MODE
    for (i = 0; i < gateway->client_num; i ++) {
        if (gateway->client_id[i]) {
            printf("LoraGatewayProcess send to client %d for data\n", gateway->client_id[i]);
            ret = LoraGatewaySendCmd(lora_adapter, gateway->client_id[i], ADAPTER_LORA_DATA_TYPE_CMD);
            if (ret < 0) {
                printf("LoraGatewaySendCmd client ID %d error\n", gateway->client_id[i]);
                continue;
            }

            ret = PrivSemaphoreObtainWait(&gateway_recv_data_sem, &abstime);
            if (0 == ret) {
                printf("LoraGatewayProcess receive client %d data done\n", gateway->client_id[i]);
            }
        }
    }
#endif

    return;
}

/**
 * @description: Lora Gateway task
 * @param parameter - Lora adapter pointer
 */
static void *LoraGatewayTask(void *parameter)
{
    int i;
    int ret = 0;
    struct Adapter *lora_adapter = (struct Adapter *)parameter;
    struct LoraGatewayParam *gateway = (struct LoraGatewayParam *)lora_adapter->adapter_param;

    memset(&gateway_recv_data_format, 0, sizeof(struct LoraDataFormat));

    gateway->client_num = ADAPTER_LORA_CLIENT_NUM;
    for (i = 0; i < ADAPTER_LORA_CLIENT_NUM;i ++) {
        gateway->client_id[i] = i + 1;
    }   
    
    while (1) {
        LoraGatewayProcess(lora_adapter, gateway);
    }

    return 0;
}

/**
 * @description: Lora Client data upload task
 * @param parameter - Lora adapter pointer
 */
static void *LoraClientDataTask(void *parameter)
{
    int i, ret = 0;
    struct Adapter *lora_adapter = (struct Adapter *)parameter;
    struct LoraClientParam *client = (struct LoraClientParam *)lora_adapter->adapter_param;

    for (i = 0; i < ADAPTER_LORA_CLIENT_NUM; i ++) {
        memset(&client_recv_data_format[i], 0, sizeof(struct LoraDataFormat));
    }

    client->gateway_id = 0xFF;
    client->panid = ADAPTER_LORA_NET_PANID;
    client->client_state = CLIENT_CONNECT;

    //set lora_send_buf for test
    uint8_t lora_send_buf[ADAPTER_LORA_DATA_LENGTH];
    memset(lora_send_buf, 0, ADAPTER_LORA_DATA_LENGTH);
    sprintf(lora_send_buf, "Lora client %d adapter test\n", client->client_id);

    while (1) {
        //Condition 1: Gateway send user_data cmd, client send user_data after receiving user_data cmd
#ifdef GATEWAY_CMD_MODE
        ret = LoraClientDataAnalyze(lora_adapter, (void *)lora_send_buf, strlen(lora_send_buf), 0);
        if (ret < 0) {
            printf("LoraClientDataAnalyze error, wait for next data cmd\n");
            continue;
        }
#endif
        //Condition 2: client send user_data automatically
#ifdef CLIENT_UPDATE_MODE
        if (lora_send_buf) {
            PrivTaskDelay(2000);
            printf("LoraClientSendData\n");
            LoraClientSendData(lora_adapter, (void *)lora_send_buf, strlen(lora_send_buf), 0);
        }
#endif
    }

    return 0;
}

/*******************LORA ADAPTER FUNCTION********************/

static int AdapterLoraRegister(struct Adapter *adapter)
{
    int ret = 0;
    struct LoraGatewayParam *lora_gateway;
    struct LoraClientParam *lora_client;

    strncpy(adapter->name, ADAPTER_LORA_NAME, NAME_NUM_MAX);

    adapter->net_protocol = PRIVATE_PROTOCOL;

#ifdef AS_LORA_GATEWAY_ROLE
    lora_gateway = PrivMalloc(sizeof(struct LoraGatewayParam));
    if (!lora_gateway) {
        PrivFree(lora_gateway);
        return -1;
    }

    memset(lora_gateway, 0, sizeof(struct LoraGatewayParam));

    lora_gateway->gateway_id = ADAPTER_LORA_NET_ROLE_ID;
    lora_gateway->panid = ADAPTER_LORA_NET_PANID;

    adapter->net_role = GATEWAY;
    adapter->adapter_param = (void *)lora_gateway;
#else //AS_LORA_CLIENT_ROLE
    lora_client = PrivMalloc(sizeof(struct LoraClientParam));
    if (!lora_client) {
        PrivFree(lora_client);
        return -1;
    }

    memset(lora_client, 0, sizeof(struct LoraClientParam));

    lora_client->client_id = ADAPTER_LORA_NET_ROLE_ID;
    lora_client->client_state = CLIENT_DISCONNECT;
    lora_client->panid = ADAPTER_LORA_NET_PANID;

    adapter->net_role = CLIENT;
    adapter->adapter_param = (void *)lora_client;
#endif

    adapter->net_role_id = ADAPTER_LORA_NET_ROLE_ID;

    adapter->adapter_status = UNREGISTERED;

    ret = AdapterDeviceRegister(adapter);
    if (ret < 0) {
        printf("Adapter4G register error\n");
        if (lora_gateway)
            PrivFree(lora_gateway);
        if (lora_client)
            PrivFree(lora_client);

        return -1;
    }

    return ret;
}

int AdapterLoraInit(void)
{
    int ret = 0;

    struct Adapter *adapter = PrivMalloc(sizeof(struct Adapter));
    if (!adapter) {
        PrivFree(adapter);
        return -1;
    }

    memset(adapter, 0, sizeof(struct Adapter));

    ret = AdapterLoraRegister(adapter);
    if (ret < 0) {
        printf("AdapterLoraInit register lora adapter error\n");
        PrivFree(adapter);
        return -1;
    }

#ifdef ADAPTER_SX1278
    AdapterProductInfoType product_info = Sx1278Attach(adapter);
    if (!product_info) {
        printf("AdapterLoraInit sx1278 attach error\n");
        PrivFree(adapter);
        return -1;
    }

    adapter->product_info_flag = 1;
    adapter->info = product_info;
    adapter->done = product_info->model_done;

#endif

#ifdef ADAPTER_E220
    AdapterProductInfoType product_info = E220Attach(adapter);
    if (!product_info) {
        printf("AdapterLoraInit e220 attach error\n");
        PrivFree(adapter);
        return -1;
    }

    adapter->product_info_flag = 1;
    adapter->info = product_info;
    adapter->done = product_info->model_done;
#endif

#ifdef ADAPTER_E22
    AdapterProductInfoType product_info = E22Attach(adapter);
    if (!product_info) {
        printf("AdapterLoraInit e22 attach error\n");
        PrivFree(adapter);
        return -1;
    }

    adapter->product_info_flag = 1;
    adapter->info = product_info;
    adapter->done = product_info->model_done;
#endif

    PrivSemaphoreCreate(&adapter->sem, 0, 0);

    PrivSemaphoreCreate(&gateway_recv_data_sem, 0, 0);

    PrivMutexCreate(&adapter->lock, 0);

    return ret;
}

/******************Lora TEST*********************/
static pthread_t lora_recv_data_task;

#ifdef AS_LORA_GATEWAY_ROLE
static pthread_t lora_gateway_task;
#else //AS_LORA_CLIENT_ROLE
static pthread_t lora_client_data_task;
#endif

int AdapterLoraTest(void)
{
    struct Adapter *adapter =  AdapterDeviceFindByName(ADAPTER_LORA_NAME);    
	
    AdapterDeviceOpen(adapter);

    //create lora gateway task
#ifdef AS_LORA_GATEWAY_ROLE
#ifdef ADD_NUTTX_FEATURES
    pthread_attr_t lora_gateway_attr = PTHREAD_ATTR_INITIALIZER;
    lora_gateway_attr.priority = 20;
    lora_gateway_attr.stacksize = 2048;
#else
    pthread_attr_t lora_gateway_attr;
    lora_gateway_attr.schedparam.sched_priority = 20;
    lora_gateway_attr.stacksize = 2048;
#endif

    PrivTaskCreate(&lora_recv_data_task, &lora_gateway_attr, &LoraReceiveTask, (void *)adapter);
    PrivTaskStartup(&lora_recv_data_task);

#ifdef ADD_NUTTX_FEATURES
    lora_gateway_attr.priority = 19;
#else   
    lora_gateway_attr.schedparam.sched_priority = 19;
#endif

    PrivTaskCreate(&lora_gateway_task, &lora_gateway_attr, &LoraGatewayTask, (void *)adapter);
    PrivTaskStartup(&lora_gateway_task);

#else //AS_LORA_CLIENT_ROLE
#ifdef ADD_NUTTX_FEATURES
    pthread_attr_t lora_client_attr = PTHREAD_ATTR_INITIALIZER;
    lora_client_attr.priority = 20;
    lora_client_attr.stacksize = 2048;
#else
    pthread_attr_t lora_client_attr;
    lora_client_attr.schedparam.sched_priority = 20;
    lora_client_attr.stacksize = 2048;
#endif
    PrivTaskCreate(&lora_recv_data_task, &lora_client_attr, &LoraReceiveTask, (void *)adapter);
    PrivTaskStartup(&lora_recv_data_task);

#ifdef ADD_NUTTX_FEATURES
    lora_client_attr.priority = 19;
#else   
    lora_client_attr.schedparam.sched_priority = 19;
#endif

    //create lora client task
    PrivTaskCreate(&lora_client_data_task, &lora_client_attr, &LoraClientDataTask, (void *)adapter);
    PrivTaskStartup(&lora_client_data_task);

#endif

    return 0;
}
PRIV_SHELL_CMD_FUNCTION(AdapterLoraTest, a lora test sample, PRIV_SHELL_CMD_MAIN_ATTR);
