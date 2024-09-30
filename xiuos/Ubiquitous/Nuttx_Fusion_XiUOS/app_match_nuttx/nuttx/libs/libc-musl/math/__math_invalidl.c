/**
 * @file __math_invalidl.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <float.h>
#include "libm.h"

#if LDBL_MANT_DIG != DBL_MANT_DIG
long double __math_invalidl(long double x)
{
	return (x - x) / (x - x);
}
#endif
