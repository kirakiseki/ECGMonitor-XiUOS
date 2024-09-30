/****************************************************************************
 * libs/libc-musl/math/lib_asinf.c
 *
 * This file is a part of NuttX:
 *
 *   Copyright (C) 2012, 2016 Gregory Nutt. All rights reserved.
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
* @file lib_asinf.c
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
#include <float.h>

static const double
pio2 = 1.570796326794896558e+00;

static const float
/* coefficients for R(x^2) */
pS0 =  1.6666586697e-01,
pS1 = -4.2743422091e-02,
pS2 = -8.6563630030e-03,
qS1 = -7.0662963390e-01;

static float R(float z)
{
	float_t p, q;
	p = z*(pS0+z*(pS1+z*pS2));
	q = 1.0f+z*qS1;
	return p/q;
}

float asinf(float x)
{
	double s;
	float z;
	uint32_t hx,ix;

	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix >= 0x3f800000) {  /* |x| >= 1 */
		if (ix == 0x3f800000)  /* |x| == 1 */
			return x*pio2 + 0x1p-120f;  /* asin(+-1) = +-pi/2 with inexact */
		return 0/(x-x);  /* asin(|x|>1) is NaN */
	}
	if (ix < 0x3f000000) {  /* |x| < 0.5 */
		/* if 0x1p-126 <= |x| < 0x1p-12, avoid raising underflow */
		if (ix < 0x39800000 && ix >= 0x00800000)
			return x;
		return x + x*R(x*x);
	}
	/* 1 > |x| >= 0.5 */
	z = (1 - fabsf(x))*0.5f;
	s = sqrt(z);
	x = pio2 - 2*(s+s*R(z));
	if (hx >> 31)
		return -x;
	return x;
}
