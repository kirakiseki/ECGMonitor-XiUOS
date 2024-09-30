/****************************************************************************
 * libs/libc-musl/string/lib_strlcat.c
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
* @file lib_strlcat.c
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
 * Name: strlcat
 *
 * Description:
 *   The strlcat() function appends at most (dstsize-strlen(dst)-1)
 *   characters of src to dst (dstsize being the size of the string buffer
 *   dst).
 *
 * Returned Value:
 *   Returns min{dstsize,strlen(dst)} + strlen(src).
 *
 ****************************************************************************/

#ifndef CONFIG_LIBC_ARCH_STRLCAT
size_t strlcat(FAR char *dst, FAR const char *src, size_t dsize)
{
	size_t l = strnlen(dst, dsize);
	if (l == dsize) return l + strlen(src);
	return l + strlcpy(dst+l, src, dsize-l);
}
#endif
