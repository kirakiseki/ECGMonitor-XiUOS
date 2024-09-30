/****************************************************************************
 * libs/libc-musl/string/lib_memrchr.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/**
* @file lib_memrchr.c
* @brief nuttx source code
*        https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-08-04
*/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <string.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: memrchr
 *
 * Description:
 *   The memrchr() function locates the last occurrence of 'c' (converted to
 *   an unsigned char) in the initial 'n' bytes (each interpreted as
 *   unsigned char) of the object pointed to by s.
 *
 * Returned Value:
 *   The memrchr() function returns a pointer to the located byte, or a null
 *   pointer if the byte does not occur in the object.
 *
 ****************************************************************************/

FAR void *memrchr(FAR const void *s, int c, size_t n)
{
	const unsigned char *p = (FAR const unsigned char *)s;
	c = (unsigned char)c;
	while (n--) if (p[n]==c) return (void *)(p+n);
	return NULL;
}
