// SPDX-License-Identifier: GPL-2.0-only
/* sound/soc/rockchip/rockchip_i2s.c
 *
 * ALSA SoC Audio Layer - Rockchip I2S Controller driver
 *
 * Copyright (c) 2014 Rockchip Electronics Co. Ltd.
 * Author: Jianqun <jay.xu@rock-chips.com>
 */

#include <linux/module.h>
#include <linux/mfd/syscon.h>
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/clk.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <sound/pcm_params.h>
#include <sound/dmaengine_pcm.h>

#include "rockchip_i2s.h"
#include "rockchip_pcm.h"

#define DRV_NAME "rockchip-i2s"

struct rk_i2s_pins {
	u32 reg_offset;
	u32 shift;
};

struct rk_i2s_dev {
	struct device *dev;

	struct clk *hclk;
	struct clk *mclk;

	struct snd_dmaengine_dai_dma_data capture_dma_data;
	struct snd_dmaengine_dai_dma_data playback_dma_data;

	struct regmap *regmap;
	struct regmap *grf;

/*
 * Used to indicate the tx/rx status.
 * I2S controller hopes to start the tx and rx together,
 * also to stop them when they are both try to stop.
*/
	bool tx_start;
	bool rx_start;
	bool is_master_mode;
	const struct rk_i2s_pins *pins;
};

static int i2s_runtime_suspend(struct device *dev)
{
	struct rk_i2s_dev *i2s = dev_get_drvdata(dev);

	regcache_cache_only(i2s->regmap, true);
	clk_disable_unprepare(i2s->mclk);

	return 0;
}

static int i2s_runtime_resume(struct device *dev)
{
	struct rk_i2s_dev *i2s = dev_get_drvdata(dev);
	int ret;

	ret = clk_prepare_enable(i2s->mclk);
	if (ret) {
		dev_err(i2s->dev, "clock enable failed %d\n", ret);
		return ret;
	}

	regcache_cache_only(i2s->regmap, false);
	regcache_mark_dirty(i2s->regmap);

	ret = regcache_sync(i2s->regmap);
	if (ret)
		clk_disable_unprepare(i2s->mclk);

	return ret;
}

static inline struct rk_i2s_dev *to_info(struct snd_soc_dai *dai)
{
	return snd_soc_dai_get_drvdata(dai);
}

static void rockchip_snd_txctrl(struct rk_i2s_dev *i2s, int on)
{
	unsigned int val = 0;
	int retry = 10;

	if (on) {
		regmap_update_bits(i2s->regmap, I2S_DMACR,
				   I2S_DMACR_TDE_ENABLE, I2S_DMACR_TDE_ENABLE);

		regmap_update_bits(i2s->regmap, I2S_XFER,
				   I2S_XFER_TXS_START | I2S_XFER_RXS_START,
				   I2S_XFER_TXS_START | I2S_XFER_RXS_START);

		i2s->tx_start = true;
	} else {
		i2s->tx_start = false;

		regmap_update_bits(i2s->regmap, I2S_DMACR,
				   I2S_DMACR_TDE_ENABLE, I2S_DMACR_TDE_DISABLE);

		if (!i2s->rx_start) {
			regmap_update_bits(i2s->regmap, I2S_XFER,
					   I2S_XFER_TXS_START |
					   I2S_XFER_RXS_START,
					   I2S_XFER_TXS_STOP |
					   I2S_XFER_RXS_STOP);

			udelay(150);
			regmap_update_bits(i2s->regmap, I2S_CLR,
					   I2S_CLR_TXC | I2S_CLR_RXC,
					   I2S_CLR_TXC | I2S_CLR_RXC);

			regmap_read(i2s->regmap, I2S_CLR, &val);

			/* Should wait for clear operation to finish */
			while (val) {
				regmap_read(i2s->regmap, I2S_CLR, &val);
				retry--;
				if (!retry) {
					dev_warn(i2s->dev, "fail to clear\n");
					break;
				}
			}
		}
	}
}

static void rockchip_snd_rxctrl(struct rk_i2s_dev *i2s, int on)
{
	unsigned int val = 0;
	int retry = 10;

	if (on) {
		regmap_update_bits(i2s->regmap, I2S_DMACR,
				   I2S_DMACR_RDE_ENABLE, I2S_DMACR_RDE_ENABLE);

		regmap_update_bits(i2s->regmap, I2S_XFER,
				   I2S_XFER_TXS_START | I2S_XFER_RXS_START,
				   I2S_XFER_TXS_START | I2S_XFER_RXS_START);

		i2s->rx_start = true;
	} else {
		i2s->rx_start = false;

		regmap_update_bits(i2s->regmap, I2S_DMACR,
				   I2S_DMACR_RDE_ENABLE, I2S_DMACR_RDE_DISABLE);

		if (!i2s->tx_start) {
			regmap_update_bits(i2s->regmap, I2S_XFER,
					   I2S_XFER_TXS_START |
					   I2S_XFER_RXS_START,
					   I2S_XFER_TXS_STOP |
					   I2S_XFER_RXS_STOP);

			udelay(150);
			regmap_update_bits(i2s->regmap, I2S_CLR,
					   I2S_CLR_TXC | I2S_CLR_RXC,
					   I2S_CLR_TXC | I2S_CLR_RXC);

			regmap_read(i2s->regmap, I2S_CLR, &val);

			/* Should wait for clear operation to finish */
			while (val) {
				regmap_read(i2s->regmap, I2S_CLR, &val);
				retry--;
				if (!retry) {
					dev_warn(i2s->dev, "fail to clear\n");
					break;
				}
			}
		}
	}
}

static int rockchip_i2s_set_fmt(struct snd_soc_dai *cpu_dai,
				unsigned int fmt)
{
	struct rk_i2s_dev *i2s = to_info(cpu_dai);
	unsigned int mask = 0, val = 0;

	mask = I2S_CKR_MSS_MASK;
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		/* Set source clock in Master mode */
		val = I2S_CKR_MSS_MASTER;
		i2s->is_master_mode = true;
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		val = I2S_CKR_MSS_SLAVE;
		i2s->is_master_mode = false;
		break;
	default:
		return -EINVAL;
	}

	regmap_update_bits(i2s->regmap, I2S_CKR, mask, val);

	mask = I2S_CKR_CKP_MASK;
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		val = I2S_CKR_CKP_NEG;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		val = I2S_CKR_CKP_POS;
		break;
	default:
		return -EINVAL;
	}

	regmap_update_bits(i2s->regmap, I2S_CKR, mask, val);

	mask = I2S_TXCR_IBM_MASK | I2S_TXCR_TFS_MASK | I2S_TXCR_PBM_MASK;
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_RIGHT_J:
		val = I2S_TXCR_IBM_RSJM;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		val = I2S_TXCR_IBM_LSJM;
		break;
	case SND_SOC_DAIFMT_I2S:
		val = I2S_TXCR_IBM_NORMAL;
		break;
	case SND_SOC_DAIFMT_DSP_A: /* PCM no delay mode */
		val = I2S_TXCR_TFS_PCM;
		break;
	case SND_SOC_DAIFMT_DSP_B: /* PCM delay 1 mode */
		val = I2S_TXCR_TFS_PCM | I2S_TXCR_PBM_MODE(1);
		break;
	default:
		return -EINVAL;
	}

	regmap_update_bits(i2s->regmap, I2S_TXCR, mask, val);

	mask = I2S_RXCR_IBM_MASK | I2S_RXCR_TFS_MASK | I2S_RXCR_PBM_MASK;
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_RIGHT_J:
		val = I2S_RXCR_IBM_RSJM;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		val = I2S_RXCR_IBM_LSJM;
		break;
	case SND_SOC_DAIFMT_I2S:
		val = I2S_RXCR_IBM_NORMAL;
		break;
	case SND_SOC_DAIFMT_DSP_A: /* PCM no delay mode */
		val = I2S_RXCR_TFS_PCM;
		break;
	case SND_SOC_DAIFMT_DSP_B: /* PCM delay 1 mode */
		val = I2S_RXCR_TFS_PCM | I2S_RXCR_PBM_MODE(1);
		break;
	default:
		return -EINVAL;
	}

	regmap_update_bits(i2s->regmap, I2S_RXCR, mask, val);

	return 0;
}

static int rockchip_i2s_hw_params(struct snd_pcm_substream *substream,
				  struct snd_pcm_hw_params *params,
				  struct snd_soc_dai *dai)
{
	struct rk_i2s_dev *i2s = to_info(dai);
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	unsigned int val = 0;
	unsigned int mclk_rate, bclk_rate, div_bclk, div_lrck;

	if (i2s->is_master_mode) {
		mclk_rate = clk_get_rate(i2s->mclk);
		bclk_rate = 2 * 32 * params_rate(params);
		if (bclk_rate == 0 || mclk_rate % bclk_rate)
			return -EINVAL;

		div_bclk = mclk_rate / bclk_rate;
		div_lrck = bclk_rate / params_rate(params);
		regmap_update_bits(i2s->regmap, I2S_CKR,
				   I2S_CKR_MDIV_MASK,
				   I2S_CKR_MDIV(div_bclk));

		regmap_update_bits(i2s->regmap, I2S_CKR,
				   I2S_CKR_TSD_MASK |
				   I2S_CKR_RSD_MASK,
				   I2S_CKR_TSD(div_lrck) |
				   I2S_CKR_RSD(div_lrck));
	}

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S8:
		val |= I2S_TXCR_VDW(8);
		break;
	case SNDRV_PCM_FORMAT_S16_LE:
		val |= I2S_TXCR_VDW(16);
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		val |= I2S_TXCR_VDW(20);
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		val |= I2S_TXCR_VDW(24);
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		val |= I2S_TXCR_VDW(32);
		break;
	default:
		return -EINVAL;
	}

	switch (params_channels(params)) {
	case 8:
		val |= I2S_CHN_8;
		break;
	case 6:
		val |= I2S_CHN_6;
		break;
	case 4:
		val |= I2S_CHN_4;
		break;
	case 2:
		val |= I2S_CHN_2;
		break;
	default:
		dev_err(i2s->dev, "invalid channel: %d\n",
			params_channels(params));
		return -EINVAL;
	}

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
		regmap_update_bits(i2s->regmap, I2S_RXCR,
				   I2S_RXCR_VDW_MASK | I2S_RXCR_CSR_MASK,
				   val);
	else
		regmap_update_bits(i2s->regmap, I2S_TXCR,
				   I2S_TXCR_VDW_MASK | I2S_TXCR_CSR_MASK,
				   val);

	if (!IS_ERR(i2s->grf) && i2s->pins) {
		regmap_read(i2s->regmap, I2S_TXCR, &val);
		val &= I2S_TXCR_CSR_MASK;

		switch (val) {
		case I2S_CHN_4:
			val = I2S_IO_4CH_OUT_6CH_IN;
			break;
		case I2S_CHN_6:
			val = I2S_IO_6CH_OUT_4CH_IN;
			break;
		case I2S_CHN_8:
			val = I2S_IO_8CH_OUT_2CH_IN;
			break;
		default:
			val = I2S_IO_2CH_OUT_8CH_IN;
			break;
		}

		val <<= i2s->pins->shift;
		val |= (I2S_IO_DIRECTION_MASK << i2s->pins->shift) << 16;
		regmap_write(i2s->grf, i2s->pins->reg_offset, val);
	}

	regmap_update_bits(i2s->regmap, I2S_DMACR, I2S_DMACR_TDL_MASK,
			   I2S_DMACR_TDL(16));
	regmap_update_bits(i2s->regmap, I2S_DMACR, I2S_DMACR_RDL_MASK,
			   I2S_DMACR_RDL(16));

	val = I2S_CKR_TRCM_TXRX;
	if (dai->driver->symmetric_rate && rtd->dai_link->symmetric_rate)
		val = I2S_CKR_TRCM_TXONLY;

	regmap_update_bits(i2s->regmap, I2S_CKR,
			   I2S_CKR_TRCM_MASK,
			   val);
	return 0;
}

static int rockchip_i2s_trigger(struct snd_pcm_substream *substream,
				int cmd, struct snd_soc_dai *dai)
{
	struct rk_i2s_dev *i2s = to_info(dai);
	int ret = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			rockchip_snd_rxctrl(i2s, 1);
		else
			rockchip_snd_txctrl(i2s, 1);
		break;
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			rockchip_snd_rxctrl(i2s, 0);
		else
			rockchip_snd_txctrl(i2s, 0);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int rockchip_i2s_set_sysclk(struct snd_soc_dai *cpu_dai, int clk_id,
				   unsigned int freq, int dir)
{
	struct rk_i2s_dev *i2s = to_info(cpu_dai);
	int ret;

	if (freq == 0)
		return 0;

	ret = clk_set_rate(i2s->mclk, freq);
	if (ret)
		dev_err(i2s->dev, "Fail to set mclk %d\n", ret);

	return ret;
}

static int rockchip_i2s_dai_probe(struct snd_soc_dai *dai)
{
	struct rk_i2s_dev *i2s = snd_soc_dai_get_drvdata(dai);

	dai->capture_dma_data = &i2s->capture_dma_data;
	dai->playback_dma_data = &i2s->playback_dma_data;

	return 0;
}

static const struct snd_soc_dai_ops rockchip_i2s_dai_ops = {
	.hw_params = rockchip_i2s_hw_params,
	.set_sysclk = rockchip_i2s_set_sysclk,
	.set_fmt = rockchip_i2s_set_fmt,
	.trigger = rockchip_i2s_trigger,
};

static struct snd_soc_dai_driver rockchip_i2s_dai = {
	.probe = rockchip_i2s_dai_probe,
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 8,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = (SNDRV_PCM_FMTBIT_S8 |
			    SNDRV_PCM_FMTBIT_S16_LE |
			    SNDRV_PCM_FMTBIT_S20_3LE |
			    SNDRV_PCM_FMTBIT_S24_LE |
			    SNDRV_PCM_FMTBIT_S32_LE),
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = (SNDRV_PCM_FMTBIT_S8 |
			    SNDRV_PCM_FMTBIT_S16_LE |
			    SNDRV_PCM_FMTBIT_S20_3LE |
			    SNDRV_PCM_FMTBIT_S24_LE |
			    SNDRV_PCM_FMTBIT_S32_LE),
	},
	.ops = &rockchip_i2s_dai_ops,
	.symmetric_rate = 1,
};

static const struct snd_soc_component_driver rockchip_i2s_component = {
	.name = DRV_NAME,
};

static bool rockchip_i2s_wr_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case I2S_TXCR:
	case I2S_RXCR:
	case I2S_CKR:
	case I2S_DMACR:
	case I2S_INTCR:
	case I2S_XFER:
	case I2S_CLR:
	case I2S_TXDR:
		return true;
	default:
		return false;
	}
}

static bool rockchip_i2s_rd_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case I2S_TXCR:
	case I2S_RXCR:
	case I2S_CKR:
	case I2S_DMACR:
	case I2S_INTCR:
	case I2S_XFER:
	case I2S_CLR:
	case I2S_TXDR:
	case I2S_RXDR:
	case I2S_FIFOLR:
	case I2S_INTSR:
		return true;
	default:
		return false;
	}
}

static bool rockchip_i2s_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case I2S_INTSR:
	case I2S_CLR:
	case I2S_FIFOLR:
	case I2S_TXDR:
	case I2S_RXDR:
		return true;
	default:
		return false;
	}
}

static bool rockchip_i2s_precious_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case I2S_RXDR:
		return true;
	default:
		return false;
	}
}

static const struct reg_default rockchip_i2s_reg_defaults[] = {
	{0x00, 0x0000000f},
	{0x04, 0x0000000f},
	{0x08, 0x00071f1f},
	{0x10, 0x001f0000},
	{0x14, 0x01f00000},
};

static const struct regmap_config rockchip_i2s_regmap_config = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.max_register = I2S_RXDR,
	.reg_defaults = rockchip_i2s_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(rockchip_i2s_reg_defaults),
	.writeable_reg = rockchip_i2s_wr_reg,
	.readable_reg = rockchip_i2s_rd_reg,
	.volatile_reg = rockchip_i2s_volatile_reg,
	.precious_reg = rockchip_i2s_precious_reg,
	.cache_type = REGCACHE_FLAT,
};

static const struct rk_i2s_pins rk3399_i2s_pins = {
	.reg_offset = 0xe220,
	.shift = 11,
};

static const struct of_device_id rockchip_i2s_match[] __maybe_unused = {
	{ .compatible = "rockchip,rk3066-i2s", },
	{ .compatible = "rockchip,rk3188-i2s", },
	{ .compatible = "rockchip,rk3288-i2s", },
	{ .compatible = "rockchip,rk3399-i2s", .data = &rk3399_i2s_pins },
	{},
};

static int rockchip_i2s_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	const struct of_device_id *of_id;
	struct rk_i2s_dev *i2s;
	struct snd_soc_dai_driver *soc_dai;
	struct resource *res;
	void __iomem *regs;
	int ret;
	int val;

	i2s = devm_kzalloc(&pdev->dev, sizeof(*i2s), GFP_KERNEL);
	if (!i2s)
		return -ENOMEM;

	i2s->dev = &pdev->dev;

	i2s->grf = syscon_regmap_lookup_by_phandle(node, "rockchip,grf");
	if (!IS_ERR(i2s->grf)) {
		of_id = of_match_device(rockchip_i2s_match, &pdev->dev);
		if (!of_id || !of_id->data)
			return -EINVAL;

		i2s->pins = of_id->data;
	}

	/* try to prepare related clocks */
	i2s->hclk = devm_clk_get(&pdev->dev, "i2s_hclk");
	if (IS_ERR(i2s->hclk)) {
		dev_err(&pdev->dev, "Can't retrieve i2s bus clock\n");
		return PTR_ERR(i2s->hclk);
	}
	ret = clk_prepare_enable(i2s->hclk);
	if (ret) {
		dev_err(i2s->dev, "hclock enable failed %d\n", ret);
		return ret;
	}

	i2s->mclk = devm_clk_get(&pdev->dev, "i2s_clk");
	if (IS_ERR(i2s->mclk)) {
		dev_err(&pdev->dev, "Can't retrieve i2s master clock\n");
		return PTR_ERR(i2s->mclk);
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(regs))
		return PTR_ERR(regs);

	i2s->regmap = devm_regmap_init_mmio(&pdev->dev, regs,
					    &rockchip_i2s_regmap_config);
	if (IS_ERR(i2s->regmap)) {
		dev_err(&pdev->dev,
			"Failed to initialise managed register map\n");
		return PTR_ERR(i2s->regmap);
	}

	i2s->playback_dma_data.addr = res->start + I2S_TXDR;
	i2s->playback_dma_data.addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	i2s->playback_dma_data.maxburst = 4;

	i2s->capture_dma_data.addr = res->start + I2S_RXDR;
	i2s->capture_dma_data.addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	i2s->capture_dma_data.maxburst = 4;

	dev_set_drvdata(&pdev->dev, i2s);

	pm_runtime_enable(&pdev->dev);
	if (!pm_runtime_enabled(&pdev->dev)) {
		ret = i2s_runtime_resume(&pdev->dev);
		if (ret)
			goto err_pm_disable;
	}

	soc_dai = devm_kmemdup(&pdev->dev, &rockchip_i2s_dai,
			       sizeof(*soc_dai), GFP_KERNEL);
	if (!soc_dai) {
		ret = -ENOMEM;
		goto err_pm_disable;
	}

	if (!of_property_read_u32(node, "rockchip,playback-channels", &val)) {
		if (val >= 2 && val <= 8)
			soc_dai->playback.channels_max = val;
	}

	if (!of_property_read_u32(node, "rockchip,capture-channels", &val)) {
		if (val >= 2 && val <= 8)
			soc_dai->capture.channels_max = val;
	}

	ret = devm_snd_soc_register_component(&pdev->dev,
					      &rockchip_i2s_component,
					      soc_dai, 1);

	if (ret) {
		dev_err(&pdev->dev, "Could not register DAI\n");
		goto err_suspend;
	}

	ret = rockchip_pcm_platform_register(&pdev->dev);
	if (ret) {
		dev_err(&pdev->dev, "Could not register PCM\n");
		goto err_suspend;
	}

	return 0;

err_suspend:
	if (!pm_runtime_status_suspended(&pdev->dev))
		i2s_runtime_suspend(&pdev->dev);
err_pm_disable:
	pm_runtime_disable(&pdev->dev);

	return ret;
}

static int rockchip_i2s_remove(struct platform_device *pdev)
{
	struct rk_i2s_dev *i2s = dev_get_drvdata(&pdev->dev);

	pm_runtime_disable(&pdev->dev);
	if (!pm_runtime_status_suspended(&pdev->dev))
		i2s_runtime_suspend(&pdev->dev);

	clk_disable_unprepare(i2s->hclk);

	return 0;
}

static const struct dev_pm_ops rockchip_i2s_pm_ops = {
	SET_RUNTIME_PM_OPS(i2s_runtime_suspend, i2s_runtime_resume,
			   NULL)
};

static struct platform_driver rockchip_i2s_driver = {
	.probe = rockchip_i2s_probe,
	.remove = rockchip_i2s_remove,
	.driver = {
		.name = DRV_NAME,
		.of_match_table = of_match_ptr(rockchip_i2s_match),
		.pm = &rockchip_i2s_pm_ops,
	},
};
module_platform_driver(rockchip_i2s_driver);

MODULE_DESCRIPTION("ROCKCHIP IIS ASoC Interface");
MODULE_AUTHOR("jianqun <jay.xu@rock-chips.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
MODULE_DEVICE_TABLE(of, rockchip_i2s_match);
