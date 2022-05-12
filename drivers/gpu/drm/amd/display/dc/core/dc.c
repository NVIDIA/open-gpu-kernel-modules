/*
 * Copyright 2015 Advanced Micro Devices, Inc.
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
 */

#include <linux/slab.h>
#include <linux/mm.h>

#include "dm_services.h"

#include "dc.h"

#include "core_status.h"
#include "core_types.h"
#include "hw_sequencer.h"
#include "dce/dce_hwseq.h"

#include "resource.h"

#include "clk_mgr.h"
#include "clock_source.h"
#include "dc_bios_types.h"

#include "bios_parser_interface.h"
#include "bios/bios_parser_helper.h"
#include "include/irq_service_interface.h"
#include "transform.h"
#include "dmcu.h"
#include "dpp.h"
#include "timing_generator.h"
#include "abm.h"
#include "virtual/virtual_link_encoder.h"
#include "hubp.h"

#include "link_hwss.h"
#include "link_encoder.h"
#include "link_enc_cfg.h"

#include "dc_link.h"
#include "dc_link_ddc.h"
#include "dm_helpers.h"
#include "mem_input.h"

#include "dc_link_dp.h"
#include "dc_dmub_srv.h"

#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
#include "dsc.h"
#endif

#ifdef CONFIG_DRM_AMD_DC_DCN2_x
#include "vm_helper.h"
#endif

#include "dce/dce_i2c.h"

#include "dmub/dmub_srv.h"

#include "i2caux_interface.h"
#include "dce/dmub_hw_lock_mgr.h"

#include "dc_trace.h"

#include "dce/dmub_outbox.h"

#define CTX \
	dc->ctx

#define DC_LOGGER \
	dc->ctx->logger

static const char DC_BUILD_ID[] = "production-build";

/**
 * DOC: Overview
 *
 * DC is the OS-agnostic component of the amdgpu DC driver.
 *
 * DC maintains and validates a set of structs representing the state of the
 * driver and writes that state to AMD hardware
 *
 * Main DC HW structs:
 *
 * struct dc - The central struct.  One per driver.  Created on driver load,
 * destroyed on driver unload.
 *
 * struct dc_context - One per driver.
 * Used as a backpointer by most other structs in dc.
 *
 * struct dc_link - One per connector (the physical DP, HDMI, miniDP, or eDP
 * plugpoints).  Created on driver load, destroyed on driver unload.
 *
 * struct dc_sink - One per display.  Created on boot or hotplug.
 * Destroyed on shutdown or hotunplug.  A dc_link can have a local sink
 * (the display directly attached).  It may also have one or more remote
 * sinks (in the Multi-Stream Transport case)
 *
 * struct resource_pool - One per driver.  Represents the hw blocks not in the
 * main pipeline.  Not directly accessible by dm.
 *
 * Main dc state structs:
 *
 * These structs can be created and destroyed as needed.  There is a full set of
 * these structs in dc->current_state representing the currently programmed state.
 *
 * struct dc_state - The global DC state to track global state information,
 * such as bandwidth values.
 *
 * struct dc_stream_state - Represents the hw configuration for the pipeline from
 * a framebuffer to a display.  Maps one-to-one with dc_sink.
 *
 * struct dc_plane_state - Represents a framebuffer.  Each stream has at least one,
 * and may have more in the Multi-Plane Overlay case.
 *
 * struct resource_context - Represents the programmable state of everything in
 * the resource_pool.  Not directly accessible by dm.
 *
 * struct pipe_ctx - A member of struct resource_context.  Represents the
 * internal hardware pipeline components.  Each dc_plane_state has either
 * one or two (in the pipe-split case).
 */

/*******************************************************************************
 * Private functions
 ******************************************************************************/

static inline void elevate_update_type(enum surface_update_type *original, enum surface_update_type new)
{
	if (new > *original)
		*original = new;
}

static void destroy_links(struct dc *dc)
{
	uint32_t i;

	for (i = 0; i < dc->link_count; i++) {
		if (NULL != dc->links[i])
			link_destroy(&dc->links[i]);
	}
}

static uint32_t get_num_of_internal_disp(struct dc_link **links, uint32_t num_links)
{
	int i;
	uint32_t count = 0;

	for (i = 0; i < num_links; i++) {
		if (links[i]->connector_signal == SIGNAL_TYPE_EDP ||
				links[i]->is_internal_display)
			count++;
	}

	return count;
}

static int get_seamless_boot_stream_count(struct dc_state *ctx)
{
	uint8_t i;
	uint8_t seamless_boot_stream_count = 0;

	for (i = 0; i < ctx->stream_count; i++)
		if (ctx->streams[i]->apply_seamless_boot_optimization)
			seamless_boot_stream_count++;

	return seamless_boot_stream_count;
}

static bool create_links(
		struct dc *dc,
		uint32_t num_virtual_links)
{
	int i;
	int connectors_num;
	struct dc_bios *bios = dc->ctx->dc_bios;

	dc->link_count = 0;

	connectors_num = bios->funcs->get_connectors_number(bios);

	DC_LOG_DC("BIOS object table - number of connectors: %d", connectors_num);

	if (connectors_num > ENUM_ID_COUNT) {
		dm_error(
			"DC: Number of connectors %d exceeds maximum of %d!\n",
			connectors_num,
			ENUM_ID_COUNT);
		return false;
	}

	dm_output_to_console(
		"DC: %s: connectors_num: physical:%d, virtual:%d\n",
		__func__,
		connectors_num,
		num_virtual_links);

	for (i = 0; i < connectors_num; i++) {
		struct link_init_data link_init_params = {0};
		struct dc_link *link;

		DC_LOG_DC("BIOS object table - printing link object info for connector number: %d, link_index: %d", i, dc->link_count);

		link_init_params.ctx = dc->ctx;
		/* next BIOS object table connector */
		link_init_params.connector_index = i;
		link_init_params.link_index = dc->link_count;
		link_init_params.dc = dc;
		link = link_create(&link_init_params);

		if (link) {
			dc->links[dc->link_count] = link;
			link->dc = dc;
			++dc->link_count;
		}
	}

	DC_LOG_DC("BIOS object table - end");

	/* Create a link for each usb4 dpia port */
	for (i = 0; i < dc->res_pool->usb4_dpia_count; i++) {
		struct link_init_data link_init_params = {0};
		struct dc_link *link;

		link_init_params.ctx = dc->ctx;
		link_init_params.connector_index = i;
		link_init_params.link_index = dc->link_count;
		link_init_params.dc = dc;
		link_init_params.is_dpia_link = true;

		link = link_create(&link_init_params);
		if (link) {
			dc->links[dc->link_count] = link;
			link->dc = dc;
			++dc->link_count;
		}
	}

	for (i = 0; i < num_virtual_links; i++) {
		struct dc_link *link = kzalloc(sizeof(*link), GFP_KERNEL);
		struct encoder_init_data enc_init = {0};

		if (link == NULL) {
			BREAK_TO_DEBUGGER();
			goto failed_alloc;
		}

		link->link_index = dc->link_count;
		dc->links[dc->link_count] = link;
		dc->link_count++;

		link->ctx = dc->ctx;
		link->dc = dc;
		link->connector_signal = SIGNAL_TYPE_VIRTUAL;
		link->link_id.type = OBJECT_TYPE_CONNECTOR;
		link->link_id.id = CONNECTOR_ID_VIRTUAL;
		link->link_id.enum_id = ENUM_ID_1;
		link->link_enc = kzalloc(sizeof(*link->link_enc), GFP_KERNEL);

		if (!link->link_enc) {
			BREAK_TO_DEBUGGER();
			goto failed_alloc;
		}

		link->link_status.dpcd_caps = &link->dpcd_caps;

		enc_init.ctx = dc->ctx;
		enc_init.channel = CHANNEL_ID_UNKNOWN;
		enc_init.hpd_source = HPD_SOURCEID_UNKNOWN;
		enc_init.transmitter = TRANSMITTER_UNKNOWN;
		enc_init.connector = link->link_id;
		enc_init.encoder.type = OBJECT_TYPE_ENCODER;
		enc_init.encoder.id = ENCODER_ID_INTERNAL_VIRTUAL;
		enc_init.encoder.enum_id = ENUM_ID_1;
		virtual_link_encoder_construct(link->link_enc, &enc_init);
	}

	dc->caps.num_of_internal_disp = get_num_of_internal_disp(dc->links, dc->link_count);

	return true;

failed_alloc:
	return false;
}

/* Create additional DIG link encoder objects if fewer than the platform
 * supports were created during link construction. This can happen if the
 * number of physical connectors is less than the number of DIGs.
 */
static bool create_link_encoders(struct dc *dc)
{
	bool res = true;
	unsigned int num_usb4_dpia = dc->res_pool->res_cap->num_usb4_dpia;
	unsigned int num_dig_link_enc = dc->res_pool->res_cap->num_dig_link_enc;
	int i;

	/* A platform without USB4 DPIA endpoints has a fixed mapping between DIG
	 * link encoders and physical display endpoints and does not require
	 * additional link encoder objects.
	 */
	if (num_usb4_dpia == 0)
		return res;

	/* Create as many link encoder objects as the platform supports. DPIA
	 * endpoints can be programmably mapped to any DIG.
	 */
	if (num_dig_link_enc > dc->res_pool->dig_link_enc_count) {
		for (i = 0; i < num_dig_link_enc; i++) {
			struct link_encoder *link_enc = dc->res_pool->link_encoders[i];

			if (!link_enc && dc->res_pool->funcs->link_enc_create_minimal) {
				link_enc = dc->res_pool->funcs->link_enc_create_minimal(dc->ctx,
						(enum engine_id)(ENGINE_ID_DIGA + i));
				if (link_enc) {
					dc->res_pool->link_encoders[i] = link_enc;
					dc->res_pool->dig_link_enc_count++;
				} else {
					res = false;
				}
			}
		}
	}

	return res;
}

/* Destroy any additional DIG link encoder objects created by
 * create_link_encoders().
 * NB: Must only be called after destroy_links().
 */
static void destroy_link_encoders(struct dc *dc)
{
	unsigned int num_usb4_dpia = dc->res_pool->res_cap->num_usb4_dpia;
	unsigned int num_dig_link_enc = dc->res_pool->res_cap->num_dig_link_enc;
	int i;

	/* A platform without USB4 DPIA endpoints has a fixed mapping between DIG
	 * link encoders and physical display endpoints and does not require
	 * additional link encoder objects.
	 */
	if (num_usb4_dpia == 0)
		return;

	for (i = 0; i < num_dig_link_enc; i++) {
		struct link_encoder *link_enc = dc->res_pool->link_encoders[i];

		if (link_enc) {
			link_enc->funcs->destroy(&link_enc);
			dc->res_pool->link_encoders[i] = NULL;
			dc->res_pool->dig_link_enc_count--;
		}
	}
}

static struct dc_perf_trace *dc_perf_trace_create(void)
{
	return kzalloc(sizeof(struct dc_perf_trace), GFP_KERNEL);
}

static void dc_perf_trace_destroy(struct dc_perf_trace **perf_trace)
{
	kfree(*perf_trace);
	*perf_trace = NULL;
}

/**
 *  dc_stream_adjust_vmin_vmax:
 *
 *  Looks up the pipe context of dc_stream_state and updates the
 *  vertical_total_min and vertical_total_max of the DRR, Dynamic Refresh
 *  Rate, which is a power-saving feature that targets reducing panel
 *  refresh rate while the screen is static
 *
 *  @dc:     dc reference
 *  @stream: Initial dc stream state
 *  @adjust: Updated parameters for vertical_total_min and vertical_total_max
 */
bool dc_stream_adjust_vmin_vmax(struct dc *dc,
		struct dc_stream_state *stream,
		struct dc_crtc_timing_adjust *adjust)
{
	int i;
	bool ret = false;

	stream->adjust.v_total_max = adjust->v_total_max;
	stream->adjust.v_total_mid = adjust->v_total_mid;
	stream->adjust.v_total_mid_frame_num = adjust->v_total_mid_frame_num;
	stream->adjust.v_total_min = adjust->v_total_min;

	for (i = 0; i < MAX_PIPES; i++) {
		struct pipe_ctx *pipe = &dc->current_state->res_ctx.pipe_ctx[i];

		if (pipe->stream == stream && pipe->stream_res.tg) {
			dc->hwss.set_drr(&pipe,
					1,
					*adjust);

			ret = true;
		}
	}
	return ret;
}

/**
 *****************************************************************************
 *  Function: dc_stream_get_last_vrr_vtotal
 *
 *  @brief
 *     Looks up the pipe context of dc_stream_state and gets the
 *     last VTOTAL used by DRR (Dynamic Refresh Rate)
 *
 *  @param [in] dc: dc reference
 *  @param [in] stream: Initial dc stream state
 *  @param [in] adjust: Updated parameters for vertical_total_min and
 *  vertical_total_max
 *****************************************************************************
 */
bool dc_stream_get_last_used_drr_vtotal(struct dc *dc,
		struct dc_stream_state *stream,
		uint32_t *refresh_rate)
{
	bool status = false;

	int i = 0;

	for (i = 0; i < MAX_PIPES; i++) {
		struct pipe_ctx *pipe = &dc->current_state->res_ctx.pipe_ctx[i];

		if (pipe->stream == stream && pipe->stream_res.tg) {
			/* Only execute if a function pointer has been defined for
			 * the DC version in question
			 */
			if (pipe->stream_res.tg->funcs->get_last_used_drr_vtotal) {
				pipe->stream_res.tg->funcs->get_last_used_drr_vtotal(pipe->stream_res.tg, refresh_rate);

				status = true;

				break;
			}
		}
	}

	return status;
}

bool dc_stream_get_crtc_position(struct dc *dc,
		struct dc_stream_state **streams, int num_streams,
		unsigned int *v_pos, unsigned int *nom_v_pos)
{
	/* TODO: Support multiple streams */
	const struct dc_stream_state *stream = streams[0];
	int i;
	bool ret = false;
	struct crtc_position position;

	for (i = 0; i < MAX_PIPES; i++) {
		struct pipe_ctx *pipe =
				&dc->current_state->res_ctx.pipe_ctx[i];

		if (pipe->stream == stream && pipe->stream_res.stream_enc) {
			dc->hwss.get_position(&pipe, 1, &position);

			*v_pos = position.vertical_count;
			*nom_v_pos = position.nominal_vcount;
			ret = true;
		}
	}
	return ret;
}

#if defined(CONFIG_DRM_AMD_SECURE_DISPLAY)
bool dc_stream_forward_dmcu_crc_window(struct dc *dc, struct dc_stream_state *stream,
			     struct crc_params *crc_window)
{
	int i;
	struct dmcu *dmcu = dc->res_pool->dmcu;
	struct pipe_ctx *pipe;
	struct crc_region tmp_win, *crc_win;
	struct otg_phy_mux mapping_tmp, *mux_mapping;

	/*crc window can't be null*/
	if (!crc_window)
		return false;

	if ((dmcu != NULL && dmcu->funcs->is_dmcu_initialized(dmcu))) {
		crc_win = &tmp_win;
		mux_mapping = &mapping_tmp;
		/*set crc window*/
		tmp_win.x_start = crc_window->windowa_x_start;
		tmp_win.y_start = crc_window->windowa_y_start;
		tmp_win.x_end = crc_window->windowa_x_end;
		tmp_win.y_end = crc_window->windowa_y_end;

		for (i = 0; i < MAX_PIPES; i++) {
			pipe = &dc->current_state->res_ctx.pipe_ctx[i];
			if (pipe->stream == stream && !pipe->top_pipe && !pipe->prev_odm_pipe)
				break;
		}

		/* Stream not found */
		if (i == MAX_PIPES)
			return false;


		/*set mux routing info*/
		mapping_tmp.phy_output_num = stream->link->link_enc_hw_inst;
		mapping_tmp.otg_output_num = pipe->stream_res.tg->inst;

		dmcu->funcs->forward_crc_window(dmcu, crc_win, mux_mapping);
	} else {
		DC_LOG_DC("dmcu is not initialized");
		return false;
	}

	return true;
}

bool dc_stream_stop_dmcu_crc_win_update(struct dc *dc, struct dc_stream_state *stream)
{
	int i;
	struct dmcu *dmcu = dc->res_pool->dmcu;
	struct pipe_ctx *pipe;
	struct otg_phy_mux mapping_tmp, *mux_mapping;

	if ((dmcu != NULL && dmcu->funcs->is_dmcu_initialized(dmcu))) {
		mux_mapping = &mapping_tmp;

		for (i = 0; i < MAX_PIPES; i++) {
			pipe = &dc->current_state->res_ctx.pipe_ctx[i];
			if (pipe->stream == stream && !pipe->top_pipe && !pipe->prev_odm_pipe)
				break;
		}

		/* Stream not found */
		if (i == MAX_PIPES)
			return false;


		/*set mux routing info*/
		mapping_tmp.phy_output_num = stream->link->link_enc_hw_inst;
		mapping_tmp.otg_output_num = pipe->stream_res.tg->inst;

		dmcu->funcs->stop_crc_win_update(dmcu, mux_mapping);
	} else {
		DC_LOG_DC("dmcu is not initialized");
		return false;
	}

	return true;
}
#endif

/**
 * dc_stream_configure_crc() - Configure CRC capture for the given stream.
 * @dc: DC Object
 * @stream: The stream to configure CRC on.
 * @enable: Enable CRC if true, disable otherwise.
 * @crc_window: CRC window (x/y start/end) information
 * @continuous: Capture CRC on every frame if true. Otherwise, only capture
 *              once.
 *
 * By default, only CRC0 is configured, and the entire frame is used to
 * calculate the crc.
 */
bool dc_stream_configure_crc(struct dc *dc, struct dc_stream_state *stream,
			     struct crc_params *crc_window, bool enable, bool continuous)
{
	int i;
	struct pipe_ctx *pipe;
	struct crc_params param;
	struct timing_generator *tg;

	for (i = 0; i < MAX_PIPES; i++) {
		pipe = &dc->current_state->res_ctx.pipe_ctx[i];
		if (pipe->stream == stream && !pipe->top_pipe && !pipe->prev_odm_pipe)
			break;
	}
	/* Stream not found */
	if (i == MAX_PIPES)
		return false;

	/* By default, capture the full frame */
	param.windowa_x_start = 0;
	param.windowa_y_start = 0;
	param.windowa_x_end = pipe->stream->timing.h_addressable;
	param.windowa_y_end = pipe->stream->timing.v_addressable;
	param.windowb_x_start = 0;
	param.windowb_y_start = 0;
	param.windowb_x_end = pipe->stream->timing.h_addressable;
	param.windowb_y_end = pipe->stream->timing.v_addressable;

	if (crc_window) {
		param.windowa_x_start = crc_window->windowa_x_start;
		param.windowa_y_start = crc_window->windowa_y_start;
		param.windowa_x_end = crc_window->windowa_x_end;
		param.windowa_y_end = crc_window->windowa_y_end;
		param.windowb_x_start = crc_window->windowb_x_start;
		param.windowb_y_start = crc_window->windowb_y_start;
		param.windowb_x_end = crc_window->windowb_x_end;
		param.windowb_y_end = crc_window->windowb_y_end;
	}

#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	param.dsc_mode = pipe->stream->timing.flags.DSC ? 1:0;
#endif
	param.odm_mode = pipe->next_odm_pipe ? 1:0;

	/* Default to the union of both windows */
	param.selection = UNION_WINDOW_A_B;
	param.continuous_mode = continuous;
	param.enable = enable;

	tg = pipe->stream_res.tg;

	/* Only call if supported */
	if (tg->funcs->configure_crc)
		return tg->funcs->configure_crc(tg, &param);
	DC_LOG_WARNING("CRC capture not supported.");
	return false;
}

/**
 * dc_stream_get_crc() - Get CRC values for the given stream.
 * @dc: DC object
 * @stream: The DC stream state of the stream to get CRCs from.
 * @r_cr: CRC value for the first of the 3 channels stored here.
 * @g_y:  CRC value for the second of the 3 channels stored here.
 * @b_cb: CRC value for the third of the 3 channels stored here.
 *
 * dc_stream_configure_crc needs to be called beforehand to enable CRCs.
 * Return false if stream is not found, or if CRCs are not enabled.
 */
bool dc_stream_get_crc(struct dc *dc, struct dc_stream_state *stream,
		       uint32_t *r_cr, uint32_t *g_y, uint32_t *b_cb)
{
	int i;
	struct pipe_ctx *pipe;
	struct timing_generator *tg;

	for (i = 0; i < MAX_PIPES; i++) {
		pipe = &dc->current_state->res_ctx.pipe_ctx[i];
		if (pipe->stream == stream)
			break;
	}
	/* Stream not found */
	if (i == MAX_PIPES)
		return false;

	tg = pipe->stream_res.tg;

	if (tg->funcs->get_crc)
		return tg->funcs->get_crc(tg, r_cr, g_y, b_cb);
	DC_LOG_WARNING("CRC capture not supported.");
	return false;
}

void dc_stream_set_dyn_expansion(struct dc *dc, struct dc_stream_state *stream,
		enum dc_dynamic_expansion option)
{
	/* OPP FMT dyn expansion updates*/
	int i;
	struct pipe_ctx *pipe_ctx;

	for (i = 0; i < MAX_PIPES; i++) {
		if (dc->current_state->res_ctx.pipe_ctx[i].stream
				== stream) {
			pipe_ctx = &dc->current_state->res_ctx.pipe_ctx[i];
			pipe_ctx->stream_res.opp->dyn_expansion = option;
			pipe_ctx->stream_res.opp->funcs->opp_set_dyn_expansion(
					pipe_ctx->stream_res.opp,
					COLOR_SPACE_YCBCR601,
					stream->timing.display_color_depth,
					stream->signal);
		}
	}
}

void dc_stream_set_dither_option(struct dc_stream_state *stream,
		enum dc_dither_option option)
{
	struct bit_depth_reduction_params params;
	struct dc_link *link = stream->link;
	struct pipe_ctx *pipes = NULL;
	int i;

	for (i = 0; i < MAX_PIPES; i++) {
		if (link->dc->current_state->res_ctx.pipe_ctx[i].stream ==
				stream) {
			pipes = &link->dc->current_state->res_ctx.pipe_ctx[i];
			break;
		}
	}

	if (!pipes)
		return;
	if (option > DITHER_OPTION_MAX)
		return;

	stream->dither_option = option;

	memset(&params, 0, sizeof(params));
	resource_build_bit_depth_reduction_params(stream, &params);
	stream->bit_depth_params = params;

	if (pipes->plane_res.xfm &&
	    pipes->plane_res.xfm->funcs->transform_set_pixel_storage_depth) {
		pipes->plane_res.xfm->funcs->transform_set_pixel_storage_depth(
			pipes->plane_res.xfm,
			pipes->plane_res.scl_data.lb_params.depth,
			&stream->bit_depth_params);
	}

	pipes->stream_res.opp->funcs->
		opp_program_bit_depth_reduction(pipes->stream_res.opp, &params);
}

bool dc_stream_set_gamut_remap(struct dc *dc, const struct dc_stream_state *stream)
{
	int i;
	bool ret = false;
	struct pipe_ctx *pipes;

	for (i = 0; i < MAX_PIPES; i++) {
		if (dc->current_state->res_ctx.pipe_ctx[i].stream == stream) {
			pipes = &dc->current_state->res_ctx.pipe_ctx[i];
			dc->hwss.program_gamut_remap(pipes);
			ret = true;
		}
	}

	return ret;
}

bool dc_stream_program_csc_matrix(struct dc *dc, struct dc_stream_state *stream)
{
	int i;
	bool ret = false;
	struct pipe_ctx *pipes;

	for (i = 0; i < MAX_PIPES; i++) {
		if (dc->current_state->res_ctx.pipe_ctx[i].stream
				== stream) {

			pipes = &dc->current_state->res_ctx.pipe_ctx[i];
			dc->hwss.program_output_csc(dc,
					pipes,
					stream->output_color_space,
					stream->csc_color_matrix.matrix,
					pipes->stream_res.opp->inst);
			ret = true;
		}
	}

	return ret;
}

void dc_stream_set_static_screen_params(struct dc *dc,
		struct dc_stream_state **streams,
		int num_streams,
		const struct dc_static_screen_params *params)
{
	int i, j;
	struct pipe_ctx *pipes_affected[MAX_PIPES];
	int num_pipes_affected = 0;

	for (i = 0; i < num_streams; i++) {
		struct dc_stream_state *stream = streams[i];

		for (j = 0; j < MAX_PIPES; j++) {
			if (dc->current_state->res_ctx.pipe_ctx[j].stream
					== stream) {
				pipes_affected[num_pipes_affected++] =
						&dc->current_state->res_ctx.pipe_ctx[j];
			}
		}
	}

	dc->hwss.set_static_screen_control(pipes_affected, num_pipes_affected, params);
}

static void dc_destruct(struct dc *dc)
{
	// reset link encoder assignment table on destruct
	if (dc->res_pool && dc->res_pool->funcs->link_encs_assign)
		link_enc_cfg_init(dc, dc->current_state);

	if (dc->current_state) {
		dc_release_state(dc->current_state);
		dc->current_state = NULL;
	}

	destroy_links(dc);

	destroy_link_encoders(dc);

	if (dc->clk_mgr) {
		dc_destroy_clk_mgr(dc->clk_mgr);
		dc->clk_mgr = NULL;
	}

	dc_destroy_resource_pool(dc);

	if (dc->ctx->gpio_service)
		dal_gpio_service_destroy(&dc->ctx->gpio_service);

	if (dc->ctx->created_bios)
		dal_bios_parser_destroy(&dc->ctx->dc_bios);

	dc_perf_trace_destroy(&dc->ctx->perf_trace);

	kfree(dc->ctx);
	dc->ctx = NULL;

	kfree(dc->bw_vbios);
	dc->bw_vbios = NULL;

	kfree(dc->bw_dceip);
	dc->bw_dceip = NULL;

#ifdef CONFIG_DRM_AMD_DC_DCN1_0
	kfree(dc->dcn_soc);
	dc->dcn_soc = NULL;

	kfree(dc->dcn_ip);
	dc->dcn_ip = NULL;

#endif
#ifdef CONFIG_DRM_AMD_DC_DCN2_x
	kfree(dc->vm_helper);
	dc->vm_helper = NULL;
#endif
}

static bool dc_construct_ctx(struct dc *dc,
		const struct dc_init_data *init_params)
{
	struct dc_context *dc_ctx;
	enum dce_version dc_version = DCE_VERSION_UNKNOWN;

	dc_ctx = kzalloc(sizeof(*dc_ctx), GFP_KERNEL);
	if (!dc_ctx)
		return false;

	dc_ctx->cgs_device = init_params->cgs_device;
	dc_ctx->driver_context = init_params->driver;
	dc_ctx->dc = dc;
	dc_ctx->asic_id = init_params->asic_id;
	dc_ctx->dc_sink_id_count = 0;
	dc_ctx->dc_stream_id_count = 0;
	dc_ctx->dce_environment = init_params->dce_environment;

	/* Create logger */

	dc_version = resource_parse_asic_id(init_params->asic_id);
	dc_ctx->dce_version = dc_version;

	dc_ctx->perf_trace = dc_perf_trace_create();
	if (!dc_ctx->perf_trace) {
		ASSERT_CRITICAL(false);
		return false;
	}

	dc->ctx = dc_ctx;

	return true;
}

static bool dc_construct(struct dc *dc,
		const struct dc_init_data *init_params)
{
	struct dc_context *dc_ctx;
	struct bw_calcs_dceip *dc_dceip;
	struct bw_calcs_vbios *dc_vbios;
#ifdef CONFIG_DRM_AMD_DC_DCN1_0
	struct dcn_soc_bounding_box *dcn_soc;
	struct dcn_ip_params *dcn_ip;
#endif

	dc->config = init_params->flags;

#ifdef CONFIG_DRM_AMD_DC_DCN2_x
	// Allocate memory for the vm_helper
	dc->vm_helper = kzalloc(sizeof(struct vm_helper), GFP_KERNEL);
	if (!dc->vm_helper) {
		dm_error("%s: failed to create dc->vm_helper\n", __func__);
		goto fail;
	}
#endif

	memcpy(&dc->bb_overrides, &init_params->bb_overrides, sizeof(dc->bb_overrides));

	dc_dceip = kzalloc(sizeof(*dc_dceip), GFP_KERNEL);
	if (!dc_dceip) {
		dm_error("%s: failed to create dceip\n", __func__);
		goto fail;
	}

	dc->bw_dceip = dc_dceip;

	dc_vbios = kzalloc(sizeof(*dc_vbios), GFP_KERNEL);
	if (!dc_vbios) {
		dm_error("%s: failed to create vbios\n", __func__);
		goto fail;
	}

	dc->bw_vbios = dc_vbios;
#ifdef CONFIG_DRM_AMD_DC_DCN1_0
	dcn_soc = kzalloc(sizeof(*dcn_soc), GFP_KERNEL);
	if (!dcn_soc) {
		dm_error("%s: failed to create dcn_soc\n", __func__);
		goto fail;
	}

	dc->dcn_soc = dcn_soc;

	dcn_ip = kzalloc(sizeof(*dcn_ip), GFP_KERNEL);
	if (!dcn_ip) {
		dm_error("%s: failed to create dcn_ip\n", __func__);
		goto fail;
	}

	dc->dcn_ip = dcn_ip;
#endif

	if (!dc_construct_ctx(dc, init_params)) {
		dm_error("%s: failed to create ctx\n", __func__);
		goto fail;
	}

        dc_ctx = dc->ctx;

	/* Resource should construct all asic specific resources.
	 * This should be the only place where we need to parse the asic id
	 */
	if (init_params->vbios_override)
		dc_ctx->dc_bios = init_params->vbios_override;
	else {
		/* Create BIOS parser */
		struct bp_init_data bp_init_data;

		bp_init_data.ctx = dc_ctx;
		bp_init_data.bios = init_params->asic_id.atombios_base_address;

		dc_ctx->dc_bios = dal_bios_parser_create(
				&bp_init_data, dc_ctx->dce_version);

		if (!dc_ctx->dc_bios) {
			ASSERT_CRITICAL(false);
			goto fail;
		}

		dc_ctx->created_bios = true;
	}

	dc->vendor_signature = init_params->vendor_signature;

	/* Create GPIO service */
	dc_ctx->gpio_service = dal_gpio_service_create(
			dc_ctx->dce_version,
			dc_ctx->dce_environment,
			dc_ctx);

	if (!dc_ctx->gpio_service) {
		ASSERT_CRITICAL(false);
		goto fail;
	}

	dc->res_pool = dc_create_resource_pool(dc, init_params, dc_ctx->dce_version);
	if (!dc->res_pool)
		goto fail;

	/* set i2c speed if not done by the respective dcnxxx__resource.c */
	if (dc->caps.i2c_speed_in_khz_hdcp == 0)
		dc->caps.i2c_speed_in_khz_hdcp = dc->caps.i2c_speed_in_khz;

	dc->clk_mgr = dc_clk_mgr_create(dc->ctx, dc->res_pool->pp_smu, dc->res_pool->dccg);
	if (!dc->clk_mgr)
		goto fail;
#ifdef CONFIG_DRM_AMD_DC_DCN3_x
	dc->clk_mgr->force_smu_not_present = init_params->force_smu_not_present;
#endif

#ifdef CONFIG_DRM_AMD_DC_DCN2_x
	if (dc->res_pool->funcs->update_bw_bounding_box) {
		DC_FP_START();
		dc->res_pool->funcs->update_bw_bounding_box(dc, dc->clk_mgr->bw_params);
		DC_FP_END();
	}
#endif

	/* Creation of current_state must occur after dc->dml
	 * is initialized in dc_create_resource_pool because
	 * on creation it copies the contents of dc->dml
	 */

	dc->current_state = dc_create_state(dc);

	if (!dc->current_state) {
		dm_error("%s: failed to create validate ctx\n", __func__);
		goto fail;
	}

	if (!create_links(dc, init_params->num_virtual_links))
		goto fail;

	/* Create additional DIG link encoder objects if fewer than the platform
	 * supports were created during link construction.
	 */
	if (!create_link_encoders(dc))
		goto fail;

	dc_resource_state_construct(dc, dc->current_state);

	return true;

fail:
	return false;
}

#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
static void disable_all_writeback_pipes_for_stream(
		const struct dc *dc,
		struct dc_stream_state *stream,
		struct dc_state *context)
{
	int i;

	for (i = 0; i < stream->num_wb_info; i++)
		stream->writeback_info[i].wb_enabled = false;
}
#endif

static void apply_ctx_interdependent_lock(struct dc *dc, struct dc_state *context,
					  struct dc_stream_state *stream, bool lock)
{
	int i;

	/* Checks if interdependent update function pointer is NULL or not, takes care of DCE110 case */
	if (dc->hwss.interdependent_update_lock)
		dc->hwss.interdependent_update_lock(dc, context, lock);
	else {
		for (i = 0; i < dc->res_pool->pipe_count; i++) {
			struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[i];
			struct pipe_ctx *old_pipe_ctx = &dc->current_state->res_ctx.pipe_ctx[i];

			// Copied conditions that were previously in dce110_apply_ctx_for_surface
			if (stream == pipe_ctx->stream) {
				if (!pipe_ctx->top_pipe &&
					(pipe_ctx->plane_state || old_pipe_ctx->plane_state))
					dc->hwss.pipe_control_lock(dc, pipe_ctx, lock);
			}
		}
	}
}

static void disable_dangling_plane(struct dc *dc, struct dc_state *context)
{
	int i, j;
	struct dc_state *dangling_context = dc_create_state(dc);
	struct dc_state *current_ctx;

	if (dangling_context == NULL)
		return;

	dc_resource_state_copy_construct(dc->current_state, dangling_context);

	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		struct dc_stream_state *old_stream =
				dc->current_state->res_ctx.pipe_ctx[i].stream;
		bool should_disable = true;
		bool pipe_split_change =
			context->res_ctx.pipe_ctx[i].top_pipe != dc->current_state->res_ctx.pipe_ctx[i].top_pipe;

		for (j = 0; j < context->stream_count; j++) {
			if (old_stream == context->streams[j]) {
				should_disable = false;
				break;
			}
		}
		if (!should_disable && pipe_split_change)
			should_disable = true;

		if (should_disable && old_stream) {
			dc_rem_all_planes_for_stream(dc, old_stream, dangling_context);
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
			disable_all_writeback_pipes_for_stream(dc, old_stream, dangling_context);
#endif

			if (dc->hwss.apply_ctx_for_surface) {
				apply_ctx_interdependent_lock(dc, dc->current_state, old_stream, true);
				dc->hwss.apply_ctx_for_surface(dc, old_stream, 0, dangling_context);
				apply_ctx_interdependent_lock(dc, dc->current_state, old_stream, false);
				dc->hwss.post_unlock_program_front_end(dc, dangling_context);
			}
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
			if (dc->hwss.program_front_end_for_ctx) {
				dc->hwss.interdependent_update_lock(dc, dc->current_state, true);
				dc->hwss.program_front_end_for_ctx(dc, dangling_context);
				dc->hwss.interdependent_update_lock(dc, dc->current_state, false);
				dc->hwss.post_unlock_program_front_end(dc, dangling_context);
			}
#endif
		}
	}

	current_ctx = dc->current_state;
	dc->current_state = dangling_context;
	dc_release_state(current_ctx);
}

static void disable_vbios_mode_if_required(
		struct dc *dc,
		struct dc_state *context)
{
	unsigned int i, j;

	/* check if timing_changed, disable stream*/
	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		struct dc_stream_state *stream = NULL;
		struct dc_link *link = NULL;
		struct pipe_ctx *pipe = NULL;

		pipe = &context->res_ctx.pipe_ctx[i];
		stream = pipe->stream;
		if (stream == NULL)
			continue;

		// only looking for first odm pipe
		if (pipe->prev_odm_pipe)
			continue;

		if (stream->link->local_sink &&
			stream->link->local_sink->sink_signal == SIGNAL_TYPE_EDP) {
			link = stream->link;
		}

		if (link != NULL && link->link_enc->funcs->is_dig_enabled(link->link_enc)) {
			unsigned int enc_inst, tg_inst = 0;
			unsigned int pix_clk_100hz;

			enc_inst = link->link_enc->funcs->get_dig_frontend(link->link_enc);
			if (enc_inst != ENGINE_ID_UNKNOWN) {
				for (j = 0; j < dc->res_pool->stream_enc_count; j++) {
					if (dc->res_pool->stream_enc[j]->id == enc_inst) {
						tg_inst = dc->res_pool->stream_enc[j]->funcs->dig_source_otg(
							dc->res_pool->stream_enc[j]);
						break;
					}
				}

				dc->res_pool->dp_clock_source->funcs->get_pixel_clk_frequency_100hz(
					dc->res_pool->dp_clock_source,
					tg_inst, &pix_clk_100hz);

				if (link->link_status.link_active) {
					uint32_t requested_pix_clk_100hz =
						pipe->stream_res.pix_clk_params.requested_pix_clk_100hz;

					if (pix_clk_100hz != requested_pix_clk_100hz) {
						core_link_disable_stream(pipe);
						pipe->stream->dpms_off = false;
					}
				}
			}
		}
	}
}

static void wait_for_no_pipes_pending(struct dc *dc, struct dc_state *context)
{
	int i;
	PERF_TRACE();
	for (i = 0; i < MAX_PIPES; i++) {
		int count = 0;
		struct pipe_ctx *pipe = &context->res_ctx.pipe_ctx[i];

		if (!pipe->plane_state)
			continue;

		/* Timeout 100 ms */
		while (count < 100000) {
			/* Must set to false to start with, due to OR in update function */
			pipe->plane_state->status.is_flip_pending = false;
			dc->hwss.update_pending_status(pipe);
			if (!pipe->plane_state->status.is_flip_pending)
				break;
			udelay(1);
			count++;
		}
		ASSERT(!pipe->plane_state->status.is_flip_pending);
	}
	PERF_TRACE();
}

/*******************************************************************************
 * Public functions
 ******************************************************************************/

struct dc *dc_create(const struct dc_init_data *init_params)
{
	struct dc *dc = kzalloc(sizeof(*dc), GFP_KERNEL);
	unsigned int full_pipe_count;

	if (!dc)
		return NULL;

	if (init_params->dce_environment == DCE_ENV_VIRTUAL_HW) {
		if (!dc_construct_ctx(dc, init_params))
			goto destruct_dc;
	} else {
		if (!dc_construct(dc, init_params))
			goto destruct_dc;

		full_pipe_count = dc->res_pool->pipe_count;
		if (dc->res_pool->underlay_pipe_index != NO_UNDERLAY_PIPE)
			full_pipe_count--;
		dc->caps.max_streams = min(
				full_pipe_count,
				dc->res_pool->stream_enc_count);

		dc->caps.max_links = dc->link_count;
		dc->caps.max_audios = dc->res_pool->audio_count;
		dc->caps.linear_pitch_alignment = 64;

		dc->caps.max_dp_protocol_version = DP_VERSION_1_4;

		dc->caps.max_otg_num = dc->res_pool->res_cap->num_timing_generator;

		if (dc->res_pool->dmcu != NULL)
			dc->versions.dmcu_version = dc->res_pool->dmcu->dmcu_version;
	}

	/* Populate versioning information */
	dc->versions.dc_ver = DC_VER;

	dc->build_id = DC_BUILD_ID;

	DC_LOG_DC("Display Core initialized\n");



	return dc;

destruct_dc:
	dc_destruct(dc);
	kfree(dc);
	return NULL;
}

static void detect_edp_presence(struct dc *dc)
{
	struct dc_link *edp_links[MAX_NUM_EDP];
	struct dc_link *edp_link = NULL;
	enum dc_connection_type type;
	int i;
	int edp_num;

	get_edp_links(dc, edp_links, &edp_num);
	if (!edp_num)
		return;

	for (i = 0; i < edp_num; i++) {
		edp_link = edp_links[i];
		if (dc->config.edp_not_connected) {
			edp_link->edp_sink_present = false;
		} else {
			dc_link_detect_sink(edp_link, &type);
			edp_link->edp_sink_present = (type != dc_connection_none);
		}
	}
}

void dc_hardware_init(struct dc *dc)
{

	detect_edp_presence(dc);
	if (dc->ctx->dce_environment != DCE_ENV_VIRTUAL_HW)
		dc->hwss.init_hw(dc);
}

void dc_init_callbacks(struct dc *dc,
		const struct dc_callback_init *init_params)
{
#ifdef CONFIG_DRM_AMD_DC_HDCP
	dc->ctx->cp_psp = init_params->cp_psp;
#endif
}

void dc_deinit_callbacks(struct dc *dc)
{
#ifdef CONFIG_DRM_AMD_DC_HDCP
	memset(&dc->ctx->cp_psp, 0, sizeof(dc->ctx->cp_psp));
#endif
}

void dc_destroy(struct dc **dc)
{
	dc_destruct(*dc);
	kfree(*dc);
	*dc = NULL;
}

static void enable_timing_multisync(
		struct dc *dc,
		struct dc_state *ctx)
{
	int i, multisync_count = 0;
	int pipe_count = dc->res_pool->pipe_count;
	struct pipe_ctx *multisync_pipes[MAX_PIPES] = { NULL };

	for (i = 0; i < pipe_count; i++) {
		if (!ctx->res_ctx.pipe_ctx[i].stream ||
				!ctx->res_ctx.pipe_ctx[i].stream->triggered_crtc_reset.enabled)
			continue;
		if (ctx->res_ctx.pipe_ctx[i].stream == ctx->res_ctx.pipe_ctx[i].stream->triggered_crtc_reset.event_source)
			continue;
		multisync_pipes[multisync_count] = &ctx->res_ctx.pipe_ctx[i];
		multisync_count++;
	}

	if (multisync_count > 0) {
		dc->hwss.enable_per_frame_crtc_position_reset(
			dc, multisync_count, multisync_pipes);
	}
}

static void program_timing_sync(
		struct dc *dc,
		struct dc_state *ctx)
{
	int i, j, k;
	int group_index = 0;
	int num_group = 0;
	int pipe_count = dc->res_pool->pipe_count;
	struct pipe_ctx *unsynced_pipes[MAX_PIPES] = { NULL };

	for (i = 0; i < pipe_count; i++) {
		if (!ctx->res_ctx.pipe_ctx[i].stream || ctx->res_ctx.pipe_ctx[i].top_pipe)
			continue;

		unsynced_pipes[i] = &ctx->res_ctx.pipe_ctx[i];
	}

	for (i = 0; i < pipe_count; i++) {
		int group_size = 1;
		enum timing_synchronization_type sync_type = NOT_SYNCHRONIZABLE;
		struct pipe_ctx *pipe_set[MAX_PIPES];

		if (!unsynced_pipes[i])
			continue;

		pipe_set[0] = unsynced_pipes[i];
		unsynced_pipes[i] = NULL;

		/* Add tg to the set, search rest of the tg's for ones with
		 * same timing, add all tgs with same timing to the group
		 */
		for (j = i + 1; j < pipe_count; j++) {
			if (!unsynced_pipes[j])
				continue;
			if (sync_type != TIMING_SYNCHRONIZABLE &&
				dc->hwss.enable_vblanks_synchronization &&
				unsynced_pipes[j]->stream_res.tg->funcs->align_vblanks &&
				resource_are_vblanks_synchronizable(
					unsynced_pipes[j]->stream,
					pipe_set[0]->stream)) {
				sync_type = VBLANK_SYNCHRONIZABLE;
				pipe_set[group_size] = unsynced_pipes[j];
				unsynced_pipes[j] = NULL;
				group_size++;
			} else
			if (sync_type != VBLANK_SYNCHRONIZABLE &&
				resource_are_streams_timing_synchronizable(
					unsynced_pipes[j]->stream,
					pipe_set[0]->stream)) {
				sync_type = TIMING_SYNCHRONIZABLE;
				pipe_set[group_size] = unsynced_pipes[j];
				unsynced_pipes[j] = NULL;
				group_size++;
			}
		}

		/* set first unblanked pipe as master */
		for (j = 0; j < group_size; j++) {
			bool is_blanked;

#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
			if (pipe_set[j]->stream_res.opp->funcs->dpg_is_blanked)
				is_blanked =
					pipe_set[j]->stream_res.opp->funcs->dpg_is_blanked(pipe_set[j]->stream_res.opp);
			else
#endif
				is_blanked =
					pipe_set[j]->stream_res.tg->funcs->is_blanked(pipe_set[j]->stream_res.tg);
			if (!is_blanked) {
				if (j == 0)
					break;

				swap(pipe_set[0], pipe_set[j]);
				break;
			}
		}

		for (k = 0; k < group_size; k++) {
			struct dc_stream_status *status = dc_stream_get_status_from_state(ctx, pipe_set[k]->stream);

			status->timing_sync_info.group_id = num_group;
			status->timing_sync_info.group_size = group_size;
			if (k == 0)
				status->timing_sync_info.master = true;
			else
				status->timing_sync_info.master = false;

		}

		/* remove any other pipes that are already been synced */
		if (dc->config.use_pipe_ctx_sync_logic) {
			/* check pipe's syncd to decide which pipe to be removed */
			for (j = 1; j < group_size; j++) {
				if (pipe_set[j]->pipe_idx_syncd == pipe_set[0]->pipe_idx_syncd) {
					group_size--;
					pipe_set[j] = pipe_set[group_size];
					j--;
				} else
					/* link slave pipe's syncd with master pipe */
					pipe_set[j]->pipe_idx_syncd = pipe_set[0]->pipe_idx_syncd;
			}
		} else {
			/* remove any other pipes by checking valid plane */
			for (j = j + 1; j < group_size; j++) {
				if (pipe_set[j]->plane_state) {
					group_size--;
					pipe_set[j] = pipe_set[group_size];
					j--;
				}
 			}
 		}

		if (group_size > 1) {
			if (sync_type == TIMING_SYNCHRONIZABLE) {
				dc->hwss.enable_timing_synchronization(
					dc, group_index, group_size, pipe_set);
			} else
				if (sync_type == VBLANK_SYNCHRONIZABLE) {
				dc->hwss.enable_vblanks_synchronization(
					dc, group_index, group_size, pipe_set);
				}
			group_index++;
		}
		num_group++;
	}
}

static bool context_changed(
		struct dc *dc,
		struct dc_state *context)
{
	uint8_t i;

	if (context->stream_count != dc->current_state->stream_count)
		return true;

	for (i = 0; i < dc->current_state->stream_count; i++) {
		if (dc->current_state->streams[i] != context->streams[i])
			return true;
	}

	return false;
}

bool dc_validate_boot_timing(const struct dc *dc,
				const struct dc_sink *sink,
				struct dc_crtc_timing *crtc_timing)
{
	struct timing_generator *tg;
	struct stream_encoder *se = NULL;

	struct dc_crtc_timing hw_crtc_timing = {0};

	struct dc_link *link = sink->link;
	unsigned int i, enc_inst, tg_inst = 0;

	/* Support seamless boot on EDP displays only */
	if (sink->sink_signal != SIGNAL_TYPE_EDP) {
		return false;
	}

	/* Check for enabled DIG to identify enabled display */
	if (!link->link_enc->funcs->is_dig_enabled(link->link_enc))
		return false;

	enc_inst = link->link_enc->funcs->get_dig_frontend(link->link_enc);

	if (enc_inst == ENGINE_ID_UNKNOWN)
		return false;

	for (i = 0; i < dc->res_pool->stream_enc_count; i++) {
		if (dc->res_pool->stream_enc[i]->id == enc_inst) {

			se = dc->res_pool->stream_enc[i];

			tg_inst = dc->res_pool->stream_enc[i]->funcs->dig_source_otg(
				dc->res_pool->stream_enc[i]);
			break;
		}
	}

	// tg_inst not found
	if (i == dc->res_pool->stream_enc_count)
		return false;

	if (tg_inst >= dc->res_pool->timing_generator_count)
		return false;

	tg = dc->res_pool->timing_generators[tg_inst];

	if (!tg->funcs->get_hw_timing)
		return false;

	if (!tg->funcs->get_hw_timing(tg, &hw_crtc_timing))
		return false;

	if (crtc_timing->h_total != hw_crtc_timing.h_total)
		return false;

	if (crtc_timing->h_border_left != hw_crtc_timing.h_border_left)
		return false;

	if (crtc_timing->h_addressable != hw_crtc_timing.h_addressable)
		return false;

	if (crtc_timing->h_border_right != hw_crtc_timing.h_border_right)
		return false;

	if (crtc_timing->h_front_porch != hw_crtc_timing.h_front_porch)
		return false;

	if (crtc_timing->h_sync_width != hw_crtc_timing.h_sync_width)
		return false;

	if (crtc_timing->v_total != hw_crtc_timing.v_total)
		return false;

	if (crtc_timing->v_border_top != hw_crtc_timing.v_border_top)
		return false;

	if (crtc_timing->v_addressable != hw_crtc_timing.v_addressable)
		return false;

	if (crtc_timing->v_border_bottom != hw_crtc_timing.v_border_bottom)
		return false;

	if (crtc_timing->v_front_porch != hw_crtc_timing.v_front_porch)
		return false;

	if (crtc_timing->v_sync_width != hw_crtc_timing.v_sync_width)
		return false;

#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	/* block DSC for now, as VBIOS does not currently support DSC timings */
	if (crtc_timing->flags.DSC)
		return false;
#endif

	if (dc_is_dp_signal(link->connector_signal)) {
		unsigned int pix_clk_100hz;

		dc->res_pool->dp_clock_source->funcs->get_pixel_clk_frequency_100hz(
			dc->res_pool->dp_clock_source,
			tg_inst, &pix_clk_100hz);

		if (crtc_timing->pix_clk_100hz != pix_clk_100hz)
			return false;

		if (!se->funcs->dp_get_pixel_format)
			return false;

		if (!se->funcs->dp_get_pixel_format(
			se,
			&hw_crtc_timing.pixel_encoding,
			&hw_crtc_timing.display_color_depth))
			return false;

		if (hw_crtc_timing.display_color_depth != crtc_timing->display_color_depth)
			return false;

		if (hw_crtc_timing.pixel_encoding != crtc_timing->pixel_encoding)
			return false;
	}

	if (link->dpcd_caps.dprx_feature.bits.VSC_SDP_COLORIMETRY_SUPPORTED) {
		return false;
	}

	if (is_edp_ilr_optimization_required(link, crtc_timing)) {
		DC_LOG_EVENT_LINK_TRAINING("Seamless boot disabled to optimize eDP link rate\n");
		return false;
	}

	return true;
}

static inline bool should_update_pipe_for_stream(
		struct dc_state *context,
		struct pipe_ctx *pipe_ctx,
		struct dc_stream_state *stream)
{
	return (pipe_ctx->stream && pipe_ctx->stream == stream);
}

static inline bool should_update_pipe_for_plane(
		struct dc_state *context,
		struct pipe_ctx *pipe_ctx,
		struct dc_plane_state *plane_state)
{
	return (pipe_ctx->plane_state == plane_state);
}

void dc_enable_stereo(
	struct dc *dc,
	struct dc_state *context,
	struct dc_stream_state *streams[],
	uint8_t stream_count)
{
	int i, j;
	struct pipe_ctx *pipe;

	for (i = 0; i < MAX_PIPES; i++) {
		if (context != NULL) {
			pipe = &context->res_ctx.pipe_ctx[i];
		} else {
			context = dc->current_state;
			pipe = &dc->current_state->res_ctx.pipe_ctx[i];
		}

		for (j = 0; pipe && j < stream_count; j++)  {
			if (should_update_pipe_for_stream(context, pipe, streams[j]) &&
				dc->hwss.setup_stereo)
				dc->hwss.setup_stereo(pipe, dc);
		}
	}
}

void dc_trigger_sync(struct dc *dc, struct dc_state *context)
{
	if (context->stream_count > 1 && !dc->debug.disable_timing_sync) {
		enable_timing_multisync(dc, context);
		program_timing_sync(dc, context);
	}
}

static uint8_t get_stream_mask(struct dc *dc, struct dc_state *context)
{
	int i;
	unsigned int stream_mask = 0;

	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		if (context->res_ctx.pipe_ctx[i].stream)
			stream_mask |= 1 << i;
	}

	return stream_mask;
}

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
void dc_z10_restore(const struct dc *dc)
{
	if (dc->hwss.z10_restore)
		dc->hwss.z10_restore(dc);
}

void dc_z10_save_init(struct dc *dc)
{
	if (dc->hwss.z10_save_init)
		dc->hwss.z10_save_init(dc);
}
#endif
/*
 * Applies given context to HW and copy it into current context.
 * It's up to the user to release the src context afterwards.
 */
static enum dc_status dc_commit_state_no_check(struct dc *dc, struct dc_state *context)
{
	struct dc_bios *dcb = dc->ctx->dc_bios;
	enum dc_status result = DC_ERROR_UNEXPECTED;
	struct pipe_ctx *pipe;
	int i, k, l;
	struct dc_stream_state *dc_streams[MAX_STREAMS] = {0};

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	dc_z10_restore(dc);
	dc_allow_idle_optimizations(dc, false);
#endif

	for (i = 0; i < context->stream_count; i++)
		dc_streams[i] =  context->streams[i];

	if (!dcb->funcs->is_accelerated_mode(dcb)) {
		disable_vbios_mode_if_required(dc, context);
		dc->hwss.enable_accelerated_mode(dc, context);
	}

	if (context->stream_count > get_seamless_boot_stream_count(context) ||
		context->stream_count == 0)
		dc->hwss.prepare_bandwidth(dc, context);

	disable_dangling_plane(dc, context);
	/* re-program planes for existing stream, in case we need to
	 * free up plane resource for later use
	 */
	if (dc->hwss.apply_ctx_for_surface) {
		for (i = 0; i < context->stream_count; i++) {
			if (context->streams[i]->mode_changed)
				continue;
			apply_ctx_interdependent_lock(dc, context, context->streams[i], true);
			dc->hwss.apply_ctx_for_surface(
				dc, context->streams[i],
				context->stream_status[i].plane_count,
				context); /* use new pipe config in new context */
			apply_ctx_interdependent_lock(dc, context, context->streams[i], false);
			dc->hwss.post_unlock_program_front_end(dc, context);
		}
	}

	/* Program hardware */
	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		pipe = &context->res_ctx.pipe_ctx[i];
		dc->hwss.wait_for_mpcc_disconnect(dc, dc->res_pool, pipe);
	}

	result = dc->hwss.apply_ctx_to_hw(dc, context);

	if (result != DC_OK)
		return result;

	dc_trigger_sync(dc, context);

	/* Program all planes within new context*/
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
	if (dc->hwss.program_front_end_for_ctx) {
		dc->hwss.interdependent_update_lock(dc, context, true);
		dc->hwss.program_front_end_for_ctx(dc, context);
		dc->hwss.interdependent_update_lock(dc, context, false);
		dc->hwss.post_unlock_program_front_end(dc, context);
	}
#endif
	for (i = 0; i < context->stream_count; i++) {
		const struct dc_link *link = context->streams[i]->link;

		if (!context->streams[i]->mode_changed)
			continue;

		if (dc->hwss.apply_ctx_for_surface) {
			apply_ctx_interdependent_lock(dc, context, context->streams[i], true);
			dc->hwss.apply_ctx_for_surface(
					dc, context->streams[i],
					context->stream_status[i].plane_count,
					context);
			apply_ctx_interdependent_lock(dc, context, context->streams[i], false);
			dc->hwss.post_unlock_program_front_end(dc, context);
		}

		/*
		 * enable stereo
		 * TODO rework dc_enable_stereo call to work with validation sets?
		 */
		for (k = 0; k < MAX_PIPES; k++) {
			pipe = &context->res_ctx.pipe_ctx[k];

			for (l = 0 ; pipe && l < context->stream_count; l++)  {
				if (context->streams[l] &&
					context->streams[l] == pipe->stream &&
					dc->hwss.setup_stereo)
					dc->hwss.setup_stereo(pipe, dc);
			}
		}

		CONN_MSG_MODE(link, "{%dx%d, %dx%d@%dKhz}",
				context->streams[i]->timing.h_addressable,
				context->streams[i]->timing.v_addressable,
				context->streams[i]->timing.h_total,
				context->streams[i]->timing.v_total,
				context->streams[i]->timing.pix_clk_100hz / 10);
	}

	dc_enable_stereo(dc, context, dc_streams, context->stream_count);

	if (context->stream_count > get_seamless_boot_stream_count(context) ||
		context->stream_count == 0) {
		/* Must wait for no flips to be pending before doing optimize bw */
		wait_for_no_pipes_pending(dc, context);
		/* pplib is notified if disp_num changed */
		dc->hwss.optimize_bandwidth(dc, context);
	}

	if (dc->ctx->dce_version >= DCE_VERSION_MAX)
		TRACE_DCN_CLOCK_STATE(&context->bw_ctx.bw.dcn.clk);
	else
		TRACE_DCE_CLOCK_STATE(&context->bw_ctx.bw.dce);

	context->stream_mask = get_stream_mask(dc, context);

	if (context->stream_mask != dc->current_state->stream_mask)
		dc_dmub_srv_notify_stream_mask(dc->ctx->dmub_srv, context->stream_mask);

	for (i = 0; i < context->stream_count; i++)
		context->streams[i]->mode_changed = false;

	dc_release_state(dc->current_state);

	dc->current_state = context;

	dc_retain_state(dc->current_state);

	return result;
}

bool dc_commit_state(struct dc *dc, struct dc_state *context)
{
	enum dc_status result = DC_ERROR_UNEXPECTED;
	int i;

	if (!context_changed(dc, context))
		return DC_OK;

	DC_LOG_DC("%s: %d streams\n",
				__func__, context->stream_count);

	for (i = 0; i < context->stream_count; i++) {
		struct dc_stream_state *stream = context->streams[i];

		dc_stream_log(dc, stream);
	}

	/*
	 * Previous validation was perfomred with fast_validation = true and
	 * the full DML state required for hardware programming was skipped.
	 *
	 * Re-validate here to calculate these parameters / watermarks.
	 */
	result = dc_validate_global_state(dc, context, false);
	if (result != DC_OK) {
		DC_LOG_ERROR("DC commit global validation failure: %s (%d)",
			     dc_status_to_str(result), result);
		return result;
	}

	result = dc_commit_state_no_check(dc, context);

	return (result == DC_OK);
}

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
bool dc_acquire_release_mpc_3dlut(
		struct dc *dc, bool acquire,
		struct dc_stream_state *stream,
		struct dc_3dlut **lut,
		struct dc_transfer_func **shaper)
{
	int pipe_idx;
	bool ret = false;
	bool found_pipe_idx = false;
	const struct resource_pool *pool = dc->res_pool;
	struct resource_context *res_ctx = &dc->current_state->res_ctx;
	int mpcc_id = 0;

	if (pool && res_ctx) {
		if (acquire) {
			/*find pipe idx for the given stream*/
			for (pipe_idx = 0; pipe_idx < pool->pipe_count; pipe_idx++) {
				if (res_ctx->pipe_ctx[pipe_idx].stream == stream) {
					found_pipe_idx = true;
					mpcc_id = res_ctx->pipe_ctx[pipe_idx].plane_res.hubp->inst;
					break;
				}
			}
		} else
			found_pipe_idx = true;/*for release pipe_idx is not required*/

		if (found_pipe_idx) {
			if (acquire && pool->funcs->acquire_post_bldn_3dlut)
				ret = pool->funcs->acquire_post_bldn_3dlut(res_ctx, pool, mpcc_id, lut, shaper);
			else if (!acquire && pool->funcs->release_post_bldn_3dlut)
				ret = pool->funcs->release_post_bldn_3dlut(res_ctx, pool, lut, shaper);
		}
	}
	return ret;
}
#endif
static bool is_flip_pending_in_pipes(struct dc *dc, struct dc_state *context)
{
	int i;
	struct pipe_ctx *pipe;

	for (i = 0; i < MAX_PIPES; i++) {
		pipe = &context->res_ctx.pipe_ctx[i];

		if (!pipe->plane_state)
			continue;

		/* Must set to false to start with, due to OR in update function */
		pipe->plane_state->status.is_flip_pending = false;
		dc->hwss.update_pending_status(pipe);
		if (pipe->plane_state->status.is_flip_pending)
			return true;
	}
	return false;
}

#ifdef CONFIG_DRM_AMD_DC_DCN
/* Perform updates here which need to be deferred until next vupdate
 *
 * i.e. blnd lut, 3dlut, and shaper lut bypass regs are double buffered
 * but forcing lut memory to shutdown state is immediate. This causes
 * single frame corruption as lut gets disabled mid-frame unless shutdown
 * is deferred until after entering bypass.
 */
static void process_deferred_updates(struct dc *dc)
{
	int i = 0;

	if (dc->debug.enable_mem_low_power.bits.cm) {
		ASSERT(dc->dcn_ip->max_num_dpp);
		for (i = 0; i < dc->dcn_ip->max_num_dpp; i++)
			if (dc->res_pool->dpps[i]->funcs->dpp_deferred_update)
				dc->res_pool->dpps[i]->funcs->dpp_deferred_update(dc->res_pool->dpps[i]);
	}
}
#endif /* CONFIG_DRM_AMD_DC_DCN */

void dc_post_update_surfaces_to_stream(struct dc *dc)
{
	int i;
	struct dc_state *context = dc->current_state;

	if ((!dc->optimized_required) || get_seamless_boot_stream_count(context) > 0)
		return;

	post_surface_trace(dc);

	if (dc->ctx->dce_version >= DCE_VERSION_MAX)
		TRACE_DCN_CLOCK_STATE(&context->bw_ctx.bw.dcn.clk);
	else
		TRACE_DCE_CLOCK_STATE(&context->bw_ctx.bw.dce);

	if (is_flip_pending_in_pipes(dc, context))
		return;

	for (i = 0; i < dc->res_pool->pipe_count; i++)
		if (context->res_ctx.pipe_ctx[i].stream == NULL ||
		    context->res_ctx.pipe_ctx[i].plane_state == NULL) {
			context->res_ctx.pipe_ctx[i].pipe_idx = i;
			dc->hwss.disable_plane(dc, &context->res_ctx.pipe_ctx[i]);
		}

#ifdef CONFIG_DRM_AMD_DC_DCN
	process_deferred_updates(dc);
#endif

	dc->hwss.optimize_bandwidth(dc, context);

	dc->optimized_required = false;
	dc->wm_optimized_required = false;
}

static void init_state(struct dc *dc, struct dc_state *context)
{
	/* Each context must have their own instance of VBA and in order to
	 * initialize and obtain IP and SOC the base DML instance from DC is
	 * initially copied into every context
	 */
#ifdef CONFIG_DRM_AMD_DC_DCN1_0
	memcpy(&context->bw_ctx.dml, &dc->dml, sizeof(struct display_mode_lib));
#endif
}

struct dc_state *dc_create_state(struct dc *dc)
{
	struct dc_state *context = kvzalloc(sizeof(struct dc_state),
					    GFP_KERNEL);

	if (!context)
		return NULL;

	init_state(dc, context);

	kref_init(&context->refcount);

	return context;
}

struct dc_state *dc_copy_state(struct dc_state *src_ctx)
{
	int i, j;
	struct dc_state *new_ctx = kvmalloc(sizeof(struct dc_state), GFP_KERNEL);

	if (!new_ctx)
		return NULL;
	memcpy(new_ctx, src_ctx, sizeof(struct dc_state));

	for (i = 0; i < MAX_PIPES; i++) {
			struct pipe_ctx *cur_pipe = &new_ctx->res_ctx.pipe_ctx[i];

			if (cur_pipe->top_pipe)
				cur_pipe->top_pipe =  &new_ctx->res_ctx.pipe_ctx[cur_pipe->top_pipe->pipe_idx];

			if (cur_pipe->bottom_pipe)
				cur_pipe->bottom_pipe = &new_ctx->res_ctx.pipe_ctx[cur_pipe->bottom_pipe->pipe_idx];

			if (cur_pipe->prev_odm_pipe)
				cur_pipe->prev_odm_pipe =  &new_ctx->res_ctx.pipe_ctx[cur_pipe->prev_odm_pipe->pipe_idx];

			if (cur_pipe->next_odm_pipe)
				cur_pipe->next_odm_pipe = &new_ctx->res_ctx.pipe_ctx[cur_pipe->next_odm_pipe->pipe_idx];

	}

	for (i = 0; i < new_ctx->stream_count; i++) {
			dc_stream_retain(new_ctx->streams[i]);
			for (j = 0; j < new_ctx->stream_status[i].plane_count; j++)
				dc_plane_state_retain(
					new_ctx->stream_status[i].plane_states[j]);
	}

	kref_init(&new_ctx->refcount);

	return new_ctx;
}

void dc_retain_state(struct dc_state *context)
{
	kref_get(&context->refcount);
}

static void dc_state_free(struct kref *kref)
{
	struct dc_state *context = container_of(kref, struct dc_state, refcount);
	dc_resource_state_destruct(context);
	kvfree(context);
}

void dc_release_state(struct dc_state *context)
{
	kref_put(&context->refcount, dc_state_free);
}

bool dc_set_generic_gpio_for_stereo(bool enable,
		struct gpio_service *gpio_service)
{
	enum gpio_result gpio_result = GPIO_RESULT_NON_SPECIFIC_ERROR;
	struct gpio_pin_info pin_info;
	struct gpio *generic;
	struct gpio_generic_mux_config *config = kzalloc(sizeof(struct gpio_generic_mux_config),
			   GFP_KERNEL);

	if (!config)
		return false;
	pin_info = dal_gpio_get_generic_pin_info(gpio_service, GPIO_ID_GENERIC, 0);

	if (pin_info.mask == 0xFFFFFFFF || pin_info.offset == 0xFFFFFFFF) {
		kfree(config);
		return false;
	} else {
		generic = dal_gpio_service_create_generic_mux(
			gpio_service,
			pin_info.offset,
			pin_info.mask);
	}

	if (!generic) {
		kfree(config);
		return false;
	}

	gpio_result = dal_gpio_open(generic, GPIO_MODE_OUTPUT);

	config->enable_output_from_mux = enable;
	config->mux_select = GPIO_SIGNAL_SOURCE_PASS_THROUGH_STEREO_SYNC;

	if (gpio_result == GPIO_RESULT_OK)
		gpio_result = dal_mux_setup_config(generic, config);

	if (gpio_result == GPIO_RESULT_OK) {
		dal_gpio_close(generic);
		dal_gpio_destroy_generic_mux(&generic);
		kfree(config);
		return true;
	} else {
		dal_gpio_close(generic);
		dal_gpio_destroy_generic_mux(&generic);
		kfree(config);
		return false;
	}
}

static bool is_surface_in_context(
		const struct dc_state *context,
		const struct dc_plane_state *plane_state)
{
	int j;

	for (j = 0; j < MAX_PIPES; j++) {
		const struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[j];

		if (plane_state == pipe_ctx->plane_state) {
			return true;
		}
	}

	return false;
}

static enum surface_update_type get_plane_info_update_type(const struct dc_surface_update *u)
{
	union surface_update_flags *update_flags = &u->surface->update_flags;
	enum surface_update_type update_type = UPDATE_TYPE_FAST;

	if (!u->plane_info)
		return UPDATE_TYPE_FAST;

	if (u->plane_info->color_space != u->surface->color_space) {
		update_flags->bits.color_space_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_MED);
	}

	if (u->plane_info->horizontal_mirror != u->surface->horizontal_mirror) {
		update_flags->bits.horizontal_mirror_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_MED);
	}

	if (u->plane_info->rotation != u->surface->rotation) {
		update_flags->bits.rotation_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_FULL);
	}

	if (u->plane_info->format != u->surface->format) {
		update_flags->bits.pixel_format_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_FULL);
	}

	if (u->plane_info->stereo_format != u->surface->stereo_format) {
		update_flags->bits.stereo_format_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_FULL);
	}

	if (u->plane_info->per_pixel_alpha != u->surface->per_pixel_alpha) {
		update_flags->bits.per_pixel_alpha_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_MED);
	}

	if (u->plane_info->global_alpha_value != u->surface->global_alpha_value) {
		update_flags->bits.global_alpha_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_MED);
	}

	if (u->plane_info->dcc.enable != u->surface->dcc.enable
#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
			|| u->plane_info->dcc.dcc_ind_blk != u->surface->dcc.dcc_ind_blk
#else
			|| u->plane_info->dcc.independent_64b_blks != u->surface->dcc.independent_64b_blks
#endif
			|| u->plane_info->dcc.meta_pitch != u->surface->dcc.meta_pitch) {
		/* During DCC on/off, stutter period is calculated before
		 * DCC has fully transitioned. This results in incorrect
		 * stutter period calculation. Triggering a full update will
		 * recalculate stutter period.
		 */
		update_flags->bits.dcc_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_FULL);
	}

	if (resource_pixel_format_to_bpp(u->plane_info->format) !=
			resource_pixel_format_to_bpp(u->surface->format)) {
		/* different bytes per element will require full bandwidth
		 * and DML calculation
		 */
		update_flags->bits.bpp_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_FULL);
	}

	if (u->plane_info->plane_size.surface_pitch != u->surface->plane_size.surface_pitch
			|| u->plane_info->plane_size.chroma_pitch != u->surface->plane_size.chroma_pitch) {
		update_flags->bits.plane_size_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_MED);
	}


	if (memcmp(&u->plane_info->tiling_info, &u->surface->tiling_info,
			sizeof(union dc_tiling_info)) != 0) {
		update_flags->bits.swizzle_change = 1;
		elevate_update_type(&update_type, UPDATE_TYPE_MED);

		/* todo: below are HW dependent, we should add a hook to
		 * DCE/N resource and validated there.
		 */
		if (u->plane_info->tiling_info.gfx9.swizzle != DC_SW_LINEAR) {
			/* swizzled mode requires RQ to be setup properly,
			 * thus need to run DML to calculate RQ settings
			 */
			update_flags->bits.bandwidth_change = 1;
			elevate_update_type(&update_type, UPDATE_TYPE_FULL);
		}
	}

	/* This should be UPDATE_TYPE_FAST if nothing has changed. */
	return update_type;
}

static enum surface_update_type get_scaling_info_update_type(
		const struct dc_surface_update *u)
{
	union surface_update_flags *update_flags = &u->surface->update_flags;

	if (!u->scaling_info)
		return UPDATE_TYPE_FAST;

	if (u->scaling_info->clip_rect.width != u->surface->clip_rect.width
			|| u->scaling_info->clip_rect.height != u->surface->clip_rect.height
			|| u->scaling_info->dst_rect.width != u->surface->dst_rect.width
			|| u->scaling_info->dst_rect.height != u->surface->dst_rect.height
			|| u->scaling_info->scaling_quality.integer_scaling !=
				u->surface->scaling_quality.integer_scaling
			) {
		update_flags->bits.scaling_change = 1;

		if ((u->scaling_info->dst_rect.width < u->surface->dst_rect.width
			|| u->scaling_info->dst_rect.height < u->surface->dst_rect.height)
				&& (u->scaling_info->dst_rect.width < u->surface->src_rect.width
					|| u->scaling_info->dst_rect.height < u->surface->src_rect.height))
			/* Making dst rect smaller requires a bandwidth change */
			update_flags->bits.bandwidth_change = 1;
	}

	if (u->scaling_info->src_rect.width != u->surface->src_rect.width
		|| u->scaling_info->src_rect.height != u->surface->src_rect.height) {

		update_flags->bits.scaling_change = 1;
		if (u->scaling_info->src_rect.width > u->surface->src_rect.width
				|| u->scaling_info->src_rect.height > u->surface->src_rect.height)
			/* Making src rect bigger requires a bandwidth change */
			update_flags->bits.clock_change = 1;
	}

	if (u->scaling_info->src_rect.x != u->surface->src_rect.x
			|| u->scaling_info->src_rect.y != u->surface->src_rect.y
			|| u->scaling_info->clip_rect.x != u->surface->clip_rect.x
			|| u->scaling_info->clip_rect.y != u->surface->clip_rect.y
			|| u->scaling_info->dst_rect.x != u->surface->dst_rect.x
			|| u->scaling_info->dst_rect.y != u->surface->dst_rect.y)
		update_flags->bits.position_change = 1;

	if (update_flags->bits.clock_change
			|| update_flags->bits.bandwidth_change
			|| update_flags->bits.scaling_change)
		return UPDATE_TYPE_FULL;

	if (update_flags->bits.position_change)
		return UPDATE_TYPE_MED;

	return UPDATE_TYPE_FAST;
}

static enum surface_update_type det_surface_update(const struct dc *dc,
		const struct dc_surface_update *u)
{
	const struct dc_state *context = dc->current_state;
	enum surface_update_type type;
	enum surface_update_type overall_type = UPDATE_TYPE_FAST;
	union surface_update_flags *update_flags = &u->surface->update_flags;

	if (u->flip_addr)
		update_flags->bits.addr_update = 1;

	if (!is_surface_in_context(context, u->surface) || u->surface->force_full_update) {
		update_flags->raw = 0xFFFFFFFF;
		return UPDATE_TYPE_FULL;
	}

	update_flags->raw = 0; // Reset all flags

	type = get_plane_info_update_type(u);
	elevate_update_type(&overall_type, type);

	type = get_scaling_info_update_type(u);
	elevate_update_type(&overall_type, type);

	if (u->flip_addr)
		update_flags->bits.addr_update = 1;

	if (u->in_transfer_func)
		update_flags->bits.in_transfer_func_change = 1;

	if (u->input_csc_color_matrix)
		update_flags->bits.input_csc_change = 1;

	if (u->coeff_reduction_factor)
		update_flags->bits.coeff_reduction_change = 1;

	if (u->gamut_remap_matrix)
		update_flags->bits.gamut_remap_change = 1;

	if (u->gamma) {
		enum surface_pixel_format format = SURFACE_PIXEL_FORMAT_GRPH_BEGIN;

		if (u->plane_info)
			format = u->plane_info->format;
		else if (u->surface)
			format = u->surface->format;

		if (dce_use_lut(format))
			update_flags->bits.gamma_change = 1;
	}
#ifdef CONFIG_DRM_AMD_DC_DCN2_x
	if (u->lut3d_func || u->func_shaper)
		update_flags->bits.lut_3d = 1;
#endif

	if (u->hdr_mult.value)
		if (u->hdr_mult.value != u->surface->hdr_mult.value) {
			update_flags->bits.hdr_mult = 1;
			elevate_update_type(&overall_type, UPDATE_TYPE_MED);
		}

	if (update_flags->bits.in_transfer_func_change) {
		type = UPDATE_TYPE_MED;
		elevate_update_type(&overall_type, type);
	}

	if (update_flags->bits.input_csc_change
			|| update_flags->bits.coeff_reduction_change
			|| update_flags->bits.lut_3d
			|| update_flags->bits.gamma_change
			|| update_flags->bits.gamut_remap_change) {
		type = UPDATE_TYPE_FULL;
		elevate_update_type(&overall_type, type);
	}

	return overall_type;
}

static enum surface_update_type check_update_surfaces_for_stream(
		struct dc *dc,
		struct dc_surface_update *updates,
		int surface_count,
		struct dc_stream_update *stream_update,
		const struct dc_stream_status *stream_status)
{
	int i;
	enum surface_update_type overall_type = UPDATE_TYPE_FAST;

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	if (dc->idle_optimizations_allowed)
		overall_type = UPDATE_TYPE_FULL;

#endif
	if (stream_status == NULL || stream_status->plane_count != surface_count)
		overall_type = UPDATE_TYPE_FULL;

	if (stream_update && stream_update->pending_test_pattern) {
		overall_type = UPDATE_TYPE_FULL;
	}

	/* some stream updates require passive update */
	if (stream_update) {
		union stream_update_flags *su_flags = &stream_update->stream->update_flags;

		if ((stream_update->src.height != 0 && stream_update->src.width != 0) ||
			(stream_update->dst.height != 0 && stream_update->dst.width != 0) ||
			stream_update->integer_scaling_update)
			su_flags->bits.scaling = 1;

		if (stream_update->out_transfer_func)
			su_flags->bits.out_tf = 1;

		if (stream_update->abm_level)
			su_flags->bits.abm_level = 1;

		if (stream_update->dpms_off)
			su_flags->bits.dpms_off = 1;

		if (stream_update->gamut_remap)
			su_flags->bits.gamut_remap = 1;

#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
		if (stream_update->wb_update)
			su_flags->bits.wb_update = 1;
#endif

#if defined(CONFIG_DRM_AMD_DC_DSC_SUPPORT)
		if (stream_update->dsc_config)
			su_flags->bits.dsc_changed = 1;
#endif

		if (stream_update->mst_bw_update)
			su_flags->bits.mst_bw = 1;

		if (su_flags->raw != 0)
			overall_type = UPDATE_TYPE_FULL;

		if (stream_update->output_csc_transform || stream_update->output_color_space)
			su_flags->bits.out_csc = 1;
	}

	for (i = 0 ; i < surface_count; i++) {
		enum surface_update_type type =
				det_surface_update(dc, &updates[i]);

		elevate_update_type(&overall_type, type);
	}

	return overall_type;
}

/*
 * dc_check_update_surfaces_for_stream() - Determine update type (fast, med, or full)
 *
 * See :c:type:`enum surface_update_type <surface_update_type>` for explanation of update types
 */
enum surface_update_type dc_check_update_surfaces_for_stream(
		struct dc *dc,
		struct dc_surface_update *updates,
		int surface_count,
		struct dc_stream_update *stream_update,
		const struct dc_stream_status *stream_status)
{
	int i;
	enum surface_update_type type;

	if (stream_update)
		stream_update->stream->update_flags.raw = 0;
	for (i = 0; i < surface_count; i++)
		updates[i].surface->update_flags.raw = 0;

	type = check_update_surfaces_for_stream(dc, updates, surface_count, stream_update, stream_status);
	if (type == UPDATE_TYPE_FULL) {
		if (stream_update) {
			uint32_t dsc_changed = stream_update->stream->update_flags.bits.dsc_changed;
			stream_update->stream->update_flags.raw = 0xFFFFFFFF;
			stream_update->stream->update_flags.bits.dsc_changed = dsc_changed;
		}
		for (i = 0; i < surface_count; i++)
			updates[i].surface->update_flags.raw = 0xFFFFFFFF;
	}

	if (type == UPDATE_TYPE_FAST) {
		// If there's an available clock comparator, we use that.
		if (dc->clk_mgr->funcs->are_clock_states_equal) {
			if (!dc->clk_mgr->funcs->are_clock_states_equal(&dc->clk_mgr->clks, &dc->current_state->bw_ctx.bw.dcn.clk))
				dc->optimized_required = true;
		// Else we fallback to mem compare.
		} else if (memcmp(&dc->current_state->bw_ctx.bw.dcn.clk, &dc->clk_mgr->clks, offsetof(struct dc_clocks, prev_p_state_change_support)) != 0) {
			dc->optimized_required = true;
		}

		dc->optimized_required |= dc->wm_optimized_required;
	}

	return type;
}

static struct dc_stream_status *stream_get_status(
	struct dc_state *ctx,
	struct dc_stream_state *stream)
{
	uint8_t i;

	for (i = 0; i < ctx->stream_count; i++) {
		if (stream == ctx->streams[i]) {
			return &ctx->stream_status[i];
		}
	}

	return NULL;
}

static const enum surface_update_type update_surface_trace_level = UPDATE_TYPE_FULL;

static void copy_surface_update_to_plane(
		struct dc_plane_state *surface,
		struct dc_surface_update *srf_update)
{
	if (srf_update->flip_addr) {
		surface->address = srf_update->flip_addr->address;
		surface->flip_immediate =
			srf_update->flip_addr->flip_immediate;
		surface->time.time_elapsed_in_us[surface->time.index] =
			srf_update->flip_addr->flip_timestamp_in_us -
				surface->time.prev_update_time_in_us;
		surface->time.prev_update_time_in_us =
			srf_update->flip_addr->flip_timestamp_in_us;
		surface->time.index++;
		if (surface->time.index >= DC_PLANE_UPDATE_TIMES_MAX)
			surface->time.index = 0;

#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
		surface->triplebuffer_flips = srf_update->flip_addr->triplebuffer_flips;
#endif
	}

	if (srf_update->scaling_info) {
		surface->scaling_quality =
				srf_update->scaling_info->scaling_quality;
		surface->dst_rect =
				srf_update->scaling_info->dst_rect;
		surface->src_rect =
				srf_update->scaling_info->src_rect;
		surface->clip_rect =
				srf_update->scaling_info->clip_rect;
	}

	if (srf_update->plane_info) {
		surface->color_space =
				srf_update->plane_info->color_space;
		surface->format =
				srf_update->plane_info->format;
		surface->plane_size =
				srf_update->plane_info->plane_size;
		surface->rotation =
				srf_update->plane_info->rotation;
		surface->horizontal_mirror =
				srf_update->plane_info->horizontal_mirror;
		surface->stereo_format =
				srf_update->plane_info->stereo_format;
		surface->tiling_info =
				srf_update->plane_info->tiling_info;
		surface->visible =
				srf_update->plane_info->visible;
		surface->per_pixel_alpha =
				srf_update->plane_info->per_pixel_alpha;
		surface->global_alpha =
				srf_update->plane_info->global_alpha;
		surface->global_alpha_value =
				srf_update->plane_info->global_alpha_value;
		surface->dcc =
				srf_update->plane_info->dcc;
		surface->layer_index =
				srf_update->plane_info->layer_index;
	}

	if (srf_update->gamma &&
			(surface->gamma_correction !=
					srf_update->gamma)) {
		memcpy(&surface->gamma_correction->entries,
			&srf_update->gamma->entries,
			sizeof(struct dc_gamma_entries));
		surface->gamma_correction->is_identity =
			srf_update->gamma->is_identity;
		surface->gamma_correction->num_entries =
			srf_update->gamma->num_entries;
		surface->gamma_correction->type =
			srf_update->gamma->type;
	}

	if (srf_update->in_transfer_func &&
			(surface->in_transfer_func !=
				srf_update->in_transfer_func)) {
		surface->in_transfer_func->sdr_ref_white_level =
			srf_update->in_transfer_func->sdr_ref_white_level;
		surface->in_transfer_func->tf =
			srf_update->in_transfer_func->tf;
		surface->in_transfer_func->type =
			srf_update->in_transfer_func->type;
		memcpy(&surface->in_transfer_func->tf_pts,
			&srf_update->in_transfer_func->tf_pts,
			sizeof(struct dc_transfer_func_distributed_points));
	}

#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
	if (srf_update->func_shaper &&
			(surface->in_shaper_func !=
			srf_update->func_shaper))
		memcpy(surface->in_shaper_func, srf_update->func_shaper,
		sizeof(*surface->in_shaper_func));

	if (srf_update->lut3d_func &&
			(surface->lut3d_func !=
			srf_update->lut3d_func))
		memcpy(surface->lut3d_func, srf_update->lut3d_func,
		sizeof(*surface->lut3d_func));

	if (srf_update->hdr_mult.value)
		surface->hdr_mult =
				srf_update->hdr_mult;

	if (srf_update->blend_tf &&
			(surface->blend_tf !=
			srf_update->blend_tf))
		memcpy(surface->blend_tf, srf_update->blend_tf,
		sizeof(*surface->blend_tf));
#endif

	if (srf_update->input_csc_color_matrix)
		surface->input_csc_color_matrix =
			*srf_update->input_csc_color_matrix;

	if (srf_update->coeff_reduction_factor)
		surface->coeff_reduction_factor =
			*srf_update->coeff_reduction_factor;

	if (srf_update->gamut_remap_matrix)
		surface->gamut_remap_matrix =
			*srf_update->gamut_remap_matrix;
}

static void copy_stream_update_to_stream(struct dc *dc,
					 struct dc_state *context,
					 struct dc_stream_state *stream,
					 struct dc_stream_update *update)
{
	struct dc_context *dc_ctx = dc->ctx;

	if (update == NULL || stream == NULL)
		return;

	if (update->src.height && update->src.width)
		stream->src = update->src;

	if (update->dst.height && update->dst.width)
		stream->dst = update->dst;

	if (update->out_transfer_func &&
	    stream->out_transfer_func != update->out_transfer_func) {
		stream->out_transfer_func->sdr_ref_white_level =
			update->out_transfer_func->sdr_ref_white_level;
		stream->out_transfer_func->tf = update->out_transfer_func->tf;
		stream->out_transfer_func->type =
			update->out_transfer_func->type;
		memcpy(&stream->out_transfer_func->tf_pts,
		       &update->out_transfer_func->tf_pts,
		       sizeof(struct dc_transfer_func_distributed_points));
	}

	if (update->hdr_static_metadata)
		stream->hdr_static_metadata = *update->hdr_static_metadata;

	if (update->abm_level)
		stream->abm_level = *update->abm_level;

	if (update->periodic_interrupt0)
		stream->periodic_interrupt0 = *update->periodic_interrupt0;

	if (update->periodic_interrupt1)
		stream->periodic_interrupt1 = *update->periodic_interrupt1;

	if (update->gamut_remap)
		stream->gamut_remap_matrix = *update->gamut_remap;

	/* Note: this being updated after mode set is currently not a use case
	 * however if it arises OCSC would need to be reprogrammed at the
	 * minimum
	 */
	if (update->output_color_space)
		stream->output_color_space = *update->output_color_space;

	if (update->output_csc_transform)
		stream->csc_color_matrix = *update->output_csc_transform;

	if (update->vrr_infopacket)
		stream->vrr_infopacket = *update->vrr_infopacket;

	if (update->dpms_off)
		stream->dpms_off = *update->dpms_off;

	if (update->vsc_infopacket)
		stream->vsc_infopacket = *update->vsc_infopacket;

	if (update->vsp_infopacket)
		stream->vsp_infopacket = *update->vsp_infopacket;

	if (update->dither_option)
		stream->dither_option = *update->dither_option;

	if (update->pending_test_pattern)
		stream->test_pattern = *update->pending_test_pattern;

 #if defined(CONFIG_DRM_AMD_DC_DCN2_x)
	/* update current stream with writeback info */
	if (update->wb_update) {
		int i;

		stream->num_wb_info = update->wb_update->num_wb_info;
		ASSERT(stream->num_wb_info <= MAX_DWB_PIPES);
		for (i = 0; i < stream->num_wb_info; i++)
			stream->writeback_info[i] =
				update->wb_update->writeback_info[i];
	}
#endif
#if defined(CONFIG_DRM_AMD_DC_DSC_SUPPORT)
	if (update->dsc_config) {
		struct dc_dsc_config old_dsc_cfg = stream->timing.dsc_cfg;
		uint32_t old_dsc_enabled = stream->timing.flags.DSC;
		uint32_t enable_dsc = (update->dsc_config->num_slices_h != 0 &&
				       update->dsc_config->num_slices_v != 0);

		/* Use temporarry context for validating new DSC config */
		struct dc_state *dsc_validate_context = dc_create_state(dc);

		if (dsc_validate_context) {
			dc_resource_state_copy_construct(dc->current_state, dsc_validate_context);

			stream->timing.dsc_cfg = *update->dsc_config;
			stream->timing.flags.DSC = enable_dsc;
			if (!dc->res_pool->funcs->validate_bandwidth(dc, dsc_validate_context, true)) {
				stream->timing.dsc_cfg = old_dsc_cfg;
				stream->timing.flags.DSC = old_dsc_enabled;
				update->dsc_config = NULL;
			}

			dc_release_state(dsc_validate_context);
		} else {
			DC_ERROR("Failed to allocate new validate context for DSC change\n");
			update->dsc_config = NULL;
		}
	}
#endif
}

static void commit_planes_do_stream_update(struct dc *dc,
		struct dc_stream_state *stream,
		struct dc_stream_update *stream_update,
		enum surface_update_type update_type,
		struct dc_state *context)
{
	int j;

	// Stream updates
	for (j = 0; j < dc->res_pool->pipe_count; j++) {
		struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[j];

		if (!pipe_ctx->top_pipe &&  !pipe_ctx->prev_odm_pipe && pipe_ctx->stream == stream) {

			if (stream_update->periodic_interrupt0 &&
					dc->hwss.setup_periodic_interrupt)
				dc->hwss.setup_periodic_interrupt(dc, pipe_ctx, VLINE0);

			if (stream_update->periodic_interrupt1 &&
					dc->hwss.setup_periodic_interrupt)
				dc->hwss.setup_periodic_interrupt(dc, pipe_ctx, VLINE1);

			if ((stream_update->hdr_static_metadata && !stream->use_dynamic_meta) ||
					stream_update->vrr_infopacket ||
					stream_update->vsc_infopacket ||
					stream_update->vsp_infopacket) {
				resource_build_info_frame(pipe_ctx);
				dc->hwss.update_info_frame(pipe_ctx);

				if (dc_is_dp_signal(pipe_ctx->stream->signal))
					dp_source_sequence_trace(pipe_ctx->stream->link, DPCD_SOURCE_SEQ_AFTER_UPDATE_INFO_FRAME);
			}

			if (stream_update->hdr_static_metadata &&
					stream->use_dynamic_meta &&
					dc->hwss.set_dmdata_attributes &&
					pipe_ctx->stream->dmdata_address.quad_part != 0)
				dc->hwss.set_dmdata_attributes(pipe_ctx);

			if (stream_update->gamut_remap)
				dc_stream_set_gamut_remap(dc, stream);

			if (stream_update->output_csc_transform)
				dc_stream_program_csc_matrix(dc, stream);

			if (stream_update->dither_option) {
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
				struct pipe_ctx *odm_pipe = pipe_ctx->next_odm_pipe;
#endif
				resource_build_bit_depth_reduction_params(pipe_ctx->stream,
									&pipe_ctx->stream->bit_depth_params);
				pipe_ctx->stream_res.opp->funcs->opp_program_fmt(pipe_ctx->stream_res.opp,
						&stream->bit_depth_params,
						&stream->clamping);
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
				while (odm_pipe) {
					odm_pipe->stream_res.opp->funcs->opp_program_fmt(odm_pipe->stream_res.opp,
							&stream->bit_depth_params,
							&stream->clamping);
					odm_pipe = odm_pipe->next_odm_pipe;
				}
#endif
			}


			/* Full fe update*/
			if (update_type == UPDATE_TYPE_FAST)
				continue;

#if defined(CONFIG_DRM_AMD_DC_DSC_SUPPORT)
			if (stream_update->dsc_config)
				dp_update_dsc_config(pipe_ctx);
#endif

			if (stream_update->mst_bw_update) {
				if (stream_update->mst_bw_update->is_increase)
					dc_link_increase_mst_payload(pipe_ctx, stream_update->mst_bw_update->mst_stream_bw);
				else
					dc_link_reduce_mst_payload(pipe_ctx, stream_update->mst_bw_update->mst_stream_bw);
			}

			if (stream_update->pending_test_pattern) {
				dc_link_dp_set_test_pattern(stream->link,
					stream->test_pattern.type,
					stream->test_pattern.color_space,
					stream->test_pattern.p_link_settings,
					stream->test_pattern.p_custom_pattern,
					stream->test_pattern.cust_pattern_size);
			}

			if (stream_update->dpms_off) {
				if (*stream_update->dpms_off) {
					core_link_disable_stream(pipe_ctx);
					/* for dpms, keep acquired resources*/
					if (pipe_ctx->stream_res.audio && !dc->debug.az_endpoint_mute_only)
						pipe_ctx->stream_res.audio->funcs->az_disable(pipe_ctx->stream_res.audio);

					dc->optimized_required = true;

				} else {
					if (get_seamless_boot_stream_count(context) == 0)
						dc->hwss.prepare_bandwidth(dc, dc->current_state);

					core_link_enable_stream(dc->current_state, pipe_ctx);
				}
			}

			if (stream_update->abm_level && pipe_ctx->stream_res.abm) {
				bool should_program_abm = true;

				// if otg funcs defined check if blanked before programming
				if (pipe_ctx->stream_res.tg->funcs->is_blanked)
					if (pipe_ctx->stream_res.tg->funcs->is_blanked(pipe_ctx->stream_res.tg))
						should_program_abm = false;

				if (should_program_abm) {
					if (*stream_update->abm_level == ABM_LEVEL_IMMEDIATE_DISABLE) {
						dc->hwss.set_abm_immediate_disable(pipe_ctx);
					} else {
						pipe_ctx->stream_res.abm->funcs->set_abm_level(
							pipe_ctx->stream_res.abm, stream->abm_level);
					}
				}
			}
		}
	}
}

static void commit_planes_for_stream(struct dc *dc,
		struct dc_surface_update *srf_updates,
		int surface_count,
		struct dc_stream_state *stream,
		struct dc_stream_update *stream_update,
		enum surface_update_type update_type,
		struct dc_state *context)
{
	int i, j;
	struct pipe_ctx *top_pipe_to_program = NULL;
	bool should_lock_all_pipes = (update_type != UPDATE_TYPE_FAST);

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
	dc_z10_restore(dc);
#endif

	if (get_seamless_boot_stream_count(context) > 0 && surface_count > 0) {
		/* Optimize seamless boot flag keeps clocks and watermarks high until
		 * first flip. After first flip, optimization is required to lower
		 * bandwidth. Important to note that it is expected UEFI will
		 * only light up a single display on POST, therefore we only expect
		 * one stream with seamless boot flag set.
		 */
		if (stream->apply_seamless_boot_optimization) {
			stream->apply_seamless_boot_optimization = false;

			if (get_seamless_boot_stream_count(context) == 0)
				dc->optimized_required = true;
		}
	}

	if (update_type == UPDATE_TYPE_FULL) {
#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
		dc_allow_idle_optimizations(dc, false);

#endif
		if (get_seamless_boot_stream_count(context) == 0)
			dc->hwss.prepare_bandwidth(dc, context);

		context_clock_trace(dc, context);
	}

	for (j = 0; j < dc->res_pool->pipe_count; j++) {
		struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[j];

		if (!pipe_ctx->top_pipe &&
			!pipe_ctx->prev_odm_pipe &&
			pipe_ctx->stream &&
			pipe_ctx->stream == stream) {
			top_pipe_to_program = pipe_ctx;
		}
	}

#ifdef CONFIG_DRM_AMD_DC_DCN1_0
	if (stream->test_pattern.type != DP_TEST_PATTERN_VIDEO_MODE) {
		struct pipe_ctx *mpcc_pipe;
		struct pipe_ctx *odm_pipe;

		for (mpcc_pipe = top_pipe_to_program; mpcc_pipe; mpcc_pipe = mpcc_pipe->bottom_pipe)
			for (odm_pipe = mpcc_pipe; odm_pipe; odm_pipe = odm_pipe->next_odm_pipe)
				odm_pipe->ttu_regs.min_ttu_vblank = MAX_TTU;
	}
#endif

	if ((update_type != UPDATE_TYPE_FAST) && stream->update_flags.bits.dsc_changed)
		if (top_pipe_to_program &&
			top_pipe_to_program->stream_res.tg->funcs->lock_doublebuffer_enable) {
			if (should_use_dmub_lock(stream->link)) {
				union dmub_hw_lock_flags hw_locks = { 0 };
				struct dmub_hw_lock_inst_flags inst_flags = { 0 };

				hw_locks.bits.lock_dig = 1;
				inst_flags.dig_inst = top_pipe_to_program->stream_res.tg->inst;

				dmub_hw_lock_mgr_cmd(dc->ctx->dmub_srv,
							true,
							&hw_locks,
							&inst_flags);
			} else
				top_pipe_to_program->stream_res.tg->funcs->lock_doublebuffer_enable(
						top_pipe_to_program->stream_res.tg);
		}

	if (should_lock_all_pipes && dc->hwss.interdependent_update_lock)
		dc->hwss.interdependent_update_lock(dc, context, true);
	else
		/* Lock the top pipe while updating plane addrs, since freesync requires
		 *  plane addr update event triggers to be synchronized.
		 *  top_pipe_to_program is expected to never be NULL
		 */
		dc->hwss.pipe_control_lock(dc, top_pipe_to_program, true);

	// Stream updates
	if (stream_update)
		commit_planes_do_stream_update(dc, stream, stream_update, update_type, context);

	if (surface_count == 0) {
		/*
		 * In case of turning off screen, no need to program front end a second time.
		 * just return after program blank.
		 */
		if (dc->hwss.apply_ctx_for_surface)
			dc->hwss.apply_ctx_for_surface(dc, stream, 0, context);
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
		if (dc->hwss.program_front_end_for_ctx)
			dc->hwss.program_front_end_for_ctx(dc, context);
#endif

		if (should_lock_all_pipes && dc->hwss.interdependent_update_lock)
			dc->hwss.interdependent_update_lock(dc, context, false);
		else
			dc->hwss.pipe_control_lock(dc, top_pipe_to_program, false);
		dc->hwss.post_unlock_program_front_end(dc, context);
		return;
	}

#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
	if (!IS_DIAG_DC(dc->ctx->dce_environment)) {
		for (i = 0; i < surface_count; i++) {
			struct dc_plane_state *plane_state = srf_updates[i].surface;
			/*set logical flag for lock/unlock use*/
			for (j = 0; j < dc->res_pool->pipe_count; j++) {
				struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[j];
				if (!pipe_ctx->plane_state)
					continue;
				if (should_update_pipe_for_plane(context, pipe_ctx, plane_state))
					continue;
				pipe_ctx->plane_state->triplebuffer_flips = false;
				if (update_type == UPDATE_TYPE_FAST &&
					dc->hwss.program_triplebuffer != NULL &&
					!pipe_ctx->plane_state->flip_immediate && dc->debug.enable_tri_buf) {
						/*triple buffer for VUpdate  only*/
						pipe_ctx->plane_state->triplebuffer_flips = true;
				}
			}
			if (update_type == UPDATE_TYPE_FULL) {
				/* force vsync flip when reconfiguring pipes to prevent underflow */
				plane_state->flip_immediate = false;
			}
		}
	}
#endif

	// Update Type FULL, Surface updates
	for (j = 0; j < dc->res_pool->pipe_count; j++) {
		struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[j];

		if (!pipe_ctx->top_pipe &&
			!pipe_ctx->prev_odm_pipe &&
			should_update_pipe_for_stream(context, pipe_ctx, stream)) {
			struct dc_stream_status *stream_status = NULL;

			if (!pipe_ctx->plane_state)
				continue;

			/* Full fe update*/
			if (update_type == UPDATE_TYPE_FAST)
				continue;

#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
			ASSERT(!pipe_ctx->plane_state->triplebuffer_flips);

			if (dc->hwss.program_triplebuffer != NULL && dc->debug.enable_tri_buf) {
				/*turn off triple buffer for full update*/
				dc->hwss.program_triplebuffer(
					dc, pipe_ctx, pipe_ctx->plane_state->triplebuffer_flips);
			}
#endif
			stream_status =
				stream_get_status(context, pipe_ctx->stream);

			if (dc->hwss.apply_ctx_for_surface)
				dc->hwss.apply_ctx_for_surface(
					dc, pipe_ctx->stream, stream_status->plane_count, context);
		}
	}

#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
	if (dc->hwss.program_front_end_for_ctx && update_type != UPDATE_TYPE_FAST) {
		dc->hwss.program_front_end_for_ctx(dc, context);
#ifdef CONFIG_DRM_AMD_DC_DCN1_0
		if (dc->debug.validate_dml_output) {
			for (i = 0; i < dc->res_pool->pipe_count; i++) {
				struct pipe_ctx *cur_pipe = &context->res_ctx.pipe_ctx[i];
				if (cur_pipe->stream == NULL)
					continue;

				cur_pipe->plane_res.hubp->funcs->validate_dml_output(
						cur_pipe->plane_res.hubp, dc->ctx,
						&context->res_ctx.pipe_ctx[i].rq_regs,
						&context->res_ctx.pipe_ctx[i].dlg_regs,
						&context->res_ctx.pipe_ctx[i].ttu_regs);
			}
		}
#endif
	}
#endif

	// Update Type FAST, Surface updates
	if (update_type == UPDATE_TYPE_FAST) {
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
		if (dc->hwss.set_flip_control_gsl)
			for (i = 0; i < surface_count; i++) {
				struct dc_plane_state *plane_state = srf_updates[i].surface;

				for (j = 0; j < dc->res_pool->pipe_count; j++) {
					struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[j];

					if (!should_update_pipe_for_stream(context, pipe_ctx, stream))
						continue;

					if (!should_update_pipe_for_plane(context, pipe_ctx, plane_state))
						continue;

					// GSL has to be used for flip immediate
					dc->hwss.set_flip_control_gsl(pipe_ctx,
							pipe_ctx->plane_state->flip_immediate);
				}
			}
#endif

		/* Perform requested Updates */
		for (i = 0; i < surface_count; i++) {
			struct dc_plane_state *plane_state = srf_updates[i].surface;

			for (j = 0; j < dc->res_pool->pipe_count; j++) {
				struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[j];

				if (!should_update_pipe_for_stream(context, pipe_ctx, stream))
					continue;

				if (!should_update_pipe_for_plane(context, pipe_ctx, plane_state))
					continue;
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)

				/*program triple buffer after lock based on flip type*/
				if (dc->hwss.program_triplebuffer != NULL && dc->debug.enable_tri_buf) {
					/*only enable triplebuffer for  fast_update*/
					dc->hwss.program_triplebuffer(
						dc, pipe_ctx, pipe_ctx->plane_state->triplebuffer_flips);
				}
#endif
				if (pipe_ctx->plane_state->update_flags.bits.addr_update)
					dc->hwss.update_plane_addr(dc, pipe_ctx);
			}
		}

	}

	if (should_lock_all_pipes && dc->hwss.interdependent_update_lock)
		dc->hwss.interdependent_update_lock(dc, context, false);
	else
		dc->hwss.pipe_control_lock(dc, top_pipe_to_program, false);

	if ((update_type != UPDATE_TYPE_FAST) && stream->update_flags.bits.dsc_changed)
		if (top_pipe_to_program->stream_res.tg->funcs->lock_doublebuffer_enable) {
			top_pipe_to_program->stream_res.tg->funcs->wait_for_state(
					top_pipe_to_program->stream_res.tg,
					CRTC_STATE_VACTIVE);
			top_pipe_to_program->stream_res.tg->funcs->wait_for_state(
					top_pipe_to_program->stream_res.tg,
					CRTC_STATE_VBLANK);
			top_pipe_to_program->stream_res.tg->funcs->wait_for_state(
					top_pipe_to_program->stream_res.tg,
					CRTC_STATE_VACTIVE);

			if (stream && should_use_dmub_lock(stream->link)) {
				union dmub_hw_lock_flags hw_locks = { 0 };
				struct dmub_hw_lock_inst_flags inst_flags = { 0 };

				hw_locks.bits.lock_dig = 1;
				inst_flags.dig_inst = top_pipe_to_program->stream_res.tg->inst;

				dmub_hw_lock_mgr_cmd(dc->ctx->dmub_srv,
							false,
							&hw_locks,
							&inst_flags);
			} else
				top_pipe_to_program->stream_res.tg->funcs->lock_doublebuffer_disable(
					top_pipe_to_program->stream_res.tg);
		}

	if (update_type != UPDATE_TYPE_FAST)
		dc->hwss.post_unlock_program_front_end(dc, context);

	// Fire manual trigger only when bottom plane is flipped
	for (j = 0; j < dc->res_pool->pipe_count; j++) {
		struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[j];

		if (!pipe_ctx->plane_state)
			continue;

		if (pipe_ctx->bottom_pipe || pipe_ctx->next_odm_pipe ||
				!pipe_ctx->stream || !should_update_pipe_for_stream(context, pipe_ctx, stream) ||
				!pipe_ctx->plane_state->update_flags.bits.addr_update ||
				pipe_ctx->plane_state->skip_manual_trigger)
			continue;

		if (pipe_ctx->stream_res.tg->funcs->program_manual_trigger)
			pipe_ctx->stream_res.tg->funcs->program_manual_trigger(pipe_ctx->stream_res.tg);
	}
}

void dc_commit_updates_for_stream(struct dc *dc,
		struct dc_surface_update *srf_updates,
		int surface_count,
		struct dc_stream_state *stream,
		struct dc_stream_update *stream_update,
		struct dc_state *state)
{
	const struct dc_stream_status *stream_status;
	enum surface_update_type update_type;
	struct dc_state *context;
	struct dc_context *dc_ctx = dc->ctx;
	int i, j;

	stream_status = dc_stream_get_status(stream);
	context = dc->current_state;

	update_type = dc_check_update_surfaces_for_stream(
				dc, srf_updates, surface_count, stream_update, stream_status);

	if (update_type >= update_surface_trace_level)
		update_surface_trace(dc, srf_updates, surface_count);


	if (update_type >= UPDATE_TYPE_FULL) {

		/* initialize scratch memory for building context */
		context = dc_create_state(dc);
		if (context == NULL) {
			DC_ERROR("Failed to allocate new validate context!\n");
			return;
		}

		dc_resource_state_copy_construct(state, context);

		for (i = 0; i < dc->res_pool->pipe_count; i++) {
			struct pipe_ctx *new_pipe = &context->res_ctx.pipe_ctx[i];
			struct pipe_ctx *old_pipe = &dc->current_state->res_ctx.pipe_ctx[i];

			if (new_pipe->plane_state && new_pipe->plane_state != old_pipe->plane_state)
				new_pipe->plane_state->force_full_update = true;
		}
	} else if (update_type == UPDATE_TYPE_FAST && dc_ctx->dce_version >= DCE_VERSION_MAX) {
		/*
		 * Previous frame finished and HW is ready for optimization.
		 *
		 * Only relevant for DCN behavior where we can guarantee the optimization
		 * is safe to apply - retain the legacy behavior for DCE.
		 */
		dc_post_update_surfaces_to_stream(dc);
	}


	for (i = 0; i < surface_count; i++) {
		struct dc_plane_state *surface = srf_updates[i].surface;

		copy_surface_update_to_plane(surface, &srf_updates[i]);

		if (update_type >= UPDATE_TYPE_MED) {
			for (j = 0; j < dc->res_pool->pipe_count; j++) {
				struct pipe_ctx *pipe_ctx =
					&context->res_ctx.pipe_ctx[j];

				if (pipe_ctx->plane_state != surface)
					continue;

				resource_build_scaling_params(pipe_ctx);
			}
		}
	}

	copy_stream_update_to_stream(dc, context, stream, stream_update);

	if (update_type >= UPDATE_TYPE_FULL) {
		if (!dc->res_pool->funcs->validate_bandwidth(dc, context, false)) {
			DC_ERROR("Mode validation failed for stream update!\n");
			dc_release_state(context);
			return;
		}
	}

	TRACE_DC_PIPE_STATE(pipe_ctx, i, MAX_PIPES);

	commit_planes_for_stream(
				dc,
				srf_updates,
				surface_count,
				stream,
				stream_update,
				update_type,
				context);
	/*update current_State*/
	if (dc->current_state != context) {

		struct dc_state *old = dc->current_state;

		dc->current_state = context;
		dc_release_state(old);

		for (i = 0; i < dc->res_pool->pipe_count; i++) {
			struct pipe_ctx *pipe_ctx = &context->res_ctx.pipe_ctx[i];

			if (pipe_ctx->plane_state && pipe_ctx->stream == stream)
				pipe_ctx->plane_state->force_full_update = false;
		}
	}

	/* Legacy optimization path for DCE. */
	if (update_type >= UPDATE_TYPE_FULL && dc_ctx->dce_version < DCE_VERSION_MAX) {
		dc_post_update_surfaces_to_stream(dc);
		TRACE_DCE_CLOCK_STATE(&context->bw_ctx.bw.dce);
	}

	return;

}

uint8_t dc_get_current_stream_count(struct dc *dc)
{
	return dc->current_state->stream_count;
}

struct dc_stream_state *dc_get_stream_at_index(struct dc *dc, uint8_t i)
{
	if (i < dc->current_state->stream_count)
		return dc->current_state->streams[i];
	return NULL;
}

struct dc_stream_state *dc_stream_find_from_link(const struct dc_link *link)
{
	uint8_t i;
	struct dc_context *ctx = link->ctx;

	for (i = 0; i < ctx->dc->current_state->stream_count; i++) {
		if (ctx->dc->current_state->streams[i]->link == link)
			return ctx->dc->current_state->streams[i];
	}

	return NULL;
}

#ifndef HAVE_DRM_NONBLOCKING_COMMIT_SUPPORT
void dc_flip_plane_addrs(
		struct dc *dc,
		struct dc_plane_state *const plane_states[],
		struct dc_flip_addrs flip_addrs[],
		uint32_t count)
{
	int i, j;

	for (i = 0; i < count; i++) {
		struct dc_plane_state *plane_state = plane_states[i];

		plane_state->address = flip_addrs[i].address;
		plane_state->flip_immediate = flip_addrs[i].flip_immediate;

		for (j = 0; j < dc->res_pool->pipe_count; j++) {
			struct pipe_ctx *pipe_ctx = &dc->current_state->res_ctx.pipe_ctx[j];

			if (pipe_ctx->plane_state != plane_state)
				continue;

			dc->hwss.update_plane_addr(dc, pipe_ctx);
		}
	}
}
#endif

enum dc_irq_source dc_interrupt_to_irq_source(
		struct dc *dc,
		uint32_t src_id,
		uint32_t ext_id)
{
	return dal_irq_service_to_irq_source(dc->res_pool->irqs, src_id, ext_id);
}

/*
 * dc_interrupt_set() - Enable/disable an AMD hw interrupt source
 */
bool dc_interrupt_set(struct dc *dc, enum dc_irq_source src, bool enable)
{

	if (dc == NULL)
		return false;

	return dal_irq_service_set(dc->res_pool->irqs, src, enable);
}

void dc_interrupt_ack(struct dc *dc, enum dc_irq_source src)
{
	dal_irq_service_ack(dc->res_pool->irqs, src);
}

void dc_power_down_on_boot(struct dc *dc)
{
	if (dc->ctx->dce_environment != DCE_ENV_VIRTUAL_HW &&
			dc->hwss.power_down_on_boot)
		dc->hwss.power_down_on_boot(dc);
}

void dc_set_power_state(
	struct dc *dc,
	enum dc_acpi_cm_power_state power_state)
{
	struct kref refcount;
	struct display_mode_lib *dml;

	if (!dc->current_state)
		return;

	switch (power_state) {
	case DC_ACPI_CM_POWER_STATE_D0:
		dc_resource_state_construct(dc, dc->current_state);

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)
		dc_z10_restore(dc);
#endif
		if (dc->ctx->dmub_srv)
			dc_dmub_srv_wait_phy_init(dc->ctx->dmub_srv);

		dc->hwss.init_hw(dc);

#ifdef CONFIG_DRM_AMD_DC_DCN2_x
		if (dc->hwss.init_sys_ctx != NULL &&
			dc->vm_pa_config.valid) {
			dc->hwss.init_sys_ctx(dc->hwseq, dc, &dc->vm_pa_config);
		}
#endif

		break;
	default:
		ASSERT(dc->current_state->stream_count == 0);
		/* Zero out the current context so that on resume we start with
		 * clean state, and dc hw programming optimizations will not
		 * cause any trouble.
		 */
		dml = kzalloc(sizeof(struct display_mode_lib),
				GFP_KERNEL);

		ASSERT(dml);
		if (!dml)
			return;

		/* Preserve refcount */
		refcount = dc->current_state->refcount;
		/* Preserve display mode lib */
		memcpy(dml, &dc->current_state->bw_ctx.dml, sizeof(struct display_mode_lib));

		dc_resource_state_destruct(dc->current_state);
		memset(dc->current_state, 0,
				sizeof(*dc->current_state));

		dc->current_state->refcount = refcount;
		dc->current_state->bw_ctx.dml = *dml;

		kfree(dml);

		break;
	}
}

void dc_resume(struct dc *dc)
{
	uint32_t i;

	for (i = 0; i < dc->link_count; i++)
		core_link_resume(dc->links[i]);
}

bool dc_is_dmcu_initialized(struct dc *dc)
{
	struct dmcu *dmcu = dc->res_pool->dmcu;

	if (dmcu)
		return dmcu->funcs->is_dmcu_initialized(dmcu);
	return false;
}

bool dc_submit_i2c(
		struct dc *dc,
		uint32_t link_index,
		struct i2c_command *cmd)
{

	struct dc_link *link = dc->links[link_index];
	struct ddc_service *ddc = link->ddc;
	return dce_i2c_submit_command(
		dc->res_pool,
		ddc->ddc_pin,
		cmd);
}

bool dc_submit_i2c_oem(
		struct dc *dc,
		struct i2c_command *cmd)
{
	struct ddc_service *ddc = dc->res_pool->oem_device;
	return dce_i2c_submit_command(
		dc->res_pool,
		ddc->ddc_pin,
		cmd);
}

static bool link_add_remote_sink_helper(struct dc_link *dc_link, struct dc_sink *sink)
{
	if (dc_link->sink_count >= MAX_SINKS_PER_LINK) {
		BREAK_TO_DEBUGGER();
		return false;
	}

	dc_sink_retain(sink);

	dc_link->remote_sinks[dc_link->sink_count] = sink;
	dc_link->sink_count++;

	return true;
}

/*
 * dc_link_add_remote_sink() - Create a sink and attach it to an existing link
 *
 * EDID length is in bytes
 */
struct dc_sink *dc_link_add_remote_sink(
		struct dc_link *link,
		const uint8_t *edid,
		int len,
		struct dc_sink_init_data *init_data)
{
	struct dc_sink *dc_sink;
	enum dc_edid_status edid_status;

	if (len > DC_MAX_EDID_BUFFER_SIZE) {
		dm_error("Max EDID buffer size breached!\n");
		return NULL;
	}

	if (!init_data) {
		BREAK_TO_DEBUGGER();
		return NULL;
	}

	if (!init_data->link) {
		BREAK_TO_DEBUGGER();
		return NULL;
	}

	dc_sink = dc_sink_create(init_data);

	if (!dc_sink)
		return NULL;

	memmove(dc_sink->dc_edid.raw_edid, edid, len);
	dc_sink->dc_edid.length = len;

	if (!link_add_remote_sink_helper(
			link,
			dc_sink))
		goto fail_add_sink;

	edid_status = dm_helpers_parse_edid_caps(
			link,
			&dc_sink->dc_edid,
			&dc_sink->edid_caps);

	/*
	 * Treat device as no EDID device if EDID
	 * parsing fails
	 */
	if (edid_status != EDID_OK) {
		dc_sink->dc_edid.length = 0;
		dm_error("Bad EDID, status%d!\n", edid_status);
	}

	return dc_sink;

fail_add_sink:
	dc_sink_release(dc_sink);
	return NULL;
}

/*
 * dc_link_remove_remote_sink() - Remove a remote sink from a dc_link
 *
 * Note that this just removes the struct dc_sink - it doesn't
 * program hardware or alter other members of dc_link
 */
void dc_link_remove_remote_sink(struct dc_link *link, struct dc_sink *sink)
{
	int i;

	if (!link->sink_count) {
		BREAK_TO_DEBUGGER();
		return;
	}

	for (i = 0; i < link->sink_count; i++) {
		if (link->remote_sinks[i] == sink) {
			dc_sink_release(sink);
			link->remote_sinks[i] = NULL;

			/* shrink array to remove empty place */
			while (i < link->sink_count - 1) {
				link->remote_sinks[i] = link->remote_sinks[i+1];
				i++;
			}
			link->remote_sinks[i] = NULL;
			link->sink_count--;
			return;
		}
	}
}

void get_clock_requirements_for_state(struct dc_state *state, struct AsicStateEx *info)
{
	info->displayClock				= (unsigned int)state->bw_ctx.bw.dcn.clk.dispclk_khz;
	info->engineClock				= (unsigned int)state->bw_ctx.bw.dcn.clk.dcfclk_khz;
	info->memoryClock				= (unsigned int)state->bw_ctx.bw.dcn.clk.dramclk_khz;
	info->maxSupportedDppClock		= (unsigned int)state->bw_ctx.bw.dcn.clk.max_supported_dppclk_khz;
	info->dppClock					= (unsigned int)state->bw_ctx.bw.dcn.clk.dppclk_khz;
	info->socClock					= (unsigned int)state->bw_ctx.bw.dcn.clk.socclk_khz;
	info->dcfClockDeepSleep			= (unsigned int)state->bw_ctx.bw.dcn.clk.dcfclk_deep_sleep_khz;
	info->fClock					= (unsigned int)state->bw_ctx.bw.dcn.clk.fclk_khz;
	info->phyClock					= (unsigned int)state->bw_ctx.bw.dcn.clk.phyclk_khz;
}
enum dc_status dc_set_clock(struct dc *dc, enum dc_clock_type clock_type, uint32_t clk_khz, uint32_t stepping)
{
	if (dc->hwss.set_clock)
		return dc->hwss.set_clock(dc, clock_type, clk_khz, stepping);
	return DC_ERROR_UNEXPECTED;
}
void dc_get_clock(struct dc *dc, enum dc_clock_type clock_type, struct dc_clock_config *clock_cfg)
{
	if (dc->hwss.get_clock)
		dc->hwss.get_clock(dc, clock_type, clock_cfg);
}

/* enable/disable eDP PSR without specify stream for eDP */
bool dc_set_psr_allow_active(struct dc *dc, bool enable)
{
	int i;
	bool allow_active;

	for (i = 0; i < dc->current_state->stream_count ; i++) {
		struct dc_link *link;
		struct dc_stream_state *stream = dc->current_state->streams[i];

		link = stream->link;
		if (!link)
			continue;

		if (link->psr_settings.psr_feature_enabled) {
			if (enable && !link->psr_settings.psr_allow_active) {
				allow_active = true;
				if (!dc_link_set_psr_allow_active(link, &allow_active, false, false, NULL))
					return false;
			} else if (!enable && link->psr_settings.psr_allow_active) {
				allow_active = false;
				if (!dc_link_set_psr_allow_active(link, &allow_active, true, false, NULL))
					return false;
			}
		}
	}

	return true;
}

#if defined(CONFIG_DRM_AMD_DC_DCN3_x)

void dc_allow_idle_optimizations(struct dc *dc, bool allow)
{
	if (dc->debug.disable_idle_power_optimizations)
		return;

	if (dc->clk_mgr != NULL && dc->clk_mgr->funcs->is_smu_present)
		if (!dc->clk_mgr->funcs->is_smu_present(dc->clk_mgr))
			return;

	if (allow == dc->idle_optimizations_allowed)
		return;

	if (dc->hwss.apply_idle_power_optimizations && dc->hwss.apply_idle_power_optimizations(dc, allow))
		dc->idle_optimizations_allowed = allow;
}

/*
 * blank all streams, and set min and max memory clock to
 * lowest and highest DPM level, respectively
 */
void dc_unlock_memory_clock_frequency(struct dc *dc)
{
	unsigned int i;

	for (i = 0; i < MAX_PIPES; i++)
		if (dc->current_state->res_ctx.pipe_ctx[i].plane_state)
			core_link_disable_stream(&dc->current_state->res_ctx.pipe_ctx[i]);

	dc->clk_mgr->funcs->set_hard_min_memclk(dc->clk_mgr, false);
	dc->clk_mgr->funcs->set_hard_max_memclk(dc->clk_mgr);
}

/*
 * set min memory clock to the min required for current mode,
 * max to maxDPM, and unblank streams
 */
void dc_lock_memory_clock_frequency(struct dc *dc)
{
	unsigned int i;

	dc->clk_mgr->funcs->get_memclk_states_from_smu(dc->clk_mgr);
	dc->clk_mgr->funcs->set_hard_min_memclk(dc->clk_mgr, true);
	dc->clk_mgr->funcs->set_hard_max_memclk(dc->clk_mgr);

	for (i = 0; i < MAX_PIPES; i++)
		if (dc->current_state->res_ctx.pipe_ctx[i].plane_state)
			core_link_enable_stream(dc->current_state, &dc->current_state->res_ctx.pipe_ctx[i]);
}

static void blank_and_force_memclk(struct dc *dc, bool apply, unsigned int memclk_mhz)
{
	struct dc_state *context = dc->current_state;
	struct hubp *hubp;
	struct pipe_ctx *pipe;
	int i;

	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		pipe = &context->res_ctx.pipe_ctx[i];

		if (pipe->stream != NULL) {
			dc->hwss.disable_pixel_data(dc, pipe, true);

			// wait for double buffer
			pipe->stream_res.tg->funcs->wait_for_state(pipe->stream_res.tg, CRTC_STATE_VACTIVE);
			pipe->stream_res.tg->funcs->wait_for_state(pipe->stream_res.tg, CRTC_STATE_VBLANK);
			pipe->stream_res.tg->funcs->wait_for_state(pipe->stream_res.tg, CRTC_STATE_VACTIVE);

			hubp = pipe->plane_res.hubp;
			hubp->funcs->set_blank_regs(hubp, true);
		}
	}

	dc->clk_mgr->funcs->set_max_memclk(dc->clk_mgr, memclk_mhz);
	dc->clk_mgr->funcs->set_min_memclk(dc->clk_mgr, memclk_mhz);

	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		pipe = &context->res_ctx.pipe_ctx[i];

		if (pipe->stream != NULL) {
			dc->hwss.disable_pixel_data(dc, pipe, false);

			hubp = pipe->plane_res.hubp;
			hubp->funcs->set_blank_regs(hubp, false);
		}
	}
}


/**
 * dc_enable_dcmode_clk_limit() - lower clocks in dc (battery) mode
 * @dc: pointer to dc of the dm calling this
 * @enable: True = transition to DC mode, false = transition back to AC mode
 *
 * Some SoCs define additional clock limits when in DC mode, DM should
 * invoke this function when the platform undergoes a power source transition
 * so DC can apply/unapply the limit. This interface may be disruptive to
 * the onscreen content.
 *
 * Context: Triggered by OS through DM interface, or manually by escape calls.
 * Need to hold a dclock when doing so.
 *
 * Return: none (void function)
 *
 */
void dc_enable_dcmode_clk_limit(struct dc *dc, bool enable)
{
	uint32_t hw_internal_rev = dc->ctx->asic_id.hw_internal_rev;
	unsigned int softMax, maxDPM, funcMin;
	bool p_state_change_support;

	if (!ASICREV_IS_BEIGE_GOBY_P(hw_internal_rev))
		return;

	softMax = dc->clk_mgr->bw_params->dc_mode_softmax_memclk;
	maxDPM = dc->clk_mgr->bw_params->clk_table.entries[dc->clk_mgr->bw_params->clk_table.num_entries - 1].memclk_mhz;
	funcMin = (dc->clk_mgr->clks.dramclk_khz + 999) / 1000;
	p_state_change_support = dc->clk_mgr->clks.p_state_change_support;

	if (enable && !dc->clk_mgr->dc_mode_softmax_enabled) {
		if (p_state_change_support) {
			if (funcMin <= softMax)
				dc->clk_mgr->funcs->set_max_memclk(dc->clk_mgr, softMax);
			// else: No-Op
		} else {
			if (funcMin <= softMax)
				blank_and_force_memclk(dc, true, softMax);
			// else: No-Op
		}
	} else if (!enable && dc->clk_mgr->dc_mode_softmax_enabled) {
		if (p_state_change_support) {
			if (funcMin <= softMax)
				dc->clk_mgr->funcs->set_max_memclk(dc->clk_mgr, maxDPM);
			// else: No-Op
		} else {
			if (funcMin <= softMax)
				blank_and_force_memclk(dc, true, maxDPM);
			// else: No-Op
		}
	}
	dc->clk_mgr->dc_mode_softmax_enabled = enable;
}
bool dc_is_plane_eligible_for_idle_optimizations(struct dc *dc, struct dc_plane_state *plane,
		struct dc_cursor_attributes *cursor_attr)
{
	if (dc->hwss.does_plane_fit_in_mall && dc->hwss.does_plane_fit_in_mall(dc, plane, cursor_attr))
		return true;
	return false;
}

/* cleanup on driver unload */
void dc_hardware_release(struct dc *dc)
{
	if (dc->hwss.hardware_release)
		dc->hwss.hardware_release(dc);
}
#endif

/*
 *****************************************************************************
 * Function: dc_is_dmub_outbox_supported -
 * 
 * @brief 
 *      Checks whether DMUB FW supports outbox notifications, if supported
 *		DM should register outbox interrupt prior to actually enabling interrupts
 *		via dc_enable_dmub_outbox
 *
 *  @param
 *		[in] dc: dc structure
 *
 *  @return
 *		True if DMUB FW supports outbox notifications, False otherwise
 *****************************************************************************
 */
bool dc_is_dmub_outbox_supported(struct dc *dc)
{
#if defined(CONFIG_DRM_AMD_DC_DCN2_x)
	/* YELLOW_CARP B0 USB4 DPIA needs dmub notifications for interrupts */
	if (dc->ctx->asic_id.chip_family == FAMILY_YELLOW_CARP &&
	    dc->ctx->asic_id.hw_internal_rev == YELLOW_CARP_B0 &&
	    !dc->debug.dpia_debug.bits.disable_dpia)
		return true;
#endif
	/* dmub aux needs dmub notifications to be enabled */
	return dc->debug.enable_dmub_aux_for_legacy_ddc;
}

/**
 * dc_process_dmub_aux_transfer_async - Submits aux command to dmub via inbox message
 *  Function: dc_enable_dmub_notifications
 *
 *  @brief
 *		Calls dc_is_dmub_outbox_supported to check if dmub fw supports outbox
 *		notifications. All DMs shall switch to dc_is_dmub_outbox_supported.
 *		This API shall be removed after switching.
 *
 *  @param
 *		[in] dc: dc structure
 *
 *  @return
 *		True if DMUB FW supports outbox notifications, False otherwise
 *****************************************************************************
 */
bool dc_enable_dmub_notifications(struct dc *dc)
{
	return dc_is_dmub_outbox_supported(dc);
}

/**
 *****************************************************************************
 *  Function: dc_enable_dmub_outbox
 *
 *  @brief
 *		Enables DMUB unsolicited notifications to x86 via outbox
 *
 *  @param
 *		[in] dc: dc structure
 *
 *  @return
 *		None
 *****************************************************************************
 */
void dc_enable_dmub_outbox(struct dc *dc)
{
	struct dc_context *dc_ctx = dc->ctx;

	dmub_enable_outbox_notification(dc_ctx->dmub_srv);
}

/**
 *****************************************************************************
 *                                      Sets port index appropriately for legacy DDC
 * @dc: dc structure
 * @link_index: link index
 * @payload: aux payload
 *
 * Returns: True if successful, False if failure
 */
bool dc_process_dmub_aux_transfer_async(struct dc *dc,
				uint32_t link_index,
				struct aux_payload *payload)
{
	uint8_t action;
	union dmub_rb_cmd cmd = {0};
	struct dc_dmub_srv *dmub_srv = dc->ctx->dmub_srv;

	ASSERT(payload->length <= 16);

	cmd.dp_aux_access.header.type = DMUB_CMD__DP_AUX_ACCESS;
	cmd.dp_aux_access.header.payload_bytes = 0;
	/* For dpia, ddc_pin is set to NULL */
	if (!dc->links[link_index]->ddc->ddc_pin)
		cmd.dp_aux_access.aux_control.type = AUX_CHANNEL_DPIA;
	else
		cmd.dp_aux_access.aux_control.type = AUX_CHANNEL_LEGACY_DDC;

	cmd.dp_aux_access.aux_control.instance = dc->links[link_index]->ddc_hw_inst;
	cmd.dp_aux_access.aux_control.sw_crc_enabled = 0;
	cmd.dp_aux_access.aux_control.timeout = 0;
	cmd.dp_aux_access.aux_control.dpaux.address = payload->address;
	cmd.dp_aux_access.aux_control.dpaux.is_i2c_over_aux = payload->i2c_over_aux;
	cmd.dp_aux_access.aux_control.dpaux.length = payload->length;

	/* set aux action */
	if (payload->i2c_over_aux) {
		if (payload->write) {
			if (payload->mot)
				action = DP_AUX_REQ_ACTION_I2C_WRITE_MOT;
			else
				action = DP_AUX_REQ_ACTION_I2C_WRITE;
		} else {
			if (payload->mot)
				action = DP_AUX_REQ_ACTION_I2C_READ_MOT;
			else
				action = DP_AUX_REQ_ACTION_I2C_READ;
			}
	} else {
		if (payload->write)
			action = DP_AUX_REQ_ACTION_DPCD_WRITE;
		else
			action = DP_AUX_REQ_ACTION_DPCD_READ;
	}

	cmd.dp_aux_access.aux_control.dpaux.action = action;

	if (payload->length && payload->write) {
		memcpy(cmd.dp_aux_access.aux_control.dpaux.data,
			payload->data,
			payload->length
			);
	}

	dc_dmub_srv_cmd_queue(dmub_srv, &cmd);
	dc_dmub_srv_cmd_execute(dmub_srv);
	dc_dmub_srv_wait_idle(dmub_srv);

	return true;
}

uint8_t get_link_index_from_dpia_port_index(const struct dc *dc,
					    uint8_t dpia_port_index)
{
	uint8_t index, link_index = 0xFF;

	for (index = 0; index < dc->link_count; index++) {
		/* ddc_hw_inst has dpia port index for dpia links
		 * and ddc instance for legacy links
		 */
		if (!dc->links[index]->ddc->ddc_pin) {
			if (dc->links[index]->ddc_hw_inst == dpia_port_index) {
				link_index = index;
				break;
			}
		}
	}
	ASSERT(link_index != 0xFF);
	return link_index;
}

/**
 *****************************************************************************
 *  Function: dc_process_dmub_set_config_async
 *
 *  @brief
 *		Submits set_config command to dmub via inbox message
 *
 *  @param
 *		[in] dc: dc structure
 *		[in] link_index: link index
 *		[in] payload: aux payload
 *		[out] notify: set_config immediate reply
 *
 *  @return
 *		True if successful, False if failure
 *****************************************************************************
 */
bool dc_process_dmub_set_config_async(struct dc *dc,
				uint32_t link_index,
				struct set_config_cmd_payload *payload,
				struct dmub_notification *notify)
{
	union dmub_rb_cmd cmd = {0};
	struct dc_dmub_srv *dmub_srv = dc->ctx->dmub_srv;
	bool is_cmd_complete = true;

	/* prepare SET_CONFIG command */
	cmd.set_config_access.header.type = DMUB_CMD__DPIA;
	cmd.set_config_access.header.sub_type = DMUB_CMD__DPIA_SET_CONFIG_ACCESS;

	cmd.set_config_access.set_config_control.instance = dc->links[link_index]->ddc_hw_inst;
	cmd.set_config_access.set_config_control.cmd_pkt.msg_type = payload->msg_type;
	cmd.set_config_access.set_config_control.cmd_pkt.msg_data = payload->msg_data;

	if (!dc_dmub_srv_cmd_with_reply_data(dmub_srv, &cmd)) {
		/* command is not processed by dmub */
		notify->sc_status = SET_CONFIG_UNKNOWN_ERROR;
		return is_cmd_complete;
	}

	/* command processed by dmub, if ret_status is 1, it is completed instantly */
	if (cmd.set_config_access.header.ret_status == 1)
		notify->sc_status = cmd.set_config_access.set_config_control.immed_status;
	else
		/* cmd pending, will receive notification via outbox */
		is_cmd_complete = false;

	return is_cmd_complete;
}

/**
 *****************************************************************************
 *  Function: dc_process_dmub_set_mst_slots
 *
 *  @brief
 *		Submits mst slot allocation command to dmub via inbox message
 *
 *  @param
 *		[in] dc: dc structure
 *		[in] link_index: link index
 *		[in] mst_alloc_slots: mst slots to be allotted
 *		[out] mst_slots_in_use: mst slots in use returned in failure case
 *
 *	@return
 *		DC_OK if successful, DC_ERROR if failure
 *****************************************************************************
 */
enum dc_status dc_process_dmub_set_mst_slots(const struct dc *dc,
				uint32_t link_index,
				uint8_t mst_alloc_slots,
				uint8_t *mst_slots_in_use)
{
	union dmub_rb_cmd cmd = {0};
	struct dc_dmub_srv *dmub_srv = dc->ctx->dmub_srv;

	/* prepare MST_ALLOC_SLOTS command */
	cmd.set_mst_alloc_slots.header.type = DMUB_CMD__DPIA;
	cmd.set_mst_alloc_slots.header.sub_type = DMUB_CMD__DPIA_MST_ALLOC_SLOTS;

	cmd.set_mst_alloc_slots.mst_slots_control.instance = dc->links[link_index]->ddc_hw_inst;
	cmd.set_mst_alloc_slots.mst_slots_control.mst_alloc_slots = mst_alloc_slots;

	if (!dc_dmub_srv_cmd_with_reply_data(dmub_srv, &cmd))
		/* command is not processed by dmub */
		return DC_ERROR_UNEXPECTED;

	/* command processed by dmub, if ret_status is 1 */
	if (cmd.set_config_access.header.ret_status != 1)
		/* command processing error */
		return DC_ERROR_UNEXPECTED;

	/* command processed and we have a status of 2, mst not enabled in dpia */
	if (cmd.set_mst_alloc_slots.mst_slots_control.immed_status == 2)
		return DC_FAIL_UNSUPPORTED_1;

	/* previously configured mst alloc and used slots did not match */
	if (cmd.set_mst_alloc_slots.mst_slots_control.immed_status == 3) {
		*mst_slots_in_use = cmd.set_mst_alloc_slots.mst_slots_control.mst_slots_in_use;
		return DC_NOT_SUPPORTED;
	}

	return DC_OK;
}

/**
 * dc_disable_accelerated_mode - disable accelerated mode
 * @dc: dc structure
 */
void dc_disable_accelerated_mode(struct dc *dc)
{
	bios_set_scratch_acc_mode_change(dc->ctx->dc_bios, 0);
}


/**
 *****************************************************************************
 *  dc_notify_vsync_int_state() - notifies vsync enable/disable state
 *  @dc: dc structure
 *	@stream: stream where vsync int state changed
 *	@enable: whether vsync is enabled or disabled
 *
 *  Called when vsync is enabled/disabled
 *	Will notify DMUB to start/stop ABM interrupts after steady state is reached
 *
 *****************************************************************************
 */
void dc_notify_vsync_int_state(struct dc *dc, struct dc_stream_state *stream, bool enable)
{
	int i;
	int edp_num;
	struct pipe_ctx *pipe = NULL;
	struct dc_link *link = stream->sink->link;
	struct dc_link *edp_links[MAX_NUM_EDP];


	if (link->psr_settings.psr_feature_enabled)
		return;

	/*find primary pipe associated with stream*/
	for (i = 0; i < MAX_PIPES; i++) {
		pipe = &dc->current_state->res_ctx.pipe_ctx[i];

		if (pipe->stream == stream && pipe->stream_res.tg)
			break;
	}

	if (i == MAX_PIPES) {
		ASSERT(0);
		return;
	}

	get_edp_links(dc, edp_links, &edp_num);

	/* Determine panel inst */
	for (i = 0; i < edp_num; i++) {
		if (edp_links[i] == link)
			break;
	}

	if (i == edp_num) {
		return;
	}

	if (pipe->stream_res.abm && pipe->stream_res.abm->funcs->set_abm_pause)
		pipe->stream_res.abm->funcs->set_abm_pause(pipe->stream_res.abm, !enable, i, pipe->stream_res.tg->inst);
}
