/**
 * @file imaxabs.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <inttypes.h>

intmax_t imaxabs(intmax_t a)
{
	return a>0 ? a : -a;
}