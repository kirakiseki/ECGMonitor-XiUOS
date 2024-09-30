/****************************************************************************
 * libs/libc-musl/math/lib_cosh.c
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
* @file lib_cosh.c
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
double cosh(double x)
{
	union {double f; uint64_t i;} u = {.f = x};
	uint32_t w;
	double t;

	/* |x| */
	u.i &= (uint64_t)-1/2;
	x = u.f;
	w = u.i >> 32;

	/* |x| < log(2) */
	if (w < 0x3fe62e42) {
		if (w < 0x3ff00000 - (26<<20)) {
			/* raise inexact if x!=0 */
			FORCE_EVAL(x + 0x1p120f);
			return 1;
		}
		t = expm1(x);
		return 1 + t*t/(2*(1+t));
	}

	/* |x| < log(DBL_MAX) */
	if (w < 0x40862e42) {
		t = exp(x);
		/* note: if x>log(0x1p26) then the 1/t is not needed */
		return 0.5*(t + 1/t);
	}

	/* |x| > log(DBL_MAX) or nan */
	/* note: the result is stored to handle overflow */
	t = __expo2(x, 1.0);
	return t;
}
#endif
