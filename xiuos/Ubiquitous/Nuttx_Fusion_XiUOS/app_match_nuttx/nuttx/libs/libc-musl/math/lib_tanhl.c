/****************************************************************************
 * libs/libc-musl/math/lib_tanhl.c
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
* @file lib_tanhl.c
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
long double tanhl(long double x)
{
	return tanh(x);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
long double tanhl(long double x)
{
	union ldshape u = {x};
	unsigned ex = u.i.se & 0x7fff;
	unsigned sign = u.i.se & 0x8000;
	uint32_t w;
	long double t;

	/* x = |x| */
	u.i.se = ex;
	x = u.f;
	w = u.i.m >> 32;

	if (ex > 0x3ffe || (ex == 0x3ffe && w > 0x8c9f53d5)) {
		/* |x| > log(3)/2 ~= 0.5493 or nan */
		if (ex >= 0x3fff+5) {
			/* |x| >= 32 */
			t = 1 + 0/(x + 0x1p-120f);
		} else {
			t = expm1l(2*x);
			t = 1 - 2/(t+2);
		}
	} else if (ex > 0x3ffd || (ex == 0x3ffd && w > 0x82c577d4)) {
		/* |x| > log(5/3)/2 ~= 0.2554 */
		t = expm1l(2*x);
		t = t/(t+2);
	} else {
		/* |x| is small */
		t = expm1l(-2*x);
		t = -t/(t+2);
	}
	return sign ? -t : t;
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
// TODO: broken implementation to make things compile
long double tanhl(long double x)
{
	return tanh(x);
}
#endif

#endif
