/****************************************************************************
 * libs/libc-musl/math/lib_asinh.c
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
* @file lib_asinh.c
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
double asinh(double x)
{
	union {double f; uint64_t i;} u = {.f = x};
	unsigned e = u.i >> 52 & 0x7ff;
	unsigned s = u.i >> 63;

	/* |x| */
	u.i &= (uint64_t)-1/2;
	x = u.f;

	if (e >= 0x3ff + 26) {
		/* |x| >= 0x1p26 or inf or nan */
		x = log(x) + 0.693147180559945309417232121458176568;
	} else if (e >= 0x3ff + 1) {
		/* |x| >= 2 */
		x = log(2*x + 1/(sqrt(x*x+1)+x));
	} else if (e >= 0x3ff - 26) {
		/* |x| >= 0x1p-26, up to 1.6ulp error in [0.125,0.5] */
		x = log1p(x + x*x/(sqrt(x*x+1)+1));
	} else {
		/* |x| < 0x1p-26, raise inexact if x != 0 */
		FORCE_EVAL(x + 0x1p120f);
	}
	return s ? -x : x;
}
#endif
