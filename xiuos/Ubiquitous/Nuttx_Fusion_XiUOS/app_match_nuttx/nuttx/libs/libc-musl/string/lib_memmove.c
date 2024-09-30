/****************************************************************************
 * libs/libc-musl/string/lib_memmove.c
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
* @file lib_memmove.c
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
#include <sys/types.h>
#include <string.h>
#include <stdint.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifndef CONFIG_LIBC_ARCH_MEMMOVE
#undef memmove /* See mm/README.txt */
FAR void *memmove(FAR void *dest, FAR const void *src, size_t count)
{
	char *d = dest;
	const char *s = src;

	if (d==s) return d;
	if ((uintptr_t)s-(uintptr_t)d-count <= -2*count) return memcpy(d, s, count);

	if (d<s) {
		for (; count; count--) *d++ = *s++;
	} else {
		while (count) count--, d[count] = s[count];
	}

	return dest;
}
#endif
