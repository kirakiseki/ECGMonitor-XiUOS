/****************************************************************************
 * libs/libc-musl/stdlib/lib_atoi.c
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
* @file lib_atoi.c
* @brief nuttx source code
*        https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-08-04
*/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdlib.h>
#include <ctype.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#undef atoi /* See mm/README.txt */
int atoi(FAR const char *nptr)
{
	int n=0, neg=0;
	while (isspace(*nptr)) nptr++;
	switch (*nptr) {
	case '-': neg=1;
	case '+': nptr++;
	}
	/* Compute n as a negative number to avoid overflow on INT_MIN */
	while (isdigit(*nptr))
		n = 10*n - (*nptr++ - '0');
	return neg ? n : -n;
}
