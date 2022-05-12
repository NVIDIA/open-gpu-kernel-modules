/*
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2008 - 2011 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * The full GNU General Public License is included in this distribution
 * in the file called LICENSE.GPL.
 *
 * BSD LICENSE
 *
 * Copyright(c) 2008 - 2011 Intel Corporation. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SCIC_SDS_REMOTE_NODE_CONTEXT_H_
#define _SCIC_SDS_REMOTE_NODE_CONTEXT_H_

/**
 * This file contains the structures, constants, and prototypes associated with
 *    the remote node context in the silicon.  It exists to model and manage
 *    the remote node context in the silicon.
 *
 *
 */

#include "isci.h"

/**
 *
 *
 * This constant represents an invalid remote device id, it is used to program
 * the STPDARNI register so the driver knows when it has received a SIGNATURE
 * FIS from the SCU.
 */
#define SCIC_SDS_REMOTE_NODE_CONTEXT_INVALID_INDEX    0x0FFF

enum sci_remote_node_suspension_reasons {
	SCI_HW_SUSPEND,
	SCI_SW_SUSPEND_NORMAL,
	SCI_SW_SUSPEND_LINKHANG_DETECT
};
#define SCI_SOFTWARE_SUSPEND_CMD SCU_CONTEXT_COMMAND_POST_RNC_SUSPEND_TX_RX
#define SCI_SOFTWARE_SUSPEND_EXPECTED_EVENT SCU_EVENT_TL_RNC_SUSPEND_TX_RX

struct isci_request;
struct isci_remote_device;
struct sci_remote_node_context;

typedef void (*scics_sds_remote_node_context_callback)(void *);

/**
 * enum sci_remote_node_context_states
 * @SCI_RNC_INITIAL initial state for a remote node context.  On a resume
 * request the remote node context will transition to the posting state.
 *
 * @SCI_RNC_POSTING: transition state that posts the RNi to the hardware. Once
 * the RNC is posted the remote node context will be made ready.
 *
 * @SCI_RNC_INVALIDATING: transition state that will post an RNC invalidate to
 * the hardware.  Once the invalidate is complete the remote node context will
 * transition to the posting state.
 *
 * @SCI_RNC_RESUMING: transition state that will post an RNC resume to the
 * hardare.  Once the event notification of resume complete is received the
 * remote node context will transition to the ready state.
 *
 * @SCI_RNC_READY: state that the remote node context must be in to accept io
 * request operations.
 *
 * @SCI_RNC_TX_SUSPENDED: state that the remote node context transitions to when
 * it gets a TX suspend notification from the hardware.
 *
 * @SCI_RNC_TX_RX_SUSPENDED: state that the remote node context transitions to
 * when it gets a TX RX suspend notification from the hardware.
 *
 * @SCI_RNC_AWAIT_SUSPENSION: wait state for the remote node context that waits
 * for a suspend notification from the hardware.  This state is entered when
 * either there is a request to supend the remote node context or when there is
 * a TC completion where the remote node will be suspended by the hardware.
 */
#define RNC_STATES {\
	C(RNC_INITIAL),\
	C(RNC_POSTING),\
	C(RNC_INVALIDATING),\
	C(RNC_RESUMING),\
	C(RNC_READY),\
	C(RNC_TX_SUSPENDED),\
	C(RNC_TX_RX_SUSPENDED),\
	C(RNC_AWAIT_SUSPENSION),\
	}
#undef C
#define C(a) SCI_##a
enum scis_sds_remote_node_context_states RNC_STATES;
#undef C
const char *rnc_state_name(enum scis_sds_remote_node_context_states state);

/**
 *
 *
 * This enumeration is used to define the end destination state for the remote
 * node context.
 */
enum sci_remote_node_context_destination_state {
	RNC_DEST_UNSPECIFIED,
	RNC_DEST_READY,
	RNC_DEST_FINAL,
	RNC_DEST_SUSPENDED,       /* Set when suspend during post/invalidate */
	RNC_DEST_SUSPENDED_RESUME /* Set when a resume was done during posting
				   * or invalidating and already suspending.
				   */
};

/**
 * struct sci_remote_node_context - This structure contains the data
 *    associated with the remote node context object.  The remote node context
 *    (RNC) object models the the remote device information necessary to manage
 *    the silicon RNC.
 */
struct sci_remote_node_context {
	/**
	 * This field indicates the remote node index (RNI) associated with
	 * this RNC.
	 */
	u16 remote_node_index;

	/**
	 * This field is the recored suspension type of the remote node
	 * context suspension.
	 */
	u32 suspend_type;
	enum sci_remote_node_suspension_reasons suspend_reason;
	u32 suspend_count;

	/**
	 * This field is true if the remote node context is resuming from its current
	 * state.  This can cause an automatic resume on receiving a suspension
	 * notification.
	 */
	enum sci_remote_node_context_destination_state destination_state;

	/**
	 * This field contains the callback function that the user requested to be
	 * called when the requested state transition is complete.
	 */
	scics_sds_remote_node_context_callback user_callback;

	/**
	 * This field contains the parameter that is called when the user requested
	 * state transition is completed.
	 */
	void *user_cookie;

	/**
	 * This field contains the data for the object's state machine.
	 */
	struct sci_base_state_machine sm;
};

void sci_remote_node_context_construct(struct sci_remote_node_context *rnc,
					    u16 remote_node_index);


bool sci_remote_node_context_is_ready(
	struct sci_remote_node_context *sci_rnc);

bool sci_remote_node_context_is_suspended(struct sci_remote_node_context *sci_rnc);

enum sci_status sci_remote_node_context_event_handler(struct sci_remote_node_context *sci_rnc,
							   u32 event_code);
enum sci_status sci_remote_node_context_destruct(struct sci_remote_node_context *sci_rnc,
						      scics_sds_remote_node_context_callback callback,
						      void *callback_parameter);
enum sci_status sci_remote_node_context_suspend(struct sci_remote_node_context *sci_rnc,
						     enum sci_remote_node_suspension_reasons reason,
						     u32 suspension_code);
enum sci_status sci_remote_node_context_resume(struct sci_remote_node_context *sci_rnc,
						    scics_sds_remote_node_context_callback cb_fn,
						    void *cb_p);
enum sci_status sci_remote_node_context_start_task(struct sci_remote_node_context *sci_rnc,
						   struct isci_request *ireq,
						   scics_sds_remote_node_context_callback cb_fn,
						   void *cb_p);
enum sci_status sci_remote_node_context_start_io(struct sci_remote_node_context *sci_rnc,
						      struct isci_request *ireq);
int sci_remote_node_context_is_safe_to_abort(
	struct sci_remote_node_context *sci_rnc);

static inline bool sci_remote_node_context_is_being_destroyed(
	struct sci_remote_node_context *sci_rnc)
{
	return (sci_rnc->destination_state == RNC_DEST_FINAL)
		|| ((sci_rnc->sm.current_state_id == SCI_RNC_INITIAL)
		    && (sci_rnc->destination_state == RNC_DEST_UNSPECIFIED));
}
#endif  /* _SCIC_SDS_REMOTE_NODE_CONTEXT_H_ */
