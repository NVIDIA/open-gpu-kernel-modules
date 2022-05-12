// SPDX-License-Identifier: (GPL-2.0 OR MIT)
//
// Copyright (c) 2018 BayLibre, SAS.
// Author: Jerome Brunet <jbrunet@baylibre.com>

#include <linux/clk.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>

#include "axg-fifo.h"

/*
 * This file implements the platform operations common to the playback and
 * capture frontend DAI. The logic behind this two types of fifo is very
 * similar but some difference exist.
 * These differences are handled in the respective DAI drivers
 */

static struct snd_pcm_hardware axg_fifo_hw = {
	.info = (SNDRV_PCM_INFO_INTERLEAVED |
		 SNDRV_PCM_INFO_MMAP |
		 SNDRV_PCM_INFO_MMAP_VALID |
		 SNDRV_PCM_INFO_BLOCK_TRANSFER |
		 SNDRV_PCM_INFO_PAUSE |
		 SNDRV_PCM_INFO_NO_PERIOD_WAKEUP),
	.formats = AXG_FIFO_FORMATS,
	.rate_min = 5512,
	.rate_max = 192000,
	.channels_min = 1,
	.channels_max = AXG_FIFO_CH_MAX,
	.period_bytes_min = AXG_FIFO_BURST,
	.period_bytes_max = UINT_MAX,
	.periods_min = 2,
	.periods_max = UINT_MAX,

	/* No real justification for this */
	.buffer_bytes_max = 1 * 1024 * 1024,
};

static struct snd_soc_dai *axg_fifo_dai(struct snd_pcm_substream *ss)
{
	struct snd_soc_pcm_runtime *rtd = ss->private_data;

	return asoc_rtd_to_cpu(rtd, 0);
}

static struct axg_fifo *axg_fifo_data(struct snd_pcm_substream *ss)
{
	struct snd_soc_dai *dai = axg_fifo_dai(ss);

	return snd_soc_dai_get_drvdata(dai);
}

static struct device *axg_fifo_dev(struct snd_pcm_substream *ss)
{
	struct snd_soc_dai *dai = axg_fifo_dai(ss);

	return dai->dev;
}

static void __dma_enable(struct axg_fifo *fifo,  bool enable)
{
	regmap_update_bits(fifo->map, FIFO_CTRL0, CTRL0_DMA_EN,
			   enable ? CTRL0_DMA_EN : 0);
}

int axg_fifo_pcm_trigger(struct snd_soc_component *component,
			 struct snd_pcm_substream *ss, int cmd)
{
	struct axg_fifo *fifo = axg_fifo_data(ss);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		__dma_enable(fifo, true);
		break;
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
	case SNDRV_PCM_TRIGGER_STOP:
		__dma_enable(fifo, false);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(axg_fifo_pcm_trigger);

snd_pcm_uframes_t axg_fifo_pcm_pointer(struct snd_soc_component *component,
				       struct snd_pcm_substream *ss)
{
	struct axg_fifo *fifo = axg_fifo_data(ss);
	struct snd_pcm_runtime *runtime = ss->runtime;
	unsigned int addr;

	regmap_read(fifo->map, FIFO_STATUS2, &addr);

	return bytes_to_frames(runtime, addr - (unsigned int)runtime->dma_addr);
}
EXPORT_SYMBOL_GPL(axg_fifo_pcm_pointer);

int axg_fifo_pcm_hw_params(struct snd_soc_component *component,
			   struct snd_pcm_substream *ss,
			   struct snd_pcm_hw_params *params)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct axg_fifo *fifo = axg_fifo_data(ss);
	unsigned int burst_num, period, threshold, irq_en;
	dma_addr_t end_ptr;

	period = params_period_bytes(params);

	/* Setup dma memory pointers */
	end_ptr = runtime->dma_addr + runtime->dma_bytes - AXG_FIFO_BURST;
	regmap_write(fifo->map, FIFO_START_ADDR, runtime->dma_addr);
	regmap_write(fifo->map, FIFO_FINISH_ADDR, end_ptr);

	/* Setup interrupt periodicity */
	burst_num = period / AXG_FIFO_BURST;
	regmap_write(fifo->map, FIFO_INT_ADDR, burst_num);

	/*
	 * Start the fifo request on the smallest of the following:
	 * - Half the fifo size
	 * - Half the period size
	 */
	threshold = min(period / 2, fifo->depth / 2);

	/*
	 * With the threshold in bytes, register value is:
	 * V = (threshold / burst) - 1
	 */
	threshold /= AXG_FIFO_BURST;
	regmap_field_write(fifo->field_threshold,
			   threshold ? threshold - 1 : 0);

	/* Enable irq if necessary  */
	irq_en = runtime->no_period_wakeup ? 0 : FIFO_INT_COUNT_REPEAT;
	regmap_update_bits(fifo->map, FIFO_CTRL0,
			   CTRL0_INT_EN(FIFO_INT_COUNT_REPEAT),
			   CTRL0_INT_EN(irq_en));

	return 0;
}
EXPORT_SYMBOL_GPL(axg_fifo_pcm_hw_params);

int g12a_fifo_pcm_hw_params(struct snd_soc_component *component,
			    struct snd_pcm_substream *ss,
			    struct snd_pcm_hw_params *params)
{
	struct axg_fifo *fifo = axg_fifo_data(ss);
	struct snd_pcm_runtime *runtime = ss->runtime;
	int ret;

	ret = axg_fifo_pcm_hw_params(component, ss, params);
	if (ret)
		return ret;

	/* Set the initial memory address of the DMA */
	regmap_write(fifo->map, FIFO_INIT_ADDR, runtime->dma_addr);

	return 0;
}
EXPORT_SYMBOL_GPL(g12a_fifo_pcm_hw_params);

int axg_fifo_pcm_hw_free(struct snd_soc_component *component,
			 struct snd_pcm_substream *ss)
{
	struct axg_fifo *fifo = axg_fifo_data(ss);

	/* Disable the block count irq */
	regmap_update_bits(fifo->map, FIFO_CTRL0,
			   CTRL0_INT_EN(FIFO_INT_COUNT_REPEAT), 0);

	return 0;
}
EXPORT_SYMBOL_GPL(axg_fifo_pcm_hw_free);

static void axg_fifo_ack_irq(struct axg_fifo *fifo, u8 mask)
{
	regmap_update_bits(fifo->map, FIFO_CTRL1,
			   CTRL1_INT_CLR(FIFO_INT_MASK),
			   CTRL1_INT_CLR(mask));

	/* Clear must also be cleared */
	regmap_update_bits(fifo->map, FIFO_CTRL1,
			   CTRL1_INT_CLR(FIFO_INT_MASK),
			   0);
}

static irqreturn_t axg_fifo_pcm_irq_block(int irq, void *dev_id)
{
	struct snd_pcm_substream *ss = dev_id;
	struct axg_fifo *fifo = axg_fifo_data(ss);
	unsigned int status;

	regmap_read(fifo->map, FIFO_STATUS1, &status);

	status = STATUS1_INT_STS(status) & FIFO_INT_MASK;
	if (status & FIFO_INT_COUNT_REPEAT)
		snd_pcm_period_elapsed(ss);
	else
		dev_dbg(axg_fifo_dev(ss), "unexpected irq - STS 0x%02x\n",
			status);

	/* Ack irqs */
	axg_fifo_ack_irq(fifo, status);

	return IRQ_RETVAL(status);
}

int axg_fifo_pcm_open(struct snd_soc_component *component,
		      struct snd_pcm_substream *ss)
{
	struct axg_fifo *fifo = axg_fifo_data(ss);
	struct device *dev = axg_fifo_dev(ss);
	int ret;

	snd_soc_set_runtime_hwparams(ss, &axg_fifo_hw);

	/*
	 * Make sure the buffer and period size are multiple of the FIFO
	 * burst
	 */
	ret = snd_pcm_hw_constraint_step(ss->runtime, 0,
					 SNDRV_PCM_HW_PARAM_BUFFER_BYTES,
					 AXG_FIFO_BURST);
	if (ret)
		return ret;

	ret = snd_pcm_hw_constraint_step(ss->runtime, 0,
					 SNDRV_PCM_HW_PARAM_PERIOD_BYTES,
					 AXG_FIFO_BURST);
	if (ret)
		return ret;

	ret = request_irq(fifo->irq, axg_fifo_pcm_irq_block, 0,
			  dev_name(dev), ss);
	if (ret)
		return ret;

	/* Enable pclk to access registers and clock the fifo ip */
	ret = clk_prepare_enable(fifo->pclk);
	if (ret)
		goto free_irq;

	/* Setup status2 so it reports the memory pointer */
	regmap_update_bits(fifo->map, FIFO_CTRL1,
			   CTRL1_STATUS2_SEL_MASK,
			   CTRL1_STATUS2_SEL(STATUS2_SEL_DDR_READ));

	/* Make sure the dma is initially disabled */
	__dma_enable(fifo, false);

	/* Disable irqs until params are ready */
	regmap_update_bits(fifo->map, FIFO_CTRL0,
			   CTRL0_INT_EN(FIFO_INT_MASK), 0);

	/* Clear any pending interrupt */
	axg_fifo_ack_irq(fifo, FIFO_INT_MASK);

	/* Take memory arbitror out of reset */
	ret = reset_control_deassert(fifo->arb);
	if (ret)
		goto free_clk;

	return 0;

free_clk:
	clk_disable_unprepare(fifo->pclk);
free_irq:
	free_irq(fifo->irq, ss);
	return ret;
}
EXPORT_SYMBOL_GPL(axg_fifo_pcm_open);

int axg_fifo_pcm_close(struct snd_soc_component *component,
		       struct snd_pcm_substream *ss)
{
	struct axg_fifo *fifo = axg_fifo_data(ss);
	int ret;

	/* Put the memory arbitror back in reset */
	ret = reset_control_assert(fifo->arb);

	/* Disable fifo ip and register access */
	clk_disable_unprepare(fifo->pclk);

	/* remove IRQ */
	free_irq(fifo->irq, ss);

	return ret;
}
EXPORT_SYMBOL_GPL(axg_fifo_pcm_close);

int axg_fifo_pcm_new(struct snd_soc_pcm_runtime *rtd, unsigned int type)
{
	struct snd_card *card = rtd->card->snd_card;
	size_t size = axg_fifo_hw.buffer_bytes_max;

	snd_pcm_set_managed_buffer(rtd->pcm->streams[type].substream,
				   SNDRV_DMA_TYPE_DEV, card->dev,
				   size, size);
	return 0;
}
EXPORT_SYMBOL_GPL(axg_fifo_pcm_new);

static const struct regmap_config axg_fifo_regmap_cfg = {
	.reg_bits	= 32,
	.val_bits	= 32,
	.reg_stride	= 4,
	.max_register	= FIFO_CTRL2,
};

int axg_fifo_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct axg_fifo_match_data *data;
	struct axg_fifo *fifo;
	void __iomem *regs;
	int ret;

	data = of_device_get_match_data(dev);
	if (!data) {
		dev_err(dev, "failed to match device\n");
		return -ENODEV;
	}

	fifo = devm_kzalloc(dev, sizeof(*fifo), GFP_KERNEL);
	if (!fifo)
		return -ENOMEM;
	platform_set_drvdata(pdev, fifo);

	regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(regs))
		return PTR_ERR(regs);

	fifo->map = devm_regmap_init_mmio(dev, regs, &axg_fifo_regmap_cfg);
	if (IS_ERR(fifo->map)) {
		dev_err(dev, "failed to init regmap: %ld\n",
			PTR_ERR(fifo->map));
		return PTR_ERR(fifo->map);
	}

	fifo->pclk = devm_clk_get(dev, NULL);
	if (IS_ERR(fifo->pclk)) {
		if (PTR_ERR(fifo->pclk) != -EPROBE_DEFER)
			dev_err(dev, "failed to get pclk: %ld\n",
				PTR_ERR(fifo->pclk));
		return PTR_ERR(fifo->pclk);
	}

	fifo->arb = devm_reset_control_get_exclusive(dev, NULL);
	if (IS_ERR(fifo->arb)) {
		if (PTR_ERR(fifo->arb) != -EPROBE_DEFER)
			dev_err(dev, "failed to get arb reset: %ld\n",
				PTR_ERR(fifo->arb));
		return PTR_ERR(fifo->arb);
	}

	fifo->irq = of_irq_get(dev->of_node, 0);
	if (fifo->irq <= 0) {
		dev_err(dev, "failed to get irq: %d\n", fifo->irq);
		return fifo->irq;
	}

	fifo->field_threshold =
		devm_regmap_field_alloc(dev, fifo->map, data->field_threshold);
	if (IS_ERR(fifo->field_threshold))
		return PTR_ERR(fifo->field_threshold);

	ret = of_property_read_u32(dev->of_node, "amlogic,fifo-depth",
				   &fifo->depth);
	if (ret) {
		/* Error out for anything but a missing property */
		if (ret != -EINVAL)
			return ret;
		/*
		 * If the property is missing, it might be because of an old
		 * DT. In such case, assume the smallest known fifo depth
		 */
		fifo->depth = 256;
		dev_warn(dev, "fifo depth not found, assume %u bytes\n",
			 fifo->depth);
	}

	return devm_snd_soc_register_component(dev, data->component_drv,
					       data->dai_drv, 1);
}
EXPORT_SYMBOL_GPL(axg_fifo_probe);

MODULE_DESCRIPTION("Amlogic AXG/G12A fifo driver");
MODULE_AUTHOR("Jerome Brunet <jbrunet@baylibre.com>");
MODULE_LICENSE("GPL v2");
