/****************************************************************************
 * libs/libc-musl/math/lib_expf.c
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
* @file lib_expf.c
* @brief nuttx source code
*        https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-08-04
*/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <sys/types.h>
#include "libm.h"

#include "libc-musl.h"

#define N (1 << EXP2F_TABLE_BITS)
#define InvLn2N __exp2f_data.invln2_scaled
#define T __exp2f_data.tab
#define C __exp2f_data.poly_scaled

static inline uint32_t top12(float x)
{
	return asuint(x) >> 20;
}

float expf(float x)
{
	uint32_t abstop;
	uint64_t ki, t;
	double_t kd, xd, z, r, r2, y, s;

	xd = (double_t)x;
	abstop = top12(x) & 0x7ff;
	if (predict_false(abstop >= top12(88.0f))) {
		/* |x| >= 88 or x is nan.  */
		if (asuint(x) == asuint(-INFINITY))
			return 0.0f;
		if (abstop >= top12(INFINITY))
			return x + x;
		if (x > 0x1.62e42ep6f) /* x > log(0x1p128) ~= 88.72 */
			return __math_oflowf(0);
		if (x < -0x1.9fe368p6f) /* x < log(0x1p-150) ~= -103.97 */
			return __math_uflowf(0);
	}

	/* x*N/Ln2 = k + r with r in [-1/2, 1/2] and int k.  */
	z = InvLn2N * xd;

	/* Round and convert z to int, the result is in [-150*N, 128*N] and
	   ideally ties-to-even rule is used, otherwise the magnitude of r
	   can be bigger which gives larger approximation error.  */
#if TOINT_INTRINSICS
	kd = roundtoint(z);
	ki = converttoint(z);
#else
# define SHIFT __exp2f_data.shift
	kd = eval_as_double(z + SHIFT);
	ki = asuint64(kd);
	kd -= SHIFT;
#endif
	r = z - kd;

	/* exp(x) = 2^(k/N) * 2^(r/N) ~= s * (C0*r^3 + C1*r^2 + C2*r + 1) */
	t = T[ki % N];
	t += ki << (52 - EXP2F_TABLE_BITS);
	s = asdouble(t);
	z = C[0] * r + C[1];
	r2 = r * r;
	y = C[2] * r + 1;
	y = z * r2 + y;
	y = y * s;
	return eval_as_float(y);
}
