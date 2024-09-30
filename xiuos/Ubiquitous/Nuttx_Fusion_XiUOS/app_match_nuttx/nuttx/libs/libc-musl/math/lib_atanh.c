/****************************************************************************
 * libs/libc-musl/math/lib_atanh.c
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
* @file lib_atanh.c
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

#ifdef CONFIG_HAVE_DOUBLE

double atanh(double x)
{
	union {double f; uint64_t i;} u = {.f = x};
	unsigned e = u.i >> 52 & 0x7ff;
	unsigned s = u.i >> 63;
	double_t y;

	/* |x| */
	u.i &= (uint64_t)-1/2;
	y = u.f;

	if (e < 0x3ff - 1) {
		if (e < 0x3ff - 32) {
			/* handle underflow */
			if (e == 0)
				FORCE_EVAL((float)y);
		} else {
			/* |x| < 0.5, up to 1.7ulp error */
			y = 0.5*log1p(2*y + 2*y*y/(1-y));
		}
	} else {
		/* avoid overflow */
		y = 0.5*log1p(2*(y/(1-y)));
	}
	return s ? -y : y;
}

#endif
