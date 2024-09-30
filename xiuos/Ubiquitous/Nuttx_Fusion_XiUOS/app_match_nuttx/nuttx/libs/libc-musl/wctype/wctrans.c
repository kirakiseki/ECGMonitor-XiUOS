/**
 * @file wctrans.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <wctype.h>
#include <string.h>

wctrans_t wctrans(const char *class)
{
	if (!strcmp(class, "toupper")) return (wctrans_t)1;
	if (!strcmp(class, "tolower")) return (wctrans_t)2;
	return 0;
}

wint_t towctrans(wint_t wc, wctrans_t trans)
{
	if (trans == (wctrans_t)1) return towupper(wc);
	if (trans == (wctrans_t)2) return towlower(wc);
	return wc;
}
