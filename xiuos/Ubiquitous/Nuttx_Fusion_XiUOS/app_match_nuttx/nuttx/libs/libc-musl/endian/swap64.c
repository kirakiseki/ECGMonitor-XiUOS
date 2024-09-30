/**
 * @file swap64.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <nuttx/compiler.h>

#include <stdint.h>
#include <endian.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef CONFIG_HAVE_LONG_LONG
uint64_t __swap_uint64(uint64_t n)
{
	return (__swap_uint32(n)+ (0ULL<<32)) | __swap_uint32(n>>32);
}
#endif
