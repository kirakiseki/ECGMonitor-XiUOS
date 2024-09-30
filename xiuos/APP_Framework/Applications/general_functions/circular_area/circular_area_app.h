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
* @file:   circular_area_app.h
* @brief:  function declaration and structure defintion of circular area for applications
* @version: 3.0
* @author:  AIIT XUOS Lab
* @date:    2022/11/21
*
*/

#ifndef CIRCULAR_AREA_APP_H
#define CIRCULAR_AREA_APP_H

#include <string.h>
#include <transform.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CA_CHECK(TRUE_CONDITION)                                                                                   \
    do                                                                                                             \
    {                                                                                                              \
        if(!(TRUE_CONDITION)) {                                                                                    \
            printf("%s CHECK condition is false at line[%d] of [%s] func.\n", #TRUE_CONDITION, __LINE__, __func__);\
            while(1);                                                                                              \
        }                                                                                                          \
    }while (0) 

#define CA_PARAM_CHECK(param)                                                                  \
    do                                                                                         \
    {                                                                                          \
        if(param == NULL) {                                                                    \
            printf("PARAM CHECK FAILED ...%s %d %s is NULL.\n", __func__, __LINE__, #param);  \
            while(1);                                                                          \
        }                                                                                      \
    }while (0) 

#define CA_ALIGN_DOWN(size, align)         ((size)/(align)*(align))

typedef struct CircularAreaApp *CircularAreaAppType;

struct CircularAreaAppOps
{
    int (*read) (CircularAreaAppType circular_area, uint8_t *output_buffer, uint32_t data_length);
    int (*write) (CircularAreaAppType circular_area, uint8_t *input_buffer, uint32_t data_length, int b_force);
    void (*release) (CircularAreaAppType circular_area);
    void (*reset) (CircularAreaAppType circular_area);
};

struct CircularAreaApp
{
    uint8_t *data_buffer;

    uint32_t readidx;
    uint32_t writeidx;

    uint8_t *p_head;
    uint8_t *p_tail;

    uint32_t area_length;
    int b_status;

    struct CircularAreaAppOps *CircularAreaAppOperations;
};

/*This function will return whether the circular_area is full or not*/
int CircularAreaAppIsFull(CircularAreaAppType circular_area);

/*This function will return whether the circular_area is empty or not*/
int CircularAreaAppIsEmpty(CircularAreaAppType circular_area);

/*This function will reset the circular_area and set the descriptor to default*/
void CircularAreaAppReset(CircularAreaAppType circular_area);

/*This function will release the circular_area descriptor and free the memory*/
void CircularAreaAppRelease(CircularAreaAppType circular_area);

/*This function will read data from the circular_area*/
int CircularAreaAppRead(CircularAreaAppType circular_area, uint8_t *output_buffer, uint32_t data_length);

/*This function will write data to the circular_area*/
int CircularAreaAppWrite(CircularAreaAppType circular_area, uint8_t *input_buffer, uint32_t data_length, int b_force);

/*This function will get the circual_area max length*/
uint32_t CircularAreaAppGetMaxLength(CircularAreaAppType circular_area);

/*This function will get the data length of the circular_area*/
uint32_t CircularAreaAppGetDataLength(CircularAreaAppType circular_area);

/*This function will initialize the circular_area*/
CircularAreaAppType CircularAreaAppInit(uint32_t circular_area_length);

#ifdef __cplusplus
}
#endif

#endif
