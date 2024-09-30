/****************************************************************************
 * libs/libc-musl/math/lib_sinh.c
 *
 * This file is a part of NuttX:
 *
 *   Copyright (C) 2012, 2015 Gregory Nutt. All rights reserved.
 *   Ported by: Darcy Gong
 *
 * It derives from the Rhombus OS math library by Nick Johnson which has
 * a compatibile, MIT-style license:
 *
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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
* @file lib_sinh.c
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
double sinh(double x)
{
	union {double f; uint64_t i;} u = {.f = x};
	uint32_t w;
	double t, h, absx;

	h = 0.5;
	if (u.i >> 63)
		h = -h;
	/* |x| */
	u.i &= (uint64_t)-1/2;
	absx = u.f;
	w = u.i >> 32;

	/* |x| < log(DBL_MAX) */
	if (w < 0x40862e42) {
		t = expm1(absx);
		if (w < 0x3ff00000) {
			if (w < 0x3ff00000 - (26<<20))
				/* note: inexact and underflow are raised by expm1 */
				/* note: this branch avoids spurious underflow */
				return x;
			return h*(2*t - t*t/(t+1));
		}
		/* note: |x|>log(0x1p26)+eps could be just h*exp(x) */
		return h*(t + t/(t+1));
	}

	/* |x| > log(DBL_MAX) or nan */
	/* note: the result is stored to handle overflow */
	t = __expo2(absx, 2*h);
	return t;
}
#endif
