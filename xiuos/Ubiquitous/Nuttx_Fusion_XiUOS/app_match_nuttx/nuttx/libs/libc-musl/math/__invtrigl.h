/**
 * @file __invtrigl.h
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

/* shared by acosl, asinl and atan2l */
#define pio2_hi __pio2_hi
#define pio2_lo __pio2_lo
extern const long double pio2_hi, pio2_lo;

long double __invtrigl_R(long double z);
