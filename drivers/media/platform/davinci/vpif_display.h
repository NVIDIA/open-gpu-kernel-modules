/*
 * VPIF display header file
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - https://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed .as is. WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef VPIF_DISPLAY_H
#define VPIF_DISPLAY_H

/* Header files */
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-device.h>

#include "vpif.h"

/* Macros */
#define VPIF_DISPLAY_VERSION	"0.0.2"

#define VPIF_VALID_FIELD(field) \
	(((V4L2_FIELD_ANY == field) || (V4L2_FIELD_NONE == field)) || \
	(((V4L2_FIELD_INTERLACED == field) || (V4L2_FIELD_SEQ_TB == field)) || \
	(V4L2_FIELD_SEQ_BT == field)))

#define VPIF_DISPLAY_MAX_DEVICES	(2)
#define VPIF_SLICED_BUF_SIZE		(256)
#define VPIF_SLICED_MAX_SERVICES	(3)
#define VPIF_VIDEO_INDEX		(0)
#define VPIF_VBI_INDEX			(1)
#define VPIF_HBI_INDEX			(2)

/* Setting it to 1 as HBI/VBI support yet to be added , else 3*/
#define VPIF_NUMOBJECTS	(1)

/* Macros */
#define ISALIGNED(a)    (0 == ((a) & 7))

/* enumerated data types */
/* Enumerated data type to give id to each device per channel */
enum vpif_channel_id {
	VPIF_CHANNEL2_VIDEO = 0,	/* Channel2 Video */
	VPIF_CHANNEL3_VIDEO,		/* Channel3 Video */
};

/* structures */

struct video_obj {
	enum v4l2_field buf_field;
	u32 latest_only;		/* indicate whether to return
					 * most recent displayed frame only */
	v4l2_std_id stdid;		/* Currently selected or default
					 * standard */
	struct v4l2_dv_timings dv_timings;
};

struct vpif_disp_buffer {
	struct vb2_v4l2_buffer vb;
	struct list_head list;
};

struct common_obj {
	struct vpif_disp_buffer *cur_frm;	/* Pointer pointing to current
						 * vb2_buffer */
	struct vpif_disp_buffer *next_frm;	/* Pointer pointing to next
						 * vb2_buffer */
	struct v4l2_format fmt;			/* Used to store the format */
	struct vb2_queue buffer_queue;		/* Buffer queue used in
						 * video-buf */

	struct list_head dma_queue;		/* Queue of filled frames */
	spinlock_t irqlock;			/* Used in video-buf */

	/* channel specific parameters */
	struct mutex lock;			/* lock used to access this
						 * structure */
	u32 ytop_off;				/* offset of Y top from the
						 * starting of the buffer */
	u32 ybtm_off;				/* offset of Y bottom from the
						 * starting of the buffer */
	u32 ctop_off;				/* offset of C top from the
						 * starting of the buffer */
	u32 cbtm_off;				/* offset of C bottom from the
						 * starting of the buffer */
	/* Function pointer to set the addresses */
	void (*set_addr)(unsigned long, unsigned long,
				unsigned long, unsigned long);
	u32 height;
	u32 width;
};

struct channel_obj {
	/* V4l2 specific parameters */
	struct video_device video_dev;	/* Identifies video device for
					 * this channel */
	u32 field_id;			/* Indicates id of the field
					 * which is being displayed */
	u8 initialized;			/* flag to indicate whether
					 * encoder is initialized */
	u32 output_idx;			/* Current output index */
	struct v4l2_subdev *sd;		/* Current output subdev(may be NULL) */

	enum vpif_channel_id channel_id;/* Identifies channel */
	struct vpif_params vpifparams;
	struct common_obj common[VPIF_NUMOBJECTS];
	struct video_obj video;
};

/* vpif device structure */
struct vpif_device {
	struct v4l2_device v4l2_dev;
	struct channel_obj *dev[VPIF_DISPLAY_NUM_CHANNELS];
	struct v4l2_subdev **sd;
	struct vpif_display_config *config;
};

#endif				/* VPIF_DISPLAY_H */
