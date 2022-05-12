// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015 MediaTek Inc.
 */

#include <drm/drm_fourcc.h>

#include <linux/clk.h>
#include <linux/component.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/soc/mediatek/mtk-cmdq.h>

#include "mtk_disp_drv.h"
#include "mtk_drm_crtc.h"
#include "mtk_drm_ddp_comp.h"

#define DISP_REG_OVL_INTEN			0x0004
#define OVL_FME_CPL_INT					BIT(1)
#define DISP_REG_OVL_INTSTA			0x0008
#define DISP_REG_OVL_EN				0x000c
#define DISP_REG_OVL_RST			0x0014
#define DISP_REG_OVL_ROI_SIZE			0x0020
#define DISP_REG_OVL_DATAPATH_CON		0x0024
#define OVL_LAYER_SMI_ID_EN				BIT(0)
#define OVL_BGCLR_SEL_IN				BIT(2)
#define DISP_REG_OVL_ROI_BGCLR			0x0028
#define DISP_REG_OVL_SRC_CON			0x002c
#define DISP_REG_OVL_CON(n)			(0x0030 + 0x20 * (n))
#define DISP_REG_OVL_SRC_SIZE(n)		(0x0038 + 0x20 * (n))
#define DISP_REG_OVL_OFFSET(n)			(0x003c + 0x20 * (n))
#define DISP_REG_OVL_PITCH(n)			(0x0044 + 0x20 * (n))
#define DISP_REG_OVL_RDMA_CTRL(n)		(0x00c0 + 0x20 * (n))
#define DISP_REG_OVL_RDMA_GMC(n)		(0x00c8 + 0x20 * (n))
#define DISP_REG_OVL_ADDR_MT2701		0x0040
#define DISP_REG_OVL_ADDR_MT8173		0x0f40
#define DISP_REG_OVL_ADDR(ovl, n)		((ovl)->data->addr + 0x20 * (n))

#define GMC_THRESHOLD_BITS	16
#define GMC_THRESHOLD_HIGH	((1 << GMC_THRESHOLD_BITS) / 4)
#define GMC_THRESHOLD_LOW	((1 << GMC_THRESHOLD_BITS) / 8)

#define OVL_CON_BYTE_SWAP	BIT(24)
#define OVL_CON_MTX_YUV_TO_RGB	(6 << 16)
#define OVL_CON_CLRFMT_RGB	(1 << 12)
#define OVL_CON_CLRFMT_RGBA8888	(2 << 12)
#define OVL_CON_CLRFMT_ARGB8888	(3 << 12)
#define OVL_CON_CLRFMT_UYVY	(4 << 12)
#define OVL_CON_CLRFMT_YUYV	(5 << 12)
#define OVL_CON_CLRFMT_RGB565(ovl)	((ovl)->data->fmt_rgb565_is_0 ? \
					0 : OVL_CON_CLRFMT_RGB)
#define OVL_CON_CLRFMT_RGB888(ovl)	((ovl)->data->fmt_rgb565_is_0 ? \
					OVL_CON_CLRFMT_RGB : 0)
#define	OVL_CON_AEN		BIT(8)
#define	OVL_CON_ALPHA		0xff
#define	OVL_CON_VIRT_FLIP	BIT(9)
#define	OVL_CON_HORZ_FLIP	BIT(10)

struct mtk_disp_ovl_data {
	unsigned int addr;
	unsigned int gmc_bits;
	unsigned int layer_nr;
	bool fmt_rgb565_is_0;
	bool smi_id_en;
};

/*
 * struct mtk_disp_ovl - DISP_OVL driver structure
 * @crtc: associated crtc to report vblank events to
 * @data: platform data
 */
struct mtk_disp_ovl {
	struct drm_crtc			*crtc;
	struct clk			*clk;
	void __iomem			*regs;
	struct cmdq_client_reg		cmdq_reg;
	const struct mtk_disp_ovl_data	*data;
	void				(*vblank_cb)(void *data);
	void				*vblank_cb_data;
};

static irqreturn_t mtk_disp_ovl_irq_handler(int irq, void *dev_id)
{
	struct mtk_disp_ovl *priv = dev_id;

	/* Clear frame completion interrupt */
	writel(0x0, priv->regs + DISP_REG_OVL_INTSTA);

	if (!priv->vblank_cb)
		return IRQ_NONE;

	priv->vblank_cb(priv->vblank_cb_data);

	return IRQ_HANDLED;
}

void mtk_ovl_enable_vblank(struct device *dev,
			   void (*vblank_cb)(void *),
			   void *vblank_cb_data)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	ovl->vblank_cb = vblank_cb;
	ovl->vblank_cb_data = vblank_cb_data;
	writel(0x0, ovl->regs + DISP_REG_OVL_INTSTA);
	writel_relaxed(OVL_FME_CPL_INT, ovl->regs + DISP_REG_OVL_INTEN);
}

void mtk_ovl_disable_vblank(struct device *dev)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	ovl->vblank_cb = NULL;
	ovl->vblank_cb_data = NULL;
	writel_relaxed(0x0, ovl->regs + DISP_REG_OVL_INTEN);
}

int mtk_ovl_clk_enable(struct device *dev)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	return clk_prepare_enable(ovl->clk);
}

void mtk_ovl_clk_disable(struct device *dev)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	clk_disable_unprepare(ovl->clk);
}

void mtk_ovl_start(struct device *dev)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	if (ovl->data->smi_id_en) {
		unsigned int reg;

		reg = readl(ovl->regs + DISP_REG_OVL_DATAPATH_CON);
		reg = reg | OVL_LAYER_SMI_ID_EN;
		writel_relaxed(reg, ovl->regs + DISP_REG_OVL_DATAPATH_CON);
	}
	writel_relaxed(0x1, ovl->regs + DISP_REG_OVL_EN);
}

void mtk_ovl_stop(struct device *dev)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	writel_relaxed(0x0, ovl->regs + DISP_REG_OVL_EN);
	if (ovl->data->smi_id_en) {
		unsigned int reg;

		reg = readl(ovl->regs + DISP_REG_OVL_DATAPATH_CON);
		reg = reg & ~OVL_LAYER_SMI_ID_EN;
		writel_relaxed(reg, ovl->regs + DISP_REG_OVL_DATAPATH_CON);
	}

}

void mtk_ovl_config(struct device *dev, unsigned int w,
		    unsigned int h, unsigned int vrefresh,
		    unsigned int bpc, struct cmdq_pkt *cmdq_pkt)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	if (w != 0 && h != 0)
		mtk_ddp_write_relaxed(cmdq_pkt, h << 16 | w, &ovl->cmdq_reg, ovl->regs,
				      DISP_REG_OVL_ROI_SIZE);
	mtk_ddp_write_relaxed(cmdq_pkt, 0x0, &ovl->cmdq_reg, ovl->regs, DISP_REG_OVL_ROI_BGCLR);

	mtk_ddp_write(cmdq_pkt, 0x1, &ovl->cmdq_reg, ovl->regs, DISP_REG_OVL_RST);
	mtk_ddp_write(cmdq_pkt, 0x0, &ovl->cmdq_reg, ovl->regs, DISP_REG_OVL_RST);
}

unsigned int mtk_ovl_layer_nr(struct device *dev)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	return ovl->data->layer_nr;
}

unsigned int mtk_ovl_supported_rotations(struct device *dev)
{
	return DRM_MODE_ROTATE_0 | DRM_MODE_ROTATE_180 |
	       DRM_MODE_REFLECT_X | DRM_MODE_REFLECT_Y;
}

int mtk_ovl_layer_check(struct device *dev, unsigned int idx,
			struct mtk_plane_state *mtk_state)
{
	struct drm_plane_state *state = &mtk_state->base;
	unsigned int rotation = 0;

	rotation = drm_rotation_simplify(state->rotation,
					 DRM_MODE_ROTATE_0 |
					 DRM_MODE_REFLECT_X |
					 DRM_MODE_REFLECT_Y);
	rotation &= ~DRM_MODE_ROTATE_0;

	/* We can only do reflection, not rotation */
	if ((rotation & DRM_MODE_ROTATE_MASK) != 0)
		return -EINVAL;

	/*
	 * TODO: Rotating/reflecting YUV buffers is not supported at this time.
	 *	 Only RGB[AX] variants are supported.
	 */
	if (state->fb->format->is_yuv && rotation != 0)
		return -EINVAL;

	state->rotation = rotation;

	return 0;
}

void mtk_ovl_layer_on(struct device *dev, unsigned int idx,
		      struct cmdq_pkt *cmdq_pkt)
{
	unsigned int gmc_thrshd_l;
	unsigned int gmc_thrshd_h;
	unsigned int gmc_value;
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	mtk_ddp_write(cmdq_pkt, 0x1, &ovl->cmdq_reg, ovl->regs,
		      DISP_REG_OVL_RDMA_CTRL(idx));
	gmc_thrshd_l = GMC_THRESHOLD_LOW >>
		      (GMC_THRESHOLD_BITS - ovl->data->gmc_bits);
	gmc_thrshd_h = GMC_THRESHOLD_HIGH >>
		      (GMC_THRESHOLD_BITS - ovl->data->gmc_bits);
	if (ovl->data->gmc_bits == 10)
		gmc_value = gmc_thrshd_h | gmc_thrshd_h << 16;
	else
		gmc_value = gmc_thrshd_l | gmc_thrshd_l << 8 |
			    gmc_thrshd_h << 16 | gmc_thrshd_h << 24;
	mtk_ddp_write(cmdq_pkt, gmc_value,
		      &ovl->cmdq_reg, ovl->regs, DISP_REG_OVL_RDMA_GMC(idx));
	mtk_ddp_write_mask(cmdq_pkt, BIT(idx), &ovl->cmdq_reg, ovl->regs,
			   DISP_REG_OVL_SRC_CON, BIT(idx));
}

void mtk_ovl_layer_off(struct device *dev, unsigned int idx,
		       struct cmdq_pkt *cmdq_pkt)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);

	mtk_ddp_write_mask(cmdq_pkt, 0, &ovl->cmdq_reg, ovl->regs,
			   DISP_REG_OVL_SRC_CON, BIT(idx));
	mtk_ddp_write(cmdq_pkt, 0, &ovl->cmdq_reg, ovl->regs,
		      DISP_REG_OVL_RDMA_CTRL(idx));
}

static unsigned int ovl_fmt_convert(struct mtk_disp_ovl *ovl, unsigned int fmt)
{
	/* The return value in switch "MEM_MODE_INPUT_FORMAT_XXX"
	 * is defined in mediatek HW data sheet.
	 * The alphabet order in XXX is no relation to data
	 * arrangement in memory.
	 */
	switch (fmt) {
	default:
	case DRM_FORMAT_RGB565:
		return OVL_CON_CLRFMT_RGB565(ovl);
	case DRM_FORMAT_BGR565:
		return OVL_CON_CLRFMT_RGB565(ovl) | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_RGB888:
		return OVL_CON_CLRFMT_RGB888(ovl);
	case DRM_FORMAT_BGR888:
		return OVL_CON_CLRFMT_RGB888(ovl) | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_RGBX8888:
	case DRM_FORMAT_RGBA8888:
		return OVL_CON_CLRFMT_ARGB8888;
	case DRM_FORMAT_BGRX8888:
	case DRM_FORMAT_BGRA8888:
		return OVL_CON_CLRFMT_ARGB8888 | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ARGB8888:
		return OVL_CON_CLRFMT_RGBA8888;
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ABGR8888:
		return OVL_CON_CLRFMT_RGBA8888 | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_UYVY:
		return OVL_CON_CLRFMT_UYVY | OVL_CON_MTX_YUV_TO_RGB;
	case DRM_FORMAT_YUYV:
		return OVL_CON_CLRFMT_YUYV | OVL_CON_MTX_YUV_TO_RGB;
	}
}

void mtk_ovl_layer_config(struct device *dev, unsigned int idx,
			  struct mtk_plane_state *state,
			  struct cmdq_pkt *cmdq_pkt)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);
	struct mtk_plane_pending_state *pending = &state->pending;
	unsigned int addr = pending->addr;
	unsigned int pitch = pending->pitch & 0xffff;
	unsigned int fmt = pending->format;
	unsigned int offset = (pending->y << 16) | pending->x;
	unsigned int src_size = (pending->height << 16) | pending->width;
	unsigned int con;

	if (!pending->enable) {
		mtk_ovl_layer_off(dev, idx, cmdq_pkt);
		return;
	}

	con = ovl_fmt_convert(ovl, fmt);
	if (state->base.fb && state->base.fb->format->has_alpha)
		con |= OVL_CON_AEN | OVL_CON_ALPHA;

	if (pending->rotation & DRM_MODE_REFLECT_Y) {
		con |= OVL_CON_VIRT_FLIP;
		addr += (pending->height - 1) * pending->pitch;
	}

	if (pending->rotation & DRM_MODE_REFLECT_X) {
		con |= OVL_CON_HORZ_FLIP;
		addr += pending->pitch - 1;
	}

	mtk_ddp_write_relaxed(cmdq_pkt, con, &ovl->cmdq_reg, ovl->regs,
			      DISP_REG_OVL_CON(idx));
	mtk_ddp_write_relaxed(cmdq_pkt, pitch, &ovl->cmdq_reg, ovl->regs,
			      DISP_REG_OVL_PITCH(idx));
	mtk_ddp_write_relaxed(cmdq_pkt, src_size, &ovl->cmdq_reg, ovl->regs,
			      DISP_REG_OVL_SRC_SIZE(idx));
	mtk_ddp_write_relaxed(cmdq_pkt, offset, &ovl->cmdq_reg, ovl->regs,
			      DISP_REG_OVL_OFFSET(idx));
	mtk_ddp_write_relaxed(cmdq_pkt, addr, &ovl->cmdq_reg, ovl->regs,
			      DISP_REG_OVL_ADDR(ovl, idx));

	mtk_ovl_layer_on(dev, idx, cmdq_pkt);
}

void mtk_ovl_bgclr_in_on(struct device *dev)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);
	unsigned int reg;

	reg = readl(ovl->regs + DISP_REG_OVL_DATAPATH_CON);
	reg = reg | OVL_BGCLR_SEL_IN;
	writel(reg, ovl->regs + DISP_REG_OVL_DATAPATH_CON);
}

void mtk_ovl_bgclr_in_off(struct device *dev)
{
	struct mtk_disp_ovl *ovl = dev_get_drvdata(dev);
	unsigned int reg;

	reg = readl(ovl->regs + DISP_REG_OVL_DATAPATH_CON);
	reg = reg & ~OVL_BGCLR_SEL_IN;
	writel(reg, ovl->regs + DISP_REG_OVL_DATAPATH_CON);
}

static int mtk_disp_ovl_bind(struct device *dev, struct device *master,
			     void *data)
{
	return 0;
}

static void mtk_disp_ovl_unbind(struct device *dev, struct device *master,
				void *data)
{
}

static const struct component_ops mtk_disp_ovl_component_ops = {
	.bind	= mtk_disp_ovl_bind,
	.unbind = mtk_disp_ovl_unbind,
};

static int mtk_disp_ovl_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct mtk_disp_ovl *priv;
	struct resource *res;
	int irq;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	priv->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(priv->clk)) {
		dev_err(dev, "failed to get ovl clk\n");
		return PTR_ERR(priv->clk);
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->regs)) {
		dev_err(dev, "failed to ioremap ovl\n");
		return PTR_ERR(priv->regs);
	}
#if IS_REACHABLE(CONFIG_MTK_CMDQ)
	ret = cmdq_dev_get_client_reg(dev, &priv->cmdq_reg, 0);
	if (ret)
		dev_dbg(dev, "get mediatek,gce-client-reg fail!\n");
#endif

	priv->data = of_device_get_match_data(dev);
	platform_set_drvdata(pdev, priv);

	ret = devm_request_irq(dev, irq, mtk_disp_ovl_irq_handler,
			       IRQF_TRIGGER_NONE, dev_name(dev), priv);
	if (ret < 0) {
		dev_err(dev, "Failed to request irq %d: %d\n", irq, ret);
		return ret;
	}

	ret = component_add(dev, &mtk_disp_ovl_component_ops);
	if (ret)
		dev_err(dev, "Failed to add component: %d\n", ret);

	return ret;
}

static int mtk_disp_ovl_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct mtk_disp_ovl_data mt2701_ovl_driver_data = {
	.addr = DISP_REG_OVL_ADDR_MT2701,
	.gmc_bits = 8,
	.layer_nr = 4,
	.fmt_rgb565_is_0 = false,
};

static const struct mtk_disp_ovl_data mt8173_ovl_driver_data = {
	.addr = DISP_REG_OVL_ADDR_MT8173,
	.gmc_bits = 8,
	.layer_nr = 4,
	.fmt_rgb565_is_0 = true,
};

static const struct mtk_disp_ovl_data mt8183_ovl_driver_data = {
	.addr = DISP_REG_OVL_ADDR_MT8173,
	.gmc_bits = 10,
	.layer_nr = 4,
	.fmt_rgb565_is_0 = true,
};

static const struct mtk_disp_ovl_data mt8183_ovl_2l_driver_data = {
	.addr = DISP_REG_OVL_ADDR_MT8173,
	.gmc_bits = 10,
	.layer_nr = 2,
	.fmt_rgb565_is_0 = true,
};

static const struct of_device_id mtk_disp_ovl_driver_dt_match[] = {
	{ .compatible = "mediatek,mt2701-disp-ovl",
	  .data = &mt2701_ovl_driver_data},
	{ .compatible = "mediatek,mt8173-disp-ovl",
	  .data = &mt8173_ovl_driver_data},
	{ .compatible = "mediatek,mt8183-disp-ovl",
	  .data = &mt8183_ovl_driver_data},
	{ .compatible = "mediatek,mt8183-disp-ovl-2l",
	  .data = &mt8183_ovl_2l_driver_data},
	{},
};
MODULE_DEVICE_TABLE(of, mtk_disp_ovl_driver_dt_match);

struct platform_driver mtk_disp_ovl_driver = {
	.probe		= mtk_disp_ovl_probe,
	.remove		= mtk_disp_ovl_remove,
	.driver		= {
		.name	= "mediatek-disp-ovl",
		.owner	= THIS_MODULE,
		.of_match_table = mtk_disp_ovl_driver_dt_match,
	},
};
