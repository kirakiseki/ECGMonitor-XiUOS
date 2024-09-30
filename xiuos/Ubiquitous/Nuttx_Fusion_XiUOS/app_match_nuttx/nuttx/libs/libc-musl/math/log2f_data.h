/**
 * @file log2f_data.h
 * @brief musl source code
 *        https://github.com/bminor/musl.git
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022-08-04
 */

/*
 * Copyright (c) 2017-2018, Arm Limited.
 * SPDX-License-Identifier: MIT
 */
#ifndef _LOG2F_DATA_H
#define _LOG2F_DATA_H

#define LOG2F_TABLE_BITS 4
#define LOG2F_POLY_ORDER 4
extern const struct log2f_data {
	struct {
		double invc, logc;
	} tab[1 << LOG2F_TABLE_BITS];
	double poly[LOG2F_POLY_ORDER];
} __log2f_data;

#endif
