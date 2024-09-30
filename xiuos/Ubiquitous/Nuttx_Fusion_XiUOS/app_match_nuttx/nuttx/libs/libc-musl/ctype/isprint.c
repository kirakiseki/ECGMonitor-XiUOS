/**
 * @file isprint.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <ctype.h>
#undef isprint

int isprint(int c)
{
	return (unsigned)c-0x20 < 0x5f;
}
