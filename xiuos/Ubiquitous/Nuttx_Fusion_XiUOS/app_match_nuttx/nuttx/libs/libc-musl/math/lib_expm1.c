/****************************************************************************
 * libs/libc-musl/math/lib_expm1.c
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
* @file lib_expm1.c
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
static const double
o_threshold = 7.09782712893383973096e+02, /* 0x40862E42, 0xFEFA39EF */
ln2_hi      = 6.93147180369123816490e-01, /* 0x3fe62e42, 0xfee00000 */
ln2_lo      = 1.90821492927058770002e-10, /* 0x3dea39ef, 0x35793c76 */
invln2      = 1.44269504088896338700e+00, /* 0x3ff71547, 0x652b82fe */
/* Scaled Q's: Qn_here = 2**n * Qn_above, for R(2*z) where z = hxs = x*x/2: */
Q1 = -3.33333333333331316428e-02, /* BFA11111 111110F4 */
Q2 =  1.58730158725481460165e-03, /* 3F5A01A0 19FE5585 */
Q3 = -7.93650757867487942473e-05, /* BF14CE19 9EAADBB7 */
Q4 =  4.00821782732936239552e-06, /* 3ED0CFCA 86E65239 */
Q5 = -2.01099218183624371326e-07; /* BE8AFDB7 6E09C32D */

double expm1(double x)
{
	double_t y,hi,lo,c,t,e,hxs,hfx,r1,twopk;
	union {double f; uint64_t i;} u = {x};
	uint32_t hx = u.i>>32 & 0x7fffffff;
	int k, sign = u.i>>63;

	/* filter out huge and non-finite argument */
	if (hx >= 0x4043687A) {  /* if |x|>=56*ln2 */
		if (isnan(x))
			return x;
		if (sign)
			return -1;
		if (x > o_threshold) {
			x *= 0x1p1023;
			return x;
		}
	}

	/* argument reduction */
	if (hx > 0x3fd62e42) {  /* if  |x| > 0.5 ln2 */
		if (hx < 0x3FF0A2B2) {  /* and |x| < 1.5 ln2 */
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
			k  = invln2*x + (sign ? -0.5 : 0.5);
			t  = k;
			hi = x - t*ln2_hi;  /* t*ln2_hi is exact here */
			lo = t*ln2_lo;
		}
		x = hi-lo;
		c = (hi-x)-lo;
	} else if (hx < 0x3c900000) {  /* |x| < 2**-54, return x */
		if (hx < 0x00100000)
			FORCE_EVAL((float)x);
		return x;
	} else
		k = 0;

	/* x is now in primary range */
	hfx = 0.5*x;
	hxs = x*hfx;
	r1 = 1.0+hxs*(Q1+hxs*(Q2+hxs*(Q3+hxs*(Q4+hxs*Q5))));
	t  = 3.0-r1*hfx;
	e  = hxs*((r1-t)/(6.0 - x*t));
	if (k == 0)   /* c is 0 */
		return x - (x*e-hxs);
	e  = x*(e-c) - c;
	e -= hxs;
	/* exp(x) ~ 2^k (x_reduced - e + 1) */
	if (k == -1)
		return 0.5*(x-e) - 0.5;
	if (k == 1) {
		if (x < -0.25)
			return -2.0*(e-(x+0.5));
		return 1.0+2.0*(x-e);
	}
	u.i = (uint64_t)(0x3ff + k)<<52;  /* 2^k */
	twopk = u.f;
	if (k < 0 || k > 56) {  /* suffice to return exp(x)-1 */
		y = x - e + 1.0;
		if (k == 1024)
			y = y*2.0*0x1p1023;
		else
			y = y*twopk;
		return y - 1.0;
	}
	u.i = (uint64_t)(0x3ff - k)<<52;  /* 2^-k */
	if (k < 20)
		y = (x-e+(1-u.f))*twopk;
	else
		y = (x-(e+u.f)+1)*twopk;
	return y;
}

#endif
