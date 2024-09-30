/**
 * @file iswdigit.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <wctype.h>

int iswdigit(wint_t wc)
{
	return (unsigned)wc-'0' < 10;
}
