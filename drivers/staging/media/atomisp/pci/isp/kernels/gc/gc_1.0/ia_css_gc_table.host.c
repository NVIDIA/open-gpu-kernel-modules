// SPDX-License-Identifier: GPL-2.0
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

#include <linux/string.h> /* for memcpy() */

#include <type_support.h>
#include "system_global.h"
#include "vamem.h"
#include "ia_css_types.h"
#include "ia_css_gc_table.host.h"


struct ia_css_gamma_table default_gamma_table;

static const uint16_t
default_gamma_table_data[IA_CSS_VAMEM_2_GAMMA_TABLE_SIZE] = {
	0,   4,   8,  12,  17,  21,  27,  32,
	38,  44,  49,  55,  61,  66,  71,  76,
	80,  84,  88,  92,  95,  98, 102, 105,
	108, 110, 113, 116, 118, 121, 123, 126,
	128, 130, 132, 135, 137, 139, 141, 143,
	145, 146, 148, 150, 152, 153, 155, 156,
	158, 160, 161, 162, 164, 165, 166, 168,
	169, 170, 171, 172, 174, 175, 176, 177,
	178, 179, 180, 181, 182, 183, 184, 184,
	185, 186, 187, 188, 189, 189, 190, 191,
	192, 192, 193, 194, 195, 195, 196, 197,
	197, 198, 198, 199, 200, 200, 201, 201,
	202, 203, 203, 204, 204, 205, 205, 206,
	206, 207, 207, 208, 208, 209, 209, 210,
	210, 210, 211, 211, 212, 212, 213, 213,
	214, 214, 214, 215, 215, 216, 216, 216,
	217, 217, 218, 218, 218, 219, 219, 220,
	220, 220, 221, 221, 222, 222, 222, 223,
	223, 223, 224, 224, 225, 225, 225, 226,
	226, 226, 227, 227, 227, 228, 228, 228,
	229, 229, 229, 230, 230, 230, 231, 231,
	231, 232, 232, 232, 233, 233, 233, 234,
	234, 234, 234, 235, 235, 235, 236, 236,
	236, 237, 237, 237, 237, 238, 238, 238,
	239, 239, 239, 239, 240, 240, 240, 241,
	241, 241, 241, 242, 242, 242, 242, 243,
	243, 243, 243, 244, 244, 244, 245, 245,
	245, 245, 246, 246, 246, 246, 247, 247,
	247, 247, 248, 248, 248, 248, 249, 249,
	249, 249, 250, 250, 250, 250, 251, 251,
	251, 251, 252, 252, 252, 252, 253, 253,
	253, 253, 254, 254, 254, 254, 255, 255,
	255
};


void
ia_css_config_gamma_table(void)
{
	memcpy(default_gamma_table.data.vamem_2, default_gamma_table_data,
	       sizeof(default_gamma_table_data));
	default_gamma_table.vamem_type   = IA_CSS_VAMEM_TYPE_2;
}
