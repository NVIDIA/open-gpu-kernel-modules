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

#ifndef __IA_CSS_YNR2_PARAM_H
#define __IA_CSS_YNR2_PARAM_H

#include "type_support.h"

/* YNR (Y Noise Reduction), YEE (Y Edge Enhancement) */
struct sh_css_isp_yee2_params {
	s32 edge_sense_gain_0;
	s32 edge_sense_gain_1;
	s32 corner_sense_gain_0;
	s32 corner_sense_gain_1;
};

/* Fringe Control */
struct sh_css_isp_fc_params {
	s32 gain_exp;
	u16 coring_pos_0;
	u16 coring_pos_1;
	u16 coring_neg_0;
	u16 coring_neg_1;
	s32 gain_pos_0;
	s32 gain_pos_1;
	s32 gain_neg_0;
	s32 gain_neg_1;
	s32 crop_pos_0;
	s32 crop_pos_1;
	s32 crop_neg_0;
	s32 crop_neg_1;
};

#endif /* __IA_CSS_YNR2_PARAM_H */
