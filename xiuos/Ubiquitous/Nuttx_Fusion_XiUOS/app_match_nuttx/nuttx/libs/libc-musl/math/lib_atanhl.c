/****************************************************************************
 * libs/libc-musl/math/lib_atanhl.c
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
* @file lib_atanhl.c
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
#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double atanhl(long double x)
{
	return atanh(x);
}
#elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113) && LDBL_MAX_EXP == 16384
/* atanh(x) = log((1+x)/(1-x))/2 = log1p(2x/(1-x))/2 ~= x + x^3/3 + o(x^5) */
long double atanhl(long double x)
{
	union ldshape u = {x};
	unsigned e = u.i.se & 0x7fff;
	unsigned s = u.i.se >> 15;

	/* |x| */
	u.i.se = e;
	x = u.f;

	if (e < 0x3ff - 1) {
		if (e < 0x3ff - LDBL_MANT_DIG/2) {
			/* handle underflow */
			if (e == 0)
				FORCE_EVAL((float)x);
		} else {
			/* |x| < 0.5, up to 1.7ulp error */
			x = 0.5*log1pl(2*x + 2*x*x/(1-x));
		}
	} else {
		/* avoid overflow */
		x = 0.5*log1pl(2*(x/(1-x)));
	}
	return s ? -x : x;
}
#endif
#endif
