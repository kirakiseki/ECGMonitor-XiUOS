/**
 * @file isascii.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <ctype.h>
#undef isascii

int isascii(int c)
{
	return !(c&~0x7f);
}
