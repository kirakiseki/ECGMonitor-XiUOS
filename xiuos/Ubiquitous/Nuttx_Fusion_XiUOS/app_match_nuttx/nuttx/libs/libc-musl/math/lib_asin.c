/****************************************************************************
 * libs/libc-musl/math/lib_asin.c
 *
 * This file is a part of NuttX:
 *
 *   Copyright (C) 2012, 2015-2016 Gregory Nutt. All rights reserved.
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
* @file lib_asin.c
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
#include <float.h>

#ifdef CONFIG_HAVE_DOUBLE

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

static const double
pio2_hi = 1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
pio2_lo = 6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
/* coefficients for R(x^2) */
pS0 =  1.66666666666666657415e-01, /* 0x3FC55555, 0x55555555 */
pS1 = -3.25565818622400915405e-01, /* 0xBFD4D612, 0x03EB6F7D */
pS2 =  2.01212532134862925881e-01, /* 0x3FC9C155, 0x0E884455 */
pS3 = -4.00555345006794114027e-02, /* 0xBFA48228, 0xB5688F3B */
pS4 =  7.91534994289814532176e-04, /* 0x3F49EFE0, 0x7501B288 */
pS5 =  3.47933107596021167570e-05, /* 0x3F023DE1, 0x0DFDF709 */
qS1 = -2.40339491173441421878e+00, /* 0xC0033A27, 0x1C8A2D4B */
qS2 =  2.02094576023350569471e+00, /* 0x40002AE5, 0x9C598AC8 */
qS3 = -6.88283971605453293030e-01, /* 0xBFE6066C, 0x1B8D0159 */
qS4 =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

static double R(double z)
{
	double_t p, q;
	p = z*(pS0+z*(pS1+z*(pS2+z*(pS3+z*(pS4+z*pS5)))));
	q = 1.0+z*(qS1+z*(qS2+z*(qS3+z*qS4)));
	return p/q;
}

double asin(double x)
{
	double z,r,s;
	uint32_t hx,ix;

	GET_HIGH_WORD(hx, x);
	ix = hx & 0x7fffffff;
	/* |x| >= 1 or nan */
	if (ix >= 0x3ff00000) {
		uint32_t lx;
		GET_LOW_WORD(lx, x);
		if (((ix - 0x3ff00000) | lx) == 0)
			/* asin(1) = +-pi/2 with inexact */
			return x*pio2_hi + 0x1p-120f;
		return 0/(x-x);
	}
	/* |x| < 0.5 */
	if (ix < 0x3fe00000) {
		/* if 0x1p-1022 <= |x| < 0x1p-26, avoid raising underflow */
		if (ix < 0x3e500000 && ix >= 0x00100000)
			return x;
		return x + x*R(x*x);
	}
	/* 1 > |x| >= 0.5 */
	z = (1 - fabs(x))*0.5;
	s = sqrt(z);
	r = R(z);
	if (ix >= 0x3fef3333) {  /* if |x| > 0.975 */
		x = pio2_hi-(2*(s+s*r)-pio2_lo);
	} else {
		double f,c;
		/* f+c = sqrt(z) */
		f = s;
		SET_LOW_WORD(f,0);
		c = (z-f*f)/(s+f);
		x = 0.5*pio2_hi - (2*s*r - (pio2_lo-2*c) - (0.5*pio2_hi-2*f));
	}
	if (hx >> 31)
		return -x;
	return x;
}

#endif /* CONFIG_HAVE_DOUBLE */
