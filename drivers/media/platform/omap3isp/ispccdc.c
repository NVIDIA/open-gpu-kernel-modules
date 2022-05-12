// SPDX-License-Identifier: GPL-2.0-only
/*
 * ispccdc.c
 *
 * TI OMAP3 ISP - CCDC module
 *
 * Copyright (C) 2009-2010 Nokia Corporation
 * Copyright (C) 2009 Texas Instruments, Inc.
 *
 * Contacts: Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 *	     Sakari Ailus <sakari.ailus@iki.fi>
 */

#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <media/v4l2-event.h>

#include "isp.h"
#include "ispreg.h"
#include "ispccdc.h"

#define CCDC_MIN_WIDTH		32
#define CCDC_MIN_HEIGHT		32

static struct v4l2_mbus_framefmt *
__ccdc_get_format(struct isp_ccdc_device *ccdc, struct v4l2_subdev_pad_config *cfg,
		  unsigned int pad, enum v4l2_subdev_format_whence which);

static const unsigned int ccdc_fmts[] = {
	MEDIA_BUS_FMT_Y8_1X8,
	MEDIA_BUS_FMT_Y10_1X10,
	MEDIA_BUS_FMT_Y12_1X12,
	MEDIA_BUS_FMT_SGRBG8_1X8,
	MEDIA_BUS_FMT_SRGGB8_1X8,
	MEDIA_BUS_FMT_SBGGR8_1X8,
	MEDIA_BUS_FMT_SGBRG8_1X8,
	MEDIA_BUS_FMT_SGRBG10_1X10,
	MEDIA_BUS_FMT_SRGGB10_1X10,
	MEDIA_BUS_FMT_SBGGR10_1X10,
	MEDIA_BUS_FMT_SGBRG10_1X10,
	MEDIA_BUS_FMT_SGRBG12_1X12,
	MEDIA_BUS_FMT_SRGGB12_1X12,
	MEDIA_BUS_FMT_SBGGR12_1X12,
	MEDIA_BUS_FMT_SGBRG12_1X12,
	MEDIA_BUS_FMT_YUYV8_2X8,
	MEDIA_BUS_FMT_UYVY8_2X8,
};

/*
 * ccdc_print_status - Print current CCDC Module register values.
 * @ccdc: Pointer to ISP CCDC device.
 *
 * Also prints other debug information stored in the CCDC module.
 */
#define CCDC_PRINT_REGISTER(isp, name)\
	dev_dbg(isp->dev, "###CCDC " #name "=0x%08x\n", \
		isp_reg_readl(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_##name))

static void ccdc_print_status(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);

	dev_dbg(isp->dev, "-------------CCDC Register dump-------------\n");

	CCDC_PRINT_REGISTER(isp, PCR);
	CCDC_PRINT_REGISTER(isp, SYN_MODE);
	CCDC_PRINT_REGISTER(isp, HD_VD_WID);
	CCDC_PRINT_REGISTER(isp, PIX_LINES);
	CCDC_PRINT_REGISTER(isp, HORZ_INFO);
	CCDC_PRINT_REGISTER(isp, VERT_START);
	CCDC_PRINT_REGISTER(isp, VERT_LINES);
	CCDC_PRINT_REGISTER(isp, CULLING);
	CCDC_PRINT_REGISTER(isp, HSIZE_OFF);
	CCDC_PRINT_REGISTER(isp, SDOFST);
	CCDC_PRINT_REGISTER(isp, SDR_ADDR);
	CCDC_PRINT_REGISTER(isp, CLAMP);
	CCDC_PRINT_REGISTER(isp, DCSUB);
	CCDC_PRINT_REGISTER(isp, COLPTN);
	CCDC_PRINT_REGISTER(isp, BLKCMP);
	CCDC_PRINT_REGISTER(isp, FPC);
	CCDC_PRINT_REGISTER(isp, FPC_ADDR);
	CCDC_PRINT_REGISTER(isp, VDINT);
	CCDC_PRINT_REGISTER(isp, ALAW);
	CCDC_PRINT_REGISTER(isp, REC656IF);
	CCDC_PRINT_REGISTER(isp, CFG);
	CCDC_PRINT_REGISTER(isp, FMTCFG);
	CCDC_PRINT_REGISTER(isp, FMT_HORZ);
	CCDC_PRINT_REGISTER(isp, FMT_VERT);
	CCDC_PRINT_REGISTER(isp, PRGEVEN0);
	CCDC_PRINT_REGISTER(isp, PRGEVEN1);
	CCDC_PRINT_REGISTER(isp, PRGODD0);
	CCDC_PRINT_REGISTER(isp, PRGODD1);
	CCDC_PRINT_REGISTER(isp, VP_OUT);
	CCDC_PRINT_REGISTER(isp, LSC_CONFIG);
	CCDC_PRINT_REGISTER(isp, LSC_INITIAL);
	CCDC_PRINT_REGISTER(isp, LSC_TABLE_BASE);
	CCDC_PRINT_REGISTER(isp, LSC_TABLE_OFFSET);

	dev_dbg(isp->dev, "--------------------------------------------\n");
}

/*
 * omap3isp_ccdc_busy - Get busy state of the CCDC.
 * @ccdc: Pointer to ISP CCDC device.
 */
int omap3isp_ccdc_busy(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);

	return isp_reg_readl(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_PCR) &
		ISPCCDC_PCR_BUSY;
}

/* -----------------------------------------------------------------------------
 * Lens Shading Compensation
 */

/*
 * ccdc_lsc_validate_config - Check that LSC configuration is valid.
 * @ccdc: Pointer to ISP CCDC device.
 * @lsc_cfg: the LSC configuration to check.
 *
 * Returns 0 if the LSC configuration is valid, or -EINVAL if invalid.
 */
static int ccdc_lsc_validate_config(struct isp_ccdc_device *ccdc,
				    struct omap3isp_ccdc_lsc_config *lsc_cfg)
{
	struct isp_device *isp = to_isp_device(ccdc);
	struct v4l2_mbus_framefmt *format;
	unsigned int paxel_width, paxel_height;
	unsigned int paxel_shift_x, paxel_shift_y;
	unsigned int min_width, min_height, min_size;
	unsigned int input_width, input_height;

	paxel_shift_x = lsc_cfg->gain_mode_m;
	paxel_shift_y = lsc_cfg->gain_mode_n;

	if ((paxel_shift_x < 2) || (paxel_shift_x > 6) ||
	    (paxel_shift_y < 2) || (paxel_shift_y > 6)) {
		dev_dbg(isp->dev, "CCDC: LSC: Invalid paxel size\n");
		return -EINVAL;
	}

	if (lsc_cfg->offset & 3) {
		dev_dbg(isp->dev,
			"CCDC: LSC: Offset must be a multiple of 4\n");
		return -EINVAL;
	}

	if ((lsc_cfg->initial_x & 1) || (lsc_cfg->initial_y & 1)) {
		dev_dbg(isp->dev, "CCDC: LSC: initial_x and y must be even\n");
		return -EINVAL;
	}

	format = __ccdc_get_format(ccdc, NULL, CCDC_PAD_SINK,
				   V4L2_SUBDEV_FORMAT_ACTIVE);
	input_width = format->width;
	input_height = format->height;

	/* Calculate minimum bytesize for validation */
	paxel_width = 1 << paxel_shift_x;
	min_width = ((input_width + lsc_cfg->initial_x + paxel_width - 1)
		     >> paxel_shift_x) + 1;

	paxel_height = 1 << paxel_shift_y;
	min_height = ((input_height + lsc_cfg->initial_y + paxel_height - 1)
		     >> paxel_shift_y) + 1;

	min_size = 4 * min_width * min_height;
	if (min_size > lsc_cfg->size) {
		dev_dbg(isp->dev, "CCDC: LSC: too small table\n");
		return -EINVAL;
	}
	if (lsc_cfg->offset < (min_width * 4)) {
		dev_dbg(isp->dev, "CCDC: LSC: Offset is too small\n");
		return -EINVAL;
	}
	if ((lsc_cfg->size / lsc_cfg->offset) < min_height) {
		dev_dbg(isp->dev, "CCDC: LSC: Wrong size/offset combination\n");
		return -EINVAL;
	}
	return 0;
}

/*
 * ccdc_lsc_program_table - Program Lens Shading Compensation table address.
 * @ccdc: Pointer to ISP CCDC device.
 */
static void ccdc_lsc_program_table(struct isp_ccdc_device *ccdc,
				   dma_addr_t addr)
{
	isp_reg_writel(to_isp_device(ccdc), addr,
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_LSC_TABLE_BASE);
}

/*
 * ccdc_lsc_setup_regs - Configures the lens shading compensation module
 * @ccdc: Pointer to ISP CCDC device.
 */
static void ccdc_lsc_setup_regs(struct isp_ccdc_device *ccdc,
				struct omap3isp_ccdc_lsc_config *cfg)
{
	struct isp_device *isp = to_isp_device(ccdc);
	int reg;

	isp_reg_writel(isp, cfg->offset, OMAP3_ISP_IOMEM_CCDC,
		       ISPCCDC_LSC_TABLE_OFFSET);

	reg = 0;
	reg |= cfg->gain_mode_n << ISPCCDC_LSC_GAIN_MODE_N_SHIFT;
	reg |= cfg->gain_mode_m << ISPCCDC_LSC_GAIN_MODE_M_SHIFT;
	reg |= cfg->gain_format << ISPCCDC_LSC_GAIN_FORMAT_SHIFT;
	isp_reg_writel(isp, reg, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_LSC_CONFIG);

	reg = 0;
	reg &= ~ISPCCDC_LSC_INITIAL_X_MASK;
	reg |= cfg->initial_x << ISPCCDC_LSC_INITIAL_X_SHIFT;
	reg &= ~ISPCCDC_LSC_INITIAL_Y_MASK;
	reg |= cfg->initial_y << ISPCCDC_LSC_INITIAL_Y_SHIFT;
	isp_reg_writel(isp, reg, OMAP3_ISP_IOMEM_CCDC,
		       ISPCCDC_LSC_INITIAL);
}

static int ccdc_lsc_wait_prefetch(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);
	unsigned int wait;

	isp_reg_writel(isp, IRQ0STATUS_CCDC_LSC_PREF_COMP_IRQ,
		       OMAP3_ISP_IOMEM_MAIN, ISP_IRQ0STATUS);

	/* timeout 1 ms */
	for (wait = 0; wait < 1000; wait++) {
		if (isp_reg_readl(isp, OMAP3_ISP_IOMEM_MAIN, ISP_IRQ0STATUS) &
				  IRQ0STATUS_CCDC_LSC_PREF_COMP_IRQ) {
			isp_reg_writel(isp, IRQ0STATUS_CCDC_LSC_PREF_COMP_IRQ,
				       OMAP3_ISP_IOMEM_MAIN, ISP_IRQ0STATUS);
			return 0;
		}

		rmb();
		udelay(1);
	}

	return -ETIMEDOUT;
}

/*
 * __ccdc_lsc_enable - Enables/Disables the Lens Shading Compensation module.
 * @ccdc: Pointer to ISP CCDC device.
 * @enable: 0 Disables LSC, 1 Enables LSC.
 */
static int __ccdc_lsc_enable(struct isp_ccdc_device *ccdc, int enable)
{
	struct isp_device *isp = to_isp_device(ccdc);
	const struct v4l2_mbus_framefmt *format =
		__ccdc_get_format(ccdc, NULL, CCDC_PAD_SINK,
				  V4L2_SUBDEV_FORMAT_ACTIVE);

	if ((format->code != MEDIA_BUS_FMT_SGRBG10_1X10) &&
	    (format->code != MEDIA_BUS_FMT_SRGGB10_1X10) &&
	    (format->code != MEDIA_BUS_FMT_SBGGR10_1X10) &&
	    (format->code != MEDIA_BUS_FMT_SGBRG10_1X10))
		return -EINVAL;

	if (enable)
		omap3isp_sbl_enable(isp, OMAP3_ISP_SBL_CCDC_LSC_READ);

	isp_reg_clr_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_LSC_CONFIG,
			ISPCCDC_LSC_ENABLE, enable ? ISPCCDC_LSC_ENABLE : 0);

	if (enable) {
		if (ccdc_lsc_wait_prefetch(ccdc) < 0) {
			isp_reg_clr(isp, OMAP3_ISP_IOMEM_CCDC,
				    ISPCCDC_LSC_CONFIG, ISPCCDC_LSC_ENABLE);
			ccdc->lsc.state = LSC_STATE_STOPPED;
			dev_warn(to_device(ccdc), "LSC prefetch timeout\n");
			return -ETIMEDOUT;
		}
		ccdc->lsc.state = LSC_STATE_RUNNING;
	} else {
		ccdc->lsc.state = LSC_STATE_STOPPING;
	}

	return 0;
}

static int ccdc_lsc_busy(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);

	return isp_reg_readl(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_LSC_CONFIG) &
			     ISPCCDC_LSC_BUSY;
}

/*
 * __ccdc_lsc_configure - Apply a new configuration to the LSC engine
 * @ccdc: Pointer to ISP CCDC device
 * @req: New configuration request
 */
static int __ccdc_lsc_configure(struct isp_ccdc_device *ccdc,
				struct ispccdc_lsc_config_req *req)
{
	if (!req->enable)
		return -EINVAL;

	if (ccdc_lsc_validate_config(ccdc, &req->config) < 0) {
		dev_dbg(to_device(ccdc), "Discard LSC configuration\n");
		return -EINVAL;
	}

	if (ccdc_lsc_busy(ccdc))
		return -EBUSY;

	ccdc_lsc_setup_regs(ccdc, &req->config);
	ccdc_lsc_program_table(ccdc, req->table.dma);
	return 0;
}

/*
 * ccdc_lsc_error_handler - Handle LSC prefetch error scenario.
 * @ccdc: Pointer to ISP CCDC device.
 *
 * Disables LSC, and defers enablement to shadow registers update time.
 */
static void ccdc_lsc_error_handler(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);
	/*
	 * From OMAP3 TRM: When this event is pending, the module
	 * goes into transparent mode (output =input). Normal
	 * operation can be resumed at the start of the next frame
	 * after:
	 *  1) Clearing this event
	 *  2) Disabling the LSC module
	 *  3) Enabling it
	 */
	isp_reg_clr(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_LSC_CONFIG,
		    ISPCCDC_LSC_ENABLE);
	ccdc->lsc.state = LSC_STATE_STOPPED;
}

static void ccdc_lsc_free_request(struct isp_ccdc_device *ccdc,
				  struct ispccdc_lsc_config_req *req)
{
	struct isp_device *isp = to_isp_device(ccdc);

	if (req == NULL)
		return;

	if (req->table.addr) {
		sg_free_table(&req->table.sgt);
		dma_free_coherent(isp->dev, req->config.size, req->table.addr,
				  req->table.dma);
	}

	kfree(req);
}

static void ccdc_lsc_free_queue(struct isp_ccdc_device *ccdc,
				struct list_head *queue)
{
	struct ispccdc_lsc_config_req *req, *n;
	unsigned long flags;

	spin_lock_irqsave(&ccdc->lsc.req_lock, flags);
	list_for_each_entry_safe(req, n, queue, list) {
		list_del(&req->list);
		spin_unlock_irqrestore(&ccdc->lsc.req_lock, flags);
		ccdc_lsc_free_request(ccdc, req);
		spin_lock_irqsave(&ccdc->lsc.req_lock, flags);
	}
	spin_unlock_irqrestore(&ccdc->lsc.req_lock, flags);
}

static void ccdc_lsc_free_table_work(struct work_struct *work)
{
	struct isp_ccdc_device *ccdc;
	struct ispccdc_lsc *lsc;

	lsc = container_of(work, struct ispccdc_lsc, table_work);
	ccdc = container_of(lsc, struct isp_ccdc_device, lsc);

	ccdc_lsc_free_queue(ccdc, &lsc->free_queue);
}

/*
 * ccdc_lsc_config - Configure the LSC module from a userspace request
 *
 * Store the request LSC configuration in the LSC engine request pointer. The
 * configuration will be applied to the hardware when the CCDC will be enabled,
 * or at the next LSC interrupt if the CCDC is already running.
 */
static int ccdc_lsc_config(struct isp_ccdc_device *ccdc,
			   struct omap3isp_ccdc_update_config *config)
{
	struct isp_device *isp = to_isp_device(ccdc);
	struct ispccdc_lsc_config_req *req;
	unsigned long flags;
	u16 update;
	int ret;

	update = config->update &
		 (OMAP3ISP_CCDC_CONFIG_LSC | OMAP3ISP_CCDC_TBL_LSC);
	if (!update)
		return 0;

	if (update != (OMAP3ISP_CCDC_CONFIG_LSC | OMAP3ISP_CCDC_TBL_LSC)) {
		dev_dbg(to_device(ccdc),
			"%s: Both LSC configuration and table need to be supplied\n",
			__func__);
		return -EINVAL;
	}

	req = kzalloc(sizeof(*req), GFP_KERNEL);
	if (req == NULL)
		return -ENOMEM;

	if (config->flag & OMAP3ISP_CCDC_CONFIG_LSC) {
		if (copy_from_user(&req->config, config->lsc_cfg,
				   sizeof(req->config))) {
			ret = -EFAULT;
			goto done;
		}

		req->enable = 1;

		req->table.addr = dma_alloc_coherent(isp->dev, req->config.size,
						     &req->table.dma,
						     GFP_KERNEL);
		if (req->table.addr == NULL) {
			ret = -ENOMEM;
			goto done;
		}

		ret = dma_get_sgtable(isp->dev, &req->table.sgt,
				      req->table.addr, req->table.dma,
				      req->config.size);
		if (ret < 0)
			goto done;

		dma_sync_sg_for_cpu(isp->dev, req->table.sgt.sgl,
				    req->table.sgt.nents, DMA_TO_DEVICE);

		if (copy_from_user(req->table.addr, config->lsc,
				   req->config.size)) {
			ret = -EFAULT;
			goto done;
		}

		dma_sync_sg_for_device(isp->dev, req->table.sgt.sgl,
				       req->table.sgt.nents, DMA_TO_DEVICE);
	}

	spin_lock_irqsave(&ccdc->lsc.req_lock, flags);
	if (ccdc->lsc.request) {
		list_add_tail(&ccdc->lsc.request->list, &ccdc->lsc.free_queue);
		schedule_work(&ccdc->lsc.table_work);
	}
	ccdc->lsc.request = req;
	spin_unlock_irqrestore(&ccdc->lsc.req_lock, flags);

	ret = 0;

done:
	if (ret < 0)
		ccdc_lsc_free_request(ccdc, req);

	return ret;
}

static inline int ccdc_lsc_is_configured(struct isp_ccdc_device *ccdc)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&ccdc->lsc.req_lock, flags);
	ret = ccdc->lsc.active != NULL;
	spin_unlock_irqrestore(&ccdc->lsc.req_lock, flags);

	return ret;
}

static int ccdc_lsc_enable(struct isp_ccdc_device *ccdc)
{
	struct ispccdc_lsc *lsc = &ccdc->lsc;

	if (lsc->state != LSC_STATE_STOPPED)
		return -EINVAL;

	if (lsc->active) {
		list_add_tail(&lsc->active->list, &lsc->free_queue);
		lsc->active = NULL;
	}

	if (__ccdc_lsc_configure(ccdc, lsc->request) < 0) {
		omap3isp_sbl_disable(to_isp_device(ccdc),
				OMAP3_ISP_SBL_CCDC_LSC_READ);
		list_add_tail(&lsc->request->list, &lsc->free_queue);
		lsc->request = NULL;
		goto done;
	}

	lsc->active = lsc->request;
	lsc->request = NULL;
	__ccdc_lsc_enable(ccdc, 1);

done:
	if (!list_empty(&lsc->free_queue))
		schedule_work(&lsc->table_work);

	return 0;
}

/* -----------------------------------------------------------------------------
 * Parameters configuration
 */

/*
 * ccdc_configure_clamp - Configure optical-black or digital clamping
 * @ccdc: Pointer to ISP CCDC device.
 *
 * The CCDC performs either optical-black or digital clamp. Configure and enable
 * the selected clamp method.
 */
static void ccdc_configure_clamp(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);
	u32 clamp;

	if (ccdc->obclamp) {
		clamp  = ccdc->clamp.obgain << ISPCCDC_CLAMP_OBGAIN_SHIFT;
		clamp |= ccdc->clamp.oblen << ISPCCDC_CLAMP_OBSLEN_SHIFT;
		clamp |= ccdc->clamp.oblines << ISPCCDC_CLAMP_OBSLN_SHIFT;
		clamp |= ccdc->clamp.obstpixel << ISPCCDC_CLAMP_OBST_SHIFT;
		isp_reg_writel(isp, clamp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_CLAMP);
	} else {
		isp_reg_writel(isp, ccdc->clamp.dcsubval,
			       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_DCSUB);
	}

	isp_reg_clr_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_CLAMP,
			ISPCCDC_CLAMP_CLAMPEN,
			ccdc->obclamp ? ISPCCDC_CLAMP_CLAMPEN : 0);
}

/*
 * ccdc_configure_fpc - Configure Faulty Pixel Correction
 * @ccdc: Pointer to ISP CCDC device.
 */
static void ccdc_configure_fpc(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);

	isp_reg_clr(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_FPC, ISPCCDC_FPC_FPCEN);

	if (!ccdc->fpc_en)
		return;

	isp_reg_writel(isp, ccdc->fpc.dma, OMAP3_ISP_IOMEM_CCDC,
		       ISPCCDC_FPC_ADDR);
	/* The FPNUM field must be set before enabling FPC. */
	isp_reg_writel(isp, (ccdc->fpc.fpnum << ISPCCDC_FPC_FPNUM_SHIFT),
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_FPC);
	isp_reg_writel(isp, (ccdc->fpc.fpnum << ISPCCDC_FPC_FPNUM_SHIFT) |
		       ISPCCDC_FPC_FPCEN, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_FPC);
}

/*
 * ccdc_configure_black_comp - Configure Black Level Compensation.
 * @ccdc: Pointer to ISP CCDC device.
 */
static void ccdc_configure_black_comp(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);
	u32 blcomp;

	blcomp  = ccdc->blcomp.b_mg << ISPCCDC_BLKCMP_B_MG_SHIFT;
	blcomp |= ccdc->blcomp.gb_g << ISPCCDC_BLKCMP_GB_G_SHIFT;
	blcomp |= ccdc->blcomp.gr_cy << ISPCCDC_BLKCMP_GR_CY_SHIFT;
	blcomp |= ccdc->blcomp.r_ye << ISPCCDC_BLKCMP_R_YE_SHIFT;

	isp_reg_writel(isp, blcomp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_BLKCMP);
}

/*
 * ccdc_configure_lpf - Configure Low-Pass Filter (LPF).
 * @ccdc: Pointer to ISP CCDC device.
 */
static void ccdc_configure_lpf(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);

	isp_reg_clr_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_SYN_MODE,
			ISPCCDC_SYN_MODE_LPF,
			ccdc->lpf ? ISPCCDC_SYN_MODE_LPF : 0);
}

/*
 * ccdc_configure_alaw - Configure A-law compression.
 * @ccdc: Pointer to ISP CCDC device.
 */
static void ccdc_configure_alaw(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);
	const struct isp_format_info *info;
	u32 alaw = 0;

	info = omap3isp_video_format_info(ccdc->formats[CCDC_PAD_SINK].code);

	switch (info->width) {
	case 8:
		return;

	case 10:
		alaw = ISPCCDC_ALAW_GWDI_9_0;
		break;
	case 11:
		alaw = ISPCCDC_ALAW_GWDI_10_1;
		break;
	case 12:
		alaw = ISPCCDC_ALAW_GWDI_11_2;
		break;
	case 13:
		alaw = ISPCCDC_ALAW_GWDI_12_3;
		break;
	}

	if (ccdc->alaw)
		alaw |= ISPCCDC_ALAW_CCDTBL;

	isp_reg_writel(isp, alaw, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_ALAW);
}

/*
 * ccdc_config_imgattr - Configure sensor image specific attributes.
 * @ccdc: Pointer to ISP CCDC device.
 * @colptn: Color pattern of the sensor.
 */
static void ccdc_config_imgattr(struct isp_ccdc_device *ccdc, u32 colptn)
{
	struct isp_device *isp = to_isp_device(ccdc);

	isp_reg_writel(isp, colptn, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_COLPTN);
}

/*
 * ccdc_config - Set CCDC configuration from userspace
 * @ccdc: Pointer to ISP CCDC device.
 * @ccdc_struct: Structure containing CCDC configuration sent from userspace.
 *
 * Returns 0 if successful, -EINVAL if the pointer to the configuration
 * structure is null, or the copy_from_user function fails to copy user space
 * memory to kernel space memory.
 */
static int ccdc_config(struct isp_ccdc_device *ccdc,
		       struct omap3isp_ccdc_update_config *ccdc_struct)
{
	struct isp_device *isp = to_isp_device(ccdc);
	unsigned long flags;

	spin_lock_irqsave(&ccdc->lock, flags);
	ccdc->shadow_update = 1;
	spin_unlock_irqrestore(&ccdc->lock, flags);

	if (OMAP3ISP_CCDC_ALAW & ccdc_struct->update) {
		ccdc->alaw = !!(OMAP3ISP_CCDC_ALAW & ccdc_struct->flag);
		ccdc->update |= OMAP3ISP_CCDC_ALAW;
	}

	if (OMAP3ISP_CCDC_LPF & ccdc_struct->update) {
		ccdc->lpf = !!(OMAP3ISP_CCDC_LPF & ccdc_struct->flag);
		ccdc->update |= OMAP3ISP_CCDC_LPF;
	}

	if (OMAP3ISP_CCDC_BLCLAMP & ccdc_struct->update) {
		if (copy_from_user(&ccdc->clamp, ccdc_struct->bclamp,
				   sizeof(ccdc->clamp))) {
			ccdc->shadow_update = 0;
			return -EFAULT;
		}

		ccdc->obclamp = !!(OMAP3ISP_CCDC_BLCLAMP & ccdc_struct->flag);
		ccdc->update |= OMAP3ISP_CCDC_BLCLAMP;
	}

	if (OMAP3ISP_CCDC_BCOMP & ccdc_struct->update) {
		if (copy_from_user(&ccdc->blcomp, ccdc_struct->blcomp,
				   sizeof(ccdc->blcomp))) {
			ccdc->shadow_update = 0;
			return -EFAULT;
		}

		ccdc->update |= OMAP3ISP_CCDC_BCOMP;
	}

	ccdc->shadow_update = 0;

	if (OMAP3ISP_CCDC_FPC & ccdc_struct->update) {
		struct omap3isp_ccdc_fpc fpc;
		struct ispccdc_fpc fpc_old = { .addr = NULL, };
		struct ispccdc_fpc fpc_new;
		u32 size;

		if (ccdc->state != ISP_PIPELINE_STREAM_STOPPED)
			return -EBUSY;

		ccdc->fpc_en = !!(OMAP3ISP_CCDC_FPC & ccdc_struct->flag);

		if (ccdc->fpc_en) {
			if (copy_from_user(&fpc, ccdc_struct->fpc, sizeof(fpc)))
				return -EFAULT;

			size = fpc.fpnum * 4;

			/*
			 * The table address must be 64-bytes aligned, which is
			 * guaranteed by dma_alloc_coherent().
			 */
			fpc_new.fpnum = fpc.fpnum;
			fpc_new.addr = dma_alloc_coherent(isp->dev, size,
							  &fpc_new.dma,
							  GFP_KERNEL);
			if (fpc_new.addr == NULL)
				return -ENOMEM;

			if (copy_from_user(fpc_new.addr,
					   (__force void __user *)(long)fpc.fpcaddr,
					   size)) {
				dma_free_coherent(isp->dev, size, fpc_new.addr,
						  fpc_new.dma);
				return -EFAULT;
			}

			fpc_old = ccdc->fpc;
			ccdc->fpc = fpc_new;
		}

		ccdc_configure_fpc(ccdc);

		if (fpc_old.addr != NULL)
			dma_free_coherent(isp->dev, fpc_old.fpnum * 4,
					  fpc_old.addr, fpc_old.dma);
	}

	return ccdc_lsc_config(ccdc, ccdc_struct);
}

static void ccdc_apply_controls(struct isp_ccdc_device *ccdc)
{
	if (ccdc->update & OMAP3ISP_CCDC_ALAW) {
		ccdc_configure_alaw(ccdc);
		ccdc->update &= ~OMAP3ISP_CCDC_ALAW;
	}

	if (ccdc->update & OMAP3ISP_CCDC_LPF) {
		ccdc_configure_lpf(ccdc);
		ccdc->update &= ~OMAP3ISP_CCDC_LPF;
	}

	if (ccdc->update & OMAP3ISP_CCDC_BLCLAMP) {
		ccdc_configure_clamp(ccdc);
		ccdc->update &= ~OMAP3ISP_CCDC_BLCLAMP;
	}

	if (ccdc->update & OMAP3ISP_CCDC_BCOMP) {
		ccdc_configure_black_comp(ccdc);
		ccdc->update &= ~OMAP3ISP_CCDC_BCOMP;
	}
}

/*
 * omap3isp_ccdc_restore_context - Restore values of the CCDC module registers
 * @isp: Pointer to ISP device
 */
void omap3isp_ccdc_restore_context(struct isp_device *isp)
{
	struct isp_ccdc_device *ccdc = &isp->isp_ccdc;

	isp_reg_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_CFG, ISPCCDC_CFG_VDLC);

	ccdc->update = OMAP3ISP_CCDC_ALAW | OMAP3ISP_CCDC_LPF
		     | OMAP3ISP_CCDC_BLCLAMP | OMAP3ISP_CCDC_BCOMP;
	ccdc_apply_controls(ccdc);
	ccdc_configure_fpc(ccdc);
}

/* -----------------------------------------------------------------------------
 * Format- and pipeline-related configuration helpers
 */

/*
 * ccdc_config_vp - Configure the Video Port.
 * @ccdc: Pointer to ISP CCDC device.
 */
static void ccdc_config_vp(struct isp_ccdc_device *ccdc)
{
	struct isp_pipeline *pipe = to_isp_pipeline(&ccdc->subdev.entity);
	struct isp_device *isp = to_isp_device(ccdc);
	const struct isp_format_info *info;
	struct v4l2_mbus_framefmt *format;
	unsigned long l3_ick = pipe->l3_ick;
	unsigned int max_div = isp->revision == ISP_REVISION_15_0 ? 64 : 8;
	unsigned int div = 0;
	u32 fmtcfg = ISPCCDC_FMTCFG_VPEN;

	format = &ccdc->formats[CCDC_PAD_SOURCE_VP];

	if (!format->code) {
		/* Disable the video port when the input format isn't supported.
		 * This is indicated by a pixel code set to 0.
		 */
		isp_reg_writel(isp, 0, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_FMTCFG);
		return;
	}

	isp_reg_writel(isp, (0 << ISPCCDC_FMT_HORZ_FMTSPH_SHIFT) |
		       (format->width << ISPCCDC_FMT_HORZ_FMTLNH_SHIFT),
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_FMT_HORZ);
	isp_reg_writel(isp, (0 << ISPCCDC_FMT_VERT_FMTSLV_SHIFT) |
		       ((format->height + 1) << ISPCCDC_FMT_VERT_FMTLNV_SHIFT),
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_FMT_VERT);

	isp_reg_writel(isp, (format->width << ISPCCDC_VP_OUT_HORZ_NUM_SHIFT) |
		       (format->height << ISPCCDC_VP_OUT_VERT_NUM_SHIFT),
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_VP_OUT);

	info = omap3isp_video_format_info(ccdc->formats[CCDC_PAD_SINK].code);

	switch (info->width) {
	case 8:
	case 10:
		fmtcfg |= ISPCCDC_FMTCFG_VPIN_9_0;
		break;
	case 11:
		fmtcfg |= ISPCCDC_FMTCFG_VPIN_10_1;
		break;
	case 12:
		fmtcfg |= ISPCCDC_FMTCFG_VPIN_11_2;
		break;
	case 13:
		fmtcfg |= ISPCCDC_FMTCFG_VPIN_12_3;
		break;
	}

	if (pipe->input)
		div = DIV_ROUND_UP(l3_ick, pipe->max_rate);
	else if (pipe->external_rate)
		div = l3_ick / pipe->external_rate;

	div = clamp(div, 2U, max_div);
	fmtcfg |= (div - 2) << ISPCCDC_FMTCFG_VPIF_FRQ_SHIFT;

	isp_reg_writel(isp, fmtcfg, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_FMTCFG);
}

/*
 * ccdc_config_outlineoffset - Configure memory saving output line offset
 * @ccdc: Pointer to ISP CCDC device.
 * @bpl: Number of bytes per line when stored in memory.
 * @field: Field order when storing interlaced formats in memory.
 *
 * Configure the offsets for the line output control:
 *
 * - The horizontal line offset is defined as the number of bytes between the
 *   start of two consecutive lines in memory. Set it to the given bytes per
 *   line value.
 *
 * - The field offset value is defined as the number of lines to offset the
 *   start of the field identified by FID = 1. Set it to one.
 *
 * - The line offset values are defined as the number of lines (as defined by
 *   the horizontal line offset) between the start of two consecutive lines for
 *   all combinations of odd/even lines in odd/even fields. When interleaving
 *   fields set them all to two lines, and to one line otherwise.
 */
static void ccdc_config_outlineoffset(struct isp_ccdc_device *ccdc,
				      unsigned int bpl,
				      enum v4l2_field field)
{
	struct isp_device *isp = to_isp_device(ccdc);
	u32 sdofst = 0;

	isp_reg_writel(isp, bpl & 0xffff, OMAP3_ISP_IOMEM_CCDC,
		       ISPCCDC_HSIZE_OFF);

	switch (field) {
	case V4L2_FIELD_INTERLACED_TB:
	case V4L2_FIELD_INTERLACED_BT:
		/* When interleaving fields in memory offset field one by one
		 * line and set the line offset to two lines.
		 */
		sdofst |= (1 << ISPCCDC_SDOFST_LOFST0_SHIFT)
		       |  (1 << ISPCCDC_SDOFST_LOFST1_SHIFT)
		       |  (1 << ISPCCDC_SDOFST_LOFST2_SHIFT)
		       |  (1 << ISPCCDC_SDOFST_LOFST3_SHIFT);
		break;

	default:
		/* In all other cases set the line offsets to one line. */
		break;
	}

	isp_reg_writel(isp, sdofst, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_SDOFST);
}

/*
 * ccdc_set_outaddr - Set memory address to save output image
 * @ccdc: Pointer to ISP CCDC device.
 * @addr: ISP MMU Mapped 32-bit memory address aligned on 32 byte boundary.
 *
 * Sets the memory address where the output will be saved.
 */
static void ccdc_set_outaddr(struct isp_ccdc_device *ccdc, u32 addr)
{
	struct isp_device *isp = to_isp_device(ccdc);

	isp_reg_writel(isp, addr, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_SDR_ADDR);
}

/*
 * omap3isp_ccdc_max_rate - Calculate maximum input data rate based on the input
 * @ccdc: Pointer to ISP CCDC device.
 * @max_rate: Maximum calculated data rate.
 *
 * Returns in *max_rate less value between calculated and passed
 */
void omap3isp_ccdc_max_rate(struct isp_ccdc_device *ccdc,
			    unsigned int *max_rate)
{
	struct isp_pipeline *pipe = to_isp_pipeline(&ccdc->subdev.entity);
	unsigned int rate;

	if (pipe == NULL)
		return;

	/*
	 * TRM says that for parallel sensors the maximum data rate
	 * should be 90% form L3/2 clock, otherwise just L3/2.
	 */
	if (ccdc->input == CCDC_INPUT_PARALLEL)
		rate = pipe->l3_ick / 2 * 9 / 10;
	else
		rate = pipe->l3_ick / 2;

	*max_rate = min(*max_rate, rate);
}

/*
 * ccdc_config_sync_if - Set CCDC sync interface configuration
 * @ccdc: Pointer to ISP CCDC device.
 * @parcfg: Parallel interface platform data (may be NULL)
 * @data_size: Data size
 */
static void ccdc_config_sync_if(struct isp_ccdc_device *ccdc,
				struct isp_parallel_cfg *parcfg,
				unsigned int data_size)
{
	struct isp_device *isp = to_isp_device(ccdc);
	const struct v4l2_mbus_framefmt *format;
	u32 syn_mode = ISPCCDC_SYN_MODE_VDHDEN;

	format = &ccdc->formats[CCDC_PAD_SINK];

	if (format->code == MEDIA_BUS_FMT_YUYV8_2X8 ||
	    format->code == MEDIA_BUS_FMT_UYVY8_2X8) {
		/* According to the OMAP3 TRM the input mode only affects SYNC
		 * mode, enabling BT.656 mode should take precedence. However,
		 * in practice setting the input mode to YCbCr data on 8 bits
		 * seems to be required in BT.656 mode. In SYNC mode set it to
		 * YCbCr on 16 bits as the bridge is enabled in that case.
		 */
		if (ccdc->bt656)
			syn_mode |= ISPCCDC_SYN_MODE_INPMOD_YCBCR8;
		else
			syn_mode |= ISPCCDC_SYN_MODE_INPMOD_YCBCR16;
	}

	switch (data_size) {
	case 8:
		syn_mode |= ISPCCDC_SYN_MODE_DATSIZ_8;
		break;
	case 10:
		syn_mode |= ISPCCDC_SYN_MODE_DATSIZ_10;
		break;
	case 11:
		syn_mode |= ISPCCDC_SYN_MODE_DATSIZ_11;
		break;
	case 12:
		syn_mode |= ISPCCDC_SYN_MODE_DATSIZ_12;
		break;
	}

	if (parcfg && parcfg->data_pol)
		syn_mode |= ISPCCDC_SYN_MODE_DATAPOL;

	if (parcfg && parcfg->hs_pol)
		syn_mode |= ISPCCDC_SYN_MODE_HDPOL;

	/* The polarity of the vertical sync signal output by the BT.656
	 * decoder is not documented and seems to be active low.
	 */
	if ((parcfg && parcfg->vs_pol) || ccdc->bt656)
		syn_mode |= ISPCCDC_SYN_MODE_VDPOL;

	if (parcfg && parcfg->fld_pol)
		syn_mode |= ISPCCDC_SYN_MODE_FLDPOL;

	isp_reg_writel(isp, syn_mode, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_SYN_MODE);

	/* The CCDC_CFG.Y8POS bit is used in YCbCr8 input mode only. The
	 * hardware seems to ignore it in all other input modes.
	 */
	if (format->code == MEDIA_BUS_FMT_UYVY8_2X8)
		isp_reg_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_CFG,
			    ISPCCDC_CFG_Y8POS);
	else
		isp_reg_clr(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_CFG,
			    ISPCCDC_CFG_Y8POS);

	/* Enable or disable BT.656 mode, including error correction for the
	 * synchronization codes.
	 */
	if (ccdc->bt656)
		isp_reg_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_REC656IF,
			    ISPCCDC_REC656IF_R656ON | ISPCCDC_REC656IF_ECCFVH);
	else
		isp_reg_clr(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_REC656IF,
			    ISPCCDC_REC656IF_R656ON | ISPCCDC_REC656IF_ECCFVH);

}

/* CCDC formats descriptions */
static const u32 ccdc_sgrbg_pattern =
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP0PLC0_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP0PLC1_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP0PLC2_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP0PLC3_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP1PLC0_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP1PLC1_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP1PLC2_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP1PLC3_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP2PLC0_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP2PLC1_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP2PLC2_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP2PLC3_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP3PLC0_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP3PLC1_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP3PLC2_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP3PLC3_SHIFT;

static const u32 ccdc_srggb_pattern =
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP0PLC0_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP0PLC1_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP0PLC2_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP0PLC3_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP1PLC0_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP1PLC1_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP1PLC2_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP1PLC3_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP2PLC0_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP2PLC1_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP2PLC2_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP2PLC3_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP3PLC0_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP3PLC1_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP3PLC2_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP3PLC3_SHIFT;

static const u32 ccdc_sbggr_pattern =
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP0PLC0_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP0PLC1_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP0PLC2_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP0PLC3_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP1PLC0_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP1PLC1_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP1PLC2_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP1PLC3_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP2PLC0_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP2PLC1_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP2PLC2_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP2PLC3_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP3PLC0_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP3PLC1_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP3PLC2_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP3PLC3_SHIFT;

static const u32 ccdc_sgbrg_pattern =
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP0PLC0_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP0PLC1_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP0PLC2_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP0PLC3_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP1PLC0_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP1PLC1_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP1PLC2_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP1PLC3_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP2PLC0_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP2PLC1_SHIFT |
	ISPCCDC_COLPTN_Gb_G  << ISPCCDC_COLPTN_CP2PLC2_SHIFT |
	ISPCCDC_COLPTN_B_Mg  << ISPCCDC_COLPTN_CP2PLC3_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP3PLC0_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP3PLC1_SHIFT |
	ISPCCDC_COLPTN_R_Ye  << ISPCCDC_COLPTN_CP3PLC2_SHIFT |
	ISPCCDC_COLPTN_Gr_Cy << ISPCCDC_COLPTN_CP3PLC3_SHIFT;

static void ccdc_configure(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);
	struct isp_parallel_cfg *parcfg = NULL;
	struct v4l2_subdev *sensor;
	struct v4l2_mbus_framefmt *format;
	const struct v4l2_rect *crop;
	const struct isp_format_info *fmt_info;
	struct v4l2_subdev_format fmt_src;
	unsigned int depth_out;
	unsigned int depth_in = 0;
	struct media_pad *pad;
	unsigned long flags;
	unsigned int bridge;
	unsigned int shift;
	unsigned int nph;
	unsigned int sph;
	u32 syn_mode;
	u32 ccdc_pattern;

	ccdc->bt656 = false;
	ccdc->fields = 0;

	pad = media_entity_remote_pad(&ccdc->pads[CCDC_PAD_SINK]);
	sensor = media_entity_to_v4l2_subdev(pad->entity);
	if (ccdc->input == CCDC_INPUT_PARALLEL) {
		struct v4l2_subdev *sd =
			to_isp_pipeline(&ccdc->subdev.entity)->external;

		parcfg = &v4l2_subdev_to_bus_cfg(sd)->bus.parallel;
		ccdc->bt656 = parcfg->bt656;
	}

	/* CCDC_PAD_SINK */
	format = &ccdc->formats[CCDC_PAD_SINK];

	/* Compute the lane shifter shift value and enable the bridge when the
	 * input format is a non-BT.656 YUV variant.
	 */
	fmt_src.pad = pad->index;
	fmt_src.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	if (!v4l2_subdev_call(sensor, pad, get_fmt, NULL, &fmt_src)) {
		fmt_info = omap3isp_video_format_info(fmt_src.format.code);
		depth_in = fmt_info->width;
	}

	fmt_info = omap3isp_video_format_info(format->code);
	depth_out = fmt_info->width;
	shift = depth_in - depth_out;

	if (ccdc->bt656)
		bridge = ISPCTRL_PAR_BRIDGE_DISABLE;
	else if (fmt_info->code == MEDIA_BUS_FMT_YUYV8_2X8)
		bridge = ISPCTRL_PAR_BRIDGE_LENDIAN;
	else if (fmt_info->code == MEDIA_BUS_FMT_UYVY8_2X8)
		bridge = ISPCTRL_PAR_BRIDGE_BENDIAN;
	else
		bridge = ISPCTRL_PAR_BRIDGE_DISABLE;

	omap3isp_configure_bridge(isp, ccdc->input, parcfg, shift, bridge);

	/* Configure the sync interface. */
	ccdc_config_sync_if(ccdc, parcfg, depth_out);

	syn_mode = isp_reg_readl(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_SYN_MODE);

	/* Use the raw, unprocessed data when writing to memory. The H3A and
	 * histogram modules are still fed with lens shading corrected data.
	 */
	syn_mode &= ~ISPCCDC_SYN_MODE_VP2SDR;

	if (ccdc->output & CCDC_OUTPUT_MEMORY)
		syn_mode |= ISPCCDC_SYN_MODE_WEN;
	else
		syn_mode &= ~ISPCCDC_SYN_MODE_WEN;

	if (ccdc->output & CCDC_OUTPUT_RESIZER)
		syn_mode |= ISPCCDC_SYN_MODE_SDR2RSZ;
	else
		syn_mode &= ~ISPCCDC_SYN_MODE_SDR2RSZ;

	/* Mosaic filter */
	switch (format->code) {
	case MEDIA_BUS_FMT_SRGGB10_1X10:
	case MEDIA_BUS_FMT_SRGGB12_1X12:
		ccdc_pattern = ccdc_srggb_pattern;
		break;
	case MEDIA_BUS_FMT_SBGGR10_1X10:
	case MEDIA_BUS_FMT_SBGGR12_1X12:
		ccdc_pattern = ccdc_sbggr_pattern;
		break;
	case MEDIA_BUS_FMT_SGBRG10_1X10:
	case MEDIA_BUS_FMT_SGBRG12_1X12:
		ccdc_pattern = ccdc_sgbrg_pattern;
		break;
	default:
		/* Use GRBG */
		ccdc_pattern = ccdc_sgrbg_pattern;
		break;
	}
	ccdc_config_imgattr(ccdc, ccdc_pattern);

	/* Generate VD0 on the last line of the image and VD1 on the
	 * 2/3 height line.
	 */
	isp_reg_writel(isp, ((format->height - 2) << ISPCCDC_VDINT_0_SHIFT) |
		       ((format->height * 2 / 3) << ISPCCDC_VDINT_1_SHIFT),
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_VDINT);

	/* CCDC_PAD_SOURCE_OF */
	format = &ccdc->formats[CCDC_PAD_SOURCE_OF];
	crop = &ccdc->crop;

	/* The horizontal coordinates are expressed in pixel clock cycles. We
	 * need two cycles per pixel in BT.656 mode, and one cycle per pixel in
	 * SYNC mode regardless of the format as the bridge is enabled for YUV
	 * formats in that case.
	 */
	if (ccdc->bt656) {
		sph = crop->left * 2;
		nph = crop->width * 2 - 1;
	} else {
		sph = crop->left;
		nph = crop->width - 1;
	}

	isp_reg_writel(isp, (sph << ISPCCDC_HORZ_INFO_SPH_SHIFT) |
		       (nph << ISPCCDC_HORZ_INFO_NPH_SHIFT),
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_HORZ_INFO);
	isp_reg_writel(isp, (crop->top << ISPCCDC_VERT_START_SLV0_SHIFT) |
		       (crop->top << ISPCCDC_VERT_START_SLV1_SHIFT),
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_VERT_START);
	isp_reg_writel(isp, (crop->height - 1)
			<< ISPCCDC_VERT_LINES_NLV_SHIFT,
		       OMAP3_ISP_IOMEM_CCDC, ISPCCDC_VERT_LINES);

	ccdc_config_outlineoffset(ccdc, ccdc->video_out.bpl_value,
				  format->field);

	/* When interleaving fields enable processing of the field input signal.
	 * This will cause the line output control module to apply the field
	 * offset to field 1.
	 */
	if (ccdc->formats[CCDC_PAD_SINK].field == V4L2_FIELD_ALTERNATE &&
	    (format->field == V4L2_FIELD_INTERLACED_TB ||
	     format->field == V4L2_FIELD_INTERLACED_BT))
		syn_mode |= ISPCCDC_SYN_MODE_FLDMODE;

	/* The CCDC outputs data in UYVY order by default. Swap bytes to get
	 * YUYV.
	 */
	if (format->code == MEDIA_BUS_FMT_YUYV8_1X16)
		isp_reg_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_CFG,
			    ISPCCDC_CFG_BSWD);
	else
		isp_reg_clr(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_CFG,
			    ISPCCDC_CFG_BSWD);

	/* Use PACK8 mode for 1byte per pixel formats. Check for BT.656 mode
	 * explicitly as the driver reports 1X16 instead of 2X8 at the OF pad
	 * for simplicity.
	 */
	if (omap3isp_video_format_info(format->code)->width <= 8 || ccdc->bt656)
		syn_mode |= ISPCCDC_SYN_MODE_PACK8;
	else
		syn_mode &= ~ISPCCDC_SYN_MODE_PACK8;

	isp_reg_writel(isp, syn_mode, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_SYN_MODE);

	/* CCDC_PAD_SOURCE_VP */
	ccdc_config_vp(ccdc);

	/* Lens shading correction. */
	spin_lock_irqsave(&ccdc->lsc.req_lock, flags);
	if (ccdc->lsc.request == NULL)
		goto unlock;

	WARN_ON(ccdc->lsc.active);

	/* Get last good LSC configuration. If it is not supported for
	 * the current active resolution discard it.
	 */
	if (ccdc->lsc.active == NULL &&
	    __ccdc_lsc_configure(ccdc, ccdc->lsc.request) == 0) {
		ccdc->lsc.active = ccdc->lsc.request;
	} else {
		list_add_tail(&ccdc->lsc.request->list, &ccdc->lsc.free_queue);
		schedule_work(&ccdc->lsc.table_work);
	}

	ccdc->lsc.request = NULL;

unlock:
	spin_unlock_irqrestore(&ccdc->lsc.req_lock, flags);

	ccdc_apply_controls(ccdc);
}

static void __ccdc_enable(struct isp_ccdc_device *ccdc, int enable)
{
	struct isp_device *isp = to_isp_device(ccdc);

	/* Avoid restarting the CCDC when streaming is stopping. */
	if (enable && ccdc->stopping & CCDC_STOP_REQUEST)
		return;

	isp_reg_clr_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_PCR,
			ISPCCDC_PCR_EN, enable ? ISPCCDC_PCR_EN : 0);

	ccdc->running = enable;
}

static int ccdc_disable(struct isp_ccdc_device *ccdc)
{
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&ccdc->lock, flags);
	if (ccdc->state == ISP_PIPELINE_STREAM_CONTINUOUS)
		ccdc->stopping = CCDC_STOP_REQUEST;
	if (!ccdc->running)
		ccdc->stopping = CCDC_STOP_FINISHED;
	spin_unlock_irqrestore(&ccdc->lock, flags);

	ret = wait_event_timeout(ccdc->wait,
				 ccdc->stopping == CCDC_STOP_FINISHED,
				 msecs_to_jiffies(2000));
	if (ret == 0) {
		ret = -ETIMEDOUT;
		dev_warn(to_device(ccdc), "CCDC stop timeout!\n");
	}

	omap3isp_sbl_disable(to_isp_device(ccdc), OMAP3_ISP_SBL_CCDC_LSC_READ);

	mutex_lock(&ccdc->ioctl_lock);
	ccdc_lsc_free_request(ccdc, ccdc->lsc.request);
	ccdc->lsc.request = ccdc->lsc.active;
	ccdc->lsc.active = NULL;
	cancel_work_sync(&ccdc->lsc.table_work);
	ccdc_lsc_free_queue(ccdc, &ccdc->lsc.free_queue);
	mutex_unlock(&ccdc->ioctl_lock);

	ccdc->stopping = CCDC_STOP_NOT_REQUESTED;

	return ret > 0 ? 0 : ret;
}

static void ccdc_enable(struct isp_ccdc_device *ccdc)
{
	if (ccdc_lsc_is_configured(ccdc))
		__ccdc_lsc_enable(ccdc, 1);
	__ccdc_enable(ccdc, 1);
}

/* -----------------------------------------------------------------------------
 * Interrupt handling
 */

/*
 * ccdc_sbl_busy - Poll idle state of CCDC and related SBL memory write bits
 * @ccdc: Pointer to ISP CCDC device.
 *
 * Returns zero if the CCDC is idle and the image has been written to
 * memory, too.
 */
static int ccdc_sbl_busy(struct isp_ccdc_device *ccdc)
{
	struct isp_device *isp = to_isp_device(ccdc);

	return omap3isp_ccdc_busy(ccdc)
		| (isp_reg_readl(isp, OMAP3_ISP_IOMEM_SBL, ISPSBL_CCDC_WR_0) &
		   ISPSBL_CCDC_WR_0_DATA_READY)
		| (isp_reg_readl(isp, OMAP3_ISP_IOMEM_SBL, ISPSBL_CCDC_WR_1) &
		   ISPSBL_CCDC_WR_0_DATA_READY)
		| (isp_reg_readl(isp, OMAP3_ISP_IOMEM_SBL, ISPSBL_CCDC_WR_2) &
		   ISPSBL_CCDC_WR_0_DATA_READY)
		| (isp_reg_readl(isp, OMAP3_ISP_IOMEM_SBL, ISPSBL_CCDC_WR_3) &
		   ISPSBL_CCDC_WR_0_DATA_READY);
}

/*
 * ccdc_sbl_wait_idle - Wait until the CCDC and related SBL are idle
 * @ccdc: Pointer to ISP CCDC device.
 * @max_wait: Max retry count in us for wait for idle/busy transition.
 */
static int ccdc_sbl_wait_idle(struct isp_ccdc_device *ccdc,
			      unsigned int max_wait)
{
	unsigned int wait = 0;

	if (max_wait == 0)
		max_wait = 10000; /* 10 ms */

	for (wait = 0; wait <= max_wait; wait++) {
		if (!ccdc_sbl_busy(ccdc))
			return 0;

		rmb();
		udelay(1);
	}

	return -EBUSY;
}

/* ccdc_handle_stopping - Handle CCDC and/or LSC stopping sequence
 * @ccdc: Pointer to ISP CCDC device.
 * @event: Pointing which event trigger handler
 *
 * Return 1 when the event and stopping request combination is satisfied,
 * zero otherwise.
 */
static int ccdc_handle_stopping(struct isp_ccdc_device *ccdc, u32 event)
{
	int rval = 0;

	switch ((ccdc->stopping & 3) | event) {
	case CCDC_STOP_REQUEST | CCDC_EVENT_VD1:
		if (ccdc->lsc.state != LSC_STATE_STOPPED)
			__ccdc_lsc_enable(ccdc, 0);
		__ccdc_enable(ccdc, 0);
		ccdc->stopping = CCDC_STOP_EXECUTED;
		return 1;

	case CCDC_STOP_EXECUTED | CCDC_EVENT_VD0:
		ccdc->stopping |= CCDC_STOP_CCDC_FINISHED;
		if (ccdc->lsc.state == LSC_STATE_STOPPED)
			ccdc->stopping |= CCDC_STOP_LSC_FINISHED;
		rval = 1;
		break;

	case CCDC_STOP_EXECUTED | CCDC_EVENT_LSC_DONE:
		ccdc->stopping |= CCDC_STOP_LSC_FINISHED;
		rval = 1;
		break;

	case CCDC_STOP_EXECUTED | CCDC_EVENT_VD1:
		return 1;
	}

	if (ccdc->stopping == CCDC_STOP_FINISHED) {
		wake_up(&ccdc->wait);
		rval = 1;
	}

	return rval;
}

static void ccdc_hs_vs_isr(struct isp_ccdc_device *ccdc)
{
	struct isp_pipeline *pipe = to_isp_pipeline(&ccdc->subdev.entity);
	struct video_device *vdev = ccdc->subdev.devnode;
	struct v4l2_event event;

	/* Frame number propagation */
	atomic_inc(&pipe->frame_number);

	memset(&event, 0, sizeof(event));
	event.type = V4L2_EVENT_FRAME_SYNC;
	event.u.frame_sync.frame_sequence = atomic_read(&pipe->frame_number);

	v4l2_event_queue(vdev, &event);
}

/*
 * ccdc_lsc_isr - Handle LSC events
 * @ccdc: Pointer to ISP CCDC device.
 * @events: LSC events
 */
static void ccdc_lsc_isr(struct isp_ccdc_device *ccdc, u32 events)
{
	unsigned long flags;

	if (events & IRQ0STATUS_CCDC_LSC_PREF_ERR_IRQ) {
		struct isp_pipeline *pipe =
			to_isp_pipeline(&ccdc->subdev.entity);

		ccdc_lsc_error_handler(ccdc);
		pipe->error = true;
		dev_dbg(to_device(ccdc), "lsc prefetch error\n");
	}

	if (!(events & IRQ0STATUS_CCDC_LSC_DONE_IRQ))
		return;

	/* LSC_DONE interrupt occur, there are two cases
	 * 1. stopping for reconfiguration
	 * 2. stopping because of STREAM OFF command
	 */
	spin_lock_irqsave(&ccdc->lsc.req_lock, flags);

	if (ccdc->lsc.state == LSC_STATE_STOPPING)
		ccdc->lsc.state = LSC_STATE_STOPPED;

	if (ccdc_handle_stopping(ccdc, CCDC_EVENT_LSC_DONE))
		goto done;

	if (ccdc->lsc.state != LSC_STATE_RECONFIG)
		goto done;

	/* LSC is in STOPPING state, change to the new state */
	ccdc->lsc.state = LSC_STATE_STOPPED;

	/* This is an exception. Start of frame and LSC_DONE interrupt
	 * have been received on the same time. Skip this event and wait
	 * for better times.
	 */
	if (events & IRQ0STATUS_HS_VS_IRQ)
		goto done;

	/* The LSC engine is stopped at this point. Enable it if there's a
	 * pending request.
	 */
	if (ccdc->lsc.request == NULL)
		goto done;

	ccdc_lsc_enable(ccdc);

done:
	spin_unlock_irqrestore(&ccdc->lsc.req_lock, flags);
}

/*
 * Check whether the CCDC has captured all fields necessary to complete the
 * buffer.
 */
static bool ccdc_has_all_fields(struct isp_ccdc_device *ccdc)
{
	struct isp_pipeline *pipe = to_isp_pipeline(&ccdc->subdev.entity);
	struct isp_device *isp = to_isp_device(ccdc);
	enum v4l2_field of_field = ccdc->formats[CCDC_PAD_SOURCE_OF].field;
	enum v4l2_field field;

	/* When the input is progressive fields don't matter. */
	if (of_field == V4L2_FIELD_NONE)
		return true;

	/* Read the current field identifier. */
	field = isp_reg_readl(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_SYN_MODE)
	      & ISPCCDC_SYN_MODE_FLDSTAT
	      ? V4L2_FIELD_BOTTOM : V4L2_FIELD_TOP;

	/* When capturing fields in alternate order just store the current field
	 * identifier in the pipeline.
	 */
	if (of_field == V4L2_FIELD_ALTERNATE) {
		pipe->field = field;
		return true;
	}

	/* The format is interlaced. Make sure we've captured both fields. */
	ccdc->fields |= field == V4L2_FIELD_BOTTOM
		      ? CCDC_FIELD_BOTTOM : CCDC_FIELD_TOP;

	if (ccdc->fields != CCDC_FIELD_BOTH)
		return false;

	/* Verify that the field just captured corresponds to the last field
	 * needed based on the desired field order.
	 */
	if ((of_field == V4L2_FIELD_INTERLACED_TB && field == V4L2_FIELD_TOP) ||
	    (of_field == V4L2_FIELD_INTERLACED_BT && field == V4L2_FIELD_BOTTOM))
		return false;

	/* The buffer can be completed, reset the fields for the next buffer. */
	ccdc->fields = 0;

	return true;
}

static int ccdc_isr_buffer(struct isp_ccdc_device *ccdc)
{
	struct isp_pipeline *pipe = to_isp_pipeline(&ccdc->subdev.entity);
	struct isp_device *isp = to_isp_device(ccdc);
	struct isp_buffer *buffer;

	/* The CCDC generates VD0 interrupts even when disabled (the datasheet
	 * doesn't explicitly state if that's supposed to happen or not, so it
	 * can be considered as a hardware bug or as a feature, but we have to
	 * deal with it anyway). Disabling the CCDC when no buffer is available
	 * would thus not be enough, we need to handle the situation explicitly.
	 */
	if (list_empty(&ccdc->video_out.dmaqueue))
		return 0;

	/* We're in continuous mode, and memory writes were disabled due to a
	 * buffer underrun. Re-enable them now that we have a buffer. The buffer
	 * address has been set in ccdc_video_queue.
	 */
	if (ccdc->state == ISP_PIPELINE_STREAM_CONTINUOUS && ccdc->underrun) {
		ccdc->underrun = 0;
		return 1;
	}

	/* Wait for the CCDC to become idle. */
	if (ccdc_sbl_wait_idle(ccdc, 1000)) {
		dev_info(isp->dev, "CCDC won't become idle!\n");
		media_entity_enum_set(&isp->crashed, &ccdc->subdev.entity);
		omap3isp_pipeline_cancel_stream(pipe);
		return 0;
	}

	/* Don't restart CCDC if we're just about to stop streaming. */
	if (ccdc->state == ISP_PIPELINE_STREAM_CONTINUOUS &&
	    ccdc->stopping & CCDC_STOP_REQUEST)
		return 0;

	if (!ccdc_has_all_fields(ccdc))
		return 1;

	buffer = omap3isp_video_buffer_next(&ccdc->video_out);
	if (buffer != NULL)
		ccdc_set_outaddr(ccdc, buffer->dma);

	pipe->state |= ISP_PIPELINE_IDLE_OUTPUT;

	if (ccdc->state == ISP_PIPELINE_STREAM_SINGLESHOT &&
	    isp_pipeline_ready(pipe))
		omap3isp_pipeline_set_stream(pipe,
					ISP_PIPELINE_STREAM_SINGLESHOT);

	return buffer != NULL;
}

/*
 * ccdc_vd0_isr - Handle VD0 event
 * @ccdc: Pointer to ISP CCDC device.
 *
 * Executes LSC deferred enablement before next frame starts.
 */
static void ccdc_vd0_isr(struct isp_ccdc_device *ccdc)
{
	unsigned long flags;
	int restart = 0;

	/* In BT.656 mode the CCDC doesn't generate an HS/VS interrupt. We thus
	 * need to increment the frame counter here.
	 */
	if (ccdc->bt656) {
		struct isp_pipeline *pipe =
			to_isp_pipeline(&ccdc->subdev.entity);

		atomic_inc(&pipe->frame_number);
	}

	/* Emulate a VD1 interrupt for BT.656 mode, as we can't stop the CCDC in
	 * the VD1 interrupt handler in that mode without risking a CCDC stall
	 * if a short frame is received.
	 */
	if (ccdc->bt656) {
		spin_lock_irqsave(&ccdc->lock, flags);
		if (ccdc->state == ISP_PIPELINE_STREAM_CONTINUOUS &&
		    ccdc->output & CCDC_OUTPUT_MEMORY) {
			if (ccdc->lsc.state != LSC_STATE_STOPPED)
				__ccdc_lsc_enable(ccdc, 0);
			__ccdc_enable(ccdc, 0);
		}
		ccdc_handle_stopping(ccdc, CCDC_EVENT_VD1);
		spin_unlock_irqrestore(&ccdc->lock, flags);
	}

	spin_lock_irqsave(&ccdc->lock, flags);
	if (ccdc_handle_stopping(ccdc, CCDC_EVENT_VD0)) {
		spin_unlock_irqrestore(&ccdc->lock, flags);
		return;
	}

	if (ccdc->output & CCDC_OUTPUT_MEMORY)
		restart = ccdc_isr_buffer(ccdc);

	if (!ccdc->shadow_update)
		ccdc_apply_controls(ccdc);
	spin_unlock_irqrestore(&ccdc->lock, flags);

	if (restart)
		ccdc_enable(ccdc);
}

/*
 * ccdc_vd1_isr - Handle VD1 event
 * @ccdc: Pointer to ISP CCDC device.
 */
static void ccdc_vd1_isr(struct isp_ccdc_device *ccdc)
{
	unsigned long flags;

	/* In BT.656 mode the synchronization signals are generated by the CCDC
	 * from the embedded sync codes. The VD0 and VD1 interrupts are thus
	 * only triggered when the CCDC is enabled, unlike external sync mode
	 * where the line counter runs even when the CCDC is stopped. We can't
	 * disable the CCDC at VD1 time, as no VD0 interrupt would be generated
	 * for a short frame, which would result in the CCDC being stopped and
	 * no VD interrupt generated anymore. The CCDC is stopped from the VD0
	 * interrupt handler instead for BT.656.
	 */
	if (ccdc->bt656)
		return;

	spin_lock_irqsave(&ccdc->lsc.req_lock, flags);

	/*
	 * Depending on the CCDC pipeline state, CCDC stopping should be
	 * handled differently. In SINGLESHOT we emulate an internal CCDC
	 * stopping because the CCDC hw works only in continuous mode.
	 * When CONTINUOUS pipeline state is used and the CCDC writes it's
	 * data to memory the CCDC and LSC are stopped immediately but
	 * without change the CCDC stopping state machine. The CCDC
	 * stopping state machine should be used only when user request
	 * for stopping is received (SINGLESHOT is an exception).
	 */
	switch (ccdc->state) {
	case ISP_PIPELINE_STREAM_SINGLESHOT:
		ccdc->stopping = CCDC_STOP_REQUEST;
		break;

	case ISP_PIPELINE_STREAM_CONTINUOUS:
		if (ccdc->output & CCDC_OUTPUT_MEMORY) {
			if (ccdc->lsc.state != LSC_STATE_STOPPED)
				__ccdc_lsc_enable(ccdc, 0);
			__ccdc_enable(ccdc, 0);
		}
		break;

	case ISP_PIPELINE_STREAM_STOPPED:
		break;
	}

	if (ccdc_handle_stopping(ccdc, CCDC_EVENT_VD1))
		goto done;

	if (ccdc->lsc.request == NULL)
		goto done;

	/*
	 * LSC need to be reconfigured. Stop it here and on next LSC_DONE IRQ
	 * do the appropriate changes in registers
	 */
	if (ccdc->lsc.state == LSC_STATE_RUNNING) {
		__ccdc_lsc_enable(ccdc, 0);
		ccdc->lsc.state = LSC_STATE_RECONFIG;
		goto done;
	}

	/* LSC has been in STOPPED state, enable it */
	if (ccdc->lsc.state == LSC_STATE_STOPPED)
		ccdc_lsc_enable(ccdc);

done:
	spin_unlock_irqrestore(&ccdc->lsc.req_lock, flags);
}

/*
 * omap3isp_ccdc_isr - Configure CCDC during interframe time.
 * @ccdc: Pointer to ISP CCDC device.
 * @events: CCDC events
 */
int omap3isp_ccdc_isr(struct isp_ccdc_device *ccdc, u32 events)
{
	if (ccdc->state == ISP_PIPELINE_STREAM_STOPPED)
		return 0;

	if (events & IRQ0STATUS_CCDC_VD1_IRQ)
		ccdc_vd1_isr(ccdc);

	ccdc_lsc_isr(ccdc, events);

	if (events & IRQ0STATUS_CCDC_VD0_IRQ)
		ccdc_vd0_isr(ccdc);

	if (events & IRQ0STATUS_HS_VS_IRQ)
		ccdc_hs_vs_isr(ccdc);

	return 0;
}

/* -----------------------------------------------------------------------------
 * ISP video operations
 */

static int ccdc_video_queue(struct isp_video *video, struct isp_buffer *buffer)
{
	struct isp_ccdc_device *ccdc = &video->isp->isp_ccdc;
	unsigned long flags;
	bool restart = false;

	if (!(ccdc->output & CCDC_OUTPUT_MEMORY))
		return -ENODEV;

	ccdc_set_outaddr(ccdc, buffer->dma);

	/* We now have a buffer queued on the output, restart the pipeline
	 * on the next CCDC interrupt if running in continuous mode (or when
	 * starting the stream) in external sync mode, or immediately in BT.656
	 * sync mode as no CCDC interrupt is generated when the CCDC is stopped
	 * in that case.
	 */
	spin_lock_irqsave(&ccdc->lock, flags);
	if (ccdc->state == ISP_PIPELINE_STREAM_CONTINUOUS && !ccdc->running &&
	    ccdc->bt656)
		restart = true;
	else
		ccdc->underrun = 1;
	spin_unlock_irqrestore(&ccdc->lock, flags);

	if (restart)
		ccdc_enable(ccdc);

	return 0;
}

static const struct isp_video_operations ccdc_video_ops = {
	.queue = ccdc_video_queue,
};

/* -----------------------------------------------------------------------------
 * V4L2 subdev operations
 */

/*
 * ccdc_ioctl - CCDC module private ioctl's
 * @sd: ISP CCDC V4L2 subdevice
 * @cmd: ioctl command
 * @arg: ioctl argument
 *
 * Return 0 on success or a negative error code otherwise.
 */
static long ccdc_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	int ret;

	switch (cmd) {
	case VIDIOC_OMAP3ISP_CCDC_CFG:
		mutex_lock(&ccdc->ioctl_lock);
		ret = ccdc_config(ccdc, arg);
		mutex_unlock(&ccdc->ioctl_lock);
		break;

	default:
		return -ENOIOCTLCMD;
	}

	return ret;
}

static int ccdc_subscribe_event(struct v4l2_subdev *sd, struct v4l2_fh *fh,
				struct v4l2_event_subscription *sub)
{
	if (sub->type != V4L2_EVENT_FRAME_SYNC)
		return -EINVAL;

	/* line number is zero at frame start */
	if (sub->id != 0)
		return -EINVAL;

	return v4l2_event_subscribe(fh, sub, OMAP3ISP_CCDC_NEVENTS, NULL);
}

static int ccdc_unsubscribe_event(struct v4l2_subdev *sd, struct v4l2_fh *fh,
				  struct v4l2_event_subscription *sub)
{
	return v4l2_event_unsubscribe(fh, sub);
}

/*
 * ccdc_set_stream - Enable/Disable streaming on the CCDC module
 * @sd: ISP CCDC V4L2 subdevice
 * @enable: Enable/disable stream
 *
 * When writing to memory, the CCDC hardware can't be enabled without a memory
 * buffer to write to. As the s_stream operation is called in response to a
 * STREAMON call without any buffer queued yet, just update the enabled field
 * and return immediately. The CCDC will be enabled in ccdc_isr_buffer().
 *
 * When not writing to memory enable the CCDC immediately.
 */
static int ccdc_set_stream(struct v4l2_subdev *sd, int enable)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	struct isp_device *isp = to_isp_device(ccdc);
	int ret = 0;

	if (ccdc->state == ISP_PIPELINE_STREAM_STOPPED) {
		if (enable == ISP_PIPELINE_STREAM_STOPPED)
			return 0;

		omap3isp_subclk_enable(isp, OMAP3_ISP_SUBCLK_CCDC);
		isp_reg_set(isp, OMAP3_ISP_IOMEM_CCDC, ISPCCDC_CFG,
			    ISPCCDC_CFG_VDLC);

		ccdc_configure(ccdc);

		ccdc_print_status(ccdc);
	}

	switch (enable) {
	case ISP_PIPELINE_STREAM_CONTINUOUS:
		if (ccdc->output & CCDC_OUTPUT_MEMORY)
			omap3isp_sbl_enable(isp, OMAP3_ISP_SBL_CCDC_WRITE);

		if (ccdc->underrun || !(ccdc->output & CCDC_OUTPUT_MEMORY))
			ccdc_enable(ccdc);

		ccdc->underrun = 0;
		break;

	case ISP_PIPELINE_STREAM_SINGLESHOT:
		if (ccdc->output & CCDC_OUTPUT_MEMORY &&
		    ccdc->state != ISP_PIPELINE_STREAM_SINGLESHOT)
			omap3isp_sbl_enable(isp, OMAP3_ISP_SBL_CCDC_WRITE);

		ccdc_enable(ccdc);
		break;

	case ISP_PIPELINE_STREAM_STOPPED:
		ret = ccdc_disable(ccdc);
		if (ccdc->output & CCDC_OUTPUT_MEMORY)
			omap3isp_sbl_disable(isp, OMAP3_ISP_SBL_CCDC_WRITE);
		omap3isp_subclk_disable(isp, OMAP3_ISP_SUBCLK_CCDC);
		ccdc->underrun = 0;
		break;
	}

	ccdc->state = enable;
	return ret;
}

static struct v4l2_mbus_framefmt *
__ccdc_get_format(struct isp_ccdc_device *ccdc, struct v4l2_subdev_pad_config *cfg,
		  unsigned int pad, enum v4l2_subdev_format_whence which)
{
	if (which == V4L2_SUBDEV_FORMAT_TRY)
		return v4l2_subdev_get_try_format(&ccdc->subdev, cfg, pad);
	else
		return &ccdc->formats[pad];
}

static struct v4l2_rect *
__ccdc_get_crop(struct isp_ccdc_device *ccdc, struct v4l2_subdev_pad_config *cfg,
		enum v4l2_subdev_format_whence which)
{
	if (which == V4L2_SUBDEV_FORMAT_TRY)
		return v4l2_subdev_get_try_crop(&ccdc->subdev, cfg, CCDC_PAD_SOURCE_OF);
	else
		return &ccdc->crop;
}

/*
 * ccdc_try_format - Try video format on a pad
 * @ccdc: ISP CCDC device
 * @cfg : V4L2 subdev pad configuration
 * @pad: Pad number
 * @fmt: Format
 */
static void
ccdc_try_format(struct isp_ccdc_device *ccdc, struct v4l2_subdev_pad_config *cfg,
		unsigned int pad, struct v4l2_mbus_framefmt *fmt,
		enum v4l2_subdev_format_whence which)
{
	const struct isp_format_info *info;
	u32 pixelcode;
	unsigned int width = fmt->width;
	unsigned int height = fmt->height;
	struct v4l2_rect *crop;
	enum v4l2_field field;
	unsigned int i;

	switch (pad) {
	case CCDC_PAD_SINK:
		for (i = 0; i < ARRAY_SIZE(ccdc_fmts); i++) {
			if (fmt->code == ccdc_fmts[i])
				break;
		}

		/* If not found, use SGRBG10 as default */
		if (i >= ARRAY_SIZE(ccdc_fmts))
			fmt->code = MEDIA_BUS_FMT_SGRBG10_1X10;

		/* Clamp the input size. */
		fmt->width = clamp_t(u32, width, 32, 4096);
		fmt->height = clamp_t(u32, height, 32, 4096);

		/* Default to progressive field order. */
		if (fmt->field == V4L2_FIELD_ANY)
			fmt->field = V4L2_FIELD_NONE;

		break;

	case CCDC_PAD_SOURCE_OF:
		pixelcode = fmt->code;
		field = fmt->field;
		*fmt = *__ccdc_get_format(ccdc, cfg, CCDC_PAD_SINK, which);

		/* In SYNC mode the bridge converts YUV formats from 2X8 to
		 * 1X16. In BT.656 no such conversion occurs. As we don't know
		 * at this point whether the source will use SYNC or BT.656 mode
		 * let's pretend the conversion always occurs. The CCDC will be
		 * configured to pack bytes in BT.656, hiding the inaccuracy.
		 * In all cases bytes can be swapped.
		 */
		if (fmt->code == MEDIA_BUS_FMT_YUYV8_2X8 ||
		    fmt->code == MEDIA_BUS_FMT_UYVY8_2X8) {
			/* Use the user requested format if YUV. */
			if (pixelcode == MEDIA_BUS_FMT_YUYV8_2X8 ||
			    pixelcode == MEDIA_BUS_FMT_UYVY8_2X8 ||
			    pixelcode == MEDIA_BUS_FMT_YUYV8_1X16 ||
			    pixelcode == MEDIA_BUS_FMT_UYVY8_1X16)
				fmt->code = pixelcode;

			if (fmt->code == MEDIA_BUS_FMT_YUYV8_2X8)
				fmt->code = MEDIA_BUS_FMT_YUYV8_1X16;
			else if (fmt->code == MEDIA_BUS_FMT_UYVY8_2X8)
				fmt->code = MEDIA_BUS_FMT_UYVY8_1X16;
		}

		/* Hardcode the output size to the crop rectangle size. */
		crop = __ccdc_get_crop(ccdc, cfg, which);
		fmt->width = crop->width;
		fmt->height = crop->height;

		/* When input format is interlaced with alternating fields the
		 * CCDC can interleave the fields.
		 */
		if (fmt->field == V4L2_FIELD_ALTERNATE &&
		    (field == V4L2_FIELD_INTERLACED_TB ||
		     field == V4L2_FIELD_INTERLACED_BT)) {
			fmt->field = field;
			fmt->height *= 2;
		}

		break;

	case CCDC_PAD_SOURCE_VP:
		*fmt = *__ccdc_get_format(ccdc, cfg, CCDC_PAD_SINK, which);

		/* The video port interface truncates the data to 10 bits. */
		info = omap3isp_video_format_info(fmt->code);
		fmt->code = info->truncated;

		/* YUV formats are not supported by the video port. */
		if (fmt->code == MEDIA_BUS_FMT_YUYV8_2X8 ||
		    fmt->code == MEDIA_BUS_FMT_UYVY8_2X8)
			fmt->code = 0;

		/* The number of lines that can be clocked out from the video
		 * port output must be at least one line less than the number
		 * of input lines.
		 */
		fmt->width = clamp_t(u32, width, 32, fmt->width);
		fmt->height = clamp_t(u32, height, 32, fmt->height - 1);
		break;
	}

	/* Data is written to memory unpacked, each 10-bit or 12-bit pixel is
	 * stored on 2 bytes.
	 */
	fmt->colorspace = V4L2_COLORSPACE_SRGB;
}

/*
 * ccdc_try_crop - Validate a crop rectangle
 * @ccdc: ISP CCDC device
 * @sink: format on the sink pad
 * @crop: crop rectangle to be validated
 */
static void ccdc_try_crop(struct isp_ccdc_device *ccdc,
			  const struct v4l2_mbus_framefmt *sink,
			  struct v4l2_rect *crop)
{
	const struct isp_format_info *info;
	unsigned int max_width;

	/* For Bayer formats, restrict left/top and width/height to even values
	 * to keep the Bayer pattern.
	 */
	info = omap3isp_video_format_info(sink->code);
	if (info->flavor != MEDIA_BUS_FMT_Y8_1X8) {
		crop->left &= ~1;
		crop->top &= ~1;
	}

	crop->left = clamp_t(u32, crop->left, 0, sink->width - CCDC_MIN_WIDTH);
	crop->top = clamp_t(u32, crop->top, 0, sink->height - CCDC_MIN_HEIGHT);

	/* The data formatter truncates the number of horizontal output pixels
	 * to a multiple of 16. To avoid clipping data, allow callers to request
	 * an output size bigger than the input size up to the nearest multiple
	 * of 16.
	 */
	max_width = (sink->width - crop->left + 15) & ~15;
	crop->width = clamp_t(u32, crop->width, CCDC_MIN_WIDTH, max_width)
		    & ~15;
	crop->height = clamp_t(u32, crop->height, CCDC_MIN_HEIGHT,
			       sink->height - crop->top);

	/* Odd width/height values don't make sense for Bayer formats. */
	if (info->flavor != MEDIA_BUS_FMT_Y8_1X8) {
		crop->width &= ~1;
		crop->height &= ~1;
	}
}

/*
 * ccdc_enum_mbus_code - Handle pixel format enumeration
 * @sd     : pointer to v4l2 subdev structure
 * @cfg : V4L2 subdev pad configuration
 * @code   : pointer to v4l2_subdev_mbus_code_enum structure
 * return -EINVAL or zero on success
 */
static int ccdc_enum_mbus_code(struct v4l2_subdev *sd,
			       struct v4l2_subdev_pad_config *cfg,
			       struct v4l2_subdev_mbus_code_enum *code)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *format;

	switch (code->pad) {
	case CCDC_PAD_SINK:
		if (code->index >= ARRAY_SIZE(ccdc_fmts))
			return -EINVAL;

		code->code = ccdc_fmts[code->index];
		break;

	case CCDC_PAD_SOURCE_OF:
		format = __ccdc_get_format(ccdc, cfg, code->pad,
					   code->which);

		if (format->code == MEDIA_BUS_FMT_YUYV8_2X8 ||
		    format->code == MEDIA_BUS_FMT_UYVY8_2X8) {
			/* In YUV mode the CCDC can swap bytes. */
			if (code->index == 0)
				code->code = MEDIA_BUS_FMT_YUYV8_1X16;
			else if (code->index == 1)
				code->code = MEDIA_BUS_FMT_UYVY8_1X16;
			else
				return -EINVAL;
		} else {
			/* In raw mode, no configurable format confversion is
			 * available.
			 */
			if (code->index == 0)
				code->code = format->code;
			else
				return -EINVAL;
		}
		break;

	case CCDC_PAD_SOURCE_VP:
		/* The CCDC supports no configurable format conversion
		 * compatible with the video port. Enumerate a single output
		 * format code.
		 */
		if (code->index != 0)
			return -EINVAL;

		format = __ccdc_get_format(ccdc, cfg, code->pad,
					   code->which);

		/* A pixel code equal to 0 means that the video port doesn't
		 * support the input format. Don't enumerate any pixel code.
		 */
		if (format->code == 0)
			return -EINVAL;

		code->code = format->code;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int ccdc_enum_frame_size(struct v4l2_subdev *sd,
				struct v4l2_subdev_pad_config *cfg,
				struct v4l2_subdev_frame_size_enum *fse)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt format;

	if (fse->index != 0)
		return -EINVAL;

	format.code = fse->code;
	format.width = 1;
	format.height = 1;
	ccdc_try_format(ccdc, cfg, fse->pad, &format, fse->which);
	fse->min_width = format.width;
	fse->min_height = format.height;

	if (format.code != fse->code)
		return -EINVAL;

	format.code = fse->code;
	format.width = -1;
	format.height = -1;
	ccdc_try_format(ccdc, cfg, fse->pad, &format, fse->which);
	fse->max_width = format.width;
	fse->max_height = format.height;

	return 0;
}

/*
 * ccdc_get_selection - Retrieve a selection rectangle on a pad
 * @sd: ISP CCDC V4L2 subdevice
 * @cfg: V4L2 subdev pad configuration
 * @sel: Selection rectangle
 *
 * The only supported rectangles are the crop rectangles on the output formatter
 * source pad.
 *
 * Return 0 on success or a negative error code otherwise.
 */
static int ccdc_get_selection(struct v4l2_subdev *sd, struct v4l2_subdev_pad_config *cfg,
			      struct v4l2_subdev_selection *sel)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *format;

	if (sel->pad != CCDC_PAD_SOURCE_OF)
		return -EINVAL;

	switch (sel->target) {
	case V4L2_SEL_TGT_CROP_BOUNDS:
		sel->r.left = 0;
		sel->r.top = 0;
		sel->r.width = INT_MAX;
		sel->r.height = INT_MAX;

		format = __ccdc_get_format(ccdc, cfg, CCDC_PAD_SINK, sel->which);
		ccdc_try_crop(ccdc, format, &sel->r);
		break;

	case V4L2_SEL_TGT_CROP:
		sel->r = *__ccdc_get_crop(ccdc, cfg, sel->which);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/*
 * ccdc_set_selection - Set a selection rectangle on a pad
 * @sd: ISP CCDC V4L2 subdevice
 * @cfg: V4L2 subdev pad configuration
 * @sel: Selection rectangle
 *
 * The only supported rectangle is the actual crop rectangle on the output
 * formatter source pad.
 *
 * Return 0 on success or a negative error code otherwise.
 */
static int ccdc_set_selection(struct v4l2_subdev *sd, struct v4l2_subdev_pad_config *cfg,
			      struct v4l2_subdev_selection *sel)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *format;

	if (sel->target != V4L2_SEL_TGT_CROP ||
	    sel->pad != CCDC_PAD_SOURCE_OF)
		return -EINVAL;

	/* The crop rectangle can't be changed while streaming. */
	if (ccdc->state != ISP_PIPELINE_STREAM_STOPPED)
		return -EBUSY;

	/* Modifying the crop rectangle always changes the format on the source
	 * pad. If the KEEP_CONFIG flag is set, just return the current crop
	 * rectangle.
	 */
	if (sel->flags & V4L2_SEL_FLAG_KEEP_CONFIG) {
		sel->r = *__ccdc_get_crop(ccdc, cfg, sel->which);
		return 0;
	}

	format = __ccdc_get_format(ccdc, cfg, CCDC_PAD_SINK, sel->which);
	ccdc_try_crop(ccdc, format, &sel->r);
	*__ccdc_get_crop(ccdc, cfg, sel->which) = sel->r;

	/* Update the source format. */
	format = __ccdc_get_format(ccdc, cfg, CCDC_PAD_SOURCE_OF, sel->which);
	ccdc_try_format(ccdc, cfg, CCDC_PAD_SOURCE_OF, format, sel->which);

	return 0;
}

/*
 * ccdc_get_format - Retrieve the video format on a pad
 * @sd : ISP CCDC V4L2 subdevice
 * @cfg: V4L2 subdev pad configuration
 * @fmt: Format
 *
 * Return 0 on success or -EINVAL if the pad is invalid or doesn't correspond
 * to the format type.
 */
static int ccdc_get_format(struct v4l2_subdev *sd, struct v4l2_subdev_pad_config *cfg,
			   struct v4l2_subdev_format *fmt)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *format;

	format = __ccdc_get_format(ccdc, cfg, fmt->pad, fmt->which);
	if (format == NULL)
		return -EINVAL;

	fmt->format = *format;
	return 0;
}

/*
 * ccdc_set_format - Set the video format on a pad
 * @sd : ISP CCDC V4L2 subdevice
 * @cfg: V4L2 subdev pad configuration
 * @fmt: Format
 *
 * Return 0 on success or -EINVAL if the pad is invalid or doesn't correspond
 * to the format type.
 */
static int ccdc_set_format(struct v4l2_subdev *sd, struct v4l2_subdev_pad_config *cfg,
			   struct v4l2_subdev_format *fmt)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *format;
	struct v4l2_rect *crop;

	format = __ccdc_get_format(ccdc, cfg, fmt->pad, fmt->which);
	if (format == NULL)
		return -EINVAL;

	ccdc_try_format(ccdc, cfg, fmt->pad, &fmt->format, fmt->which);
	*format = fmt->format;

	/* Propagate the format from sink to source */
	if (fmt->pad == CCDC_PAD_SINK) {
		/* Reset the crop rectangle. */
		crop = __ccdc_get_crop(ccdc, cfg, fmt->which);
		crop->left = 0;
		crop->top = 0;
		crop->width = fmt->format.width;
		crop->height = fmt->format.height;

		ccdc_try_crop(ccdc, &fmt->format, crop);

		/* Update the source formats. */
		format = __ccdc_get_format(ccdc, cfg, CCDC_PAD_SOURCE_OF,
					   fmt->which);
		*format = fmt->format;
		ccdc_try_format(ccdc, cfg, CCDC_PAD_SOURCE_OF, format,
				fmt->which);

		format = __ccdc_get_format(ccdc, cfg, CCDC_PAD_SOURCE_VP,
					   fmt->which);
		*format = fmt->format;
		ccdc_try_format(ccdc, cfg, CCDC_PAD_SOURCE_VP, format,
				fmt->which);
	}

	return 0;
}

/*
 * Decide whether desired output pixel code can be obtained with
 * the lane shifter by shifting the input pixel code.
 * @in: input pixelcode to shifter
 * @out: output pixelcode from shifter
 * @additional_shift: # of bits the sensor's LSB is offset from CAMEXT[0]
 *
 * return true if the combination is possible
 * return false otherwise
 */
static bool ccdc_is_shiftable(u32 in, u32 out, unsigned int additional_shift)
{
	const struct isp_format_info *in_info, *out_info;

	if (in == out)
		return true;

	in_info = omap3isp_video_format_info(in);
	out_info = omap3isp_video_format_info(out);

	if ((in_info->flavor == 0) || (out_info->flavor == 0))
		return false;

	if (in_info->flavor != out_info->flavor)
		return false;

	return in_info->width - out_info->width + additional_shift <= 6;
}

static int ccdc_link_validate(struct v4l2_subdev *sd,
			      struct media_link *link,
			      struct v4l2_subdev_format *source_fmt,
			      struct v4l2_subdev_format *sink_fmt)
{
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	unsigned long parallel_shift;

	/* Check if the two ends match */
	if (source_fmt->format.width != sink_fmt->format.width ||
	    source_fmt->format.height != sink_fmt->format.height)
		return -EPIPE;

	/* We've got a parallel sensor here. */
	if (ccdc->input == CCDC_INPUT_PARALLEL) {
		struct v4l2_subdev *sd =
			media_entity_to_v4l2_subdev(link->source->entity);
		struct isp_bus_cfg *bus_cfg = v4l2_subdev_to_bus_cfg(sd);

		parallel_shift = bus_cfg->bus.parallel.data_lane_shift;
	} else {
		parallel_shift = 0;
	}

	/* Lane shifter may be used to drop bits on CCDC sink pad */
	if (!ccdc_is_shiftable(source_fmt->format.code,
			       sink_fmt->format.code, parallel_shift))
		return -EPIPE;

	return 0;
}

/*
 * ccdc_init_formats - Initialize formats on all pads
 * @sd: ISP CCDC V4L2 subdevice
 * @fh: V4L2 subdev file handle
 *
 * Initialize all pad formats with default values. If fh is not NULL, try
 * formats are initialized on the file handle. Otherwise active formats are
 * initialized on the device.
 */
static int ccdc_init_formats(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct v4l2_subdev_format format;

	memset(&format, 0, sizeof(format));
	format.pad = CCDC_PAD_SINK;
	format.which = fh ? V4L2_SUBDEV_FORMAT_TRY : V4L2_SUBDEV_FORMAT_ACTIVE;
	format.format.code = MEDIA_BUS_FMT_SGRBG10_1X10;
	format.format.width = 4096;
	format.format.height = 4096;
	ccdc_set_format(sd, fh ? fh->pad : NULL, &format);

	return 0;
}

/* V4L2 subdev core operations */
static const struct v4l2_subdev_core_ops ccdc_v4l2_core_ops = {
	.ioctl = ccdc_ioctl,
	.subscribe_event = ccdc_subscribe_event,
	.unsubscribe_event = ccdc_unsubscribe_event,
};

/* V4L2 subdev video operations */
static const struct v4l2_subdev_video_ops ccdc_v4l2_video_ops = {
	.s_stream = ccdc_set_stream,
};

/* V4L2 subdev pad operations */
static const struct v4l2_subdev_pad_ops ccdc_v4l2_pad_ops = {
	.enum_mbus_code = ccdc_enum_mbus_code,
	.enum_frame_size = ccdc_enum_frame_size,
	.get_fmt = ccdc_get_format,
	.set_fmt = ccdc_set_format,
	.get_selection = ccdc_get_selection,
	.set_selection = ccdc_set_selection,
	.link_validate = ccdc_link_validate,
};

/* V4L2 subdev operations */
static const struct v4l2_subdev_ops ccdc_v4l2_ops = {
	.core = &ccdc_v4l2_core_ops,
	.video = &ccdc_v4l2_video_ops,
	.pad = &ccdc_v4l2_pad_ops,
};

/* V4L2 subdev internal operations */
static const struct v4l2_subdev_internal_ops ccdc_v4l2_internal_ops = {
	.open = ccdc_init_formats,
};

/* -----------------------------------------------------------------------------
 * Media entity operations
 */

/*
 * ccdc_link_setup - Setup CCDC connections
 * @entity: CCDC media entity
 * @local: Pad at the local end of the link
 * @remote: Pad at the remote end of the link
 * @flags: Link flags
 *
 * return -EINVAL or zero on success
 */
static int ccdc_link_setup(struct media_entity *entity,
			   const struct media_pad *local,
			   const struct media_pad *remote, u32 flags)
{
	struct v4l2_subdev *sd = media_entity_to_v4l2_subdev(entity);
	struct isp_ccdc_device *ccdc = v4l2_get_subdevdata(sd);
	struct isp_device *isp = to_isp_device(ccdc);
	unsigned int index = local->index;

	/* FIXME: this is actually a hack! */
	if (is_media_entity_v4l2_subdev(remote->entity))
		index |= 2 << 16;

	switch (index) {
	case CCDC_PAD_SINK | 2 << 16:
		/* Read from the sensor (parallel interface), CCP2, CSI2a or
		 * CSI2c.
		 */
		if (!(flags & MEDIA_LNK_FL_ENABLED)) {
			ccdc->input = CCDC_INPUT_NONE;
			break;
		}

		if (ccdc->input != CCDC_INPUT_NONE)
			return -EBUSY;

		if (remote->entity == &isp->isp_ccp2.subdev.entity)
			ccdc->input = CCDC_INPUT_CCP2B;
		else if (remote->entity == &isp->isp_csi2a.subdev.entity)
			ccdc->input = CCDC_INPUT_CSI2A;
		else if (remote->entity == &isp->isp_csi2c.subdev.entity)
			ccdc->input = CCDC_INPUT_CSI2C;
		else
			ccdc->input = CCDC_INPUT_PARALLEL;

		break;

	/*
	 * The ISP core doesn't support pipelines with multiple video outputs.
	 * Revisit this when it will be implemented, and return -EBUSY for now.
	 */

	case CCDC_PAD_SOURCE_VP | 2 << 16:
		/* Write to preview engine, histogram and H3A. When none of
		 * those links are active, the video port can be disabled.
		 */
		if (flags & MEDIA_LNK_FL_ENABLED) {
			if (ccdc->output & ~CCDC_OUTPUT_PREVIEW)
				return -EBUSY;
			ccdc->output |= CCDC_OUTPUT_PREVIEW;
		} else {
			ccdc->output &= ~CCDC_OUTPUT_PREVIEW;
		}
		break;

	case CCDC_PAD_SOURCE_OF:
		/* Write to memory */
		if (flags & MEDIA_LNK_FL_ENABLED) {
			if (ccdc->output & ~CCDC_OUTPUT_MEMORY)
				return -EBUSY;
			ccdc->output |= CCDC_OUTPUT_MEMORY;
		} else {
			ccdc->output &= ~CCDC_OUTPUT_MEMORY;
		}
		break;

	case CCDC_PAD_SOURCE_OF | 2 << 16:
		/* Write to resizer */
		if (flags & MEDIA_LNK_FL_ENABLED) {
			if (ccdc->output & ~CCDC_OUTPUT_RESIZER)
				return -EBUSY;
			ccdc->output |= CCDC_OUTPUT_RESIZER;
		} else {
			ccdc->output &= ~CCDC_OUTPUT_RESIZER;
		}
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/* media operations */
static const struct media_entity_operations ccdc_media_ops = {
	.link_setup = ccdc_link_setup,
	.link_validate = v4l2_subdev_link_validate,
};

void omap3isp_ccdc_unregister_entities(struct isp_ccdc_device *ccdc)
{
	v4l2_device_unregister_subdev(&ccdc->subdev);
	omap3isp_video_unregister(&ccdc->video_out);
}

int omap3isp_ccdc_register_entities(struct isp_ccdc_device *ccdc,
	struct v4l2_device *vdev)
{
	int ret;

	/* Register the subdev and video node. */
	ccdc->subdev.dev = vdev->mdev->dev;
	ret = v4l2_device_register_subdev(vdev, &ccdc->subdev);
	if (ret < 0)
		goto error;

	ret = omap3isp_video_register(&ccdc->video_out, vdev);
	if (ret < 0)
		goto error;

	return 0;

error:
	omap3isp_ccdc_unregister_entities(ccdc);
	return ret;
}

/* -----------------------------------------------------------------------------
 * ISP CCDC initialisation and cleanup
 */

/*
 * ccdc_init_entities - Initialize V4L2 subdev and media entity
 * @ccdc: ISP CCDC module
 *
 * Return 0 on success and a negative error code on failure.
 */
static int ccdc_init_entities(struct isp_ccdc_device *ccdc)
{
	struct v4l2_subdev *sd = &ccdc->subdev;
	struct media_pad *pads = ccdc->pads;
	struct media_entity *me = &sd->entity;
	int ret;

	ccdc->input = CCDC_INPUT_NONE;

	v4l2_subdev_init(sd, &ccdc_v4l2_ops);
	sd->internal_ops = &ccdc_v4l2_internal_ops;
	strscpy(sd->name, "OMAP3 ISP CCDC", sizeof(sd->name));
	sd->grp_id = 1 << 16;	/* group ID for isp subdevs */
	v4l2_set_subdevdata(sd, ccdc);
	sd->flags |= V4L2_SUBDEV_FL_HAS_EVENTS | V4L2_SUBDEV_FL_HAS_DEVNODE;

	pads[CCDC_PAD_SINK].flags = MEDIA_PAD_FL_SINK
				    | MEDIA_PAD_FL_MUST_CONNECT;
	pads[CCDC_PAD_SOURCE_VP].flags = MEDIA_PAD_FL_SOURCE;
	pads[CCDC_PAD_SOURCE_OF].flags = MEDIA_PAD_FL_SOURCE;

	me->ops = &ccdc_media_ops;
	ret = media_entity_pads_init(me, CCDC_PADS_NUM, pads);
	if (ret < 0)
		return ret;

	ccdc_init_formats(sd, NULL);

	ccdc->video_out.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ccdc->video_out.ops = &ccdc_video_ops;
	ccdc->video_out.isp = to_isp_device(ccdc);
	ccdc->video_out.capture_mem = PAGE_ALIGN(4096 * 4096) * 3;
	ccdc->video_out.bpl_alignment = 32;

	ret = omap3isp_video_init(&ccdc->video_out, "CCDC");
	if (ret < 0)
		goto error;

	return 0;

error:
	media_entity_cleanup(me);
	return ret;
}

/*
 * omap3isp_ccdc_init - CCDC module initialization.
 * @isp: Device pointer specific to the OMAP3 ISP.
 *
 * TODO: Get the initialisation values from platform data.
 *
 * Return 0 on success or a negative error code otherwise.
 */
int omap3isp_ccdc_init(struct isp_device *isp)
{
	struct isp_ccdc_device *ccdc = &isp->isp_ccdc;
	int ret;

	spin_lock_init(&ccdc->lock);
	init_waitqueue_head(&ccdc->wait);
	mutex_init(&ccdc->ioctl_lock);

	ccdc->stopping = CCDC_STOP_NOT_REQUESTED;

	INIT_WORK(&ccdc->lsc.table_work, ccdc_lsc_free_table_work);
	ccdc->lsc.state = LSC_STATE_STOPPED;
	INIT_LIST_HEAD(&ccdc->lsc.free_queue);
	spin_lock_init(&ccdc->lsc.req_lock);

	ccdc->clamp.oblen = 0;
	ccdc->clamp.dcsubval = 0;

	ccdc->update = OMAP3ISP_CCDC_BLCLAMP;
	ccdc_apply_controls(ccdc);

	ret = ccdc_init_entities(ccdc);
	if (ret < 0) {
		mutex_destroy(&ccdc->ioctl_lock);
		return ret;
	}

	return 0;
}

/*
 * omap3isp_ccdc_cleanup - CCDC module cleanup.
 * @isp: Device pointer specific to the OMAP3 ISP.
 */
void omap3isp_ccdc_cleanup(struct isp_device *isp)
{
	struct isp_ccdc_device *ccdc = &isp->isp_ccdc;

	omap3isp_video_cleanup(&ccdc->video_out);
	media_entity_cleanup(&ccdc->subdev.entity);

	/* Free LSC requests. As the CCDC is stopped there's no active request,
	 * so only the pending request and the free queue need to be handled.
	 */
	ccdc_lsc_free_request(ccdc, ccdc->lsc.request);
	cancel_work_sync(&ccdc->lsc.table_work);
	ccdc_lsc_free_queue(ccdc, &ccdc->lsc.free_queue);

	if (ccdc->fpc.addr != NULL)
		dma_free_coherent(isp->dev, ccdc->fpc.fpnum * 4, ccdc->fpc.addr,
				  ccdc->fpc.dma);

	mutex_destroy(&ccdc->ioctl_lock);
}
