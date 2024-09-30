/****************************************************************************
 * libs/libc-musl/math/lib_sinhl.c
 *
 * This file is a part of NuttX:
 *
 *   Copyright (C) 2012 Gregory Nutt. All rights reserved.
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
* @file lib_sinhl.c
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
long double sinhl(long double x)
{
	return sinh(x);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
long double sinhl(long double x)
{
	union ldshape u = {x};
	unsigned ex = u.i.se & 0x7fff;
	long double h, t, absx;

	h = 0.5;
	if (u.i.se & 0x8000)
		h = -h;
	/* |x| */
	u.i.se = ex;
	absx = u.f;

	/* |x| < log(LDBL_MAX) */
	if (ex < 0x3fff+13 || (ex == 0x3fff+13 && u.i.m>>32 < 0xb17217f7)) {
		t = expm1l(absx);
		if (ex < 0x3fff) {
			if (ex < 0x3fff-32)
				return x;
			return h*(2*t - t*t/(1+t));
		}
		return h*(t + t/(t+1));
	}

	/* |x| > log(LDBL_MAX) or nan */
	t = expl(0.5*absx);
	return h*t*t;
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
// TODO: broken implementation to make things compile
long double sinhl(long double x)
{
	return sinh(x);
}
#endif
#endif
