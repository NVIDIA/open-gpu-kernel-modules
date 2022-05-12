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

#ifndef _DMUB_DC_SRV_H_
#define _DMUB_DC_SRV_H_

#include "os_types.h"
#include "dmub/dmub_srv.h"

struct dmub_srv;
struct dc;

struct dc_reg_helper_state {
	bool gather_in_progress;
	uint32_t same_addr_count;
	bool should_burst_write;
	union dmub_rb_cmd cmd_data;
	unsigned int reg_seq_count;
};

struct dc_dmub_srv {
	struct dmub_srv *dmub;
	struct dc_reg_helper_state reg_helper_offload;

	struct dc_context *ctx;
	void *dm;
};

void dc_dmub_srv_cmd_queue(struct dc_dmub_srv *dc_dmub_srv,
			   union dmub_rb_cmd *cmd);

void dc_dmub_srv_cmd_execute(struct dc_dmub_srv *dc_dmub_srv);

void dc_dmub_srv_wait_idle(struct dc_dmub_srv *dc_dmub_srv);

void dc_dmub_srv_wait_phy_init(struct dc_dmub_srv *dc_dmub_srv);

bool dc_dmub_srv_cmd_with_reply_data(struct dc_dmub_srv *dc_dmub_srv, union dmub_rb_cmd *cmd);

bool dc_dmub_srv_notify_stream_mask(struct dc_dmub_srv *dc_dmub_srv,
				    unsigned int stream_mask);

bool dc_dmub_srv_is_restore_required(struct dc_dmub_srv *dc_dmub_srv);

bool dc_dmub_srv_get_dmub_outbox0_msg(const struct dc *dc, struct dmcub_trace_buf_entry *entry);

void dc_dmub_trace_event_control(struct dc *dc, bool enable);

void dc_dmub_srv_clear_inbox0_ack(struct dc_dmub_srv *dmub_srv);
void dc_dmub_srv_wait_for_inbox0_ack(struct dc_dmub_srv *dmub_srv);
void dc_dmub_srv_send_inbox0_cmd(struct dc_dmub_srv *dmub_srv, union dmub_inbox0_data_register data);

bool dc_dmub_srv_get_diagnostic_data(struct dc_dmub_srv *dc_dmub_srv, struct dmub_diagnostic_data *dmub_oca);

void dc_dmub_srv_log_diagnostic_data(struct dc_dmub_srv *dc_dmub_srv);

#endif /* _DMUB_DC_SRV_H_ */
