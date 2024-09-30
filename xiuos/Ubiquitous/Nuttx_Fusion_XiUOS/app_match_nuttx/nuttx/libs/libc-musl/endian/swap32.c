/**
 * @file swap32.c
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

#include <stdint.h>
#include <endian.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifndef __SWAP_UINT32_ISMACRO
uint32_t __swap_uint32(uint32_t n)
{
	return (n>>24) | (n>>8&0xff00) | (n<<8&0xff0000) | (n<<24);
}
#endif
