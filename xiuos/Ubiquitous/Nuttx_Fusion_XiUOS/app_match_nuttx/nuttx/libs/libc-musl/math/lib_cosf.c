/****************************************************************************
 * libs/libc-musl/math/lib_cosf.c
 *
 * This file is a part of NuttX:
 *
 *   Copyright (C) 2012 Gregory Nutt. All rights reserved.
 *   Ported by: Darcy Gong
 *
 * It derives from the Rhombus OS math library by Nick Johnson which has
 * a compatibile, MIT-style license:
 *
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 ****************************************************************************/

/**
* @file lib_cosf.c
* @brief nuttx source code
*        https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-08-04
*/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "libm.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* Small multiples of pi/2 rounded to double precision. */
static const double
c1pio2 = 1*M_PI_2, /* 0x3FF921FB, 0x54442D18 */
c2pio2 = 2*M_PI_2, /* 0x400921FB, 0x54442D18 */
c3pio2 = 3*M_PI_2, /* 0x4012D97C, 0x7F3321D2 */
c4pio2 = 4*M_PI_2; /* 0x401921FB, 0x54442D18 */

float cosf(float x)
{
	double y;
	uint32_t ix;
	unsigned n, sign;

	GET_FLOAT_WORD(ix, x);
	sign = ix >> 31;
	ix &= 0x7fffffff;

	if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
		if (ix < 0x39800000) {  /* |x| < 2**-12 */
			/* raise inexact if x != 0 */
			FORCE_EVAL(x + 0x1p120f);
			return 1.0f;
		}
		return __cosdf(x);
	}
	if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
		if (ix > 0x4016cbe3)  /* |x|  ~> 3*pi/4 */
			return -__cosdf(sign ? x+c2pio2 : x-c2pio2);
		else {
			if (sign)
				return __sindf(x + c1pio2);
			else
				return __sindf(c1pio2 - x);
		}
	}
	if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
		if (ix > 0x40afeddf)  /* |x| ~> 7*pi/4 */
			return __cosdf(sign ? x+c4pio2 : x-c4pio2);
		else {
			if (sign)
				return __sindf(-x - c3pio2);
			else
				return __sindf(x - c3pio2);
		}
	}

	/* cos(Inf or NaN) is NaN */
	if (ix >= 0x7f800000)
		return x-x;

	/* general argument reduction needed */
	n = __rem_pio2f(x,&y);
	switch (n&3) {
	case 0: return  __cosdf(y);
	case 1: return  __sindf(-y);
	case 2: return -__cosdf(y);
	default:
		return  __sindf(y);
	}
}

