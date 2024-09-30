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
 * @file control.c
 * @brief code for control framework app
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022-09-27
 */

#include <control.h>
#include <control_def.h>

/**
 * @description: Control Framework Find certain Protocol
 * @param void
 * @return Control Protocol pointer
 */
ControlProtocolType ControlProtocolFind(void)
{
    return control_protocol;
}

/**
 * @description: Control Framework Protocol Init
 * @param control_protocol - control protocol pointer
 * @return success : 0 error : -1
 */
static int ControlProtocolInit(ControlProtocolType control_protocol)
{
    CONTROL_PARAM_CHECK(control_protocol);
    int ret = -1;

    control_protocol->protocol_status = CONTROL_INIT;

    ret = PrivMutexCreate(&control_protocol->lock, 0);
    if(ret < 0) {
        printf("ControlProtocolInit mutex create failed.\n");
        goto _out;
    }

    ret = PrivSemaphoreCreate(&control_protocol->sem, 0, 0);
    if (ret < 0) {
        printf("ControlProtocolInit create sem error\n");
        goto _out;
    }

_out:
    return ret;    
}

/**
 * @description: Analyze Recipe
 * @param control_protocol - Control Protocol pointer
 * @param recipe_name - recipe name
 * @return success : 0 error : -1
 */
static int ControlAnalyzeRecipe(ControlProtocolType control_protocol, const char *recipe_name)
{
    int recipe_file_fd = -1;
    struct stat recipe_file_status;
    uint16_t recipe_file_length = 0;
    char *recipe_file_buf;

    /*wait for SD-card mount done*/
    PrivTaskDelay(5000);

    //Step1 : read recipe file data from SD card or other store device
    recipe_file_fd = PrivOpen(recipe_name, O_RDONLY);
    if (recipe_file_fd < 0) {
        printf("Open recipe file %s failed\n", recipe_name);
        PrivClose(recipe_file_fd);
        return -1;
    }

    if (0 != fstat(recipe_file_fd, &recipe_file_status)) {
        printf("Get recipe file information failed!\n");
        PrivClose(recipe_file_fd);
        return -1;
    } else {
        recipe_file_length = recipe_file_status.st_size;
    }

    recipe_file_buf = PrivMalloc(recipe_file_length);
    if (NULL == recipe_file_buf) {
        printf("Get recipe file memory failed!\n");
        PrivFree(recipe_file_buf);
        PrivClose(recipe_file_fd);
        return -1;
    }

    if (PrivRead(recipe_file_fd, recipe_file_buf, recipe_file_length) < 0) {
        printf("Read recipe file failed!\n");
        PrivFree(recipe_file_buf);
        PrivClose(recipe_file_fd);
        return -1;
    }

    PrivClose(recipe_file_fd);  

    //Step2 : CJSON analyze
#ifdef LIB_USING_CJSON
    cJSON *recipe_file_json = cJSON_Parse(recipe_file_buf);
    if (NULL == recipe_file_json) {
        printf("Parse recipe_file_buf failed!\n");
        return -1;
    }
    
    control_protocol->recipe = (struct ControlRecipe *)PrivMalloc(sizeof(struct ControlRecipe));
    memset(control_protocol->recipe, 0, sizeof(struct ControlRecipe));

    /*Get basic information from recipe file*/
    if (RecipeBasicInformation(control_protocol->recipe, recipe_file_json) < 0) {
        return -1;
    }

    strncpy(control_protocol->device->dev_name, control_protocol->recipe->device_name, 20);
    control_protocol->protocol_type = control_protocol->recipe->protocol_type;

    /*Get the variable need to read from recipe file*/
    RecipeReadVariableItem(control_protocol->recipe, recipe_file_json);

    control_protocol->done = control_protocol->recipe->done;

    cJSON_Delete(recipe_file_json);

    PrivFree(recipe_file_buf);
    printf("Read and parse recipe file done!\n");
#endif

    return 0;
}

/**
 * @description: Control Framework Protocol Open
 * @param control_protocol - Control Protocol pointer
 * @return success : 0 error : -1
 */
int ControlProtocolOpen(struct ControlProtocol *control_protocol)
{
    CONTROL_PARAM_CHECK(control_protocol);
    CONTROL_PARAM_CHECK(control_protocol->done);
    int ret = -1;

    if (control_protocol->done->_open) {
        ret = control_protocol->done->_open(control_protocol);
    }

    return ret;
}

/**
 * @description: Control Framework Protocol Close
 * @param control_protocol - Control Protocol pointer
 * @return success : 0 error : -1
 */
int ControlProtocolClose(struct ControlProtocol *control_protocol)
{
    CONTROL_PARAM_CHECK(control_protocol);
    CONTROL_PARAM_CHECK(control_protocol->done);
    int ret = -1;

    if (control_protocol->done->_close) {
        ret = control_protocol->done->_close(control_protocol);
    }

    return ret;
}

/**
 * @description: Control Framework Protocol Read Data
 * @param control_protocol - Control Protocol pointer
 * @param buf - read buffer
 * @param len - read data length
 * @return success : data length error : -1
 */
int ControlProtocolRead(struct ControlProtocol *control_protocol, void *buf, size_t len)
{
    CONTROL_PARAM_CHECK(control_protocol);
    CONTROL_PARAM_CHECK(control_protocol->done);
    int ret = -1;

    if (control_protocol->done->_read) {
        ret = control_protocol->done->_read(control_protocol, buf, len);
    }

    return ret;
}

/**
 * @description: Control Framework Protocol Write Cmd
 * @param control_protocol - Control Protocol pointer
 * @param buf - write buffer
 * @param len - write data length
 * @return success : data length error : -1
 */
int ControlProtocolWrite(struct ControlProtocol *control_protocol, const void *buf, size_t len)
{
    CONTROL_PARAM_CHECK(control_protocol);
    CONTROL_PARAM_CHECK(control_protocol->done);
    int ret = -1;

    if (control_protocol->done->_write) {
        ret = control_protocol->done->_write(control_protocol, buf, len);
    }

    return ret;
}

/**
 * @description: Control Framework Protocol Ioctl
 * @param control_protocol - Control Protocol pointer
 * @param cmd - ioctl cmd
 * @param args - args
 * @return success : 0 error : -1
 */
int ControlProtocolIoctl(struct ControlProtocol *control_protocol, int cmd, void *args)
{
    CONTROL_PARAM_CHECK(control_protocol);
    CONTROL_PARAM_CHECK(control_protocol->done);
    int ret = -1;

    if (control_protocol->done->_ioctl) {
        ret = control_protocol->done->_ioctl(control_protocol, cmd, args);
    }

    return ret;    
}

/**
 * @description: Control Framework Init
 * @param void
 * @return success : 0 error : -1
 */
int ControlFrameworkInit(void)
{
    int ret = 0;

    control_protocol = (struct ControlProtocol *)PrivMalloc(sizeof(struct ControlProtocol));
    if (NULL == control_protocol) {
        printf("%s malloc control protocol failed!\n", __func__);
        PrivFree(control_protocol);
        ret = -1;
        goto _out;
    }

    control_protocol->device = (struct ControlDevice *)PrivMalloc(sizeof(struct ControlDevice));
    if (NULL == control_protocol->device) {
        printf("%s malloc control device failed!\n", __func__);
        PrivFree(control_protocol->device);
        PrivFree(control_protocol);
        ret = -1;
        goto _out;
    }
    
    //Control Protocol Struct Init
    ret = ControlProtocolInit(control_protocol);
    if (ret < 0) {
        printf("%s failed!\n", __func__);
        PrivFree(control_protocol);
        goto _out;
    }

    printf("%s malloc CONTROL_RECIPE_FILE %s\n", __func__, CONTROL_RECIPE_FILE);

    //Read Recipe File, Get Control Protocol Configure Param
    ret = ControlAnalyzeRecipe(control_protocol, CONTROL_RECIPE_FILE);
    if (ret < 0) {
        printf("%s failed!\n", __func__);
        PrivFree(control_protocol);
        goto _out;
    }

    control_protocol->protocol_status = CONTROL_REGISTERED;
    control_protocol->device->status = CONTROL_REGISTERED;

    ret = ControlPeripheralInit(control_protocol->recipe);
    if (ret < 0) {
        printf("%s failed!\n", __func__);
        PrivFree(control_protocol);
        goto _out;
    }
    
    printf("%u %u\n",control_protocol->recipe->total_data_length,control_protocol->recipe->device_id);

    printf("%s ControlPeripheralInit done\n", __func__);

_out:
    return ret;
}

static char *const protocol_type_str[] =
{
    "TYPE_START",
    "S7",
    "MODBUS_TCP",
    "MODBUS_UART",
    "OPC_UA",
    "FINS",
    "MELSEC_1E",
    "MELSEC_3E_Q_L",
    "MELSEC_3E_IQ_R",
    "MELSEC_1C",
    "MELSEC_3C",
    "TYPE_END"
};

/**
 * @description: Control Framework Shell Cmd Information
 * @param void
 * @return success : 0 error : -1
 */
void ShowControl(void)
{
    int i = 0; 
    int maxlen;
    const char *item_type = "control_protocol_type";
    const char *item_name_0 = "control_protocol_name";
    const char *item_name_1 = "control_device_name";
    const char *item_status = "status";

    ControlProtocolType control_protocol = ControlProtocolFind();

    printf(" %-28s%-28s%-26s%-20s\n", item_type, item_name_0, item_name_1, item_status); 
    maxlen = 90;
    while (i < maxlen) {
        i++;
        if (maxlen == i) {
            printf("-\n");
        } else {
            printf("-");
        }
    }

    if (control_protocol) {
        printf("%s", " ");
        KPrintf("%-28s%-28s%-26s%-8d\n", 
            protocol_type_str[1], 
            protocol_type_str[1],
            control_protocol->device->dev_name,
            control_protocol->device->status);
    }
}
PRIV_SHELL_CMD_FUNCTION(ShowControl, show control framework information, PRIV_SHELL_CMD_FUNC_ATTR);
