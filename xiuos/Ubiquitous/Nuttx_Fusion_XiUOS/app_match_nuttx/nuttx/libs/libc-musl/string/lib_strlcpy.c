/****************************************************************************
 * libs/libc-musl/string/lib_strlcpy.c
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
* @file lib_strlcpy.c
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
 * Name: strlcpy
 *
 * Description:
 *   Copy src to string dst of size dsize.  At most dsize-1 characters
 *   will be copied.  Always NUL terminates (unless dsize == 0).
 *
 * Returned Value:
 *   Returns strlen(src); if retval >= dsize, truncation occurred.
 *
 ****************************************************************************/

#ifndef CONFIG_LIBC_ARCH_STRLCPY
size_t strlcpy(FAR char *dst, FAR const char *src, size_t dsize)
{
	char *d0 = dst;
	if (!dsize--) goto finish;
	for (; dsize && (*dst=*src); dsize--, src++, dst++);
	*dst = 0;
finish:
	return dst-d0 + strlen(src);
}
#endif
