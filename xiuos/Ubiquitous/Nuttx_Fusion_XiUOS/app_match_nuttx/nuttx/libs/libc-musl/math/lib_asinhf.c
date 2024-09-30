/****************************************************************************
 * libs/libc-musl/math/lib_asinhf.c
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
* @file lib_asinhf.c
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

float asinhf(float x)
{
	union {float f; uint32_t i;} u = {.f = x};
	uint32_t i = u.i & 0x7fffffff;
	unsigned s = u.i >> 31;

	/* |x| */
	u.i = i;
	x = u.f;

	if (i >= 0x3f800000 + (12<<23)) {
		/* |x| >= 0x1p12 or inf or nan */
		x = logf(x) + 0.693147180559945309417232121458176568f;
	} else if (i >= 0x3f800000 + (1<<23)) {
		/* |x| >= 2 */
		x = logf(2*x + 1/(sqrtf(x*x+1)+x));
	} else if (i >= 0x3f800000 - (12<<23)) {
		/* |x| >= 0x1p-12, up to 1.6ulp error in [0.125,0.5] */
		x = log1pf(x + x*x/(sqrtf(x*x+1)+1));
	} else {
		/* |x| < 0x1p-12, raise inexact if x!=0 */
		FORCE_EVAL(x + 0x1p120f);
	}
	return s ? -x : x;
}
