// SPDX-License-Identifier: GPL-2.0-only
/*
 * TI VPFE capture Driver
 *
 * Copyright (C) 2013 - 2014 Texas Instruments, Inc.
 *
 * Benoit Parrot <bparrot@ti.com>
 * Lad, Prabhakar <prabhakar.csengg@gmail.com>
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_graph.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/videodev2.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fwnode.h>
#include <media/v4l2-rect.h>

#include "am437x-vpfe.h"

#define VPFE_MODULE_NAME	"vpfe"
#define VPFE_VERSION		"0.1.0"

static int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Debug level 0-8");

#define vpfe_dbg(level, dev, fmt, arg...)	\
		v4l2_dbg(level, debug, &dev->v4l2_dev, fmt, ##arg)
#define vpfe_info(dev, fmt, arg...)	\
		v4l2_info(&dev->v4l2_dev, fmt, ##arg)
#define vpfe_err(dev, fmt, arg...)	\
		v4l2_err(&dev->v4l2_dev, fmt, ##arg)

/* standard information */
struct vpfe_standard {
	v4l2_std_id std_id;
	unsigned int width;
	unsigned int height;
	struct v4l2_fract pixelaspect;
	int frame_format;
};

static const struct vpfe_standard vpfe_standards[] = {
	{V4L2_STD_525_60, 720, 480, {11, 10}, 1},
	{V4L2_STD_625_50, 720, 576, {54, 59}, 1},
};

static struct vpfe_fmt formats[VPFE_NUM_FORMATS] = {
	{
		.fourcc		= V4L2_PIX_FMT_YUYV,
		.code		= MEDIA_BUS_FMT_YUYV8_2X8,
		.bitsperpixel	= 16,
	}, {
		.fourcc		= V4L2_PIX_FMT_UYVY,
		.code		= MEDIA_BUS_FMT_UYVY8_2X8,
		.bitsperpixel	= 16,
	}, {
		.fourcc		= V4L2_PIX_FMT_YVYU,
		.code		= MEDIA_BUS_FMT_YVYU8_2X8,
		.bitsperpixel	= 16,
	}, {
		.fourcc		= V4L2_PIX_FMT_VYUY,
		.code		= MEDIA_BUS_FMT_VYUY8_2X8,
		.bitsperpixel	= 16,
	}, {
		.fourcc		= V4L2_PIX_FMT_SBGGR8,
		.code		= MEDIA_BUS_FMT_SBGGR8_1X8,
		.bitsperpixel	= 8,
	}, {
		.fourcc		= V4L2_PIX_FMT_SGBRG8,
		.code		= MEDIA_BUS_FMT_SGBRG8_1X8,
		.bitsperpixel	= 8,
	}, {
		.fourcc		= V4L2_PIX_FMT_SGRBG8,
		.code		= MEDIA_BUS_FMT_SGRBG8_1X8,
		.bitsperpixel	= 8,
	}, {
		.fourcc		= V4L2_PIX_FMT_SRGGB8,
		.code		= MEDIA_BUS_FMT_SRGGB8_1X8,
		.bitsperpixel	= 8,
	}, {
		.fourcc		= V4L2_PIX_FMT_RGB565,
		.code		= MEDIA_BUS_FMT_RGB565_2X8_LE,
		.bitsperpixel	= 16,
	}, {
		.fourcc		= V4L2_PIX_FMT_RGB565X,
		.code		= MEDIA_BUS_FMT_RGB565_2X8_BE,
		.bitsperpixel	= 16,
	},
};

static int __subdev_get_format(struct vpfe_device *vpfe,
			       struct v4l2_mbus_framefmt *fmt);
static int vpfe_calc_format_size(struct vpfe_device *vpfe,
				 const struct vpfe_fmt *fmt,
				 struct v4l2_format *f);

static struct vpfe_fmt *find_format_by_code(struct vpfe_device *vpfe,
					    unsigned int code)
{
	struct vpfe_fmt *fmt;
	unsigned int k;

	for (k = 0; k < vpfe->num_active_fmt; k++) {
		fmt = vpfe->active_fmt[k];
		if (fmt->code == code)
			return fmt;
	}

	return NULL;
}

static struct vpfe_fmt *find_format_by_pix(struct vpfe_device *vpfe,
					   unsigned int pixelformat)
{
	struct vpfe_fmt *fmt;
	unsigned int k;

	for (k = 0; k < vpfe->num_active_fmt; k++) {
		fmt = vpfe->active_fmt[k];
		if (fmt->fourcc == pixelformat)
			return fmt;
	}

	return NULL;
}

static unsigned int __get_bytesperpixel(struct vpfe_device *vpfe,
					const struct vpfe_fmt *fmt)
{
	struct vpfe_subdev_info *sdinfo = vpfe->current_subdev;
	unsigned int bus_width = sdinfo->vpfe_param.bus_width;
	u32 bpp, bus_width_bytes, clocksperpixel;

	bus_width_bytes = ALIGN(bus_width, 8) >> 3;
	clocksperpixel = DIV_ROUND_UP(fmt->bitsperpixel, bus_width);
	bpp = clocksperpixel * bus_width_bytes;

	return bpp;
}

/*  Print Four-character-code (FOURCC) */
static char *print_fourcc(u32 fmt)
{
	static char code[5];

	code[0] = (unsigned char)(fmt & 0xff);
	code[1] = (unsigned char)((fmt >> 8) & 0xff);
	code[2] = (unsigned char)((fmt >> 16) & 0xff);
	code[3] = (unsigned char)((fmt >> 24) & 0xff);
	code[4] = '\0';

	return code;
}

static inline u32 vpfe_reg_read(struct vpfe_ccdc *ccdc, u32 offset)
{
	return ioread32(ccdc->ccdc_cfg.base_addr + offset);
}

static inline void vpfe_reg_write(struct vpfe_ccdc *ccdc, u32 val, u32 offset)
{
	iowrite32(val, ccdc->ccdc_cfg.base_addr + offset);
}

static inline struct vpfe_device *to_vpfe(struct vpfe_ccdc *ccdc)
{
	return container_of(ccdc, struct vpfe_device, ccdc);
}

static inline
struct vpfe_cap_buffer *to_vpfe_buffer(struct vb2_v4l2_buffer *vb)
{
	return container_of(vb, struct vpfe_cap_buffer, vb);
}

static inline void vpfe_pcr_enable(struct vpfe_ccdc *ccdc, int flag)
{
	vpfe_reg_write(ccdc, !!flag, VPFE_PCR);
}

static void vpfe_config_enable(struct vpfe_ccdc *ccdc, int flag)
{
	unsigned int cfg;

	if (!flag) {
		cfg = vpfe_reg_read(ccdc, VPFE_CONFIG);
		cfg &= ~(VPFE_CONFIG_EN_ENABLE << VPFE_CONFIG_EN_SHIFT);
	} else {
		cfg = VPFE_CONFIG_EN_ENABLE << VPFE_CONFIG_EN_SHIFT;
	}

	vpfe_reg_write(ccdc, cfg, VPFE_CONFIG);
}

static void vpfe_ccdc_setwin(struct vpfe_ccdc *ccdc,
			     struct v4l2_rect *image_win,
			     enum ccdc_frmfmt frm_fmt,
			     int bpp)
{
	int horz_start, horz_nr_pixels;
	int vert_start, vert_nr_lines;
	int val, mid_img;

	/*
	 * ppc - per pixel count. indicates how many pixels per cell
	 * output to SDRAM. example, for ycbcr, it is one y and one c, so 2.
	 * raw capture this is 1
	 */
	horz_start = image_win->left * bpp;
	horz_nr_pixels = (image_win->width * bpp) - 1;
	vpfe_reg_write(ccdc, (horz_start << VPFE_HORZ_INFO_SPH_SHIFT) |
				horz_nr_pixels, VPFE_HORZ_INFO);

	vert_start = image_win->top;

	if (frm_fmt == CCDC_FRMFMT_INTERLACED) {
		vert_nr_lines = (image_win->height >> 1) - 1;
		vert_start >>= 1;
		/* configure VDINT0 */
		val = (vert_start << VPFE_VDINT_VDINT0_SHIFT);
	} else {
		vert_nr_lines = image_win->height - 1;
		/*
		 * configure VDINT0 and VDINT1. VDINT1 will be at half
		 * of image height
		 */
		mid_img = vert_start + (image_win->height / 2);
		val = (vert_start << VPFE_VDINT_VDINT0_SHIFT) |
				(mid_img & VPFE_VDINT_VDINT1_MASK);
	}

	vpfe_reg_write(ccdc, val, VPFE_VDINT);

	vpfe_reg_write(ccdc, (vert_start << VPFE_VERT_START_SLV0_SHIFT) |
				vert_start, VPFE_VERT_START);
	vpfe_reg_write(ccdc, vert_nr_lines, VPFE_VERT_LINES);
}

static void vpfe_reg_dump(struct vpfe_ccdc *ccdc)
{
	struct vpfe_device *vpfe = to_vpfe(ccdc);

	vpfe_dbg(3, vpfe, "ALAW: 0x%x\n", vpfe_reg_read(ccdc, VPFE_ALAW));
	vpfe_dbg(3, vpfe, "CLAMP: 0x%x\n", vpfe_reg_read(ccdc, VPFE_CLAMP));
	vpfe_dbg(3, vpfe, "DCSUB: 0x%x\n", vpfe_reg_read(ccdc, VPFE_DCSUB));
	vpfe_dbg(3, vpfe, "BLKCMP: 0x%x\n", vpfe_reg_read(ccdc, VPFE_BLKCMP));
	vpfe_dbg(3, vpfe, "COLPTN: 0x%x\n", vpfe_reg_read(ccdc, VPFE_COLPTN));
	vpfe_dbg(3, vpfe, "SDOFST: 0x%x\n", vpfe_reg_read(ccdc, VPFE_SDOFST));
	vpfe_dbg(3, vpfe, "SYN_MODE: 0x%x\n",
		 vpfe_reg_read(ccdc, VPFE_SYNMODE));
	vpfe_dbg(3, vpfe, "HSIZE_OFF: 0x%x\n",
		 vpfe_reg_read(ccdc, VPFE_HSIZE_OFF));
	vpfe_dbg(3, vpfe, "HORZ_INFO: 0x%x\n",
		 vpfe_reg_read(ccdc, VPFE_HORZ_INFO));
	vpfe_dbg(3, vpfe, "VERT_START: 0x%x\n",
		 vpfe_reg_read(ccdc, VPFE_VERT_START));
	vpfe_dbg(3, vpfe, "VERT_LINES: 0x%x\n",
		 vpfe_reg_read(ccdc, VPFE_VERT_LINES));
}

static int
vpfe_ccdc_validate_param(struct vpfe_ccdc *ccdc,
			 struct vpfe_ccdc_config_params_raw *ccdcparam)
{
	struct vpfe_device *vpfe = to_vpfe(ccdc);
	u8 max_gamma, max_data;

	if (!ccdcparam->alaw.enable)
		return 0;

	max_gamma = ccdc_gamma_width_max_bit(ccdcparam->alaw.gamma_wd);
	max_data = ccdc_data_size_max_bit(ccdcparam->data_sz);

	if (ccdcparam->alaw.gamma_wd > VPFE_CCDC_GAMMA_BITS_09_0 ||
	    ccdcparam->data_sz > VPFE_CCDC_DATA_8BITS ||
	    max_gamma > max_data) {
		vpfe_dbg(1, vpfe, "Invalid data line select\n");
		return -EINVAL;
	}

	return 0;
}

static void
vpfe_ccdc_update_raw_params(struct vpfe_ccdc *ccdc,
			    struct vpfe_ccdc_config_params_raw *raw_params)
{
	struct vpfe_ccdc_config_params_raw *config_params =
				&ccdc->ccdc_cfg.bayer.config_params;

	*config_params = *raw_params;
}

/*
 * vpfe_ccdc_restore_defaults()
 * This function will write defaults to all CCDC registers
 */
static void vpfe_ccdc_restore_defaults(struct vpfe_ccdc *ccdc)
{
	int i;

	/* Disable CCDC */
	vpfe_pcr_enable(ccdc, 0);

	/* set all registers to default value */
	for (i = 4; i <= 0x94; i += 4)
		vpfe_reg_write(ccdc, 0,  i);

	vpfe_reg_write(ccdc, VPFE_NO_CULLING, VPFE_CULLING);
	vpfe_reg_write(ccdc, VPFE_CCDC_GAMMA_BITS_11_2, VPFE_ALAW);
}

static int vpfe_ccdc_close(struct vpfe_ccdc *ccdc, struct device *dev)
{
	struct vpfe_device *vpfe = to_vpfe(ccdc);
	u32 dma_cntl, pcr;

	pcr = vpfe_reg_read(ccdc, VPFE_PCR);
	if (pcr)
		vpfe_dbg(1, vpfe, "VPFE_PCR is still set (%x)", pcr);

	dma_cntl = vpfe_reg_read(ccdc, VPFE_DMA_CNTL);
	if ((dma_cntl & VPFE_DMA_CNTL_OVERFLOW))
		vpfe_dbg(1, vpfe, "VPFE_DMA_CNTL_OVERFLOW is still set (%x)",
			 dma_cntl);

	/* Disable CCDC by resetting all register to default POR values */
	vpfe_ccdc_restore_defaults(ccdc);

	/* Disabled the module at the CONFIG level */
	vpfe_config_enable(ccdc, 0);

	pm_runtime_put_sync(dev);
	return 0;
}

static int vpfe_ccdc_set_params(struct vpfe_ccdc *ccdc, void __user *params)
{
	struct vpfe_device *vpfe = to_vpfe(ccdc);
	struct vpfe_ccdc_config_params_raw raw_params;
	int x;

	if (ccdc->ccdc_cfg.if_type != VPFE_RAW_BAYER)
		return -EINVAL;

	x = copy_from_user(&raw_params, params, sizeof(raw_params));
	if (x) {
		vpfe_dbg(1, vpfe,
			 "%s: error in copying ccdc params, %d\n",
			 __func__, x);
		return -EFAULT;
	}

	if (!vpfe_ccdc_validate_param(ccdc, &raw_params)) {
		vpfe_ccdc_update_raw_params(ccdc, &raw_params);
		return 0;
	}

	return -EINVAL;
}

/*
 * vpfe_ccdc_config_ycbcr()
 * This function will configure CCDC for YCbCr video capture
 */
static void vpfe_ccdc_config_ycbcr(struct vpfe_ccdc *ccdc)
{
	struct ccdc_params_ycbcr *params = &ccdc->ccdc_cfg.ycbcr;
	u32 syn_mode;

	/*
	 * first restore the CCDC registers to default values
	 * This is important since we assume default values to be set in
	 * a lot of registers that we didn't touch
	 */
	vpfe_ccdc_restore_defaults(ccdc);

	/*
	 * configure pixel format, frame format, configure video frame
	 * format, enable output to SDRAM, enable internal timing generator
	 * and 8bit pack mode
	 */
	syn_mode = (((params->pix_fmt & VPFE_SYN_MODE_INPMOD_MASK) <<
		    VPFE_SYN_MODE_INPMOD_SHIFT) |
		    ((params->frm_fmt & VPFE_SYN_FLDMODE_MASK) <<
		    VPFE_SYN_FLDMODE_SHIFT) | VPFE_VDHDEN_ENABLE |
		    VPFE_WEN_ENABLE | VPFE_DATA_PACK_ENABLE);

	/* setup BT.656 sync mode */
	if (params->bt656_enable) {
		vpfe_reg_write(ccdc, VPFE_REC656IF_BT656_EN, VPFE_REC656IF);

		/*
		 * configure the FID, VD, HD pin polarity,
		 * fld,hd pol positive, vd negative, 8-bit data
		 */
		syn_mode |= VPFE_SYN_MODE_VD_POL_NEGATIVE;
		if (ccdc->ccdc_cfg.if_type == VPFE_BT656_10BIT)
			syn_mode |= VPFE_SYN_MODE_10BITS;
		else
			syn_mode |= VPFE_SYN_MODE_8BITS;
	} else {
		/* y/c external sync mode */
		syn_mode |= (((params->fid_pol & VPFE_FID_POL_MASK) <<
			     VPFE_FID_POL_SHIFT) |
			     ((params->hd_pol & VPFE_HD_POL_MASK) <<
			     VPFE_HD_POL_SHIFT) |
			     ((params->vd_pol & VPFE_VD_POL_MASK) <<
			     VPFE_VD_POL_SHIFT));
	}
	vpfe_reg_write(ccdc, syn_mode, VPFE_SYNMODE);

	/* configure video window */
	vpfe_ccdc_setwin(ccdc, &params->win,
			 params->frm_fmt, params->bytesperpixel);

	/*
	 * configure the order of y cb cr in SDRAM, and disable latch
	 * internal register on vsync
	 */
	if (ccdc->ccdc_cfg.if_type == VPFE_BT656_10BIT)
		vpfe_reg_write(ccdc,
			       (params->pix_order << VPFE_CCDCFG_Y8POS_SHIFT) |
			       VPFE_LATCH_ON_VSYNC_DISABLE |
			       VPFE_CCDCFG_BW656_10BIT, VPFE_CCDCFG);
	else
		vpfe_reg_write(ccdc,
			       (params->pix_order << VPFE_CCDCFG_Y8POS_SHIFT) |
			       VPFE_LATCH_ON_VSYNC_DISABLE, VPFE_CCDCFG);

	/*
	 * configure the horizontal line offset. This should be a
	 * on 32 byte boundary. So clear LSB 5 bits
	 */
	vpfe_reg_write(ccdc, params->bytesperline, VPFE_HSIZE_OFF);

	/* configure the memory line offset */
	if (params->buf_type == CCDC_BUFTYPE_FLD_INTERLEAVED)
		/* two fields are interleaved in memory */
		vpfe_reg_write(ccdc, VPFE_SDOFST_FIELD_INTERLEAVED,
			       VPFE_SDOFST);
}

static void
vpfe_ccdc_config_black_clamp(struct vpfe_ccdc *ccdc,
			     struct vpfe_ccdc_black_clamp *bclamp)
{
	u32 val;

	if (!bclamp->enable) {
		/* configure DCSub */
		val = (bclamp->dc_sub) & VPFE_BLK_DC_SUB_MASK;
		vpfe_reg_write(ccdc, val, VPFE_DCSUB);
		vpfe_reg_write(ccdc, VPFE_CLAMP_DEFAULT_VAL, VPFE_CLAMP);
		return;
	}
	/*
	 * Configure gain,  Start pixel, No of line to be avg,
	 * No of pixel/line to be avg, & Enable the Black clamping
	 */
	val = ((bclamp->sgain & VPFE_BLK_SGAIN_MASK) |
	       ((bclamp->start_pixel & VPFE_BLK_ST_PXL_MASK) <<
		VPFE_BLK_ST_PXL_SHIFT) |
	       ((bclamp->sample_ln & VPFE_BLK_SAMPLE_LINE_MASK) <<
		VPFE_BLK_SAMPLE_LINE_SHIFT) |
	       ((bclamp->sample_pixel & VPFE_BLK_SAMPLE_LN_MASK) <<
		VPFE_BLK_SAMPLE_LN_SHIFT) | VPFE_BLK_CLAMP_ENABLE);
	vpfe_reg_write(ccdc, val, VPFE_CLAMP);
	/* If Black clamping is enable then make dcsub 0 */
	vpfe_reg_write(ccdc, VPFE_DCSUB_DEFAULT_VAL, VPFE_DCSUB);
}

static void
vpfe_ccdc_config_black_compense(struct vpfe_ccdc *ccdc,
				struct vpfe_ccdc_black_compensation *bcomp)
{
	u32 val;

	val = ((bcomp->b & VPFE_BLK_COMP_MASK) |
	      ((bcomp->gb & VPFE_BLK_COMP_MASK) <<
	       VPFE_BLK_COMP_GB_COMP_SHIFT) |
	      ((bcomp->gr & VPFE_BLK_COMP_MASK) <<
	       VPFE_BLK_COMP_GR_COMP_SHIFT) |
	      ((bcomp->r & VPFE_BLK_COMP_MASK) <<
	       VPFE_BLK_COMP_R_COMP_SHIFT));
	vpfe_reg_write(ccdc, val, VPFE_BLKCMP);
}

/*
 * vpfe_ccdc_config_raw()
 * This function will configure CCDC for Raw capture mode
 */
static void vpfe_ccdc_config_raw(struct vpfe_ccdc *ccdc)
{
	struct vpfe_device *vpfe = to_vpfe(ccdc);
	struct vpfe_ccdc_config_params_raw *config_params =
				&ccdc->ccdc_cfg.bayer.config_params;
	struct ccdc_params_raw *params = &ccdc->ccdc_cfg.bayer;
	unsigned int syn_mode;
	unsigned int val;

	/* Reset CCDC */
	vpfe_ccdc_restore_defaults(ccdc);

	/* Disable latching function registers on VSYNC  */
	vpfe_reg_write(ccdc, VPFE_LATCH_ON_VSYNC_DISABLE, VPFE_CCDCFG);

	/*
	 * Configure the vertical sync polarity(SYN_MODE.VDPOL),
	 * horizontal sync polarity (SYN_MODE.HDPOL), frame id polarity
	 * (SYN_MODE.FLDPOL), frame format(progressive or interlace),
	 * data size(SYNMODE.DATSIZ), &pixel format (Input mode), output
	 * SDRAM, enable internal timing generator
	 */
	syn_mode = (((params->vd_pol & VPFE_VD_POL_MASK) << VPFE_VD_POL_SHIFT) |
		   ((params->hd_pol & VPFE_HD_POL_MASK) << VPFE_HD_POL_SHIFT) |
		   ((params->fid_pol & VPFE_FID_POL_MASK) <<
		   VPFE_FID_POL_SHIFT) | ((params->frm_fmt &
		   VPFE_FRM_FMT_MASK) << VPFE_FRM_FMT_SHIFT) |
		   ((config_params->data_sz & VPFE_DATA_SZ_MASK) <<
		   VPFE_DATA_SZ_SHIFT) | ((params->pix_fmt &
		   VPFE_PIX_FMT_MASK) << VPFE_PIX_FMT_SHIFT) |
		   VPFE_WEN_ENABLE | VPFE_VDHDEN_ENABLE);

	/* Enable and configure aLaw register if needed */
	if (config_params->alaw.enable) {
		val = ((config_params->alaw.gamma_wd &
		      VPFE_ALAW_GAMMA_WD_MASK) | VPFE_ALAW_ENABLE);
		vpfe_reg_write(ccdc, val, VPFE_ALAW);
		vpfe_dbg(3, vpfe, "\nWriting 0x%x to ALAW...\n", val);
	}

	/* Configure video window */
	vpfe_ccdc_setwin(ccdc, &params->win, params->frm_fmt,
			 params->bytesperpixel);

	/* Configure Black Clamp */
	vpfe_ccdc_config_black_clamp(ccdc, &config_params->blk_clamp);

	/* Configure Black level compensation */
	vpfe_ccdc_config_black_compense(ccdc, &config_params->blk_comp);

	/* If data size is 8 bit then pack the data */
	if ((config_params->data_sz == VPFE_CCDC_DATA_8BITS) ||
	    config_params->alaw.enable)
		syn_mode |= VPFE_DATA_PACK_ENABLE;

	/*
	 * Configure Horizontal offset register. If pack 8 is enabled then
	 * 1 pixel will take 1 byte
	 */
	vpfe_reg_write(ccdc, params->bytesperline, VPFE_HSIZE_OFF);

	vpfe_dbg(3, vpfe, "Writing %d (%x) to HSIZE_OFF\n",
		params->bytesperline, params->bytesperline);

	/* Set value for SDOFST */
	if (params->frm_fmt == CCDC_FRMFMT_INTERLACED) {
		if (params->image_invert_enable) {
			/* For interlace inverse mode */
			vpfe_reg_write(ccdc, VPFE_INTERLACED_IMAGE_INVERT,
				   VPFE_SDOFST);
		} else {
			/* For interlace non inverse mode */
			vpfe_reg_write(ccdc, VPFE_INTERLACED_NO_IMAGE_INVERT,
				   VPFE_SDOFST);
		}
	} else if (params->frm_fmt == CCDC_FRMFMT_PROGRESSIVE) {
		vpfe_reg_write(ccdc, VPFE_PROGRESSIVE_NO_IMAGE_INVERT,
			   VPFE_SDOFST);
	}

	vpfe_reg_write(ccdc, syn_mode, VPFE_SYNMODE);

	vpfe_reg_dump(ccdc);
}

static inline int
vpfe_ccdc_set_buftype(struct vpfe_ccdc *ccdc,
		      enum ccdc_buftype buf_type)
{
	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER)
		ccdc->ccdc_cfg.bayer.buf_type = buf_type;
	else
		ccdc->ccdc_cfg.ycbcr.buf_type = buf_type;

	return 0;
}

static inline enum ccdc_buftype vpfe_ccdc_get_buftype(struct vpfe_ccdc *ccdc)
{
	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER)
		return ccdc->ccdc_cfg.bayer.buf_type;

	return ccdc->ccdc_cfg.ycbcr.buf_type;
}

static int vpfe_ccdc_set_pixel_format(struct vpfe_ccdc *ccdc, u32 pixfmt)
{
	struct vpfe_device *vpfe = to_vpfe(ccdc);

	vpfe_dbg(1, vpfe, "%s: if_type: %d, pixfmt:%s\n",
		 __func__, ccdc->ccdc_cfg.if_type, print_fourcc(pixfmt));

	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER) {
		ccdc->ccdc_cfg.bayer.pix_fmt = CCDC_PIXFMT_RAW;
		/*
		 * Need to clear it in case it was left on
		 * after the last capture.
		 */
		ccdc->ccdc_cfg.bayer.config_params.alaw.enable = 0;

		switch (pixfmt) {
		case V4L2_PIX_FMT_SBGGR8:
			ccdc->ccdc_cfg.bayer.config_params.alaw.enable = 1;
			break;

		case V4L2_PIX_FMT_YUYV:
		case V4L2_PIX_FMT_UYVY:
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_RGB565X:
			break;

		case V4L2_PIX_FMT_SBGGR16:
		default:
			return -EINVAL;
		}
	} else {
		switch (pixfmt) {
		case V4L2_PIX_FMT_YUYV:
			ccdc->ccdc_cfg.ycbcr.pix_order = CCDC_PIXORDER_YCBYCR;
			break;

		case V4L2_PIX_FMT_UYVY:
			ccdc->ccdc_cfg.ycbcr.pix_order = CCDC_PIXORDER_CBYCRY;
			break;

		default:
			return -EINVAL;
		}
	}

	return 0;
}

static u32 vpfe_ccdc_get_pixel_format(struct vpfe_ccdc *ccdc)
{
	u32 pixfmt;

	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER) {
		pixfmt = V4L2_PIX_FMT_YUYV;
	} else {
		if (ccdc->ccdc_cfg.ycbcr.pix_order == CCDC_PIXORDER_YCBYCR)
			pixfmt = V4L2_PIX_FMT_YUYV;
		else
			pixfmt = V4L2_PIX_FMT_UYVY;
	}

	return pixfmt;
}

static int
vpfe_ccdc_set_image_window(struct vpfe_ccdc *ccdc,
			   struct v4l2_rect *win, unsigned int bpp)
{
	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER) {
		ccdc->ccdc_cfg.bayer.win = *win;
		ccdc->ccdc_cfg.bayer.bytesperpixel = bpp;
		ccdc->ccdc_cfg.bayer.bytesperline = ALIGN(win->width * bpp, 32);
	} else {
		ccdc->ccdc_cfg.ycbcr.win = *win;
		ccdc->ccdc_cfg.ycbcr.bytesperpixel = bpp;
		ccdc->ccdc_cfg.ycbcr.bytesperline = ALIGN(win->width * bpp, 32);
	}

	return 0;
}

static inline void
vpfe_ccdc_get_image_window(struct vpfe_ccdc *ccdc,
			   struct v4l2_rect *win)
{
	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER)
		*win = ccdc->ccdc_cfg.bayer.win;
	else
		*win = ccdc->ccdc_cfg.ycbcr.win;
}

static inline unsigned int vpfe_ccdc_get_line_length(struct vpfe_ccdc *ccdc)
{
	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER)
		return ccdc->ccdc_cfg.bayer.bytesperline;

	return ccdc->ccdc_cfg.ycbcr.bytesperline;
}

static inline int
vpfe_ccdc_set_frame_format(struct vpfe_ccdc *ccdc,
			   enum ccdc_frmfmt frm_fmt)
{
	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER)
		ccdc->ccdc_cfg.bayer.frm_fmt = frm_fmt;
	else
		ccdc->ccdc_cfg.ycbcr.frm_fmt = frm_fmt;

	return 0;
}

static inline enum ccdc_frmfmt
vpfe_ccdc_get_frame_format(struct vpfe_ccdc *ccdc)
{
	if (ccdc->ccdc_cfg.if_type == VPFE_RAW_BAYER)
		return ccdc->ccdc_cfg.bayer.frm_fmt;

	return ccdc->ccdc_cfg.ycbcr.frm_fmt;
}

static inline int vpfe_ccdc_getfid(struct vpfe_ccdc *ccdc)
{
	return (vpfe_reg_read(ccdc, VPFE_SYNMODE) >> 15) & 1;
}

static inline void vpfe_set_sdr_addr(struct vpfe_ccdc *ccdc, unsigned long addr)
{
	vpfe_reg_write(ccdc, addr & 0xffffffe0, VPFE_SDR_ADDR);
}

static int vpfe_ccdc_set_hw_if_params(struct vpfe_ccdc *ccdc,
				      struct vpfe_hw_if_param *params)
{
	struct vpfe_device *vpfe = to_vpfe(ccdc);

	ccdc->ccdc_cfg.if_type = params->if_type;

	switch (params->if_type) {
	case VPFE_BT656:
	case VPFE_YCBCR_SYNC_16:
	case VPFE_YCBCR_SYNC_8:
	case VPFE_BT656_10BIT:
		ccdc->ccdc_cfg.ycbcr.vd_pol = params->vdpol;
		ccdc->ccdc_cfg.ycbcr.hd_pol = params->hdpol;
		break;

	case VPFE_RAW_BAYER:
		ccdc->ccdc_cfg.bayer.vd_pol = params->vdpol;
		ccdc->ccdc_cfg.bayer.hd_pol = params->hdpol;
		if (params->bus_width == 10)
			ccdc->ccdc_cfg.bayer.config_params.data_sz =
				VPFE_CCDC_DATA_10BITS;
		else
			ccdc->ccdc_cfg.bayer.config_params.data_sz =
				VPFE_CCDC_DATA_8BITS;
		vpfe_dbg(1, vpfe, "params.bus_width: %d\n",
			params->bus_width);
		vpfe_dbg(1, vpfe, "config_params.data_sz: %d\n",
			ccdc->ccdc_cfg.bayer.config_params.data_sz);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static void vpfe_clear_intr(struct vpfe_ccdc *ccdc, int vdint)
{
	unsigned int vpfe_int_status;

	vpfe_int_status = vpfe_reg_read(ccdc, VPFE_IRQ_STS);

	switch (vdint) {
	/* VD0 interrupt */
	case VPFE_VDINT0:
		vpfe_int_status &= ~VPFE_VDINT0;
		vpfe_int_status |= VPFE_VDINT0;
		break;

	/* VD1 interrupt */
	case VPFE_VDINT1:
		vpfe_int_status &= ~VPFE_VDINT1;
		vpfe_int_status |= VPFE_VDINT1;
		break;

	/* VD2 interrupt */
	case VPFE_VDINT2:
		vpfe_int_status &= ~VPFE_VDINT2;
		vpfe_int_status |= VPFE_VDINT2;
		break;

	/* Clear all interrupts */
	default:
		vpfe_int_status &= ~(VPFE_VDINT0 |
				VPFE_VDINT1 |
				VPFE_VDINT2);
		vpfe_int_status |= (VPFE_VDINT0 |
				VPFE_VDINT1 |
				VPFE_VDINT2);
		break;
	}
	/* Clear specific VDINT from the status register */
	vpfe_reg_write(ccdc, vpfe_int_status, VPFE_IRQ_STS);

	vpfe_int_status = vpfe_reg_read(ccdc, VPFE_IRQ_STS);

	/* Acknowledge that we are done with all interrupts */
	vpfe_reg_write(ccdc, 1, VPFE_IRQ_EOI);
}

static void vpfe_ccdc_config_defaults(struct vpfe_ccdc *ccdc)
{
	ccdc->ccdc_cfg.if_type = VPFE_RAW_BAYER;

	ccdc->ccdc_cfg.ycbcr.pix_fmt = CCDC_PIXFMT_YCBCR_8BIT;
	ccdc->ccdc_cfg.ycbcr.frm_fmt = CCDC_FRMFMT_INTERLACED;
	ccdc->ccdc_cfg.ycbcr.fid_pol = VPFE_PINPOL_POSITIVE;
	ccdc->ccdc_cfg.ycbcr.vd_pol = VPFE_PINPOL_POSITIVE;
	ccdc->ccdc_cfg.ycbcr.hd_pol = VPFE_PINPOL_POSITIVE;
	ccdc->ccdc_cfg.ycbcr.pix_order = CCDC_PIXORDER_CBYCRY;
	ccdc->ccdc_cfg.ycbcr.buf_type = CCDC_BUFTYPE_FLD_INTERLEAVED;

	ccdc->ccdc_cfg.ycbcr.win.left = 0;
	ccdc->ccdc_cfg.ycbcr.win.top = 0;
	ccdc->ccdc_cfg.ycbcr.win.width = 720;
	ccdc->ccdc_cfg.ycbcr.win.height = 576;
	ccdc->ccdc_cfg.ycbcr.bt656_enable = 1;

	ccdc->ccdc_cfg.bayer.pix_fmt = CCDC_PIXFMT_RAW;
	ccdc->ccdc_cfg.bayer.frm_fmt = CCDC_FRMFMT_PROGRESSIVE;
	ccdc->ccdc_cfg.bayer.fid_pol = VPFE_PINPOL_POSITIVE;
	ccdc->ccdc_cfg.bayer.vd_pol = VPFE_PINPOL_POSITIVE;
	ccdc->ccdc_cfg.bayer.hd_pol = VPFE_PINPOL_POSITIVE;

	ccdc->ccdc_cfg.bayer.win.left = 0;
	ccdc->ccdc_cfg.bayer.win.top = 0;
	ccdc->ccdc_cfg.bayer.win.width = 800;
	ccdc->ccdc_cfg.bayer.win.height = 600;
	ccdc->ccdc_cfg.bayer.config_params.data_sz = VPFE_CCDC_DATA_8BITS;
	ccdc->ccdc_cfg.bayer.config_params.alaw.gamma_wd =
						VPFE_CCDC_GAMMA_BITS_09_0;
}

/*
 * vpfe_get_ccdc_image_format - Get image parameters based on CCDC settings
 */
static int vpfe_get_ccdc_image_format(struct vpfe_device *vpfe,
				      struct v4l2_format *f)
{
	struct v4l2_rect image_win;
	enum ccdc_buftype buf_type;
	enum ccdc_frmfmt frm_fmt;

	memset(f, 0, sizeof(*f));
	f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vpfe_ccdc_get_image_window(&vpfe->ccdc, &image_win);
	f->fmt.pix.width = image_win.width;
	f->fmt.pix.height = image_win.height;
	f->fmt.pix.bytesperline = vpfe_ccdc_get_line_length(&vpfe->ccdc);
	f->fmt.pix.sizeimage = f->fmt.pix.bytesperline *
				f->fmt.pix.height;
	buf_type = vpfe_ccdc_get_buftype(&vpfe->ccdc);
	f->fmt.pix.pixelformat = vpfe_ccdc_get_pixel_format(&vpfe->ccdc);
	frm_fmt = vpfe_ccdc_get_frame_format(&vpfe->ccdc);

	if (frm_fmt == CCDC_FRMFMT_PROGRESSIVE) {
		f->fmt.pix.field = V4L2_FIELD_NONE;
	} else if (frm_fmt == CCDC_FRMFMT_INTERLACED) {
		if (buf_type == CCDC_BUFTYPE_FLD_INTERLEAVED) {
			f->fmt.pix.field = V4L2_FIELD_INTERLACED;
		 } else if (buf_type == CCDC_BUFTYPE_FLD_SEPARATED) {
			f->fmt.pix.field = V4L2_FIELD_SEQ_TB;
		} else {
			vpfe_err(vpfe, "Invalid buf_type\n");
			return -EINVAL;
		}
	} else {
		vpfe_err(vpfe, "Invalid frm_fmt\n");
		return -EINVAL;
	}
	return 0;
}

static int vpfe_config_ccdc_image_format(struct vpfe_device *vpfe)
{
	enum ccdc_frmfmt frm_fmt = CCDC_FRMFMT_INTERLACED;
	u32 bpp;
	int ret = 0;

	vpfe_dbg(1, vpfe, "pixelformat: %s\n",
		print_fourcc(vpfe->fmt.fmt.pix.pixelformat));

	if (vpfe_ccdc_set_pixel_format(&vpfe->ccdc,
			vpfe->fmt.fmt.pix.pixelformat) < 0) {
		vpfe_err(vpfe, "couldn't set pix format in ccdc\n");
		return -EINVAL;
	}

	/* configure the image window */
	bpp = __get_bytesperpixel(vpfe, vpfe->current_vpfe_fmt);
	vpfe_ccdc_set_image_window(&vpfe->ccdc, &vpfe->crop, bpp);

	switch (vpfe->fmt.fmt.pix.field) {
	case V4L2_FIELD_INTERLACED:
		/* do nothing, since it is default */
		ret = vpfe_ccdc_set_buftype(
				&vpfe->ccdc,
				CCDC_BUFTYPE_FLD_INTERLEAVED);
		break;

	case V4L2_FIELD_NONE:
		frm_fmt = CCDC_FRMFMT_PROGRESSIVE;
		/* buffer type only applicable for interlaced scan */
		break;

	case V4L2_FIELD_SEQ_TB:
		ret = vpfe_ccdc_set_buftype(
				&vpfe->ccdc,
				CCDC_BUFTYPE_FLD_SEPARATED);
		break;

	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return vpfe_ccdc_set_frame_format(&vpfe->ccdc, frm_fmt);
}

/*
 * vpfe_config_image_format()
 * For a given standard, this functions sets up the default
 * pix format & crop values in the vpfe device and ccdc.  It first
 * starts with defaults based values from the standard table.
 * It then checks if sub device supports get_fmt and then override the
 * values based on that.Sets crop values to match with scan resolution
 * starting at 0,0. It calls vpfe_config_ccdc_image_format() set the
 * values in ccdc
 */
static int vpfe_config_image_format(struct vpfe_device *vpfe,
				    v4l2_std_id std_id)
{
	struct vpfe_fmt *fmt;
	struct v4l2_mbus_framefmt mbus_fmt;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(vpfe_standards); i++) {
		if (vpfe_standards[i].std_id & std_id) {
			vpfe->std_info.active_pixels =
					vpfe_standards[i].width;
			vpfe->std_info.active_lines =
					vpfe_standards[i].height;
			vpfe->std_info.frame_format =
					vpfe_standards[i].frame_format;
			vpfe->std_index = i;

			break;
		}
	}

	if (i ==  ARRAY_SIZE(vpfe_standards)) {
		vpfe_err(vpfe, "standard not supported\n");
		return -EINVAL;
	}

	ret = __subdev_get_format(vpfe, &mbus_fmt);
	if (ret)
		return ret;

	fmt = find_format_by_code(vpfe, mbus_fmt.code);
	if (!fmt) {
		vpfe_dbg(3, vpfe, "mbus code format (0x%08x) not found.\n",
			 mbus_fmt.code);
		return -EINVAL;
	}

	/* Save current subdev format */
	v4l2_fill_pix_format(&vpfe->fmt.fmt.pix, &mbus_fmt);
	vpfe->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vpfe->fmt.fmt.pix.pixelformat = fmt->fourcc;
	vpfe_calc_format_size(vpfe, fmt, &vpfe->fmt);
	vpfe->current_vpfe_fmt = fmt;

	/* Update the crop window based on found values */
	vpfe->crop.top = 0;
	vpfe->crop.left = 0;
	vpfe->crop.width = mbus_fmt.width;
	vpfe->crop.height = mbus_fmt.height;

	return vpfe_config_ccdc_image_format(vpfe);
}

static int vpfe_initialize_device(struct vpfe_device *vpfe)
{
	struct vpfe_subdev_info *sdinfo;
	int ret;

	sdinfo = &vpfe->cfg->sub_devs[0];
	sdinfo->sd = vpfe->sd[0];
	vpfe->current_input = 0;
	vpfe->std_index = 0;
	/* Configure the default format information */
	ret = vpfe_config_image_format(vpfe,
				       vpfe_standards[vpfe->std_index].std_id);
	if (ret)
		return ret;

	pm_runtime_get_sync(vpfe->pdev);

	vpfe_config_enable(&vpfe->ccdc, 1);

	vpfe_ccdc_restore_defaults(&vpfe->ccdc);

	/* Clear all VPFE interrupts */
	vpfe_clear_intr(&vpfe->ccdc, -1);

	return ret;
}

/*
 * vpfe_release : This function is based on the vb2_fop_release
 * helper function.
 * It has been augmented to handle module power management,
 * by disabling/enabling h/w module fcntl clock when necessary.
 */
static int vpfe_release(struct file *file)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	bool fh_singular;
	int ret;

	mutex_lock(&vpfe->lock);

	/* Save the singular status before we call the clean-up helper */
	fh_singular = v4l2_fh_is_singular_file(file);

	/* the release helper will cleanup any on-going streaming */
	ret = _vb2_fop_release(file, NULL);

	/*
	 * If this was the last open file.
	 * Then de-initialize hw module.
	 */
	if (fh_singular)
		vpfe_ccdc_close(&vpfe->ccdc, vpfe->pdev);

	mutex_unlock(&vpfe->lock);

	return ret;
}

/*
 * vpfe_open : This function is based on the v4l2_fh_open helper function.
 * It has been augmented to handle module power management,
 * by disabling/enabling h/w module fcntl clock when necessary.
 */
static int vpfe_open(struct file *file)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	int ret;

	mutex_lock(&vpfe->lock);

	ret = v4l2_fh_open(file);
	if (ret) {
		vpfe_err(vpfe, "v4l2_fh_open failed\n");
		goto unlock;
	}

	if (!v4l2_fh_is_singular_file(file))
		goto unlock;

	if (vpfe_initialize_device(vpfe)) {
		v4l2_fh_release(file);
		ret = -ENODEV;
	}

unlock:
	mutex_unlock(&vpfe->lock);
	return ret;
}

/**
 * vpfe_schedule_next_buffer: set next buffer address for capture
 * @vpfe : ptr to vpfe device
 *
 * This function will get next buffer from the dma queue and
 * set the buffer address in the vpfe register for capture.
 * the buffer is marked active
 */
static void vpfe_schedule_next_buffer(struct vpfe_device *vpfe)
{
	dma_addr_t addr;

	spin_lock(&vpfe->dma_queue_lock);
	if (list_empty(&vpfe->dma_queue)) {
		spin_unlock(&vpfe->dma_queue_lock);
		return;
	}

	vpfe->next_frm = list_entry(vpfe->dma_queue.next,
				    struct vpfe_cap_buffer, list);
	list_del(&vpfe->next_frm->list);
	spin_unlock(&vpfe->dma_queue_lock);

	addr = vb2_dma_contig_plane_dma_addr(&vpfe->next_frm->vb.vb2_buf, 0);
	vpfe_set_sdr_addr(&vpfe->ccdc, addr);
}

static inline void vpfe_schedule_bottom_field(struct vpfe_device *vpfe)
{
	dma_addr_t addr;

	addr = vb2_dma_contig_plane_dma_addr(&vpfe->next_frm->vb.vb2_buf, 0) +
					vpfe->field_off;

	vpfe_set_sdr_addr(&vpfe->ccdc, addr);
}

/*
 * vpfe_process_buffer_complete: process a completed buffer
 * @vpfe : ptr to vpfe device
 *
 * This function time stamp the buffer and mark it as DONE. It also
 * wake up any process waiting on the QUEUE and set the next buffer
 * as current
 */
static inline void vpfe_process_buffer_complete(struct vpfe_device *vpfe)
{
	vpfe->cur_frm->vb.vb2_buf.timestamp = ktime_get_ns();
	vpfe->cur_frm->vb.field = vpfe->fmt.fmt.pix.field;
	vpfe->cur_frm->vb.sequence = vpfe->sequence++;
	vb2_buffer_done(&vpfe->cur_frm->vb.vb2_buf, VB2_BUF_STATE_DONE);
	vpfe->cur_frm = vpfe->next_frm;
}

static void vpfe_handle_interlaced_irq(struct vpfe_device *vpfe,
				       enum v4l2_field field)
{
	int fid;

	/* interlaced or TB capture check which field
	 * we are in hardware
	 */
	fid = vpfe_ccdc_getfid(&vpfe->ccdc);

	/* switch the software maintained field id */
	vpfe->field ^= 1;
	if (fid == vpfe->field) {
		/* we are in-sync here,continue */
		if (fid == 0) {
			/*
			 * One frame is just being captured. If the
			 * next frame is available, release the
			 * current frame and move on
			 */
			if (vpfe->cur_frm != vpfe->next_frm)
				vpfe_process_buffer_complete(vpfe);

			if (vpfe->stopping)
				return;

			/*
			 * based on whether the two fields are stored
			 * interleave or separately in memory,
			 * reconfigure the CCDC memory address
			 */
			if (field == V4L2_FIELD_SEQ_TB)
				vpfe_schedule_bottom_field(vpfe);
		} else {
			/*
			 * if one field is just being captured configure
			 * the next frame get the next frame from the empty
			 * queue if no frame is available hold on to the
			 * current buffer
			 */
			if (vpfe->cur_frm == vpfe->next_frm)
				vpfe_schedule_next_buffer(vpfe);
		}
	} else if (fid == 0) {
		/*
		 * out of sync. Recover from any hardware out-of-sync.
		 * May loose one frame
		 */
		vpfe->field = fid;
	}
}

/*
 * vpfe_isr : ISR handler for vpfe capture (VINT0)
 * @irq: irq number
 * @dev_id: dev_id ptr
 *
 * It changes status of the captured buffer, takes next buffer from the queue
 * and sets its address in VPFE registers
 */
static irqreturn_t vpfe_isr(int irq, void *dev)
{
	struct vpfe_device *vpfe = (struct vpfe_device *)dev;
	enum v4l2_field field = vpfe->fmt.fmt.pix.field;
	int intr_status, stopping = vpfe->stopping;

	intr_status = vpfe_reg_read(&vpfe->ccdc, VPFE_IRQ_STS);

	if (intr_status & VPFE_VDINT0) {
		if (field == V4L2_FIELD_NONE) {
			if (vpfe->cur_frm != vpfe->next_frm)
				vpfe_process_buffer_complete(vpfe);
		} else {
			vpfe_handle_interlaced_irq(vpfe, field);
		}
		if (stopping) {
			vpfe->stopping = false;
			complete(&vpfe->capture_stop);
		}
	}

	if (intr_status & VPFE_VDINT1 && !stopping) {
		if (field == V4L2_FIELD_NONE &&
		    vpfe->cur_frm == vpfe->next_frm)
			vpfe_schedule_next_buffer(vpfe);
	}

	vpfe_clear_intr(&vpfe->ccdc, intr_status);

	return IRQ_HANDLED;
}

static inline void vpfe_detach_irq(struct vpfe_device *vpfe)
{
	unsigned int intr = VPFE_VDINT0;
	enum ccdc_frmfmt frame_format;

	frame_format = vpfe_ccdc_get_frame_format(&vpfe->ccdc);
	if (frame_format == CCDC_FRMFMT_PROGRESSIVE)
		intr |= VPFE_VDINT1;

	vpfe_reg_write(&vpfe->ccdc, intr, VPFE_IRQ_EN_CLR);
}

static inline void vpfe_attach_irq(struct vpfe_device *vpfe)
{
	unsigned int intr = VPFE_VDINT0;
	enum ccdc_frmfmt frame_format;

	frame_format = vpfe_ccdc_get_frame_format(&vpfe->ccdc);
	if (frame_format == CCDC_FRMFMT_PROGRESSIVE)
		intr |= VPFE_VDINT1;

	vpfe_reg_write(&vpfe->ccdc, intr, VPFE_IRQ_EN_SET);
}

static int vpfe_querycap(struct file *file, void  *priv,
			 struct v4l2_capability *cap)
{
	struct vpfe_device *vpfe = video_drvdata(file);

	strscpy(cap->driver, VPFE_MODULE_NAME, sizeof(cap->driver));
	strscpy(cap->card, "TI AM437x VPFE", sizeof(cap->card));
	snprintf(cap->bus_info, sizeof(cap->bus_info),
			"platform:%s", vpfe->v4l2_dev.name);
	return 0;
}

/* get the format set at output pad of the adjacent subdev */
static int __subdev_get_format(struct vpfe_device *vpfe,
			       struct v4l2_mbus_framefmt *fmt)
{
	struct v4l2_subdev *sd = vpfe->current_subdev->sd;
	struct v4l2_subdev_format sd_fmt;
	struct v4l2_mbus_framefmt *mbus_fmt = &sd_fmt.format;
	int ret;

	sd_fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	sd_fmt.pad = 0;

	ret = v4l2_subdev_call(sd, pad, get_fmt, NULL, &sd_fmt);
	if (ret)
		return ret;

	*fmt = *mbus_fmt;

	vpfe_dbg(1, vpfe, "%s: %dx%d code:%04X\n", __func__,
		 fmt->width, fmt->height, fmt->code);

	return 0;
}

/* set the format at output pad of the adjacent subdev */
static int __subdev_set_format(struct vpfe_device *vpfe,
			       struct v4l2_mbus_framefmt *fmt)
{
	struct v4l2_subdev *sd = vpfe->current_subdev->sd;
	struct v4l2_subdev_format sd_fmt;
	struct v4l2_mbus_framefmt *mbus_fmt = &sd_fmt.format;
	int ret;

	sd_fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	sd_fmt.pad = 0;
	*mbus_fmt = *fmt;

	ret = v4l2_subdev_call(sd, pad, set_fmt, NULL, &sd_fmt);
	if (ret)
		return ret;

	vpfe_dbg(1, vpfe, "%s %dx%d code:%04X\n", __func__,
		 fmt->width, fmt->height, fmt->code);

	return 0;
}

static int vpfe_calc_format_size(struct vpfe_device *vpfe,
				 const struct vpfe_fmt *fmt,
				 struct v4l2_format *f)
{
	u32 bpp;

	if (!fmt) {
		vpfe_dbg(3, vpfe, "No vpfe_fmt provided!\n");
		return -EINVAL;
	}

	bpp = __get_bytesperpixel(vpfe, fmt);

	/* pitch should be 32 bytes aligned */
	f->fmt.pix.bytesperline = ALIGN(f->fmt.pix.width * bpp, 32);
	f->fmt.pix.sizeimage = f->fmt.pix.bytesperline *
			       f->fmt.pix.height;

	vpfe_dbg(3, vpfe, "%s: fourcc: %s size: %dx%d bpl:%d img_size:%d\n",
		 __func__, print_fourcc(f->fmt.pix.pixelformat),
		 f->fmt.pix.width, f->fmt.pix.height,
		 f->fmt.pix.bytesperline, f->fmt.pix.sizeimage);

	return 0;
}

static int vpfe_g_fmt(struct file *file, void *priv,
		      struct v4l2_format *fmt)
{
	struct vpfe_device *vpfe = video_drvdata(file);

	*fmt = vpfe->fmt;

	return 0;
}

static int vpfe_enum_fmt(struct file *file, void  *priv,
			 struct v4l2_fmtdesc *f)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct vpfe_subdev_info *sdinfo;
	struct vpfe_fmt *fmt;

	sdinfo = vpfe->current_subdev;
	if (!sdinfo->sd)
		return -EINVAL;

	if (f->index >= vpfe->num_active_fmt)
		return -EINVAL;

	fmt = vpfe->active_fmt[f->index];

	f->pixelformat = fmt->fourcc;

	vpfe_dbg(1, vpfe, "%s: mbus index: %d code: %x pixelformat: %s\n",
		 __func__, f->index, fmt->code, print_fourcc(fmt->fourcc));

	return 0;
}

static int vpfe_try_fmt(struct file *file, void *priv,
			struct v4l2_format *f)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct v4l2_subdev *sd = vpfe->current_subdev->sd;
	const struct vpfe_fmt *fmt;
	struct v4l2_subdev_frame_size_enum fse;
	int ret, found;

	fmt = find_format_by_pix(vpfe, f->fmt.pix.pixelformat);
	if (!fmt) {
		/* default to first entry */
		vpfe_dbg(3, vpfe, "Invalid pixel code: %x, default used instead\n",
			 f->fmt.pix.pixelformat);
		fmt = vpfe->active_fmt[0];
		f->fmt.pix.pixelformat = fmt->fourcc;
	}

	f->fmt.pix.field = vpfe->fmt.fmt.pix.field;

	/* check for/find a valid width/height */
	ret = 0;
	found = false;
	fse.pad = 0;
	fse.code = fmt->code;
	fse.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	for (fse.index = 0; ; fse.index++) {
		ret = v4l2_subdev_call(sd, pad, enum_frame_size,
				       NULL, &fse);
		if (ret)
			break;

		if (f->fmt.pix.width == fse.max_width &&
		    f->fmt.pix.height == fse.max_height) {
			found = true;
			break;
		} else if (f->fmt.pix.width >= fse.min_width &&
			   f->fmt.pix.width <= fse.max_width &&
			   f->fmt.pix.height >= fse.min_height &&
			   f->fmt.pix.height <= fse.max_height) {
			found = true;
			break;
		}
	}

	if (!found) {
		/* use existing values as default */
		f->fmt.pix.width = vpfe->fmt.fmt.pix.width;
		f->fmt.pix.height =  vpfe->fmt.fmt.pix.height;
	}

	/*
	 * Use current colorspace for now, it will get
	 * updated properly during s_fmt
	 */
	f->fmt.pix.colorspace = vpfe->fmt.fmt.pix.colorspace;
	return vpfe_calc_format_size(vpfe, fmt, f);
}

static int vpfe_s_fmt(struct file *file, void *priv,
		      struct v4l2_format *fmt)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct vpfe_fmt *f;
	struct v4l2_mbus_framefmt mbus_fmt;
	int ret;

	/* If streaming is started, return error */
	if (vb2_is_busy(&vpfe->buffer_queue)) {
		vpfe_err(vpfe, "%s device busy\n", __func__);
		return -EBUSY;
	}

	ret = vpfe_try_fmt(file, priv, fmt);
	if (ret < 0)
		return ret;

	f = find_format_by_pix(vpfe, fmt->fmt.pix.pixelformat);

	v4l2_fill_mbus_format(&mbus_fmt, &fmt->fmt.pix, f->code);

	ret = __subdev_set_format(vpfe, &mbus_fmt);
	if (ret)
		return ret;

	/* Just double check nothing has gone wrong */
	if (mbus_fmt.code != f->code) {
		vpfe_dbg(3, vpfe,
			 "%s subdev changed format on us, this should not happen\n",
			 __func__);
		return -EINVAL;
	}

	v4l2_fill_pix_format(&vpfe->fmt.fmt.pix, &mbus_fmt);
	vpfe->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vpfe->fmt.fmt.pix.pixelformat  = f->fourcc;
	vpfe_calc_format_size(vpfe, f, &vpfe->fmt);
	*fmt = vpfe->fmt;
	vpfe->current_vpfe_fmt = f;

	/* Update the crop window based on found values */
	vpfe->crop.width = fmt->fmt.pix.width;
	vpfe->crop.height = fmt->fmt.pix.height;

	/* set image capture parameters in the ccdc */
	return vpfe_config_ccdc_image_format(vpfe);
}

static int vpfe_enum_size(struct file *file, void  *priv,
			  struct v4l2_frmsizeenum *fsize)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct v4l2_subdev_frame_size_enum fse;
	struct v4l2_subdev *sd = vpfe->current_subdev->sd;
	struct vpfe_fmt *fmt;
	int ret;

	/* check for valid format */
	fmt = find_format_by_pix(vpfe, fsize->pixel_format);
	if (!fmt) {
		vpfe_dbg(3, vpfe, "Invalid pixel code: %x\n",
			 fsize->pixel_format);
		return -EINVAL;
	}

	memset(fsize->reserved, 0x0, sizeof(fsize->reserved));

	memset(&fse, 0x0, sizeof(fse));
	fse.index = fsize->index;
	fse.pad = 0;
	fse.code = fmt->code;
	fse.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	ret = v4l2_subdev_call(sd, pad, enum_frame_size, NULL, &fse);
	if (ret)
		return ret;

	vpfe_dbg(1, vpfe, "%s: index: %d code: %x W:[%d,%d] H:[%d,%d]\n",
		 __func__, fse.index, fse.code, fse.min_width, fse.max_width,
		 fse.min_height, fse.max_height);

	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete.width = fse.max_width;
	fsize->discrete.height = fse.max_height;

	vpfe_dbg(1, vpfe, "%s: index: %d pixformat: %s size: %dx%d\n",
		 __func__, fsize->index, print_fourcc(fsize->pixel_format),
		 fsize->discrete.width, fsize->discrete.height);

	return 0;
}

/*
 * vpfe_get_subdev_input_index - Get subdev index and subdev input index for a
 * given app input index
 */
static int
vpfe_get_subdev_input_index(struct vpfe_device *vpfe,
			    int *subdev_index,
			    int *subdev_input_index,
			    int app_input_index)
{
	int i, j = 0;

	for (i = 0; i < ARRAY_SIZE(vpfe->cfg->asd); i++) {
		if (app_input_index < (j + 1)) {
			*subdev_index = i;
			*subdev_input_index = app_input_index - j;
			return 0;
		}
		j++;
	}
	return -EINVAL;
}

/*
 * vpfe_get_app_input - Get app input index for a given subdev input index
 * driver stores the input index of the current sub device and translate it
 * when application request the current input
 */
static int vpfe_get_app_input_index(struct vpfe_device *vpfe,
				    int *app_input_index)
{
	struct vpfe_config *cfg = vpfe->cfg;
	struct vpfe_subdev_info *sdinfo;
	struct i2c_client *client;
	struct i2c_client *curr_client;
	int i, j = 0;

	curr_client = v4l2_get_subdevdata(vpfe->current_subdev->sd);
	for (i = 0; i < ARRAY_SIZE(vpfe->cfg->asd); i++) {
		sdinfo = &cfg->sub_devs[i];
		client = v4l2_get_subdevdata(sdinfo->sd);
		if (client->addr == curr_client->addr &&
		    client->adapter->nr == curr_client->adapter->nr) {
			if (vpfe->current_input >= 1)
				return -1;
			*app_input_index = j + vpfe->current_input;
			return 0;
		}
		j++;
	}
	return -EINVAL;
}

static int vpfe_enum_input(struct file *file, void *priv,
			   struct v4l2_input *inp)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct vpfe_subdev_info *sdinfo;
	int subdev, index;

	if (vpfe_get_subdev_input_index(vpfe, &subdev, &index,
					inp->index) < 0) {
		vpfe_dbg(1, vpfe,
			"input information not found for the subdev\n");
		return -EINVAL;
	}
	sdinfo = &vpfe->cfg->sub_devs[subdev];
	*inp = sdinfo->inputs[index];

	return 0;
}

static int vpfe_g_input(struct file *file, void *priv, unsigned int *index)
{
	struct vpfe_device *vpfe = video_drvdata(file);

	return vpfe_get_app_input_index(vpfe, index);
}

/* Assumes caller is holding vpfe_dev->lock */
static int vpfe_set_input(struct vpfe_device *vpfe, unsigned int index)
{
	int subdev_index = 0, inp_index = 0;
	struct vpfe_subdev_info *sdinfo;
	struct vpfe_route *route;
	u32 input, output;
	int ret;

	/* If streaming is started, return error */
	if (vb2_is_busy(&vpfe->buffer_queue)) {
		vpfe_err(vpfe, "%s device busy\n", __func__);
		return -EBUSY;
	}
	ret = vpfe_get_subdev_input_index(vpfe,
					  &subdev_index,
					  &inp_index,
					  index);
	if (ret < 0) {
		vpfe_err(vpfe, "invalid input index: %d\n", index);
		goto get_out;
	}

	sdinfo = &vpfe->cfg->sub_devs[subdev_index];
	sdinfo->sd = vpfe->sd[subdev_index];
	route = &sdinfo->routes[inp_index];
	if (route && sdinfo->can_route) {
		input = route->input;
		output = route->output;
		if (sdinfo->sd) {
			ret = v4l2_subdev_call(sdinfo->sd, video,
					s_routing, input, output, 0);
			if (ret) {
				vpfe_err(vpfe, "s_routing failed\n");
				ret = -EINVAL;
				goto get_out;
			}
		}

	}

	vpfe->current_subdev = sdinfo;
	if (sdinfo->sd)
		vpfe->v4l2_dev.ctrl_handler = sdinfo->sd->ctrl_handler;
	vpfe->current_input = index;
	vpfe->std_index = 0;

	/* set the bus/interface parameter for the sub device in ccdc */
	ret = vpfe_ccdc_set_hw_if_params(&vpfe->ccdc, &sdinfo->vpfe_param);
	if (ret)
		return ret;

	/* set the default image parameters in the device */
	return vpfe_config_image_format(vpfe,
					vpfe_standards[vpfe->std_index].std_id);

get_out:
	return ret;
}

static int vpfe_s_input(struct file *file, void *priv, unsigned int index)
{
	struct vpfe_device *vpfe = video_drvdata(file);

	return vpfe_set_input(vpfe, index);
}

static int vpfe_querystd(struct file *file, void *priv, v4l2_std_id *std_id)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct vpfe_subdev_info *sdinfo;

	sdinfo = vpfe->current_subdev;
	if (!(sdinfo->inputs[0].capabilities & V4L2_IN_CAP_STD))
		return -ENODATA;

	/* Call querystd function of decoder device */
	return v4l2_device_call_until_err(&vpfe->v4l2_dev, sdinfo->grp_id,
					 video, querystd, std_id);
}

static int vpfe_s_std(struct file *file, void *priv, v4l2_std_id std_id)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct vpfe_subdev_info *sdinfo;
	int ret;

	sdinfo = vpfe->current_subdev;
	if (!(sdinfo->inputs[0].capabilities & V4L2_IN_CAP_STD))
		return -ENODATA;

	/* if trying to set the same std then nothing to do */
	if (vpfe_standards[vpfe->std_index].std_id == std_id)
		return 0;

	/* If streaming is started, return error */
	if (vb2_is_busy(&vpfe->buffer_queue)) {
		vpfe_err(vpfe, "%s device busy\n", __func__);
		ret = -EBUSY;
		return ret;
	}

	ret = v4l2_device_call_until_err(&vpfe->v4l2_dev, sdinfo->grp_id,
					 video, s_std, std_id);
	if (ret < 0) {
		vpfe_err(vpfe, "Failed to set standard\n");
		return ret;
	}
	ret = vpfe_config_image_format(vpfe, std_id);

	return ret;
}

static int vpfe_g_std(struct file *file, void *priv, v4l2_std_id *std_id)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct vpfe_subdev_info *sdinfo;

	sdinfo = vpfe->current_subdev;
	if (sdinfo->inputs[0].capabilities != V4L2_IN_CAP_STD)
		return -ENODATA;

	*std_id = vpfe_standards[vpfe->std_index].std_id;

	return 0;
}

/*
 * vpfe_calculate_offsets : This function calculates buffers offset
 * for top and bottom field
 */
static void vpfe_calculate_offsets(struct vpfe_device *vpfe)
{
	struct v4l2_rect image_win;

	vpfe_ccdc_get_image_window(&vpfe->ccdc, &image_win);
	vpfe->field_off = image_win.height * image_win.width;
}

/*
 * vpfe_queue_setup - Callback function for buffer setup.
 * @vq: vb2_queue ptr
 * @nbuffers: ptr to number of buffers requested by application
 * @nplanes:: contains number of distinct video planes needed to hold a frame
 * @sizes[]: contains the size (in bytes) of each plane.
 * @alloc_devs: ptr to allocation context
 *
 * This callback function is called when reqbuf() is called to adjust
 * the buffer count and buffer size
 */
static int vpfe_queue_setup(struct vb2_queue *vq,
			    unsigned int *nbuffers, unsigned int *nplanes,
			    unsigned int sizes[], struct device *alloc_devs[])
{
	struct vpfe_device *vpfe = vb2_get_drv_priv(vq);
	unsigned size = vpfe->fmt.fmt.pix.sizeimage;

	if (vq->num_buffers + *nbuffers < 3)
		*nbuffers = 3 - vq->num_buffers;

	if (*nplanes) {
		if (sizes[0] < size)
			return -EINVAL;
		size = sizes[0];
	}

	*nplanes = 1;
	sizes[0] = size;

	vpfe_dbg(1, vpfe,
		"nbuffers=%d, size=%u\n", *nbuffers, sizes[0]);

	/* Calculate field offset */
	vpfe_calculate_offsets(vpfe);

	return 0;
}

/*
 * vpfe_buffer_prepare :  callback function for buffer prepare
 * @vb: ptr to vb2_buffer
 *
 * This is the callback function for buffer prepare when vb2_qbuf()
 * function is called. The buffer is prepared and user space virtual address
 * or user address is converted into  physical address
 */
static int vpfe_buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct vpfe_device *vpfe = vb2_get_drv_priv(vb->vb2_queue);

	vb2_set_plane_payload(vb, 0, vpfe->fmt.fmt.pix.sizeimage);

	if (vb2_get_plane_payload(vb, 0) > vb2_plane_size(vb, 0))
		return -EINVAL;

	vbuf->field = vpfe->fmt.fmt.pix.field;

	return 0;
}

/*
 * vpfe_buffer_queue : Callback function to add buffer to DMA queue
 * @vb: ptr to vb2_buffer
 */
static void vpfe_buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct vpfe_device *vpfe = vb2_get_drv_priv(vb->vb2_queue);
	struct vpfe_cap_buffer *buf = to_vpfe_buffer(vbuf);
	unsigned long flags = 0;

	/* add the buffer to the DMA queue */
	spin_lock_irqsave(&vpfe->dma_queue_lock, flags);
	list_add_tail(&buf->list, &vpfe->dma_queue);
	spin_unlock_irqrestore(&vpfe->dma_queue_lock, flags);
}

static void vpfe_return_all_buffers(struct vpfe_device *vpfe,
				    enum vb2_buffer_state state)
{
	struct vpfe_cap_buffer *buf, *node;
	unsigned long flags;

	spin_lock_irqsave(&vpfe->dma_queue_lock, flags);
	list_for_each_entry_safe(buf, node, &vpfe->dma_queue, list) {
		vb2_buffer_done(&buf->vb.vb2_buf, state);
		list_del(&buf->list);
	}

	if (vpfe->cur_frm)
		vb2_buffer_done(&vpfe->cur_frm->vb.vb2_buf, state);

	if (vpfe->next_frm && vpfe->next_frm != vpfe->cur_frm)
		vb2_buffer_done(&vpfe->next_frm->vb.vb2_buf, state);

	vpfe->cur_frm = NULL;
	vpfe->next_frm = NULL;
	spin_unlock_irqrestore(&vpfe->dma_queue_lock, flags);
}

/*
 * vpfe_start_streaming : Starts the DMA engine for streaming
 * @vb: ptr to vb2_buffer
 * @count: number of buffers
 */
static int vpfe_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct vpfe_device *vpfe = vb2_get_drv_priv(vq);
	struct vpfe_subdev_info *sdinfo;
	unsigned long flags;
	unsigned long addr;
	int ret;

	spin_lock_irqsave(&vpfe->dma_queue_lock, flags);

	vpfe->field = 0;
	vpfe->sequence = 0;

	sdinfo = vpfe->current_subdev;

	vpfe_attach_irq(vpfe);

	vpfe->stopping = false;
	init_completion(&vpfe->capture_stop);

	if (vpfe->ccdc.ccdc_cfg.if_type == VPFE_RAW_BAYER)
		vpfe_ccdc_config_raw(&vpfe->ccdc);
	else
		vpfe_ccdc_config_ycbcr(&vpfe->ccdc);

	/* Get the next frame from the buffer queue */
	vpfe->next_frm = list_entry(vpfe->dma_queue.next,
				    struct vpfe_cap_buffer, list);
	vpfe->cur_frm = vpfe->next_frm;
	/* Remove buffer from the buffer queue */
	list_del(&vpfe->cur_frm->list);
	spin_unlock_irqrestore(&vpfe->dma_queue_lock, flags);

	addr = vb2_dma_contig_plane_dma_addr(&vpfe->cur_frm->vb.vb2_buf, 0);

	vpfe_set_sdr_addr(&vpfe->ccdc, (unsigned long)(addr));

	vpfe_pcr_enable(&vpfe->ccdc, 1);

	ret = v4l2_subdev_call(sdinfo->sd, video, s_stream, 1);
	if (ret < 0) {
		vpfe_err(vpfe, "Error in attaching interrupt handle\n");
		goto err;
	}

	return 0;

err:
	vpfe_return_all_buffers(vpfe, VB2_BUF_STATE_QUEUED);
	vpfe_pcr_enable(&vpfe->ccdc, 0);
	return ret;
}

/*
 * vpfe_stop_streaming : Stop the DMA engine
 * @vq: ptr to vb2_queue
 *
 * This callback stops the DMA engine and any remaining buffers
 * in the DMA queue are released.
 */
static void vpfe_stop_streaming(struct vb2_queue *vq)
{
	struct vpfe_device *vpfe = vb2_get_drv_priv(vq);
	struct vpfe_subdev_info *sdinfo;
	int ret;

	vpfe_pcr_enable(&vpfe->ccdc, 0);

	/* Wait for the last frame to be captured */
	vpfe->stopping = true;
	wait_for_completion_timeout(&vpfe->capture_stop,
				    msecs_to_jiffies(250));

	vpfe_detach_irq(vpfe);

	sdinfo = vpfe->current_subdev;
	ret = v4l2_subdev_call(sdinfo->sd, video, s_stream, 0);
	if (ret && ret != -ENOIOCTLCMD && ret != -ENODEV)
		vpfe_dbg(1, vpfe, "stream off failed in subdev\n");

	/* release all active buffers */
	vpfe_return_all_buffers(vpfe, VB2_BUF_STATE_ERROR);
}

static int vpfe_g_pixelaspect(struct file *file, void *priv,
			      int type, struct v4l2_fract *f)
{
	struct vpfe_device *vpfe = video_drvdata(file);

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    vpfe->std_index >= ARRAY_SIZE(vpfe_standards))
		return -EINVAL;

	*f = vpfe_standards[vpfe->std_index].pixelaspect;

	return 0;
}

static int
vpfe_g_selection(struct file *file, void *fh, struct v4l2_selection *s)
{
	struct vpfe_device *vpfe = video_drvdata(file);

	if (s->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    vpfe->std_index >= ARRAY_SIZE(vpfe_standards))
		return -EINVAL;

	switch (s->target) {
	case V4L2_SEL_TGT_CROP_BOUNDS:
	case V4L2_SEL_TGT_CROP_DEFAULT:
		s->r.left = 0;
		s->r.top = 0;
		s->r.width = vpfe_standards[vpfe->std_index].width;
		s->r.height = vpfe_standards[vpfe->std_index].height;
		break;

	case V4L2_SEL_TGT_CROP:
		s->r = vpfe->crop;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int
vpfe_s_selection(struct file *file, void *fh, struct v4l2_selection *s)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	struct v4l2_rect cr = vpfe->crop;
	struct v4l2_rect r = s->r;
	u32 bpp;

	/* If streaming is started, return error */
	if (vb2_is_busy(&vpfe->buffer_queue)) {
		vpfe_err(vpfe, "%s device busy\n", __func__);
		return -EBUSY;
	}

	if (s->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
			s->target != V4L2_SEL_TGT_CROP)
		return -EINVAL;

	v4l_bound_align_image(&r.width, 0, cr.width, 0,
			      &r.height, 0, cr.height, 0, 0);

	r.left = clamp_t(unsigned int, r.left, 0, cr.width - r.width);
	r.top  = clamp_t(unsigned int, r.top, 0, cr.height - r.height);

	if (s->flags & V4L2_SEL_FLAG_LE && !v4l2_rect_enclosed(&r, &s->r))
		return -ERANGE;

	if (s->flags & V4L2_SEL_FLAG_GE && !v4l2_rect_enclosed(&s->r, &r))
		return -ERANGE;

	s->r = vpfe->crop = r;

	bpp = __get_bytesperpixel(vpfe, vpfe->current_vpfe_fmt);
	vpfe_ccdc_set_image_window(&vpfe->ccdc, &r, bpp);
	vpfe->fmt.fmt.pix.width = r.width;
	vpfe->fmt.fmt.pix.height = r.height;
	vpfe->fmt.fmt.pix.bytesperline =
		vpfe_ccdc_get_line_length(&vpfe->ccdc);
	vpfe->fmt.fmt.pix.sizeimage = vpfe->fmt.fmt.pix.bytesperline *
						vpfe->fmt.fmt.pix.height;

	vpfe_dbg(1, vpfe, "cropped (%d,%d)/%dx%d of %dx%d\n",
		 r.left, r.top, r.width, r.height, cr.width, cr.height);

	return 0;
}

static long vpfe_ioctl_default(struct file *file, void *priv,
			       bool valid_prio, unsigned int cmd, void *param)
{
	struct vpfe_device *vpfe = video_drvdata(file);
	int ret;

	if (!valid_prio) {
		vpfe_err(vpfe, "%s device busy\n", __func__);
		return -EBUSY;
	}

	/* If streaming is started, return error */
	if (vb2_is_busy(&vpfe->buffer_queue)) {
		vpfe_err(vpfe, "%s device busy\n", __func__);
		return -EBUSY;
	}

	switch (cmd) {
	case VIDIOC_AM437X_CCDC_CFG:
		ret = vpfe_ccdc_set_params(&vpfe->ccdc, (void __user *)param);
		if (ret) {
			vpfe_dbg(2, vpfe,
				"Error setting parameters in CCDC\n");
			return ret;
		}
		ret = vpfe_get_ccdc_image_format(vpfe,
						 &vpfe->fmt);
		if (ret < 0) {
			vpfe_dbg(2, vpfe,
				"Invalid image format at CCDC\n");
			return ret;
		}
		break;

	default:
		ret = -ENOTTY;
		break;
	}

	return ret;
}

static const struct vb2_ops vpfe_video_qops = {
	.wait_prepare		= vb2_ops_wait_prepare,
	.wait_finish		= vb2_ops_wait_finish,
	.queue_setup		= vpfe_queue_setup,
	.buf_prepare		= vpfe_buffer_prepare,
	.buf_queue		= vpfe_buffer_queue,
	.start_streaming	= vpfe_start_streaming,
	.stop_streaming		= vpfe_stop_streaming,
};

/* vpfe capture driver file operations */
static const struct v4l2_file_operations vpfe_fops = {
	.owner		= THIS_MODULE,
	.open		= vpfe_open,
	.release	= vpfe_release,
	.read		= vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl	= video_ioctl2,
	.mmap		= vb2_fop_mmap,
};

/* vpfe capture ioctl operations */
static const struct v4l2_ioctl_ops vpfe_ioctl_ops = {
	.vidioc_querycap		= vpfe_querycap,
	.vidioc_enum_fmt_vid_cap	= vpfe_enum_fmt,
	.vidioc_g_fmt_vid_cap		= vpfe_g_fmt,
	.vidioc_s_fmt_vid_cap		= vpfe_s_fmt,
	.vidioc_try_fmt_vid_cap		= vpfe_try_fmt,

	.vidioc_enum_framesizes		= vpfe_enum_size,

	.vidioc_enum_input		= vpfe_enum_input,
	.vidioc_g_input			= vpfe_g_input,
	.vidioc_s_input			= vpfe_s_input,

	.vidioc_querystd		= vpfe_querystd,
	.vidioc_s_std			= vpfe_s_std,
	.vidioc_g_std			= vpfe_g_std,

	.vidioc_reqbufs			= vb2_ioctl_reqbufs,
	.vidioc_create_bufs		= vb2_ioctl_create_bufs,
	.vidioc_prepare_buf		= vb2_ioctl_prepare_buf,
	.vidioc_querybuf		= vb2_ioctl_querybuf,
	.vidioc_qbuf			= vb2_ioctl_qbuf,
	.vidioc_dqbuf			= vb2_ioctl_dqbuf,
	.vidioc_expbuf			= vb2_ioctl_expbuf,
	.vidioc_streamon		= vb2_ioctl_streamon,
	.vidioc_streamoff		= vb2_ioctl_streamoff,

	.vidioc_log_status		= v4l2_ctrl_log_status,
	.vidioc_subscribe_event		= v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event	= v4l2_event_unsubscribe,

	.vidioc_g_pixelaspect		= vpfe_g_pixelaspect,
	.vidioc_g_selection		= vpfe_g_selection,
	.vidioc_s_selection		= vpfe_s_selection,

	.vidioc_default			= vpfe_ioctl_default,
};

static int
vpfe_async_bound(struct v4l2_async_notifier *notifier,
		 struct v4l2_subdev *subdev,
		 struct v4l2_async_subdev *asd)
{
	struct vpfe_device *vpfe = container_of(notifier->v4l2_dev,
					       struct vpfe_device, v4l2_dev);
	struct v4l2_subdev_mbus_code_enum mbus_code;
	struct vpfe_subdev_info *sdinfo;
	struct vpfe_fmt *fmt;
	int ret = 0;
	bool found = false;
	int i, j, k;

	for (i = 0; i < ARRAY_SIZE(vpfe->cfg->asd); i++) {
		if (vpfe->cfg->asd[i]->match.fwnode ==
		    asd[i].match.fwnode) {
			sdinfo = &vpfe->cfg->sub_devs[i];
			vpfe->sd[i] = subdev;
			vpfe->sd[i]->grp_id = sdinfo->grp_id;
			found = true;
			break;
		}
	}

	if (!found) {
		vpfe_info(vpfe, "sub device (%s) not matched\n", subdev->name);
		return -EINVAL;
	}

	vpfe->video_dev.tvnorms |= sdinfo->inputs[0].std;

	vpfe->num_active_fmt = 0;
	for (j = 0, i = 0; (ret != -EINVAL); ++j) {
		memset(&mbus_code, 0, sizeof(mbus_code));
		mbus_code.index = j;
		mbus_code.which = V4L2_SUBDEV_FORMAT_ACTIVE;
		ret = v4l2_subdev_call(subdev, pad, enum_mbus_code,
				       NULL, &mbus_code);
		if (ret)
			continue;

		vpfe_dbg(3, vpfe,
			 "subdev %s: code: %04x idx: %d\n",
			 subdev->name, mbus_code.code, j);

		for (k = 0; k < ARRAY_SIZE(formats); k++) {
			fmt = &formats[k];
			if (mbus_code.code != fmt->code)
				continue;
			vpfe->active_fmt[i] = fmt;
			vpfe_dbg(3, vpfe,
				 "matched fourcc: %s code: %04x idx: %d\n",
				 print_fourcc(fmt->fourcc), mbus_code.code, i);
			vpfe->num_active_fmt = ++i;
		}
	}

	if (!i) {
		vpfe_err(vpfe, "No suitable format reported by subdev %s\n",
			 subdev->name);
		return -EINVAL;
	}
	return 0;
}

static int vpfe_probe_complete(struct vpfe_device *vpfe)
{
	struct video_device *vdev;
	struct vb2_queue *q;
	int err;

	spin_lock_init(&vpfe->dma_queue_lock);
	mutex_init(&vpfe->lock);

	vpfe->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	/* set first sub device as current one */
	vpfe->current_subdev = &vpfe->cfg->sub_devs[0];
	vpfe->v4l2_dev.ctrl_handler = vpfe->sd[0]->ctrl_handler;

	err = vpfe_set_input(vpfe, 0);
	if (err)
		goto probe_out;

	/* Initialize videobuf2 queue as per the buffer type */
	q = &vpfe->buffer_queue;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_DMABUF | VB2_READ;
	q->drv_priv = vpfe;
	q->ops = &vpfe_video_qops;
	q->mem_ops = &vb2_dma_contig_memops;
	q->buf_struct_size = sizeof(struct vpfe_cap_buffer);
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &vpfe->lock;
	q->min_buffers_needed = 1;
	q->dev = vpfe->pdev;

	err = vb2_queue_init(q);
	if (err) {
		vpfe_err(vpfe, "vb2_queue_init() failed\n");
		goto probe_out;
	}

	INIT_LIST_HEAD(&vpfe->dma_queue);

	vdev = &vpfe->video_dev;
	strscpy(vdev->name, VPFE_MODULE_NAME, sizeof(vdev->name));
	vdev->release = video_device_release_empty;
	vdev->fops = &vpfe_fops;
	vdev->ioctl_ops = &vpfe_ioctl_ops;
	vdev->v4l2_dev = &vpfe->v4l2_dev;
	vdev->vfl_dir = VFL_DIR_RX;
	vdev->queue = q;
	vdev->lock = &vpfe->lock;
	vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING |
			    V4L2_CAP_READWRITE;
	video_set_drvdata(vdev, vpfe);
	err = video_register_device(&vpfe->video_dev, VFL_TYPE_VIDEO, -1);
	if (err) {
		vpfe_err(vpfe,
			"Unable to register video device.\n");
		goto probe_out;
	}

	return 0;

probe_out:
	v4l2_device_unregister(&vpfe->v4l2_dev);
	return err;
}

static int vpfe_async_complete(struct v4l2_async_notifier *notifier)
{
	struct vpfe_device *vpfe = container_of(notifier->v4l2_dev,
					struct vpfe_device, v4l2_dev);

	return vpfe_probe_complete(vpfe);
}

static const struct v4l2_async_notifier_operations vpfe_async_ops = {
	.bound = vpfe_async_bound,
	.complete = vpfe_async_complete,
};

static struct vpfe_config *
vpfe_get_pdata(struct vpfe_device *vpfe)
{
	struct device_node *endpoint = NULL;
	struct device *dev = vpfe->pdev;
	struct vpfe_subdev_info *sdinfo;
	struct vpfe_config *pdata;
	unsigned int flags;
	unsigned int i;
	int err;

	dev_dbg(dev, "vpfe_get_pdata\n");

	v4l2_async_notifier_init(&vpfe->notifier);

	if (!IS_ENABLED(CONFIG_OF) || !dev->of_node)
		return dev->platform_data;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return NULL;

	for (i = 0; ; i++) {
		struct v4l2_fwnode_endpoint bus_cfg = { .bus_type = 0 };
		struct device_node *rem;

		endpoint = of_graph_get_next_endpoint(dev->of_node, endpoint);
		if (!endpoint)
			break;

		sdinfo = &pdata->sub_devs[i];
		sdinfo->grp_id = 0;

		/* we only support camera */
		sdinfo->inputs[0].index = i;
		strscpy(sdinfo->inputs[0].name, "Camera",
			sizeof(sdinfo->inputs[0].name));
		sdinfo->inputs[0].type = V4L2_INPUT_TYPE_CAMERA;
		sdinfo->inputs[0].std = V4L2_STD_ALL;
		sdinfo->inputs[0].capabilities = V4L2_IN_CAP_STD;

		sdinfo->can_route = 0;
		sdinfo->routes = NULL;

		of_property_read_u32(endpoint, "ti,am437x-vpfe-interface",
				     &sdinfo->vpfe_param.if_type);
		if (sdinfo->vpfe_param.if_type < 0 ||
			sdinfo->vpfe_param.if_type > 4) {
			sdinfo->vpfe_param.if_type = VPFE_RAW_BAYER;
		}

		err = v4l2_fwnode_endpoint_parse(of_fwnode_handle(endpoint),
						 &bus_cfg);
		if (err) {
			dev_err(dev, "Could not parse the endpoint\n");
			goto cleanup;
		}

		sdinfo->vpfe_param.bus_width = bus_cfg.bus.parallel.bus_width;

		if (sdinfo->vpfe_param.bus_width < 8 ||
			sdinfo->vpfe_param.bus_width > 16) {
			dev_err(dev, "Invalid bus width.\n");
			goto cleanup;
		}

		flags = bus_cfg.bus.parallel.flags;

		if (flags & V4L2_MBUS_HSYNC_ACTIVE_HIGH)
			sdinfo->vpfe_param.hdpol = 1;

		if (flags & V4L2_MBUS_VSYNC_ACTIVE_HIGH)
			sdinfo->vpfe_param.vdpol = 1;

		rem = of_graph_get_remote_port_parent(endpoint);
		if (!rem) {
			dev_err(dev, "Remote device at %pOF not found\n",
				endpoint);
			goto cleanup;
		}

		pdata->asd[i] = v4l2_async_notifier_add_fwnode_subdev(
			&vpfe->notifier, of_fwnode_handle(rem),
			struct v4l2_async_subdev);
		of_node_put(rem);
		if (IS_ERR(pdata->asd[i]))
			goto cleanup;
	}

	of_node_put(endpoint);
	return pdata;

cleanup:
	v4l2_async_notifier_cleanup(&vpfe->notifier);
	of_node_put(endpoint);
	return NULL;
}

/*
 * vpfe_probe : This function creates device entries by register
 * itself to the V4L2 driver and initializes fields of each
 * device objects
 */
static int vpfe_probe(struct platform_device *pdev)
{
	struct vpfe_config *vpfe_cfg;
	struct vpfe_device *vpfe;
	struct vpfe_ccdc *ccdc;
	struct resource	*res;
	int ret;

	vpfe = devm_kzalloc(&pdev->dev, sizeof(*vpfe), GFP_KERNEL);
	if (!vpfe)
		return -ENOMEM;

	vpfe->pdev = &pdev->dev;

	vpfe_cfg = vpfe_get_pdata(vpfe);
	if (!vpfe_cfg) {
		dev_err(&pdev->dev, "No platform data\n");
		return -EINVAL;
	}

	vpfe->cfg = vpfe_cfg;
	ccdc = &vpfe->ccdc;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ccdc->ccdc_cfg.base_addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ccdc->ccdc_cfg.base_addr)) {
		ret = PTR_ERR(ccdc->ccdc_cfg.base_addr);
		goto probe_out_cleanup;
	}

	ret = platform_get_irq(pdev, 0);
	if (ret <= 0) {
		ret = -ENODEV;
		goto probe_out_cleanup;
	}
	vpfe->irq = ret;

	ret = devm_request_irq(vpfe->pdev, vpfe->irq, vpfe_isr, 0,
			       "vpfe_capture0", vpfe);
	if (ret) {
		dev_err(&pdev->dev, "Unable to request interrupt\n");
		ret = -EINVAL;
		goto probe_out_cleanup;
	}

	ret = v4l2_device_register(&pdev->dev, &vpfe->v4l2_dev);
	if (ret) {
		vpfe_err(vpfe,
			"Unable to register v4l2 device.\n");
		goto probe_out_cleanup;
	}

	/* set the driver data in platform device */
	platform_set_drvdata(pdev, vpfe);
	/* Enabling module functional clock */
	pm_runtime_enable(&pdev->dev);

	/* for now just enable it here instead of waiting for the open */
	pm_runtime_get_sync(&pdev->dev);

	vpfe_ccdc_config_defaults(ccdc);

	pm_runtime_put_sync(&pdev->dev);

	vpfe->sd = devm_kcalloc(&pdev->dev,
				ARRAY_SIZE(vpfe->cfg->asd),
				sizeof(struct v4l2_subdev *),
				GFP_KERNEL);
	if (!vpfe->sd) {
		ret = -ENOMEM;
		goto probe_out_v4l2_unregister;
	}

	vpfe->notifier.ops = &vpfe_async_ops;
	ret = v4l2_async_notifier_register(&vpfe->v4l2_dev, &vpfe->notifier);
	if (ret) {
		vpfe_err(vpfe, "Error registering async notifier\n");
		ret = -EINVAL;
		goto probe_out_v4l2_unregister;
	}

	return 0;

probe_out_v4l2_unregister:
	v4l2_device_unregister(&vpfe->v4l2_dev);
probe_out_cleanup:
	v4l2_async_notifier_cleanup(&vpfe->notifier);
	return ret;
}

/*
 * vpfe_remove : It un-register device from V4L2 driver
 */
static int vpfe_remove(struct platform_device *pdev)
{
	struct vpfe_device *vpfe = platform_get_drvdata(pdev);

	pm_runtime_disable(&pdev->dev);

	v4l2_async_notifier_unregister(&vpfe->notifier);
	v4l2_async_notifier_cleanup(&vpfe->notifier);
	v4l2_device_unregister(&vpfe->v4l2_dev);
	video_unregister_device(&vpfe->video_dev);

	return 0;
}

#ifdef CONFIG_PM_SLEEP

static void vpfe_save_context(struct vpfe_ccdc *ccdc)
{
	ccdc->ccdc_ctx[VPFE_PCR >> 2] = vpfe_reg_read(ccdc, VPFE_PCR);
	ccdc->ccdc_ctx[VPFE_SYNMODE >> 2] = vpfe_reg_read(ccdc, VPFE_SYNMODE);
	ccdc->ccdc_ctx[VPFE_SDOFST >> 2] = vpfe_reg_read(ccdc, VPFE_SDOFST);
	ccdc->ccdc_ctx[VPFE_SDR_ADDR >> 2] = vpfe_reg_read(ccdc, VPFE_SDR_ADDR);
	ccdc->ccdc_ctx[VPFE_CLAMP >> 2] = vpfe_reg_read(ccdc, VPFE_CLAMP);
	ccdc->ccdc_ctx[VPFE_DCSUB >> 2] = vpfe_reg_read(ccdc, VPFE_DCSUB);
	ccdc->ccdc_ctx[VPFE_COLPTN >> 2] = vpfe_reg_read(ccdc, VPFE_COLPTN);
	ccdc->ccdc_ctx[VPFE_BLKCMP >> 2] = vpfe_reg_read(ccdc, VPFE_BLKCMP);
	ccdc->ccdc_ctx[VPFE_VDINT >> 2] = vpfe_reg_read(ccdc, VPFE_VDINT);
	ccdc->ccdc_ctx[VPFE_ALAW >> 2] = vpfe_reg_read(ccdc, VPFE_ALAW);
	ccdc->ccdc_ctx[VPFE_REC656IF >> 2] = vpfe_reg_read(ccdc, VPFE_REC656IF);
	ccdc->ccdc_ctx[VPFE_CCDCFG >> 2] = vpfe_reg_read(ccdc, VPFE_CCDCFG);
	ccdc->ccdc_ctx[VPFE_CULLING >> 2] = vpfe_reg_read(ccdc, VPFE_CULLING);
	ccdc->ccdc_ctx[VPFE_HD_VD_WID >> 2] = vpfe_reg_read(ccdc,
							    VPFE_HD_VD_WID);
	ccdc->ccdc_ctx[VPFE_PIX_LINES >> 2] = vpfe_reg_read(ccdc,
							    VPFE_PIX_LINES);
	ccdc->ccdc_ctx[VPFE_HORZ_INFO >> 2] = vpfe_reg_read(ccdc,
							    VPFE_HORZ_INFO);
	ccdc->ccdc_ctx[VPFE_VERT_START >> 2] = vpfe_reg_read(ccdc,
							     VPFE_VERT_START);
	ccdc->ccdc_ctx[VPFE_VERT_LINES >> 2] = vpfe_reg_read(ccdc,
							     VPFE_VERT_LINES);
	ccdc->ccdc_ctx[VPFE_HSIZE_OFF >> 2] = vpfe_reg_read(ccdc,
							    VPFE_HSIZE_OFF);
}

static int vpfe_suspend(struct device *dev)
{
	struct vpfe_device *vpfe = dev_get_drvdata(dev);
	struct vpfe_ccdc *ccdc = &vpfe->ccdc;

	/* only do full suspend if streaming has started */
	if (vb2_start_streaming_called(&vpfe->buffer_queue)) {
		pm_runtime_get_sync(dev);
		vpfe_config_enable(ccdc, 1);

		/* Save VPFE context */
		vpfe_save_context(ccdc);

		/* Disable CCDC */
		vpfe_pcr_enable(ccdc, 0);
		vpfe_config_enable(ccdc, 0);

		/* Disable both master and slave clock */
		pm_runtime_put_sync(dev);
	}

	/* Select sleep pin state */
	pinctrl_pm_select_sleep_state(dev);

	return 0;
}

static void vpfe_restore_context(struct vpfe_ccdc *ccdc)
{
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_SYNMODE >> 2], VPFE_SYNMODE);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_CULLING >> 2], VPFE_CULLING);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_SDOFST >> 2], VPFE_SDOFST);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_SDR_ADDR >> 2], VPFE_SDR_ADDR);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_CLAMP >> 2], VPFE_CLAMP);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_DCSUB >> 2], VPFE_DCSUB);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_COLPTN >> 2], VPFE_COLPTN);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_BLKCMP >> 2], VPFE_BLKCMP);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_VDINT >> 2], VPFE_VDINT);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_ALAW >> 2], VPFE_ALAW);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_REC656IF >> 2], VPFE_REC656IF);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_CCDCFG >> 2], VPFE_CCDCFG);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_PCR >> 2], VPFE_PCR);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_HD_VD_WID >> 2],
						VPFE_HD_VD_WID);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_PIX_LINES >> 2],
						VPFE_PIX_LINES);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_HORZ_INFO >> 2],
						VPFE_HORZ_INFO);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_VERT_START >> 2],
						VPFE_VERT_START);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_VERT_LINES >> 2],
						VPFE_VERT_LINES);
	vpfe_reg_write(ccdc, ccdc->ccdc_ctx[VPFE_HSIZE_OFF >> 2],
						VPFE_HSIZE_OFF);
}

static int vpfe_resume(struct device *dev)
{
	struct vpfe_device *vpfe = dev_get_drvdata(dev);
	struct vpfe_ccdc *ccdc = &vpfe->ccdc;

	/* only do full resume if streaming has started */
	if (vb2_start_streaming_called(&vpfe->buffer_queue)) {
		/* Enable both master and slave clock */
		pm_runtime_get_sync(dev);
		vpfe_config_enable(ccdc, 1);

		/* Restore VPFE context */
		vpfe_restore_context(ccdc);

		vpfe_config_enable(ccdc, 0);
		pm_runtime_put_sync(dev);
	}

	/* Select default pin state */
	pinctrl_pm_select_default_state(dev);

	return 0;
}

#endif

static SIMPLE_DEV_PM_OPS(vpfe_pm_ops, vpfe_suspend, vpfe_resume);

static const struct of_device_id vpfe_of_match[] = {
	{ .compatible = "ti,am437x-vpfe", },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, vpfe_of_match);

static struct platform_driver vpfe_driver = {
	.probe		= vpfe_probe,
	.remove		= vpfe_remove,
	.driver = {
		.name	= VPFE_MODULE_NAME,
		.pm	= &vpfe_pm_ops,
		.of_match_table = of_match_ptr(vpfe_of_match),
	},
};

module_platform_driver(vpfe_driver);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("TI AM437x VPFE driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(VPFE_VERSION);
