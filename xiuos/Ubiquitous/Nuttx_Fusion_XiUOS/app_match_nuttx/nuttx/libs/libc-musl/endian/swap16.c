/**
 * @file swap16.c
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

#ifndef __SWAP_UINT16_ISMACRO
uint16_t __swap_uint16(uint16_t n)
{
  return n<<8 | n>>8;
}
#endif
