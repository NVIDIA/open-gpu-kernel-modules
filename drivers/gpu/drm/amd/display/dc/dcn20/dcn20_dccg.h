/*
 * Copyright 2018 Advanced Micro Devices, Inc.
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

#ifndef __DCN20_DCCG_H__
#define __DCN20_DCCG_H__

#include "dccg.h"

#define DCCG_COMMON_REG_LIST_DCN_BASE() \
	SR(DPPCLK_DTO_CTRL),\
	DCCG_SRII(DTO_PARAM, DPPCLK, 0),\
	DCCG_SRII(DTO_PARAM, DPPCLK, 1),\
	DCCG_SRII(DTO_PARAM, DPPCLK, 2),\
	DCCG_SRII(DTO_PARAM, DPPCLK, 3),\
	SR(REFCLK_CNTL),\
	DCCG_SRII(PIXEL_RATE_CNTL, OTG, 0),\
	DCCG_SRII(PIXEL_RATE_CNTL, OTG, 1),\
	SR(DISPCLK_FREQ_CHANGE_CNTL)

#define DCCG_REG_LIST_DCN2() \
	DCCG_COMMON_REG_LIST_DCN_BASE(),\
	DCCG_SRII(DTO_PARAM, DPPCLK, 4),\
	DCCG_SRII(DTO_PARAM, DPPCLK, 5),\
	DCCG_SRII(PIXEL_RATE_CNTL, OTG, 2),\
	DCCG_SRII(PIXEL_RATE_CNTL, OTG, 3),\
	DCCG_SRII(PIXEL_RATE_CNTL, OTG, 4),\
	DCCG_SRII(PIXEL_RATE_CNTL, OTG, 5)

#define DCCG_SF(reg_name, field_name, post_fix)\
	.field_name = reg_name ## __ ## field_name ## post_fix

#define DCCG_SFI(reg_name, field_name, field_prefix, inst, post_fix)\
	.field_prefix ## _ ## field_name[inst] = reg_name ## __ ## field_prefix ## inst ## _ ## field_name ## post_fix

#define DCCG_SFII(block, reg_name, field_prefix, field_name, inst, post_fix)\
	.field_prefix ## _ ## field_name[inst] = block ## inst ## _ ## reg_name ## __ ## field_prefix ## inst ## _ ## field_name ## post_fix

#define DCCG_COMMON_MASK_SH_LIST_DCN_COMMON_BASE(mask_sh) \
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_ENABLE, DPPCLK, 0, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_DB_EN, DPPCLK, 0, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_ENABLE, DPPCLK, 1, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_DB_EN, DPPCLK, 1, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_ENABLE, DPPCLK, 2, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_DB_EN, DPPCLK, 2, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_ENABLE, DPPCLK, 3, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_DB_EN, DPPCLK, 3, mask_sh),\
	DCCG_SF(DPPCLK0_DTO_PARAM, DPPCLK0_DTO_PHASE, mask_sh),\
	DCCG_SF(DPPCLK0_DTO_PARAM, DPPCLK0_DTO_MODULO, mask_sh),\
	DCCG_SF(REFCLK_CNTL, REFCLK_CLOCK_EN, mask_sh),\
	DCCG_SF(REFCLK_CNTL, REFCLK_SRC_SEL, mask_sh),\
	DCCG_SF(DISPCLK_FREQ_CHANGE_CNTL, DISPCLK_STEP_DELAY, mask_sh),\
	DCCG_SF(DISPCLK_FREQ_CHANGE_CNTL, DISPCLK_STEP_SIZE, mask_sh),\
	DCCG_SF(DISPCLK_FREQ_CHANGE_CNTL, DISPCLK_FREQ_RAMP_DONE, mask_sh),\
	DCCG_SF(DISPCLK_FREQ_CHANGE_CNTL, DISPCLK_MAX_ERRDET_CYCLES, mask_sh),\
	DCCG_SF(DISPCLK_FREQ_CHANGE_CNTL, DCCG_FIFO_ERRDET_RESET, mask_sh),\
	DCCG_SF(DISPCLK_FREQ_CHANGE_CNTL, DCCG_FIFO_ERRDET_STATE, mask_sh),\
	DCCG_SF(DISPCLK_FREQ_CHANGE_CNTL, DCCG_FIFO_ERRDET_OVR_EN, mask_sh),\
	DCCG_SF(DISPCLK_FREQ_CHANGE_CNTL, DISPCLK_CHG_FWD_CORR_DISABLE, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, ADD_PIXEL, 0, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, ADD_PIXEL, 1, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, DROP_PIXEL, 0, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, DROP_PIXEL, 1, mask_sh)




#define DCCG_MASK_SH_LIST_DCN2(mask_sh) \
	DCCG_COMMON_MASK_SH_LIST_DCN_COMMON_BASE(mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_ENABLE, DPPCLK, 4, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_DB_EN, DPPCLK, 4, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_ENABLE, DPPCLK, 5, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_DB_EN, DPPCLK, 5, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, ADD_PIXEL, 2, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, ADD_PIXEL, 3, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, ADD_PIXEL, 4, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, ADD_PIXEL, 5, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, DROP_PIXEL, 2, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, DROP_PIXEL, 3, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, DROP_PIXEL, 4, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, DROP_PIXEL, 5, mask_sh)

#define DCCG_MASK_SH_LIST_DCN2_1(mask_sh) \
	DCCG_COMMON_MASK_SH_LIST_DCN_COMMON_BASE(mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_ENABLE, DPPCLK, 4, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_DB_EN, DPPCLK, 4, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_ENABLE, DPPCLK, 5, mask_sh),\
	DCCG_SFI(DPPCLK_DTO_CTRL, DTO_DB_EN, DPPCLK, 5, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, ADD_PIXEL, 2, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, ADD_PIXEL, 3, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, DROP_PIXEL, 2, mask_sh),\
	DCCG_SFII(OTG, PIXEL_RATE_CNTL, OTG, DROP_PIXEL, 3, mask_sh)


#define DCCG_REG_FIELD_LIST(type) \
	type DPPCLK0_DTO_PHASE;\
	type DPPCLK0_DTO_MODULO;\
	type DPPCLK_DTO_ENABLE[6];\
	type DPPCLK_DTO_DB_EN[6];\
	type REFCLK_CLOCK_EN;\
	type REFCLK_SRC_SEL;\
	type DISPCLK_STEP_DELAY;\
	type DISPCLK_STEP_SIZE;\
	type DISPCLK_FREQ_RAMP_DONE;\
	type DISPCLK_MAX_ERRDET_CYCLES;\
	type DCCG_FIFO_ERRDET_RESET;\
	type DCCG_FIFO_ERRDET_STATE;\
	type DCCG_FIFO_ERRDET_OVR_EN;\
	type DISPCLK_CHG_FWD_CORR_DISABLE;\
	type DISPCLK_FREQ_CHANGE_CNTL;\
	type OTG_ADD_PIXEL[MAX_PIPES];\
	type OTG_DROP_PIXEL[MAX_PIPES];

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
#define DCCG3_REG_FIELD_LIST(type) \
	type PHYASYMCLK_FORCE_EN;\
	type PHYASYMCLK_FORCE_SRC_SEL;\
	type PHYBSYMCLK_FORCE_EN;\
	type PHYBSYMCLK_FORCE_SRC_SEL;\
	type PHYCSYMCLK_FORCE_EN;\
	type PHYCSYMCLK_FORCE_SRC_SEL;

#define DCCG31_REG_FIELD_LIST(type) \
	type PHYDSYMCLK_FORCE_EN;\
	type PHYDSYMCLK_FORCE_SRC_SEL;\
	type PHYESYMCLK_FORCE_EN;\
	type PHYESYMCLK_FORCE_SRC_SEL;\
	type DPSTREAMCLK_PIPE0_EN;\
	type DPSTREAMCLK_PIPE1_EN;\
	type DPSTREAMCLK_PIPE2_EN;\
	type DPSTREAMCLK_PIPE3_EN;\
	type HDMISTREAMCLK0_SRC_SEL;\
	type HDMISTREAMCLK0_DTO_FORCE_DIS;\
	type SYMCLK32_SE0_SRC_SEL;\
	type SYMCLK32_SE1_SRC_SEL;\
	type SYMCLK32_SE2_SRC_SEL;\
	type SYMCLK32_SE3_SRC_SEL;\
	type SYMCLK32_SE0_EN;\
	type SYMCLK32_SE1_EN;\
	type SYMCLK32_SE2_EN;\
	type SYMCLK32_SE3_EN;\
	type SYMCLK32_LE0_SRC_SEL;\
	type SYMCLK32_LE1_SRC_SEL;\
	type SYMCLK32_LE0_EN;\
	type SYMCLK32_LE1_EN;\
	type DTBCLK_DTO_ENABLE[MAX_PIPES];\
	type DTBCLKDTO_ENABLE_STATUS[MAX_PIPES];\
	type PIPE_DTO_SRC_SEL[MAX_PIPES];\
	type DTBCLK_DTO_DIV[MAX_PIPES];\
	type DCCG_AUDIO_DTO_SEL;\
	type DCCG_AUDIO_DTO0_SOURCE_SEL;\
	type DENTIST_DISPCLK_CHG_MODE;\
	type DSCCLK0_DTO_PHASE;\
	type DSCCLK0_DTO_MODULO;\
	type DSCCLK1_DTO_PHASE;\
	type DSCCLK1_DTO_MODULO;\
	type DSCCLK2_DTO_PHASE;\
	type DSCCLK2_DTO_MODULO;\
	type DSCCLK0_DTO_ENABLE;\
	type DSCCLK1_DTO_ENABLE;\
	type DSCCLK2_DTO_ENABLE;\
	type SYMCLK32_ROOT_SE0_GATE_DISABLE;\
	type SYMCLK32_ROOT_SE1_GATE_DISABLE;\
	type SYMCLK32_ROOT_SE2_GATE_DISABLE;\
	type SYMCLK32_ROOT_SE3_GATE_DISABLE;\
	type SYMCLK32_SE0_GATE_DISABLE;\
	type SYMCLK32_SE1_GATE_DISABLE;\
	type SYMCLK32_SE2_GATE_DISABLE;\
	type SYMCLK32_SE3_GATE_DISABLE;\
	type SYMCLK32_ROOT_LE0_GATE_DISABLE;\
	type SYMCLK32_ROOT_LE1_GATE_DISABLE;\
	type SYMCLK32_LE0_GATE_DISABLE;\
	type SYMCLK32_LE1_GATE_DISABLE;\
	type DPSTREAMCLK_ROOT_GATE_DISABLE;\
	type DPSTREAMCLK_GATE_DISABLE;\
	type HDMISTREAMCLK0_DTO_PHASE;\
	type HDMISTREAMCLK0_DTO_MODULO;\
	type HDMICHARCLK0_GATE_DISABLE;\
	type HDMICHARCLK0_ROOT_GATE_DISABLE; \
	type PHYASYMCLK_GATE_DISABLE; \
	type PHYBSYMCLK_GATE_DISABLE; \
	type PHYCSYMCLK_GATE_DISABLE; \
	type PHYDSYMCLK_GATE_DISABLE; \
	type PHYESYMCLK_GATE_DISABLE;
#endif

struct dccg_shift {
	DCCG_REG_FIELD_LIST(uint8_t)
#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	DCCG3_REG_FIELD_LIST(uint8_t)
	DCCG31_REG_FIELD_LIST(uint8_t)
#endif
};

struct dccg_mask {
	DCCG_REG_FIELD_LIST(uint32_t)
#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	DCCG3_REG_FIELD_LIST(uint32_t)
	DCCG31_REG_FIELD_LIST(uint32_t)
#endif
};

struct dccg_registers {
	uint32_t DPPCLK_DTO_CTRL;
	uint32_t DPPCLK_DTO_PARAM[6];
	uint32_t REFCLK_CNTL;
	uint32_t DISPCLK_FREQ_CHANGE_CNTL;
	uint32_t OTG_PIXEL_RATE_CNTL[MAX_PIPES];
 #if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	uint32_t HDMICHARCLK_CLOCK_CNTL[6];
	uint32_t PHYASYMCLK_CLOCK_CNTL;
	uint32_t PHYBSYMCLK_CLOCK_CNTL;
	uint32_t PHYCSYMCLK_CLOCK_CNTL;
	uint32_t PHYDSYMCLK_CLOCK_CNTL;
	uint32_t PHYESYMCLK_CLOCK_CNTL;
	uint32_t DTBCLK_DTO_MODULO[MAX_PIPES];
	uint32_t DTBCLK_DTO_PHASE[MAX_PIPES];
	uint32_t DCCG_AUDIO_DTBCLK_DTO_MODULO;
	uint32_t DCCG_AUDIO_DTBCLK_DTO_PHASE;
	uint32_t DCCG_AUDIO_DTO_SOURCE;
	uint32_t DPSTREAMCLK_CNTL;
	uint32_t HDMISTREAMCLK_CNTL;
	uint32_t SYMCLK32_SE_CNTL;
	uint32_t SYMCLK32_LE_CNTL;
	uint32_t DENTIST_DISPCLK_CNTL;
	uint32_t DSCCLK_DTO_CTRL;
	uint32_t DSCCLK0_DTO_PARAM;
	uint32_t DSCCLK1_DTO_PARAM;
	uint32_t DSCCLK2_DTO_PARAM;
#endif
	uint32_t DPSTREAMCLK_ROOT_GATE_DISABLE;
	uint32_t DPSTREAMCLK_GATE_DISABLE;
	uint32_t DCCG_GATE_DISABLE_CNTL2;
	uint32_t DCCG_GATE_DISABLE_CNTL3;
	uint32_t HDMISTREAMCLK0_DTO_PARAM;
	uint32_t DCCG_GATE_DISABLE_CNTL4;

};

struct dcn_dccg {
	struct dccg base;
	const struct dccg_registers *regs;
	const struct dccg_shift *dccg_shift;
	const struct dccg_mask *dccg_mask;
};

void dccg2_update_dpp_dto(struct dccg *dccg, int dpp_inst, int req_dppclk);

void dccg2_get_dccg_ref_freq(struct dccg *dccg,
		unsigned int xtalin_freq_inKhz,
		unsigned int *dccg_ref_freq_inKhz);

void dccg2_set_fifo_errdet_ovr_en(struct dccg *dccg,
		bool en);
void dccg2_otg_add_pixel(struct dccg *dccg,
		uint32_t otg_inst);
void dccg2_otg_drop_pixel(struct dccg *dccg,
		uint32_t otg_inst);


void dccg2_init(struct dccg *dccg);

struct dccg *dccg2_create(
	struct dc_context *ctx,
	const struct dccg_registers *regs,
	const struct dccg_shift *dccg_shift,
	const struct dccg_mask *dccg_mask);

void dcn_dccg_destroy(struct dccg **dccg);

#endif //__DCN20_DCCG_H__
