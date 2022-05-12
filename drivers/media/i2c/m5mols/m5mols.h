/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Header for M-5MOLS 8M Pixel camera sensor with ISP
 *
 * Copyright (C) 2011 Samsung Electronics Co., Ltd.
 * Author: HeungJun Kim <riverful.kim@samsung.com>
 *
 * Copyright (C) 2009 Samsung Electronics Co., Ltd.
 * Author: Dongsoo Nathaniel Kim <dongsoo45.kim@samsung.com>
 */

#ifndef M5MOLS_H
#define M5MOLS_H

#include <linux/sizes.h>
#include <media/v4l2-subdev.h>
#include "m5mols_reg.h"


/* An amount of data transmitted in addition to the value
 * determined by CAPP_JPEG_SIZE_MAX register.
 */
#define M5MOLS_JPEG_TAGS_SIZE		0x20000
#define M5MOLS_MAIN_JPEG_SIZE_MAX	(5 * SZ_1M)

extern int m5mols_debug;

enum m5mols_restype {
	M5MOLS_RESTYPE_MONITOR,
	M5MOLS_RESTYPE_CAPTURE,
	M5MOLS_RESTYPE_MAX,
};

/**
 * struct m5mols_resolution - structure for the resolution
 * @type: resolution type according to the pixel code
 * @width: width of the resolution
 * @height: height of the resolution
 * @reg: resolution preset register value
 */
struct m5mols_resolution {
	u8 reg;
	enum m5mols_restype type;
	u16 width;
	u16 height;
};

/**
 * struct m5mols_exif - structure for the EXIF information of M-5MOLS
 * @exposure_time: exposure time register value
 * @shutter_speed: speed of the shutter register value
 * @aperture: aperture register value
 * @brightness: brightness register value
 * @exposure_bias: it calls also EV bias
 * @iso_speed: ISO register value
 * @flash: status register value of the flash
 * @sdr: status register value of the Subject Distance Range
 * @qval: not written exact meaning in document
 */
struct m5mols_exif {
	u32 exposure_time;
	u32 shutter_speed;
	u32 aperture;
	u32 brightness;
	u32 exposure_bias;
	u16 iso_speed;
	u16 flash;
	u16 sdr;
	u16 qval;
};

/**
 * struct m5mols_capture - Structure for the capture capability
 * @exif: EXIF information
 * @buf_size: internal JPEG frame buffer size, in bytes
 * @main: size in bytes of the main image
 * @thumb: size in bytes of the thumb image, if it was accompanied
 * @total: total size in bytes of the produced image
 */
struct m5mols_capture {
	struct m5mols_exif exif;
	unsigned int buf_size;
	u32 main;
	u32 thumb;
	u32 total;
};

/**
 * struct m5mols_scenemode - structure for the scenemode capability
 * @metering: metering light register value
 * @ev_bias: EV bias register value
 * @wb_mode: mode which means the WhiteBalance is Auto or Manual
 * @wb_preset: whitebalance preset register value in the Manual mode
 * @chroma_en: register value whether the Chroma capability is enabled or not
 * @chroma_lvl: chroma's level register value
 * @edge_en: register value Whether the Edge capability is enabled or not
 * @edge_lvl: edge's level register value
 * @af_range: Auto Focus's range
 * @fd_mode: Face Detection mode
 * @mcc: Multi-axis Color Conversion which means emotion color
 * @light: status of the Light
 * @flash: status of the Flash
 * @tone: Tone color which means Contrast
 * @iso: ISO register value
 * @capt_mode: Mode of the Image Stabilization while the camera capturing
 * @wdr: Wide Dynamic Range register value
 *
 * The each value according to each scenemode is recommended in the documents.
 */
struct m5mols_scenemode {
	u8 metering;
	u8 ev_bias;
	u8 wb_mode;
	u8 wb_preset;
	u8 chroma_en;
	u8 chroma_lvl;
	u8 edge_en;
	u8 edge_lvl;
	u8 af_range;
	u8 fd_mode;
	u8 mcc;
	u8 light;
	u8 flash;
	u8 tone;
	u8 iso;
	u8 capt_mode;
	u8 wdr;
};

#define VERSION_STRING_SIZE	22

/**
 * struct m5mols_version - firmware version information
 * @customer:	customer information
 * @project:	version of project information according to customer
 * @fw:		firmware revision
 * @hw:		hardware revision
 * @param:	version of the parameter
 * @awb:	Auto WhiteBalance algorithm version
 * @str:	information about manufacturer and packaging vendor
 * @af:		Auto Focus version
 *
 * The register offset starts the customer version at 0x0, and it ends
 * the awb version at 0x09. The customer, project information occupies 1 bytes
 * each. And also the fw, hw, param, awb each requires 2 bytes. The str is
 * unique string associated with firmware's version. It includes information
 * about manufacturer and the vendor of the sensor's packaging. The least
 * significant 2 bytes of the string indicate packaging manufacturer.
 */
struct m5mols_version {
	u8	customer;
	u8	project;
	u16	fw;
	u16	hw;
	u16	param;
	u16	awb;
	u8	str[VERSION_STRING_SIZE];
	u8	af;
};

/**
 * struct m5mols_info - M-5MOLS driver data structure
 * @pdata: platform data
 * @sd: v4l-subdev instance
 * @pad: media pad
 * @irq_waitq: waitqueue for the capture
 * @irq_done: set to 1 in the interrupt handler
 * @handle: control handler
 * @auto_exposure: auto/manual exposure control
 * @exposure_bias: exposure compensation control
 * @exposure: manual exposure control
 * @metering: exposure metering control
 * @auto_iso: auto/manual ISO sensitivity control
 * @iso: manual ISO sensitivity control
 * @auto_wb: auto white balance control
 * @lock_3a: 3A lock control
 * @colorfx: color effect control
 * @saturation: saturation control
 * @zoom: zoom control
 * @wdr: wide dynamic range control
 * @stabilization: image stabilization control
 * @jpeg_quality: JPEG compression quality control
 * @set_power: optional power callback to the board code
 * @lock: mutex protecting the structure fields below
 * @ffmt: current fmt according to resolution type
 * @res_type: current resolution type
 * @ver: information of the version
 * @cap: the capture mode attributes
 * @isp_ready: 1 when the ISP controller has completed booting
 * @power: current sensor's power status
 * @ctrl_sync: 1 when the control handler state is restored in H/W
 * @resolution:	register value for current resolution
 * @mode: register value for current operation mode
 */
struct m5mols_info {
	const struct m5mols_platform_data *pdata;
	struct v4l2_subdev sd;
	struct media_pad pad;

	wait_queue_head_t irq_waitq;
	atomic_t irq_done;

	struct v4l2_ctrl_handler handle;
	struct {
		/* exposure/exposure bias/auto exposure cluster */
		struct v4l2_ctrl *auto_exposure;
		struct v4l2_ctrl *exposure_bias;
		struct v4l2_ctrl *exposure;
		struct v4l2_ctrl *metering;
	};
	struct {
		/* iso/auto iso cluster */
		struct v4l2_ctrl *auto_iso;
		struct v4l2_ctrl *iso;
	};
	struct v4l2_ctrl *auto_wb;

	struct v4l2_ctrl *lock_3a;
	struct v4l2_ctrl *colorfx;
	struct v4l2_ctrl *saturation;
	struct v4l2_ctrl *zoom;
	struct v4l2_ctrl *wdr;
	struct v4l2_ctrl *stabilization;
	struct v4l2_ctrl *jpeg_quality;

	int (*set_power)(struct device *dev, int on);

	struct mutex lock;

	struct v4l2_mbus_framefmt ffmt[M5MOLS_RESTYPE_MAX];
	int res_type;

	struct m5mols_version ver;
	struct m5mols_capture cap;

	unsigned int isp_ready:1;
	unsigned int power:1;
	unsigned int ctrl_sync:1;

	u8 resolution;
	u8 mode;
};

#define is_available_af(__info)	(__info->ver.af)
#define is_code(__code, __type) (__code == m5mols_default_ffmt[__type].code)
#define is_manufacturer(__info, __manufacturer)	\
				(__info->ver.str[0] == __manufacturer[0] && \
				 __info->ver.str[1] == __manufacturer[1])
/*
 * I2C operation of the M-5MOLS
 *
 * The I2C read operation of the M-5MOLS requires 2 messages. The first
 * message sends the information about the command, command category, and total
 * message size. The second message is used to retrieve the data specified in
 * the first message
 *
 *   1st message                                2nd message
 *   +-------+---+----------+-----+-------+     +------+------+------+------+
 *   | size1 | R | category | cmd | size2 |     | d[0] | d[1] | d[2] | d[3] |
 *   +-------+---+----------+-----+-------+     +------+------+------+------+
 *   - size1: message data size(5 in this case)
 *   - size2: desired buffer size of the 2nd message
 *   - d[0..3]: according to size2
 *
 * The I2C write operation needs just one message. The message includes
 * category, command, total size, and desired data.
 *
 *   1st message
 *   +-------+---+----------+-----+------+------+------+------+
 *   | size1 | W | category | cmd | d[0] | d[1] | d[2] | d[3] |
 *   +-------+---+----------+-----+------+------+------+------+
 *   - d[0..3]: according to size1
 */
int m5mols_read_u8(struct v4l2_subdev *sd, u32 reg_comb, u8 *val);
int m5mols_read_u16(struct v4l2_subdev *sd, u32 reg_comb, u16 *val);
int m5mols_read_u32(struct v4l2_subdev *sd, u32 reg_comb, u32 *val);
int m5mols_write(struct v4l2_subdev *sd, u32 reg_comb, u32 val);

int m5mols_busy_wait(struct v4l2_subdev *sd, u32 reg, u32 value, u32 mask,
		     int timeout);

/* Mask value for busy waiting until M-5MOLS I2C interface is initialized */
#define M5MOLS_I2C_RDY_WAIT_FL		(1 << 16)
/* ISP state transition timeout, in ms */
#define M5MOLS_MODE_CHANGE_TIMEOUT	200
#define M5MOLS_BUSY_WAIT_DEF_TIMEOUT	250

/*
 * Mode operation of the M-5MOLS
 *
 * Changing the mode of the M-5MOLS is needed right executing order.
 * There are three modes(PARAMETER, MONITOR, CAPTURE) which can be changed
 * by user. There are various categories associated with each mode.
 *
 * +============================================================+
 * | mode	| category					|
 * +============================================================+
 * | FLASH	| FLASH(only after Stand-by or Power-on)	|
 * | SYSTEM	| SYSTEM(only after sensor arm-booting)		|
 * | PARAMETER	| PARAMETER					|
 * | MONITOR	| MONITOR(preview), Auto Focus, Face Detection	|
 * | CAPTURE	| Single CAPTURE, Preview(recording)		|
 * +============================================================+
 *
 * The available executing order between each modes are as follows:
 *   PARAMETER <---> MONITOR <---> CAPTURE
 */
int m5mols_set_mode(struct m5mols_info *info, u8 mode);

int m5mols_enable_interrupt(struct v4l2_subdev *sd, u8 reg);
int m5mols_wait_interrupt(struct v4l2_subdev *sd, u8 condition, u32 timeout);
int m5mols_restore_controls(struct m5mols_info *info);
int m5mols_start_capture(struct m5mols_info *info);
int m5mols_do_scenemode(struct m5mols_info *info, u8 mode);
int m5mols_lock_3a(struct m5mols_info *info, bool lock);
int m5mols_set_ctrl(struct v4l2_ctrl *ctrl);
int m5mols_init_controls(struct v4l2_subdev *sd);

/* The firmware function */
int m5mols_update_fw(struct v4l2_subdev *sd,
		     int (*set_power)(struct m5mols_info *, bool));

static inline struct m5mols_info *to_m5mols(struct v4l2_subdev *subdev)
{
	return container_of(subdev, struct m5mols_info, sd);
}

static inline struct v4l2_subdev *to_sd(struct v4l2_ctrl *ctrl)
{
	struct m5mols_info *info = container_of(ctrl->handler,
						struct m5mols_info, handle);
	return &info->sd;
}

static inline void m5mols_set_ctrl_mode(struct v4l2_ctrl *ctrl,
					unsigned int mode)
{
	ctrl->priv = (void *)(uintptr_t)mode;
}

static inline unsigned int m5mols_get_ctrl_mode(struct v4l2_ctrl *ctrl)
{
	return (unsigned int)(uintptr_t)ctrl->priv;
}

#endif	/* M5MOLS_H */
