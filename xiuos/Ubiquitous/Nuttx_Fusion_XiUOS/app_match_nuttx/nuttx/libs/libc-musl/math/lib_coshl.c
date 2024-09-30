/****************************************************************************
 * libs/libc-musl/math/lib_coshl.c
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
* @file lib_coshl.c
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
long double coshl(long double x)
{
	return cosh(x);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
long double coshl(long double x)
{
	union ldshape u = {x};
	unsigned ex = u.i.se & 0x7fff;
	uint32_t w;
	long double t;

	/* |x| */
	u.i.se = ex;
	x = u.f;
	w = u.i.m >> 32;

	/* |x| < log(2) */
	if (ex < 0x3fff-1 || (ex == 0x3fff-1 && w < 0xb17217f7)) {
		if (ex < 0x3fff-32) {
			FORCE_EVAL(x + 0x1p120f);
			return 1;
		}
		t = expm1l(x);
		return 1 + t*t/(2*(1+t));
	}

	/* |x| < log(LDBL_MAX) */
	if (ex < 0x3fff+13 || (ex == 0x3fff+13 && w < 0xb17217f7)) {
		t = expl(x);
		return 0.5*(t + 1/t);
	}

	/* |x| > log(LDBL_MAX) or nan */
	t = expl(0.5*x);
	return 0.5*t*t;
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
// TODO: broken implementation to make things compile
long double coshl(long double x)
{
	return cosh(x);
}
#endif
#endif
