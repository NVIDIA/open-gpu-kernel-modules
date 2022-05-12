// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) STMicroelectronics 2018 - All Rights Reserved
 * Author: Ludovic.barre@st.com for STMicroelectronics.
 */
#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/iopoll.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/of_address.h>
#include <linux/reset.h>
#include <linux/scatterlist.h>
#include "mmci.h"

#define SDMMC_LLI_BUF_LEN	PAGE_SIZE
#define SDMMC_IDMA_BURST	BIT(MMCI_STM32_IDMABNDT_SHIFT)

#define DLYB_CR			0x0
#define DLYB_CR_DEN		BIT(0)
#define DLYB_CR_SEN		BIT(1)

#define DLYB_CFGR		0x4
#define DLYB_CFGR_SEL_MASK	GENMASK(3, 0)
#define DLYB_CFGR_UNIT_MASK	GENMASK(14, 8)
#define DLYB_CFGR_LNG_MASK	GENMASK(27, 16)
#define DLYB_CFGR_LNGF		BIT(31)

#define DLYB_NB_DELAY		11
#define DLYB_CFGR_SEL_MAX	(DLYB_NB_DELAY + 1)
#define DLYB_CFGR_UNIT_MAX	127

#define DLYB_LNG_TIMEOUT_US	1000
#define SDMMC_VSWEND_TIMEOUT_US 10000

struct sdmmc_lli_desc {
	u32 idmalar;
	u32 idmabase;
	u32 idmasize;
};

struct sdmmc_idma {
	dma_addr_t sg_dma;
	void *sg_cpu;
};

struct sdmmc_dlyb {
	void __iomem *base;
	u32 unit;
	u32 max;
};

static int sdmmc_idma_validate_data(struct mmci_host *host,
				    struct mmc_data *data)
{
	struct scatterlist *sg;
	int i;

	/*
	 * idma has constraints on idmabase & idmasize for each element
	 * excepted the last element which has no constraint on idmasize
	 */
	for_each_sg(data->sg, sg, data->sg_len - 1, i) {
		if (!IS_ALIGNED(data->sg->offset, sizeof(u32)) ||
		    !IS_ALIGNED(data->sg->length, SDMMC_IDMA_BURST)) {
			dev_err(mmc_dev(host->mmc),
				"unaligned scatterlist: ofst:%x length:%d\n",
				data->sg->offset, data->sg->length);
			return -EINVAL;
		}
	}

	if (!IS_ALIGNED(data->sg->offset, sizeof(u32))) {
		dev_err(mmc_dev(host->mmc),
			"unaligned last scatterlist: ofst:%x length:%d\n",
			data->sg->offset, data->sg->length);
		return -EINVAL;
	}

	return 0;
}

static int _sdmmc_idma_prep_data(struct mmci_host *host,
				 struct mmc_data *data)
{
	int n_elem;

	n_elem = dma_map_sg(mmc_dev(host->mmc),
			    data->sg,
			    data->sg_len,
			    mmc_get_dma_dir(data));

	if (!n_elem) {
		dev_err(mmc_dev(host->mmc), "dma_map_sg failed\n");
		return -EINVAL;
	}

	return 0;
}

static int sdmmc_idma_prep_data(struct mmci_host *host,
				struct mmc_data *data, bool next)
{
	/* Check if job is already prepared. */
	if (!next && data->host_cookie == host->next_cookie)
		return 0;

	return _sdmmc_idma_prep_data(host, data);
}

static void sdmmc_idma_unprep_data(struct mmci_host *host,
				   struct mmc_data *data, int err)
{
	dma_unmap_sg(mmc_dev(host->mmc), data->sg, data->sg_len,
		     mmc_get_dma_dir(data));
}

static int sdmmc_idma_setup(struct mmci_host *host)
{
	struct sdmmc_idma *idma;
	struct device *dev = mmc_dev(host->mmc);

	idma = devm_kzalloc(dev, sizeof(*idma), GFP_KERNEL);
	if (!idma)
		return -ENOMEM;

	host->dma_priv = idma;

	if (host->variant->dma_lli) {
		idma->sg_cpu = dmam_alloc_coherent(dev, SDMMC_LLI_BUF_LEN,
						   &idma->sg_dma, GFP_KERNEL);
		if (!idma->sg_cpu) {
			dev_err(dev, "Failed to alloc IDMA descriptor\n");
			return -ENOMEM;
		}
		host->mmc->max_segs = SDMMC_LLI_BUF_LEN /
			sizeof(struct sdmmc_lli_desc);
		host->mmc->max_seg_size = host->variant->stm32_idmabsize_mask;
	} else {
		host->mmc->max_segs = 1;
		host->mmc->max_seg_size = host->mmc->max_req_size;
	}

	return dma_set_max_seg_size(dev, host->mmc->max_seg_size);
}

static int sdmmc_idma_start(struct mmci_host *host, unsigned int *datactrl)

{
	struct sdmmc_idma *idma = host->dma_priv;
	struct sdmmc_lli_desc *desc = (struct sdmmc_lli_desc *)idma->sg_cpu;
	struct mmc_data *data = host->data;
	struct scatterlist *sg;
	int i;

	if (!host->variant->dma_lli || data->sg_len == 1) {
		writel_relaxed(sg_dma_address(data->sg),
			       host->base + MMCI_STM32_IDMABASE0R);
		writel_relaxed(MMCI_STM32_IDMAEN,
			       host->base + MMCI_STM32_IDMACTRLR);
		return 0;
	}

	for_each_sg(data->sg, sg, data->sg_len, i) {
		desc[i].idmalar = (i + 1) * sizeof(struct sdmmc_lli_desc);
		desc[i].idmalar |= MMCI_STM32_ULA | MMCI_STM32_ULS
			| MMCI_STM32_ABR;
		desc[i].idmabase = sg_dma_address(sg);
		desc[i].idmasize = sg_dma_len(sg);
	}

	/* notice the end of link list */
	desc[data->sg_len - 1].idmalar &= ~MMCI_STM32_ULA;

	dma_wmb();
	writel_relaxed(idma->sg_dma, host->base + MMCI_STM32_IDMABAR);
	writel_relaxed(desc[0].idmalar, host->base + MMCI_STM32_IDMALAR);
	writel_relaxed(desc[0].idmabase, host->base + MMCI_STM32_IDMABASE0R);
	writel_relaxed(desc[0].idmasize, host->base + MMCI_STM32_IDMABSIZER);
	writel_relaxed(MMCI_STM32_IDMAEN | MMCI_STM32_IDMALLIEN,
		       host->base + MMCI_STM32_IDMACTRLR);

	return 0;
}

static void sdmmc_idma_finalize(struct mmci_host *host, struct mmc_data *data)
{
	writel_relaxed(0, host->base + MMCI_STM32_IDMACTRLR);

	if (!data->host_cookie)
		sdmmc_idma_unprep_data(host, data, 0);
}

static void mmci_sdmmc_set_clkreg(struct mmci_host *host, unsigned int desired)
{
	unsigned int clk = 0, ddr = 0;

	if (host->mmc->ios.timing == MMC_TIMING_MMC_DDR52 ||
	    host->mmc->ios.timing == MMC_TIMING_UHS_DDR50)
		ddr = MCI_STM32_CLK_DDR;

	/*
	 * cclk = mclk / (2 * clkdiv)
	 * clkdiv 0 => bypass
	 * in ddr mode bypass is not possible
	 */
	if (desired) {
		if (desired >= host->mclk && !ddr) {
			host->cclk = host->mclk;
		} else {
			clk = DIV_ROUND_UP(host->mclk, 2 * desired);
			if (clk > MCI_STM32_CLK_CLKDIV_MSK)
				clk = MCI_STM32_CLK_CLKDIV_MSK;
			host->cclk = host->mclk / (2 * clk);
		}
	} else {
		/*
		 * while power-on phase the clock can't be define to 0,
		 * Only power-off and power-cyc deactivate the clock.
		 * if desired clock is 0, set max divider
		 */
		clk = MCI_STM32_CLK_CLKDIV_MSK;
		host->cclk = host->mclk / (2 * clk);
	}

	/* Set actual clock for debug */
	if (host->mmc->ios.power_mode == MMC_POWER_ON)
		host->mmc->actual_clock = host->cclk;
	else
		host->mmc->actual_clock = 0;

	if (host->mmc->ios.bus_width == MMC_BUS_WIDTH_4)
		clk |= MCI_STM32_CLK_WIDEBUS_4;
	if (host->mmc->ios.bus_width == MMC_BUS_WIDTH_8)
		clk |= MCI_STM32_CLK_WIDEBUS_8;

	clk |= MCI_STM32_CLK_HWFCEN;
	clk |= host->clk_reg_add;
	clk |= ddr;

	/*
	 * SDMMC_FBCK is selected when an external Delay Block is needed
	 * with SDR104.
	 */
	if (host->mmc->ios.timing >= MMC_TIMING_UHS_SDR50) {
		clk |= MCI_STM32_CLK_BUSSPEED;
		if (host->mmc->ios.timing == MMC_TIMING_UHS_SDR104) {
			clk &= ~MCI_STM32_CLK_SEL_MSK;
			clk |= MCI_STM32_CLK_SELFBCK;
		}
	}

	mmci_write_clkreg(host, clk);
}

static void sdmmc_dlyb_input_ck(struct sdmmc_dlyb *dlyb)
{
	if (!dlyb || !dlyb->base)
		return;

	/* Output clock = Input clock */
	writel_relaxed(0, dlyb->base + DLYB_CR);
}

static void mmci_sdmmc_set_pwrreg(struct mmci_host *host, unsigned int pwr)
{
	struct mmc_ios ios = host->mmc->ios;
	struct sdmmc_dlyb *dlyb = host->variant_priv;

	/* adds OF options */
	pwr = host->pwr_reg_add;

	sdmmc_dlyb_input_ck(dlyb);

	if (ios.power_mode == MMC_POWER_OFF) {
		/* Only a reset could power-off sdmmc */
		reset_control_assert(host->rst);
		udelay(2);
		reset_control_deassert(host->rst);

		/*
		 * Set the SDMMC in Power-cycle state.
		 * This will make that the SDMMC_D[7:0], SDMMC_CMD and SDMMC_CK
		 * are driven low, to prevent the Card from being supplied
		 * through the signal lines.
		 */
		mmci_write_pwrreg(host, MCI_STM32_PWR_CYC | pwr);
	} else if (ios.power_mode == MMC_POWER_ON) {
		/*
		 * After power-off (reset): the irq mask defined in probe
		 * functionis lost
		 * ault irq mask (probe) must be activated
		 */
		writel(MCI_IRQENABLE | host->variant->start_err,
		       host->base + MMCIMASK0);

		/* preserves voltage switch bits */
		pwr |= host->pwr_reg & (MCI_STM32_VSWITCHEN |
					MCI_STM32_VSWITCH);

		/*
		 * After a power-cycle state, we must set the SDMMC in
		 * Power-off. The SDMMC_D[7:0], SDMMC_CMD and SDMMC_CK are
		 * driven high. Then we can set the SDMMC to Power-on state
		 */
		mmci_write_pwrreg(host, MCI_PWR_OFF | pwr);
		mdelay(1);
		mmci_write_pwrreg(host, MCI_PWR_ON | pwr);
	}
}

static u32 sdmmc_get_dctrl_cfg(struct mmci_host *host)
{
	u32 datactrl;

	datactrl = mmci_dctrl_blksz(host);

	if (host->mmc->card && mmc_card_sdio(host->mmc->card) &&
	    host->data->blocks == 1)
		datactrl |= MCI_DPSM_STM32_MODE_SDIO;
	else if (host->data->stop && !host->mrq->sbc)
		datactrl |= MCI_DPSM_STM32_MODE_BLOCK_STOP;
	else
		datactrl |= MCI_DPSM_STM32_MODE_BLOCK;

	return datactrl;
}

static bool sdmmc_busy_complete(struct mmci_host *host, u32 status, u32 err_msk)
{
	void __iomem *base = host->base;
	u32 busy_d0, busy_d0end, mask, sdmmc_status;

	mask = readl_relaxed(base + MMCIMASK0);
	sdmmc_status = readl_relaxed(base + MMCISTATUS);
	busy_d0end = sdmmc_status & MCI_STM32_BUSYD0END;
	busy_d0 = sdmmc_status & MCI_STM32_BUSYD0;

	/* complete if there is an error or busy_d0end */
	if ((status & err_msk) || busy_d0end)
		goto complete;

	/*
	 * On response the busy signaling is reflected in the BUSYD0 flag.
	 * if busy_d0 is in-progress we must activate busyd0end interrupt
	 * to wait this completion. Else this request has no busy step.
	 */
	if (busy_d0) {
		if (!host->busy_status) {
			writel_relaxed(mask | host->variant->busy_detect_mask,
				       base + MMCIMASK0);
			host->busy_status = status &
				(MCI_CMDSENT | MCI_CMDRESPEND);
		}
		return false;
	}

complete:
	if (host->busy_status) {
		writel_relaxed(mask & ~host->variant->busy_detect_mask,
			       base + MMCIMASK0);
		host->busy_status = 0;
	}

	writel_relaxed(host->variant->busy_detect_mask, base + MMCICLEAR);

	return true;
}

static void sdmmc_dlyb_set_cfgr(struct sdmmc_dlyb *dlyb,
				int unit, int phase, bool sampler)
{
	u32 cfgr;

	writel_relaxed(DLYB_CR_SEN | DLYB_CR_DEN, dlyb->base + DLYB_CR);

	cfgr = FIELD_PREP(DLYB_CFGR_UNIT_MASK, unit) |
	       FIELD_PREP(DLYB_CFGR_SEL_MASK, phase);
	writel_relaxed(cfgr, dlyb->base + DLYB_CFGR);

	if (!sampler)
		writel_relaxed(DLYB_CR_DEN, dlyb->base + DLYB_CR);
}

static int sdmmc_dlyb_lng_tuning(struct mmci_host *host)
{
	struct sdmmc_dlyb *dlyb = host->variant_priv;
	u32 cfgr;
	int i, lng, ret;

	for (i = 0; i <= DLYB_CFGR_UNIT_MAX; i++) {
		sdmmc_dlyb_set_cfgr(dlyb, i, DLYB_CFGR_SEL_MAX, true);

		ret = readl_relaxed_poll_timeout(dlyb->base + DLYB_CFGR, cfgr,
						 (cfgr & DLYB_CFGR_LNGF),
						 1, DLYB_LNG_TIMEOUT_US);
		if (ret) {
			dev_warn(mmc_dev(host->mmc),
				 "delay line cfg timeout unit:%d cfgr:%d\n",
				 i, cfgr);
			continue;
		}

		lng = FIELD_GET(DLYB_CFGR_LNG_MASK, cfgr);
		if (lng < BIT(DLYB_NB_DELAY) && lng > 0)
			break;
	}

	if (i > DLYB_CFGR_UNIT_MAX)
		return -EINVAL;

	dlyb->unit = i;
	dlyb->max = __fls(lng);

	return 0;
}

static int sdmmc_dlyb_phase_tuning(struct mmci_host *host, u32 opcode)
{
	struct sdmmc_dlyb *dlyb = host->variant_priv;
	int cur_len = 0, max_len = 0, end_of_len = 0;
	int phase;

	for (phase = 0; phase <= dlyb->max; phase++) {
		sdmmc_dlyb_set_cfgr(dlyb, dlyb->unit, phase, false);

		if (mmc_send_tuning(host->mmc, opcode, NULL)) {
			cur_len = 0;
		} else {
			cur_len++;
			if (cur_len > max_len) {
				max_len = cur_len;
				end_of_len = phase;
			}
		}
	}

	if (!max_len) {
		dev_err(mmc_dev(host->mmc), "no tuning point found\n");
		return -EINVAL;
	}

	phase = end_of_len - max_len / 2;
	sdmmc_dlyb_set_cfgr(dlyb, dlyb->unit, phase, false);

	dev_dbg(mmc_dev(host->mmc), "unit:%d max_dly:%d phase:%d\n",
		dlyb->unit, dlyb->max, phase);

	return 0;
}

static int sdmmc_execute_tuning(struct mmc_host *mmc, u32 opcode)
{
	struct mmci_host *host = mmc_priv(mmc);
	struct sdmmc_dlyb *dlyb = host->variant_priv;

	if (!dlyb || !dlyb->base)
		return -EINVAL;

	if (sdmmc_dlyb_lng_tuning(host))
		return -EINVAL;

	return sdmmc_dlyb_phase_tuning(host, opcode);
}

static void sdmmc_pre_sig_volt_vswitch(struct mmci_host *host)
{
	/* clear the voltage switch completion flag */
	writel_relaxed(MCI_STM32_VSWENDC, host->base + MMCICLEAR);
	/* enable Voltage switch procedure */
	mmci_write_pwrreg(host, host->pwr_reg | MCI_STM32_VSWITCHEN);
}

static int sdmmc_post_sig_volt_switch(struct mmci_host *host,
				      struct mmc_ios *ios)
{
	unsigned long flags;
	u32 status;
	int ret = 0;

	if (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_180) {
		spin_lock_irqsave(&host->lock, flags);
		mmci_write_pwrreg(host, host->pwr_reg | MCI_STM32_VSWITCH);
		spin_unlock_irqrestore(&host->lock, flags);

		/* wait voltage switch completion while 10ms */
		ret = readl_relaxed_poll_timeout(host->base + MMCISTATUS,
						 status,
						 (status & MCI_STM32_VSWEND),
						 10, SDMMC_VSWEND_TIMEOUT_US);

		writel_relaxed(MCI_STM32_VSWENDC | MCI_STM32_CKSTOPC,
			       host->base + MMCICLEAR);
		mmci_write_pwrreg(host, host->pwr_reg &
				  ~(MCI_STM32_VSWITCHEN | MCI_STM32_VSWITCH));
	}

	return ret;
}

static struct mmci_host_ops sdmmc_variant_ops = {
	.validate_data = sdmmc_idma_validate_data,
	.prep_data = sdmmc_idma_prep_data,
	.unprep_data = sdmmc_idma_unprep_data,
	.get_datactrl_cfg = sdmmc_get_dctrl_cfg,
	.dma_setup = sdmmc_idma_setup,
	.dma_start = sdmmc_idma_start,
	.dma_finalize = sdmmc_idma_finalize,
	.set_clkreg = mmci_sdmmc_set_clkreg,
	.set_pwrreg = mmci_sdmmc_set_pwrreg,
	.busy_complete = sdmmc_busy_complete,
	.pre_sig_volt_switch = sdmmc_pre_sig_volt_vswitch,
	.post_sig_volt_switch = sdmmc_post_sig_volt_switch,
};

void sdmmc_variant_init(struct mmci_host *host)
{
	struct device_node *np = host->mmc->parent->of_node;
	void __iomem *base_dlyb;
	struct sdmmc_dlyb *dlyb;

	host->ops = &sdmmc_variant_ops;
	host->pwr_reg = readl_relaxed(host->base + MMCIPOWER);

	base_dlyb = devm_of_iomap(mmc_dev(host->mmc), np, 1, NULL);
	if (IS_ERR(base_dlyb))
		return;

	dlyb = devm_kzalloc(mmc_dev(host->mmc), sizeof(*dlyb), GFP_KERNEL);
	if (!dlyb)
		return;

	dlyb->base = base_dlyb;
	host->variant_priv = dlyb;
	host->mmc_ops->execute_tuning = sdmmc_execute_tuning;
}
