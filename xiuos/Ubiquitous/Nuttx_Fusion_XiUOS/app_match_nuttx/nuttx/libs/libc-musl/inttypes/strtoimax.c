/**
 * @file strtoimax.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <nuttx/config.h>

#include <stdlib.h>
#include <inttypes.h>

/* Current implementation depends on strtoull() and, hence, is only
 * available if long long types are supported.
 */

#ifdef CONFIG_HAVE_LONG_LONG

intmax_t strtoimax(const char *restrict s, char **restrict p, int base)
{
	return strtoll(s, p, base);
}

#endif /* CONFIG_HAVE_LONG_LONG */
