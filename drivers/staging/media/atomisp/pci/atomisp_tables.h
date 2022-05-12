/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Support for Medifield PNW Camera Imaging ISP subsystem.
 *
 * Copyright (c) 2010 Intel Corporation. All Rights Reserved.
 *
 * Copyright (c) 2010 Silicon Hive www.siliconhive.com.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 */
#ifndef	__ATOMISP_TABLES_H__
#define	__ATOMISP_TABLES_H__

#include "sh_css_params.h"

/*Sepia image effect table*/
static struct ia_css_cc_config sepia_cc_config = {
	.fraction_bits  = 8,
	.matrix	 = {141, 18, 68, -40, -5, -19, 35, 4, 16},
};

/*Negative image effect table*/
static struct ia_css_cc_config nega_cc_config = {
	.fraction_bits  = 8,
	.matrix	 = {255, 29, 120, 0, 374, 342, 0, 672, -301},
};

/*Mono image effect table*/
static struct ia_css_cc_config mono_cc_config = {
	.fraction_bits  = 8,
	.matrix	 = {255, 29, 120, 0, 0, 0, 0, 0, 0},
};

/*Skin whiten image effect table*/
static struct ia_css_macc_table skin_low_macc_table = {
	.data = {
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		7168, 0, 2048, 8192,
		5120, -1024, 2048, 8192,
		8192, 2048, -1024, 5120,
		8192, 2048, 0, 7168,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192
	}
};

static struct ia_css_macc_table skin_medium_macc_table = {
	.data = {
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		5120, 0, 6144, 8192,
		3072, -1024, 2048, 6144,
		6144, 2048, -1024, 3072,
		8192, 6144, 0, 5120,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192
	}
};

static struct ia_css_macc_table skin_high_macc_table = {
	.data = {
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		4096, 0, 8192, 8192,
		0, -2048, 4096, 6144,
		6144, 4096, -2048, 0,
		8192, 8192, 0, 4096,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192
	}
};

/*Blue enhencement image effect table*/
static struct ia_css_macc_table blue_macc_table = {
	.data = {
		9728, -3072, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		9728, 0, -3072, 8192,
		12800, 1536, -3072, 8192,
		11264, 0, 0, 11264,
		9728, -3072, 0, 11264
	}
};

/*Green enhencement image effect table*/
static struct ia_css_macc_table green_macc_table = {
	.data = {
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		10240, 4096, 0, 8192,
		10240, 4096, 0, 12288,
		12288, 0, 0, 12288,
		14336, -2048, 4096, 8192,
		10240, 0, 4096, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192,
		8192, 0, 0, 8192
	}
};

static struct ia_css_ctc_table vivid_ctc_table = {
	.data.vamem_2 = {
		0,  384,  837,  957, 1011, 1062, 1083, 1080,
		1078, 1077, 1053, 1039, 1012,  992,  969,  951,
		929,  906,  886,  866,  845,  823,  809,  790,
		772,  758,  741,  726,  711,  701,  688,  675,
		666,  656,  648,  639,  633,  626,  618,  612,
		603,  594,  582,  572,  557,  545,  529,  516,
		504,  491,  480,  467,  459,  447,  438,  429,
		419,  412,  404,  397,  389,  382,  376,  368,
		363,  357,  351,  345,  340,  336,  330,  326,
		321,  318,  312,  308,  304,  300,  297,  294,
		291,  286,  284,  281,  278,  275,  271,  268,
		261,  257,  251,  245,  240,  235,  232,  225,
		223,  218,  213,  209,  206,  204,  199,  197,
		193,  189,  186,  185,  183,  179,  177,  175,
		172,  170,  169,  167,  164,  164,  162,  160,
		158,  157,  156,  154,  154,  152,  151,  150,
		149,  148,  146,  147,  146,  144,  143,  143,
		142,  141,  140,  141,  139,  138,  138,  138,
		137,  136,  136,  135,  134,  134,  134,  133,
		132,  132,  131,  130,  131,  130,  129,  128,
		129,  127,  127,  127,  127,  125,  125,  125,
		123,  123,  122,  120,  118,  115,  114,  111,
		110,  108,  106,  105,  103,  102,  100,   99,
		97,   97,   96,   95,   94,   93,   93,   91,
		91,   91,   90,   90,   89,   89,   88,   88,
		89,   88,   88,   87,   87,   87,   87,   86,
		87,   87,   86,   87,   86,   86,   84,   84,
		82,   80,   78,   76,   74,   72,   70,   68,
		67,   65,   62,   60,   58,   56,   55,   54,
		53,   51,   49,   49,   47,   45,   45,   45,
		41,   40,   39,   39,   34,   33,   34,   32,
		25,   23,   24,   20,   13,    9,   12,    0,
		0
	}
};
#endif
