/*
 * Copyright 2012-15 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
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

#ifndef __DC_TIMING_GENERATOR_DCN10_H__
#define __DC_TIMING_GENERATOR_DCN10_H__

#include "timing_generator.h"

#define DCN10TG_FROM_TG(tg)\
	container_of(tg, struct optc, base)

#define TG_COMMON_REG_LIST_DCN(inst) \
	SRI(OTG_VSTARTUP_PARAM, OTG, inst),\
	SRI(OTG_VUPDATE_PARAM, OTG, inst),\
	SRI(OTG_VREADY_PARAM, OTG, inst),\
	SRI(OTG_BLANK_CONTROL, OTG, inst),\
	SRI(OTG_MASTER_UPDATE_LOCK, OTG, inst),\
	SRI(OTG_GLOBAL_CONTROL0, OTG, inst),\
	SRI(OTG_DOUBLE_BUFFER_CONTROL, OTG, inst),\
	SRI(OTG_H_TOTAL, OTG, inst),\
	SRI(OTG_H_BLANK_START_END, OTG, inst),\
	SRI(OTG_H_SYNC_A, OTG, inst),\
	SRI(OTG_H_SYNC_A_CNTL, OTG, inst),\
	SRI(OTG_H_TIMING_CNTL, OTG, inst),\
	SRI(OTG_V_TOTAL, OTG, inst),\
	SRI(OTG_V_BLANK_START_END, OTG, inst),\
	SRI(OTG_V_SYNC_A, OTG, inst),\
	SRI(OTG_V_SYNC_A_CNTL, OTG, inst),\
	SRI(OTG_INTERLACE_CONTROL, OTG, inst),\
	SRI(OTG_CONTROL, OTG, inst),\
	SRI(OTG_STEREO_CONTROL, OTG, inst),\
	SRI(OTG_3D_STRUCTURE_CONTROL, OTG, inst),\
	SRI(OTG_STEREO_STATUS, OTG, inst),\
	SRI(OTG_V_TOTAL_MAX, OTG, inst),\
	SRI(OTG_V_TOTAL_MID, OTG, inst),\
	SRI(OTG_V_TOTAL_MIN, OTG, inst),\
	SRI(OTG_V_TOTAL_CONTROL, OTG, inst),\
	SRI(OTG_TRIGA_CNTL, OTG, inst),\
	SRI(OTG_FORCE_COUNT_NOW_CNTL, OTG, inst),\
	SRI(OTG_STATIC_SCREEN_CONTROL, OTG, inst),\
	SRI(OTG_STATUS_FRAME_COUNT, OTG, inst),\
	SRI(OTG_STATUS, OTG, inst),\
	SRI(OTG_STATUS_POSITION, OTG, inst),\
	SRI(OTG_NOM_VERT_POSITION, OTG, inst),\
	SRI(OTG_BLACK_COLOR, OTG, inst),\
	SRI(OTG_CLOCK_CONTROL, OTG, inst),\
	SRI(OTG_VERTICAL_INTERRUPT0_CONTROL, OTG, inst),\
	SRI(OTG_VERTICAL_INTERRUPT0_POSITION, OTG, inst),\
	SRI(OTG_VERTICAL_INTERRUPT1_CONTROL, OTG, inst),\
	SRI(OTG_VERTICAL_INTERRUPT1_POSITION, OTG, inst),\
	SRI(OTG_VERTICAL_INTERRUPT2_CONTROL, OTG, inst),\
	SRI(OTG_VERTICAL_INTERRUPT2_POSITION, OTG, inst),\
	SRI(OPTC_INPUT_CLOCK_CONTROL, ODM, inst),\
	SRI(OPTC_DATA_SOURCE_SELECT, ODM, inst),\
	SRI(OPTC_INPUT_GLOBAL_CONTROL, ODM, inst),\
	SRI(CONTROL, VTG, inst),\
	SRI(OTG_VERT_SYNC_CONTROL, OTG, inst),\
	SRI(OTG_MASTER_UPDATE_MODE, OTG, inst),\
	SRI(OTG_GSL_CONTROL, OTG, inst),\
	SRI(OTG_CRC_CNTL, OTG, inst),\
	SRI(OTG_CRC0_DATA_RG, OTG, inst),\
	SRI(OTG_CRC0_DATA_B, OTG, inst),\
	SRI(OTG_CRC0_WINDOWA_X_CONTROL, OTG, inst),\
	SRI(OTG_CRC0_WINDOWA_Y_CONTROL, OTG, inst),\
	SRI(OTG_CRC0_WINDOWB_X_CONTROL, OTG, inst),\
	SRI(OTG_CRC0_WINDOWB_Y_CONTROL, OTG, inst),\
	SR(GSL_SOURCE_SELECT),\
	SRI(OTG_GLOBAL_CONTROL2, OTG, inst),\
	SRI(OTG_TRIGA_MANUAL_TRIG, OTG, inst)

#define TG_COMMON_REG_LIST_DCN1_0(inst) \
	TG_COMMON_REG_LIST_DCN(inst),\
	SRI(OTG_TEST_PATTERN_PARAMETERS, OTG, inst),\
	SRI(OTG_TEST_PATTERN_CONTROL, OTG, inst),\
	SRI(OTG_TEST_PATTERN_COLOR, OTG, inst),\
	SRI(OTG_MANUAL_FLOW_CONTROL, OTG, inst)


struct dcn_optc_registers {
	uint32_t OTG_GLOBAL_CONTROL1;
	uint32_t OTG_GLOBAL_CONTROL2;
	uint32_t OTG_VERT_SYNC_CONTROL;
	uint32_t OTG_MASTER_UPDATE_MODE;
	uint32_t OTG_GSL_CONTROL;
	uint32_t OTG_VSTARTUP_PARAM;
	uint32_t OTG_VUPDATE_PARAM;
	uint32_t OTG_VREADY_PARAM;
	uint32_t OTG_BLANK_CONTROL;
	uint32_t OTG_MASTER_UPDATE_LOCK;
	uint32_t OTG_GLOBAL_CONTROL0;
	uint32_t OTG_DOUBLE_BUFFER_CONTROL;
	uint32_t OTG_H_TOTAL;
	uint32_t OTG_H_BLANK_START_END;
	uint32_t OTG_H_SYNC_A;
	uint32_t OTG_H_SYNC_A_CNTL;
	uint32_t OTG_H_TIMING_CNTL;
	uint32_t OTG_V_TOTAL;
	uint32_t OTG_V_BLANK_START_END;
	uint32_t OTG_V_SYNC_A;
	uint32_t OTG_V_SYNC_A_CNTL;
	uint32_t OTG_INTERLACE_CONTROL;
	uint32_t OTG_CONTROL;
	uint32_t OTG_STEREO_CONTROL;
	uint32_t OTG_3D_STRUCTURE_CONTROL;
	uint32_t OTG_STEREO_STATUS;
	uint32_t OTG_V_TOTAL_MAX;
	uint32_t OTG_V_TOTAL_MID;
	uint32_t OTG_V_TOTAL_MIN;
	uint32_t OTG_V_TOTAL_CONTROL;
	uint32_t OTG_TRIGA_CNTL;
	uint32_t OTG_TRIGA_MANUAL_TRIG;
	uint32_t OTG_MANUAL_FLOW_CONTROL;
	uint32_t OTG_FORCE_COUNT_NOW_CNTL;
	uint32_t OTG_STATIC_SCREEN_CONTROL;
	uint32_t OTG_STATUS_FRAME_COUNT;
	uint32_t OTG_STATUS;
	uint32_t OTG_STATUS_POSITION;
	uint32_t OTG_NOM_VERT_POSITION;
	uint32_t OTG_BLACK_COLOR;
	uint32_t OTG_TEST_PATTERN_PARAMETERS;
	uint32_t OTG_TEST_PATTERN_CONTROL;
	uint32_t OTG_TEST_PATTERN_COLOR;
	uint32_t OTG_CLOCK_CONTROL;
	uint32_t OTG_VERTICAL_INTERRUPT0_CONTROL;
	uint32_t OTG_VERTICAL_INTERRUPT0_POSITION;
	uint32_t OTG_VERTICAL_INTERRUPT1_CONTROL;
	uint32_t OTG_VERTICAL_INTERRUPT1_POSITION;
	uint32_t OTG_VERTICAL_INTERRUPT2_CONTROL;
	uint32_t OTG_VERTICAL_INTERRUPT2_POSITION;
	uint32_t OPTC_INPUT_CLOCK_CONTROL;
	uint32_t OPTC_DATA_SOURCE_SELECT;
	uint32_t OPTC_MEMORY_CONFIG;
	uint32_t OPTC_INPUT_GLOBAL_CONTROL;
	uint32_t CONTROL;
	uint32_t OTG_GSL_WINDOW_X;
	uint32_t OTG_GSL_WINDOW_Y;
	uint32_t OTG_VUPDATE_KEEPOUT;
	uint32_t OTG_CRC_CNTL;
	uint32_t OTG_CRC_CNTL2;
	uint32_t OTG_CRC0_DATA_RG;
	uint32_t OTG_CRC0_DATA_B;
	uint32_t OTG_CRC0_WINDOWA_X_CONTROL;
	uint32_t OTG_CRC0_WINDOWA_Y_CONTROL;
	uint32_t OTG_CRC0_WINDOWB_X_CONTROL;
	uint32_t OTG_CRC0_WINDOWB_Y_CONTROL;
	uint32_t GSL_SOURCE_SELECT;
#ifdef CONFIG_DRM_AMD_DC_DCN2_x
	uint32_t DWB_SOURCE_SELECT;
	uint32_t OTG_DSC_START_POSITION;
	uint32_t OPTC_DATA_FORMAT_CONTROL;
	uint32_t OPTC_BYTES_PER_PIXEL;
	uint32_t OPTC_WIDTH_CONTROL;
#endif
	uint32_t OTG_DRR_CONTROL;
#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	uint32_t OTG_BLANK_DATA_COLOR;
	uint32_t OTG_BLANK_DATA_COLOR_EXT;
	uint32_t OTG_DRR_TRIGGER_WINDOW;
	uint32_t OTG_M_CONST_DTO0;
	uint32_t OTG_M_CONST_DTO1;
	uint32_t OTG_DRR_V_TOTAL_CHANGE;
	uint32_t OTG_GLOBAL_CONTROL4;
#endif
};

#define TG_COMMON_MASK_SH_LIST_DCN(mask_sh)\
	SF(OTG0_OTG_VSTARTUP_PARAM, VSTARTUP_START, mask_sh),\
	SF(OTG0_OTG_VUPDATE_PARAM, VUPDATE_OFFSET, mask_sh),\
	SF(OTG0_OTG_VUPDATE_PARAM, VUPDATE_WIDTH, mask_sh),\
	SF(OTG0_OTG_VREADY_PARAM, VREADY_OFFSET, mask_sh),\
	SF(OTG0_OTG_BLANK_CONTROL, OTG_BLANK_DATA_EN, mask_sh),\
	SF(OTG0_OTG_BLANK_CONTROL, OTG_BLANK_DE_MODE, mask_sh),\
	SF(OTG0_OTG_BLANK_CONTROL, OTG_CURRENT_BLANK_STATE, mask_sh),\
	SF(OTG0_OTG_MASTER_UPDATE_LOCK, OTG_MASTER_UPDATE_LOCK, mask_sh),\
	SF(OTG0_OTG_MASTER_UPDATE_LOCK, UPDATE_LOCK_STATUS, mask_sh),\
	SF(OTG0_OTG_GLOBAL_CONTROL0, OTG_MASTER_UPDATE_LOCK_SEL, mask_sh),\
	SF(OTG0_OTG_DOUBLE_BUFFER_CONTROL, OTG_UPDATE_PENDING, mask_sh),\
	SF(OTG0_OTG_DOUBLE_BUFFER_CONTROL, OTG_BLANK_DATA_DOUBLE_BUFFER_EN, mask_sh),\
	SF(OTG0_OTG_DOUBLE_BUFFER_CONTROL, OTG_RANGE_TIMING_DBUF_UPDATE_MODE, mask_sh),\
	SF(OTG0_OTG_VUPDATE_KEEPOUT, OTG_MASTER_UPDATE_LOCK_VUPDATE_KEEPOUT_EN, mask_sh), \
	SF(OTG0_OTG_VUPDATE_KEEPOUT, MASTER_UPDATE_LOCK_VUPDATE_KEEPOUT_START_OFFSET, mask_sh), \
	SF(OTG0_OTG_VUPDATE_KEEPOUT, MASTER_UPDATE_LOCK_VUPDATE_KEEPOUT_END_OFFSET, mask_sh), \
	SF(OTG0_OTG_H_TOTAL, OTG_H_TOTAL, mask_sh),\
	SF(OTG0_OTG_H_BLANK_START_END, OTG_H_BLANK_START, mask_sh),\
	SF(OTG0_OTG_H_BLANK_START_END, OTG_H_BLANK_END, mask_sh),\
	SF(OTG0_OTG_H_SYNC_A, OTG_H_SYNC_A_START, mask_sh),\
	SF(OTG0_OTG_H_SYNC_A, OTG_H_SYNC_A_END, mask_sh),\
	SF(OTG0_OTG_H_SYNC_A_CNTL, OTG_H_SYNC_A_POL, mask_sh),\
	SF(OTG0_OTG_H_TIMING_CNTL, OTG_H_TIMING_DIV_BY2, mask_sh),\
	SF(OTG0_OTG_V_TOTAL, OTG_V_TOTAL, mask_sh),\
	SF(OTG0_OTG_V_BLANK_START_END, OTG_V_BLANK_START, mask_sh),\
	SF(OTG0_OTG_V_BLANK_START_END, OTG_V_BLANK_END, mask_sh),\
	SF(OTG0_OTG_V_SYNC_A, OTG_V_SYNC_A_START, mask_sh),\
	SF(OTG0_OTG_V_SYNC_A, OTG_V_SYNC_A_END, mask_sh),\
	SF(OTG0_OTG_V_SYNC_A_CNTL, OTG_V_SYNC_A_POL, mask_sh),\
	SF(OTG0_OTG_INTERLACE_CONTROL, OTG_INTERLACE_ENABLE, mask_sh),\
	SF(OTG0_OTG_CONTROL, OTG_MASTER_EN, mask_sh),\
	SF(OTG0_OTG_CONTROL, OTG_START_POINT_CNTL, mask_sh),\
	SF(OTG0_OTG_CONTROL, OTG_DISABLE_POINT_CNTL, mask_sh),\
	SF(OTG0_OTG_CONTROL, OTG_FIELD_NUMBER_CNTL, mask_sh),\
	SF(OTG0_OTG_CONTROL, OTG_CURRENT_MASTER_EN_STATE, mask_sh),\
	SF(OTG0_OTG_STEREO_CONTROL, OTG_STEREO_EN, mask_sh),\
	SF(OTG0_OTG_STEREO_CONTROL, OTG_STEREO_SYNC_OUTPUT_LINE_NUM, mask_sh),\
	SF(OTG0_OTG_STEREO_CONTROL, OTG_STEREO_SYNC_OUTPUT_POLARITY, mask_sh),\
	SF(OTG0_OTG_STEREO_CONTROL, OTG_STEREO_EYE_FLAG_POLARITY, mask_sh),\
	SF(OTG0_OTG_STEREO_CONTROL, OTG_DISABLE_STEREOSYNC_OUTPUT_FOR_DP, mask_sh),\
	SF(OTG0_OTG_STEREO_CONTROL, OTG_DISABLE_STEREOSYNC_OUTPUT_FOR_DP, mask_sh),\
	SF(OTG0_OTG_STEREO_STATUS, OTG_STEREO_CURRENT_EYE, mask_sh),\
	SF(OTG0_OTG_3D_STRUCTURE_CONTROL, OTG_3D_STRUCTURE_EN, mask_sh),\
	SF(OTG0_OTG_3D_STRUCTURE_CONTROL, OTG_3D_STRUCTURE_V_UPDATE_MODE, mask_sh),\
	SF(OTG0_OTG_3D_STRUCTURE_CONTROL, OTG_3D_STRUCTURE_STEREO_SEL_OVR, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_MAX, OTG_V_TOTAL_MAX, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_MID, OTG_V_TOTAL_MID, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_MIN, OTG_V_TOTAL_MIN, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_CONTROL, OTG_V_TOTAL_MIN_SEL, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_CONTROL, OTG_V_TOTAL_MAX_SEL, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_CONTROL, OTG_FORCE_LOCK_ON_EVENT, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_CONTROL, OTG_SET_V_TOTAL_MIN_MASK_EN, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_CONTROL, OTG_SET_V_TOTAL_MIN_MASK, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_CONTROL, OTG_VTOTAL_MID_REPLACING_MAX_EN, mask_sh),\
	SF(OTG0_OTG_V_TOTAL_CONTROL, OTG_VTOTAL_MID_FRAME_NUM, mask_sh),\
	SF(OTG0_OTG_FORCE_COUNT_NOW_CNTL, OTG_FORCE_COUNT_NOW_CLEAR, mask_sh),\
	SF(OTG0_OTG_FORCE_COUNT_NOW_CNTL, OTG_FORCE_COUNT_NOW_MODE, mask_sh),\
	SF(OTG0_OTG_FORCE_COUNT_NOW_CNTL, OTG_FORCE_COUNT_NOW_OCCURRED, mask_sh),\
	SF(OTG0_OTG_TRIGA_CNTL, OTG_TRIGA_SOURCE_SELECT, mask_sh),\
	SF(OTG0_OTG_TRIGA_CNTL, OTG_TRIGA_SOURCE_PIPE_SELECT, mask_sh),\
	SF(OTG0_OTG_TRIGA_CNTL, OTG_TRIGA_RISING_EDGE_DETECT_CNTL, mask_sh),\
	SF(OTG0_OTG_TRIGA_CNTL, OTG_TRIGA_FALLING_EDGE_DETECT_CNTL, mask_sh),\
	SF(OTG0_OTG_TRIGA_CNTL, OTG_TRIGA_POLARITY_SELECT, mask_sh),\
	SF(OTG0_OTG_TRIGA_CNTL, OTG_TRIGA_FREQUENCY_SELECT, mask_sh),\
	SF(OTG0_OTG_TRIGA_CNTL, OTG_TRIGA_DELAY, mask_sh),\
	SF(OTG0_OTG_TRIGA_CNTL, OTG_TRIGA_CLEAR, mask_sh),\
	SF(OTG0_OTG_TRIGA_MANUAL_TRIG, OTG_TRIGA_MANUAL_TRIG, mask_sh),\
	SF(OTG0_OTG_STATIC_SCREEN_CONTROL, OTG_STATIC_SCREEN_EVENT_MASK, mask_sh),\
	SF(OTG0_OTG_STATIC_SCREEN_CONTROL, OTG_STATIC_SCREEN_FRAME_COUNT, mask_sh),\
	SF(OTG0_OTG_STATUS_FRAME_COUNT, OTG_FRAME_COUNT, mask_sh),\
	SF(OTG0_OTG_STATUS, OTG_V_BLANK, mask_sh),\
	SF(OTG0_OTG_STATUS, OTG_V_ACTIVE_DISP, mask_sh),\
	SF(OTG0_OTG_STATUS_POSITION, OTG_HORZ_COUNT, mask_sh),\
	SF(OTG0_OTG_STATUS_POSITION, OTG_VERT_COUNT, mask_sh),\
	SF(OTG0_OTG_NOM_VERT_POSITION, OTG_VERT_COUNT_NOM, mask_sh),\
	SF(OTG0_OTG_BLACK_COLOR, OTG_BLACK_COLOR_B_CB, mask_sh),\
	SF(OTG0_OTG_BLACK_COLOR, OTG_BLACK_COLOR_G_Y, mask_sh),\
	SF(OTG0_OTG_BLACK_COLOR, OTG_BLACK_COLOR_R_CR, mask_sh),\
	SF(OTG0_OTG_CLOCK_CONTROL, OTG_BUSY, mask_sh),\
	SF(OTG0_OTG_CLOCK_CONTROL, OTG_CLOCK_EN, mask_sh),\
	SF(OTG0_OTG_CLOCK_CONTROL, OTG_CLOCK_ON, mask_sh),\
	SF(OTG0_OTG_CLOCK_CONTROL, OTG_CLOCK_GATE_DIS, mask_sh),\
	SF(OTG0_OTG_VERTICAL_INTERRUPT0_CONTROL, OTG_VERTICAL_INTERRUPT0_INT_ENABLE, mask_sh),\
	SF(OTG0_OTG_VERTICAL_INTERRUPT0_POSITION, OTG_VERTICAL_INTERRUPT0_LINE_START, mask_sh),\
	SF(OTG0_OTG_VERTICAL_INTERRUPT0_POSITION, OTG_VERTICAL_INTERRUPT0_LINE_END, mask_sh),\
	SF(OTG0_OTG_VERTICAL_INTERRUPT1_CONTROL, OTG_VERTICAL_INTERRUPT1_INT_ENABLE, mask_sh),\
	SF(OTG0_OTG_VERTICAL_INTERRUPT1_POSITION, OTG_VERTICAL_INTERRUPT1_LINE_START, mask_sh),\
	SF(OTG0_OTG_VERTICAL_INTERRUPT2_CONTROL, OTG_VERTICAL_INTERRUPT2_INT_ENABLE, mask_sh),\
	SF(OTG0_OTG_VERTICAL_INTERRUPT2_POSITION, OTG_VERTICAL_INTERRUPT2_LINE_START, mask_sh),\
	SF(ODM0_OPTC_INPUT_CLOCK_CONTROL, OPTC_INPUT_CLK_EN, mask_sh),\
	SF(ODM0_OPTC_INPUT_CLOCK_CONTROL, OPTC_INPUT_CLK_ON, mask_sh),\
	SF(ODM0_OPTC_INPUT_CLOCK_CONTROL, OPTC_INPUT_CLK_GATE_DIS, mask_sh),\
	SF(ODM0_OPTC_INPUT_GLOBAL_CONTROL, OPTC_UNDERFLOW_OCCURRED_STATUS, mask_sh),\
	SF(ODM0_OPTC_INPUT_GLOBAL_CONTROL, OPTC_UNDERFLOW_CLEAR, mask_sh),\
	SF(VTG0_CONTROL, VTG0_ENABLE, mask_sh),\
	SF(VTG0_CONTROL, VTG0_FP2, mask_sh),\
	SF(VTG0_CONTROL, VTG0_VCOUNT_INIT, mask_sh),\
	SF(OTG0_OTG_VERT_SYNC_CONTROL, OTG_FORCE_VSYNC_NEXT_LINE_OCCURRED, mask_sh),\
	SF(OTG0_OTG_VERT_SYNC_CONTROL, OTG_FORCE_VSYNC_NEXT_LINE_CLEAR, mask_sh),\
	SF(OTG0_OTG_VERT_SYNC_CONTROL, OTG_AUTO_FORCE_VSYNC_MODE, mask_sh),\
	SF(OTG0_OTG_MASTER_UPDATE_MODE, MASTER_UPDATE_INTERLACED_MODE, mask_sh),\
	SF(OTG0_OTG_GSL_CONTROL, OTG_GSL0_EN, mask_sh),\
	SF(OTG0_OTG_GSL_CONTROL, OTG_GSL1_EN, mask_sh),\
	SF(OTG0_OTG_GSL_CONTROL, OTG_GSL2_EN, mask_sh),\
	SF(OTG0_OTG_GSL_CONTROL, OTG_GSL_MASTER_EN, mask_sh),\
	SF(OTG0_OTG_GSL_CONTROL, OTG_GSL_FORCE_DELAY, mask_sh),\
	SF(OTG0_OTG_GSL_CONTROL, OTG_GSL_CHECK_ALL_FIELDS, mask_sh),\
	SF(OTG0_OTG_CRC_CNTL, OTG_CRC_CONT_EN, mask_sh),\
	SF(OTG0_OTG_CRC_CNTL, OTG_CRC0_SELECT, mask_sh),\
	SF(OTG0_OTG_CRC_CNTL, OTG_CRC_EN, mask_sh),\
	SF(OTG0_OTG_CRC0_DATA_RG, CRC0_R_CR, mask_sh),\
	SF(OTG0_OTG_CRC0_DATA_RG, CRC0_G_Y, mask_sh),\
	SF(OTG0_OTG_CRC0_DATA_B, CRC0_B_CB, mask_sh),\
	SF(OTG0_OTG_CRC0_WINDOWA_X_CONTROL, OTG_CRC0_WINDOWA_X_START, mask_sh),\
	SF(OTG0_OTG_CRC0_WINDOWA_X_CONTROL, OTG_CRC0_WINDOWA_X_END, mask_sh),\
	SF(OTG0_OTG_CRC0_WINDOWA_Y_CONTROL, OTG_CRC0_WINDOWA_Y_START, mask_sh),\
	SF(OTG0_OTG_CRC0_WINDOWA_Y_CONTROL, OTG_CRC0_WINDOWA_Y_END, mask_sh),\
	SF(OTG0_OTG_CRC0_WINDOWB_X_CONTROL, OTG_CRC0_WINDOWB_X_START, mask_sh),\
	SF(OTG0_OTG_CRC0_WINDOWB_X_CONTROL, OTG_CRC0_WINDOWB_X_END, mask_sh),\
	SF(OTG0_OTG_CRC0_WINDOWB_Y_CONTROL, OTG_CRC0_WINDOWB_Y_START, mask_sh),\
	SF(OTG0_OTG_CRC0_WINDOWB_Y_CONTROL, OTG_CRC0_WINDOWB_Y_END, mask_sh),\
	SF(GSL_SOURCE_SELECT, GSL0_READY_SOURCE_SEL, mask_sh),\
	SF(GSL_SOURCE_SELECT, GSL1_READY_SOURCE_SEL, mask_sh),\
	SF(GSL_SOURCE_SELECT, GSL2_READY_SOURCE_SEL, mask_sh),\
	SF(OTG0_OTG_GLOBAL_CONTROL2, MANUAL_FLOW_CONTROL_SEL, mask_sh)



#define TG_COMMON_MASK_SH_LIST_DCN1_0(mask_sh)\
	TG_COMMON_MASK_SH_LIST_DCN(mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_PARAMETERS, OTG_TEST_PATTERN_INC0, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_PARAMETERS, OTG_TEST_PATTERN_INC1, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_PARAMETERS, OTG_TEST_PATTERN_VRES, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_PARAMETERS, OTG_TEST_PATTERN_HRES, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_PARAMETERS, OTG_TEST_PATTERN_RAMP0_OFFSET, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_CONTROL, OTG_TEST_PATTERN_EN, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_CONTROL, OTG_TEST_PATTERN_MODE, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_CONTROL, OTG_TEST_PATTERN_DYNAMIC_RANGE, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_CONTROL, OTG_TEST_PATTERN_COLOR_FORMAT, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_COLOR, OTG_TEST_PATTERN_MASK, mask_sh),\
	SF(OTG0_OTG_TEST_PATTERN_COLOR, OTG_TEST_PATTERN_DATA, mask_sh),\
	SF(ODM0_OPTC_DATA_SOURCE_SELECT, OPTC_SRC_SEL, mask_sh),\
	SF(OTG0_OTG_MANUAL_FLOW_CONTROL, MANUAL_FLOW_CONTROL, mask_sh),\

#define TG_REG_FIELD_LIST_DCN1_0(type) \
	type VSTARTUP_START;\
	type VUPDATE_OFFSET;\
	type VUPDATE_WIDTH;\
	type VREADY_OFFSET;\
	type OTG_BLANK_DATA_EN;\
	type OTG_BLANK_DE_MODE;\
	type OTG_CURRENT_BLANK_STATE;\
	type OTG_MASTER_UPDATE_LOCK;\
	type UPDATE_LOCK_STATUS;\
	type OTG_UPDATE_PENDING;\
	type OTG_MASTER_UPDATE_LOCK_SEL;\
	type OTG_BLANK_DATA_DOUBLE_BUFFER_EN;\
	type OTG_H_TOTAL;\
	type OTG_H_BLANK_START;\
	type OTG_H_BLANK_END;\
	type OTG_H_SYNC_A_START;\
	type OTG_H_SYNC_A_END;\
	type OTG_H_SYNC_A_POL;\
	type OTG_H_TIMING_DIV_BY2;\
	type OTG_V_TOTAL;\
	type OTG_V_BLANK_START;\
	type OTG_V_BLANK_END;\
	type OTG_V_SYNC_A_START;\
	type OTG_V_SYNC_A_END;\
	type OTG_V_SYNC_A_POL;\
	type OTG_INTERLACE_ENABLE;\
	type OTG_MASTER_EN;\
	type OTG_START_POINT_CNTL;\
	type OTG_DISABLE_POINT_CNTL;\
	type OTG_FIELD_NUMBER_CNTL;\
	type OTG_CURRENT_MASTER_EN_STATE;\
	type OTG_STEREO_EN;\
	type OTG_STEREO_SYNC_OUTPUT_LINE_NUM;\
	type OTG_STEREO_SYNC_OUTPUT_POLARITY;\
	type OTG_STEREO_EYE_FLAG_POLARITY;\
	type OTG_STEREO_CURRENT_EYE;\
	type OTG_DISABLE_STEREOSYNC_OUTPUT_FOR_DP;\
	type OTG_3D_STRUCTURE_EN;\
	type OTG_3D_STRUCTURE_V_UPDATE_MODE;\
	type OTG_3D_STRUCTURE_STEREO_SEL_OVR;\
	type OTG_V_TOTAL_MAX;\
	type OTG_V_TOTAL_MID;\
	type OTG_V_TOTAL_MIN;\
	type OTG_V_TOTAL_MIN_SEL;\
	type OTG_V_TOTAL_MAX_SEL;\
	type OTG_VTOTAL_MID_REPLACING_MAX_EN;\
	type OTG_VTOTAL_MID_FRAME_NUM;\
	type OTG_FORCE_LOCK_ON_EVENT;\
	type OTG_SET_V_TOTAL_MIN_MASK_EN;\
	type OTG_SET_V_TOTAL_MIN_MASK;\
	type OTG_FORCE_COUNT_NOW_CLEAR;\
	type OTG_FORCE_COUNT_NOW_MODE;\
	type OTG_FORCE_COUNT_NOW_OCCURRED;\
	type OTG_TRIGA_SOURCE_SELECT;\
	type OTG_TRIGA_SOURCE_PIPE_SELECT;\
	type OTG_TRIGA_RISING_EDGE_DETECT_CNTL;\
	type OTG_TRIGA_FALLING_EDGE_DETECT_CNTL;\
	type OTG_TRIGA_POLARITY_SELECT;\
	type OTG_TRIGA_FREQUENCY_SELECT;\
	type OTG_TRIGA_DELAY;\
	type OTG_TRIGA_CLEAR;\
	type OTG_TRIGA_MANUAL_TRIG;\
	type OTG_STATIC_SCREEN_EVENT_MASK;\
	type OTG_STATIC_SCREEN_FRAME_COUNT;\
	type OTG_FRAME_COUNT;\
	type OTG_V_BLANK;\
	type OTG_V_ACTIVE_DISP;\
	type OTG_HORZ_COUNT;\
	type OTG_VERT_COUNT;\
	type OTG_VERT_COUNT_NOM;\
	type OTG_BLACK_COLOR_B_CB;\
	type OTG_BLACK_COLOR_G_Y;\
	type OTG_BLACK_COLOR_R_CR;\
	type OTG_BLANK_DATA_COLOR_BLUE_CB;\
	type OTG_BLANK_DATA_COLOR_GREEN_Y;\
	type OTG_BLANK_DATA_COLOR_RED_CR;\
	type OTG_BLANK_DATA_COLOR_BLUE_CB_EXT;\
	type OTG_BLANK_DATA_COLOR_GREEN_Y_EXT;\
	type OTG_BLANK_DATA_COLOR_RED_CR_EXT;\
	type OTG_VTOTAL_MID_REPLACING_MIN_EN;\
	type OTG_TEST_PATTERN_INC0;\
	type OTG_TEST_PATTERN_INC1;\
	type OTG_TEST_PATTERN_VRES;\
	type OTG_TEST_PATTERN_HRES;\
	type OTG_TEST_PATTERN_RAMP0_OFFSET;\
	type OTG_TEST_PATTERN_EN;\
	type OTG_TEST_PATTERN_MODE;\
	type OTG_TEST_PATTERN_DYNAMIC_RANGE;\
	type OTG_TEST_PATTERN_COLOR_FORMAT;\
	type OTG_TEST_PATTERN_MASK;\
	type OTG_TEST_PATTERN_DATA;\
	type OTG_BUSY;\
	type OTG_CLOCK_EN;\
	type OTG_CLOCK_ON;\
	type OTG_CLOCK_GATE_DIS;\
	type OTG_VERTICAL_INTERRUPT0_INT_ENABLE;\
	type OTG_VERTICAL_INTERRUPT0_LINE_START;\
	type OTG_VERTICAL_INTERRUPT0_LINE_END;\
	type OTG_VERTICAL_INTERRUPT1_INT_ENABLE;\
	type OTG_VERTICAL_INTERRUPT1_LINE_START;\
	type OTG_VERTICAL_INTERRUPT2_INT_ENABLE;\
	type OTG_VERTICAL_INTERRUPT2_LINE_START;\
	type OPTC_INPUT_CLK_EN;\
	type OPTC_INPUT_CLK_ON;\
	type OPTC_INPUT_CLK_GATE_DIS;\
	type OPTC_UNDERFLOW_OCCURRED_STATUS;\
	type OPTC_UNDERFLOW_CLEAR;\
	type OPTC_SRC_SEL;\
	type VTG0_ENABLE;\
	type VTG0_FP2;\
	type VTG0_VCOUNT_INIT;\
	type OTG_FORCE_VSYNC_NEXT_LINE_OCCURRED;\
	type OTG_FORCE_VSYNC_NEXT_LINE_CLEAR;\
	type OTG_AUTO_FORCE_VSYNC_MODE;\
	type MASTER_UPDATE_INTERLACED_MODE;\
	type OTG_GSL0_EN;\
	type OTG_GSL1_EN;\
	type OTG_GSL2_EN;\
	type OTG_GSL_MASTER_EN;\
	type OTG_GSL_FORCE_DELAY;\
	type OTG_GSL_CHECK_ALL_FIELDS;\
	type OTG_GSL_WINDOW_START_X;\
	type OTG_GSL_WINDOW_END_X;\
	type OTG_GSL_WINDOW_START_Y;\
	type OTG_GSL_WINDOW_END_Y;\
	type OTG_RANGE_TIMING_DBUF_UPDATE_MODE;\
	type OTG_GSL_MASTER_MODE;\
	type OTG_MASTER_UPDATE_LOCK_GSL_EN;\
	type MASTER_UPDATE_LOCK_VUPDATE_KEEPOUT_START_OFFSET;\
	type MASTER_UPDATE_LOCK_VUPDATE_KEEPOUT_END_OFFSET;\
	type OTG_MASTER_UPDATE_LOCK_VUPDATE_KEEPOUT_EN;\
	type OTG_CRC_CONT_EN;\
	type OTG_CRC0_SELECT;\
	type OTG_CRC_EN;\
	type CRC0_R_CR;\
	type CRC0_G_Y;\
	type CRC0_B_CB;\
	type OTG_CRC0_WINDOWA_X_START;\
	type OTG_CRC0_WINDOWA_X_END;\
	type OTG_CRC0_WINDOWA_Y_START;\
	type OTG_CRC0_WINDOWA_Y_END;\
	type OTG_CRC0_WINDOWB_X_START;\
	type OTG_CRC0_WINDOWB_X_END;\
	type OTG_CRC0_WINDOWB_Y_START;\
	type OTG_CRC0_WINDOWB_Y_END;\
	type GSL0_READY_SOURCE_SEL;\
	type GSL1_READY_SOURCE_SEL;\
	type GSL2_READY_SOURCE_SEL;\
	type MANUAL_FLOW_CONTROL;\
	type MANUAL_FLOW_CONTROL_SEL;

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)

#define TG_REG_FIELD_LIST(type) \
	TG_REG_FIELD_LIST_DCN1_0(type)\
	type OTG_V_SYNC_MODE;\
	type OTG_DRR_TRIGGER_WINDOW_START_X;\
	type OTG_DRR_TRIGGER_WINDOW_END_X;\
	type OTG_DRR_V_TOTAL_CHANGE_LIMIT;\
	type OTG_OUT_MUX;\
	type OTG_M_CONST_DTO_PHASE;\
	type OTG_M_CONST_DTO_MODULO;\
	type MASTER_UPDATE_LOCK_DB_X;\
	type MASTER_UPDATE_LOCK_DB_Y;\
	type MASTER_UPDATE_LOCK_DB_EN;\
	type GLOBAL_UPDATE_LOCK_EN;\
	type DIG_UPDATE_LOCATION;\
	type OTG_DSC_START_POSITION_X;\
	type OTG_DSC_START_POSITION_LINE_NUM;\
	type OPTC_NUM_OF_INPUT_SEGMENT;\
	type OPTC_SEG0_SRC_SEL;\
	type OPTC_SEG1_SRC_SEL;\
	type OPTC_SEG2_SRC_SEL;\
	type OPTC_SEG3_SRC_SEL;\
	type OPTC_MEM_SEL;\
	type OPTC_DATA_FORMAT;\
	type OPTC_DSC_MODE;\
	type OPTC_DSC_BYTES_PER_PIXEL;\
	type OPTC_DSC_SLICE_WIDTH;\
	type OPTC_SEGMENT_WIDTH;\
	type OPTC_DWB0_SOURCE_SELECT;\
	type OPTC_DWB1_SOURCE_SELECT;\
	type MASTER_UPDATE_LOCK_DB_START_X;\
	type MASTER_UPDATE_LOCK_DB_END_X;\
	type MASTER_UPDATE_LOCK_DB_START_Y;\
	type MASTER_UPDATE_LOCK_DB_END_Y;\
	type DIG_UPDATE_POSITION_X;\
	type DIG_UPDATE_POSITION_Y;\
	type OTG_H_TIMING_DIV_MODE;\
	type OTG_DRR_TIMING_DBUF_UPDATE_MODE;\
	type OTG_CRC_DSC_MODE;\
	type OTG_CRC_DATA_STREAM_COMBINE_MODE;\
	type OTG_CRC_DATA_STREAM_SPLIT_MODE;\
	type OTG_CRC_DATA_FORMAT;\
	type OTG_V_TOTAL_LAST_USED_BY_DRR;

#elif defined(CONFIG_DRM_AMD_DC_DCN2_x)
#define TG_REG_FIELD_LIST(type) \
        TG_REG_FIELD_LIST_DCN1_0(type)\
        type MASTER_UPDATE_LOCK_DB_X;\
        type MASTER_UPDATE_LOCK_DB_Y;\
        type MASTER_UPDATE_LOCK_DB_EN;\
        type GLOBAL_UPDATE_LOCK_EN;\
        type DIG_UPDATE_LOCATION;\
        type OTG_DSC_START_POSITION_X;\
        type OTG_DSC_START_POSITION_LINE_NUM;\
        type OPTC_NUM_OF_INPUT_SEGMENT;\
        type OPTC_SEG0_SRC_SEL;\
        type OPTC_SEG1_SRC_SEL;\
        type OPTC_MEM_SEL;\
        type OPTC_DATA_FORMAT;\
        type OPTC_DSC_MODE;\
        type OPTC_DSC_BYTES_PER_PIXEL;\
        type OPTC_DSC_SLICE_WIDTH;\
        type OPTC_SEGMENT_WIDTH;\
        type OPTC_DWB0_SOURCE_SELECT;\
        type OPTC_DWB1_SOURCE_SELECT;\
        type OTG_CRC_DSC_MODE;\
        type OTG_CRC_DATA_STREAM_COMBINE_MODE;\
        type OTG_CRC_DATA_STREAM_SPLIT_MODE;\
        type OTG_CRC_DATA_FORMAT;
#else

#define TG_REG_FIELD_LIST(type) \
        TG_REG_FIELD_LIST_DCN1_0(type)

#endif


struct dcn_optc_shift {
	TG_REG_FIELD_LIST(uint8_t)
};

struct dcn_optc_mask {
	TG_REG_FIELD_LIST(uint32_t)
};

struct optc {
	struct timing_generator base;

	const struct dcn_optc_registers *tg_regs;
	const struct dcn_optc_shift *tg_shift;
	const struct dcn_optc_mask *tg_mask;

	int opp_count;

	uint32_t max_h_total;
	uint32_t max_v_total;

	uint32_t min_h_blank;

	uint32_t min_h_sync_width;
	uint32_t min_v_sync_width;
	uint32_t min_v_blank;
	uint32_t min_v_blank_interlace;

	int vstartup_start;
	int vupdate_offset;
	int vupdate_width;
	int vready_offset;
	enum signal_type signal;
};

void dcn10_timing_generator_init(struct optc *optc);

struct dcn_otg_state {
	uint32_t v_blank_start;
	uint32_t v_blank_end;
	uint32_t v_sync_a_pol;
	uint32_t v_total;
	uint32_t v_total_max;
	uint32_t v_total_min;
	uint32_t v_total_min_sel;
	uint32_t v_total_max_sel;
	uint32_t v_sync_a_start;
	uint32_t v_sync_a_end;
	uint32_t h_blank_start;
	uint32_t h_blank_end;
	uint32_t h_sync_a_start;
	uint32_t h_sync_a_end;
	uint32_t h_sync_a_pol;
	uint32_t h_total;
	uint32_t underflow_occurred_status;
	uint32_t otg_enabled;
	uint32_t blank_enabled;
	uint32_t vertical_interrupt2_en;
	uint32_t vertical_interrupt2_line;
};

void optc1_read_otg_state(struct optc *optc1,
		struct dcn_otg_state *s);

bool optc1_get_hw_timing(struct timing_generator *tg,
		struct dc_crtc_timing *hw_crtc_timing);

bool optc1_validate_timing(
	struct timing_generator *optc,
	const struct dc_crtc_timing *timing);

void optc1_program_timing(
	struct timing_generator *optc,
	const struct dc_crtc_timing *dc_crtc_timing,
	int vready_offset,
	int vstartup_start,
	int vupdate_offset,
	int vupdate_width,
	const enum signal_type signal,
	bool use_vbios);

void optc1_setup_vertical_interrupt0(
		struct timing_generator *optc,
		uint32_t start_line,
		uint32_t end_line);
void optc1_setup_vertical_interrupt1(
		struct timing_generator *optc,
		uint32_t start_line);
void optc1_setup_vertical_interrupt2(
		struct timing_generator *optc,
		uint32_t start_line);

void optc1_program_global_sync(
		struct timing_generator *optc,
		int vready_offset,
		int vstartup_start,
		int vupdate_offset,
		int vupdate_width);

bool optc1_disable_crtc(struct timing_generator *optc);

bool optc1_is_counter_moving(struct timing_generator *optc);

void optc1_get_position(struct timing_generator *optc,
		struct crtc_position *position);

uint32_t optc1_get_vblank_counter(struct timing_generator *optc);

void optc1_get_crtc_scanoutpos(
	struct timing_generator *optc,
	uint32_t *v_blank_start,
	uint32_t *v_blank_end,
	uint32_t *h_position,
	uint32_t *v_position);

void optc1_set_early_control(
	struct timing_generator *optc,
	uint32_t early_cntl);

void optc1_wait_for_state(struct timing_generator *optc,
		enum crtc_state state);

void optc1_set_blank(struct timing_generator *optc,
		bool enable_blanking);

bool optc1_is_blanked(struct timing_generator *optc);
bool optc1_is_locked(struct timing_generator *optc);

void optc1_program_blank_color(
		struct timing_generator *optc,
		const struct tg_color *black_color);

bool optc1_did_triggered_reset_occur(
	struct timing_generator *optc);

void optc1_enable_reset_trigger(struct timing_generator *optc, int source_tg_inst);

void optc1_disable_reset_trigger(struct timing_generator *optc);

void optc1_lock(struct timing_generator *optc);

void optc1_unlock(struct timing_generator *optc);

void optc1_enable_optc_clock(struct timing_generator *optc, bool enable);

void optc1_set_drr(
	struct timing_generator *optc,
	const struct drr_params *params);

void optc1_set_vtotal_min_max(struct timing_generator *optc, int vtotal_min, int vtotal_max);

void optc1_set_static_screen_control(
	struct timing_generator *optc,
	uint32_t event_triggers,
	uint32_t num_frames);

void optc1_program_stereo(struct timing_generator *optc,
	const struct dc_crtc_timing *timing, struct crtc_stereo_flags *flags);

bool optc1_is_stereo_left_eye(struct timing_generator *optc);

void optc1_clear_optc_underflow(struct timing_generator *optc);

void optc1_tg_init(struct timing_generator *optc);

bool optc1_is_tg_enabled(struct timing_generator *optc);

bool optc1_is_optc_underflow_occurred(struct timing_generator *optc);

void optc1_set_blank_data_double_buffer(struct timing_generator *optc, bool enable);

void optc1_set_timing_double_buffer(struct timing_generator *optc, bool enable);

bool optc1_get_otg_active_size(struct timing_generator *optc,
		uint32_t *otg_active_width,
		uint32_t *otg_active_height);

void optc1_enable_crtc_reset(
		struct timing_generator *optc,
		int source_tg_inst,
		struct crtc_trigger_info *crtc_tp);

bool optc1_configure_crc(struct timing_generator *optc,
			  const struct crc_params *params);

bool optc1_get_crc(struct timing_generator *optc,
		    uint32_t *r_cr, uint32_t *g_y, uint32_t *b_cb);

bool optc1_is_two_pixels_per_containter(const struct dc_crtc_timing *timing);

void optc1_set_vtg_params(struct timing_generator *optc,
		const struct dc_crtc_timing *dc_crtc_timing, bool program_fp2);

#endif /* __DC_TIMING_GENERATOR_DCN10_H__ */
