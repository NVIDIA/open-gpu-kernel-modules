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

#ifndef _SH_CSS_UDS_H_
#define _SH_CSS_UDS_H_

#include <type_support.h>

#define SIZE_OF_SH_CSS_UDS_INFO_IN_BITS (4 * 16)
#define SIZE_OF_SH_CSS_CROP_POS_IN_BITS (2 * 16)

/* Uds types, used in pipeline_global.h and sh_css_internal.h */

struct sh_css_uds_info {
	u16 curr_dx;
	u16 curr_dy;
	u16 xc;
	u16 yc;
};

struct sh_css_crop_pos {
	u16 x;
	u16 y;
};

#endif /* _SH_CSS_UDS_H_ */
