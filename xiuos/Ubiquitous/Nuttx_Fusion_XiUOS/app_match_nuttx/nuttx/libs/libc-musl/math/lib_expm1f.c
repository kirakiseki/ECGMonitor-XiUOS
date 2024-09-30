/****************************************************************************
 * libs/libc-musl/math/lib_expm1f.c
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
* @file lib_expm1f.c
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

static const float
ln2_hi      = 6.9313812256e-01, /* 0x3f317180 */
ln2_lo      = 9.0580006145e-06, /* 0x3717f7d1 */
invln2      = 1.4426950216e+00, /* 0x3fb8aa3b */
/*
 * Domain [-0.34568, 0.34568], range ~[-6.694e-10, 6.696e-10]:
 * |6 / x * (1 + 2 * (1 / (exp(x) - 1) - 1 / x)) - q(x)| < 2**-30.04
 * Scaled coefficients: Qn_here = 2**n * Qn_for_q (see s_expm1.c):
 */
Q1 = -3.3333212137e-2, /* -0x888868.0p-28 */
Q2 =  1.5807170421e-3; /*  0xcf3010.0p-33 */

float expm1f(float x)
{
	float_t y,hi,lo,c,t,e,hxs,hfx,r1,twopk;
	union {float f; uint32_t i;} u = {x};
	uint32_t hx = u.i & 0x7fffffff;
	int k, sign = u.i >> 31;

	/* filter out huge and non-finite argument */
	if (hx >= 0x4195b844) {  /* if |x|>=27*ln2 */
		if (hx > 0x7f800000)  /* NaN */
			return x;
		if (sign)
			return -1;
		if (hx > 0x42b17217) { /* x > log(FLT_MAX) */
			x *= 0x1p127f;
			return x;
		}
	}

	/* argument reduction */
	if (hx > 0x3eb17218) {           /* if  |x| > 0.5 ln2 */
		if (hx < 0x3F851592) {       /* and |x| < 1.5 ln2 */
			if (!sign) {
				hi = x - ln2_hi;
				lo = ln2_lo;
				k =  1;
			} else {
				hi = x + ln2_hi;
				lo = -ln2_lo;
				k = -1;
			}
		} else {
			k  = invln2*x + (sign ? -0.5f : 0.5f);
			t  = k;
			hi = x - t*ln2_hi;      /* t*ln2_hi is exact here */
			lo = t*ln2_lo;
		}
		x = hi-lo;
		c = (hi-x)-lo;
	} else if (hx < 0x33000000) {  /* when |x|<2**-25, return x */
		if (hx < 0x00800000)
			FORCE_EVAL(x*x);
		return x;
	} else
		k = 0;

	/* x is now in primary range */
	hfx = 0.5f*x;
	hxs = x*hfx;
	r1 = 1.0f+hxs*(Q1+hxs*Q2);
	t  = 3.0f - r1*hfx;
	e  = hxs*((r1-t)/(6.0f - x*t));
	if (k == 0)  /* c is 0 */
		return x - (x*e-hxs);
	e  = x*(e-c) - c;
	e -= hxs;
	/* exp(x) ~ 2^k (x_reduced - e + 1) */
	if (k == -1)
		return 0.5f*(x-e) - 0.5f;
	if (k == 1) {
		if (x < -0.25f)
			return -2.0f*(e-(x+0.5f));
		return 1.0f + 2.0f*(x-e);
	}
	u.i = (0x7f+k)<<23;  /* 2^k */
	twopk = u.f;
	if (k < 0 || k > 56) {   /* suffice to return exp(x)-1 */
		y = x - e + 1.0f;
		if (k == 128)
			y = y*2.0f*0x1p127f;
		else
			y = y*twopk;
		return y - 1.0f;
	}
	u.i = (0x7f-k)<<23;  /* 2^-k */
	if (k < 23)
		y = (x-e+(1-u.f))*twopk;
	else
		y = (x-(e+u.f)+1)*twopk;
	return y;
}

