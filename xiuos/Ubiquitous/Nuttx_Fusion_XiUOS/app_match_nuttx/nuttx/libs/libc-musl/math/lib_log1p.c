/**
 * @file lib_log1p.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include "libm.h"

static const double
ln2_hi = 6.93147180369123816490e-01,  /* 3fe62e42 fee00000 */
ln2_lo = 1.90821492927058770002e-10,  /* 3dea39ef 35793c76 */
Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

double log1p(double x)
{
	union {double f; uint64_t i;} u = {x};
	double_t hfsq,f,c,s,z,R,w,t1,t2,dk;
	uint32_t hx,hu;
	int k;

	hx = u.i>>32;
	k = 1;
	if (hx < 0x3fda827a || hx>>31) {  /* 1+x < sqrt(2)+ */
		if (hx >= 0xbff00000) {  /* x <= -1.0 */
			if (x == -1)
				return x/0.0; /* log1p(-1) = -inf */
			return (x-x)/0.0;     /* log1p(x<-1) = NaN */
		}
		if (hx<<1 < 0x3ca00000<<1) {  /* |x| < 2**-53 */
			/* underflow if subnormal */
			if ((hx&0x7ff00000) == 0)
				FORCE_EVAL((float)x);
			return x;
		}
		if (hx <= 0xbfd2bec4) {  /* sqrt(2)/2- <= 1+x < sqrt(2)+ */
			k = 0;
			c = 0;
			f = x;
		}
	} else if (hx >= 0x7ff00000)
		return x;
	if (k) {
		u.f = 1 + x;
		hu = u.i>>32;
		hu += 0x3ff00000 - 0x3fe6a09e;
		k = (int)(hu>>20) - 0x3ff;
		/* correction term ~ log(1+x)-log(u), avoid underflow in c/u */
		if (k < 54) {
			c = k >= 2 ? 1-(u.f-x) : x-(u.f-1);
			c /= u.f;
		} else
			c = 0;
		/* reduce u into [sqrt(2)/2, sqrt(2)] */
		hu = (hu&0x000fffff) + 0x3fe6a09e;
		u.i = (uint64_t)hu<<32 | (u.i&0xffffffff);
		f = u.f - 1;
	}
	hfsq = 0.5*f*f;
	s = f/(2.0+f);
	z = s*s;
	w = z*z;
	t1 = w*(Lg2+w*(Lg4+w*Lg6));
	t2 = z*(Lg1+w*(Lg3+w*(Lg5+w*Lg7)));
	R = t2 + t1;
	dk = k;
	return s*(hfsq+R) + (dk*ln2_lo+c) - hfsq + f + dk*ln2_hi;
}
