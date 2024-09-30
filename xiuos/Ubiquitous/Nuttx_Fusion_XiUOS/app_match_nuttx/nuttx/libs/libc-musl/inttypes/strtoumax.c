/**
 * @file strtoumax.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <stdlib.h>
#include <inttypes.h>

uintmax_t strtoumax(const char *restrict s, char **restrict p, int base)
{
	return strtoull(s, p, base);
}
