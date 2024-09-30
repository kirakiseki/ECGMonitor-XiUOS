/**
 * @file isspace.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <ctype.h>
#undef isspace

int isspace(int c)
{
	return c == ' ' || (unsigned)c-'\t' < 5;
}
