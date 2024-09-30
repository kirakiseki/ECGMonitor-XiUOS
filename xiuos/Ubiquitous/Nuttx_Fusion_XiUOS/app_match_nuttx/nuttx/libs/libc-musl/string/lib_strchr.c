/****************************************************************************
 * libs/libc-musl/string/lib_strchr.c
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
* @file lib_strchr.c
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
 * Name: strchr
 *
 * Description:
 *   The strchr() function locates the first occurrence of 'c' (converted to
 *   a char) in the string pointed to by 's'. The terminating null byte is
 *   considered to be part of the string.
 *
 * Returned Value:
 *   Upon completion, strchr() returns a pointer to the byte, or a null
 *   pointer if the byte was not found.
 *
 ****************************************************************************/

#ifndef CONFIG_LIBC_ARCH_STRCHR
#undef strchr /* See mm/README.txt */
FAR char *strchr(FAR const char *s, int c)
{
	char *r = strchrnul(s, c);
	return *(unsigned char *)r == (unsigned char)c ? r : NULL;
}
#endif
