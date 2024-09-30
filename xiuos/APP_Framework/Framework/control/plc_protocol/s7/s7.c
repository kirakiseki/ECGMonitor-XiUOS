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
 * @file s7.c
 * @brief plc protocol s7 
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023-4-14
 */

#include <s7.h>

/**
 * @description: S7 Receive Plc Data Task
 * @param parameter - parameter pointer
 * @return
 */
void *ReceivePlcDataTask(void *parameter)
{
    printf("%s %d\n", __func__, __LINE__);
    ReadPlcDataByRecipe(control_protocol->recipe);
}

/**
 * @description: S7 Protocol Open
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int S7Open(struct ControlProtocol *control_protocol)
{
    ControlProtocolOpenDef(control_protocol);
    return 0;
}

/**
 * @description: S7 Protocol Close
 * @param control_protocol - control protocol pointer
 * @return success : 0 error
 */
int S7Close(struct ControlProtocol *control_protocol)
{    
    ControlProtocolCloseDef();

    return 0;
}

/**
 * @description: S7 Protocol Read Data
 * @param control_protocol - control protocol pointer
 * @param buf - read data buffer pointer
 * @param len - read data length
 * @return success : data length error : 0
 */
int S7Read(struct ControlProtocol *control_protocol, void *buf, size_t len)
{
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;
    return CircularAreaAppRead(circular_area, buf, len);
}

static struct ControlDone s7_protocol_done = 
{
    ._open = S7Open,
    ._close = S7Close,
    ._read = S7Read,
    ._write = NULL,
    ._ioctl = NULL,
};

/**
 * @description: Push Data Onto a Stack One By One
 * @param datastack - data stack pointer
 * @param args - data pointer
 * @param length - data length
 * @return 
 */
void PushDataIntoStack(uint8_t *datastack,uint8_t* args,uint16_t length)
{
    static int index = 8;    
    for(int i =0; i < length; i ++) {
        datastack[index] = args[i];
        index++;
        if(index >= control_protocol->recipe->protocol_data.data_length){
            index = 8;
        }     
    }
}

/**
 * @description: Read PLC Data By Recipe
 * @param p_recipe - recipe pointer
 * @return success : 0 error : -1
 */
int8_t ReadPlcDataByRecipe(struct ControlRecipe *p_recipe)
{   
    uint16_t data_length = control_protocol->recipe->protocol_data.data_length;
    uint8_t *s7_data = control_protocol->recipe->protocol_data.data;
    struct CircularAreaApp *circular_area = (struct CircularAreaApp *)control_protocol->args;

    S7Object s7_plc = {0};
    char plc_ip_string[15] = {0};
    s7_plc = Cli_Create();
    sprintf(plc_ip_string, "%u.%u.%u.%u",
        p_recipe->socket_config.plc_ip[0],
        p_recipe->socket_config.plc_ip[1],
        p_recipe->socket_config.plc_ip[2],
        p_recipe->socket_config.plc_ip[3]);
    int16_t read_item_count = p_recipe->read_item_count;
    uint8_t *p_read_item = (uint8_t *)(p_recipe->read_item);

    while (1) {
        int8_t error = 0;
        while (!error) {
            uint16_t i = 0;
            for (i = 0; i < read_item_count; i ++) {
                int is_connected = 0;
                Cli_GetConnected(s7_plc, &is_connected);  
                while (!is_connected) {
                    if (Cli_ConnectTo(s7_plc, plc_ip_string, 0, 1) != 0) {
                        PrivTaskDelay(1000);
                    } else {
                        break;
                    }
                }
                TS7DataItem data_info = ((S7ReadItem*)p_read_item + i)->data_info;
                Cli_ReadMultiVars(s7_plc, &data_info, 1);
                uint16_t Size = GetValueTypeMemorySize(((S7ReadItem*)p_read_item + i)->value_type);
                ControlPrintfList("S7 RECV", data_info.pdata,Size);          
                PushDataIntoStack(s7_data,data_info.pdata,Size);
                PrivTaskDelay(100);
            }

            /*read all variable item data, put them into circular_area*/
            if (i == read_item_count) {
                printf("%s get %d item %d length\n", __func__, i, data_length);
                CircularAreaAppWrite(circular_area, s7_data, data_length, 0);
            }    
        }
    } 
    return 0; 
}

/**
 * @description: S7 Protocol read item Init
 * @param p_read_item - read item pointer
 * @param read_item_json - read item json pointer
 * @param p_data - unused
 * @return success : 0 error : -1
 */
static uint8_t InitialS7ReadItem(S7ReadItem* p_read_item, cJSON* read_item_json, uint8_t* p_data)
{
    p_read_item->value_type = cJSON_GetObjectItem(read_item_json, "value_type")->valueint;
    strncpy(p_read_item->value_name, cJSON_GetObjectItem(read_item_json, "value_name")->valuestring, 20);
    TS7DataItem* p_data_info = &(p_read_item->data_info);
    p_data_info->Amount = cJSON_GetObjectItem(read_item_json, "amount")->valueint;
    p_data_info->Start = cJSON_GetObjectItem(read_item_json, "start")->valueint;
    p_data_info->DBNumber = cJSON_GetObjectItem(read_item_json, "db_number")->valueint;
    char* area_valuestring = cJSON_GetObjectItem(read_item_json, "area")->valuestring;
    if (strcmp(area_valuestring, "I") == 0)
        p_data_info->Area = S7AreaPE;
    else if (strcmp(area_valuestring, "Q") == 0)
        p_data_info->Area = S7AreaPA;
    else if (strcmp(area_valuestring, "M") == 0)
        p_data_info->Area = S7AreaMK;
    else if (strcmp(area_valuestring, "DB") == 0)
        p_data_info->Area = S7AreaDB;
    else if (strcmp(area_valuestring, "C") == 0)
        p_data_info->Area = S7AreaCT;
    else if (strcmp(area_valuestring, "T") == 0)
        p_data_info->Area = S7AreaTM;
    char* wordlen_valuestring = cJSON_GetObjectItem(read_item_json, "wordlen")->valuestring;
    if (strcmp(wordlen_valuestring, "Bit") == 0)
        p_data_info->WordLen = S7WLBit;
    else if (strcmp(wordlen_valuestring, "Byte") == 0)
        p_data_info->WordLen = S7WLByte;
    else if (strcmp(wordlen_valuestring, "Word") == 0)
        p_data_info->WordLen = S7WLWord;
    else if (strcmp(wordlen_valuestring, "DWord") == 0)
        p_data_info->WordLen = S7WLDWord;
    else if (strcmp(wordlen_valuestring, "Real") == 0)
        p_data_info->WordLen = S7WLReal;
    else if (strcmp(wordlen_valuestring, "Counter") == 0)
        p_data_info->WordLen = S7WLCounter;
    else if (strcmp(wordlen_valuestring, "Timer") == 0)
        p_data_info->WordLen = S7WLTimer;
    p_data_info->pdata = p_data;
    printf("value_type is %d, amount is %d, start is %04d, db_number is %d, area is 0x%03x, wordlen is %d.\n",
        p_read_item->value_type, p_data_info->Amount, p_data_info->Start, p_data_info->DBNumber,
        p_data_info->Area, p_data_info->WordLen);
    return GetValueTypeMemorySize(p_read_item->value_type);
}

/**
 * @description: S7 Protocol Cmd Generate
 * @param p_recipe - recipe pointer
 * @param protocol_format_info - protocol format info pointer
 * @return success : 0 error : -1
 */
int S7ProtocolFormatCmd(struct ControlRecipe *p_recipe, ProtocolFormatInfo *protocol_format_info)
{
    int ret = 0;
    uint8_t *S7_plc_read_data = PrivMalloc(p_recipe->protocol_data.data_length);
    uint16_t S7_plc_read_data_index = 8;
    cJSON *read_single_item_json = protocol_format_info->read_single_item_json;
    int i = protocol_format_info->read_item_index;
    if (0 == i)
        p_recipe->read_item = PrivMalloc(sizeof(S7ReadItem) * p_recipe->read_item_count);
        S7_plc_read_data_index += InitialS7ReadItem((S7ReadItem *)(p_recipe->read_item) + i,
            read_single_item_json, S7_plc_read_data + S7_plc_read_data_index);
        if (S7_plc_read_data_index == 8) {
            ret = -1;
            printf("%s read %d item failed!\n", __func__, i);
        }    
    return ret;
}
/**
 * @description: S7 Protocol Init
 * @param p_recipe - recipe pointer
 * @return success : 0 error : -1
 */
int S7ProtocolInit(struct ControlRecipe *p_recipe)
{   
    p_recipe->read_item = PrivMalloc(sizeof(S7ReadItem) * p_recipe->read_item_count);
    if (NULL == p_recipe->read_item) {
        PrivFree(p_recipe->read_item);
        return -1;
    }
    memset(p_recipe->read_item, 0, sizeof(S7ReadItem));
    p_recipe->ControlProtocolFormatCmd = S7ProtocolFormatCmd;
    p_recipe->done = &s7_protocol_done;

    return 0;
}
