/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

/**
* @file mem_syscalls.c
* @brief support newlib memory
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2020-09-23
*/

/*************************************************
File name: mem_syscalls.c
Description: support newlib memory
Others: take RT-Thread v4.0.2/components/libc/compilers/newlib/syscalls.c for references
                https://github.com/RT-Thread/rt-thread/tree/v4.0.2
History: 
1. Date: 2020-09-23
Author: AIIT XUOS Lab
Modification: Use malloc, realloc, calloc and free functions
*************************************************/
/* wwg debug here */
// #include <xizi.h>
#include <errno.h>

void *_malloc_r (struct _reent *ptr, size_t size)
{
    /* wwg debug here */
    // void* result = (void*)x_malloc(size);
    void* result = NULL;

    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void *_realloc_r (struct _reent *ptr, void *old, size_t newlen)
{
    /* wwg debug here */
    // void* result = (void*)x_realloc(old, newlen);
    void* result = NULL;

    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void *_calloc_r (struct _reent *ptr, size_t size, size_t len)
{
    /* wwg debug here */
    // void* result = (void*)x_calloc(size, len);
    void* result = NULL;
    
    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void _free_r (struct _reent *ptr, void *address)
{
    /* wwg debug here */
    // x_free (address);
}