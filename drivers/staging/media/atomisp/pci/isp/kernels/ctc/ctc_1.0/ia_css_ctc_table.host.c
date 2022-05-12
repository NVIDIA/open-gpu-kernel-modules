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
#include "ia_css_ctc_table.host.h"

struct ia_css_ctc_table       default_ctc_table;


static const uint16_t
default_ctc_table_data[IA_CSS_VAMEM_2_CTC_TABLE_SIZE] = {
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
};


void
ia_css_config_ctc_table(void)
{
	memcpy(default_ctc_table.data.vamem_2, default_ctc_table_data,
	       sizeof(default_ctc_table_data));
	default_ctc_table.vamem_type     = IA_CSS_VAMEM_TYPE_2;
}
