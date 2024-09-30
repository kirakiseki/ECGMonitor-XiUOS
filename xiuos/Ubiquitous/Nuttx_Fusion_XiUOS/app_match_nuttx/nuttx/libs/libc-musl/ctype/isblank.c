/**
 * @file isblank.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <ctype.h>

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}
