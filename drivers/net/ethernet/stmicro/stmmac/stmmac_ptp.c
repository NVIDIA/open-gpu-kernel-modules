// SPDX-License-Identifier: GPL-2.0-only
/*******************************************************************************
  PTP 1588 clock using the STMMAC.

  Copyright (C) 2013  Vayavya Labs Pvt Ltd


  Author: Rayagond Kokatanur <rayagond@vayavyalabs.com>
*******************************************************************************/
#include "stmmac.h"
#include "stmmac_ptp.h"
#include "dwmac4.h"

/**
 * stmmac_adjust_freq
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ppb: desired period change in parts ber billion
 *
 * Description: this function will adjust the frequency of hardware clock.
 */
static int stmmac_adjust_freq(struct ptp_clock_info *ptp, s32 ppb)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	unsigned long flags;
	u32 diff, addend;
	int neg_adj = 0;
	u64 adj;

	if (ppb < 0) {
		neg_adj = 1;
		ppb = -ppb;
	}

	addend = priv->default_addend;
	adj = addend;
	adj *= ppb;
	diff = div_u64(adj, 1000000000ULL);
	addend = neg_adj ? (addend - diff) : (addend + diff);

	spin_lock_irqsave(&priv->ptp_lock, flags);
	stmmac_config_addend(priv, priv->ptpaddr, addend);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return 0;
}

/**
 * stmmac_adjust_time
 *
 * @ptp: pointer to ptp_clock_info structure
 * @delta: desired change in nanoseconds
 *
 * Description: this function will shift/adjust the hardware clock time.
 */
static int stmmac_adjust_time(struct ptp_clock_info *ptp, s64 delta)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	unsigned long flags;
	u32 sec, nsec;
	u32 quotient, reminder;
	int neg_adj = 0;
	bool xmac;

	xmac = priv->plat->has_gmac4 || priv->plat->has_xgmac;

	if (delta < 0) {
		neg_adj = 1;
		delta = -delta;
	}

	quotient = div_u64_rem(delta, 1000000000ULL, &reminder);
	sec = quotient;
	nsec = reminder;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	stmmac_adjust_systime(priv, priv->ptpaddr, sec, nsec, neg_adj, xmac);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return 0;
}

/**
 * stmmac_get_time
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ts: pointer to hold time/result
 *
 * Description: this function will read the current time from the
 * hardware clock and store it in @ts.
 */
static int stmmac_get_time(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	unsigned long flags;
	u64 ns = 0;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	stmmac_get_systime(priv, priv->ptpaddr, &ns);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	*ts = ns_to_timespec64(ns);

	return 0;
}

/**
 * stmmac_set_time
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ts: time value to set
 *
 * Description: this function will set the current time on the
 * hardware clock.
 */
static int stmmac_set_time(struct ptp_clock_info *ptp,
			   const struct timespec64 *ts)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	unsigned long flags;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	stmmac_init_systime(priv, priv->ptpaddr, ts->tv_sec, ts->tv_nsec);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return 0;
}

static int stmmac_enable(struct ptp_clock_info *ptp,
			 struct ptp_clock_request *rq, int on)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	void __iomem *ptpaddr = priv->ptpaddr;
	void __iomem *ioaddr = priv->hw->pcsr;
	struct stmmac_pps_cfg *cfg;
	u32 intr_value, acr_value;
	int ret = -EOPNOTSUPP;
	unsigned long flags;

	switch (rq->type) {
	case PTP_CLK_REQ_PEROUT:
		/* Reject requests with unsupported flags */
		if (rq->perout.flags)
			return -EOPNOTSUPP;

		cfg = &priv->pps[rq->perout.index];

		cfg->start.tv_sec = rq->perout.start.sec;
		cfg->start.tv_nsec = rq->perout.start.nsec;
		cfg->period.tv_sec = rq->perout.period.sec;
		cfg->period.tv_nsec = rq->perout.period.nsec;

		spin_lock_irqsave(&priv->ptp_lock, flags);
		ret = stmmac_flex_pps_config(priv, priv->ioaddr,
					     rq->perout.index, cfg, on,
					     priv->sub_second_inc,
					     priv->systime_flags);
		spin_unlock_irqrestore(&priv->ptp_lock, flags);
		break;
	case PTP_CLK_REQ_EXTTS:
		priv->plat->ext_snapshot_en = on;
		mutex_lock(&priv->aux_ts_lock);
		acr_value = readl(ptpaddr + PTP_ACR);
		acr_value &= ~PTP_ACR_MASK;
		if (on) {
			/* Enable External snapshot trigger */
			acr_value |= priv->plat->ext_snapshot_num;
			acr_value |= PTP_ACR_ATSFC;
			netdev_dbg(priv->dev, "Auxiliary Snapshot %d enabled.\n",
				   priv->plat->ext_snapshot_num >>
				   PTP_ACR_ATSEN_SHIFT);
			/* Enable Timestamp Interrupt */
			intr_value = readl(ioaddr + GMAC_INT_EN);
			intr_value |= GMAC_INT_TSIE;
			writel(intr_value, ioaddr + GMAC_INT_EN);

		} else {
			netdev_dbg(priv->dev, "Auxiliary Snapshot %d disabled.\n",
				   priv->plat->ext_snapshot_num >>
				   PTP_ACR_ATSEN_SHIFT);
			/* Disable Timestamp Interrupt */
			intr_value = readl(ioaddr + GMAC_INT_EN);
			intr_value &= ~GMAC_INT_TSIE;
			writel(intr_value, ioaddr + GMAC_INT_EN);
		}
		writel(acr_value, ptpaddr + PTP_ACR);
		mutex_unlock(&priv->aux_ts_lock);
		ret = 0;
		break;

	default:
		break;
	}

	return ret;
}

/**
 * stmmac_get_syncdevicetime
 * @device: current device time
 * @system: system counter value read synchronously with device time
 * @ctx: context provided by timekeeping code
 * Description: Read device and system clock simultaneously and return the
 * corrected clock values in ns.
 **/
static int stmmac_get_syncdevicetime(ktime_t *device,
				     struct system_counterval_t *system,
				     void *ctx)
{
	struct stmmac_priv *priv = (struct stmmac_priv *)ctx;

	if (priv->plat->crosststamp)
		return priv->plat->crosststamp(device, system, ctx);
	else
		return -EOPNOTSUPP;
}

static int stmmac_getcrosststamp(struct ptp_clock_info *ptp,
				 struct system_device_crosststamp *xtstamp)
{
	struct stmmac_priv *priv =
		container_of(ptp, struct stmmac_priv, ptp_clock_ops);

	return get_device_system_crosststamp(stmmac_get_syncdevicetime,
					     priv, NULL, xtstamp);
}

/* structure describing a PTP hardware clock */
static struct ptp_clock_info stmmac_ptp_clock_ops = {
	.owner = THIS_MODULE,
	.name = "stmmac ptp",
	.max_adj = 62500000,
	.n_alarm = 0,
	.n_ext_ts = 0, /* will be overwritten in stmmac_ptp_register */
	.n_per_out = 0, /* will be overwritten in stmmac_ptp_register */
	.n_pins = 0,
	.pps = 0,
	.adjfreq = stmmac_adjust_freq,
	.adjtime = stmmac_adjust_time,
	.gettime64 = stmmac_get_time,
	.settime64 = stmmac_set_time,
	.enable = stmmac_enable,
	.getcrosststamp = stmmac_getcrosststamp,
};

/**
 * stmmac_ptp_register
 * @priv: driver private structure
 * Description: this function will register the ptp clock driver
 * to kernel. It also does some house keeping work.
 */
void stmmac_ptp_register(struct stmmac_priv *priv)
{
	int i;

	if (priv->plat->ptp_clk_freq_config)
		priv->plat->ptp_clk_freq_config(priv);

	for (i = 0; i < priv->dma_cap.pps_out_num; i++) {
		if (i >= STMMAC_PPS_MAX)
			break;
		priv->pps[i].available = true;
	}

	if (priv->plat->ptp_max_adj)
		stmmac_ptp_clock_ops.max_adj = priv->plat->ptp_max_adj;

	stmmac_ptp_clock_ops.n_per_out = priv->dma_cap.pps_out_num;
	stmmac_ptp_clock_ops.n_ext_ts = priv->dma_cap.aux_snapshot_n;

	spin_lock_init(&priv->ptp_lock);
	mutex_init(&priv->aux_ts_lock);
	priv->ptp_clock_ops = stmmac_ptp_clock_ops;

	priv->ptp_clock = ptp_clock_register(&priv->ptp_clock_ops,
					     priv->device);
	if (IS_ERR(priv->ptp_clock)) {
		netdev_err(priv->dev, "ptp_clock_register failed\n");
		priv->ptp_clock = NULL;
	} else if (priv->ptp_clock)
		netdev_info(priv->dev, "registered PTP clock\n");
}

/**
 * stmmac_ptp_unregister
 * @priv: driver private structure
 * Description: this function will remove/unregister the ptp clock driver
 * from the kernel.
 */
void stmmac_ptp_unregister(struct stmmac_priv *priv)
{
	if (priv->ptp_clock) {
		ptp_clock_unregister(priv->ptp_clock);
		priv->ptp_clock = NULL;
		pr_debug("Removed PTP HW clock successfully on %s\n",
			 priv->dev->name);
	}

	mutex_destroy(&priv->aux_ts_lock);
}
