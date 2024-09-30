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
* @file:    circular_area_app.c
* @brief:   circular area file for applications
* @version: 3.0
* @author:  AIIT XUOS Lab
* @date:    2022/11/21
*
*/

#include "circular_area_app.h"

/**
 * This function will return whether the circular_area is full or not
 *
 * @param circular_area CircularAreaApp descriptor
 */
int CircularAreaAppIsFull(CircularAreaAppType circular_area)
{
    CA_PARAM_CHECK(circular_area);

    if((circular_area->readidx == circular_area->writeidx) && (circular_area->b_status)) {
        printf("the circular area is full\n");
        return 1;
    } else {
        return 0;
    }
}

/**
 * This function will return whether the circular_area is empty or not
 *
 * @param circular_area CircularAreaApp descriptor
 */
int CircularAreaAppIsEmpty(CircularAreaAppType circular_area)
{
    CA_PARAM_CHECK(circular_area);

    if((circular_area->readidx == circular_area->writeidx) && (!circular_area->b_status)) {
        //printf("the circular area is empty\n");
        return 1;
    } else {
        return 0;
    }
}

/**
 * This function will reset the circular_area and set the descriptor to default
 *
 * @param circular_area CircularAreaApp descriptor
 */
void CircularAreaAppReset(CircularAreaAppType circular_area)
{
    circular_area->writeidx = 0;
    circular_area->readidx = 0;
    circular_area->b_status = 0;
}

/**
 * This function will release the circular_area descriptor and free the memory
 *
 * @param circular_area CircularAreaApp descriptor
 */
void CircularAreaAppRelease(CircularAreaAppType circular_area)
{
    circular_area->readidx = 0;
    circular_area->writeidx = 0;
    circular_area->p_head = NULL;
    circular_area->p_tail = NULL;
    circular_area->b_status = 0;
    circular_area->area_length = 0;

    PrivFree(circular_area->data_buffer);
    PrivFree(circular_area);
}

/**
 * This function will get the circual_area max length
 *
 * @param circular_area CircularAreaApp descriptor
 */
uint32_t CircularAreaAppGetMaxLength(CircularAreaAppType circular_area)
{
    CA_PARAM_CHECK(circular_area);

    return circular_area->area_length;
}

/**
 * This function will get the data length of the circular_area
 *
 * @param circular_area CircularAreaApp descriptor
 */
uint32_t CircularAreaAppGetDataLength(CircularAreaAppType circular_area)
{
    CA_PARAM_CHECK(circular_area);

    if(CircularAreaAppIsFull(circular_area)) {
        return circular_area->area_length;
    } else {
        return (circular_area->writeidx - circular_area->readidx + circular_area->area_length) % circular_area->area_length;
    }
}

/**
 * This function will return whether it is need to divide the read data into two parts  or not
 *
 * @param circular_area CircularAreaApp descriptor
 * @param data_length output data length
 */
static uint32_t CircularAreaAppDivideRdData(CircularAreaAppType circular_area, uint32_t data_length)
{
    CA_PARAM_CHECK(circular_area);

    if(circular_area->readidx + data_length <= circular_area->area_length) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * This function will return whether it is need to divide the write data into two parts  or not
 *
 * @param circular_area CircularAreaApp descriptor
 * @param data_length input data length
 */
static uint32_t CircularAreaAppDivideWrData(CircularAreaAppType circular_area, uint32_t data_length)
{
    CA_PARAM_CHECK(circular_area);

    if(circular_area->writeidx + data_length <= circular_area->area_length) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * This function will read data from the circular_area
 *
 * @param circular_area CircularAreaApp descriptor
 * @param output_buffer output data buffer poniter
 * @param data_length output data length
 */
int CircularAreaAppRead(CircularAreaAppType circular_area, uint8_t *output_buffer, uint32_t data_length)
{
    CA_PARAM_CHECK(circular_area);
    CA_PARAM_CHECK(output_buffer);
    CA_CHECK(data_length > 0);

    if(CircularAreaAppIsEmpty(circular_area)) {
        return -1;
    }

    int read_length = (data_length > CircularAreaAppGetDataLength(circular_area)) ? CircularAreaAppGetDataLength(circular_area) : data_length;
    // if (data_length > CircularAreaAppGetDataLength(circular_area)) {
    //     return -1;
    // }

    if(CircularAreaAppDivideRdData(circular_area, read_length)) {
        uint32_t read_len_up = circular_area->area_length - circular_area->readidx;
        uint32_t read_len_down = read_length - read_len_up;

        memcpy(output_buffer, &circular_area->data_buffer[circular_area->readidx], read_len_up);
        memcpy(output_buffer + read_len_up, circular_area->p_head, read_len_down);

        circular_area->readidx = read_len_down;
    } else {
        memcpy(output_buffer, &circular_area->data_buffer[circular_area->readidx], read_length);
        circular_area->readidx = (circular_area->readidx + read_length) % circular_area->area_length;
    }

    circular_area->b_status = 0;

    return read_length;
}

/**
 * This function will write data to the circular_area
 *
 * @param circular_area CircularAreaApp descriptor
 * @param input_buffer input data buffer poniter
 * @param data_length input data length
 * @param b_force whether to force to write data disregard the length limit
 */
int CircularAreaAppWrite(CircularAreaAppType circular_area, uint8_t *input_buffer, uint32_t data_length, int b_force)
{
    CA_PARAM_CHECK(circular_area);
    CA_PARAM_CHECK(input_buffer);
    CA_CHECK(data_length > 0);

    if(CircularAreaAppIsFull(circular_area) && (!b_force)) {
        return -1;
    }

    uint32_t write_data_length = circular_area->area_length - CircularAreaAppGetDataLength(circular_area);
    //data_length = (data_length > write_data_length) ? write_data_length : data_length;
    if (data_length > write_data_length) {
        return -1;
    }

    if(CircularAreaAppDivideWrData(circular_area, data_length)) {
        uint32_t write_len_up = circular_area->area_length - circular_area->writeidx;
        uint32_t write_len_down = data_length - write_len_up;

        memcpy(&circular_area->data_buffer[circular_area->writeidx], input_buffer, write_len_up);
        memcpy(circular_area->p_head, input_buffer + write_len_up, write_len_down);

        circular_area->writeidx = write_len_down;
    } else {
        memcpy(&circular_area->data_buffer[circular_area->writeidx], input_buffer, data_length);
        circular_area->writeidx = (circular_area->writeidx + data_length) % circular_area->area_length;
    }

    circular_area->b_status = 1;

    if(b_force) {
        circular_area->readidx = circular_area->writeidx;
    }

    return 0;
}

static struct CircularAreaAppOps CircularAreaAppOperations =
{
    CircularAreaAppRead,
    CircularAreaAppWrite,
    CircularAreaAppRelease,
    CircularAreaAppReset,
};

/**
 * This function will initialize the circular_area
 *
 * @param circular_area_length circular_area length
 */
CircularAreaAppType CircularAreaAppInit(uint32_t circular_area_length)
{
    CA_CHECK(circular_area_length > 0);

    circular_area_length = CA_ALIGN_DOWN(circular_area_length, 8);

    CircularAreaAppType circular_area = PrivMalloc(sizeof(struct CircularAreaApp));
    if(NULL == circular_area) {
        printf("CircularAreaAppInit malloc struct circular_area failed\n");
        PrivFree(circular_area);
        return NULL;
    }

    CircularAreaAppReset(circular_area);

    circular_area->data_buffer = PrivMalloc(circular_area_length);
    if(NULL == circular_area->data_buffer) {
        printf("CircularAreaAppInit malloc circular_area data_buffer failed\n");
        PrivFree(circular_area->data_buffer);
        return NULL;
    }

    circular_area->p_head = circular_area->data_buffer;
    circular_area->p_tail = circular_area->data_buffer + circular_area_length;
    circular_area->area_length = circular_area_length;

    printf("CircularAreaAppInit done p_head %8p p_tail %8p length %u\n",
        circular_area->p_head, circular_area->p_tail, circular_area->area_length);

    circular_area->CircularAreaAppOperations = &CircularAreaAppOperations;

    return circular_area;
}
