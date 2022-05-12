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

#ifndef __GP_DEVICE_GLOBAL_H_INCLUDED__
#define __GP_DEVICE_GLOBAL_H_INCLUDED__

#define IS_GP_DEVICE_VERSION_2

#define _REG_GP_IRQ_REQ0_ADDR				0x08
#define _REG_GP_IRQ_REQ1_ADDR				0x0C
/* The SP sends SW interrupt info to this register */
#define _REG_GP_IRQ_REQUEST0_ADDR			_REG_GP_IRQ_REQ0_ADDR
#define _REG_GP_IRQ_REQUEST1_ADDR			_REG_GP_IRQ_REQ1_ADDR

/* The SP configures FIFO switches in these registers */
#define _REG_GP_SWITCH_IF_ADDR						0x40
#define _REG_GP_SWITCH_GDC1_ADDR					0x44
#define _REG_GP_SWITCH_GDC2_ADDR					0x48
/* @ INPUT_FORMATTER_BASE -> GP_DEVICE_BASE */
#define _REG_GP_IFMT_input_switch_lut_reg0			0x00030800
#define _REG_GP_IFMT_input_switch_lut_reg1			0x00030804
#define _REG_GP_IFMT_input_switch_lut_reg2			0x00030808
#define _REG_GP_IFMT_input_switch_lut_reg3			0x0003080C
#define _REG_GP_IFMT_input_switch_lut_reg4			0x00030810
#define _REG_GP_IFMT_input_switch_lut_reg5			0x00030814
#define _REG_GP_IFMT_input_switch_lut_reg6			0x00030818
#define _REG_GP_IFMT_input_switch_lut_reg7			0x0003081C
#define _REG_GP_IFMT_input_switch_fsync_lut			0x00030820
#define _REG_GP_IFMT_srst							0x00030824
#define _REG_GP_IFMT_slv_reg_srst					0x00030828
#define _REG_GP_IFMT_input_switch_ch_id_fmt_type	0x0003082C

/* @ GP_DEVICE_BASE */
#define _REG_GP_SYNCGEN_ENABLE_ADDR					0x00090000
#define _REG_GP_SYNCGEN_FREE_RUNNING_ADDR			0x00090004
#define _REG_GP_SYNCGEN_PAUSE_ADDR					0x00090008
#define _REG_GP_NR_FRAMES_ADDR						0x0009000C
#define _REG_GP_SYNGEN_NR_PIX_ADDR					0x00090010
#define _REG_GP_SYNGEN_NR_LINES_ADDR				0x00090014
#define _REG_GP_SYNGEN_HBLANK_CYCLES_ADDR			0x00090018
#define _REG_GP_SYNGEN_VBLANK_CYCLES_ADDR			0x0009001C
#define _REG_GP_ISEL_SOF_ADDR						0x00090020
#define _REG_GP_ISEL_EOF_ADDR						0x00090024
#define _REG_GP_ISEL_SOL_ADDR						0x00090028
#define _REG_GP_ISEL_EOL_ADDR						0x0009002C
#define _REG_GP_ISEL_LFSR_ENABLE_ADDR				0x00090030
#define _REG_GP_ISEL_LFSR_ENABLE_B_ADDR				0x00090034
#define _REG_GP_ISEL_LFSR_RESET_VALUE_ADDR			0x00090038
#define _REG_GP_ISEL_TPG_ENABLE_ADDR				0x0009003C
#define _REG_GP_ISEL_TPG_ENABLE_B_ADDR				0x00090040
#define _REG_GP_ISEL_HOR_CNT_MASK_ADDR				0x00090044
#define _REG_GP_ISEL_VER_CNT_MASK_ADDR				0x00090048
#define _REG_GP_ISEL_XY_CNT_MASK_ADDR				0x0009004C
#define _REG_GP_ISEL_HOR_CNT_DELTA_ADDR				0x00090050
#define _REG_GP_ISEL_VER_CNT_DELTA_ADDR				0x00090054
#define _REG_GP_ISEL_TPG_MODE_ADDR					0x00090058
#define _REG_GP_ISEL_TPG_RED1_ADDR					0x0009005C
#define _REG_GP_ISEL_TPG_GREEN1_ADDR				0x00090060
#define _REG_GP_ISEL_TPG_BLUE1_ADDR					0x00090064
#define _REG_GP_ISEL_TPG_RED2_ADDR					0x00090068
#define _REG_GP_ISEL_TPG_GREEN2_ADDR				0x0009006C
#define _REG_GP_ISEL_TPG_BLUE2_ADDR					0x00090070
#define _REG_GP_ISEL_CH_ID_ADDR						0x00090074
#define _REG_GP_ISEL_FMT_TYPE_ADDR					0x00090078
#define _REG_GP_ISEL_DATA_SEL_ADDR					0x0009007C
#define _REG_GP_ISEL_SBAND_SEL_ADDR					0x00090080
#define _REG_GP_ISEL_SYNC_SEL_ADDR					0x00090084
#define _REG_GP_SYNCGEN_HOR_CNT_ADDR				0x00090088
#define _REG_GP_SYNCGEN_VER_CNT_ADDR				0x0009008C
#define _REG_GP_SYNCGEN_FRAME_CNT_ADDR				0x00090090
#define _REG_GP_SOFT_RESET_ADDR						0x00090094

#endif /* __GP_DEVICE_GLOBAL_H_INCLUDED__ */
