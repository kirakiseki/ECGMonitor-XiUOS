/****************************************************************************
 * libs/libc-musl/math/lib_coshf.c
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
* @file lib_coshf.c
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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

float coshf(float x)
{
	union {float f; uint32_t i;} u = {.f = x};
	uint32_t w;
	float t;

	/* |x| */
	u.i &= 0x7fffffff;
	x = u.f;
	w = u.i;

	/* |x| < log(2) */
	if (w < 0x3f317217) {
		if (w < 0x3f800000 - (12<<23)) {
			FORCE_EVAL(x + 0x1p120f);
			return 1;
		}
		t = expm1f(x);
		return 1 + t*t/(2*(1+t));
	}

	/* |x| < log(FLT_MAX) */
	if (w < 0x42b17217) {
		t = expf(x);
		return 0.5f*(t + 1/t);
	}

	/* |x| > log(FLT_MAX) or nan */
	t = __expo2f(x, 1.0f);
	return t;
}