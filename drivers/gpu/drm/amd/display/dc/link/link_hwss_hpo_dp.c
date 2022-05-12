/*
 * Copyright 2022 Advanced Micro Devices, Inc.
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
#include "link_hwss_hpo_dp.h"
#include "dm_helpers.h"
#include "core_types.h"
#include "dccg.h"
#include "dc_link_dp.h"

static enum phyd32clk_clock_source get_phyd32clk_src(struct dc_link *link)
{
	switch (link->link_enc->transmitter) {
	case TRANSMITTER_UNIPHY_A:
		return PHYD32CLKA;
	case TRANSMITTER_UNIPHY_B:
		return PHYD32CLKB;
	case TRANSMITTER_UNIPHY_C:
		return PHYD32CLKC;
	case TRANSMITTER_UNIPHY_D:
		return PHYD32CLKD;
	case TRANSMITTER_UNIPHY_E:
		return PHYD32CLKE;
	default:
		return PHYD32CLKA;
	}
}

static void set_hpo_dp_throttled_vcp_size(struct pipe_ctx *pipe_ctx,
		struct fixed31_32 throttled_vcp_size)
{
	struct hpo_dp_stream_encoder *hpo_dp_stream_encoder =
			pipe_ctx->stream_res.hpo_dp_stream_enc;
	struct hpo_dp_link_encoder *hpo_dp_link_encoder =
			pipe_ctx->link_res.hpo_dp_link_enc;

	hpo_dp_link_encoder->funcs->set_throttled_vcp_size(hpo_dp_link_encoder,
			hpo_dp_stream_encoder->inst,
			throttled_vcp_size);
}

static void set_hpo_dp_hblank_min_symbol_width(struct pipe_ctx *pipe_ctx,
		const struct dc_link_settings *link_settings,
		struct fixed31_32 throttled_vcp_size)
{
	struct hpo_dp_stream_encoder *hpo_dp_stream_encoder =
			pipe_ctx->stream_res.hpo_dp_stream_enc;
	struct dc_crtc_timing *timing = &pipe_ctx->stream->timing;
	struct fixed31_32 h_blank_in_ms, time_slot_in_ms, mtp_cnt_per_h_blank;
	uint32_t link_bw_in_kbps =
			dc_link_bandwidth_kbps(pipe_ctx->stream->link, link_settings);
	uint16_t hblank_min_symbol_width = 0;

	if (link_bw_in_kbps > 0) {
		h_blank_in_ms = dc_fixpt_div(dc_fixpt_from_int(
				timing->h_total - timing->h_addressable),
				dc_fixpt_from_fraction(timing->pix_clk_100hz, 10));
		time_slot_in_ms = dc_fixpt_from_fraction(32 * 4, link_bw_in_kbps);
		mtp_cnt_per_h_blank = dc_fixpt_div(h_blank_in_ms,
				dc_fixpt_mul_int(time_slot_in_ms, 64));
		hblank_min_symbol_width = dc_fixpt_floor(
				dc_fixpt_mul(mtp_cnt_per_h_blank, throttled_vcp_size));
	}

	hpo_dp_stream_encoder->funcs->set_hblank_min_symbol_width(hpo_dp_stream_encoder,
			hblank_min_symbol_width);
}

static int get_odm_segment_count(struct pipe_ctx *pipe_ctx)
{
	struct pipe_ctx *odm_pipe = pipe_ctx->next_odm_pipe;
	int count = 1;

	while (odm_pipe != NULL) {
		count++;
		odm_pipe = odm_pipe->next_odm_pipe;
	}

	return count;
}

static void setup_hpo_dp_stream_encoder(struct pipe_ctx *pipe_ctx)
{
	struct dc *dc = pipe_ctx->stream->ctx->dc;
	struct hpo_dp_stream_encoder *stream_enc = pipe_ctx->stream_res.hpo_dp_stream_enc;
	struct hpo_dp_link_encoder *link_enc = pipe_ctx->link_res.hpo_dp_link_enc;
	struct dccg *dccg = dc->res_pool->dccg;
	struct timing_generator *tg = pipe_ctx->stream_res.tg;
	int odm_segment_count = get_odm_segment_count(pipe_ctx);
	enum phyd32clk_clock_source phyd32clk = get_phyd32clk_src(pipe_ctx->stream->link);

	dccg->funcs->set_dpstreamclk(dccg, DTBCLK0, tg->inst);
	dccg->funcs->enable_symclk32_se(dccg, stream_enc->inst, phyd32clk);
	dccg->funcs->set_dtbclk_dto(dccg, tg->inst, pipe_ctx->stream->phy_pix_clk,
			odm_segment_count,
			&pipe_ctx->stream->timing);
	stream_enc->funcs->enable_stream(stream_enc);
	stream_enc->funcs->map_stream_to_link(stream_enc, stream_enc->inst, link_enc->inst);
}

static void reset_hpo_dp_stream_encoder(struct pipe_ctx *pipe_ctx)
{
	struct dc *dc = pipe_ctx->stream->ctx->dc;
	struct hpo_dp_stream_encoder *stream_enc = pipe_ctx->stream_res.hpo_dp_stream_enc;
	struct dccg *dccg = dc->res_pool->dccg;
	struct timing_generator *tg = pipe_ctx->stream_res.tg;

	stream_enc->funcs->disable(stream_enc);
	dccg->funcs->set_dtbclk_dto(dccg, tg->inst, 0, 0, &pipe_ctx->stream->timing);
	dccg->funcs->disable_symclk32_se(dccg, stream_enc->inst);
	dccg->funcs->set_dpstreamclk(dccg, REFCLK, tg->inst);
}

static void enable_hpo_dp_fpga_link_output(struct dc_link *link,
		const struct link_resource *link_res,
		enum signal_type signal,
		enum clock_source_id clock_source,
		const struct dc_link_settings *link_settings)
{
	const struct dc *dc = link->dc;
	enum phyd32clk_clock_source phyd32clk = get_phyd32clk_src(link);
	int phyd32clk_freq_khz = link_settings->link_rate == LINK_RATE_UHBR10 ? 312500 :
			link_settings->link_rate == LINK_RATE_UHBR13_5 ? 412875 :
			link_settings->link_rate == LINK_RATE_UHBR20 ? 625000 : 0;

	dm_set_phyd32clk(dc->ctx, phyd32clk_freq_khz);
	dc->res_pool->dccg->funcs->set_physymclk(
			dc->res_pool->dccg,
			link->link_enc_hw_inst,
			PHYSYMCLK_FORCE_SRC_PHYD32CLK,
			true);
	dc->res_pool->dccg->funcs->enable_symclk32_le(
			dc->res_pool->dccg,
			link_res->hpo_dp_link_enc->inst,
			phyd32clk);
	link_res->hpo_dp_link_enc->funcs->link_enable(
			link_res->hpo_dp_link_enc,
			link_settings->lane_count);

}

static void enable_hpo_dp_link_output(struct dc_link *link,
		const struct link_resource *link_res,
		enum signal_type signal,
		enum clock_source_id clock_source,
		const struct dc_link_settings *link_settings)
{
	if (IS_FPGA_MAXIMUS_DC(link->dc->ctx->dce_environment))
		enable_hpo_dp_fpga_link_output(link, link_res, signal,
				clock_source, link_settings);
	else
		link_res->hpo_dp_link_enc->funcs->enable_link_phy(
				link_res->hpo_dp_link_enc,
				link_settings,
				link->link_enc->transmitter,
				link->link_enc->hpd_source);
}


static void disable_hpo_dp_fpga_link_output(struct dc_link *link,
		const struct link_resource *link_res,
		enum signal_type signal)
{
	const struct dc *dc = link->dc;

	link_res->hpo_dp_link_enc->funcs->link_disable(link_res->hpo_dp_link_enc);
	dc->res_pool->dccg->funcs->disable_symclk32_le(
			dc->res_pool->dccg,
			link_res->hpo_dp_link_enc->inst);
	dc->res_pool->dccg->funcs->set_physymclk(
			dc->res_pool->dccg,
			link->link_enc_hw_inst,
			PHYSYMCLK_FORCE_SRC_SYMCLK,
			false);
	dm_set_phyd32clk(dc->ctx, 0);
}

static void disable_hpo_dp_link_output(struct dc_link *link,
		const struct link_resource *link_res,
		enum signal_type signal)
{
	if (IS_FPGA_MAXIMUS_DC(link->dc->ctx->dce_environment)) {
		disable_hpo_dp_fpga_link_output(link, link_res, signal);
	} else {
		link_res->hpo_dp_link_enc->funcs->link_disable(link_res->hpo_dp_link_enc);
		link_res->hpo_dp_link_enc->funcs->disable_link_phy(
				link_res->hpo_dp_link_enc, signal);
	}
}

static void set_hpo_dp_link_test_pattern(struct dc_link *link,
		const struct link_resource *link_res,
		struct encoder_set_dp_phy_pattern_param *tp_params)
{
	link_res->hpo_dp_link_enc->funcs->set_link_test_pattern(
			link_res->hpo_dp_link_enc, tp_params);
	dp_source_sequence_trace(link, DPCD_SOURCE_SEQ_AFTER_SET_SOURCE_PATTERN);
}

static void set_hpo_dp_lane_settings(struct dc_link *link,
		const struct link_resource *link_res,
		const struct dc_link_settings *link_settings,
		const struct dc_lane_settings lane_settings[LANE_COUNT_DP_MAX])
{
	link_res->hpo_dp_link_enc->funcs->set_ffe(
			link_res->hpo_dp_link_enc,
			link_settings,
			lane_settings[0].FFE_PRESET.raw);
}

static const struct link_hwss hpo_dp_link_hwss = {
	.setup_stream_encoder = setup_hpo_dp_stream_encoder,
	.reset_stream_encoder = reset_hpo_dp_stream_encoder,
	.ext = {
		.set_throttled_vcp_size = set_hpo_dp_throttled_vcp_size,
		.set_hblank_min_symbol_width = set_hpo_dp_hblank_min_symbol_width,
		.enable_dp_link_output = enable_hpo_dp_link_output,
		.disable_dp_link_output = disable_hpo_dp_link_output,
		.set_dp_link_test_pattern  = set_hpo_dp_link_test_pattern,
		.set_dp_lane_settings = set_hpo_dp_lane_settings,
	},
};

bool can_use_hpo_dp_link_hwss(const struct dc_link *link,
		const struct link_resource *link_res)
{
	return link_res->hpo_dp_link_enc != NULL;
}

const struct link_hwss *get_hpo_dp_link_hwss(void)
{
	return &hpo_dp_link_hwss;
}

