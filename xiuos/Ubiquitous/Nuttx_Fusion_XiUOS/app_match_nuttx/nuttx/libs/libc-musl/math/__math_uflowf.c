/**
 * @file __math_uflowf.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include "libm.h"

float __math_uflowf(uint32_t sign)
{
	return __math_xflowf(sign, 0x1p-95f);
}
