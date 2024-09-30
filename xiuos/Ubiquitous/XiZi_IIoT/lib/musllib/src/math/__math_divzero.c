#include "libm.h"

double __math_divzero(uint32_t sign)
{
	return fp_barrier(sign ? -1.0 : 1.0) / 0.0;
}

int raise(int a)
{
	return 0;
}