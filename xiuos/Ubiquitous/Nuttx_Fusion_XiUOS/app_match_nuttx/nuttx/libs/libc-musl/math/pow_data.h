/**
 * @file pow_data.h
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

/*
 * Copyright (c) 2018, Arm Limited.
 * SPDX-License-Identifier: MIT
 */
#ifndef _POW_DATA_H
#define _POW_DATA_H

#define POW_LOG_TABLE_BITS 7
#define POW_LOG_POLY_ORDER 8
extern const struct pow_log_data {
	double ln2hi;
	double ln2lo;
	double poly[POW_LOG_POLY_ORDER - 1]; /* First coefficient is 1.  */
	/* Note: the pad field is unused, but allows slightly faster indexing.  */
	struct {
		double invc, pad, logc, logctail;
	} tab[1 << POW_LOG_TABLE_BITS];
} __pow_log_data;

#endif
