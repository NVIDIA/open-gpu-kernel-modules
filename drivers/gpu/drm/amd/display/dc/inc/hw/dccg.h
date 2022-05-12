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

#ifndef __DAL_DCCG_H__
#define __DAL_DCCG_H__

#include "dc_types.h"
#include "hw_shared.h"

enum phyd32clk_clock_source {
	PHYD32CLKA,
	PHYD32CLKB,
	PHYD32CLKC,
	PHYD32CLKD,
	PHYD32CLKE,
	PHYD32CLKF,
	PHYD32CLKG,
};

enum physymclk_clock_source {
	PHYSYMCLK_FORCE_SRC_SYMCLK,    // Select symclk as source of clock which is output to PHY through DCIO.
	PHYSYMCLK_FORCE_SRC_PHYD18CLK, // Select phyd18clk as the source of clock which is output to PHY through DCIO.
	PHYSYMCLK_FORCE_SRC_PHYD32CLK, // Select phyd32clk as the source of clock which is output to PHY through DCIO.
};

enum hdmistreamclk_source {
	REFCLK,                   // Selects REFCLK as source for hdmistreamclk.
	DTBCLK0,                  // Selects DTBCLK0 as source for hdmistreamclk.
};

enum dentist_dispclk_change_mode {
	DISPCLK_CHANGE_MODE_IMMEDIATE,
	DISPCLK_CHANGE_MODE_RAMPING,
};

struct dccg {
	struct dc_context *ctx;
	const struct dccg_funcs *funcs;
	int pipe_dppclk_khz[MAX_PIPES];
	int ref_dppclk;
	int dtbclk_khz[MAX_PIPES];
	int audio_dtbclk_khz;
	int ref_dtbclk_khz;
};

struct dccg_funcs {
	void (*update_dpp_dto)(struct dccg *dccg,
			int dpp_inst,
			int req_dppclk);
	void (*get_dccg_ref_freq)(struct dccg *dccg,
			unsigned int xtalin_freq_inKhz,
			unsigned int *dccg_ref_freq_inKhz);
	void (*set_fifo_errdet_ovr_en)(struct dccg *dccg,
			bool en);
	void (*otg_add_pixel)(struct dccg *dccg,
			uint32_t otg_inst);
	void (*otg_drop_pixel)(struct dccg *dccg,
			uint32_t otg_inst);
	void (*dccg_init)(struct dccg *dccg);

	void (*set_dpstreamclk)(
			struct dccg *dccg,
			enum hdmistreamclk_source src,
			int otg_inst);

	void (*enable_symclk32_se)(
			struct dccg *dccg,
			int hpo_se_inst,
			enum phyd32clk_clock_source phyd32clk);

	void (*disable_symclk32_se)(
			struct dccg *dccg,
			int hpo_se_inst);

	void (*enable_symclk32_le)(
			struct dccg *dccg,
			int hpo_le_inst,
			enum phyd32clk_clock_source phyd32clk);

	void (*disable_symclk32_le)(
			struct dccg *dccg,
			int hpo_le_inst);
	void (*set_physymclk)(
			struct dccg *dccg,
			int phy_inst,
			enum physymclk_clock_source clk_src,
			bool force_enable);

	void (*set_dtbclk_dto)(
			struct dccg *dccg,
			int dtbclk_inst,
			int req_dtbclk_khz,
			int num_odm_segments,
			const struct dc_crtc_timing *timing);

	void (*set_audio_dtbclk_dto)(
			struct dccg *dccg,
			uint32_t req_audio_dtbclk_khz);

	void (*set_dispclk_change_mode)(
			struct dccg *dccg,
			enum dentist_dispclk_change_mode change_mode);

	void (*disable_dsc)(
		struct dccg *dccg,
		int inst);

	void (*enable_dsc)(
		struct dccg *dccg,
		int inst);

};

#endif //__DAL_DCCG_H__
