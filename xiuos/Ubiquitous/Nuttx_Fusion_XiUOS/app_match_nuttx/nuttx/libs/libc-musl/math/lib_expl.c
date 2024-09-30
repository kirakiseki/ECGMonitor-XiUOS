/****************************************************************************
 * libs/libc-musl/math/lib_expl.c
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
* @file lib_expl.c
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

#include <sys/types.h>
#include "libm.h"

#include "libc-musl.h"

#ifdef CONFIG_HAVE_LONG_DOUBLE

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double expl(long double x)
{
	return exp(x);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384

static const long double P[3] = {
 1.2617719307481059087798E-4L,
 3.0299440770744196129956E-2L,
 9.9999999999999999991025E-1L,
};
static const long double Q[4] = {
 3.0019850513866445504159E-6L,
 2.5244834034968410419224E-3L,
 2.2726554820815502876593E-1L,
 2.0000000000000000000897E0L,
};
static const long double
LN2HI = 6.9314575195312500000000E-1L,
LN2LO = 1.4286068203094172321215E-6L,
LOG2E = 1.4426950408889634073599E0L;

long double expl(long double x)
{
	long double px, xx;
	int k;

	if (isnan(x))
		return x;
	if (x > 11356.5234062941439488L) /* x > ln(2^16384 - 0.5) */
		return x * 0x1p16383L;
	if (x < -11399.4985314888605581L) /* x < ln(2^-16446) */
		return -0x1p-16445L/x;

	/* Express e**x = e**f 2**k
	 *   = e**(f + k ln(2))
	 */
	px = floorl(LOG2E * x + 0.5);
	k = px;
	x -= px * LN2HI;
	x -= px * LN2LO;

	/* rational approximation of the fractional part:
	 * e**x =  1 + 2x P(x**2)/(Q(x**2) - x P(x**2))
	 */
	xx = x * x;
	px = x * __polevll(xx, P, 2);
	x = px/(__polevll(xx, Q, 3) - px);
	x = 1.0 + 2.0 * x;
	return scalbnl(x, k);
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
// TODO: broken implementation to make things compile
long double expl(long double x)
{
	return exp(x);
}
#endif


#endif
