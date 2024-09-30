/****************************************************************************
 * libs/libc-musl/math/lib_tanh.c
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
* @file lib_tanh.c
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

double tanh(double x)
{
	union {double f; uint64_t i;} u = {.f = x};
	uint32_t w;
	int sign;
	double_t t;

	/* x = |x| */
	sign = u.i >> 63;
	u.i &= (uint64_t)-1/2;
	x = u.f;
	w = u.i >> 32;

	if (w > 0x3fe193ea) {
		/* |x| > log(3)/2 ~= 0.5493 or nan */
		if (w > 0x40340000) {
			/* |x| > 20 or nan */
			/* note: this branch avoids raising overflow */
			t = 1 - 0/x;
		} else {
			t = expm1(2*x);
			t = 1 - 2/(t+2);
		}
	} else if (w > 0x3fd058ae) {
		/* |x| > log(5/3)/2 ~= 0.2554 */
		t = expm1(2*x);
		t = t/(t+2);
	} else if (w >= 0x00100000) {
		/* |x| >= 0x1p-1022, up to 2ulp error in [0.1,0.2554] */
		t = expm1(-2*x);
		t = -t/(t+2);
	} else {
		/* |x| is subnormal */
		/* note: the branch above would not raise underflow in [0x1p-1023,0x1p-1022) */
		FORCE_EVAL((float)x);
		t = x;
	}
	return sign ? -t : t;
}


#endif
