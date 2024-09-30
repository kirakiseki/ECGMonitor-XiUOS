/****************************************************************************
 * libs/libc-musl/math/lib_asinhl.c
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
* @file lib_asinhl.c
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

#ifdef CONFIG_HAVE_LONG_DOUBLE

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double asinhl(long double x)
{
	return asinh(x);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
/* asinh(x) = sign(x)*log(|x|+sqrt(x*x+1)) ~= x - x^3/6 + o(x^5) */
long double asinhl(long double x)
{
	union ldshape u = {x};
	unsigned e = u.i.se & 0x7fff;
	unsigned s = u.i.se >> 15;

	/* |x| */
	u.i.se = e;
	x = u.f;

	if (e >= 0x3fff + 32) {
		/* |x| >= 0x1p32 or inf or nan */
		x = logl(x) + 0.693147180559945309417232121458176568L;
	} else if (e >= 0x3fff + 1) {
		/* |x| >= 2 */
		x = logl(2*x + 1/(sqrtl(x*x+1)+x));
	} else if (e >= 0x3fff - 32) {
		/* |x| >= 0x1p-32 */
		x = log1pl(x + x*x/(sqrtl(x*x+1)+1));
	} else {
		/* |x| < 0x1p-32, raise inexact if x!=0 */
		FORCE_EVAL(x + 0x1p120f);
	}
	return s ? -x : x;
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
// TODO: broken implementation to make things compile
long double asinhl(long double x)
{
	return asinh(x);
}
#endif

#endif
