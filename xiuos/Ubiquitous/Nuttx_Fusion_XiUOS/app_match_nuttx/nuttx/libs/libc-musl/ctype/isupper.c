/**
 * @file isupper.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <ctype.h>
#undef isupper

int isupper(int c)
{
	return (unsigned)c-'A' < 26;
}
