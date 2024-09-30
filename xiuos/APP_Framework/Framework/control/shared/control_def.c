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
 * @file control_def.c
 * @brief code for control framework
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-10-9
 */

#include <control_def.h>
#include <control_io.h>

/*using cirtular area to receive data*/
#define PLC_DATA_LENGTH 1024
struct CircularAreaApp *g_circular_area;
static pthread_t recv_plc_data_task;

/*extern function*/
extern void *ReceivePlcDataTask(void *parameter);

#ifdef CONTROL_PROTOCOL_FINS
extern int FinsProtocolInit(struct ControlRecipe *p_recipe);
#endif

#ifdef CONTROL_PROTOCOL_MELSEC
extern int MelsecProtocolInit(struct ControlRecipe *p_recipe);
#endif

#ifdef CONTROL_PROTOCOL_MODBUS_TCP
extern int ModbusTcpProtocolInit(struct ControlRecipe *p_recipe);
#endif

#ifdef CONTROL_PROTOCOL_MODBUS_UART
extern int ModbusUartProtocolInit(struct ControlRecipe *p_recipe);
#endif

#ifdef CONTROL_PROTOCOL_S7
extern int S7ProtocolInit(struct ControlRecipe *p_recipe);
#endif

/*
CONTROL FRAMEWORK READ DATA FORMAT:
|  HEAD |device_id|read data length|read item count|         data         |
|2 Bytes| 2 Bytes |    2 Bytes     |    2 Bytes    |read data length Bytes|
*/
#define CONTROL_DATA_HEAD_LENGTH    8
#define CONTROL_DATA_HEAD_1      0xAA
#define CONTROL_DATA_HEAD_2      0xBB

typedef int (*ControlProtocolInitFunc)(struct ControlRecipe *p_recipe);

struct ControlProtocolInitParam
{
	int protocol_type;
	const ControlProtocolInitFunc fn;
};

static struct ControlProtocolInitParam protocol_init[] = 
{
#ifdef CONTROL_PROTOCOL_FINS
	{ PROTOCOL_FINS, FinsProtocolInit },
#endif
#ifdef CONTROL_PROTOCOL_MELSEC
	{ PROTOCOL_MELSEC_1E, MelsecProtocolInit },
    { PROTOCOL_MELSEC_3E_Q_L, MelsecProtocolInit },
    { PROTOCOL_MELSEC_3E_IQ_R, MelsecProtocolInit },
    { PROTOCOL_MELSEC_1C, MelsecProtocolInit },
    { PROTOCOL_MELSEC_3C, MelsecProtocolInit },
#endif
#ifdef CONTROL_PROTOCOL_MODBUS_TCP
    { PROTOCOL_MODBUS_TCP, ModbusTcpProtocolInit },
#endif
#ifdef CONTROL_PROTOCOL_MODBUS_UART
    { PROTOCOL_MODBUS_UART, ModbusUartProtocolInit },
#endif
#ifdef CONTROL_PROTOCOL_S7
    { PROTOCOL_S7, S7ProtocolInit },
#endif

	{ PROTOCOL_END, NULL },
};

/**
 * @description: Control Framework Sub_Protocol Desc Init
 * @param p_recipe - Control recipe pointer
 * @param sub_protocol_desc - sub_protocol desc
 * @return success : 0 error : -1
 */
static int ControlProtocolInitDesc(struct ControlRecipe *p_recipe, struct ControlProtocolInitParam sub_protocol_desc[])
{
	int i = 0;
	int ret = 0;
	for( i = 0; sub_protocol_desc[i].fn != NULL; i++ ) {
		if (p_recipe->protocol_type == sub_protocol_desc[i].protocol_type) {
			ret = sub_protocol_desc[i].fn(p_recipe);
            printf("%s initialize %d %s\n", __func__, sub_protocol_desc[i].protocol_type, ret == 0 ? "success" : "failed");
			break;
		}
	}
	return ret;
}

/**
 * @description: Control Framework Protocol Data Header Format
 * @param p_recipe - Control recipe pointer
 * @return 
 */
static void FormatDataHeader(struct ControlRecipe *p_recipe)
{
    uint16_t plc_read_data_length = CONTROL_DATA_HEAD_LENGTH + p_recipe->total_data_length;//Head length is CONTROL_DATA_HEAD_LENGTH
    uint8_t *data = p_recipe->protocol_data.data;

    data[0] = CONTROL_DATA_HEAD_1;
    data[1] = CONTROL_DATA_HEAD_2;
    data[2] = (uint8_t)(p_recipe->device_id >> 8);
    data[3] = (uint8_t)p_recipe->device_id;
    data[4] = (uint8_t)(plc_read_data_length >> 8);
    data[5] = (uint8_t)plc_read_data_length;
    data[6] = (uint8_t)(p_recipe->read_item_count >> 8);
    data[7] = (uint8_t)p_recipe->read_item_count;
}

/**
 * @description: Get Recipe Total Data Length
 * @param read_item_list_json - read_item_list_json pointer
 * @return success : total_data_length error : 0
 */
static uint16_t GetRecipeTotalDataLength(cJSON* read_item_list_json)
{
    uint16_t read_item_count = cJSON_GetArraySize(read_item_list_json);
    uint16_t total_data_length = 0;
    for (uint16_t read_item_index = 0; read_item_index < read_item_count; read_item_index++) {
        cJSON* read_item_json = cJSON_GetArrayItem(read_item_list_json, read_item_index);
        UniformValueType value_type = cJSON_GetObjectItem(read_item_json, "value_type")->valueint;
        total_data_length += GetValueTypeMemorySize(value_type);
    }
    return total_data_length;
}

/**
 * @description: Control Framework Basic Serial Configure
 * @param p_recipe - Control recipe pointer
 * @param p_recipe_file_json - p_recipe_file_json pointer
 * @return
 */
static void ControlBasicSerialConfig(struct ControlRecipe *p_recipe, cJSON *p_recipe_file_json)
{
    cJSON *p_serial_config_json = cJSON_GetObjectItem(p_recipe_file_json, "serial_config");
    p_recipe->serial_config.station = cJSON_GetObjectItem(p_serial_config_json, "station")->valueint;
    p_recipe->serial_config.baud_rate = cJSON_GetObjectItem(p_serial_config_json, "baud_rate")->valueint;
    p_recipe->serial_config.data_bits = cJSON_GetObjectItem(p_serial_config_json, "data_bits")->valueint;
    p_recipe->serial_config.stop_bits = cJSON_GetObjectItem(p_serial_config_json, "stop_bits")->valueint;
    p_recipe->serial_config.check_mode = cJSON_GetObjectItem(p_serial_config_json, "check_mode")->valueint;
    printf("Serial_config:station: %d baud_rate: %d, data_bits: %d, stop_bits: %d, check_mode is %d\n",
        p_recipe->serial_config.station, p_recipe->serial_config.baud_rate, p_recipe->serial_config.data_bits, p_recipe->serial_config.stop_bits, p_recipe->serial_config.check_mode);
}

/**
 * @description: Control Framework Basic Socket Configure
 * @param p_recipe - Control recipe pointer
 * @param p_recipe_file_json - p_recipe_file_json pointer
 * @return
 */
static void ControlBasicSocketConfig(struct ControlRecipe *p_recipe, cJSON *p_recipe_file_json)
{
    cJSON *p_socket_address_json = cJSON_GetObjectItem(p_recipe_file_json, "socket_config");
    char *plc_ip_string = cJSON_GetObjectItem(p_socket_address_json, "plc_ip")->valuestring;
    sscanf(plc_ip_string, "%d.%d.%d.%d",
        p_recipe->socket_config.plc_ip,
        p_recipe->socket_config.plc_ip + 1,
        p_recipe->socket_config.plc_ip + 2,
        p_recipe->socket_config.plc_ip + 3);

    char *local_ip_string = cJSON_GetObjectItem(p_socket_address_json, "local_ip")->valuestring;
    sscanf(local_ip_string, "%d.%d.%d.%d",
        p_recipe->socket_config.local_ip,
        p_recipe->socket_config.local_ip + 1,
        p_recipe->socket_config.local_ip + 2,
        p_recipe->socket_config.local_ip + 3);

    char *gateway_ip_string = cJSON_GetObjectItem(p_socket_address_json, "gateway")->valuestring;
    sscanf(gateway_ip_string, "%d.%d.%d.%d",
        p_recipe->socket_config.gateway,
        p_recipe->socket_config.gateway + 1,
        p_recipe->socket_config.gateway + 2,
        p_recipe->socket_config.gateway + 3);

    char *netmask_string = cJSON_GetObjectItem(p_socket_address_json, "netmask")->valuestring;
    sscanf(netmask_string, "%d.%d.%d.%d",
        p_recipe->socket_config.netmask,
        p_recipe->socket_config.netmask + 1,
        p_recipe->socket_config.netmask + 2,
        p_recipe->socket_config.netmask + 3);

    p_recipe->socket_config.port = cJSON_GetObjectItem(p_socket_address_json, "port")->valueint;
    printf("Socket_config: local ip is %s, plc ip is %s, gateway is %s, port is %d.\n", 
        local_ip_string, plc_ip_string, gateway_ip_string, p_recipe->socket_config.port);
}

/**
 * @description: Control Framework Printf List Function
 * @param name - printf function name
 * @param number_list - number_list pointer
 * @param length - number_list length
 * @return
 */
void ControlPrintfList(char name[5], uint8_t *number_list, uint16_t length)
{
    printf("\n******************%s****************\n", name);
    for (int32_t i = 0;i < length;i ++) {
        printf("0x%x ", number_list[i]);
    }
    printf("\n**************************************\n");
}

#ifdef CONTROL_USING_SOCKET
/**
 * @description: Control Framework Connect Socket
 * @param p_plc - basic socket plc pointer
 * @return success : 0 error : -1 -2 -3 -4 -5
 */
int ControlConnectSocket(BasicSocketPlc *p_plc)
{
    if (p_plc->socket >= 0)
        return 0;

    struct sockaddr_in plc_addr_in;
    plc_addr_in.sin_family = AF_INET;
    plc_addr_in.sin_port = htons(p_plc->port);

    char ip_string[20] = {0};
    sprintf(ip_string, "%u.%u.%u.%u", p_plc->ip[0], p_plc->ip[1], p_plc->ip[2], p_plc->ip[3]);
    plc_addr_in.sin_addr.s_addr = inet_addr(ip_string);
    memset(&(plc_addr_in.sin_zero), 0, sizeof(plc_addr_in.sin_zero));

    int plc_socket = socket(AF_INET, SOCK_STREAM, 0);
    int flag = 1;
    
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    if (setsockopt(plc_socket, IPPROTO_TCP, TCP_NODELAY, (void*)&flag, sizeof(flag)) < 0) {
        printf("Error setting TCP_NODELAY function!\n");
        return -1;
    }

    if (setsockopt(plc_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, (socklen_t)sizeof(struct timeval)) < 0) {
        printf("Error setting SO_SNDTIMEO function!\n");
        return -2;
    }

    if (setsockopt(plc_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, (socklen_t)sizeof(struct timeval)) < 0) {
        printf("Error setting SO_RCVTIMEO function!\n");
        return -3;
    }

    if (plc_socket < 0) {
        printf("Get socket error!\n");
        return -4;
    }

    printf("%s %d ip %u.%u.%u.%u port %d\n", __func__, __LINE__, 
        p_plc->ip[0], p_plc->ip[1], p_plc->ip[2], p_plc->ip[3],
        p_plc->port);

    if (connect(plc_socket, (struct sockaddr*)&plc_addr_in, sizeof(struct sockaddr)) == -1) {
        printf("Connect plc socket failed!errno %d\n", errno);
        closesocket(plc_socket);
        return -5;
    } else {
        p_plc->socket = plc_socket;
        printf("Connect plc socket success!\n");
        return 0;
    }
}

/**
 * @description: Control Framework Disconnect Socket
 * @param p_plc - basic socket plc pointer
 * @return success : 0 error : -1
 */
int ControlDisconnectSocket(BasicSocketPlc *p_plc)
{
    if (p_plc->socket < 0)
        return -1;

    int error = closesocket(p_plc->socket);
    if (0 == error)
        p_plc->socket = -1;
        
    return error;
}
#endif

/**
 * @description: Control Framework Protocol Open for Sub_Protocol, Init Circular Area and Receive Data Task
 * @param control_protocol - Control protocol pointer
 * @return success : 0 error : -1
 */
int ControlProtocolOpenDef(struct ControlProtocol *control_protocol)
{
    g_circular_area = CircularAreaAppInit(PLC_DATA_LENGTH);
    if (NULL == g_circular_area) {
        printf("%s CircularAreaInit error\n", __func__);
        return -1;
    }

    control_protocol->args = (void *)g_circular_area;

    pthread_attr_t attr;
    attr.schedparam.sched_priority = 19;
    attr.stacksize = 2048;

    PrivTaskCreate(&recv_plc_data_task, &attr, &ReceivePlcDataTask, control_protocol);
    PrivTaskStartup(&recv_plc_data_task);
}

/**
 * @description: Control Framework Protocol Open for Sub_Protocol, Release Circular Area and Delete Receive Data Task
 * @param void
 * @return success : 0 error : -1
 */
int ControlProtocolCloseDef(void)
{
    CircularAreaAppRelease(g_circular_area);

    PrivTaskDelete(recv_plc_data_task, 0);

    return 0;
}

/**
 * @description: Control Framework Get Value Memory Size From Recipe File
 * @param uniform_value_type - uniform value type
 * @return success : size error : 0
 */
uint8_t GetValueTypeMemorySize(UniformValueType uniform_value_type)
{
    switch (uniform_value_type)
    {
    case UNIFORM_BOOL:
    case UNIFORM_INT8:
    case UNIFORM_UINT8:
        return 1;
        break;
    case UNIFORM_INT16:
    case UNIFORM_UINT16:
        return 2;
        break;
    case UNIFORM_INT32:
    case UNIFORM_UINT32:
    case UNIFORM_FLOAT:
        return 4;
        break;
    case UNIFORM_DOUBLE:
        return 8;
        break;
    default:
        break;
    }

    return 0;
}

/**
 * @description: Control Framework Peripheral Device Init
 * @param p_recipe - Control recipe pointer
 * @return success : 0 error : 
 */
int ControlPeripheralInit(struct ControlRecipe *p_recipe)
{
    switch (p_recipe->communication_type)
    {
    case 0://Socket Init
        SocketInit(p_recipe->socket_config.local_ip, p_recipe->socket_config.netmask, p_recipe->socket_config.gateway);
        break;
    case 1://Serial Init
        SerialInit(p_recipe->serial_config.baud_rate, p_recipe->serial_config.data_bits, p_recipe->serial_config.stop_bits, p_recipe->serial_config.check_mode);
        break;
    default:
        break;
    }

    return 0;
}

/**
 * @description: Control Framework Get Recipe Basic Information
 * @param p_recipe - Control recipe pointer
 * @param p_recipe_file_json - recipe_file_json pointer
 * @return success : 0 error : -1
 */
int RecipeBasicInformation(struct ControlRecipe *p_recipe, cJSON *p_recipe_file_json)
{
    p_recipe->protocol_type = (ProtocolType)(cJSON_GetObjectItem(p_recipe_file_json, "protocol_type")->valueint);

    p_recipe->device_id = cJSON_GetObjectItem(p_recipe_file_json, "device_id")->valueint;
    strncpy(p_recipe->device_name, cJSON_GetObjectItem(p_recipe_file_json, "device_name")->valuestring, 20);
    p_recipe->read_period = cJSON_GetObjectItem(p_recipe_file_json, "read_period")->valueint;
    p_recipe->communication_type = cJSON_GetObjectItem(p_recipe_file_json, "communication_type")->valueint;

    printf("\n**************** RECIPE BASIC INFORMATION ******************\n");
    printf("\nprotocol_type: %d, communication_type: %d, device_id: %d, device_name: %s, read_period is %d\n",
        p_recipe->protocol_type, p_recipe->communication_type, p_recipe->device_id, p_recipe->device_name, p_recipe->read_period);

    switch (p_recipe->communication_type)
    {
    case 0://Get Socket Config
        ControlBasicSocketConfig(p_recipe, p_recipe_file_json);
        break;
    case 1://Get Serial Config
        ControlBasicSerialConfig(p_recipe, p_recipe_file_json);
        break;
    default:
        break;
    }

    printf("\n************************************************************\n");
}

/**
 * @description: Control Framework Read Variable Item Function
 * @param p_recipe - Control recipe pointer
 * @param p_recipe_file_json - recipe_file_json pointer
 * @return
 */
void RecipeReadVariableItem(struct ControlRecipe *p_recipe, cJSON *p_recipe_file_json)
{
    int i, ret = 0;

    ProtocolFormatInfo protocol_format_info;
    memset(&protocol_format_info, 0, sizeof(ProtocolFormatInfo));

    cJSON *read_item_list_json = cJSON_GetObjectItem(p_recipe_file_json, "read_item_list");
    if (cJSON_IsArray(read_item_list_json)) {
        /*Get Recipe Variable Item Count and total length*/
        p_recipe->read_item_count = cJSON_GetArraySize(read_item_list_json);
        p_recipe->total_data_length = GetRecipeTotalDataLength(read_item_list_json);

        /*Malloc Read Data Pointer, Reference "CONTROL FRAMEWORK READ DATA FORMAT"*/
        p_recipe->protocol_data.data = PrivMalloc(CONTROL_DATA_HEAD_LENGTH + p_recipe->total_data_length);
        p_recipe->protocol_data.data_length = CONTROL_DATA_HEAD_LENGTH + p_recipe->total_data_length;
        memset(p_recipe->protocol_data.data, 0, p_recipe->protocol_data.data_length);
        protocol_format_info.p_read_item_data = p_recipe->protocol_data.data + CONTROL_DATA_HEAD_LENGTH;
        /*Init The Control Protocol*/
        ControlProtocolInitDesc(p_recipe, protocol_init);
        /*Format Data Header, Reference "CONTROL FRAMEWORK READ DATA FORMAT"*/
        FormatDataHeader(p_recipe);
        uint16_t read_item_count = p_recipe->read_item_count;
        for (i = 0; i < read_item_count; i ++) {
            cJSON *read_single_item_json = cJSON_GetArrayItem(read_item_list_json, i);
            protocol_format_info.read_single_item_json = read_single_item_json;
            protocol_format_info.read_item_index = i;
            /*Format Protocol Cmd By Analyze Variable Item One By One*/
            ret = p_recipe->ControlProtocolFormatCmd(p_recipe, &protocol_format_info);
                if (ret < 0) {
                    printf("%s read %d item failed!\n", __func__, i);
                    continue;
            }
        }
    }
  
}

