/**
 * @file iswupper.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <wctype.h>

int iswupper(wint_t wc)
{
	return towlower(wc) != wc;
}
