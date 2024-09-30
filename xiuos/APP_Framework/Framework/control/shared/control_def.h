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
 * @file control_def.h
 * @brief DEFINE code for control framework 
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-10-08
 */

#ifndef CONTROL_DEF_H
#define CONTROL_DEF_H

#include <transform.h>
#include <list.h>
#include <circular_area_app.h>
#include <control.h>
#include <control_io.h>

#ifdef LIB_USING_CJSON
#include <cJSON.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CONTROL_PARAM_CHECK(param)                                                                  \
    do                                                                                              \
    {                                                                                               \
        if(NULL == param) {                                                                         \
            KPrintf("CONTROL CHECK FAILED ...%s %d %s is NULL.\n", __FUNCTION__, __LINE__, #param); \
            return -1;                                                                              \
        }                                                                                           \
    }while (0)   


typedef enum
{
    UNIFORM_BOOL = 1,
    UNIFORM_INT8,
    UNIFORM_INT16,
    UNIFORM_INT32,
    UNIFORM_UINT8,
    UNIFORM_UINT16,
    UNIFORM_UINT32,
    UNIFORM_DOUBLE,
    UNIFORM_FLOAT
}UniformValueType;

typedef struct 
{
    uint8_t ip[4];
    uint16_t port;
    int32_t socket;
    int8_t secondary_connect_flag;//0: enble, no connected;  1: enable, connected;  -1: disable
}BasicSocketPlc;

typedef struct 
{
    uint16_t command_length;
    uint16_t data_size;
    uint8_t command_ready;
    uint8_t *p_command;
    uint8_t *p_data;
}BasicPlcDataInfo;

typedef struct 
{
    cJSON *read_single_item_json;
    uint8_t *p_read_item_data;
    uint16_t read_item_index;//Variable item index(1 ++)
    uint8_t last_item_size;
}ProtocolFormatInfo;

struct ProtocolData
{
    uint8_t *data;
    uint16_t data_length;
};

struct SerialConfig
{
    uint8_t station;
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t check_mode;
};

struct SocketConfig
{
    uint16_t port;
    uint8_t plc_ip[4];
    uint8_t local_ip[4];
    uint8_t gateway[4];
    uint8_t netmask[4];
};

struct ControlRecipe
{
    uint8_t device_name[20];
    uint16_t device_id;
    uint16_t read_period;
    uint16_t read_item_count;
    uint16_t total_data_length;
    uint8_t communication_type;

    ProtocolType protocol_type;

    void *read_item;
    struct ControlDone *done;

    struct SerialConfig serial_config;
    struct SocketConfig socket_config;

    struct ProtocolData protocol_data;

    int (*ControlProtocolFormatCmd)(struct ControlRecipe *p_recipe, ProtocolFormatInfo *protocol_format_info);
};

/*Get Value Type Memory Size*/
uint8_t GetValueTypeMemorySize(UniformValueType uniform_value_type);

/*Get basic information from recipe file*/
int RecipeBasicInformation(struct ControlRecipe *p_recipe, cJSON *p_recipe_file_json);

/*Get the variable need to read from recipe file*/
void RecipeReadVariableItem(struct ControlRecipe *p_recipe, cJSON *p_recipe_file_json);

/*Control Framework Peripheral Device Init*/
int ControlPeripheralInit(struct ControlRecipe *p_recipe);

/*Control Framework Printf List Function*/
void ControlPrintfList(char name[5], uint8_t *number_list, uint16_t length);

/*Control Framework Connect Socket*/
int ControlConnectSocket(BasicSocketPlc *p_plc);

/*Control Framework Disconnect Socket*/
int ControlDisconnectSocket(BasicSocketPlc *p_plc);

/*Control Framework Protocol Open for Sub_Protocol, Init Circular Area and Receive Data Task*/
int ControlProtocolOpenDef(struct ControlProtocol *control_protocol);

/*Control Framework Protocol Open for Sub_Protocol, Release Circular Area and Delete Receive Data Task*/
int ControlProtocolCloseDef(void);

#ifdef __cplusplus
}
#endif

#endif

