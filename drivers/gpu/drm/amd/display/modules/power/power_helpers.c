/* Copyright 2018 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: AMD
 *
 */

#include "power_helpers.h"
#include "dc/inc/hw/dmcu.h"
#include "dc/inc/hw/abm.h"
#include "dc.h"
#include "core_types.h"
#include "dmub_cmd.h"

#define DIV_ROUNDUP(a, b) (((a)+((b)/2))/(b))
#define bswap16_based_on_endian(big_endian, value) \
	(big_endian) ? cpu_to_be16(value) : cpu_to_le16(value)

/* Possible Min Reduction config from least aggressive to most aggressive
 *  0    1     2     3     4     5     6     7     8     9     10    11   12
 * 100  98.0 94.1  94.1  85.1  80.3  75.3  69.4  60.0  57.6  50.2  49.8  40.0 %
 */
static const unsigned char min_reduction_table[13] = {
0xff, 0xfa, 0xf0, 0xf0, 0xd9, 0xcd, 0xc0, 0xb1, 0x99, 0x93, 0x80, 0x82, 0x66};

/* Possible Max Reduction configs from least aggressive to most aggressive
 *  0    1     2     3     4     5     6     7     8     9     10    11   12
 * 96.1 89.8 85.1  80.3  69.4  64.7  64.7  50.2  39.6  30.2  30.2  30.2  19.6 %
 */
static const unsigned char max_reduction_table[13] = {
0xf5, 0xe5, 0xd9, 0xcd, 0xb1, 0xa5, 0xa5, 0x80, 0x65, 0x4d, 0x4d, 0x4d, 0x32};

/* Possible ABM 2.2 Min Reduction configs from least aggressive to most aggressive
 *  0    1     2     3     4     5     6     7     8     9     10    11   12
 * 100  100   100   100   100   100   100   100  100  92.2  83.1  75.3  75.3 %
 */
static const unsigned char min_reduction_table_v_2_2[13] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xeb, 0xd4, 0xc0, 0xc0};

/* Possible ABM 2.2 Max Reduction configs from least aggressive to most aggressive
 *  0    1     2     3     4     5     6     7     8     9     10    11   12
 * 96.1 89.8 74.9  69.4  64.7  52.2  48.6  39.6  30.2  25.1  19.6  12.5  12.5 %
 */
static const unsigned char max_reduction_table_v_2_2[13] = {
0xf5, 0xe5, 0xbf, 0xb1, 0xa5, 0x85, 0x7c, 0x65, 0x4d, 0x40, 0x32, 0x20, 0x20};

/* Predefined ABM configuration sets. We may have different configuration sets
 * in order to satisfy different power/quality requirements.
 */
static const unsigned char abm_config[abm_defines_max_config][abm_defines_max_level] = {
/*  ABM Level 1,    ABM Level 2,    ABM Level 3,    ABM Level 4 */
{       2,              5,              7,              8       },	/* Default - Medium aggressiveness */
{       2,              5,              8,              11      },	/* Alt #1  - Increased aggressiveness */
{       0,              2,              4,              8       },	/* Alt #2  - Minimal aggressiveness */
{       3,              6,              10,             12      },	/* Alt #3  - Super aggressiveness */
};

struct abm_parameters {
	unsigned char min_reduction;
	unsigned char max_reduction;
	unsigned char bright_pos_gain;
	unsigned char dark_pos_gain;
	unsigned char brightness_gain;
	unsigned char contrast_factor;
	unsigned char deviation_gain;
	unsigned char min_knee;
	unsigned char max_knee;
	unsigned short blRampReduction;
	unsigned short blRampStart;
};

static const struct abm_parameters abm_settings_config0[abm_defines_max_level] = {
//  min_red  max_red  bright_pos  dark_pos  bright_gain  contrast  dev   min_knee  max_knee  blRed    blStart
	{0xff,   0xbf,    0x20,       0x00,     0xff,        0x99,     0xb3, 0x40,     0xe0,     0xf777,  0xcccc},
	{0xde,   0x85,    0x20,       0x00,     0xe0,        0x90,     0xa8, 0x40,     0xc8,     0xf777,  0xcccc},
	{0xb0,   0x50,    0x20,       0x00,     0xc0,        0x88,     0x78, 0x70,     0xa0,     0xeeee,  0x9999},
	{0x82,   0x40,    0x20,       0x00,     0x00,        0xb8,     0xb3, 0x70,     0x70,     0xe333,  0xb333},
};

static const struct abm_parameters abm_settings_config1[abm_defines_max_level] = {
//  min_red  max_red  bright_pos  dark_pos  bright_gain  contrast  dev   min_knee  max_knee  blRed  blStart
	{0xf0,   0xd9,    0x20,       0x00,     0x00,        0xff,     0xb3, 0x70,     0x70,     0xcccc,  0xcccc},
	{0xcd,   0xa5,    0x20,       0x00,     0x00,        0xff,     0xb3, 0x70,     0x70,     0xcccc,  0xcccc},
	{0x99,   0x65,    0x20,       0x00,     0x00,        0xff,     0xb3, 0x70,     0x70,     0xcccc,  0xcccc},
	{0x82,   0x4d,    0x20,       0x00,     0x00,        0xff,     0xb3, 0x70,     0x70,     0xcccc,  0xcccc},
};

static const struct abm_parameters * const abm_settings[] = {
	abm_settings_config0,
	abm_settings_config1,
};

#define NUM_AMBI_LEVEL    5
#define NUM_AGGR_LEVEL    4
#define NUM_POWER_FN_SEGS 8
#define NUM_BL_CURVE_SEGS 16
#define IRAM_SIZE 256

#define IRAM_RESERVE_AREA_START_V2 0xF0  // reserve 0xF0~0xF6 are write by DMCU only
#define IRAM_RESERVE_AREA_END_V2 0xF6  // reserve 0xF0~0xF6 are write by DMCU only

#define IRAM_RESERVE_AREA_START_V2_2 0xF0  // reserve 0xF0~0xFF are write by DMCU only
#define IRAM_RESERVE_AREA_END_V2_2 0xFF  // reserve 0xF0~0xFF are write by DMCU only

#pragma pack(push, 1)
/* NOTE: iRAM is 256B in size */
struct iram_table_v_2 {
	/* flags                      */
	uint16_t min_abm_backlight;					/* 0x00 U16  */

	/* parameters for ABM2.0 algorithm */
	uint8_t min_reduction[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];		/* 0x02 U0.8 */
	uint8_t max_reduction[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];		/* 0x16 U0.8 */
	uint8_t bright_pos_gain[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];	/* 0x2a U2.6 */
	uint8_t bright_neg_gain[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];	/* 0x3e U2.6 */
	uint8_t dark_pos_gain[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];		/* 0x52 U2.6 */
	uint8_t dark_neg_gain[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];		/* 0x66 U2.6 */
	uint8_t iir_curve[NUM_AMBI_LEVEL];				/* 0x7a U0.8 */
	uint8_t deviation_gain;						/* 0x7f U0.8 */

	/* parameters for crgb conversion */
	uint16_t crgb_thresh[NUM_POWER_FN_SEGS];			/* 0x80 U3.13 */
	uint16_t crgb_offset[NUM_POWER_FN_SEGS];			/* 0x90 U1.15 */
	uint16_t crgb_slope[NUM_POWER_FN_SEGS];				/* 0xa0 U4.12 */

	/* parameters for custom curve */
	/* thresholds for brightness --> backlight */
	uint16_t backlight_thresholds[NUM_BL_CURVE_SEGS];		/* 0xb0 U16.0 */
	/* offsets for brightness --> backlight */
	uint16_t backlight_offsets[NUM_BL_CURVE_SEGS];			/* 0xd0 U16.0 */

	/* For reading PSR State directly from IRAM */
	uint8_t psr_state;						/* 0xf0       */
	uint8_t dmcu_mcp_interface_version;				/* 0xf1       */
	uint8_t dmcu_abm_feature_version;				/* 0xf2       */
	uint8_t dmcu_psr_feature_version;				/* 0xf3       */
	uint16_t dmcu_version;						/* 0xf4       */
	uint8_t dmcu_state;						/* 0xf6       */

	uint16_t blRampReduction;					/* 0xf7       */
	uint16_t blRampStart;						/* 0xf9       */
	uint8_t dummy5;							/* 0xfb       */
	uint8_t dummy6;							/* 0xfc       */
	uint8_t dummy7;							/* 0xfd       */
	uint8_t dummy8;							/* 0xfe       */
	uint8_t dummy9;							/* 0xff       */
};

struct iram_table_v_2_2 {
	/* flags                      */
	uint16_t flags;							/* 0x00 U16  */

	/* parameters for ABM2.2 algorithm */
	uint8_t min_reduction[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];		/* 0x02 U0.8 */
	uint8_t max_reduction[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];		/* 0x16 U0.8 */
	uint8_t bright_pos_gain[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];	/* 0x2a U2.6 */
	uint8_t dark_pos_gain[NUM_AMBI_LEVEL][NUM_AGGR_LEVEL];		/* 0x3e U2.6 */
	uint8_t hybrid_factor[NUM_AGGR_LEVEL];				/* 0x52 U0.8 */
	uint8_t contrast_factor[NUM_AGGR_LEVEL];			/* 0x56 U0.8 */
	uint8_t deviation_gain[NUM_AGGR_LEVEL];				/* 0x5a U0.8 */
	uint8_t iir_curve[NUM_AMBI_LEVEL];				/* 0x5e U0.8 */
	uint8_t min_knee[NUM_AGGR_LEVEL];				/* 0x63 U0.8 */
	uint8_t max_knee[NUM_AGGR_LEVEL];				/* 0x67 U0.8 */
	uint16_t min_abm_backlight;					/* 0x6b U16  */
	uint8_t pad[19];						/* 0x6d U0.8 */

	/* parameters for crgb conversion */
	uint16_t crgb_thresh[NUM_POWER_FN_SEGS];			/* 0x80 U3.13 */
	uint16_t crgb_offset[NUM_POWER_FN_SEGS];			/* 0x90 U1.15 */
	uint16_t crgb_slope[NUM_POWER_FN_SEGS];				/* 0xa0 U4.12 */

	/* parameters for custom curve */
	/* thresholds for brightness --> backlight */
	uint16_t backlight_thresholds[NUM_BL_CURVE_SEGS];		/* 0xb0 U16.0 */
	/* offsets for brightness --> backlight */
	uint16_t backlight_offsets[NUM_BL_CURVE_SEGS];			/* 0xd0 U16.0 */

	/* For reading PSR State directly from IRAM */
	uint8_t psr_state;						/* 0xf0       */
	uint8_t dmcu_mcp_interface_version;				/* 0xf1       */
	uint8_t dmcu_abm_feature_version;				/* 0xf2       */
	uint8_t dmcu_psr_feature_version;				/* 0xf3       */
	uint16_t dmcu_version;						/* 0xf4       */
	uint8_t dmcu_state;						/* 0xf6       */

	uint8_t dummy1;							/* 0xf7       */
	uint8_t dummy2;							/* 0xf8       */
	uint8_t dummy3;							/* 0xf9       */
	uint8_t dummy4;							/* 0xfa       */
	uint8_t dummy5;							/* 0xfb       */
	uint8_t dummy6;							/* 0xfc       */
	uint8_t dummy7;							/* 0xfd       */
	uint8_t dummy8;							/* 0xfe       */
	uint8_t dummy9;							/* 0xff       */
};
#pragma pack(pop)

static void fill_backlight_transform_table(struct dmcu_iram_parameters params,
		struct iram_table_v_2 *table)
{
	unsigned int i;
	unsigned int num_entries = NUM_BL_CURVE_SEGS;
	unsigned int lut_index;

	table->backlight_thresholds[0] = 0;
	table->backlight_offsets[0] = params.backlight_lut_array[0];
	table->backlight_thresholds[num_entries-1] = 0xFFFF;
	table->backlight_offsets[num_entries-1] =
		params.backlight_lut_array[params.backlight_lut_array_size - 1];

	/* Setup all brightness levels between 0% and 100% exclusive
	 * Fills brightness-to-backlight transform table. Backlight custom curve
	 * describes transform from brightness to backlight. It will be defined
	 * as set of thresholds and set of offsets, together, implying
	 * extrapolation of custom curve into 16 uniformly spanned linear
	 * segments.  Each threshold/offset represented by 16 bit entry in
	 * format U4.10.
	 */
	for (i = 1; i+1 < num_entries; i++) {
		lut_index = (params.backlight_lut_array_size - 1) * i / (num_entries - 1);
		ASSERT(lut_index < params.backlight_lut_array_size);

		table->backlight_thresholds[i] =
			cpu_to_be16(DIV_ROUNDUP((i * 65536), num_entries));
		table->backlight_offsets[i] =
			cpu_to_be16(params.backlight_lut_array[lut_index]);
	}
}

static void fill_backlight_transform_table_v_2_2(struct dmcu_iram_parameters params,
		struct iram_table_v_2_2 *table, bool big_endian)
{
	unsigned int i;
	unsigned int num_entries = NUM_BL_CURVE_SEGS;
	unsigned int lut_index;

	table->backlight_thresholds[0] = 0;
	table->backlight_offsets[0] = params.backlight_lut_array[0];
	table->backlight_thresholds[num_entries-1] = 0xFFFF;
	table->backlight_offsets[num_entries-1] =
		params.backlight_lut_array[params.backlight_lut_array_size - 1];

	/* Setup all brightness levels between 0% and 100% exclusive
	 * Fills brightness-to-backlight transform table. Backlight custom curve
	 * describes transform from brightness to backlight. It will be defined
	 * as set of thresholds and set of offsets, together, implying
	 * extrapolation of custom curve into 16 uniformly spanned linear
	 * segments.  Each threshold/offset represented by 16 bit entry in
	 * format U4.10.
	 */
	for (i = 1; i+1 < num_entries; i++) {
		lut_index = DIV_ROUNDUP((i * params.backlight_lut_array_size), num_entries);
		ASSERT(lut_index < params.backlight_lut_array_size);

		table->backlight_thresholds[i] = (big_endian) ?
			cpu_to_be16(DIV_ROUNDUP((i * 65536), num_entries)) :
			cpu_to_le16(DIV_ROUNDUP((i * 65536), num_entries));
		table->backlight_offsets[i] = (big_endian) ?
			cpu_to_be16(params.backlight_lut_array[lut_index]) :
			cpu_to_le16(params.backlight_lut_array[lut_index]);
	}
}

static void fill_iram_v_2(struct iram_table_v_2 *ram_table, struct dmcu_iram_parameters params)
{
	unsigned int set = params.set;

	ram_table->min_abm_backlight =
			cpu_to_be16(params.min_abm_backlight);
	ram_table->deviation_gain = 0xb3;

	ram_table->blRampReduction =
		cpu_to_be16(params.backlight_ramping_reduction);
	ram_table->blRampStart =
		cpu_to_be16(params.backlight_ramping_start);

	ram_table->min_reduction[0][0] = min_reduction_table[abm_config[set][0]];
	ram_table->min_reduction[1][0] = min_reduction_table[abm_config[set][0]];
	ram_table->min_reduction[2][0] = min_reduction_table[abm_config[set][0]];
	ram_table->min_reduction[3][0] = min_reduction_table[abm_config[set][0]];
	ram_table->min_reduction[4][0] = min_reduction_table[abm_config[set][0]];
	ram_table->max_reduction[0][0] = max_reduction_table[abm_config[set][0]];
	ram_table->max_reduction[1][0] = max_reduction_table[abm_config[set][0]];
	ram_table->max_reduction[2][0] = max_reduction_table[abm_config[set][0]];
	ram_table->max_reduction[3][0] = max_reduction_table[abm_config[set][0]];
	ram_table->max_reduction[4][0] = max_reduction_table[abm_config[set][0]];

	ram_table->min_reduction[0][1] = min_reduction_table[abm_config[set][1]];
	ram_table->min_reduction[1][1] = min_reduction_table[abm_config[set][1]];
	ram_table->min_reduction[2][1] = min_reduction_table[abm_config[set][1]];
	ram_table->min_reduction[3][1] = min_reduction_table[abm_config[set][1]];
	ram_table->min_reduction[4][1] = min_reduction_table[abm_config[set][1]];
	ram_table->max_reduction[0][1] = max_reduction_table[abm_config[set][1]];
	ram_table->max_reduction[1][1] = max_reduction_table[abm_config[set][1]];
	ram_table->max_reduction[2][1] = max_reduction_table[abm_config[set][1]];
	ram_table->max_reduction[3][1] = max_reduction_table[abm_config[set][1]];
	ram_table->max_reduction[4][1] = max_reduction_table[abm_config[set][1]];

	ram_table->min_reduction[0][2] = min_reduction_table[abm_config[set][2]];
	ram_table->min_reduction[1][2] = min_reduction_table[abm_config[set][2]];
	ram_table->min_reduction[2][2] = min_reduction_table[abm_config[set][2]];
	ram_table->min_reduction[3][2] = min_reduction_table[abm_config[set][2]];
	ram_table->min_reduction[4][2] = min_reduction_table[abm_config[set][2]];
	ram_table->max_reduction[0][2] = max_reduction_table[abm_config[set][2]];
	ram_table->max_reduction[1][2] = max_reduction_table[abm_config[set][2]];
	ram_table->max_reduction[2][2] = max_reduction_table[abm_config[set][2]];
	ram_table->max_reduction[3][2] = max_reduction_table[abm_config[set][2]];
	ram_table->max_reduction[4][2] = max_reduction_table[abm_config[set][2]];

	ram_table->min_reduction[0][3] = min_reduction_table[abm_config[set][3]];
	ram_table->min_reduction[1][3] = min_reduction_table[abm_config[set][3]];
	ram_table->min_reduction[2][3] = min_reduction_table[abm_config[set][3]];
	ram_table->min_reduction[3][3] = min_reduction_table[abm_config[set][3]];
	ram_table->min_reduction[4][3] = min_reduction_table[abm_config[set][3]];
	ram_table->max_reduction[0][3] = max_reduction_table[abm_config[set][3]];
	ram_table->max_reduction[1][3] = max_reduction_table[abm_config[set][3]];
	ram_table->max_reduction[2][3] = max_reduction_table[abm_config[set][3]];
	ram_table->max_reduction[3][3] = max_reduction_table[abm_config[set][3]];
	ram_table->max_reduction[4][3] = max_reduction_table[abm_config[set][3]];

	ram_table->bright_pos_gain[0][0] = 0x20;
	ram_table->bright_pos_gain[0][1] = 0x20;
	ram_table->bright_pos_gain[0][2] = 0x20;
	ram_table->bright_pos_gain[0][3] = 0x20;
	ram_table->bright_pos_gain[1][0] = 0x20;
	ram_table->bright_pos_gain[1][1] = 0x20;
	ram_table->bright_pos_gain[1][2] = 0x20;
	ram_table->bright_pos_gain[1][3] = 0x20;
	ram_table->bright_pos_gain[2][0] = 0x20;
	ram_table->bright_pos_gain[2][1] = 0x20;
	ram_table->bright_pos_gain[2][2] = 0x20;
	ram_table->bright_pos_gain[2][3] = 0x20;
	ram_table->bright_pos_gain[3][0] = 0x20;
	ram_table->bright_pos_gain[3][1] = 0x20;
	ram_table->bright_pos_gain[3][2] = 0x20;
	ram_table->bright_pos_gain[3][3] = 0x20;
	ram_table->bright_pos_gain[4][0] = 0x20;
	ram_table->bright_pos_gain[4][1] = 0x20;
	ram_table->bright_pos_gain[4][2] = 0x20;
	ram_table->bright_pos_gain[4][3] = 0x20;
	ram_table->bright_neg_gain[0][0] = 0x00;
	ram_table->bright_neg_gain[0][1] = 0x00;
	ram_table->bright_neg_gain[0][2] = 0x00;
	ram_table->bright_neg_gain[0][3] = 0x00;
	ram_table->bright_neg_gain[1][0] = 0x00;
	ram_table->bright_neg_gain[1][1] = 0x00;
	ram_table->bright_neg_gain[1][2] = 0x00;
	ram_table->bright_neg_gain[1][3] = 0x00;
	ram_table->bright_neg_gain[2][0] = 0x00;
	ram_table->bright_neg_gain[2][1] = 0x00;
	ram_table->bright_neg_gain[2][2] = 0x00;
	ram_table->bright_neg_gain[2][3] = 0x00;
	ram_table->bright_neg_gain[3][0] = 0x00;
	ram_table->bright_neg_gain[3][1] = 0x00;
	ram_table->bright_neg_gain[3][2] = 0x00;
	ram_table->bright_neg_gain[3][3] = 0x00;
	ram_table->bright_neg_gain[4][0] = 0x00;
	ram_table->bright_neg_gain[4][1] = 0x00;
	ram_table->bright_neg_gain[4][2] = 0x00;
	ram_table->bright_neg_gain[4][3] = 0x00;
	ram_table->dark_pos_gain[0][0] = 0x00;
	ram_table->dark_pos_gain[0][1] = 0x00;
	ram_table->dark_pos_gain[0][2] = 0x00;
	ram_table->dark_pos_gain[0][3] = 0x00;
	ram_table->dark_pos_gain[1][0] = 0x00;
	ram_table->dark_pos_gain[1][1] = 0x00;
	ram_table->dark_pos_gain[1][2] = 0x00;
	ram_table->dark_pos_gain[1][3] = 0x00;
	ram_table->dark_pos_gain[2][0] = 0x00;
	ram_table->dark_pos_gain[2][1] = 0x00;
	ram_table->dark_pos_gain[2][2] = 0x00;
	ram_table->dark_pos_gain[2][3] = 0x00;
	ram_table->dark_pos_gain[3][0] = 0x00;
	ram_table->dark_pos_gain[3][1] = 0x00;
	ram_table->dark_pos_gain[3][2] = 0x00;
	ram_table->dark_pos_gain[3][3] = 0x00;
	ram_table->dark_pos_gain[4][0] = 0x00;
	ram_table->dark_pos_gain[4][1] = 0x00;
	ram_table->dark_pos_gain[4][2] = 0x00;
	ram_table->dark_pos_gain[4][3] = 0x00;
	ram_table->dark_neg_gain[0][0] = 0x00;
	ram_table->dark_neg_gain[0][1] = 0x00;
	ram_table->dark_neg_gain[0][2] = 0x00;
	ram_table->dark_neg_gain[0][3] = 0x00;
	ram_table->dark_neg_gain[1][0] = 0x00;
	ram_table->dark_neg_gain[1][1] = 0x00;
	ram_table->dark_neg_gain[1][2] = 0x00;
	ram_table->dark_neg_gain[1][3] = 0x00;
	ram_table->dark_neg_gain[2][0] = 0x00;
	ram_table->dark_neg_gain[2][1] = 0x00;
	ram_table->dark_neg_gain[2][2] = 0x00;
	ram_table->dark_neg_gain[2][3] = 0x00;
	ram_table->dark_neg_gain[3][0] = 0x00;
	ram_table->dark_neg_gain[3][1] = 0x00;
	ram_table->dark_neg_gain[3][2] = 0x00;
	ram_table->dark_neg_gain[3][3] = 0x00;
	ram_table->dark_neg_gain[4][0] = 0x00;
	ram_table->dark_neg_gain[4][1] = 0x00;
	ram_table->dark_neg_gain[4][2] = 0x00;
	ram_table->dark_neg_gain[4][3] = 0x00;

	ram_table->iir_curve[0] = 0x65;
	ram_table->iir_curve[1] = 0x65;
	ram_table->iir_curve[2] = 0x65;
	ram_table->iir_curve[3] = 0x65;
	ram_table->iir_curve[4] = 0x65;

	//Gamma 2.4
	ram_table->crgb_thresh[0] = cpu_to_be16(0x13b6);
	ram_table->crgb_thresh[1] = cpu_to_be16(0x1648);
	ram_table->crgb_thresh[2] = cpu_to_be16(0x18e3);
	ram_table->crgb_thresh[3] = cpu_to_be16(0x1b41);
	ram_table->crgb_thresh[4] = cpu_to_be16(0x1d46);
	ram_table->crgb_thresh[5] = cpu_to_be16(0x1f21);
	ram_table->crgb_thresh[6] = cpu_to_be16(0x2167);
	ram_table->crgb_thresh[7] = cpu_to_be16(0x2384);
	ram_table->crgb_offset[0] = cpu_to_be16(0x2999);
	ram_table->crgb_offset[1] = cpu_to_be16(0x3999);
	ram_table->crgb_offset[2] = cpu_to_be16(0x4666);
	ram_table->crgb_offset[3] = cpu_to_be16(0x5999);
	ram_table->crgb_offset[4] = cpu_to_be16(0x6333);
	ram_table->crgb_offset[5] = cpu_to_be16(0x7800);
	ram_table->crgb_offset[6] = cpu_to_be16(0x8c00);
	ram_table->crgb_offset[7] = cpu_to_be16(0xa000);
	ram_table->crgb_slope[0]  = cpu_to_be16(0x3147);
	ram_table->crgb_slope[1]  = cpu_to_be16(0x2978);
	ram_table->crgb_slope[2]  = cpu_to_be16(0x23a2);
	ram_table->crgb_slope[3]  = cpu_to_be16(0x1f55);
	ram_table->crgb_slope[4]  = cpu_to_be16(0x1c63);
	ram_table->crgb_slope[5]  = cpu_to_be16(0x1a0f);
	ram_table->crgb_slope[6]  = cpu_to_be16(0x178d);
	ram_table->crgb_slope[7]  = cpu_to_be16(0x15ab);

	fill_backlight_transform_table(
			params, ram_table);
}

static void fill_iram_v_2_2(struct iram_table_v_2_2 *ram_table, struct dmcu_iram_parameters params)
{
	unsigned int set = params.set;

	ram_table->flags = 0x0;

	ram_table->min_abm_backlight =
			cpu_to_be16(params.min_abm_backlight);

	ram_table->deviation_gain[0] = 0xb3;
	ram_table->deviation_gain[1] = 0xa8;
	ram_table->deviation_gain[2] = 0x98;
	ram_table->deviation_gain[3] = 0x68;

	ram_table->min_reduction[0][0] = min_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->min_reduction[1][0] = min_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->min_reduction[2][0] = min_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->min_reduction[3][0] = min_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->min_reduction[4][0] = min_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->max_reduction[0][0] = max_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->max_reduction[1][0] = max_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->max_reduction[2][0] = max_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->max_reduction[3][0] = max_reduction_table_v_2_2[abm_config[set][0]];
	ram_table->max_reduction[4][0] = max_reduction_table_v_2_2[abm_config[set][0]];

	ram_table->min_reduction[0][1] = min_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->min_reduction[1][1] = min_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->min_reduction[2][1] = min_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->min_reduction[3][1] = min_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->min_reduction[4][1] = min_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->max_reduction[0][1] = max_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->max_reduction[1][1] = max_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->max_reduction[2][1] = max_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->max_reduction[3][1] = max_reduction_table_v_2_2[abm_config[set][1]];
	ram_table->max_reduction[4][1] = max_reduction_table_v_2_2[abm_config[set][1]];

	ram_table->min_reduction[0][2] = min_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->min_reduction[1][2] = min_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->min_reduction[2][2] = min_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->min_reduction[3][2] = min_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->min_reduction[4][2] = min_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->max_reduction[0][2] = max_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->max_reduction[1][2] = max_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->max_reduction[2][2] = max_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->max_reduction[3][2] = max_reduction_table_v_2_2[abm_config[set][2]];
	ram_table->max_reduction[4][2] = max_reduction_table_v_2_2[abm_config[set][2]];

	ram_table->min_reduction[0][3] = min_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->min_reduction[1][3] = min_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->min_reduction[2][3] = min_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->min_reduction[3][3] = min_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->min_reduction[4][3] = min_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->max_reduction[0][3] = max_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->max_reduction[1][3] = max_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->max_reduction[2][3] = max_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->max_reduction[3][3] = max_reduction_table_v_2_2[abm_config[set][3]];
	ram_table->max_reduction[4][3] = max_reduction_table_v_2_2[abm_config[set][3]];

	ram_table->bright_pos_gain[0][0] = 0x20;
	ram_table->bright_pos_gain[0][1] = 0x20;
	ram_table->bright_pos_gain[0][2] = 0x20;
	ram_table->bright_pos_gain[0][3] = 0x20;
	ram_table->bright_pos_gain[1][0] = 0x20;
	ram_table->bright_pos_gain[1][1] = 0x20;
	ram_table->bright_pos_gain[1][2] = 0x20;
	ram_table->bright_pos_gain[1][3] = 0x20;
	ram_table->bright_pos_gain[2][0] = 0x20;
	ram_table->bright_pos_gain[2][1] = 0x20;
	ram_table->bright_pos_gain[2][2] = 0x20;
	ram_table->bright_pos_gain[2][3] = 0x20;
	ram_table->bright_pos_gain[3][0] = 0x20;
	ram_table->bright_pos_gain[3][1] = 0x20;
	ram_table->bright_pos_gain[3][2] = 0x20;
	ram_table->bright_pos_gain[3][3] = 0x20;
	ram_table->bright_pos_gain[4][0] = 0x20;
	ram_table->bright_pos_gain[4][1] = 0x20;
	ram_table->bright_pos_gain[4][2] = 0x20;
	ram_table->bright_pos_gain[4][3] = 0x20;

	ram_table->dark_pos_gain[0][0] = 0x00;
	ram_table->dark_pos_gain[0][1] = 0x00;
	ram_table->dark_pos_gain[0][2] = 0x00;
	ram_table->dark_pos_gain[0][3] = 0x00;
	ram_table->dark_pos_gain[1][0] = 0x00;
	ram_table->dark_pos_gain[1][1] = 0x00;
	ram_table->dark_pos_gain[1][2] = 0x00;
	ram_table->dark_pos_gain[1][3] = 0x00;
	ram_table->dark_pos_gain[2][0] = 0x00;
	ram_table->dark_pos_gain[2][1] = 0x00;
	ram_table->dark_pos_gain[2][2] = 0x00;
	ram_table->dark_pos_gain[2][3] = 0x00;
	ram_table->dark_pos_gain[3][0] = 0x00;
	ram_table->dark_pos_gain[3][1] = 0x00;
	ram_table->dark_pos_gain[3][2] = 0x00;
	ram_table->dark_pos_gain[3][3] = 0x00;
	ram_table->dark_pos_gain[4][0] = 0x00;
	ram_table->dark_pos_gain[4][1] = 0x00;
	ram_table->dark_pos_gain[4][2] = 0x00;
	ram_table->dark_pos_gain[4][3] = 0x00;

	ram_table->hybrid_factor[0] = 0xff;
	ram_table->hybrid_factor[1] = 0xff;
	ram_table->hybrid_factor[2] = 0xff;
	ram_table->hybrid_factor[3] = 0xc0;

	ram_table->contrast_factor[0] = 0x99;
	ram_table->contrast_factor[1] = 0x99;
	ram_table->contrast_factor[2] = 0x90;
	ram_table->contrast_factor[3] = 0x80;

	ram_table->iir_curve[0] = 0x65;
	ram_table->iir_curve[1] = 0x65;
	ram_table->iir_curve[2] = 0x65;
	ram_table->iir_curve[3] = 0x65;
	ram_table->iir_curve[4] = 0x65;

	//Gamma 2.2
	ram_table->crgb_thresh[0] = cpu_to_be16(0x127c);
	ram_table->crgb_thresh[1] = cpu_to_be16(0x151b);
	ram_table->crgb_thresh[2] = cpu_to_be16(0x17d5);
	ram_table->crgb_thresh[3] = cpu_to_be16(0x1a56);
	ram_table->crgb_thresh[4] = cpu_to_be16(0x1c83);
	ram_table->crgb_thresh[5] = cpu_to_be16(0x1e72);
	ram_table->crgb_thresh[6] = cpu_to_be16(0x20f0);
	ram_table->crgb_thresh[7] = cpu_to_be16(0x232b);
	ram_table->crgb_offset[0] = cpu_to_be16(0x2999);
	ram_table->crgb_offset[1] = cpu_to_be16(0x3999);
	ram_table->crgb_offset[2] = cpu_to_be16(0x4666);
	ram_table->crgb_offset[3] = cpu_to_be16(0x5999);
	ram_table->crgb_offset[4] = cpu_to_be16(0x6333);
	ram_table->crgb_offset[5] = cpu_to_be16(0x7800);
	ram_table->crgb_offset[6] = cpu_to_be16(0x8c00);
	ram_table->crgb_offset[7] = cpu_to_be16(0xa000);
	ram_table->crgb_slope[0]  = cpu_to_be16(0x3609);
	ram_table->crgb_slope[1]  = cpu_to_be16(0x2dfa);
	ram_table->crgb_slope[2]  = cpu_to_be16(0x27ea);
	ram_table->crgb_slope[3]  = cpu_to_be16(0x235d);
	ram_table->crgb_slope[4]  = cpu_to_be16(0x2042);
	ram_table->crgb_slope[5]  = cpu_to_be16(0x1dc3);
	ram_table->crgb_slope[6]  = cpu_to_be16(0x1b1a);
	ram_table->crgb_slope[7]  = cpu_to_be16(0x1910);

	fill_backlight_transform_table_v_2_2(
			params, ram_table, true);
}

static void fill_iram_v_2_3(struct iram_table_v_2_2 *ram_table, struct dmcu_iram_parameters params, bool big_endian)
{
	unsigned int i, j;
	unsigned int set = params.set;

	ram_table->flags = 0x0;
	ram_table->min_abm_backlight = (big_endian) ?
		cpu_to_be16(params.min_abm_backlight) :
		cpu_to_le16(params.min_abm_backlight);

	for (i = 0; i < NUM_AGGR_LEVEL; i++) {
		ram_table->hybrid_factor[i] = abm_settings[set][i].brightness_gain;
		ram_table->contrast_factor[i] = abm_settings[set][i].contrast_factor;
		ram_table->deviation_gain[i] = abm_settings[set][i].deviation_gain;
		ram_table->min_knee[i] = abm_settings[set][i].min_knee;
		ram_table->max_knee[i] = abm_settings[set][i].max_knee;

		for (j = 0; j < NUM_AMBI_LEVEL; j++) {
			ram_table->min_reduction[j][i] = abm_settings[set][i].min_reduction;
			ram_table->max_reduction[j][i] = abm_settings[set][i].max_reduction;
			ram_table->bright_pos_gain[j][i] = abm_settings[set][i].bright_pos_gain;
			ram_table->dark_pos_gain[j][i] = abm_settings[set][i].dark_pos_gain;
		}
	}

	ram_table->iir_curve[0] = 0x65;
	ram_table->iir_curve[1] = 0x65;
	ram_table->iir_curve[2] = 0x65;
	ram_table->iir_curve[3] = 0x65;
	ram_table->iir_curve[4] = 0x65;

	//Gamma 2.2
	ram_table->crgb_thresh[0] = bswap16_based_on_endian(big_endian, 0x127c);
	ram_table->crgb_thresh[1] = bswap16_based_on_endian(big_endian, 0x151b);
	ram_table->crgb_thresh[2] = bswap16_based_on_endian(big_endian, 0x17d5);
	ram_table->crgb_thresh[3] = bswap16_based_on_endian(big_endian, 0x1a56);
	ram_table->crgb_thresh[4] = bswap16_based_on_endian(big_endian, 0x1c83);
	ram_table->crgb_thresh[5] = bswap16_based_on_endian(big_endian, 0x1e72);
	ram_table->crgb_thresh[6] = bswap16_based_on_endian(big_endian, 0x20f0);
	ram_table->crgb_thresh[7] = bswap16_based_on_endian(big_endian, 0x232b);
	ram_table->crgb_offset[0] = bswap16_based_on_endian(big_endian, 0x2999);
	ram_table->crgb_offset[1] = bswap16_based_on_endian(big_endian, 0x3999);
	ram_table->crgb_offset[2] = bswap16_based_on_endian(big_endian, 0x4666);
	ram_table->crgb_offset[3] = bswap16_based_on_endian(big_endian, 0x5999);
	ram_table->crgb_offset[4] = bswap16_based_on_endian(big_endian, 0x6333);
	ram_table->crgb_offset[5] = bswap16_based_on_endian(big_endian, 0x7800);
	ram_table->crgb_offset[6] = bswap16_based_on_endian(big_endian, 0x8c00);
	ram_table->crgb_offset[7] = bswap16_based_on_endian(big_endian, 0xa000);
	ram_table->crgb_slope[0]  = bswap16_based_on_endian(big_endian, 0x3609);
	ram_table->crgb_slope[1]  = bswap16_based_on_endian(big_endian, 0x2dfa);
	ram_table->crgb_slope[2]  = bswap16_based_on_endian(big_endian, 0x27ea);
	ram_table->crgb_slope[3]  = bswap16_based_on_endian(big_endian, 0x235d);
	ram_table->crgb_slope[4]  = bswap16_based_on_endian(big_endian, 0x2042);
	ram_table->crgb_slope[5]  = bswap16_based_on_endian(big_endian, 0x1dc3);
	ram_table->crgb_slope[6]  = bswap16_based_on_endian(big_endian, 0x1b1a);
	ram_table->crgb_slope[7]  = bswap16_based_on_endian(big_endian, 0x1910);

	fill_backlight_transform_table_v_2_2(
			params, ram_table, big_endian);
}

bool dmub_init_abm_config(struct resource_pool *res_pool,
	struct dmcu_iram_parameters params,
	unsigned int inst)
{
	struct iram_table_v_2_2 ram_table;
	struct abm_config_table config;
	unsigned int set = params.set;
	bool result = false;
	uint32_t i, j = 0;

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	if (res_pool->abm == NULL && res_pool->multiple_abms[inst] == NULL)
		return false;
#else
	if (res_pool->abm == NULL)
		return false;
#endif

	memset(&ram_table, 0, sizeof(ram_table));
	memset(&config, 0, sizeof(config));

	fill_iram_v_2_3(&ram_table, params, false);

	// We must copy to structure that is aligned to 32-bit
	for (i = 0; i < NUM_POWER_FN_SEGS; i++) {
		config.crgb_thresh[i] = ram_table.crgb_thresh[i];
		config.crgb_offset[i] = ram_table.crgb_offset[i];
		config.crgb_slope[i] = ram_table.crgb_slope[i];
	}

	for (i = 0; i < NUM_BL_CURVE_SEGS; i++) {
		config.backlight_thresholds[i] = ram_table.backlight_thresholds[i];
		config.backlight_offsets[i] = ram_table.backlight_offsets[i];
	}

	for (i = 0; i < NUM_AMBI_LEVEL; i++)
		config.iir_curve[i] = ram_table.iir_curve[i];

	for (i = 0; i < NUM_AMBI_LEVEL; i++) {
		for (j = 0; j < NUM_AGGR_LEVEL; j++) {
			config.min_reduction[i][j] = ram_table.min_reduction[i][j];
			config.max_reduction[i][j] = ram_table.max_reduction[i][j];
			config.bright_pos_gain[i][j] = ram_table.bright_pos_gain[i][j];
			config.dark_pos_gain[i][j] = ram_table.dark_pos_gain[i][j];
		}
	}

	for (i = 0; i < NUM_AGGR_LEVEL; i++) {
		config.hybrid_factor[i] = ram_table.hybrid_factor[i];
		config.contrast_factor[i] = ram_table.contrast_factor[i];
		config.deviation_gain[i] = ram_table.deviation_gain[i];
		config.min_knee[i] = ram_table.min_knee[i];
		config.max_knee[i] = ram_table.max_knee[i];
	}

	if (params.backlight_ramping_override) {
		for (i = 0; i < NUM_AGGR_LEVEL; i++) {
			config.blRampReduction[i] = params.backlight_ramping_reduction;
			config.blRampStart[i] = params.backlight_ramping_start;
			}
		} else {
			for (i = 0; i < NUM_AGGR_LEVEL; i++) {
				config.blRampReduction[i] = abm_settings[set][i].blRampReduction;
				config.blRampStart[i] = abm_settings[set][i].blRampStart;
				}
			}

	config.min_abm_backlight = ram_table.min_abm_backlight;

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	if (res_pool->multiple_abms[inst]) {
		result = res_pool->multiple_abms[inst]->funcs->init_abm_config(
			res_pool->multiple_abms[inst], (char *)(&config), sizeof(struct abm_config_table), inst);
	} else
#endif
		result = res_pool->abm->funcs->init_abm_config(
			res_pool->abm, (char *)(&config), sizeof(struct abm_config_table), 0);

	return result;
}

bool dmcu_load_iram(struct dmcu *dmcu,
	struct dmcu_iram_parameters params)
{
	unsigned char ram_table[IRAM_SIZE];
	bool result = false;

	if (dmcu == NULL)
		return false;

	if (dmcu && !dmcu->funcs->is_dmcu_initialized(dmcu))
		return true;

	memset(&ram_table, 0, sizeof(ram_table));

	if (dmcu->dmcu_version.abm_version == 0x24) {
		fill_iram_v_2_3((struct iram_table_v_2_2 *)ram_table, params, true);
			result = dmcu->funcs->load_iram(
					dmcu, 0, (char *)(&ram_table), IRAM_RESERVE_AREA_START_V2_2);
	} else if (dmcu->dmcu_version.abm_version == 0x23) {
		fill_iram_v_2_3((struct iram_table_v_2_2 *)ram_table, params, true);

		result = dmcu->funcs->load_iram(
				dmcu, 0, (char *)(&ram_table), IRAM_RESERVE_AREA_START_V2_2);
	} else if (dmcu->dmcu_version.abm_version == 0x22) {
		fill_iram_v_2_2((struct iram_table_v_2_2 *)ram_table, params);

		result = dmcu->funcs->load_iram(
				dmcu, 0, (char *)(&ram_table), IRAM_RESERVE_AREA_START_V2_2);
	} else {
		fill_iram_v_2((struct iram_table_v_2 *)ram_table, params);

		result = dmcu->funcs->load_iram(
				dmcu, 0, (char *)(&ram_table), IRAM_RESERVE_AREA_START_V2);

		if (result)
			result = dmcu->funcs->load_iram(
					dmcu, IRAM_RESERVE_AREA_END_V2 + 1,
					(char *)(&ram_table) + IRAM_RESERVE_AREA_END_V2 + 1,
					sizeof(ram_table) - IRAM_RESERVE_AREA_END_V2 - 1);
	}

	return result;
}

