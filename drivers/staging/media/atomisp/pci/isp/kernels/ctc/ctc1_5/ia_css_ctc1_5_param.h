/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Support for Intel Camera Imaging ISP subsystem.
 * Copyright (c) 2015, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __IA_CSS_CTC1_5_PARAM_H
#define __IA_CSS_CTC1_5_PARAM_H

#include "type_support.h"
#include "ctc/ctc_1.0/ia_css_ctc_param.h" /* vamem params */

/* CTC (Color Tone Control) */
struct sh_css_isp_ctc_params {
	s32 y0;
	s32 y1;
	s32 y2;
	s32 y3;
	s32 y4;
	s32 y5;
	s32 ce_gain_exp;
	s32 x1;
	s32 x2;
	s32 x3;
	s32 x4;
	s32 dydx0;
	s32 dydx0_shift;
	s32 dydx1;
	s32 dydx1_shift;
	s32 dydx2;
	s32 dydx2_shift;
	s32 dydx3;
	s32 dydx3_shift;
	s32 dydx4;
	s32 dydx4_shift;
};

#endif /* __IA_CSS_CTC1_5_PARAM_H */
