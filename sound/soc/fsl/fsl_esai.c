// SPDX-License-Identifier: GPL-2.0
//
// Freescale ESAI ALSA SoC Digital Audio Interface (DAI) driver
//
// Copyright (C) 2014 Freescale Semiconductor, Inc.

#include <linux/clk.h>
#include <linux/dmaengine.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/pm_runtime.h>
#include <sound/dmaengine_pcm.h>
#include <sound/pcm_params.h>

#include "fsl_esai.h"
#include "imx-pcm.h"

#define FSL_ESAI_FORMATS	(SNDRV_PCM_FMTBIT_S8 | \
				SNDRV_PCM_FMTBIT_S16_LE | \
				SNDRV_PCM_FMTBIT_S20_3LE | \
				SNDRV_PCM_FMTBIT_S24_LE)

/**
 * struct fsl_esai_soc_data - soc specific data
 * @reset_at_xrun: flags for enable reset operaton
 */
struct fsl_esai_soc_data {
	bool reset_at_xrun;
};

/**
 * struct fsl_esai - ESAI private data
 * @dma_params_rx: DMA parameters for receive channel
 * @dma_params_tx: DMA parameters for transmit channel
 * @pdev: platform device pointer
 * @regmap: regmap handler
 * @coreclk: clock source to access register
 * @extalclk: esai clock source to derive HCK, SCK and FS
 * @fsysclk: system clock source to derive HCK, SCK and FS
 * @spbaclk: SPBA clock (optional, depending on SoC design)
 * @work: work to handle the reset operation
 * @soc: soc specific data
 * @lock: spin lock between hw_reset() and trigger()
 * @fifo_depth: depth of tx/rx FIFO
 * @slot_width: width of each DAI slot
 * @slots: number of slots
 * @tx_mask: slot mask for TX
 * @rx_mask: slot mask for RX
 * @channels: channel num for tx or rx
 * @hck_rate: clock rate of desired HCKx clock
 * @sck_rate: clock rate of desired SCKx clock
 * @hck_dir: the direction of HCKx pads
 * @sck_div: if using PSR/PM dividers for SCKx clock
 * @slave_mode: if fully using DAI slave mode
 * @synchronous: if using tx/rx synchronous mode
 * @name: driver name
 */
struct fsl_esai {
	struct snd_dmaengine_dai_dma_data dma_params_rx;
	struct snd_dmaengine_dai_dma_data dma_params_tx;
	struct platform_device *pdev;
	struct regmap *regmap;
	struct clk *coreclk;
	struct clk *extalclk;
	struct clk *fsysclk;
	struct clk *spbaclk;
	struct work_struct work;
	const struct fsl_esai_soc_data *soc;
	spinlock_t lock; /* Protect hw_reset and trigger */
	u32 fifo_depth;
	u32 slot_width;
	u32 slots;
	u32 tx_mask;
	u32 rx_mask;
	u32 channels[2];
	u32 hck_rate[2];
	u32 sck_rate[2];
	bool hck_dir[2];
	bool sck_div[2];
	bool slave_mode;
	bool synchronous;
	char name[32];
};

static struct fsl_esai_soc_data fsl_esai_vf610 = {
	.reset_at_xrun = true,
};

static struct fsl_esai_soc_data fsl_esai_imx35 = {
	.reset_at_xrun = true,
};

static struct fsl_esai_soc_data fsl_esai_imx6ull = {
	.reset_at_xrun = false,
};

static irqreturn_t esai_isr(int irq, void *devid)
{
	struct fsl_esai *esai_priv = (struct fsl_esai *)devid;
	struct platform_device *pdev = esai_priv->pdev;
	u32 esr;
	u32 saisr;

	regmap_read(esai_priv->regmap, REG_ESAI_ESR, &esr);
	regmap_read(esai_priv->regmap, REG_ESAI_SAISR, &saisr);

	if ((saisr & (ESAI_SAISR_TUE | ESAI_SAISR_ROE)) &&
	    esai_priv->soc->reset_at_xrun) {
		dev_dbg(&pdev->dev, "reset module for xrun\n");
		regmap_update_bits(esai_priv->regmap, REG_ESAI_TCR,
				   ESAI_xCR_xEIE_MASK, 0);
		regmap_update_bits(esai_priv->regmap, REG_ESAI_RCR,
				   ESAI_xCR_xEIE_MASK, 0);
		schedule_work(&esai_priv->work);
	}

	if (esr & ESAI_ESR_TINIT_MASK)
		dev_dbg(&pdev->dev, "isr: Transmission Initialized\n");

	if (esr & ESAI_ESR_RFF_MASK)
		dev_warn(&pdev->dev, "isr: Receiving overrun\n");

	if (esr & ESAI_ESR_TFE_MASK)
		dev_warn(&pdev->dev, "isr: Transmission underrun\n");

	if (esr & ESAI_ESR_TLS_MASK)
		dev_dbg(&pdev->dev, "isr: Just transmitted the last slot\n");

	if (esr & ESAI_ESR_TDE_MASK)
		dev_dbg(&pdev->dev, "isr: Transmission data exception\n");

	if (esr & ESAI_ESR_TED_MASK)
		dev_dbg(&pdev->dev, "isr: Transmitting even slots\n");

	if (esr & ESAI_ESR_TD_MASK)
		dev_dbg(&pdev->dev, "isr: Transmitting data\n");

	if (esr & ESAI_ESR_RLS_MASK)
		dev_dbg(&pdev->dev, "isr: Just received the last slot\n");

	if (esr & ESAI_ESR_RDE_MASK)
		dev_dbg(&pdev->dev, "isr: Receiving data exception\n");

	if (esr & ESAI_ESR_RED_MASK)
		dev_dbg(&pdev->dev, "isr: Receiving even slots\n");

	if (esr & ESAI_ESR_RD_MASK)
		dev_dbg(&pdev->dev, "isr: Receiving data\n");

	return IRQ_HANDLED;
}

/**
 * fsl_esai_divisor_cal - This function is used to calculate the
 * divisors of psr, pm, fp and it is supposed to be called in
 * set_dai_sysclk() and set_bclk().
 *
 * @dai: pointer to DAI
 * @tx: current setting is for playback or capture
 * @ratio: desired overall ratio for the paticipating dividers
 * @usefp: for HCK setting, there is no need to set fp divider
 * @fp: bypass other dividers by setting fp directly if fp != 0
 */
static int fsl_esai_divisor_cal(struct snd_soc_dai *dai, bool tx, u32 ratio,
				bool usefp, u32 fp)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);
	u32 psr, pm = 999, maxfp, prod, sub, savesub, i, j;

	maxfp = usefp ? 16 : 1;

	if (usefp && fp)
		goto out_fp;

	if (ratio > 2 * 8 * 256 * maxfp || ratio < 2) {
		dev_err(dai->dev, "the ratio is out of range (2 ~ %d)\n",
				2 * 8 * 256 * maxfp);
		return -EINVAL;
	} else if (ratio % 2) {
		dev_err(dai->dev, "the raio must be even if using upper divider\n");
		return -EINVAL;
	}

	ratio /= 2;

	psr = ratio <= 256 * maxfp ? ESAI_xCCR_xPSR_BYPASS : ESAI_xCCR_xPSR_DIV8;

	/* Do not loop-search if PM (1 ~ 256) alone can serve the ratio */
	if (ratio <= 256) {
		pm = ratio;
		fp = 1;
		goto out;
	}

	/* Set the max fluctuation -- 0.1% of the max devisor */
	savesub = (psr ? 1 : 8)  * 256 * maxfp / 1000;

	/* Find the best value for PM */
	for (i = 1; i <= 256; i++) {
		for (j = 1; j <= maxfp; j++) {
			/* PSR (1 or 8) * PM (1 ~ 256) * FP (1 ~ 16) */
			prod = (psr ? 1 : 8) * i * j;

			if (prod == ratio)
				sub = 0;
			else if (prod / ratio == 1)
				sub = prod - ratio;
			else if (ratio / prod == 1)
				sub = ratio - prod;
			else
				continue;

			/* Calculate the fraction */
			sub = sub * 1000 / ratio;
			if (sub < savesub) {
				savesub = sub;
				pm = i;
				fp = j;
			}

			/* We are lucky */
			if (savesub == 0)
				goto out;
		}
	}

	if (pm == 999) {
		dev_err(dai->dev, "failed to calculate proper divisors\n");
		return -EINVAL;
	}

out:
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCCR(tx),
			   ESAI_xCCR_xPSR_MASK | ESAI_xCCR_xPM_MASK,
			   psr | ESAI_xCCR_xPM(pm));

out_fp:
	/* Bypass fp if not being required */
	if (maxfp <= 1)
		return 0;

	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCCR(tx),
			   ESAI_xCCR_xFP_MASK, ESAI_xCCR_xFP(fp));

	return 0;
}

/**
 * fsl_esai_set_dai_sysclk - configure the clock frequency of MCLK (HCKT/HCKR)
 * @dai: pointer to DAI
 * @clk_id: The clock source of HCKT/HCKR
 *	  (Input from outside; output from inside, FSYS or EXTAL)
 * @freq: The required clock rate of HCKT/HCKR
 * @dir: The clock direction of HCKT/HCKR
 *
 * Note: If the direction is input, we do not care about clk_id.
 */
static int fsl_esai_set_dai_sysclk(struct snd_soc_dai *dai, int clk_id,
				   unsigned int freq, int dir)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);
	struct clk *clksrc = esai_priv->extalclk;
	bool tx = (clk_id <= ESAI_HCKT_EXTAL || esai_priv->synchronous);
	bool in = dir == SND_SOC_CLOCK_IN;
	u32 ratio, ecr = 0;
	unsigned long clk_rate;
	int ret;

	if (freq == 0) {
		dev_err(dai->dev, "%sput freq of HCK%c should not be 0Hz\n",
			in ? "in" : "out", tx ? 'T' : 'R');
		return -EINVAL;
	}

	/* Bypass divider settings if the requirement doesn't change */
	if (freq == esai_priv->hck_rate[tx] && dir == esai_priv->hck_dir[tx])
		return 0;

	/* sck_div can be only bypassed if ETO/ERO=0 and SNC_SOC_CLOCK_OUT */
	esai_priv->sck_div[tx] = true;

	/* Set the direction of HCKT/HCKR pins */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCCR(tx),
			   ESAI_xCCR_xHCKD, in ? 0 : ESAI_xCCR_xHCKD);

	if (in)
		goto out;

	switch (clk_id) {
	case ESAI_HCKT_FSYS:
	case ESAI_HCKR_FSYS:
		clksrc = esai_priv->fsysclk;
		break;
	case ESAI_HCKT_EXTAL:
		ecr |= ESAI_ECR_ETI;
		break;
	case ESAI_HCKR_EXTAL:
		ecr |= esai_priv->synchronous ? ESAI_ECR_ETI : ESAI_ECR_ERI;
		break;
	default:
		return -EINVAL;
	}

	if (IS_ERR(clksrc)) {
		dev_err(dai->dev, "no assigned %s clock\n",
			(clk_id % 2) ? "extal" : "fsys");
		return PTR_ERR(clksrc);
	}
	clk_rate = clk_get_rate(clksrc);

	ratio = clk_rate / freq;
	if (ratio * freq > clk_rate)
		ret = ratio * freq - clk_rate;
	else if (ratio * freq < clk_rate)
		ret = clk_rate - ratio * freq;
	else
		ret = 0;

	/* Block if clock source can not be divided into the required rate */
	if (ret != 0 && clk_rate / ret < 1000) {
		dev_err(dai->dev, "failed to derive required HCK%c rate\n",
				tx ? 'T' : 'R');
		return -EINVAL;
	}

	/* Only EXTAL source can be output directly without using PSR and PM */
	if (ratio == 1 && clksrc == esai_priv->extalclk) {
		/* Bypass all the dividers if not being needed */
		ecr |= tx ? ESAI_ECR_ETO : ESAI_ECR_ERO;
		goto out;
	} else if (ratio < 2) {
		/* The ratio should be no less than 2 if using other sources */
		dev_err(dai->dev, "failed to derive required HCK%c rate\n",
				tx ? 'T' : 'R');
		return -EINVAL;
	}

	ret = fsl_esai_divisor_cal(dai, tx, ratio, false, 0);
	if (ret)
		return ret;

	esai_priv->sck_div[tx] = false;

out:
	esai_priv->hck_dir[tx] = dir;
	esai_priv->hck_rate[tx] = freq;

	regmap_update_bits(esai_priv->regmap, REG_ESAI_ECR,
			   tx ? ESAI_ECR_ETI | ESAI_ECR_ETO :
			   ESAI_ECR_ERI | ESAI_ECR_ERO, ecr);

	return 0;
}

/**
 * fsl_esai_set_bclk - configure the related dividers according to the bclk rate
 * @dai: pointer to DAI
 * @tx: direction boolean
 * @freq: bclk freq
 */
static int fsl_esai_set_bclk(struct snd_soc_dai *dai, bool tx, u32 freq)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);
	u32 hck_rate = esai_priv->hck_rate[tx];
	u32 sub, ratio = hck_rate / freq;
	int ret;

	/* Don't apply for fully slave mode or unchanged bclk */
	if (esai_priv->slave_mode || esai_priv->sck_rate[tx] == freq)
		return 0;

	if (ratio * freq > hck_rate)
		sub = ratio * freq - hck_rate;
	else if (ratio * freq < hck_rate)
		sub = hck_rate - ratio * freq;
	else
		sub = 0;

	/* Block if clock source can not be divided into the required rate */
	if (sub != 0 && hck_rate / sub < 1000) {
		dev_err(dai->dev, "failed to derive required SCK%c rate\n",
				tx ? 'T' : 'R');
		return -EINVAL;
	}

	/* The ratio should be contented by FP alone if bypassing PM and PSR */
	if (!esai_priv->sck_div[tx] && (ratio > 16 || ratio == 0)) {
		dev_err(dai->dev, "the ratio is out of range (1 ~ 16)\n");
		return -EINVAL;
	}

	ret = fsl_esai_divisor_cal(dai, tx, ratio, true,
			esai_priv->sck_div[tx] ? 0 : ratio);
	if (ret)
		return ret;

	/* Save current bclk rate */
	esai_priv->sck_rate[tx] = freq;

	return 0;
}

static int fsl_esai_set_dai_tdm_slot(struct snd_soc_dai *dai, u32 tx_mask,
				     u32 rx_mask, int slots, int slot_width)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);

	regmap_update_bits(esai_priv->regmap, REG_ESAI_TCCR,
			   ESAI_xCCR_xDC_MASK, ESAI_xCCR_xDC(slots));

	regmap_update_bits(esai_priv->regmap, REG_ESAI_RCCR,
			   ESAI_xCCR_xDC_MASK, ESAI_xCCR_xDC(slots));

	esai_priv->slot_width = slot_width;
	esai_priv->slots = slots;
	esai_priv->tx_mask = tx_mask;
	esai_priv->rx_mask = rx_mask;

	return 0;
}

static int fsl_esai_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);
	u32 xcr = 0, xccr = 0, mask;

	/* DAI mode */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		/* Data on rising edge of bclk, frame low, 1clk before data */
		xcr |= ESAI_xCR_xFSR;
		xccr |= ESAI_xCCR_xFSP | ESAI_xCCR_xCKP | ESAI_xCCR_xHCKP;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		/* Data on rising edge of bclk, frame high */
		xccr |= ESAI_xCCR_xCKP | ESAI_xCCR_xHCKP;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		/* Data on rising edge of bclk, frame high, right aligned */
		xccr |= ESAI_xCCR_xCKP | ESAI_xCCR_xHCKP;
		xcr  |= ESAI_xCR_xWA;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		/* Data on rising edge of bclk, frame high, 1clk before data */
		xcr |= ESAI_xCR_xFSL | ESAI_xCR_xFSR;
		xccr |= ESAI_xCCR_xCKP | ESAI_xCCR_xHCKP;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		/* Data on rising edge of bclk, frame high */
		xcr |= ESAI_xCR_xFSL;
		xccr |= ESAI_xCCR_xCKP | ESAI_xCCR_xHCKP;
		break;
	default:
		return -EINVAL;
	}

	/* DAI clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		/* Nothing to do for both normal cases */
		break;
	case SND_SOC_DAIFMT_IB_NF:
		/* Invert bit clock */
		xccr ^= ESAI_xCCR_xCKP | ESAI_xCCR_xHCKP;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		/* Invert frame clock */
		xccr ^= ESAI_xCCR_xFSP;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		/* Invert both clocks */
		xccr ^= ESAI_xCCR_xCKP | ESAI_xCCR_xHCKP | ESAI_xCCR_xFSP;
		break;
	default:
		return -EINVAL;
	}

	esai_priv->slave_mode = false;

	/* DAI clock master masks */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		esai_priv->slave_mode = true;
		break;
	case SND_SOC_DAIFMT_CBS_CFM:
		xccr |= ESAI_xCCR_xCKD;
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		xccr |= ESAI_xCCR_xFSD;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		xccr |= ESAI_xCCR_xFSD | ESAI_xCCR_xCKD;
		break;
	default:
		return -EINVAL;
	}

	mask = ESAI_xCR_xFSL | ESAI_xCR_xFSR | ESAI_xCR_xWA;
	regmap_update_bits(esai_priv->regmap, REG_ESAI_TCR, mask, xcr);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_RCR, mask, xcr);

	mask = ESAI_xCCR_xCKP | ESAI_xCCR_xHCKP | ESAI_xCCR_xFSP |
		ESAI_xCCR_xFSD | ESAI_xCCR_xCKD;
	regmap_update_bits(esai_priv->regmap, REG_ESAI_TCCR, mask, xccr);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_RCCR, mask, xccr);

	return 0;
}

static int fsl_esai_startup(struct snd_pcm_substream *substream,
			    struct snd_soc_dai *dai)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);

	if (!snd_soc_dai_active(dai)) {
		/* Set synchronous mode */
		regmap_update_bits(esai_priv->regmap, REG_ESAI_SAICR,
				   ESAI_SAICR_SYNC, esai_priv->synchronous ?
				   ESAI_SAICR_SYNC : 0);

		/* Set slots count */
		regmap_update_bits(esai_priv->regmap, REG_ESAI_TCCR,
				   ESAI_xCCR_xDC_MASK,
				   ESAI_xCCR_xDC(esai_priv->slots));
		regmap_update_bits(esai_priv->regmap, REG_ESAI_RCCR,
				   ESAI_xCCR_xDC_MASK,
				   ESAI_xCCR_xDC(esai_priv->slots));
	}

	return 0;

}

static int fsl_esai_hw_params(struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *params,
			      struct snd_soc_dai *dai)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);
	bool tx = substream->stream == SNDRV_PCM_STREAM_PLAYBACK;
	u32 width = params_width(params);
	u32 channels = params_channels(params);
	u32 pins = DIV_ROUND_UP(channels, esai_priv->slots);
	u32 slot_width = width;
	u32 bclk, mask, val;
	int ret;

	/* Override slot_width if being specifically set */
	if (esai_priv->slot_width)
		slot_width = esai_priv->slot_width;

	bclk = params_rate(params) * slot_width * esai_priv->slots;

	ret = fsl_esai_set_bclk(dai, esai_priv->synchronous || tx, bclk);
	if (ret)
		return ret;

	mask = ESAI_xCR_xSWS_MASK;
	val = ESAI_xCR_xSWS(slot_width, width);

	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCR(tx), mask, val);
	/* Recording in synchronous mode needs to set TCR also */
	if (!tx && esai_priv->synchronous)
		regmap_update_bits(esai_priv->regmap, REG_ESAI_TCR, mask, val);

	/* Use Normal mode to support monaural audio */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCR(tx),
			   ESAI_xCR_xMOD_MASK, params_channels(params) > 1 ?
			   ESAI_xCR_xMOD_NETWORK : 0);

	regmap_update_bits(esai_priv->regmap, REG_ESAI_xFCR(tx),
			   ESAI_xFCR_xFR_MASK, ESAI_xFCR_xFR);

	mask = ESAI_xFCR_xFR_MASK | ESAI_xFCR_xWA_MASK | ESAI_xFCR_xFWM_MASK |
	      (tx ? ESAI_xFCR_TE_MASK | ESAI_xFCR_TIEN : ESAI_xFCR_RE_MASK);
	val = ESAI_xFCR_xWA(width) | ESAI_xFCR_xFWM(esai_priv->fifo_depth) |
	     (tx ? ESAI_xFCR_TE(pins) | ESAI_xFCR_TIEN : ESAI_xFCR_RE(pins));

	regmap_update_bits(esai_priv->regmap, REG_ESAI_xFCR(tx), mask, val);

	if (tx)
		regmap_update_bits(esai_priv->regmap, REG_ESAI_TCR,
				ESAI_xCR_PADC, ESAI_xCR_PADC);

	/* Remove ESAI personal reset by configuring ESAI_PCRC and ESAI_PRRC */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_PRRC,
			   ESAI_PRRC_PDC_MASK, ESAI_PRRC_PDC(ESAI_GPIO));
	regmap_update_bits(esai_priv->regmap, REG_ESAI_PCRC,
			   ESAI_PCRC_PC_MASK, ESAI_PCRC_PC(ESAI_GPIO));
	return 0;
}

static int fsl_esai_hw_init(struct fsl_esai *esai_priv)
{
	struct platform_device *pdev = esai_priv->pdev;
	int ret;

	/* Reset ESAI unit */
	ret = regmap_update_bits(esai_priv->regmap, REG_ESAI_ECR,
				 ESAI_ECR_ESAIEN_MASK | ESAI_ECR_ERST_MASK,
				 ESAI_ECR_ESAIEN | ESAI_ECR_ERST);
	if (ret) {
		dev_err(&pdev->dev, "failed to reset ESAI: %d\n", ret);
		return ret;
	}

	/*
	 * We need to enable ESAI so as to access some of its registers.
	 * Otherwise, we would fail to dump regmap from user space.
	 */
	ret = regmap_update_bits(esai_priv->regmap, REG_ESAI_ECR,
				 ESAI_ECR_ESAIEN_MASK | ESAI_ECR_ERST_MASK,
				 ESAI_ECR_ESAIEN);
	if (ret) {
		dev_err(&pdev->dev, "failed to enable ESAI: %d\n", ret);
		return ret;
	}

	regmap_update_bits(esai_priv->regmap, REG_ESAI_PRRC,
			   ESAI_PRRC_PDC_MASK, 0);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_PCRC,
			   ESAI_PCRC_PC_MASK, 0);

	return 0;
}

static int fsl_esai_register_restore(struct fsl_esai *esai_priv)
{
	int ret;

	/* FIFO reset for safety */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_TFCR,
			   ESAI_xFCR_xFR, ESAI_xFCR_xFR);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_RFCR,
			   ESAI_xFCR_xFR, ESAI_xFCR_xFR);

	regcache_mark_dirty(esai_priv->regmap);
	ret = regcache_sync(esai_priv->regmap);
	if (ret)
		return ret;

	/* FIFO reset done */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_TFCR, ESAI_xFCR_xFR, 0);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_RFCR, ESAI_xFCR_xFR, 0);

	return 0;
}

static void fsl_esai_trigger_start(struct fsl_esai *esai_priv, bool tx)
{
	u8 i, channels = esai_priv->channels[tx];
	u32 pins = DIV_ROUND_UP(channels, esai_priv->slots);
	u32 mask;

	regmap_update_bits(esai_priv->regmap, REG_ESAI_xFCR(tx),
			   ESAI_xFCR_xFEN_MASK, ESAI_xFCR_xFEN);

	/* Write initial words reqiured by ESAI as normal procedure */
	for (i = 0; tx && i < channels; i++)
		regmap_write(esai_priv->regmap, REG_ESAI_ETDR, 0x0);

	/*
	 * When set the TE/RE in the end of enablement flow, there
	 * will be channel swap issue for multi data line case.
	 * In order to workaround this issue, we switch the bit
	 * enablement sequence to below sequence
	 * 1) clear the xSMB & xSMA: which is done in probe and
	 *                           stop state.
	 * 2) set TE/RE
	 * 3) set xSMB
	 * 4) set xSMA:  xSMA is the last one in this flow, which
	 *               will trigger esai to start.
	 */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCR(tx),
			   tx ? ESAI_xCR_TE_MASK : ESAI_xCR_RE_MASK,
			   tx ? ESAI_xCR_TE(pins) : ESAI_xCR_RE(pins));
	mask = tx ? esai_priv->tx_mask : esai_priv->rx_mask;

	regmap_update_bits(esai_priv->regmap, REG_ESAI_xSMB(tx),
			   ESAI_xSMB_xS_MASK, ESAI_xSMB_xS(mask));
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xSMA(tx),
			   ESAI_xSMA_xS_MASK, ESAI_xSMA_xS(mask));

	/* Enable Exception interrupt */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCR(tx),
			   ESAI_xCR_xEIE_MASK, ESAI_xCR_xEIE);
}

static void fsl_esai_trigger_stop(struct fsl_esai *esai_priv, bool tx)
{
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCR(tx),
			   ESAI_xCR_xEIE_MASK, 0);

	regmap_update_bits(esai_priv->regmap, REG_ESAI_xCR(tx),
			   tx ? ESAI_xCR_TE_MASK : ESAI_xCR_RE_MASK, 0);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xSMA(tx),
			   ESAI_xSMA_xS_MASK, 0);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xSMB(tx),
			   ESAI_xSMB_xS_MASK, 0);

	/* Disable and reset FIFO */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xFCR(tx),
			   ESAI_xFCR_xFR | ESAI_xFCR_xFEN, ESAI_xFCR_xFR);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_xFCR(tx),
			   ESAI_xFCR_xFR, 0);
}

static void fsl_esai_hw_reset(struct work_struct *work)
{
	struct fsl_esai *esai_priv = container_of(work, struct fsl_esai, work);
	bool tx = true, rx = false, enabled[2];
	unsigned long lock_flags;
	u32 tfcr, rfcr;

	spin_lock_irqsave(&esai_priv->lock, lock_flags);
	/* Save the registers */
	regmap_read(esai_priv->regmap, REG_ESAI_TFCR, &tfcr);
	regmap_read(esai_priv->regmap, REG_ESAI_RFCR, &rfcr);
	enabled[tx] = tfcr & ESAI_xFCR_xFEN;
	enabled[rx] = rfcr & ESAI_xFCR_xFEN;

	/* Stop the tx & rx */
	fsl_esai_trigger_stop(esai_priv, tx);
	fsl_esai_trigger_stop(esai_priv, rx);

	/* Reset the esai, and ignore return value */
	fsl_esai_hw_init(esai_priv);

	/* Enforce ESAI personal resets for both TX and RX */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_TCR,
			   ESAI_xCR_xPR_MASK, ESAI_xCR_xPR);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_RCR,
			   ESAI_xCR_xPR_MASK, ESAI_xCR_xPR);

	/* Restore registers by regcache_sync, and ignore return value */
	fsl_esai_register_restore(esai_priv);

	/* Remove ESAI personal resets by configuring PCRC and PRRC also */
	regmap_update_bits(esai_priv->regmap, REG_ESAI_TCR,
			   ESAI_xCR_xPR_MASK, 0);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_RCR,
			   ESAI_xCR_xPR_MASK, 0);
	regmap_update_bits(esai_priv->regmap, REG_ESAI_PRRC,
			   ESAI_PRRC_PDC_MASK, ESAI_PRRC_PDC(ESAI_GPIO));
	regmap_update_bits(esai_priv->regmap, REG_ESAI_PCRC,
			   ESAI_PCRC_PC_MASK, ESAI_PCRC_PC(ESAI_GPIO));

	/* Restart tx / rx, if they already enabled */
	if (enabled[tx])
		fsl_esai_trigger_start(esai_priv, tx);
	if (enabled[rx])
		fsl_esai_trigger_start(esai_priv, rx);

	spin_unlock_irqrestore(&esai_priv->lock, lock_flags);
}

static int fsl_esai_trigger(struct snd_pcm_substream *substream, int cmd,
			    struct snd_soc_dai *dai)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);
	bool tx = substream->stream == SNDRV_PCM_STREAM_PLAYBACK;
	unsigned long lock_flags;

	esai_priv->channels[tx] = substream->runtime->channels;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		spin_lock_irqsave(&esai_priv->lock, lock_flags);
		fsl_esai_trigger_start(esai_priv, tx);
		spin_unlock_irqrestore(&esai_priv->lock, lock_flags);
		break;
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		spin_lock_irqsave(&esai_priv->lock, lock_flags);
		fsl_esai_trigger_stop(esai_priv, tx);
		spin_unlock_irqrestore(&esai_priv->lock, lock_flags);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const struct snd_soc_dai_ops fsl_esai_dai_ops = {
	.startup = fsl_esai_startup,
	.trigger = fsl_esai_trigger,
	.hw_params = fsl_esai_hw_params,
	.set_sysclk = fsl_esai_set_dai_sysclk,
	.set_fmt = fsl_esai_set_dai_fmt,
	.set_tdm_slot = fsl_esai_set_dai_tdm_slot,
};

static int fsl_esai_dai_probe(struct snd_soc_dai *dai)
{
	struct fsl_esai *esai_priv = snd_soc_dai_get_drvdata(dai);

	snd_soc_dai_init_dma_data(dai, &esai_priv->dma_params_tx,
				  &esai_priv->dma_params_rx);

	return 0;
}

static struct snd_soc_dai_driver fsl_esai_dai = {
	.probe = fsl_esai_dai_probe,
	.playback = {
		.stream_name = "CPU-Playback",
		.channels_min = 1,
		.channels_max = 12,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = FSL_ESAI_FORMATS,
	},
	.capture = {
		.stream_name = "CPU-Capture",
		.channels_min = 1,
		.channels_max = 8,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = FSL_ESAI_FORMATS,
	},
	.ops = &fsl_esai_dai_ops,
};

static const struct snd_soc_component_driver fsl_esai_component = {
	.name		= "fsl-esai",
};

static const struct reg_default fsl_esai_reg_defaults[] = {
	{REG_ESAI_ETDR,	 0x00000000},
	{REG_ESAI_ECR,	 0x00000000},
	{REG_ESAI_TFCR,	 0x00000000},
	{REG_ESAI_RFCR,	 0x00000000},
	{REG_ESAI_TX0,	 0x00000000},
	{REG_ESAI_TX1,	 0x00000000},
	{REG_ESAI_TX2,	 0x00000000},
	{REG_ESAI_TX3,	 0x00000000},
	{REG_ESAI_TX4,	 0x00000000},
	{REG_ESAI_TX5,	 0x00000000},
	{REG_ESAI_TSR,	 0x00000000},
	{REG_ESAI_SAICR, 0x00000000},
	{REG_ESAI_TCR,	 0x00000000},
	{REG_ESAI_TCCR,	 0x00000000},
	{REG_ESAI_RCR,	 0x00000000},
	{REG_ESAI_RCCR,	 0x00000000},
	{REG_ESAI_TSMA,  0x0000ffff},
	{REG_ESAI_TSMB,  0x0000ffff},
	{REG_ESAI_RSMA,  0x0000ffff},
	{REG_ESAI_RSMB,  0x0000ffff},
	{REG_ESAI_PRRC,  0x00000000},
	{REG_ESAI_PCRC,  0x00000000},
};

static bool fsl_esai_readable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case REG_ESAI_ERDR:
	case REG_ESAI_ECR:
	case REG_ESAI_ESR:
	case REG_ESAI_TFCR:
	case REG_ESAI_TFSR:
	case REG_ESAI_RFCR:
	case REG_ESAI_RFSR:
	case REG_ESAI_RX0:
	case REG_ESAI_RX1:
	case REG_ESAI_RX2:
	case REG_ESAI_RX3:
	case REG_ESAI_SAISR:
	case REG_ESAI_SAICR:
	case REG_ESAI_TCR:
	case REG_ESAI_TCCR:
	case REG_ESAI_RCR:
	case REG_ESAI_RCCR:
	case REG_ESAI_TSMA:
	case REG_ESAI_TSMB:
	case REG_ESAI_RSMA:
	case REG_ESAI_RSMB:
	case REG_ESAI_PRRC:
	case REG_ESAI_PCRC:
		return true;
	default:
		return false;
	}
}

static bool fsl_esai_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case REG_ESAI_ERDR:
	case REG_ESAI_ESR:
	case REG_ESAI_TFSR:
	case REG_ESAI_RFSR:
	case REG_ESAI_RX0:
	case REG_ESAI_RX1:
	case REG_ESAI_RX2:
	case REG_ESAI_RX3:
	case REG_ESAI_SAISR:
		return true;
	default:
		return false;
	}
}

static bool fsl_esai_writeable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case REG_ESAI_ETDR:
	case REG_ESAI_ECR:
	case REG_ESAI_TFCR:
	case REG_ESAI_RFCR:
	case REG_ESAI_TX0:
	case REG_ESAI_TX1:
	case REG_ESAI_TX2:
	case REG_ESAI_TX3:
	case REG_ESAI_TX4:
	case REG_ESAI_TX5:
	case REG_ESAI_TSR:
	case REG_ESAI_SAICR:
	case REG_ESAI_TCR:
	case REG_ESAI_TCCR:
	case REG_ESAI_RCR:
	case REG_ESAI_RCCR:
	case REG_ESAI_TSMA:
	case REG_ESAI_TSMB:
	case REG_ESAI_RSMA:
	case REG_ESAI_RSMB:
	case REG_ESAI_PRRC:
	case REG_ESAI_PCRC:
		return true;
	default:
		return false;
	}
}

static const struct regmap_config fsl_esai_regmap_config = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,

	.max_register = REG_ESAI_PCRC,
	.reg_defaults = fsl_esai_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(fsl_esai_reg_defaults),
	.readable_reg = fsl_esai_readable_reg,
	.volatile_reg = fsl_esai_volatile_reg,
	.writeable_reg = fsl_esai_writeable_reg,
	.cache_type = REGCACHE_FLAT,
};

static int fsl_esai_runtime_resume(struct device *dev);
static int fsl_esai_runtime_suspend(struct device *dev);

static int fsl_esai_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct fsl_esai *esai_priv;
	struct resource *res;
	const __be32 *iprop;
	void __iomem *regs;
	int irq, ret;

	esai_priv = devm_kzalloc(&pdev->dev, sizeof(*esai_priv), GFP_KERNEL);
	if (!esai_priv)
		return -ENOMEM;

	esai_priv->pdev = pdev;
	snprintf(esai_priv->name, sizeof(esai_priv->name), "%pOFn", np);

	esai_priv->soc = of_device_get_match_data(&pdev->dev);

	/* Get the addresses and IRQ */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(regs))
		return PTR_ERR(regs);

	esai_priv->regmap = devm_regmap_init_mmio(&pdev->dev, regs, &fsl_esai_regmap_config);
	if (IS_ERR(esai_priv->regmap)) {
		dev_err(&pdev->dev, "failed to init regmap: %ld\n",
				PTR_ERR(esai_priv->regmap));
		return PTR_ERR(esai_priv->regmap);
	}

	esai_priv->coreclk = devm_clk_get(&pdev->dev, "core");
	if (IS_ERR(esai_priv->coreclk)) {
		dev_err(&pdev->dev, "failed to get core clock: %ld\n",
				PTR_ERR(esai_priv->coreclk));
		return PTR_ERR(esai_priv->coreclk);
	}

	esai_priv->extalclk = devm_clk_get(&pdev->dev, "extal");
	if (IS_ERR(esai_priv->extalclk))
		dev_warn(&pdev->dev, "failed to get extal clock: %ld\n",
				PTR_ERR(esai_priv->extalclk));

	esai_priv->fsysclk = devm_clk_get(&pdev->dev, "fsys");
	if (IS_ERR(esai_priv->fsysclk))
		dev_warn(&pdev->dev, "failed to get fsys clock: %ld\n",
				PTR_ERR(esai_priv->fsysclk));

	esai_priv->spbaclk = devm_clk_get(&pdev->dev, "spba");
	if (IS_ERR(esai_priv->spbaclk))
		dev_warn(&pdev->dev, "failed to get spba clock: %ld\n",
				PTR_ERR(esai_priv->spbaclk));

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	ret = devm_request_irq(&pdev->dev, irq, esai_isr, IRQF_SHARED,
			       esai_priv->name, esai_priv);
	if (ret) {
		dev_err(&pdev->dev, "failed to claim irq %u\n", irq);
		return ret;
	}

	/* Set a default slot number */
	esai_priv->slots = 2;

	/* Set a default master/slave state */
	esai_priv->slave_mode = true;

	/* Determine the FIFO depth */
	iprop = of_get_property(np, "fsl,fifo-depth", NULL);
	if (iprop)
		esai_priv->fifo_depth = be32_to_cpup(iprop);
	else
		esai_priv->fifo_depth = 64;

	esai_priv->dma_params_tx.maxburst = 16;
	esai_priv->dma_params_rx.maxburst = 16;
	esai_priv->dma_params_tx.addr = res->start + REG_ESAI_ETDR;
	esai_priv->dma_params_rx.addr = res->start + REG_ESAI_ERDR;

	esai_priv->synchronous =
		of_property_read_bool(np, "fsl,esai-synchronous");

	/* Implement full symmetry for synchronous mode */
	if (esai_priv->synchronous) {
		fsl_esai_dai.symmetric_rate = 1;
		fsl_esai_dai.symmetric_channels = 1;
		fsl_esai_dai.symmetric_sample_bits = 1;
	}

	dev_set_drvdata(&pdev->dev, esai_priv);
	spin_lock_init(&esai_priv->lock);
	pm_runtime_enable(&pdev->dev);
	if (!pm_runtime_enabled(&pdev->dev)) {
		ret = fsl_esai_runtime_resume(&pdev->dev);
		if (ret)
			goto err_pm_disable;
	}

	ret = pm_runtime_get_sync(&pdev->dev);
	if (ret < 0) {
		pm_runtime_put_noidle(&pdev->dev);
		goto err_pm_get_sync;
	}

	ret = fsl_esai_hw_init(esai_priv);
	if (ret)
		goto err_pm_get_sync;

	esai_priv->tx_mask = 0xFFFFFFFF;
	esai_priv->rx_mask = 0xFFFFFFFF;

	/* Clear the TSMA, TSMB, RSMA, RSMB */
	regmap_write(esai_priv->regmap, REG_ESAI_TSMA, 0);
	regmap_write(esai_priv->regmap, REG_ESAI_TSMB, 0);
	regmap_write(esai_priv->regmap, REG_ESAI_RSMA, 0);
	regmap_write(esai_priv->regmap, REG_ESAI_RSMB, 0);

	ret = pm_runtime_put_sync(&pdev->dev);
	if (ret < 0)
		goto err_pm_get_sync;

	ret = devm_snd_soc_register_component(&pdev->dev, &fsl_esai_component,
					      &fsl_esai_dai, 1);
	if (ret) {
		dev_err(&pdev->dev, "failed to register DAI: %d\n", ret);
		goto err_pm_get_sync;
	}

	INIT_WORK(&esai_priv->work, fsl_esai_hw_reset);

	ret = imx_pcm_dma_init(pdev, IMX_ESAI_DMABUF_SIZE);
	if (ret) {
		dev_err(&pdev->dev, "failed to init imx pcm dma: %d\n", ret);
		goto err_pm_get_sync;
	}

	return ret;

err_pm_get_sync:
	if (!pm_runtime_status_suspended(&pdev->dev))
		fsl_esai_runtime_suspend(&pdev->dev);
err_pm_disable:
	pm_runtime_disable(&pdev->dev);
	return ret;
}

static int fsl_esai_remove(struct platform_device *pdev)
{
	struct fsl_esai *esai_priv = platform_get_drvdata(pdev);

	pm_runtime_disable(&pdev->dev);
	if (!pm_runtime_status_suspended(&pdev->dev))
		fsl_esai_runtime_suspend(&pdev->dev);

	cancel_work_sync(&esai_priv->work);

	return 0;
}

static const struct of_device_id fsl_esai_dt_ids[] = {
	{ .compatible = "fsl,imx35-esai", .data = &fsl_esai_imx35 },
	{ .compatible = "fsl,vf610-esai", .data = &fsl_esai_vf610 },
	{ .compatible = "fsl,imx6ull-esai", .data = &fsl_esai_imx6ull },
	{}
};
MODULE_DEVICE_TABLE(of, fsl_esai_dt_ids);

static int fsl_esai_runtime_resume(struct device *dev)
{
	struct fsl_esai *esai = dev_get_drvdata(dev);
	int ret;

	/*
	 * Some platforms might use the same bit to gate all three or two of
	 * clocks, so keep all clocks open/close at the same time for safety
	 */
	ret = clk_prepare_enable(esai->coreclk);
	if (ret)
		return ret;
	if (!IS_ERR(esai->spbaclk)) {
		ret = clk_prepare_enable(esai->spbaclk);
		if (ret)
			goto err_spbaclk;
	}
	if (!IS_ERR(esai->extalclk)) {
		ret = clk_prepare_enable(esai->extalclk);
		if (ret)
			goto err_extalclk;
	}
	if (!IS_ERR(esai->fsysclk)) {
		ret = clk_prepare_enable(esai->fsysclk);
		if (ret)
			goto err_fsysclk;
	}

	regcache_cache_only(esai->regmap, false);

	ret = fsl_esai_register_restore(esai);
	if (ret)
		goto err_regcache_sync;

	return 0;

err_regcache_sync:
	if (!IS_ERR(esai->fsysclk))
		clk_disable_unprepare(esai->fsysclk);
err_fsysclk:
	if (!IS_ERR(esai->extalclk))
		clk_disable_unprepare(esai->extalclk);
err_extalclk:
	if (!IS_ERR(esai->spbaclk))
		clk_disable_unprepare(esai->spbaclk);
err_spbaclk:
	clk_disable_unprepare(esai->coreclk);

	return ret;
}

static int fsl_esai_runtime_suspend(struct device *dev)
{
	struct fsl_esai *esai = dev_get_drvdata(dev);

	regcache_cache_only(esai->regmap, true);

	if (!IS_ERR(esai->fsysclk))
		clk_disable_unprepare(esai->fsysclk);
	if (!IS_ERR(esai->extalclk))
		clk_disable_unprepare(esai->extalclk);
	if (!IS_ERR(esai->spbaclk))
		clk_disable_unprepare(esai->spbaclk);
	clk_disable_unprepare(esai->coreclk);

	return 0;
}

static const struct dev_pm_ops fsl_esai_pm_ops = {
	SET_RUNTIME_PM_OPS(fsl_esai_runtime_suspend,
			   fsl_esai_runtime_resume,
			   NULL)
	SET_SYSTEM_SLEEP_PM_OPS(pm_runtime_force_suspend,
				pm_runtime_force_resume)
};

static struct platform_driver fsl_esai_driver = {
	.probe = fsl_esai_probe,
	.remove = fsl_esai_remove,
	.driver = {
		.name = "fsl-esai-dai",
		.pm = &fsl_esai_pm_ops,
		.of_match_table = fsl_esai_dt_ids,
	},
};

module_platform_driver(fsl_esai_driver);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("Freescale ESAI CPU DAI driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:fsl-esai-dai");
