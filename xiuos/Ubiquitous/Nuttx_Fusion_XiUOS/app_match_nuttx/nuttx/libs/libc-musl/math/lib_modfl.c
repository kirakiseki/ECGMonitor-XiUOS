/****************************************************************************
 * libs/libc-musl/math/lib_modfl.c
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
* @file lib_modfl.c
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

#include <stdint.h>
#include "libm.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef CONFIG_HAVE_LONG_DOUBLE
#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double modfl(long double x, long double *iptr)
{
	double d;
	long double r;

	r = modf(x, &d);
	*iptr = d;
	return r;
}
#elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113) && LDBL_MAX_EXP == 16384

static const long double toint = 1/LDBL_EPSILON;

long double modfl(long double x, long double *iptr)
{
	union ldshape u = {x};
	int e = (u.i.se & 0x7fff) - 0x3fff;
	int s = u.i.se >> 15;
	long double absx;
	long double y;

	/* no fractional part */
	if (e >= LDBL_MANT_DIG-1) {
		*iptr = x;
		if (isnan(x))
			return x;
		return s ? -0.0 : 0.0;
	}

	/* no integral part*/
	if (e < 0) {
		*iptr = s ? -0.0 : 0.0;
		return x;
	}

	/* raises spurious inexact */
	absx = s ? -x : x;
	y = absx + toint - toint - absx;
	if (y == 0) {
		*iptr = x;
		return s ? -0.0 : 0.0;
	}
	if (y > 0)
		y -= 1;
	if (s)
		y = -y;
	*iptr = x + y;
	return -y;
}
#endif

#endif
