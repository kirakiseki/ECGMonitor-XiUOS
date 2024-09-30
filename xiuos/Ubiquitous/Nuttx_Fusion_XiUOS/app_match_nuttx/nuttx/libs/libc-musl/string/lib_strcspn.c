/****************************************************************************
 * libs/libc-musl/string/lib_strcspn.c
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
* @file lib_strcspn.c
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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: strcspn
 *
 * Description:
 *  strcspn() calculates the length of the initial segment of s which
 *  consists entirely of characters not in reject.
 *
 ****************************************************************************/

#define BITOP(a,b,op) \
 ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))

#undef strcspn /* See mm/README.txt */
size_t strcspn(const char *s, const char *reject)
{
	const char *a = s;
	size_t byteset[32/sizeof(size_t)];

	if (!reject[0] || !reject[1]) return strchrnul(s, *reject) - a;

	memset(byteset, 0, sizeof(byteset));
	for (; *reject && BITOP(byteset, *(unsigned char *)reject, |=); reject++);
	for (; *s && !BITOP(byteset, *(unsigned char *)s, &); s++);
	return s - a;
}
