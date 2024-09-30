/**
 * @file toupper.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <ctype.h>

int toupper(int c)
{
	if (islower(c)) return c & 0x5f;
	return c;
}
