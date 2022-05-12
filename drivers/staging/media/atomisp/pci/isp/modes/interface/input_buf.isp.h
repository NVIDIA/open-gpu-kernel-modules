/* SPDX-License-Identifier: GPL-2.0 */
/**
Support for Intel Camera Imaging ISP subsystem.
Copyright (c) 2010 - 2015, Intel Corporation.

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.
*/

#ifndef _INPUT_BUF_ISP_H_
#define _INPUT_BUF_ISP_H_

/* Temporary include, since IA_CSS_BINARY_MODE_COPY is still needed */
#include "sh_css_defs.h"
#include "isp_const.h" /* MAX_VECTORS_PER_INPUT_LINE */

#define INPUT_BUF_HEIGHT	2 /* double buffer */
#define INPUT_BUF_LINES		2

#ifndef ENABLE_CONTINUOUS
#define ENABLE_CONTINUOUS 0
#endif

/* In continuous mode, the input buffer must be a fixed size for all binaries
 * and at a fixed address since it will be used by the SP. */
#define EXTRA_INPUT_VECTORS	2 /* For left padding */
#define MAX_VECTORS_PER_INPUT_LINE_CONT (CEIL_DIV(SH_CSS_MAX_SENSOR_WIDTH, ISP_NWAY) + EXTRA_INPUT_VECTORS)

/* The input buffer should be on a fixed address in vmem, for continuous capture */
#define INPUT_BUF_ADDR 0x0

#endif /* _INPUT_BUF_ISP_H_ */
