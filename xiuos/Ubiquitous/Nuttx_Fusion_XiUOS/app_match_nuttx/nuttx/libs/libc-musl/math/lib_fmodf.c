/****************************************************************************
 * libs/libc-musl/math/lib_fmodf.c
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
* @file lib_fmodf.c
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

float fmodf(float x, float y)
{
	union {float f; uint32_t i;} ux = {x}, uy = {y};
	int ex = ux.i>>23 & 0xff;
	int ey = uy.i>>23 & 0xff;
	uint32_t sx = ux.i & 0x80000000;
	uint32_t i;
	uint32_t uxi = ux.i;

	if (uy.i<<1 == 0 || isnan(y) || ex == 0xff)
		return (x*y)/(x*y);
	if (uxi<<1 <= uy.i<<1) {
		if (uxi<<1 == uy.i<<1)
			return 0*x;
		return x;
	}

	/* normalize x and y */
	if (!ex) {
		for (i = uxi<<9; i>>31 == 0; ex--, i <<= 1);
		uxi <<= -ex + 1;
	} else {
		uxi &= -1U >> 9;
		uxi |= 1U << 23;
	}
	if (!ey) {
		for (i = uy.i<<9; i>>31 == 0; ey--, i <<= 1);
		uy.i <<= -ey + 1;
	} else {
		uy.i &= -1U >> 9;
		uy.i |= 1U << 23;
	}

	/* x mod y */
	for (; ex > ey; ex--) {
		i = uxi - uy.i;
		if (i >> 31 == 0) {
			if (i == 0)
				return 0*x;
			uxi = i;
		}
		uxi <<= 1;
	}
	i = uxi - uy.i;
	if (i >> 31 == 0) {
		if (i == 0)
			return 0*x;
		uxi = i;
	}
	for (; uxi>>23 == 0; uxi <<= 1, ex--);

	/* scale result up */
	if (ex > 0) {
		uxi -= 1U << 23;
		uxi |= (uint32_t)ex << 23;
	} else {
		uxi >>= -ex + 1;
	}
	uxi |= sx;
	ux.i = uxi;
	return ux.f;
}

