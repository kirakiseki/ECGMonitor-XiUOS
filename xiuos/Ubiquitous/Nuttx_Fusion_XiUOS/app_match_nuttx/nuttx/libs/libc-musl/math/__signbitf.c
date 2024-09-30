/**
 * @file __signbitf.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include "libm.h"

// FIXME: macro in math.h
int __signbitf(float x)
{
	union {
		float f;
		uint32_t i;
	} y = { x };
	return y.i>>31;
}
