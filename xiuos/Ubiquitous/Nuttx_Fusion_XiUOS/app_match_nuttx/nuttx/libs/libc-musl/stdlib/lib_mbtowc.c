/****************************************************************************
 * libs/libc-musl/stdlib/lib_mbtowc.c
 *
 *   Copyright (c)1999 Citrus Project,
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 ****************************************************************************/
/**
* @file lib_mbtowc.c
* @brief nuttx source code
*        https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-08-04
*/


/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdlib.h>
#include <wchar.h>
#include <errno.h>

#ifdef CONFIG_LIBC_WCHAR

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: mbtowc.c
 *
 * Description:
 *   Minimal multibyte to wide char converter
 *
 ****************************************************************************/
int mbtowc(FAR wchar_t *pwc, FAR const char *s, size_t n)
{
	unsigned c;
	const unsigned char *src = (const void *)s;
	wchar_t dummy;

	if (!src) return 0;
	if (!n) goto ilseq;
	if (!pwc) pwc = &dummy;

	if (*src < 0x80) return !!(*pwc = *src);
	if (MB_CUR_MAX==1) return (*pwc = CODEUNIT(*src)), 1;
	if (*src-SA > SB-SA) goto ilseq;
	c = bittab[*src++-SA];

	/* Avoid excessive checks against n: If shifting the state n-1
	 * times does not clear the high bit, then the value of n is
	 * insufficient to read a character */
	if (n<4 && ((c<<(6*n-6)) & (1U<<31))) goto ilseq;

	if (OOB(c,*src)) goto ilseq;
	c = c<<6 | *src++-0x80;
	if (!(c&(1U<<31))) {
		*pwc = c;
		return 2;
	}

	if (*src-0x80u >= 0x40) goto ilseq;
	c = c<<6 | *src++-0x80;
	if (!(c&(1U<<31))) {
		*pwc = c;
		return 3;
	}

	if (*src-0x80u >= 0x40) goto ilseq;
	*pwc = c<<6 | *src++-0x80;
	return 4;

ilseq:
	errno = EILSEQ;
	return -1;
}

#endif
