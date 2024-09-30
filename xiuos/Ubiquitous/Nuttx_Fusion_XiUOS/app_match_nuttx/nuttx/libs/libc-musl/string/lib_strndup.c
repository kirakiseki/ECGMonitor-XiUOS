/****************************************************************************
 * libs/libc-musl/string/lib_strndup.c
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
* @file lib_strndup.c
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

#include "libc-musl.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: strndup
 *
 * Description:
 *   The strndup() function is equivalent to the strdup() function,
 *   duplicating the provided 's' in a new block of memory allocated as
 *   if by using malloc(), with the exception being that strndup() copies
 *   at most 'size' plus one bytes into the newly allocated memory,
 *   terminating the new string with a NUL character. If the length of 's'
 *   is larger than 'size', only 'size' bytes will be duplicated. If
 *   'size' is larger than the length of 's', all bytes in s will be
 *   copied into the new memory buffer, including the terminating NUL
 *   character. The newly created string will always be properly
 *   terminated.
 *
 ****************************************************************************/

#undef strndup /* See mm/README.txt */
FAR char *strndup(FAR const char *s, size_t size)
{
	size_t l = strnlen(s, size);
	char *d = lib_malloc(l+1);
	if (!d) return NULL;
	memcpy(d, s, l);
	d[l] = '\0';
	return d;
}
