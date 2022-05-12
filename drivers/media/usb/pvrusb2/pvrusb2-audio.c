// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *  Copyright (C) 2004 Aurelien Alleaume <slts@free.fr>
 */

#include "pvrusb2-audio.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include <linux/videodev2.h>
#include <media/drv-intf/msp3400.h>
#include <media/v4l2-common.h>


struct routing_scheme {
	const int *def;
	unsigned int cnt;
};

static const int routing_scheme0[] = {
	[PVR2_CVAL_INPUT_TV]        = MSP_INPUT_DEFAULT,
	[PVR2_CVAL_INPUT_RADIO]     = MSP_INPUT(MSP_IN_SCART2,
						MSP_IN_TUNER1,
						MSP_DSP_IN_SCART,
						MSP_DSP_IN_SCART),
	[PVR2_CVAL_INPUT_COMPOSITE] = MSP_INPUT(MSP_IN_SCART1,
						MSP_IN_TUNER1,
						MSP_DSP_IN_SCART,
						MSP_DSP_IN_SCART),
	[PVR2_CVAL_INPUT_SVIDEO]    = MSP_INPUT(MSP_IN_SCART1,
						MSP_IN_TUNER1,
						MSP_DSP_IN_SCART,
						MSP_DSP_IN_SCART),
};

static const struct routing_scheme routing_def0 = {
	.def = routing_scheme0,
	.cnt = ARRAY_SIZE(routing_scheme0),
};

static const struct routing_scheme *routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_HAUPPAUGE] = &routing_def0,
};

void pvr2_msp3400_subdev_update(struct pvr2_hdw *hdw, struct v4l2_subdev *sd)
{
	if (hdw->input_dirty || hdw->force_dirty) {
		const struct routing_scheme *sp;
		unsigned int sid = hdw->hdw_desc->signal_routing_scheme;
		u32 input;

		pvr2_trace(PVR2_TRACE_CHIPS, "subdev msp3400 v4l2 set_stereo");
		sp = (sid < ARRAY_SIZE(routing_schemes)) ?
			routing_schemes[sid] : NULL;

		if ((sp != NULL) &&
		    (hdw->input_val >= 0) &&
		    (hdw->input_val < sp->cnt)) {
			input = sp->def[hdw->input_val];
		} else {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "*** WARNING *** subdev msp3400 set_input: Invalid routing scheme (%u) and/or input (%d)",
				   sid, hdw->input_val);
			return;
		}
		sd->ops->audio->s_routing(sd, input,
			MSP_OUTPUT(MSP_SC_IN_DSP_SCART1), 0);
	}
}
