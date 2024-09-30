/****************************************************************************
 * libs/libc-musl/math/lib_expm1l.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/**
* @file lib_expm1l.c
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
#include <nuttx/compiler.h>

#include "libm.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef CONFIG_HAVE_LONG_DOUBLE

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double expm1l(long double x)
{
	return expm1(x);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384

/* exp(x) - 1 = x + 0.5 x^2 + x^3 P(x)/Q(x)
   -.5 ln 2  <  x  <  .5 ln 2
   Theoretical peak relative error = 3.4e-22  */
static const long double
P0 = -1.586135578666346600772998894928250240826E4L,
P1 =  2.642771505685952966904660652518429479531E3L,
P2 = -3.423199068835684263987132888286791620673E2L,
P3 =  1.800826371455042224581246202420972737840E1L,
P4 = -5.238523121205561042771939008061958820811E-1L,
Q0 = -9.516813471998079611319047060563358064497E4L,
Q1 =  3.964866271411091674556850458227710004570E4L,
Q2 = -7.207678383830091850230366618190187434796E3L,
Q3 =  7.206038318724600171970199625081491823079E2L,
Q4 = -4.002027679107076077238836622982900945173E1L,
/* Q5 = 1.000000000000000000000000000000000000000E0 */
/* C1 + C2 = ln 2 */
C1 = 6.93145751953125E-1L,
C2 = 1.428606820309417232121458176568075500134E-6L,
/* ln 2^-65 */
minarg = -4.5054566736396445112120088E1L,
/* ln 2^16384 */
maxarg = 1.1356523406294143949492E4L;

long double expm1l(long double x)
{
	long double px, qx, xx;
	int k;

	if (isnan(x))
		return x;
	if (x > maxarg)
		return x*0x1p16383L; /* overflow, unless x==inf */
	if (x == 0.0)
		return x;
	if (x < minarg)
		return -1.0;

	xx = C1 + C2;
	/* Express x = ln 2 (k + remainder), remainder not exceeding 1/2. */
	px = floorl(0.5 + x / xx);
	k = px;
	/* remainder times ln 2 */
	x -= px * C1;
	x -= px * C2;

	/* Approximate exp(remainder ln 2).*/
	px = (((( P4 * x + P3) * x + P2) * x + P1) * x + P0) * x;
	qx = (((( x + Q4) * x + Q3) * x + Q2) * x + Q1) * x + Q0;
	xx = x * x;
	qx = x + (0.5 * xx + xx * px / qx);

	/* exp(x) = exp(k ln 2) exp(remainder ln 2) = 2^k exp(remainder ln 2).
	 We have qx = exp(remainder ln 2) - 1, so
	 exp(x) - 1  =  2^k (qx + 1) - 1  =  2^k qx + 2^k - 1.  */
	px = scalbnl(1.0, k);
	x = px * qx + (px - 1.0);
	return x;
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
// TODO: broken implementation to make things compile
long double expm1l(long double x)
{
	return expm1(x);
}
#endif


#endif
