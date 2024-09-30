/**
 * @file __math_invalidf.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include "libm.h"

float __math_invalidf(float x)
{
	return (x - x) / (x - x);
}
