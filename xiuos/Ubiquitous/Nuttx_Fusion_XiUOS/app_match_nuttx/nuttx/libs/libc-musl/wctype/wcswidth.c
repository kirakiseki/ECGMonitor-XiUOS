/**
 * @file wcswidth.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <wchar.h>

int wcswidth(const wchar_t *wcs, size_t n)
{
	int l=0, k=0;
	for (; n-- && *wcs && (k = wcwidth(*wcs)) >= 0; l+=k, wcs++);
	return (k < 0) ? k : l;
}
