/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Microchip Image Sensor Controller (ISC) driver header file
 *
 * Copyright (C) 2016-2019 Microchip Technology, Inc.
 *
 * Author: Songjun Wu
 * Author: Eugen Hristev <eugen.hristev@microchip.com>
 *
 */
#ifndef _ATMEL_ISC_H_

#define ISC_MAX_SUPPORT_WIDTH   2592
#define ISC_MAX_SUPPORT_HEIGHT  1944

#define ISC_CLK_MAX_DIV		255

enum isc_clk_id {
	ISC_ISPCK = 0,
	ISC_MCK = 1,
};

struct isc_clk {
	struct clk_hw   hw;
	struct clk      *clk;
	struct regmap   *regmap;
	spinlock_t	lock;	/* serialize access to clock registers */
	u8		id;
	u8		parent_id;
	u32		div;
	struct device	*dev;
};

#define to_isc_clk(v) container_of(v, struct isc_clk, hw)

struct isc_buffer {
	struct vb2_v4l2_buffer  vb;
	struct list_head	list;
};

struct isc_subdev_entity {
	struct v4l2_subdev		*sd;
	struct v4l2_async_subdev	*asd;
	struct device_node		*epn;
	struct v4l2_async_notifier      notifier;

	u32 pfe_cfg0;

	struct list_head list;
};

/*
 * struct isc_format - ISC media bus format information
			This structure represents the interface between the ISC
			and the sensor. It's the input format received by
			the ISC.
 * @fourcc:		Fourcc code for this format
 * @mbus_code:		V4L2 media bus format code.
 * @cfa_baycfg:		If this format is RAW BAYER, indicate the type of bayer.
			this is either BGBG, RGRG, etc.
 * @pfe_cfg0_bps:	Number of hardware data lines connected to the ISC
 */

struct isc_format {
	u32	fourcc;
	u32	mbus_code;
	u32	cfa_baycfg;

	bool	sd_support;
	u32	pfe_cfg0_bps;
};

/* Pipeline bitmap */
#define WB_ENABLE	BIT(0)
#define CFA_ENABLE	BIT(1)
#define CC_ENABLE	BIT(2)
#define GAM_ENABLE	BIT(3)
#define GAM_BENABLE	BIT(4)
#define GAM_GENABLE	BIT(5)
#define GAM_RENABLE	BIT(6)
#define CSC_ENABLE	BIT(7)
#define CBC_ENABLE	BIT(8)
#define SUB422_ENABLE	BIT(9)
#define SUB420_ENABLE	BIT(10)

#define GAM_ENABLES	(GAM_RENABLE | GAM_GENABLE | GAM_BENABLE | GAM_ENABLE)

/*
 * struct fmt_config - ISC format configuration and internal pipeline
			This structure represents the internal configuration
			of the ISC.
			It also holds the format that ISC will present to v4l2.
 * @sd_format:		Pointer to an isc_format struct that holds the sensor
			configuration.
 * @fourcc:		Fourcc code for this format.
 * @bpp:		Bytes per pixel in the current format.
 * @rlp_cfg_mode:	Configuration of the RLP (rounding, limiting packaging)
 * @dcfg_imode:		Configuration of the input of the DMA module
 * @dctrl_dview:	Configuration of the output of the DMA module
 * @bits_pipeline:	Configuration of the pipeline, which modules are enabled
 */
struct fmt_config {
	struct isc_format	*sd_format;

	u32			fourcc;
	u8			bpp;

	u32			rlp_cfg_mode;
	u32			dcfg_imode;
	u32			dctrl_dview;

	u32			bits_pipeline;
};

#define HIST_ENTRIES		512
#define HIST_BAYER		(ISC_HIS_CFG_MODE_B + 1)

enum{
	HIST_INIT = 0,
	HIST_ENABLED,
	HIST_DISABLED,
};

struct isc_ctrls {
	struct v4l2_ctrl_handler handler;

	u32 brightness;
	u32 contrast;
	u8 gamma_index;
#define ISC_WB_NONE	0
#define ISC_WB_AUTO	1
#define ISC_WB_ONETIME	2
	u8 awb;

	/* one for each component : GR, R, GB, B */
	u32 gain[HIST_BAYER];
	s32 offset[HIST_BAYER];

	u32 hist_entry[HIST_ENTRIES];
	u32 hist_count[HIST_BAYER];
	u8 hist_id;
	u8 hist_stat;
#define HIST_MIN_INDEX		0
#define HIST_MAX_INDEX		1
	u32 hist_minmax[HIST_BAYER][2];
};

#define ISC_PIPE_LINE_NODE_NUM	11

/*
 * struct isc_device - ISC device driver data/config struct
 * @regmap:		Register map
 * @hclock:		Hclock clock input (refer datasheet)
 * @ispck:		iscpck clock (refer datasheet)
 * @isc_clks:		ISC clocks
 *
 * @dev:		Registered device driver
 * @v4l2_dev:		v4l2 registered device
 * @video_dev:		registered video device
 *
 * @vb2_vidq:		video buffer 2 video queue
 * @dma_queue_lock:	lock to serialize the dma buffer queue
 * @dma_queue:		the queue for dma buffers
 * @cur_frm:		current isc frame/buffer
 * @sequence:		current frame number
 * @stop:		true if isc is not streaming, false if streaming
 * @comp:		completion reference that signals frame completion
 *
 * @fmt:		current v42l format
 * @user_formats:	list of formats that are supported and agreed with sd
 * @num_user_formats:	how many formats are in user_formats
 *
 * @config:		current ISC format configuration
 * @try_config:		the current ISC try format , not yet activated
 *
 * @ctrls:		holds information about ISC controls
 * @do_wb_ctrl:		control regarding the DO_WHITE_BALANCE button
 * @awb_work:		workqueue reference for autowhitebalance histogram
 *			analysis
 *
 * @lock:		lock for serializing userspace file operations
 *			with ISC operations
 * @awb_lock:		lock for serializing awb work queue operations
 *			with DMA/buffer operations
 *
 * @pipeline:		configuration of the ISC pipeline
 *
 * @current_subdev:	current subdevice: the sensor
 * @subdev_entities:	list of subdevice entitites
 */
struct isc_device {
	struct regmap		*regmap;
	struct clk		*hclock;
	struct clk		*ispck;
	struct isc_clk		isc_clks[2];

	struct device		*dev;
	struct v4l2_device	v4l2_dev;
	struct video_device	video_dev;

	struct vb2_queue	vb2_vidq;
	spinlock_t		dma_queue_lock; /* serialize access to dma queue */
	struct list_head	dma_queue;
	struct isc_buffer	*cur_frm;
	unsigned int		sequence;
	bool			stop;
	struct completion	comp;

	struct v4l2_format	fmt;
	struct isc_format	**user_formats;
	unsigned int		num_user_formats;

	struct fmt_config	config;
	struct fmt_config	try_config;

	struct isc_ctrls	ctrls;
	struct work_struct	awb_work;

	struct mutex		lock; /* serialize access to file operations */
	spinlock_t		awb_lock; /* serialize access to DMA buffers from awb work queue */

	struct regmap_field	*pipeline[ISC_PIPE_LINE_NODE_NUM];

	struct isc_subdev_entity	*current_subdev;
	struct list_head		subdev_entities;

	struct {
#define ISC_CTRL_DO_WB 1
#define ISC_CTRL_R_GAIN 2
#define ISC_CTRL_B_GAIN 3
#define ISC_CTRL_GR_GAIN 4
#define ISC_CTRL_GB_GAIN 5
#define ISC_CTRL_R_OFF 6
#define ISC_CTRL_B_OFF 7
#define ISC_CTRL_GR_OFF 8
#define ISC_CTRL_GB_OFF 9
		struct v4l2_ctrl	*awb_ctrl;
		struct v4l2_ctrl	*do_wb_ctrl;
		struct v4l2_ctrl	*r_gain_ctrl;
		struct v4l2_ctrl	*b_gain_ctrl;
		struct v4l2_ctrl	*gr_gain_ctrl;
		struct v4l2_ctrl	*gb_gain_ctrl;
		struct v4l2_ctrl	*r_off_ctrl;
		struct v4l2_ctrl	*b_off_ctrl;
		struct v4l2_ctrl	*gr_off_ctrl;
		struct v4l2_ctrl	*gb_off_ctrl;
	};
};

#define GAMMA_MAX	2
#define GAMMA_ENTRIES	64

#define ATMEL_ISC_NAME "atmel-isc"

extern struct isc_format formats_list[];
extern const struct isc_format controller_formats[];
extern const u32 isc_gamma_table[GAMMA_MAX + 1][GAMMA_ENTRIES];
extern const struct regmap_config isc_regmap_config;
extern const struct v4l2_async_notifier_operations isc_async_ops;

irqreturn_t isc_interrupt(int irq, void *dev_id);
int isc_pipeline_init(struct isc_device *isc);
int isc_clk_init(struct isc_device *isc);
void isc_subdev_cleanup(struct isc_device *isc);
void isc_clk_cleanup(struct isc_device *isc);

#endif
