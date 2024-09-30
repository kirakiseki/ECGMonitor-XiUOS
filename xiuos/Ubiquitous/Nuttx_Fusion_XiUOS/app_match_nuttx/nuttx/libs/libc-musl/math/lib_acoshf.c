/****************************************************************************
 * libs/libc-musl/math/lib_acoshf.c
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
* @file lib_acoshf.c
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
#include <nuttx/compiler.h>

#include "libm.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

float acoshf(float x)
{
	union {float f; uint32_t i;} u = {x};
	uint32_t a = u.i & 0x7fffffff;

	if (a < 0x3f800000+(1<<23))
		/* |x| < 2, invalid if x < 1 */
		/* up to 2ulp error in [1,1.125] */
		return log1pf(x-1 + sqrtf((x-1)*(x-1)+2*(x-1)));
	if (u.i < 0x3f800000+(12<<23))
		/* 2 <= x < 0x1p12 */
		return logf(2*x - 1/(x+sqrtf(x*x-1)));
	/* x >= 0x1p12 or x <= -2 or nan */
	return logf(x) + 0.693147180559945309417232121458176568f;
}
