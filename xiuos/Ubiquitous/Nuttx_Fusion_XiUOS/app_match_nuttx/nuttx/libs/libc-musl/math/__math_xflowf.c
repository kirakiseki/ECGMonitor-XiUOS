/**
 * @file __math_xflowf.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include "libm.h"

float __math_xflowf(uint32_t sign, float y)
{
	return eval_as_float(fp_barrierf(sign ? -y : y) * y);
}
