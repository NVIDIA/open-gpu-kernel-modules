// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2009 Nokia Corporation
 * Author: Tomi Valkeinen <tomi.valkeinen@ti.com>
 */

#define DSS_SUBSYS_NAME "DSI"

#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/seq_file.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/pm_runtime.h>
#include <linux/of.h>
#include <linux/of_graph.h>
#include <linux/of_platform.h>
#include <linux/component.h>
#include <linux/sys_soc.h>

#include <drm/drm_bridge.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>
#include <video/mipi_display.h>

#include "omapdss.h"
#include "dss.h"

#define DSI_CATCH_MISSING_TE

#include "dsi.h"

#define REG_GET(dsi, idx, start, end) \
	FLD_GET(dsi_read_reg(dsi, idx), start, end)

#define REG_FLD_MOD(dsi, idx, val, start, end) \
	dsi_write_reg(dsi, idx, FLD_MOD(dsi_read_reg(dsi, idx), val, start, end))

static int dsi_init_dispc(struct dsi_data *dsi);
static void dsi_uninit_dispc(struct dsi_data *dsi);

static int dsi_vc_send_null(struct dsi_data *dsi, int vc, int channel);

static ssize_t _omap_dsi_host_transfer(struct dsi_data *dsi, int vc,
				       const struct mipi_dsi_msg *msg);

#ifdef DSI_PERF_MEASURE
static bool dsi_perf;
module_param(dsi_perf, bool, 0644);
#endif

/* Note: for some reason video mode seems to work only if VC_VIDEO is 0 */
#define VC_VIDEO	0
#define VC_CMD		1

#define drm_bridge_to_dsi(bridge) \
	container_of(bridge, struct dsi_data, bridge)

static inline struct dsi_data *to_dsi_data(struct omap_dss_device *dssdev)
{
	return dev_get_drvdata(dssdev->dev);
}

static inline struct dsi_data *host_to_omap(struct mipi_dsi_host *host)
{
	return container_of(host, struct dsi_data, host);
}

static inline void dsi_write_reg(struct dsi_data *dsi,
				 const struct dsi_reg idx, u32 val)
{
	void __iomem *base;

	switch(idx.module) {
		case DSI_PROTO: base = dsi->proto_base; break;
		case DSI_PHY: base = dsi->phy_base; break;
		case DSI_PLL: base = dsi->pll_base; break;
		default: return;
	}

	__raw_writel(val, base + idx.idx);
}

static inline u32 dsi_read_reg(struct dsi_data *dsi, const struct dsi_reg idx)
{
	void __iomem *base;

	switch(idx.module) {
		case DSI_PROTO: base = dsi->proto_base; break;
		case DSI_PHY: base = dsi->phy_base; break;
		case DSI_PLL: base = dsi->pll_base; break;
		default: return 0;
	}

	return __raw_readl(base + idx.idx);
}

static void dsi_bus_lock(struct dsi_data *dsi)
{
	down(&dsi->bus_lock);
}

static void dsi_bus_unlock(struct dsi_data *dsi)
{
	up(&dsi->bus_lock);
}

static bool dsi_bus_is_locked(struct dsi_data *dsi)
{
	return dsi->bus_lock.count == 0;
}

static void dsi_completion_handler(void *data, u32 mask)
{
	complete((struct completion *)data);
}

static inline bool wait_for_bit_change(struct dsi_data *dsi,
				       const struct dsi_reg idx,
				       int bitnum, int value)
{
	unsigned long timeout;
	ktime_t wait;
	int t;

	/* first busyloop to see if the bit changes right away */
	t = 100;
	while (t-- > 0) {
		if (REG_GET(dsi, idx, bitnum, bitnum) == value)
			return true;
	}

	/* then loop for 500ms, sleeping for 1ms in between */
	timeout = jiffies + msecs_to_jiffies(500);
	while (time_before(jiffies, timeout)) {
		if (REG_GET(dsi, idx, bitnum, bitnum) == value)
			return true;

		wait = ns_to_ktime(1000 * 1000);
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_hrtimeout(&wait, HRTIMER_MODE_REL);
	}

	return false;
}

#ifdef DSI_PERF_MEASURE
static void dsi_perf_mark_setup(struct dsi_data *dsi)
{
	dsi->perf_setup_time = ktime_get();
}

static void dsi_perf_mark_start(struct dsi_data *dsi)
{
	dsi->perf_start_time = ktime_get();
}

static void dsi_perf_show(struct dsi_data *dsi, const char *name)
{
	ktime_t t, setup_time, trans_time;
	u32 total_bytes;
	u32 setup_us, trans_us, total_us;

	if (!dsi_perf)
		return;

	t = ktime_get();

	setup_time = ktime_sub(dsi->perf_start_time, dsi->perf_setup_time);
	setup_us = (u32)ktime_to_us(setup_time);
	if (setup_us == 0)
		setup_us = 1;

	trans_time = ktime_sub(t, dsi->perf_start_time);
	trans_us = (u32)ktime_to_us(trans_time);
	if (trans_us == 0)
		trans_us = 1;

	total_us = setup_us + trans_us;

	total_bytes = dsi->update_bytes;

	pr_info("DSI(%s): %u us + %u us = %u us (%uHz), %u bytes, %u kbytes/sec\n",
		name,
		setup_us,
		trans_us,
		total_us,
		1000 * 1000 / total_us,
		total_bytes,
		total_bytes * 1000 / total_us);
}
#else
static inline void dsi_perf_mark_setup(struct dsi_data *dsi)
{
}

static inline void dsi_perf_mark_start(struct dsi_data *dsi)
{
}

static inline void dsi_perf_show(struct dsi_data *dsi, const char *name)
{
}
#endif

static int verbose_irq;

static void print_irq_status(u32 status)
{
	if (status == 0)
		return;

	if (!verbose_irq && (status & ~DSI_IRQ_CHANNEL_MASK) == 0)
		return;

#define PIS(x) (status & DSI_IRQ_##x) ? (#x " ") : ""

	pr_debug("DSI IRQ: 0x%x: %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		status,
		verbose_irq ? PIS(VC0) : "",
		verbose_irq ? PIS(VC1) : "",
		verbose_irq ? PIS(VC2) : "",
		verbose_irq ? PIS(VC3) : "",
		PIS(WAKEUP),
		PIS(RESYNC),
		PIS(PLL_LOCK),
		PIS(PLL_UNLOCK),
		PIS(PLL_RECALL),
		PIS(COMPLEXIO_ERR),
		PIS(HS_TX_TIMEOUT),
		PIS(LP_RX_TIMEOUT),
		PIS(TE_TRIGGER),
		PIS(ACK_TRIGGER),
		PIS(SYNC_LOST),
		PIS(LDO_POWER_GOOD),
		PIS(TA_TIMEOUT));
#undef PIS
}

static void print_irq_status_vc(int vc, u32 status)
{
	if (status == 0)
		return;

	if (!verbose_irq && (status & ~DSI_VC_IRQ_PACKET_SENT) == 0)
		return;

#define PIS(x) (status & DSI_VC_IRQ_##x) ? (#x " ") : ""

	pr_debug("DSI VC(%d) IRQ 0x%x: %s%s%s%s%s%s%s%s%s\n",
		vc,
		status,
		PIS(CS),
		PIS(ECC_CORR),
		PIS(ECC_NO_CORR),
		verbose_irq ? PIS(PACKET_SENT) : "",
		PIS(BTA),
		PIS(FIFO_TX_OVF),
		PIS(FIFO_RX_OVF),
		PIS(FIFO_TX_UDF),
		PIS(PP_BUSY_CHANGE));
#undef PIS
}

static void print_irq_status_cio(u32 status)
{
	if (status == 0)
		return;

#define PIS(x) (status & DSI_CIO_IRQ_##x) ? (#x " ") : ""

	pr_debug("DSI CIO IRQ 0x%x: %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		status,
		PIS(ERRSYNCESC1),
		PIS(ERRSYNCESC2),
		PIS(ERRSYNCESC3),
		PIS(ERRESC1),
		PIS(ERRESC2),
		PIS(ERRESC3),
		PIS(ERRCONTROL1),
		PIS(ERRCONTROL2),
		PIS(ERRCONTROL3),
		PIS(STATEULPS1),
		PIS(STATEULPS2),
		PIS(STATEULPS3),
		PIS(ERRCONTENTIONLP0_1),
		PIS(ERRCONTENTIONLP1_1),
		PIS(ERRCONTENTIONLP0_2),
		PIS(ERRCONTENTIONLP1_2),
		PIS(ERRCONTENTIONLP0_3),
		PIS(ERRCONTENTIONLP1_3),
		PIS(ULPSACTIVENOT_ALL0),
		PIS(ULPSACTIVENOT_ALL1));
#undef PIS
}

#ifdef CONFIG_OMAP2_DSS_COLLECT_IRQ_STATS
static void dsi_collect_irq_stats(struct dsi_data *dsi, u32 irqstatus,
				  u32 *vcstatus, u32 ciostatus)
{
	int i;

	spin_lock(&dsi->irq_stats_lock);

	dsi->irq_stats.irq_count++;
	dss_collect_irq_stats(irqstatus, dsi->irq_stats.dsi_irqs);

	for (i = 0; i < 4; ++i)
		dss_collect_irq_stats(vcstatus[i], dsi->irq_stats.vc_irqs[i]);

	dss_collect_irq_stats(ciostatus, dsi->irq_stats.cio_irqs);

	spin_unlock(&dsi->irq_stats_lock);
}
#else
#define dsi_collect_irq_stats(dsi, irqstatus, vcstatus, ciostatus)
#endif

static int debug_irq;

static void dsi_handle_irq_errors(struct dsi_data *dsi, u32 irqstatus,
				  u32 *vcstatus, u32 ciostatus)
{
	int i;

	if (irqstatus & DSI_IRQ_ERROR_MASK) {
		DSSERR("DSI error, irqstatus %x\n", irqstatus);
		print_irq_status(irqstatus);
		spin_lock(&dsi->errors_lock);
		dsi->errors |= irqstatus & DSI_IRQ_ERROR_MASK;
		spin_unlock(&dsi->errors_lock);
	} else if (debug_irq) {
		print_irq_status(irqstatus);
	}

	for (i = 0; i < 4; ++i) {
		if (vcstatus[i] & DSI_VC_IRQ_ERROR_MASK) {
			DSSERR("DSI VC(%d) error, vc irqstatus %x\n",
				       i, vcstatus[i]);
			print_irq_status_vc(i, vcstatus[i]);
		} else if (debug_irq) {
			print_irq_status_vc(i, vcstatus[i]);
		}
	}

	if (ciostatus & DSI_CIO_IRQ_ERROR_MASK) {
		DSSERR("DSI CIO error, cio irqstatus %x\n", ciostatus);
		print_irq_status_cio(ciostatus);
	} else if (debug_irq) {
		print_irq_status_cio(ciostatus);
	}
}

static void dsi_call_isrs(struct dsi_isr_data *isr_array,
		unsigned int isr_array_size, u32 irqstatus)
{
	struct dsi_isr_data *isr_data;
	int i;

	for (i = 0; i < isr_array_size; i++) {
		isr_data = &isr_array[i];
		if (isr_data->isr && isr_data->mask & irqstatus)
			isr_data->isr(isr_data->arg, irqstatus);
	}
}

static void dsi_handle_isrs(struct dsi_isr_tables *isr_tables,
		u32 irqstatus, u32 *vcstatus, u32 ciostatus)
{
	int i;

	dsi_call_isrs(isr_tables->isr_table,
			ARRAY_SIZE(isr_tables->isr_table),
			irqstatus);

	for (i = 0; i < 4; ++i) {
		if (vcstatus[i] == 0)
			continue;
		dsi_call_isrs(isr_tables->isr_table_vc[i],
				ARRAY_SIZE(isr_tables->isr_table_vc[i]),
				vcstatus[i]);
	}

	if (ciostatus != 0)
		dsi_call_isrs(isr_tables->isr_table_cio,
				ARRAY_SIZE(isr_tables->isr_table_cio),
				ciostatus);
}

static irqreturn_t omap_dsi_irq_handler(int irq, void *arg)
{
	struct dsi_data *dsi = arg;
	u32 irqstatus, vcstatus[4], ciostatus;
	int i;

	if (!dsi->is_enabled)
		return IRQ_NONE;

	spin_lock(&dsi->irq_lock);

	irqstatus = dsi_read_reg(dsi, DSI_IRQSTATUS);

	/* IRQ is not for us */
	if (!irqstatus) {
		spin_unlock(&dsi->irq_lock);
		return IRQ_NONE;
	}

	dsi_write_reg(dsi, DSI_IRQSTATUS, irqstatus & ~DSI_IRQ_CHANNEL_MASK);
	/* flush posted write */
	dsi_read_reg(dsi, DSI_IRQSTATUS);

	for (i = 0; i < 4; ++i) {
		if ((irqstatus & (1 << i)) == 0) {
			vcstatus[i] = 0;
			continue;
		}

		vcstatus[i] = dsi_read_reg(dsi, DSI_VC_IRQSTATUS(i));

		dsi_write_reg(dsi, DSI_VC_IRQSTATUS(i), vcstatus[i]);
		/* flush posted write */
		dsi_read_reg(dsi, DSI_VC_IRQSTATUS(i));
	}

	if (irqstatus & DSI_IRQ_COMPLEXIO_ERR) {
		ciostatus = dsi_read_reg(dsi, DSI_COMPLEXIO_IRQ_STATUS);

		dsi_write_reg(dsi, DSI_COMPLEXIO_IRQ_STATUS, ciostatus);
		/* flush posted write */
		dsi_read_reg(dsi, DSI_COMPLEXIO_IRQ_STATUS);
	} else {
		ciostatus = 0;
	}

#ifdef DSI_CATCH_MISSING_TE
	if (irqstatus & DSI_IRQ_TE_TRIGGER)
		del_timer(&dsi->te_timer);
#endif

	/* make a copy and unlock, so that isrs can unregister
	 * themselves */
	memcpy(&dsi->isr_tables_copy, &dsi->isr_tables,
		sizeof(dsi->isr_tables));

	spin_unlock(&dsi->irq_lock);

	dsi_handle_isrs(&dsi->isr_tables_copy, irqstatus, vcstatus, ciostatus);

	dsi_handle_irq_errors(dsi, irqstatus, vcstatus, ciostatus);

	dsi_collect_irq_stats(dsi, irqstatus, vcstatus, ciostatus);

	return IRQ_HANDLED;
}

/* dsi->irq_lock has to be locked by the caller */
static void _omap_dsi_configure_irqs(struct dsi_data *dsi,
				     struct dsi_isr_data *isr_array,
				     unsigned int isr_array_size,
				     u32 default_mask,
				     const struct dsi_reg enable_reg,
				     const struct dsi_reg status_reg)
{
	struct dsi_isr_data *isr_data;
	u32 mask;
	u32 old_mask;
	int i;

	mask = default_mask;

	for (i = 0; i < isr_array_size; i++) {
		isr_data = &isr_array[i];

		if (isr_data->isr == NULL)
			continue;

		mask |= isr_data->mask;
	}

	old_mask = dsi_read_reg(dsi, enable_reg);
	/* clear the irqstatus for newly enabled irqs */
	dsi_write_reg(dsi, status_reg, (mask ^ old_mask) & mask);
	dsi_write_reg(dsi, enable_reg, mask);

	/* flush posted writes */
	dsi_read_reg(dsi, enable_reg);
	dsi_read_reg(dsi, status_reg);
}

/* dsi->irq_lock has to be locked by the caller */
static void _omap_dsi_set_irqs(struct dsi_data *dsi)
{
	u32 mask = DSI_IRQ_ERROR_MASK;
#ifdef DSI_CATCH_MISSING_TE
	mask |= DSI_IRQ_TE_TRIGGER;
#endif
	_omap_dsi_configure_irqs(dsi, dsi->isr_tables.isr_table,
			ARRAY_SIZE(dsi->isr_tables.isr_table), mask,
			DSI_IRQENABLE, DSI_IRQSTATUS);
}

/* dsi->irq_lock has to be locked by the caller */
static void _omap_dsi_set_irqs_vc(struct dsi_data *dsi, int vc)
{
	_omap_dsi_configure_irqs(dsi, dsi->isr_tables.isr_table_vc[vc],
			ARRAY_SIZE(dsi->isr_tables.isr_table_vc[vc]),
			DSI_VC_IRQ_ERROR_MASK,
			DSI_VC_IRQENABLE(vc), DSI_VC_IRQSTATUS(vc));
}

/* dsi->irq_lock has to be locked by the caller */
static void _omap_dsi_set_irqs_cio(struct dsi_data *dsi)
{
	_omap_dsi_configure_irqs(dsi, dsi->isr_tables.isr_table_cio,
			ARRAY_SIZE(dsi->isr_tables.isr_table_cio),
			DSI_CIO_IRQ_ERROR_MASK,
			DSI_COMPLEXIO_IRQ_ENABLE, DSI_COMPLEXIO_IRQ_STATUS);
}

static void _dsi_initialize_irq(struct dsi_data *dsi)
{
	unsigned long flags;
	int vc;

	spin_lock_irqsave(&dsi->irq_lock, flags);

	memset(&dsi->isr_tables, 0, sizeof(dsi->isr_tables));

	_omap_dsi_set_irqs(dsi);
	for (vc = 0; vc < 4; ++vc)
		_omap_dsi_set_irqs_vc(dsi, vc);
	_omap_dsi_set_irqs_cio(dsi);

	spin_unlock_irqrestore(&dsi->irq_lock, flags);
}

static int _dsi_register_isr(omap_dsi_isr_t isr, void *arg, u32 mask,
		struct dsi_isr_data *isr_array, unsigned int isr_array_size)
{
	struct dsi_isr_data *isr_data;
	int free_idx;
	int i;

	BUG_ON(isr == NULL);

	/* check for duplicate entry and find a free slot */
	free_idx = -1;
	for (i = 0; i < isr_array_size; i++) {
		isr_data = &isr_array[i];

		if (isr_data->isr == isr && isr_data->arg == arg &&
				isr_data->mask == mask) {
			return -EINVAL;
		}

		if (isr_data->isr == NULL && free_idx == -1)
			free_idx = i;
	}

	if (free_idx == -1)
		return -EBUSY;

	isr_data = &isr_array[free_idx];
	isr_data->isr = isr;
	isr_data->arg = arg;
	isr_data->mask = mask;

	return 0;
}

static int _dsi_unregister_isr(omap_dsi_isr_t isr, void *arg, u32 mask,
		struct dsi_isr_data *isr_array, unsigned int isr_array_size)
{
	struct dsi_isr_data *isr_data;
	int i;

	for (i = 0; i < isr_array_size; i++) {
		isr_data = &isr_array[i];
		if (isr_data->isr != isr || isr_data->arg != arg ||
				isr_data->mask != mask)
			continue;

		isr_data->isr = NULL;
		isr_data->arg = NULL;
		isr_data->mask = 0;

		return 0;
	}

	return -EINVAL;
}

static int dsi_register_isr(struct dsi_data *dsi, omap_dsi_isr_t isr,
			    void *arg, u32 mask)
{
	unsigned long flags;
	int r;

	spin_lock_irqsave(&dsi->irq_lock, flags);

	r = _dsi_register_isr(isr, arg, mask, dsi->isr_tables.isr_table,
			ARRAY_SIZE(dsi->isr_tables.isr_table));

	if (r == 0)
		_omap_dsi_set_irqs(dsi);

	spin_unlock_irqrestore(&dsi->irq_lock, flags);

	return r;
}

static int dsi_unregister_isr(struct dsi_data *dsi, omap_dsi_isr_t isr,
			      void *arg, u32 mask)
{
	unsigned long flags;
	int r;

	spin_lock_irqsave(&dsi->irq_lock, flags);

	r = _dsi_unregister_isr(isr, arg, mask, dsi->isr_tables.isr_table,
			ARRAY_SIZE(dsi->isr_tables.isr_table));

	if (r == 0)
		_omap_dsi_set_irqs(dsi);

	spin_unlock_irqrestore(&dsi->irq_lock, flags);

	return r;
}

static int dsi_register_isr_vc(struct dsi_data *dsi, int vc,
			       omap_dsi_isr_t isr, void *arg, u32 mask)
{
	unsigned long flags;
	int r;

	spin_lock_irqsave(&dsi->irq_lock, flags);

	r = _dsi_register_isr(isr, arg, mask,
			dsi->isr_tables.isr_table_vc[vc],
			ARRAY_SIZE(dsi->isr_tables.isr_table_vc[vc]));

	if (r == 0)
		_omap_dsi_set_irqs_vc(dsi, vc);

	spin_unlock_irqrestore(&dsi->irq_lock, flags);

	return r;
}

static int dsi_unregister_isr_vc(struct dsi_data *dsi, int vc,
				 omap_dsi_isr_t isr, void *arg, u32 mask)
{
	unsigned long flags;
	int r;

	spin_lock_irqsave(&dsi->irq_lock, flags);

	r = _dsi_unregister_isr(isr, arg, mask,
			dsi->isr_tables.isr_table_vc[vc],
			ARRAY_SIZE(dsi->isr_tables.isr_table_vc[vc]));

	if (r == 0)
		_omap_dsi_set_irqs_vc(dsi, vc);

	spin_unlock_irqrestore(&dsi->irq_lock, flags);

	return r;
}

static u32 dsi_get_errors(struct dsi_data *dsi)
{
	unsigned long flags;
	u32 e;

	spin_lock_irqsave(&dsi->errors_lock, flags);
	e = dsi->errors;
	dsi->errors = 0;
	spin_unlock_irqrestore(&dsi->errors_lock, flags);
	return e;
}

static int dsi_runtime_get(struct dsi_data *dsi)
{
	int r;

	DSSDBG("dsi_runtime_get\n");

	r = pm_runtime_get_sync(dsi->dev);
	if (WARN_ON(r < 0)) {
		pm_runtime_put_noidle(dsi->dev);
		return r;
	}
	return 0;
}

static void dsi_runtime_put(struct dsi_data *dsi)
{
	int r;

	DSSDBG("dsi_runtime_put\n");

	r = pm_runtime_put_sync(dsi->dev);
	WARN_ON(r < 0 && r != -ENOSYS);
}

static void _dsi_print_reset_status(struct dsi_data *dsi)
{
	int b0, b1, b2;

	/* A dummy read using the SCP interface to any DSIPHY register is
	 * required after DSIPHY reset to complete the reset of the DSI complex
	 * I/O. */
	dsi_read_reg(dsi, DSI_DSIPHY_CFG5);

	if (dsi->data->quirks & DSI_QUIRK_REVERSE_TXCLKESC) {
		b0 = 28;
		b1 = 27;
		b2 = 26;
	} else {
		b0 = 24;
		b1 = 25;
		b2 = 26;
	}

#define DSI_FLD_GET(fld, start, end)\
	FLD_GET(dsi_read_reg(dsi, DSI_##fld), start, end)

	pr_debug("DSI resets: PLL (%d) CIO (%d) PHY (%x%x%x, %d, %d, %d)\n",
		DSI_FLD_GET(PLL_STATUS, 0, 0),
		DSI_FLD_GET(COMPLEXIO_CFG1, 29, 29),
		DSI_FLD_GET(DSIPHY_CFG5, b0, b0),
		DSI_FLD_GET(DSIPHY_CFG5, b1, b1),
		DSI_FLD_GET(DSIPHY_CFG5, b2, b2),
		DSI_FLD_GET(DSIPHY_CFG5, 29, 29),
		DSI_FLD_GET(DSIPHY_CFG5, 30, 30),
		DSI_FLD_GET(DSIPHY_CFG5, 31, 31));

#undef DSI_FLD_GET
}

static inline int dsi_if_enable(struct dsi_data *dsi, bool enable)
{
	DSSDBG("dsi_if_enable(%d)\n", enable);

	enable = enable ? 1 : 0;
	REG_FLD_MOD(dsi, DSI_CTRL, enable, 0, 0); /* IF_EN */

	if (!wait_for_bit_change(dsi, DSI_CTRL, 0, enable)) {
		DSSERR("Failed to set dsi_if_enable to %d\n", enable);
		return -EIO;
	}

	return 0;
}

static unsigned long dsi_get_pll_hsdiv_dispc_rate(struct dsi_data *dsi)
{
	return dsi->pll.cinfo.clkout[HSDIV_DISPC];
}

static unsigned long dsi_get_pll_hsdiv_dsi_rate(struct dsi_data *dsi)
{
	return dsi->pll.cinfo.clkout[HSDIV_DSI];
}

static unsigned long dsi_get_txbyteclkhs(struct dsi_data *dsi)
{
	return dsi->pll.cinfo.clkdco / 16;
}

static unsigned long dsi_fclk_rate(struct dsi_data *dsi)
{
	unsigned long r;
	enum dss_clk_source source;

	source = dss_get_dsi_clk_source(dsi->dss, dsi->module_id);
	if (source == DSS_CLK_SRC_FCK) {
		/* DSI FCLK source is DSS_CLK_FCK */
		r = clk_get_rate(dsi->dss_clk);
	} else {
		/* DSI FCLK source is dsi_pll_hsdiv_dsi_clk */
		r = dsi_get_pll_hsdiv_dsi_rate(dsi);
	}

	return r;
}

static int dsi_lp_clock_calc(unsigned long dsi_fclk,
		unsigned long lp_clk_min, unsigned long lp_clk_max,
		struct dsi_lp_clock_info *lp_cinfo)
{
	unsigned int lp_clk_div;
	unsigned long lp_clk;

	lp_clk_div = DIV_ROUND_UP(dsi_fclk, lp_clk_max * 2);
	lp_clk = dsi_fclk / 2 / lp_clk_div;

	if (lp_clk < lp_clk_min || lp_clk > lp_clk_max)
		return -EINVAL;

	lp_cinfo->lp_clk_div = lp_clk_div;
	lp_cinfo->lp_clk = lp_clk;

	return 0;
}

static int dsi_set_lp_clk_divisor(struct dsi_data *dsi)
{
	unsigned long dsi_fclk;
	unsigned int lp_clk_div;
	unsigned long lp_clk;
	unsigned int lpdiv_max = dsi->data->max_pll_lpdiv;


	lp_clk_div = dsi->user_lp_cinfo.lp_clk_div;

	if (lp_clk_div == 0 || lp_clk_div > lpdiv_max)
		return -EINVAL;

	dsi_fclk = dsi_fclk_rate(dsi);

	lp_clk = dsi_fclk / 2 / lp_clk_div;

	DSSDBG("LP_CLK_DIV %u, LP_CLK %lu\n", lp_clk_div, lp_clk);
	dsi->current_lp_cinfo.lp_clk = lp_clk;
	dsi->current_lp_cinfo.lp_clk_div = lp_clk_div;

	/* LP_CLK_DIVISOR */
	REG_FLD_MOD(dsi, DSI_CLK_CTRL, lp_clk_div, 12, 0);

	/* LP_RX_SYNCHRO_ENABLE */
	REG_FLD_MOD(dsi, DSI_CLK_CTRL, dsi_fclk > 30000000 ? 1 : 0, 21, 21);

	return 0;
}

static void dsi_enable_scp_clk(struct dsi_data *dsi)
{
	if (dsi->scp_clk_refcount++ == 0)
		REG_FLD_MOD(dsi, DSI_CLK_CTRL, 1, 14, 14); /* CIO_CLK_ICG */
}

static void dsi_disable_scp_clk(struct dsi_data *dsi)
{
	WARN_ON(dsi->scp_clk_refcount == 0);
	if (--dsi->scp_clk_refcount == 0)
		REG_FLD_MOD(dsi, DSI_CLK_CTRL, 0, 14, 14); /* CIO_CLK_ICG */
}

enum dsi_pll_power_state {
	DSI_PLL_POWER_OFF	= 0x0,
	DSI_PLL_POWER_ON_HSCLK	= 0x1,
	DSI_PLL_POWER_ON_ALL	= 0x2,
	DSI_PLL_POWER_ON_DIV	= 0x3,
};

static int dsi_pll_power(struct dsi_data *dsi, enum dsi_pll_power_state state)
{
	int t = 0;

	/* DSI-PLL power command 0x3 is not working */
	if ((dsi->data->quirks & DSI_QUIRK_PLL_PWR_BUG) &&
	    state == DSI_PLL_POWER_ON_DIV)
		state = DSI_PLL_POWER_ON_ALL;

	/* PLL_PWR_CMD */
	REG_FLD_MOD(dsi, DSI_CLK_CTRL, state, 31, 30);

	/* PLL_PWR_STATUS */
	while (FLD_GET(dsi_read_reg(dsi, DSI_CLK_CTRL), 29, 28) != state) {
		if (++t > 1000) {
			DSSERR("Failed to set DSI PLL power mode to %d\n",
					state);
			return -ENODEV;
		}
		udelay(1);
	}

	return 0;
}


static void dsi_pll_calc_dsi_fck(struct dsi_data *dsi,
				 struct dss_pll_clock_info *cinfo)
{
	unsigned long max_dsi_fck;

	max_dsi_fck = dsi->data->max_fck_freq;

	cinfo->mX[HSDIV_DSI] = DIV_ROUND_UP(cinfo->clkdco, max_dsi_fck);
	cinfo->clkout[HSDIV_DSI] = cinfo->clkdco / cinfo->mX[HSDIV_DSI];
}

static int dsi_pll_enable(struct dss_pll *pll)
{
	struct dsi_data *dsi = container_of(pll, struct dsi_data, pll);
	int r = 0;

	DSSDBG("PLL init\n");

	r = dsi_runtime_get(dsi);
	if (r)
		return r;

	/*
	 * Note: SCP CLK is not required on OMAP3, but it is required on OMAP4.
	 */
	dsi_enable_scp_clk(dsi);

	r = regulator_enable(dsi->vdds_dsi_reg);
	if (r)
		goto err0;

	/* XXX PLL does not come out of reset without this... */
	dispc_pck_free_enable(dsi->dss->dispc, 1);

	if (!wait_for_bit_change(dsi, DSI_PLL_STATUS, 0, 1)) {
		DSSERR("PLL not coming out of reset.\n");
		r = -ENODEV;
		dispc_pck_free_enable(dsi->dss->dispc, 0);
		goto err1;
	}

	/* XXX ... but if left on, we get problems when planes do not
	 * fill the whole display. No idea about this */
	dispc_pck_free_enable(dsi->dss->dispc, 0);

	r = dsi_pll_power(dsi, DSI_PLL_POWER_ON_ALL);

	if (r)
		goto err1;

	DSSDBG("PLL init done\n");

	return 0;
err1:
	regulator_disable(dsi->vdds_dsi_reg);
err0:
	dsi_disable_scp_clk(dsi);
	dsi_runtime_put(dsi);
	return r;
}

static void dsi_pll_disable(struct dss_pll *pll)
{
	struct dsi_data *dsi = container_of(pll, struct dsi_data, pll);

	dsi_pll_power(dsi, DSI_PLL_POWER_OFF);

	regulator_disable(dsi->vdds_dsi_reg);

	dsi_disable_scp_clk(dsi);
	dsi_runtime_put(dsi);

	DSSDBG("PLL disable done\n");
}

static int dsi_dump_dsi_clocks(struct seq_file *s, void *p)
{
	struct dsi_data *dsi = s->private;
	struct dss_pll_clock_info *cinfo = &dsi->pll.cinfo;
	enum dss_clk_source dispc_clk_src, dsi_clk_src;
	int dsi_module = dsi->module_id;
	struct dss_pll *pll = &dsi->pll;

	dispc_clk_src = dss_get_dispc_clk_source(dsi->dss);
	dsi_clk_src = dss_get_dsi_clk_source(dsi->dss, dsi_module);

	if (dsi_runtime_get(dsi))
		return 0;

	seq_printf(s,	"- DSI%d PLL -\n", dsi_module + 1);

	seq_printf(s,	"dsi pll clkin\t%lu\n", clk_get_rate(pll->clkin));

	seq_printf(s,	"Fint\t\t%-16lun %u\n", cinfo->fint, cinfo->n);

	seq_printf(s,	"CLKIN4DDR\t%-16lum %u\n",
			cinfo->clkdco, cinfo->m);

	seq_printf(s,	"DSI_PLL_HSDIV_DISPC (%s)\t%-16lum_dispc %u\t(%s)\n",
			dss_get_clk_source_name(dsi_module == 0 ?
				DSS_CLK_SRC_PLL1_1 :
				DSS_CLK_SRC_PLL2_1),
			cinfo->clkout[HSDIV_DISPC],
			cinfo->mX[HSDIV_DISPC],
			dispc_clk_src == DSS_CLK_SRC_FCK ?
			"off" : "on");

	seq_printf(s,	"DSI_PLL_HSDIV_DSI (%s)\t%-16lum_dsi %u\t(%s)\n",
			dss_get_clk_source_name(dsi_module == 0 ?
				DSS_CLK_SRC_PLL1_2 :
				DSS_CLK_SRC_PLL2_2),
			cinfo->clkout[HSDIV_DSI],
			cinfo->mX[HSDIV_DSI],
			dsi_clk_src == DSS_CLK_SRC_FCK ?
			"off" : "on");

	seq_printf(s,	"- DSI%d -\n", dsi_module + 1);

	seq_printf(s,	"dsi fclk source = %s\n",
			dss_get_clk_source_name(dsi_clk_src));

	seq_printf(s,	"DSI_FCLK\t%lu\n", dsi_fclk_rate(dsi));

	seq_printf(s,	"DDR_CLK\t\t%lu\n",
			cinfo->clkdco / 4);

	seq_printf(s,	"TxByteClkHS\t%lu\n", dsi_get_txbyteclkhs(dsi));

	seq_printf(s,	"LP_CLK\t\t%lu\n", dsi->current_lp_cinfo.lp_clk);

	dsi_runtime_put(dsi);

	return 0;
}

#ifdef CONFIG_OMAP2_DSS_COLLECT_IRQ_STATS
static int dsi_dump_dsi_irqs(struct seq_file *s, void *p)
{
	struct dsi_data *dsi = s->private;
	unsigned long flags;
	struct dsi_irq_stats stats;

	spin_lock_irqsave(&dsi->irq_stats_lock, flags);

	stats = dsi->irq_stats;
	memset(&dsi->irq_stats, 0, sizeof(dsi->irq_stats));
	dsi->irq_stats.last_reset = jiffies;

	spin_unlock_irqrestore(&dsi->irq_stats_lock, flags);

	seq_printf(s, "period %u ms\n",
			jiffies_to_msecs(jiffies - stats.last_reset));

	seq_printf(s, "irqs %d\n", stats.irq_count);
#define PIS(x) \
	seq_printf(s, "%-20s %10d\n", #x, stats.dsi_irqs[ffs(DSI_IRQ_##x)-1]);

	seq_printf(s, "-- DSI%d interrupts --\n", dsi->module_id + 1);
	PIS(VC0);
	PIS(VC1);
	PIS(VC2);
	PIS(VC3);
	PIS(WAKEUP);
	PIS(RESYNC);
	PIS(PLL_LOCK);
	PIS(PLL_UNLOCK);
	PIS(PLL_RECALL);
	PIS(COMPLEXIO_ERR);
	PIS(HS_TX_TIMEOUT);
	PIS(LP_RX_TIMEOUT);
	PIS(TE_TRIGGER);
	PIS(ACK_TRIGGER);
	PIS(SYNC_LOST);
	PIS(LDO_POWER_GOOD);
	PIS(TA_TIMEOUT);
#undef PIS

#define PIS(x) \
	seq_printf(s, "%-20s %10d %10d %10d %10d\n", #x, \
			stats.vc_irqs[0][ffs(DSI_VC_IRQ_##x)-1], \
			stats.vc_irqs[1][ffs(DSI_VC_IRQ_##x)-1], \
			stats.vc_irqs[2][ffs(DSI_VC_IRQ_##x)-1], \
			stats.vc_irqs[3][ffs(DSI_VC_IRQ_##x)-1]);

	seq_printf(s, "-- VC interrupts --\n");
	PIS(CS);
	PIS(ECC_CORR);
	PIS(PACKET_SENT);
	PIS(FIFO_TX_OVF);
	PIS(FIFO_RX_OVF);
	PIS(BTA);
	PIS(ECC_NO_CORR);
	PIS(FIFO_TX_UDF);
	PIS(PP_BUSY_CHANGE);
#undef PIS

#define PIS(x) \
	seq_printf(s, "%-20s %10d\n", #x, \
			stats.cio_irqs[ffs(DSI_CIO_IRQ_##x)-1]);

	seq_printf(s, "-- CIO interrupts --\n");
	PIS(ERRSYNCESC1);
	PIS(ERRSYNCESC2);
	PIS(ERRSYNCESC3);
	PIS(ERRESC1);
	PIS(ERRESC2);
	PIS(ERRESC3);
	PIS(ERRCONTROL1);
	PIS(ERRCONTROL2);
	PIS(ERRCONTROL3);
	PIS(STATEULPS1);
	PIS(STATEULPS2);
	PIS(STATEULPS3);
	PIS(ERRCONTENTIONLP0_1);
	PIS(ERRCONTENTIONLP1_1);
	PIS(ERRCONTENTIONLP0_2);
	PIS(ERRCONTENTIONLP1_2);
	PIS(ERRCONTENTIONLP0_3);
	PIS(ERRCONTENTIONLP1_3);
	PIS(ULPSACTIVENOT_ALL0);
	PIS(ULPSACTIVENOT_ALL1);
#undef PIS

	return 0;
}
#endif

static int dsi_dump_dsi_regs(struct seq_file *s, void *p)
{
	struct dsi_data *dsi = s->private;

	if (dsi_runtime_get(dsi))
		return 0;
	dsi_enable_scp_clk(dsi);

#define DUMPREG(r) seq_printf(s, "%-35s %08x\n", #r, dsi_read_reg(dsi, r))
	DUMPREG(DSI_REVISION);
	DUMPREG(DSI_SYSCONFIG);
	DUMPREG(DSI_SYSSTATUS);
	DUMPREG(DSI_IRQSTATUS);
	DUMPREG(DSI_IRQENABLE);
	DUMPREG(DSI_CTRL);
	DUMPREG(DSI_COMPLEXIO_CFG1);
	DUMPREG(DSI_COMPLEXIO_IRQ_STATUS);
	DUMPREG(DSI_COMPLEXIO_IRQ_ENABLE);
	DUMPREG(DSI_CLK_CTRL);
	DUMPREG(DSI_TIMING1);
	DUMPREG(DSI_TIMING2);
	DUMPREG(DSI_VM_TIMING1);
	DUMPREG(DSI_VM_TIMING2);
	DUMPREG(DSI_VM_TIMING3);
	DUMPREG(DSI_CLK_TIMING);
	DUMPREG(DSI_TX_FIFO_VC_SIZE);
	DUMPREG(DSI_RX_FIFO_VC_SIZE);
	DUMPREG(DSI_COMPLEXIO_CFG2);
	DUMPREG(DSI_RX_FIFO_VC_FULLNESS);
	DUMPREG(DSI_VM_TIMING4);
	DUMPREG(DSI_TX_FIFO_VC_EMPTINESS);
	DUMPREG(DSI_VM_TIMING5);
	DUMPREG(DSI_VM_TIMING6);
	DUMPREG(DSI_VM_TIMING7);
	DUMPREG(DSI_STOPCLK_TIMING);

	DUMPREG(DSI_VC_CTRL(0));
	DUMPREG(DSI_VC_TE(0));
	DUMPREG(DSI_VC_LONG_PACKET_HEADER(0));
	DUMPREG(DSI_VC_LONG_PACKET_PAYLOAD(0));
	DUMPREG(DSI_VC_SHORT_PACKET_HEADER(0));
	DUMPREG(DSI_VC_IRQSTATUS(0));
	DUMPREG(DSI_VC_IRQENABLE(0));

	DUMPREG(DSI_VC_CTRL(1));
	DUMPREG(DSI_VC_TE(1));
	DUMPREG(DSI_VC_LONG_PACKET_HEADER(1));
	DUMPREG(DSI_VC_LONG_PACKET_PAYLOAD(1));
	DUMPREG(DSI_VC_SHORT_PACKET_HEADER(1));
	DUMPREG(DSI_VC_IRQSTATUS(1));
	DUMPREG(DSI_VC_IRQENABLE(1));

	DUMPREG(DSI_VC_CTRL(2));
	DUMPREG(DSI_VC_TE(2));
	DUMPREG(DSI_VC_LONG_PACKET_HEADER(2));
	DUMPREG(DSI_VC_LONG_PACKET_PAYLOAD(2));
	DUMPREG(DSI_VC_SHORT_PACKET_HEADER(2));
	DUMPREG(DSI_VC_IRQSTATUS(2));
	DUMPREG(DSI_VC_IRQENABLE(2));

	DUMPREG(DSI_VC_CTRL(3));
	DUMPREG(DSI_VC_TE(3));
	DUMPREG(DSI_VC_LONG_PACKET_HEADER(3));
	DUMPREG(DSI_VC_LONG_PACKET_PAYLOAD(3));
	DUMPREG(DSI_VC_SHORT_PACKET_HEADER(3));
	DUMPREG(DSI_VC_IRQSTATUS(3));
	DUMPREG(DSI_VC_IRQENABLE(3));

	DUMPREG(DSI_DSIPHY_CFG0);
	DUMPREG(DSI_DSIPHY_CFG1);
	DUMPREG(DSI_DSIPHY_CFG2);
	DUMPREG(DSI_DSIPHY_CFG5);

	DUMPREG(DSI_PLL_CONTROL);
	DUMPREG(DSI_PLL_STATUS);
	DUMPREG(DSI_PLL_GO);
	DUMPREG(DSI_PLL_CONFIGURATION1);
	DUMPREG(DSI_PLL_CONFIGURATION2);
#undef DUMPREG

	dsi_disable_scp_clk(dsi);
	dsi_runtime_put(dsi);

	return 0;
}

enum dsi_cio_power_state {
	DSI_COMPLEXIO_POWER_OFF		= 0x0,
	DSI_COMPLEXIO_POWER_ON		= 0x1,
	DSI_COMPLEXIO_POWER_ULPS	= 0x2,
};

static int dsi_cio_power(struct dsi_data *dsi, enum dsi_cio_power_state state)
{
	int t = 0;

	/* PWR_CMD */
	REG_FLD_MOD(dsi, DSI_COMPLEXIO_CFG1, state, 28, 27);

	/* PWR_STATUS */
	while (FLD_GET(dsi_read_reg(dsi, DSI_COMPLEXIO_CFG1),
			26, 25) != state) {
		if (++t > 1000) {
			DSSERR("failed to set complexio power state to "
					"%d\n", state);
			return -ENODEV;
		}
		udelay(1);
	}

	return 0;
}

static unsigned int dsi_get_line_buf_size(struct dsi_data *dsi)
{
	int val;

	/* line buffer on OMAP3 is 1024 x 24bits */
	/* XXX: for some reason using full buffer size causes
	 * considerable TX slowdown with update sizes that fill the
	 * whole buffer */
	if (!(dsi->data->quirks & DSI_QUIRK_GNQ))
		return 1023 * 3;

	val = REG_GET(dsi, DSI_GNQ, 14, 12); /* VP1_LINE_BUFFER_SIZE */

	switch (val) {
	case 1:
		return 512 * 3;		/* 512x24 bits */
	case 2:
		return 682 * 3;		/* 682x24 bits */
	case 3:
		return 853 * 3;		/* 853x24 bits */
	case 4:
		return 1024 * 3;	/* 1024x24 bits */
	case 5:
		return 1194 * 3;	/* 1194x24 bits */
	case 6:
		return 1365 * 3;	/* 1365x24 bits */
	case 7:
		return 1920 * 3;	/* 1920x24 bits */
	default:
		BUG();
		return 0;
	}
}

static int dsi_set_lane_config(struct dsi_data *dsi)
{
	static const u8 offsets[] = { 0, 4, 8, 12, 16 };
	static const enum dsi_lane_function functions[] = {
		DSI_LANE_CLK,
		DSI_LANE_DATA1,
		DSI_LANE_DATA2,
		DSI_LANE_DATA3,
		DSI_LANE_DATA4,
	};
	u32 r;
	int i;

	r = dsi_read_reg(dsi, DSI_COMPLEXIO_CFG1);

	for (i = 0; i < dsi->num_lanes_used; ++i) {
		unsigned int offset = offsets[i];
		unsigned int polarity, lane_number;
		unsigned int t;

		for (t = 0; t < dsi->num_lanes_supported; ++t)
			if (dsi->lanes[t].function == functions[i])
				break;

		if (t == dsi->num_lanes_supported)
			return -EINVAL;

		lane_number = t;
		polarity = dsi->lanes[t].polarity;

		r = FLD_MOD(r, lane_number + 1, offset + 2, offset);
		r = FLD_MOD(r, polarity, offset + 3, offset + 3);
	}

	/* clear the unused lanes */
	for (; i < dsi->num_lanes_supported; ++i) {
		unsigned int offset = offsets[i];

		r = FLD_MOD(r, 0, offset + 2, offset);
		r = FLD_MOD(r, 0, offset + 3, offset + 3);
	}

	dsi_write_reg(dsi, DSI_COMPLEXIO_CFG1, r);

	return 0;
}

static inline unsigned int ns2ddr(struct dsi_data *dsi, unsigned int ns)
{
	/* convert time in ns to ddr ticks, rounding up */
	unsigned long ddr_clk = dsi->pll.cinfo.clkdco / 4;

	return (ns * (ddr_clk / 1000 / 1000) + 999) / 1000;
}

static inline unsigned int ddr2ns(struct dsi_data *dsi, unsigned int ddr)
{
	unsigned long ddr_clk = dsi->pll.cinfo.clkdco / 4;

	return ddr * 1000 * 1000 / (ddr_clk / 1000);
}

static void dsi_cio_timings(struct dsi_data *dsi)
{
	u32 r;
	u32 ths_prepare, ths_prepare_ths_zero, ths_trail, ths_exit;
	u32 tlpx_half, tclk_trail, tclk_zero;
	u32 tclk_prepare;

	/* calculate timings */

	/* 1 * DDR_CLK = 2 * UI */

	/* min 40ns + 4*UI	max 85ns + 6*UI */
	ths_prepare = ns2ddr(dsi, 70) + 2;

	/* min 145ns + 10*UI */
	ths_prepare_ths_zero = ns2ddr(dsi, 175) + 2;

	/* min max(8*UI, 60ns+4*UI) */
	ths_trail = ns2ddr(dsi, 60) + 5;

	/* min 100ns */
	ths_exit = ns2ddr(dsi, 145);

	/* tlpx min 50n */
	tlpx_half = ns2ddr(dsi, 25);

	/* min 60ns */
	tclk_trail = ns2ddr(dsi, 60) + 2;

	/* min 38ns, max 95ns */
	tclk_prepare = ns2ddr(dsi, 65);

	/* min tclk-prepare + tclk-zero = 300ns */
	tclk_zero = ns2ddr(dsi, 260);

	DSSDBG("ths_prepare %u (%uns), ths_prepare_ths_zero %u (%uns)\n",
		ths_prepare, ddr2ns(dsi, ths_prepare),
		ths_prepare_ths_zero, ddr2ns(dsi, ths_prepare_ths_zero));
	DSSDBG("ths_trail %u (%uns), ths_exit %u (%uns)\n",
			ths_trail, ddr2ns(dsi, ths_trail),
			ths_exit, ddr2ns(dsi, ths_exit));

	DSSDBG("tlpx_half %u (%uns), tclk_trail %u (%uns), "
			"tclk_zero %u (%uns)\n",
			tlpx_half, ddr2ns(dsi, tlpx_half),
			tclk_trail, ddr2ns(dsi, tclk_trail),
			tclk_zero, ddr2ns(dsi, tclk_zero));
	DSSDBG("tclk_prepare %u (%uns)\n",
			tclk_prepare, ddr2ns(dsi, tclk_prepare));

	/* program timings */

	r = dsi_read_reg(dsi, DSI_DSIPHY_CFG0);
	r = FLD_MOD(r, ths_prepare, 31, 24);
	r = FLD_MOD(r, ths_prepare_ths_zero, 23, 16);
	r = FLD_MOD(r, ths_trail, 15, 8);
	r = FLD_MOD(r, ths_exit, 7, 0);
	dsi_write_reg(dsi, DSI_DSIPHY_CFG0, r);

	r = dsi_read_reg(dsi, DSI_DSIPHY_CFG1);
	r = FLD_MOD(r, tlpx_half, 20, 16);
	r = FLD_MOD(r, tclk_trail, 15, 8);
	r = FLD_MOD(r, tclk_zero, 7, 0);

	if (dsi->data->quirks & DSI_QUIRK_PHY_DCC) {
		r = FLD_MOD(r, 0, 21, 21);	/* DCCEN = disable */
		r = FLD_MOD(r, 1, 22, 22);	/* CLKINP_DIVBY2EN = enable */
		r = FLD_MOD(r, 1, 23, 23);	/* CLKINP_SEL = enable */
	}

	dsi_write_reg(dsi, DSI_DSIPHY_CFG1, r);

	r = dsi_read_reg(dsi, DSI_DSIPHY_CFG2);
	r = FLD_MOD(r, tclk_prepare, 7, 0);
	dsi_write_reg(dsi, DSI_DSIPHY_CFG2, r);
}

static int dsi_cio_wait_tx_clk_esc_reset(struct dsi_data *dsi)
{
	int t, i;
	bool in_use[DSI_MAX_NR_LANES];
	static const u8 offsets_old[] = { 28, 27, 26 };
	static const u8 offsets_new[] = { 24, 25, 26, 27, 28 };
	const u8 *offsets;

	if (dsi->data->quirks & DSI_QUIRK_REVERSE_TXCLKESC)
		offsets = offsets_old;
	else
		offsets = offsets_new;

	for (i = 0; i < dsi->num_lanes_supported; ++i)
		in_use[i] = dsi->lanes[i].function != DSI_LANE_UNUSED;

	t = 100000;
	while (true) {
		u32 l;
		int ok;

		l = dsi_read_reg(dsi, DSI_DSIPHY_CFG5);

		ok = 0;
		for (i = 0; i < dsi->num_lanes_supported; ++i) {
			if (!in_use[i] || (l & (1 << offsets[i])))
				ok++;
		}

		if (ok == dsi->num_lanes_supported)
			break;

		if (--t == 0) {
			for (i = 0; i < dsi->num_lanes_supported; ++i) {
				if (!in_use[i] || (l & (1 << offsets[i])))
					continue;

				DSSERR("CIO TXCLKESC%d domain not coming " \
						"out of reset\n", i);
			}
			return -EIO;
		}
	}

	return 0;
}

/* return bitmask of enabled lanes, lane0 being the lsb */
static unsigned int dsi_get_lane_mask(struct dsi_data *dsi)
{
	unsigned int mask = 0;
	int i;

	for (i = 0; i < dsi->num_lanes_supported; ++i) {
		if (dsi->lanes[i].function != DSI_LANE_UNUSED)
			mask |= 1 << i;
	}

	return mask;
}

/* OMAP4 CONTROL_DSIPHY */
#define OMAP4_DSIPHY_SYSCON_OFFSET			0x78

#define OMAP4_DSI2_LANEENABLE_SHIFT			29
#define OMAP4_DSI2_LANEENABLE_MASK			(0x7 << 29)
#define OMAP4_DSI1_LANEENABLE_SHIFT			24
#define OMAP4_DSI1_LANEENABLE_MASK			(0x1f << 24)
#define OMAP4_DSI1_PIPD_SHIFT				19
#define OMAP4_DSI1_PIPD_MASK				(0x1f << 19)
#define OMAP4_DSI2_PIPD_SHIFT				14
#define OMAP4_DSI2_PIPD_MASK				(0x1f << 14)

static int dsi_omap4_mux_pads(struct dsi_data *dsi, unsigned int lanes)
{
	u32 enable_mask, enable_shift;
	u32 pipd_mask, pipd_shift;

	if (dsi->module_id == 0) {
		enable_mask = OMAP4_DSI1_LANEENABLE_MASK;
		enable_shift = OMAP4_DSI1_LANEENABLE_SHIFT;
		pipd_mask = OMAP4_DSI1_PIPD_MASK;
		pipd_shift = OMAP4_DSI1_PIPD_SHIFT;
	} else if (dsi->module_id == 1) {
		enable_mask = OMAP4_DSI2_LANEENABLE_MASK;
		enable_shift = OMAP4_DSI2_LANEENABLE_SHIFT;
		pipd_mask = OMAP4_DSI2_PIPD_MASK;
		pipd_shift = OMAP4_DSI2_PIPD_SHIFT;
	} else {
		return -ENODEV;
	}

	return regmap_update_bits(dsi->syscon, OMAP4_DSIPHY_SYSCON_OFFSET,
		enable_mask | pipd_mask,
		(lanes << enable_shift) | (lanes << pipd_shift));
}

/* OMAP5 CONTROL_DSIPHY */

#define OMAP5_DSIPHY_SYSCON_OFFSET	0x74

#define OMAP5_DSI1_LANEENABLE_SHIFT	24
#define OMAP5_DSI2_LANEENABLE_SHIFT	19
#define OMAP5_DSI_LANEENABLE_MASK	0x1f

static int dsi_omap5_mux_pads(struct dsi_data *dsi, unsigned int lanes)
{
	u32 enable_shift;

	if (dsi->module_id == 0)
		enable_shift = OMAP5_DSI1_LANEENABLE_SHIFT;
	else if (dsi->module_id == 1)
		enable_shift = OMAP5_DSI2_LANEENABLE_SHIFT;
	else
		return -ENODEV;

	return regmap_update_bits(dsi->syscon, OMAP5_DSIPHY_SYSCON_OFFSET,
		OMAP5_DSI_LANEENABLE_MASK << enable_shift,
		lanes << enable_shift);
}

static int dsi_enable_pads(struct dsi_data *dsi, unsigned int lane_mask)
{
	if (dsi->data->model == DSI_MODEL_OMAP4)
		return dsi_omap4_mux_pads(dsi, lane_mask);
	if (dsi->data->model == DSI_MODEL_OMAP5)
		return dsi_omap5_mux_pads(dsi, lane_mask);
	return 0;
}

static void dsi_disable_pads(struct dsi_data *dsi)
{
	if (dsi->data->model == DSI_MODEL_OMAP4)
		dsi_omap4_mux_pads(dsi, 0);
	else if (dsi->data->model == DSI_MODEL_OMAP5)
		dsi_omap5_mux_pads(dsi, 0);
}

static int dsi_cio_init(struct dsi_data *dsi)
{
	int r;
	u32 l;

	DSSDBG("DSI CIO init starts");

	r = dsi_enable_pads(dsi, dsi_get_lane_mask(dsi));
	if (r)
		return r;

	dsi_enable_scp_clk(dsi);

	/* A dummy read using the SCP interface to any DSIPHY register is
	 * required after DSIPHY reset to complete the reset of the DSI complex
	 * I/O. */
	dsi_read_reg(dsi, DSI_DSIPHY_CFG5);

	if (!wait_for_bit_change(dsi, DSI_DSIPHY_CFG5, 30, 1)) {
		DSSERR("CIO SCP Clock domain not coming out of reset.\n");
		r = -EIO;
		goto err_scp_clk_dom;
	}

	r = dsi_set_lane_config(dsi);
	if (r)
		goto err_scp_clk_dom;

	/* set TX STOP MODE timer to maximum for this operation */
	l = dsi_read_reg(dsi, DSI_TIMING1);
	l = FLD_MOD(l, 1, 15, 15);	/* FORCE_TX_STOP_MODE_IO */
	l = FLD_MOD(l, 1, 14, 14);	/* STOP_STATE_X16_IO */
	l = FLD_MOD(l, 1, 13, 13);	/* STOP_STATE_X4_IO */
	l = FLD_MOD(l, 0x1fff, 12, 0);	/* STOP_STATE_COUNTER_IO */
	dsi_write_reg(dsi, DSI_TIMING1, l);

	r = dsi_cio_power(dsi, DSI_COMPLEXIO_POWER_ON);
	if (r)
		goto err_cio_pwr;

	if (!wait_for_bit_change(dsi, DSI_COMPLEXIO_CFG1, 29, 1)) {
		DSSERR("CIO PWR clock domain not coming out of reset.\n");
		r = -ENODEV;
		goto err_cio_pwr_dom;
	}

	dsi_if_enable(dsi, true);
	dsi_if_enable(dsi, false);
	REG_FLD_MOD(dsi, DSI_CLK_CTRL, 1, 20, 20); /* LP_CLK_ENABLE */

	r = dsi_cio_wait_tx_clk_esc_reset(dsi);
	if (r)
		goto err_tx_clk_esc_rst;

	/* FORCE_TX_STOP_MODE_IO */
	REG_FLD_MOD(dsi, DSI_TIMING1, 0, 15, 15);

	dsi_cio_timings(dsi);

	/* DDR_CLK_ALWAYS_ON */
	REG_FLD_MOD(dsi, DSI_CLK_CTRL,
		    !(dsi->dsidev->mode_flags & MIPI_DSI_CLOCK_NON_CONTINUOUS),
		    13, 13);

	DSSDBG("CIO init done\n");

	return 0;

err_tx_clk_esc_rst:
	REG_FLD_MOD(dsi, DSI_CLK_CTRL, 0, 20, 20); /* LP_CLK_ENABLE */
err_cio_pwr_dom:
	dsi_cio_power(dsi, DSI_COMPLEXIO_POWER_OFF);
err_cio_pwr:
err_scp_clk_dom:
	dsi_disable_scp_clk(dsi);
	dsi_disable_pads(dsi);
	return r;
}

static void dsi_cio_uninit(struct dsi_data *dsi)
{
	/* DDR_CLK_ALWAYS_ON */
	REG_FLD_MOD(dsi, DSI_CLK_CTRL, 0, 13, 13);

	dsi_cio_power(dsi, DSI_COMPLEXIO_POWER_OFF);
	dsi_disable_scp_clk(dsi);
	dsi_disable_pads(dsi);
}

static void dsi_config_tx_fifo(struct dsi_data *dsi,
			       enum fifo_size size1, enum fifo_size size2,
			       enum fifo_size size3, enum fifo_size size4)
{
	u32 r = 0;
	int add = 0;
	int i;

	dsi->vc[0].tx_fifo_size = size1;
	dsi->vc[1].tx_fifo_size = size2;
	dsi->vc[2].tx_fifo_size = size3;
	dsi->vc[3].tx_fifo_size = size4;

	for (i = 0; i < 4; i++) {
		u8 v;
		int size = dsi->vc[i].tx_fifo_size;

		if (add + size > 4) {
			DSSERR("Illegal FIFO configuration\n");
			BUG();
			return;
		}

		v = FLD_VAL(add, 2, 0) | FLD_VAL(size, 7, 4);
		r |= v << (8 * i);
		/*DSSDBG("TX FIFO vc %d: size %d, add %d\n", i, size, add); */
		add += size;
	}

	dsi_write_reg(dsi, DSI_TX_FIFO_VC_SIZE, r);
}

static void dsi_config_rx_fifo(struct dsi_data *dsi,
		enum fifo_size size1, enum fifo_size size2,
		enum fifo_size size3, enum fifo_size size4)
{
	u32 r = 0;
	int add = 0;
	int i;

	dsi->vc[0].rx_fifo_size = size1;
	dsi->vc[1].rx_fifo_size = size2;
	dsi->vc[2].rx_fifo_size = size3;
	dsi->vc[3].rx_fifo_size = size4;

	for (i = 0; i < 4; i++) {
		u8 v;
		int size = dsi->vc[i].rx_fifo_size;

		if (add + size > 4) {
			DSSERR("Illegal FIFO configuration\n");
			BUG();
			return;
		}

		v = FLD_VAL(add, 2, 0) | FLD_VAL(size, 7, 4);
		r |= v << (8 * i);
		/*DSSDBG("RX FIFO vc %d: size %d, add %d\n", i, size, add); */
		add += size;
	}

	dsi_write_reg(dsi, DSI_RX_FIFO_VC_SIZE, r);
}

static int dsi_force_tx_stop_mode_io(struct dsi_data *dsi)
{
	u32 r;

	r = dsi_read_reg(dsi, DSI_TIMING1);
	r = FLD_MOD(r, 1, 15, 15);	/* FORCE_TX_STOP_MODE_IO */
	dsi_write_reg(dsi, DSI_TIMING1, r);

	if (!wait_for_bit_change(dsi, DSI_TIMING1, 15, 0)) {
		DSSERR("TX_STOP bit not going down\n");
		return -EIO;
	}

	return 0;
}

static bool dsi_vc_is_enabled(struct dsi_data *dsi, int vc)
{
	return REG_GET(dsi, DSI_VC_CTRL(vc), 0, 0);
}

static void dsi_packet_sent_handler_vp(void *data, u32 mask)
{
	struct dsi_packet_sent_handler_data *vp_data =
		(struct dsi_packet_sent_handler_data *) data;
	struct dsi_data *dsi = vp_data->dsi;
	const int vc = dsi->update_vc;
	u8 bit = dsi->te_enabled ? 30 : 31;

	if (REG_GET(dsi, DSI_VC_TE(vc), bit, bit) == 0)
		complete(vp_data->completion);
}

static int dsi_sync_vc_vp(struct dsi_data *dsi, int vc)
{
	DECLARE_COMPLETION_ONSTACK(completion);
	struct dsi_packet_sent_handler_data vp_data = {
		.dsi = dsi,
		.completion = &completion
	};
	int r = 0;
	u8 bit;

	bit = dsi->te_enabled ? 30 : 31;

	r = dsi_register_isr_vc(dsi, vc, dsi_packet_sent_handler_vp,
		&vp_data, DSI_VC_IRQ_PACKET_SENT);
	if (r)
		goto err0;

	/* Wait for completion only if TE_EN/TE_START is still set */
	if (REG_GET(dsi, DSI_VC_TE(vc), bit, bit)) {
		if (wait_for_completion_timeout(&completion,
				msecs_to_jiffies(10)) == 0) {
			DSSERR("Failed to complete previous frame transfer\n");
			r = -EIO;
			goto err1;
		}
	}

	dsi_unregister_isr_vc(dsi, vc, dsi_packet_sent_handler_vp,
		&vp_data, DSI_VC_IRQ_PACKET_SENT);

	return 0;
err1:
	dsi_unregister_isr_vc(dsi, vc, dsi_packet_sent_handler_vp,
		&vp_data, DSI_VC_IRQ_PACKET_SENT);
err0:
	return r;
}

static void dsi_packet_sent_handler_l4(void *data, u32 mask)
{
	struct dsi_packet_sent_handler_data *l4_data =
		(struct dsi_packet_sent_handler_data *) data;
	struct dsi_data *dsi = l4_data->dsi;
	const int vc = dsi->update_vc;

	if (REG_GET(dsi, DSI_VC_CTRL(vc), 5, 5) == 0)
		complete(l4_data->completion);
}

static int dsi_sync_vc_l4(struct dsi_data *dsi, int vc)
{
	DECLARE_COMPLETION_ONSTACK(completion);
	struct dsi_packet_sent_handler_data l4_data = {
		.dsi = dsi,
		.completion = &completion
	};
	int r = 0;

	r = dsi_register_isr_vc(dsi, vc, dsi_packet_sent_handler_l4,
		&l4_data, DSI_VC_IRQ_PACKET_SENT);
	if (r)
		goto err0;

	/* Wait for completion only if TX_FIFO_NOT_EMPTY is still set */
	if (REG_GET(dsi, DSI_VC_CTRL(vc), 5, 5)) {
		if (wait_for_completion_timeout(&completion,
				msecs_to_jiffies(10)) == 0) {
			DSSERR("Failed to complete previous l4 transfer\n");
			r = -EIO;
			goto err1;
		}
	}

	dsi_unregister_isr_vc(dsi, vc, dsi_packet_sent_handler_l4,
		&l4_data, DSI_VC_IRQ_PACKET_SENT);

	return 0;
err1:
	dsi_unregister_isr_vc(dsi, vc, dsi_packet_sent_handler_l4,
		&l4_data, DSI_VC_IRQ_PACKET_SENT);
err0:
	return r;
}

static int dsi_sync_vc(struct dsi_data *dsi, int vc)
{
	WARN_ON(!dsi_bus_is_locked(dsi));

	WARN_ON(in_interrupt());

	if (!dsi_vc_is_enabled(dsi, vc))
		return 0;

	switch (dsi->vc[vc].source) {
	case DSI_VC_SOURCE_VP:
		return dsi_sync_vc_vp(dsi, vc);
	case DSI_VC_SOURCE_L4:
		return dsi_sync_vc_l4(dsi, vc);
	default:
		BUG();
		return -EINVAL;
	}
}

static int dsi_vc_enable(struct dsi_data *dsi, int vc, bool enable)
{
	DSSDBG("dsi_vc_enable vc %d, enable %d\n",
			vc, enable);

	enable = enable ? 1 : 0;

	REG_FLD_MOD(dsi, DSI_VC_CTRL(vc), enable, 0, 0);

	if (!wait_for_bit_change(dsi, DSI_VC_CTRL(vc), 0, enable)) {
		DSSERR("Failed to set dsi_vc_enable to %d\n", enable);
		return -EIO;
	}

	return 0;
}

static void dsi_vc_initial_config(struct dsi_data *dsi, int vc)
{
	u32 r;

	DSSDBG("Initial config of VC %d", vc);

	r = dsi_read_reg(dsi, DSI_VC_CTRL(vc));

	if (FLD_GET(r, 15, 15)) /* VC_BUSY */
		DSSERR("VC(%d) busy when trying to configure it!\n",
				vc);

	r = FLD_MOD(r, 0, 1, 1); /* SOURCE, 0 = L4 */
	r = FLD_MOD(r, 0, 2, 2); /* BTA_SHORT_EN  */
	r = FLD_MOD(r, 0, 3, 3); /* BTA_LONG_EN */
	r = FLD_MOD(r, 0, 4, 4); /* MODE, 0 = command */
	r = FLD_MOD(r, 1, 7, 7); /* CS_TX_EN */
	r = FLD_MOD(r, 1, 8, 8); /* ECC_TX_EN */
	r = FLD_MOD(r, 0, 9, 9); /* MODE_SPEED, high speed on/off */
	if (dsi->data->quirks & DSI_QUIRK_VC_OCP_WIDTH)
		r = FLD_MOD(r, 3, 11, 10);	/* OCP_WIDTH = 32 bit */

	r = FLD_MOD(r, 4, 29, 27); /* DMA_RX_REQ_NB = no dma */
	r = FLD_MOD(r, 4, 23, 21); /* DMA_TX_REQ_NB = no dma */

	dsi_write_reg(dsi, DSI_VC_CTRL(vc), r);

	dsi->vc[vc].source = DSI_VC_SOURCE_L4;
}

static void dsi_vc_enable_hs(struct omap_dss_device *dssdev, int vc,
		bool enable)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);

	DSSDBG("dsi_vc_enable_hs(%d, %d)\n", vc, enable);

	if (REG_GET(dsi, DSI_VC_CTRL(vc), 9, 9) == enable)
		return;

	WARN_ON(!dsi_bus_is_locked(dsi));

	dsi_vc_enable(dsi, vc, 0);
	dsi_if_enable(dsi, 0);

	REG_FLD_MOD(dsi, DSI_VC_CTRL(vc), enable, 9, 9);

	dsi_vc_enable(dsi, vc, 1);
	dsi_if_enable(dsi, 1);

	dsi_force_tx_stop_mode_io(dsi);
}

static void dsi_vc_flush_long_data(struct dsi_data *dsi, int vc)
{
	while (REG_GET(dsi, DSI_VC_CTRL(vc), 20, 20)) {
		u32 val;
		val = dsi_read_reg(dsi, DSI_VC_SHORT_PACKET_HEADER(vc));
		DSSDBG("\t\tb1 %#02x b2 %#02x b3 %#02x b4 %#02x\n",
				(val >> 0) & 0xff,
				(val >> 8) & 0xff,
				(val >> 16) & 0xff,
				(val >> 24) & 0xff);
	}
}

static void dsi_show_rx_ack_with_err(u16 err)
{
	DSSERR("\tACK with ERROR (%#x):\n", err);
	if (err & (1 << 0))
		DSSERR("\t\tSoT Error\n");
	if (err & (1 << 1))
		DSSERR("\t\tSoT Sync Error\n");
	if (err & (1 << 2))
		DSSERR("\t\tEoT Sync Error\n");
	if (err & (1 << 3))
		DSSERR("\t\tEscape Mode Entry Command Error\n");
	if (err & (1 << 4))
		DSSERR("\t\tLP Transmit Sync Error\n");
	if (err & (1 << 5))
		DSSERR("\t\tHS Receive Timeout Error\n");
	if (err & (1 << 6))
		DSSERR("\t\tFalse Control Error\n");
	if (err & (1 << 7))
		DSSERR("\t\t(reserved7)\n");
	if (err & (1 << 8))
		DSSERR("\t\tECC Error, single-bit (corrected)\n");
	if (err & (1 << 9))
		DSSERR("\t\tECC Error, multi-bit (not corrected)\n");
	if (err & (1 << 10))
		DSSERR("\t\tChecksum Error\n");
	if (err & (1 << 11))
		DSSERR("\t\tData type not recognized\n");
	if (err & (1 << 12))
		DSSERR("\t\tInvalid VC ID\n");
	if (err & (1 << 13))
		DSSERR("\t\tInvalid Transmission Length\n");
	if (err & (1 << 14))
		DSSERR("\t\t(reserved14)\n");
	if (err & (1 << 15))
		DSSERR("\t\tDSI Protocol Violation\n");
}

static u16 dsi_vc_flush_receive_data(struct dsi_data *dsi, int vc)
{
	/* RX_FIFO_NOT_EMPTY */
	while (REG_GET(dsi, DSI_VC_CTRL(vc), 20, 20)) {
		u32 val;
		u8 dt;
		val = dsi_read_reg(dsi, DSI_VC_SHORT_PACKET_HEADER(vc));
		DSSERR("\trawval %#08x\n", val);
		dt = FLD_GET(val, 5, 0);
		if (dt == MIPI_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT) {
			u16 err = FLD_GET(val, 23, 8);
			dsi_show_rx_ack_with_err(err);
		} else if (dt == MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE) {
			DSSERR("\tDCS short response, 1 byte: %#x\n",
					FLD_GET(val, 23, 8));
		} else if (dt == MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE) {
			DSSERR("\tDCS short response, 2 byte: %#x\n",
					FLD_GET(val, 23, 8));
		} else if (dt == MIPI_DSI_RX_DCS_LONG_READ_RESPONSE) {
			DSSERR("\tDCS long response, len %d\n",
					FLD_GET(val, 23, 8));
			dsi_vc_flush_long_data(dsi, vc);
		} else {
			DSSERR("\tunknown datatype 0x%02x\n", dt);
		}
	}
	return 0;
}

static int dsi_vc_send_bta(struct dsi_data *dsi, int vc)
{
	if (dsi->debug_write || dsi->debug_read)
		DSSDBG("dsi_vc_send_bta %d\n", vc);

	WARN_ON(!dsi_bus_is_locked(dsi));

	/* RX_FIFO_NOT_EMPTY */
	if (REG_GET(dsi, DSI_VC_CTRL(vc), 20, 20)) {
		DSSERR("rx fifo not empty when sending BTA, dumping data:\n");
		dsi_vc_flush_receive_data(dsi, vc);
	}

	REG_FLD_MOD(dsi, DSI_VC_CTRL(vc), 1, 6, 6); /* BTA_EN */

	/* flush posted write */
	dsi_read_reg(dsi, DSI_VC_CTRL(vc));

	return 0;
}

static int dsi_vc_send_bta_sync(struct omap_dss_device *dssdev, int vc)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);
	DECLARE_COMPLETION_ONSTACK(completion);
	int r = 0;
	u32 err;

	r = dsi_register_isr_vc(dsi, vc, dsi_completion_handler,
			&completion, DSI_VC_IRQ_BTA);
	if (r)
		goto err0;

	r = dsi_register_isr(dsi, dsi_completion_handler, &completion,
			DSI_IRQ_ERROR_MASK);
	if (r)
		goto err1;

	r = dsi_vc_send_bta(dsi, vc);
	if (r)
		goto err2;

	if (wait_for_completion_timeout(&completion,
				msecs_to_jiffies(500)) == 0) {
		DSSERR("Failed to receive BTA\n");
		r = -EIO;
		goto err2;
	}

	err = dsi_get_errors(dsi);
	if (err) {
		DSSERR("Error while sending BTA: %x\n", err);
		r = -EIO;
		goto err2;
	}
err2:
	dsi_unregister_isr(dsi, dsi_completion_handler, &completion,
			DSI_IRQ_ERROR_MASK);
err1:
	dsi_unregister_isr_vc(dsi, vc, dsi_completion_handler,
			&completion, DSI_VC_IRQ_BTA);
err0:
	return r;
}

static inline void dsi_vc_write_long_header(struct dsi_data *dsi, int vc,
					    int channel, u8 data_type, u16 len,
					    u8 ecc)
{
	u32 val;
	u8 data_id;

	WARN_ON(!dsi_bus_is_locked(dsi));

	data_id = data_type | channel << 6;

	val = FLD_VAL(data_id, 7, 0) | FLD_VAL(len, 23, 8) |
		FLD_VAL(ecc, 31, 24);

	dsi_write_reg(dsi, DSI_VC_LONG_PACKET_HEADER(vc), val);
}

static inline void dsi_vc_write_long_payload(struct dsi_data *dsi, int vc,
					     u8 b1, u8 b2, u8 b3, u8 b4)
{
	u32 val;

	val = b4 << 24 | b3 << 16 | b2 << 8  | b1 << 0;

/*	DSSDBG("\twriting %02x, %02x, %02x, %02x (%#010x)\n",
			b1, b2, b3, b4, val); */

	dsi_write_reg(dsi, DSI_VC_LONG_PACKET_PAYLOAD(vc), val);
}

static int dsi_vc_send_long(struct dsi_data *dsi, int vc,
			    const struct mipi_dsi_msg *msg)
{
	/*u32 val; */
	int i;
	const u8 *p;
	int r = 0;
	u8 b1, b2, b3, b4;

	if (dsi->debug_write)
		DSSDBG("dsi_vc_send_long, %d bytes\n", msg->tx_len);

	/* len + header */
	if (dsi->vc[vc].tx_fifo_size * 32 * 4 < msg->tx_len + 4) {
		DSSERR("unable to send long packet: packet too long.\n");
		return -EINVAL;
	}

	dsi_vc_write_long_header(dsi, vc, msg->channel, msg->type, msg->tx_len, 0);

	p = msg->tx_buf;
	for (i = 0; i < msg->tx_len >> 2; i++) {
		if (dsi->debug_write)
			DSSDBG("\tsending full packet %d\n", i);

		b1 = *p++;
		b2 = *p++;
		b3 = *p++;
		b4 = *p++;

		dsi_vc_write_long_payload(dsi, vc, b1, b2, b3, b4);
	}

	i = msg->tx_len % 4;
	if (i) {
		b1 = 0; b2 = 0; b3 = 0;

		if (dsi->debug_write)
			DSSDBG("\tsending remainder bytes %d\n", i);

		switch (i) {
		case 3:
			b1 = *p++;
			b2 = *p++;
			b3 = *p++;
			break;
		case 2:
			b1 = *p++;
			b2 = *p++;
			break;
		case 1:
			b1 = *p++;
			break;
		}

		dsi_vc_write_long_payload(dsi, vc, b1, b2, b3, 0);
	}

	return r;
}

static int dsi_vc_send_short(struct dsi_data *dsi, int vc,
			     const struct mipi_dsi_msg *msg)
{
	struct mipi_dsi_packet pkt;
	int ret;
	u32 r;

	ret = mipi_dsi_create_packet(&pkt, msg);
	if (ret < 0)
		return ret;

	WARN_ON(!dsi_bus_is_locked(dsi));

	if (dsi->debug_write)
		DSSDBG("dsi_vc_send_short(vc%d, dt %#x, b1 %#x, b2 %#x)\n",
		       vc, msg->type, pkt.header[1], pkt.header[2]);

	if (FLD_GET(dsi_read_reg(dsi, DSI_VC_CTRL(vc)), 16, 16)) {
		DSSERR("ERROR FIFO FULL, aborting transfer\n");
		return -EINVAL;
	}

	r = pkt.header[3] << 24 | pkt.header[2] << 16 | pkt.header[1] << 8 |
	    pkt.header[0];

	dsi_write_reg(dsi, DSI_VC_SHORT_PACKET_HEADER(vc), r);

	return 0;
}

static int dsi_vc_send_null(struct dsi_data *dsi, int vc, int channel)
{
	const struct mipi_dsi_msg msg = {
		.channel = channel,
		.type = MIPI_DSI_NULL_PACKET,
	};

	return dsi_vc_send_long(dsi, vc, &msg);
}

static int dsi_vc_write_common(struct omap_dss_device *dssdev, int vc,
			       const struct mipi_dsi_msg *msg)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);
	int r;

	if (mipi_dsi_packet_format_is_short(msg->type))
		r = dsi_vc_send_short(dsi, vc, msg);
	else
		r = dsi_vc_send_long(dsi, vc, msg);

	if (r < 0)
		return r;

	/*
	 * TODO: we do not always have to do the BTA sync, for example
	 * we can improve performance by setting the update window
	 * information without sending BTA sync between the commands.
	 * In that case we can return early.
	 */

	r = dsi_vc_send_bta_sync(dssdev, vc);
	if (r) {
		DSSERR("bta sync failed\n");
		return r;
	}

	/* RX_FIFO_NOT_EMPTY */
	if (REG_GET(dsi, DSI_VC_CTRL(vc), 20, 20)) {
		DSSERR("rx fifo not empty after write, dumping data:\n");
		dsi_vc_flush_receive_data(dsi, vc);
		return -EIO;
	}

	return 0;
}

static int dsi_vc_read_rx_fifo(struct dsi_data *dsi, int vc, u8 *buf,
			       int buflen, enum dss_dsi_content_type type)
{
	u32 val;
	u8 dt;
	int r;

	/* RX_FIFO_NOT_EMPTY */
	if (REG_GET(dsi, DSI_VC_CTRL(vc), 20, 20) == 0) {
		DSSERR("RX fifo empty when trying to read.\n");
		r = -EIO;
		goto err;
	}

	val = dsi_read_reg(dsi, DSI_VC_SHORT_PACKET_HEADER(vc));
	if (dsi->debug_read)
		DSSDBG("\theader: %08x\n", val);
	dt = FLD_GET(val, 5, 0);
	if (dt == MIPI_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT) {
		u16 err = FLD_GET(val, 23, 8);
		dsi_show_rx_ack_with_err(err);
		r = -EIO;
		goto err;

	} else if (dt == (type == DSS_DSI_CONTENT_GENERIC ?
			MIPI_DSI_RX_GENERIC_SHORT_READ_RESPONSE_1BYTE :
			MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE)) {
		u8 data = FLD_GET(val, 15, 8);
		if (dsi->debug_read)
			DSSDBG("\t%s short response, 1 byte: %02x\n",
				type == DSS_DSI_CONTENT_GENERIC ? "GENERIC" :
				"DCS", data);

		if (buflen < 1) {
			r = -EIO;
			goto err;
		}

		buf[0] = data;

		return 1;
	} else if (dt == (type == DSS_DSI_CONTENT_GENERIC ?
			MIPI_DSI_RX_GENERIC_SHORT_READ_RESPONSE_2BYTE :
			MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE)) {
		u16 data = FLD_GET(val, 23, 8);
		if (dsi->debug_read)
			DSSDBG("\t%s short response, 2 byte: %04x\n",
				type == DSS_DSI_CONTENT_GENERIC ? "GENERIC" :
				"DCS", data);

		if (buflen < 2) {
			r = -EIO;
			goto err;
		}

		buf[0] = data & 0xff;
		buf[1] = (data >> 8) & 0xff;

		return 2;
	} else if (dt == (type == DSS_DSI_CONTENT_GENERIC ?
			MIPI_DSI_RX_GENERIC_LONG_READ_RESPONSE :
			MIPI_DSI_RX_DCS_LONG_READ_RESPONSE)) {
		int w;
		int len = FLD_GET(val, 23, 8);
		if (dsi->debug_read)
			DSSDBG("\t%s long response, len %d\n",
				type == DSS_DSI_CONTENT_GENERIC ? "GENERIC" :
				"DCS", len);

		if (len > buflen) {
			r = -EIO;
			goto err;
		}

		/* two byte checksum ends the packet, not included in len */
		for (w = 0; w < len + 2;) {
			int b;
			val = dsi_read_reg(dsi,
				DSI_VC_SHORT_PACKET_HEADER(vc));
			if (dsi->debug_read)
				DSSDBG("\t\t%02x %02x %02x %02x\n",
						(val >> 0) & 0xff,
						(val >> 8) & 0xff,
						(val >> 16) & 0xff,
						(val >> 24) & 0xff);

			for (b = 0; b < 4; ++b) {
				if (w < len)
					buf[w] = (val >> (b * 8)) & 0xff;
				/* we discard the 2 byte checksum */
				++w;
			}
		}

		return len;
	} else {
		DSSERR("\tunknown datatype 0x%02x\n", dt);
		r = -EIO;
		goto err;
	}

err:
	DSSERR("dsi_vc_read_rx_fifo(vc %d type %s) failed\n", vc,
		type == DSS_DSI_CONTENT_GENERIC ? "GENERIC" : "DCS");

	return r;
}

static int dsi_vc_dcs_read(struct omap_dss_device *dssdev, int vc,
			   const struct mipi_dsi_msg *msg)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);
	u8 cmd = ((u8 *)msg->tx_buf)[0];
	int r;

	if (dsi->debug_read)
		DSSDBG("%s(vc %d, cmd %x)\n", __func__, vc, cmd);

	r = dsi_vc_send_short(dsi, vc, msg);
	if (r)
		goto err;

	r = dsi_vc_send_bta_sync(dssdev, vc);
	if (r)
		goto err;

	r = dsi_vc_read_rx_fifo(dsi, vc, msg->rx_buf, msg->rx_len,
		DSS_DSI_CONTENT_DCS);
	if (r < 0)
		goto err;

	if (r != msg->rx_len) {
		r = -EIO;
		goto err;
	}

	return 0;
err:
	DSSERR("%s(vc %d, cmd 0x%02x) failed\n", __func__,  vc, cmd);
	return r;
}

static int dsi_vc_generic_read(struct omap_dss_device *dssdev, int vc,
			       const struct mipi_dsi_msg *msg)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);
	int r;

	r = dsi_vc_send_short(dsi, vc, msg);
	if (r)
		goto err;

	r = dsi_vc_send_bta_sync(dssdev, vc);
	if (r)
		goto err;

	r = dsi_vc_read_rx_fifo(dsi, vc, msg->rx_buf, msg->rx_len,
		DSS_DSI_CONTENT_GENERIC);
	if (r < 0)
		goto err;

	if (r != msg->rx_len) {
		r = -EIO;
		goto err;
	}

	return 0;
err:
	DSSERR("%s(vc %d, reqlen %d) failed\n", __func__,  vc, msg->tx_len);
	return r;
}

static void dsi_set_lp_rx_timeout(struct dsi_data *dsi, unsigned int ticks,
				  bool x4, bool x16)
{
	unsigned long fck;
	unsigned long total_ticks;
	u32 r;

	BUG_ON(ticks > 0x1fff);

	/* ticks in DSI_FCK */
	fck = dsi_fclk_rate(dsi);

	r = dsi_read_reg(dsi, DSI_TIMING2);
	r = FLD_MOD(r, 1, 15, 15);	/* LP_RX_TO */
	r = FLD_MOD(r, x16 ? 1 : 0, 14, 14);	/* LP_RX_TO_X16 */
	r = FLD_MOD(r, x4 ? 1 : 0, 13, 13);	/* LP_RX_TO_X4 */
	r = FLD_MOD(r, ticks, 12, 0);	/* LP_RX_COUNTER */
	dsi_write_reg(dsi, DSI_TIMING2, r);

	total_ticks = ticks * (x16 ? 16 : 1) * (x4 ? 4 : 1);

	DSSDBG("LP_RX_TO %lu ticks (%#x%s%s) = %lu ns\n",
			total_ticks,
			ticks, x4 ? " x4" : "", x16 ? " x16" : "",
			(total_ticks * 1000) / (fck / 1000 / 1000));
}

static void dsi_set_ta_timeout(struct dsi_data *dsi, unsigned int ticks,
			       bool x8, bool x16)
{
	unsigned long fck;
	unsigned long total_ticks;
	u32 r;

	BUG_ON(ticks > 0x1fff);

	/* ticks in DSI_FCK */
	fck = dsi_fclk_rate(dsi);

	r = dsi_read_reg(dsi, DSI_TIMING1);
	r = FLD_MOD(r, 1, 31, 31);	/* TA_TO */
	r = FLD_MOD(r, x16 ? 1 : 0, 30, 30);	/* TA_TO_X16 */
	r = FLD_MOD(r, x8 ? 1 : 0, 29, 29);	/* TA_TO_X8 */
	r = FLD_MOD(r, ticks, 28, 16);	/* TA_TO_COUNTER */
	dsi_write_reg(dsi, DSI_TIMING1, r);

	total_ticks = ticks * (x16 ? 16 : 1) * (x8 ? 8 : 1);

	DSSDBG("TA_TO %lu ticks (%#x%s%s) = %lu ns\n",
			total_ticks,
			ticks, x8 ? " x8" : "", x16 ? " x16" : "",
			(total_ticks * 1000) / (fck / 1000 / 1000));
}

static void dsi_set_stop_state_counter(struct dsi_data *dsi, unsigned int ticks,
				       bool x4, bool x16)
{
	unsigned long fck;
	unsigned long total_ticks;
	u32 r;

	BUG_ON(ticks > 0x1fff);

	/* ticks in DSI_FCK */
	fck = dsi_fclk_rate(dsi);

	r = dsi_read_reg(dsi, DSI_TIMING1);
	r = FLD_MOD(r, 1, 15, 15);	/* FORCE_TX_STOP_MODE_IO */
	r = FLD_MOD(r, x16 ? 1 : 0, 14, 14);	/* STOP_STATE_X16_IO */
	r = FLD_MOD(r, x4 ? 1 : 0, 13, 13);	/* STOP_STATE_X4_IO */
	r = FLD_MOD(r, ticks, 12, 0);	/* STOP_STATE_COUNTER_IO */
	dsi_write_reg(dsi, DSI_TIMING1, r);

	total_ticks = ticks * (x16 ? 16 : 1) * (x4 ? 4 : 1);

	DSSDBG("STOP_STATE_COUNTER %lu ticks (%#x%s%s) = %lu ns\n",
			total_ticks,
			ticks, x4 ? " x4" : "", x16 ? " x16" : "",
			(total_ticks * 1000) / (fck / 1000 / 1000));
}

static void dsi_set_hs_tx_timeout(struct dsi_data *dsi, unsigned int ticks,
				  bool x4, bool x16)
{
	unsigned long fck;
	unsigned long total_ticks;
	u32 r;

	BUG_ON(ticks > 0x1fff);

	/* ticks in TxByteClkHS */
	fck = dsi_get_txbyteclkhs(dsi);

	r = dsi_read_reg(dsi, DSI_TIMING2);
	r = FLD_MOD(r, 1, 31, 31);	/* HS_TX_TO */
	r = FLD_MOD(r, x16 ? 1 : 0, 30, 30);	/* HS_TX_TO_X16 */
	r = FLD_MOD(r, x4 ? 1 : 0, 29, 29);	/* HS_TX_TO_X8 (4 really) */
	r = FLD_MOD(r, ticks, 28, 16);	/* HS_TX_TO_COUNTER */
	dsi_write_reg(dsi, DSI_TIMING2, r);

	total_ticks = ticks * (x16 ? 16 : 1) * (x4 ? 4 : 1);

	DSSDBG("HS_TX_TO %lu ticks (%#x%s%s) = %lu ns\n",
			total_ticks,
			ticks, x4 ? " x4" : "", x16 ? " x16" : "",
			(total_ticks * 1000) / (fck / 1000 / 1000));
}

static void dsi_config_vp_num_line_buffers(struct dsi_data *dsi)
{
	int num_line_buffers;

	if (dsi->mode == OMAP_DSS_DSI_VIDEO_MODE) {
		int bpp = mipi_dsi_pixel_format_to_bpp(dsi->pix_fmt);
		const struct videomode *vm = &dsi->vm;
		/*
		 * Don't use line buffers if width is greater than the video
		 * port's line buffer size
		 */
		if (dsi->line_buffer_size <= vm->hactive * bpp / 8)
			num_line_buffers = 0;
		else
			num_line_buffers = 2;
	} else {
		/* Use maximum number of line buffers in command mode */
		num_line_buffers = 2;
	}

	/* LINE_BUFFER */
	REG_FLD_MOD(dsi, DSI_CTRL, num_line_buffers, 13, 12);
}

static void dsi_config_vp_sync_events(struct dsi_data *dsi)
{
	bool sync_end;
	u32 r;

	if (dsi->vm_timings.trans_mode == OMAP_DSS_DSI_PULSE_MODE)
		sync_end = true;
	else
		sync_end = false;

	r = dsi_read_reg(dsi, DSI_CTRL);
	r = FLD_MOD(r, 1, 9, 9);		/* VP_DE_POL */
	r = FLD_MOD(r, 1, 10, 10);		/* VP_HSYNC_POL */
	r = FLD_MOD(r, 1, 11, 11);		/* VP_VSYNC_POL */
	r = FLD_MOD(r, 1, 15, 15);		/* VP_VSYNC_START */
	r = FLD_MOD(r, sync_end, 16, 16);	/* VP_VSYNC_END */
	r = FLD_MOD(r, 1, 17, 17);		/* VP_HSYNC_START */
	r = FLD_MOD(r, sync_end, 18, 18);	/* VP_HSYNC_END */
	dsi_write_reg(dsi, DSI_CTRL, r);
}

static void dsi_config_blanking_modes(struct dsi_data *dsi)
{
	int blanking_mode = dsi->vm_timings.blanking_mode;
	int hfp_blanking_mode = dsi->vm_timings.hfp_blanking_mode;
	int hbp_blanking_mode = dsi->vm_timings.hbp_blanking_mode;
	int hsa_blanking_mode = dsi->vm_timings.hsa_blanking_mode;
	u32 r;

	/*
	 * 0 = TX FIFO packets sent or LPS in corresponding blanking periods
	 * 1 = Long blanking packets are sent in corresponding blanking periods
	 */
	r = dsi_read_reg(dsi, DSI_CTRL);
	r = FLD_MOD(r, blanking_mode, 20, 20);		/* BLANKING_MODE */
	r = FLD_MOD(r, hfp_blanking_mode, 21, 21);	/* HFP_BLANKING */
	r = FLD_MOD(r, hbp_blanking_mode, 22, 22);	/* HBP_BLANKING */
	r = FLD_MOD(r, hsa_blanking_mode, 23, 23);	/* HSA_BLANKING */
	dsi_write_reg(dsi, DSI_CTRL, r);
}

/*
 * According to section 'HS Command Mode Interleaving' in OMAP TRM, Scenario 3
 * results in maximum transition time for data and clock lanes to enter and
 * exit HS mode. Hence, this is the scenario where the least amount of command
 * mode data can be interleaved. We program the minimum amount of TXBYTECLKHS
 * clock cycles that can be used to interleave command mode data in HS so that
 * all scenarios are satisfied.
 */
static int dsi_compute_interleave_hs(int blank, bool ddr_alwon, int enter_hs,
		int exit_hs, int exiths_clk, int ddr_pre, int ddr_post)
{
	int transition;

	/*
	 * If DDR_CLK_ALWAYS_ON is set, we need to consider HS mode transition
	 * time of data lanes only, if it isn't set, we need to consider HS
	 * transition time of both data and clock lanes. HS transition time
	 * of Scenario 3 is considered.
	 */
	if (ddr_alwon) {
		transition = enter_hs + exit_hs + max(enter_hs, 2) + 1;
	} else {
		int trans1, trans2;
		trans1 = ddr_pre + enter_hs + exit_hs + max(enter_hs, 2) + 1;
		trans2 = ddr_pre + enter_hs + exiths_clk + ddr_post + ddr_pre +
				enter_hs + 1;
		transition = max(trans1, trans2);
	}

	return blank > transition ? blank - transition : 0;
}

/*
 * According to section 'LP Command Mode Interleaving' in OMAP TRM, Scenario 1
 * results in maximum transition time for data lanes to enter and exit LP mode.
 * Hence, this is the scenario where the least amount of command mode data can
 * be interleaved. We program the minimum amount of bytes that can be
 * interleaved in LP so that all scenarios are satisfied.
 */
static int dsi_compute_interleave_lp(int blank, int enter_hs, int exit_hs,
		int lp_clk_div, int tdsi_fclk)
{
	int trans_lp;	/* time required for a LP transition, in TXBYTECLKHS */
	int tlp_avail;	/* time left for interleaving commands, in CLKIN4DDR */
	int ttxclkesc;	/* period of LP transmit escape clock, in CLKIN4DDR */
	int thsbyte_clk = 16;	/* Period of TXBYTECLKHS clock, in CLKIN4DDR */
	int lp_inter;	/* cmd mode data that can be interleaved, in bytes */

	/* maximum LP transition time according to Scenario 1 */
	trans_lp = exit_hs + max(enter_hs, 2) + 1;

	/* CLKIN4DDR = 16 * TXBYTECLKHS */
	tlp_avail = thsbyte_clk * (blank - trans_lp);

	ttxclkesc = tdsi_fclk * lp_clk_div;

	lp_inter = ((tlp_avail - 8 * thsbyte_clk - 5 * tdsi_fclk) / ttxclkesc -
			26) / 16;

	return max(lp_inter, 0);
}

static void dsi_config_cmd_mode_interleaving(struct dsi_data *dsi)
{
	int blanking_mode;
	int hfp_blanking_mode, hbp_blanking_mode, hsa_blanking_mode;
	int hsa, hfp, hbp, width_bytes, bllp, lp_clk_div;
	int ddr_clk_pre, ddr_clk_post, enter_hs_mode_lat, exit_hs_mode_lat;
	int tclk_trail, ths_exit, exiths_clk;
	bool ddr_alwon;
	const struct videomode *vm = &dsi->vm;
	int bpp = mipi_dsi_pixel_format_to_bpp(dsi->pix_fmt);
	int ndl = dsi->num_lanes_used - 1;
	int dsi_fclk_hsdiv = dsi->user_dsi_cinfo.mX[HSDIV_DSI] + 1;
	int hsa_interleave_hs = 0, hsa_interleave_lp = 0;
	int hfp_interleave_hs = 0, hfp_interleave_lp = 0;
	int hbp_interleave_hs = 0, hbp_interleave_lp = 0;
	int bl_interleave_hs = 0, bl_interleave_lp = 0;
	u32 r;

	r = dsi_read_reg(dsi, DSI_CTRL);
	blanking_mode = FLD_GET(r, 20, 20);
	hfp_blanking_mode = FLD_GET(r, 21, 21);
	hbp_blanking_mode = FLD_GET(r, 22, 22);
	hsa_blanking_mode = FLD_GET(r, 23, 23);

	r = dsi_read_reg(dsi, DSI_VM_TIMING1);
	hbp = FLD_GET(r, 11, 0);
	hfp = FLD_GET(r, 23, 12);
	hsa = FLD_GET(r, 31, 24);

	r = dsi_read_reg(dsi, DSI_CLK_TIMING);
	ddr_clk_post = FLD_GET(r, 7, 0);
	ddr_clk_pre = FLD_GET(r, 15, 8);

	r = dsi_read_reg(dsi, DSI_VM_TIMING7);
	exit_hs_mode_lat = FLD_GET(r, 15, 0);
	enter_hs_mode_lat = FLD_GET(r, 31, 16);

	r = dsi_read_reg(dsi, DSI_CLK_CTRL);
	lp_clk_div = FLD_GET(r, 12, 0);
	ddr_alwon = FLD_GET(r, 13, 13);

	r = dsi_read_reg(dsi, DSI_DSIPHY_CFG0);
	ths_exit = FLD_GET(r, 7, 0);

	r = dsi_read_reg(dsi, DSI_DSIPHY_CFG1);
	tclk_trail = FLD_GET(r, 15, 8);

	exiths_clk = ths_exit + tclk_trail;

	width_bytes = DIV_ROUND_UP(vm->hactive * bpp, 8);
	bllp = hbp + hfp + hsa + DIV_ROUND_UP(width_bytes + 6, ndl);

	if (!hsa_blanking_mode) {
		hsa_interleave_hs = dsi_compute_interleave_hs(hsa, ddr_alwon,
					enter_hs_mode_lat, exit_hs_mode_lat,
					exiths_clk, ddr_clk_pre, ddr_clk_post);
		hsa_interleave_lp = dsi_compute_interleave_lp(hsa,
					enter_hs_mode_lat, exit_hs_mode_lat,
					lp_clk_div, dsi_fclk_hsdiv);
	}

	if (!hfp_blanking_mode) {
		hfp_interleave_hs = dsi_compute_interleave_hs(hfp, ddr_alwon,
					enter_hs_mode_lat, exit_hs_mode_lat,
					exiths_clk, ddr_clk_pre, ddr_clk_post);
		hfp_interleave_lp = dsi_compute_interleave_lp(hfp,
					enter_hs_mode_lat, exit_hs_mode_lat,
					lp_clk_div, dsi_fclk_hsdiv);
	}

	if (!hbp_blanking_mode) {
		hbp_interleave_hs = dsi_compute_interleave_hs(hbp, ddr_alwon,
					enter_hs_mode_lat, exit_hs_mode_lat,
					exiths_clk, ddr_clk_pre, ddr_clk_post);

		hbp_interleave_lp = dsi_compute_interleave_lp(hbp,
					enter_hs_mode_lat, exit_hs_mode_lat,
					lp_clk_div, dsi_fclk_hsdiv);
	}

	if (!blanking_mode) {
		bl_interleave_hs = dsi_compute_interleave_hs(bllp, ddr_alwon,
					enter_hs_mode_lat, exit_hs_mode_lat,
					exiths_clk, ddr_clk_pre, ddr_clk_post);

		bl_interleave_lp = dsi_compute_interleave_lp(bllp,
					enter_hs_mode_lat, exit_hs_mode_lat,
					lp_clk_div, dsi_fclk_hsdiv);
	}

	DSSDBG("DSI HS interleaving(TXBYTECLKHS) HSA %d, HFP %d, HBP %d, BLLP %d\n",
		hsa_interleave_hs, hfp_interleave_hs, hbp_interleave_hs,
		bl_interleave_hs);

	DSSDBG("DSI LP interleaving(bytes) HSA %d, HFP %d, HBP %d, BLLP %d\n",
		hsa_interleave_lp, hfp_interleave_lp, hbp_interleave_lp,
		bl_interleave_lp);

	r = dsi_read_reg(dsi, DSI_VM_TIMING4);
	r = FLD_MOD(r, hsa_interleave_hs, 23, 16);
	r = FLD_MOD(r, hfp_interleave_hs, 15, 8);
	r = FLD_MOD(r, hbp_interleave_hs, 7, 0);
	dsi_write_reg(dsi, DSI_VM_TIMING4, r);

	r = dsi_read_reg(dsi, DSI_VM_TIMING5);
	r = FLD_MOD(r, hsa_interleave_lp, 23, 16);
	r = FLD_MOD(r, hfp_interleave_lp, 15, 8);
	r = FLD_MOD(r, hbp_interleave_lp, 7, 0);
	dsi_write_reg(dsi, DSI_VM_TIMING5, r);

	r = dsi_read_reg(dsi, DSI_VM_TIMING6);
	r = FLD_MOD(r, bl_interleave_hs, 31, 15);
	r = FLD_MOD(r, bl_interleave_lp, 16, 0);
	dsi_write_reg(dsi, DSI_VM_TIMING6, r);
}

static int dsi_proto_config(struct dsi_data *dsi)
{
	u32 r;
	int buswidth = 0;

	dsi_config_tx_fifo(dsi, DSI_FIFO_SIZE_32,
			DSI_FIFO_SIZE_32,
			DSI_FIFO_SIZE_32,
			DSI_FIFO_SIZE_32);

	dsi_config_rx_fifo(dsi, DSI_FIFO_SIZE_32,
			DSI_FIFO_SIZE_32,
			DSI_FIFO_SIZE_32,
			DSI_FIFO_SIZE_32);

	/* XXX what values for the timeouts? */
	dsi_set_stop_state_counter(dsi, 0x1000, false, false);
	dsi_set_ta_timeout(dsi, 0x1fff, true, true);
	dsi_set_lp_rx_timeout(dsi, 0x1fff, true, true);
	dsi_set_hs_tx_timeout(dsi, 0x1fff, true, true);

	switch (mipi_dsi_pixel_format_to_bpp(dsi->pix_fmt)) {
	case 16:
		buswidth = 0;
		break;
	case 18:
		buswidth = 1;
		break;
	case 24:
		buswidth = 2;
		break;
	default:
		BUG();
		return -EINVAL;
	}

	r = dsi_read_reg(dsi, DSI_CTRL);
	r = FLD_MOD(r, 1, 1, 1);	/* CS_RX_EN */
	r = FLD_MOD(r, 1, 2, 2);	/* ECC_RX_EN */
	r = FLD_MOD(r, 1, 3, 3);	/* TX_FIFO_ARBITRATION */
	r = FLD_MOD(r, 1, 4, 4);	/* VP_CLK_RATIO, always 1, see errata*/
	r = FLD_MOD(r, buswidth, 7, 6); /* VP_DATA_BUS_WIDTH */
	r = FLD_MOD(r, 0, 8, 8);	/* VP_CLK_POL */
	r = FLD_MOD(r, 1, 14, 14);	/* TRIGGER_RESET_MODE */
	r = FLD_MOD(r, 1, 19, 19);	/* EOT_ENABLE */
	if (!(dsi->data->quirks & DSI_QUIRK_DCS_CMD_CONFIG_VC)) {
		r = FLD_MOD(r, 1, 24, 24);	/* DCS_CMD_ENABLE */
		/* DCS_CMD_CODE, 1=start, 0=continue */
		r = FLD_MOD(r, 0, 25, 25);
	}

	dsi_write_reg(dsi, DSI_CTRL, r);

	dsi_config_vp_num_line_buffers(dsi);

	if (dsi->mode == OMAP_DSS_DSI_VIDEO_MODE) {
		dsi_config_vp_sync_events(dsi);
		dsi_config_blanking_modes(dsi);
		dsi_config_cmd_mode_interleaving(dsi);
	}

	dsi_vc_initial_config(dsi, 0);
	dsi_vc_initial_config(dsi, 1);
	dsi_vc_initial_config(dsi, 2);
	dsi_vc_initial_config(dsi, 3);

	return 0;
}

static void dsi_proto_timings(struct dsi_data *dsi)
{
	unsigned int tlpx, tclk_zero, tclk_prepare;
	unsigned int tclk_pre, tclk_post;
	unsigned int ths_prepare, ths_prepare_ths_zero, ths_zero;
	unsigned int ths_trail, ths_exit;
	unsigned int ddr_clk_pre, ddr_clk_post;
	unsigned int enter_hs_mode_lat, exit_hs_mode_lat;
	unsigned int ths_eot;
	int ndl = dsi->num_lanes_used - 1;
	u32 r;

	r = dsi_read_reg(dsi, DSI_DSIPHY_CFG0);
	ths_prepare = FLD_GET(r, 31, 24);
	ths_prepare_ths_zero = FLD_GET(r, 23, 16);
	ths_zero = ths_prepare_ths_zero - ths_prepare;
	ths_trail = FLD_GET(r, 15, 8);
	ths_exit = FLD_GET(r, 7, 0);

	r = dsi_read_reg(dsi, DSI_DSIPHY_CFG1);
	tlpx = FLD_GET(r, 20, 16) * 2;
	tclk_zero = FLD_GET(r, 7, 0);

	r = dsi_read_reg(dsi, DSI_DSIPHY_CFG2);
	tclk_prepare = FLD_GET(r, 7, 0);

	/* min 8*UI */
	tclk_pre = 20;
	/* min 60ns + 52*UI */
	tclk_post = ns2ddr(dsi, 60) + 26;

	ths_eot = DIV_ROUND_UP(4, ndl);

	ddr_clk_pre = DIV_ROUND_UP(tclk_pre + tlpx + tclk_zero + tclk_prepare,
			4);
	ddr_clk_post = DIV_ROUND_UP(tclk_post + ths_trail, 4) + ths_eot;

	BUG_ON(ddr_clk_pre == 0 || ddr_clk_pre > 255);
	BUG_ON(ddr_clk_post == 0 || ddr_clk_post > 255);

	r = dsi_read_reg(dsi, DSI_CLK_TIMING);
	r = FLD_MOD(r, ddr_clk_pre, 15, 8);
	r = FLD_MOD(r, ddr_clk_post, 7, 0);
	dsi_write_reg(dsi, DSI_CLK_TIMING, r);

	DSSDBG("ddr_clk_pre %u, ddr_clk_post %u\n",
			ddr_clk_pre,
			ddr_clk_post);

	enter_hs_mode_lat = 1 + DIV_ROUND_UP(tlpx, 4) +
		DIV_ROUND_UP(ths_prepare, 4) +
		DIV_ROUND_UP(ths_zero + 3, 4);

	exit_hs_mode_lat = DIV_ROUND_UP(ths_trail + ths_exit, 4) + 1 + ths_eot;

	r = FLD_VAL(enter_hs_mode_lat, 31, 16) |
		FLD_VAL(exit_hs_mode_lat, 15, 0);
	dsi_write_reg(dsi, DSI_VM_TIMING7, r);

	DSSDBG("enter_hs_mode_lat %u, exit_hs_mode_lat %u\n",
			enter_hs_mode_lat, exit_hs_mode_lat);

	 if (dsi->mode == OMAP_DSS_DSI_VIDEO_MODE) {
		/* TODO: Implement a video mode check_timings function */
		int hsa = dsi->vm_timings.hsa;
		int hfp = dsi->vm_timings.hfp;
		int hbp = dsi->vm_timings.hbp;
		int vsa = dsi->vm_timings.vsa;
		int vfp = dsi->vm_timings.vfp;
		int vbp = dsi->vm_timings.vbp;
		int window_sync = dsi->vm_timings.window_sync;
		bool hsync_end;
		const struct videomode *vm = &dsi->vm;
		int bpp = mipi_dsi_pixel_format_to_bpp(dsi->pix_fmt);
		int tl, t_he, width_bytes;

		hsync_end = dsi->vm_timings.trans_mode == OMAP_DSS_DSI_PULSE_MODE;
		t_he = hsync_end ?
			((hsa == 0 && ndl == 3) ? 1 : DIV_ROUND_UP(4, ndl)) : 0;

		width_bytes = DIV_ROUND_UP(vm->hactive * bpp, 8);

		/* TL = t_HS + HSA + t_HE + HFP + ceil((WC + 6) / NDL) + HBP */
		tl = DIV_ROUND_UP(4, ndl) + (hsync_end ? hsa : 0) + t_he + hfp +
			DIV_ROUND_UP(width_bytes + 6, ndl) + hbp;

		DSSDBG("HBP: %d, HFP: %d, HSA: %d, TL: %d TXBYTECLKHS\n", hbp,
			hfp, hsync_end ? hsa : 0, tl);
		DSSDBG("VBP: %d, VFP: %d, VSA: %d, VACT: %d lines\n", vbp, vfp,
			vsa, vm->vactive);

		r = dsi_read_reg(dsi, DSI_VM_TIMING1);
		r = FLD_MOD(r, hbp, 11, 0);	/* HBP */
		r = FLD_MOD(r, hfp, 23, 12);	/* HFP */
		r = FLD_MOD(r, hsync_end ? hsa : 0, 31, 24);	/* HSA */
		dsi_write_reg(dsi, DSI_VM_TIMING1, r);

		r = dsi_read_reg(dsi, DSI_VM_TIMING2);
		r = FLD_MOD(r, vbp, 7, 0);	/* VBP */
		r = FLD_MOD(r, vfp, 15, 8);	/* VFP */
		r = FLD_MOD(r, vsa, 23, 16);	/* VSA */
		r = FLD_MOD(r, window_sync, 27, 24);	/* WINDOW_SYNC */
		dsi_write_reg(dsi, DSI_VM_TIMING2, r);

		r = dsi_read_reg(dsi, DSI_VM_TIMING3);
		r = FLD_MOD(r, vm->vactive, 14, 0);	/* VACT */
		r = FLD_MOD(r, tl, 31, 16);		/* TL */
		dsi_write_reg(dsi, DSI_VM_TIMING3, r);
	}
}

static int dsi_configure_pins(struct dsi_data *dsi,
		int num_pins, const u32 *pins)
{
	struct dsi_lane_config lanes[DSI_MAX_NR_LANES];
	int num_lanes;
	int i;

	static const enum dsi_lane_function functions[] = {
		DSI_LANE_CLK,
		DSI_LANE_DATA1,
		DSI_LANE_DATA2,
		DSI_LANE_DATA3,
		DSI_LANE_DATA4,
	};

	if (num_pins < 4 || num_pins > dsi->num_lanes_supported * 2
			|| num_pins % 2 != 0)
		return -EINVAL;

	for (i = 0; i < DSI_MAX_NR_LANES; ++i)
		lanes[i].function = DSI_LANE_UNUSED;

	num_lanes = 0;

	for (i = 0; i < num_pins; i += 2) {
		u8 lane, pol;
		u32 dx, dy;

		dx = pins[i];
		dy = pins[i + 1];

		if (dx >= dsi->num_lanes_supported * 2)
			return -EINVAL;

		if (dy >= dsi->num_lanes_supported * 2)
			return -EINVAL;

		if (dx & 1) {
			if (dy != dx - 1)
				return -EINVAL;
			pol = 1;
		} else {
			if (dy != dx + 1)
				return -EINVAL;
			pol = 0;
		}

		lane = dx / 2;

		lanes[lane].function = functions[i / 2];
		lanes[lane].polarity = pol;
		num_lanes++;
	}

	memcpy(dsi->lanes, lanes, sizeof(dsi->lanes));
	dsi->num_lanes_used = num_lanes;

	return 0;
}

static int dsi_enable_video_mode(struct dsi_data *dsi, int vc)
{
	int bpp = mipi_dsi_pixel_format_to_bpp(dsi->pix_fmt);
	u8 data_type;
	u16 word_count;

	switch (dsi->pix_fmt) {
	case MIPI_DSI_FMT_RGB888:
		data_type = MIPI_DSI_PACKED_PIXEL_STREAM_24;
		break;
	case MIPI_DSI_FMT_RGB666:
		data_type = MIPI_DSI_PIXEL_STREAM_3BYTE_18;
		break;
	case MIPI_DSI_FMT_RGB666_PACKED:
		data_type = MIPI_DSI_PACKED_PIXEL_STREAM_18;
		break;
	case MIPI_DSI_FMT_RGB565:
		data_type = MIPI_DSI_PACKED_PIXEL_STREAM_16;
		break;
	default:
		return -EINVAL;
	}

	dsi_if_enable(dsi, false);
	dsi_vc_enable(dsi, vc, false);

	/* MODE, 1 = video mode */
	REG_FLD_MOD(dsi, DSI_VC_CTRL(vc), 1, 4, 4);

	word_count = DIV_ROUND_UP(dsi->vm.hactive * bpp, 8);

	dsi_vc_write_long_header(dsi, vc, dsi->dsidev->channel, data_type,
			word_count, 0);

	dsi_vc_enable(dsi, vc, true);
	dsi_if_enable(dsi, true);

	return 0;
}

static void dsi_disable_video_mode(struct dsi_data *dsi, int vc)
{
	dsi_if_enable(dsi, false);
	dsi_vc_enable(dsi, vc, false);

	/* MODE, 0 = command mode */
	REG_FLD_MOD(dsi, DSI_VC_CTRL(vc), 0, 4, 4);

	dsi_vc_enable(dsi, vc, true);
	dsi_if_enable(dsi, true);
}

static void dsi_enable_video_output(struct omap_dss_device *dssdev, int vc)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);
	int r;

	r = dsi_init_dispc(dsi);
	if (r) {
		dev_err(dsi->dev, "failed to init dispc!\n");
		return;
	}

	if (dsi->mode == OMAP_DSS_DSI_VIDEO_MODE) {
		r = dsi_enable_video_mode(dsi, vc);
		if (r)
			goto err_video_mode;
	}

	r = dss_mgr_enable(&dsi->output);
	if (r)
		goto err_mgr_enable;

	return;

err_mgr_enable:
	if (dsi->mode == OMAP_DSS_DSI_VIDEO_MODE) {
		dsi_if_enable(dsi, false);
		dsi_vc_enable(dsi, vc, false);
	}
err_video_mode:
	dsi_uninit_dispc(dsi);
	dev_err(dsi->dev, "failed to enable DSI encoder!\n");
	return;
}

static void dsi_disable_video_output(struct omap_dss_device *dssdev, int vc)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);

	if (dsi->mode == OMAP_DSS_DSI_VIDEO_MODE)
		dsi_disable_video_mode(dsi, vc);

	dss_mgr_disable(&dsi->output);

	dsi_uninit_dispc(dsi);
}

static void dsi_update_screen_dispc(struct dsi_data *dsi)
{
	unsigned int bytespp;
	unsigned int bytespl;
	unsigned int bytespf;
	unsigned int total_len;
	unsigned int packet_payload;
	unsigned int packet_len;
	u32 l;
	int r;
	const unsigned vc = dsi->update_vc;
	const unsigned int line_buf_size = dsi->line_buffer_size;
	u16 w = dsi->vm.hactive;
	u16 h = dsi->vm.vactive;

	DSSDBG("dsi_update_screen_dispc(%dx%d)\n", w, h);

	bytespp	= mipi_dsi_pixel_format_to_bpp(dsi->pix_fmt) / 8;
	bytespl = w * bytespp;
	bytespf = bytespl * h;

	/* NOTE: packet_payload has to be equal to N * bytespl, where N is
	 * number of lines in a packet.  See errata about VP_CLK_RATIO */

	if (bytespf < line_buf_size)
		packet_payload = bytespf;
	else
		packet_payload = (line_buf_size) / bytespl * bytespl;

	packet_len = packet_payload + 1;	/* 1 byte for DCS cmd */
	total_len = (bytespf / packet_payload) * packet_len;

	if (bytespf % packet_payload)
		total_len += (bytespf % packet_payload) + 1;

	l = FLD_VAL(total_len, 23, 0); /* TE_SIZE */
	dsi_write_reg(dsi, DSI_VC_TE(vc), l);

	dsi_vc_write_long_header(dsi, vc, dsi->dsidev->channel, MIPI_DSI_DCS_LONG_WRITE,
		packet_len, 0);

	if (dsi->te_enabled)
		l = FLD_MOD(l, 1, 30, 30); /* TE_EN */
	else
		l = FLD_MOD(l, 1, 31, 31); /* TE_START */
	dsi_write_reg(dsi, DSI_VC_TE(vc), l);

	/* We put SIDLEMODE to no-idle for the duration of the transfer,
	 * because DSS interrupts are not capable of waking up the CPU and the
	 * framedone interrupt could be delayed for quite a long time. I think
	 * the same goes for any DSS interrupts, but for some reason I have not
	 * seen the problem anywhere else than here.
	 */
	dispc_disable_sidle(dsi->dss->dispc);

	dsi_perf_mark_start(dsi);

	r = schedule_delayed_work(&dsi->framedone_timeout_work,
		msecs_to_jiffies(250));
	BUG_ON(r == 0);

	dss_mgr_start_update(&dsi->output);

	if (dsi->te_enabled) {
		/* disable LP_RX_TO, so that we can receive TE.  Time to wait
		 * for TE is longer than the timer allows */
		REG_FLD_MOD(dsi, DSI_TIMING2, 0, 15, 15); /* LP_RX_TO */

		dsi_vc_send_bta(dsi, vc);

#ifdef DSI_CATCH_MISSING_TE
		mod_timer(&dsi->te_timer, jiffies + msecs_to_jiffies(250));
#endif
	}
}

#ifdef DSI_CATCH_MISSING_TE
static void dsi_te_timeout(struct timer_list *unused)
{
	DSSERR("TE not received for 250ms!\n");
}
#endif

static void dsi_handle_framedone(struct dsi_data *dsi, int error)
{
	/* SIDLEMODE back to smart-idle */
	dispc_enable_sidle(dsi->dss->dispc);

	if (dsi->te_enabled) {
		/* enable LP_RX_TO again after the TE */
		REG_FLD_MOD(dsi, DSI_TIMING2, 1, 15, 15); /* LP_RX_TO */
	}

	dsi_bus_unlock(dsi);

	if (!error)
		dsi_perf_show(dsi, "DISPC");
}

static void dsi_framedone_timeout_work_callback(struct work_struct *work)
{
	struct dsi_data *dsi = container_of(work, struct dsi_data,
			framedone_timeout_work.work);
	/* XXX While extremely unlikely, we could get FRAMEDONE interrupt after
	 * 250ms which would conflict with this timeout work. What should be
	 * done is first cancel the transfer on the HW, and then cancel the
	 * possibly scheduled framedone work. However, cancelling the transfer
	 * on the HW is buggy, and would probably require resetting the whole
	 * DSI */

	DSSERR("Framedone not received for 250ms!\n");

	dsi_handle_framedone(dsi, -ETIMEDOUT);
}

static void dsi_framedone_irq_callback(void *data)
{
	struct dsi_data *dsi = data;

	/* Note: We get FRAMEDONE when DISPC has finished sending pixels and
	 * turns itself off. However, DSI still has the pixels in its buffers,
	 * and is sending the data.
	 */

	cancel_delayed_work(&dsi->framedone_timeout_work);

	DSSDBG("Framedone received!\n");

	dsi_handle_framedone(dsi, 0);
}

static int _dsi_update(struct dsi_data *dsi)
{
	dsi_perf_mark_setup(dsi);

#ifdef DSI_PERF_MEASURE
	dsi->update_bytes = dsi->vm.hactive * dsi->vm.vactive *
		mipi_dsi_pixel_format_to_bpp(dsi->pix_fmt) / 8;
#endif
	dsi_update_screen_dispc(dsi);

	return 0;
}

static int _dsi_send_nop(struct dsi_data *dsi, int vc, int channel)
{
	const u8 payload[] = { MIPI_DCS_NOP };
	const struct mipi_dsi_msg msg = {
		.channel = channel,
		.type = MIPI_DSI_DCS_SHORT_WRITE,
		.tx_len = 1,
		.tx_buf = payload,
	};

	WARN_ON(!dsi_bus_is_locked(dsi));

	return _omap_dsi_host_transfer(dsi, vc, &msg);
}

static int dsi_update_channel(struct omap_dss_device *dssdev, int vc)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);
	int r;

	dsi_bus_lock(dsi);

	if (!dsi->video_enabled) {
		r = -EIO;
		goto err;
	}

	if (dsi->vm.hactive == 0 || dsi->vm.vactive == 0) {
		r = -EINVAL;
		goto err;
	}

	DSSDBG("dsi_update_channel: %d", vc);

	/*
	 * Send NOP between the frames. If we don't send something here, the
	 * updates stop working. This is probably related to DSI spec stating
	 * that the DSI host should transition to LP at least once per frame.
	 */
	r = _dsi_send_nop(dsi, VC_CMD, dsi->dsidev->channel);
	if (r < 0) {
		DSSWARN("failed to send nop between frames: %d\n", r);
		goto err;
	}

	dsi->update_vc = vc;

	if (dsi->te_enabled && dsi->te_gpio) {
		schedule_delayed_work(&dsi->te_timeout_work,
				      msecs_to_jiffies(250));
		atomic_set(&dsi->do_ext_te_update, 1);
	} else {
		_dsi_update(dsi);
	}

	return 0;

err:
	dsi_bus_unlock(dsi);
	return r;
}

static int dsi_update_all(struct omap_dss_device *dssdev)
{
	return dsi_update_channel(dssdev, VC_VIDEO);
}

/* Display funcs */

static int dsi_configure_dispc_clocks(struct dsi_data *dsi)
{
	struct dispc_clock_info dispc_cinfo;
	int r;
	unsigned long fck;

	fck = dsi_get_pll_hsdiv_dispc_rate(dsi);

	dispc_cinfo.lck_div = dsi->user_dispc_cinfo.lck_div;
	dispc_cinfo.pck_div = dsi->user_dispc_cinfo.pck_div;

	r = dispc_calc_clock_rates(dsi->dss->dispc, fck, &dispc_cinfo);
	if (r) {
		DSSERR("Failed to calc dispc clocks\n");
		return r;
	}

	dsi->mgr_config.clock_info = dispc_cinfo;

	return 0;
}

static int dsi_init_dispc(struct dsi_data *dsi)
{
	enum omap_channel dispc_channel = dsi->output.dispc_channel;
	int r;

	dss_select_lcd_clk_source(dsi->dss, dispc_channel, dsi->module_id == 0 ?
			DSS_CLK_SRC_PLL1_1 :
			DSS_CLK_SRC_PLL2_1);

	if (dsi->mode == OMAP_DSS_DSI_CMD_MODE) {
		r = dss_mgr_register_framedone_handler(&dsi->output,
				dsi_framedone_irq_callback, dsi);
		if (r) {
			DSSERR("can't register FRAMEDONE handler\n");
			goto err;
		}

		dsi->mgr_config.stallmode = true;
		dsi->mgr_config.fifohandcheck = true;
	} else {
		dsi->mgr_config.stallmode = false;
		dsi->mgr_config.fifohandcheck = false;
	}

	r = dsi_configure_dispc_clocks(dsi);
	if (r)
		goto err1;

	dsi->mgr_config.io_pad_mode = DSS_IO_PAD_MODE_BYPASS;
	dsi->mgr_config.video_port_width =
			mipi_dsi_pixel_format_to_bpp(dsi->pix_fmt);
	dsi->mgr_config.lcden_sig_polarity = 0;

	dss_mgr_set_lcd_config(&dsi->output, &dsi->mgr_config);

	return 0;
err1:
	if (dsi->mode == OMAP_DSS_DSI_CMD_MODE)
		dss_mgr_unregister_framedone_handler(&dsi->output,
				dsi_framedone_irq_callback, dsi);
err:
	dss_select_lcd_clk_source(dsi->dss, dispc_channel, DSS_CLK_SRC_FCK);
	return r;
}

static void dsi_uninit_dispc(struct dsi_data *dsi)
{
	enum omap_channel dispc_channel = dsi->output.dispc_channel;

	if (dsi->mode == OMAP_DSS_DSI_CMD_MODE)
		dss_mgr_unregister_framedone_handler(&dsi->output,
				dsi_framedone_irq_callback, dsi);

	dss_select_lcd_clk_source(dsi->dss, dispc_channel, DSS_CLK_SRC_FCK);
}

static int dsi_configure_dsi_clocks(struct dsi_data *dsi)
{
	struct dss_pll_clock_info cinfo;
	int r;

	cinfo = dsi->user_dsi_cinfo;

	r = dss_pll_set_config(&dsi->pll, &cinfo);
	if (r) {
		DSSERR("Failed to set dsi clocks\n");
		return r;
	}

	return 0;
}

static void dsi_setup_dsi_vcs(struct dsi_data *dsi)
{
	/* Setup VC_CMD for LP and cpu transfers */
	REG_FLD_MOD(dsi, DSI_VC_CTRL(VC_CMD), 0, 9, 9); /* LP */

	REG_FLD_MOD(dsi, DSI_VC_CTRL(VC_CMD), 0, 1, 1); /* SOURCE_L4 */
	dsi->vc[VC_CMD].source = DSI_VC_SOURCE_L4;

	/* Setup VC_VIDEO for HS and dispc transfers */
	REG_FLD_MOD(dsi, DSI_VC_CTRL(VC_VIDEO), 1, 9, 9); /* HS */

	REG_FLD_MOD(dsi, DSI_VC_CTRL(VC_VIDEO), 1, 1, 1); /* SOURCE_VP */
	dsi->vc[VC_VIDEO].source = DSI_VC_SOURCE_VP;

	if ((dsi->data->quirks & DSI_QUIRK_DCS_CMD_CONFIG_VC) &&
	    !(dsi->dsidev->mode_flags & MIPI_DSI_MODE_VIDEO))
		REG_FLD_MOD(dsi, DSI_VC_CTRL(VC_VIDEO), 1, 30, 30); /* DCS_CMD_ENABLE */

	dsi_vc_enable(dsi, VC_CMD, 1);
	dsi_vc_enable(dsi, VC_VIDEO, 1);

	dsi_if_enable(dsi, 1);

	dsi_force_tx_stop_mode_io(dsi);

	/* start the DDR clock by sending a NULL packet */
	if (!(dsi->dsidev->mode_flags & MIPI_DSI_CLOCK_NON_CONTINUOUS))
		dsi_vc_send_null(dsi, VC_CMD, dsi->dsidev->channel);
}

static int dsi_init_dsi(struct dsi_data *dsi)
{
	int r;

	r = dss_pll_enable(&dsi->pll);
	if (r)
		return r;

	r = dsi_configure_dsi_clocks(dsi);
	if (r)
		goto err0;

	dss_select_dsi_clk_source(dsi->dss, dsi->module_id,
				  dsi->module_id == 0 ?
				  DSS_CLK_SRC_PLL1_2 : DSS_CLK_SRC_PLL2_2);

	DSSDBG("PLL OK\n");

	if (!dsi->vdds_dsi_enabled) {
		r = regulator_enable(dsi->vdds_dsi_reg);
		if (r)
			goto err1;

		dsi->vdds_dsi_enabled = true;
	}

	r = dsi_cio_init(dsi);
	if (r)
		goto err2;

	_dsi_print_reset_status(dsi);

	dsi_proto_timings(dsi);
	dsi_set_lp_clk_divisor(dsi);

	if (1)
		_dsi_print_reset_status(dsi);

	r = dsi_proto_config(dsi);
	if (r)
		goto err3;

	dsi_setup_dsi_vcs(dsi);

	return 0;
err3:
	dsi_cio_uninit(dsi);
err2:
	regulator_disable(dsi->vdds_dsi_reg);
	dsi->vdds_dsi_enabled = false;
err1:
	dss_select_dsi_clk_source(dsi->dss, dsi->module_id, DSS_CLK_SRC_FCK);
err0:
	dss_pll_disable(&dsi->pll);

	return r;
}

static void dsi_uninit_dsi(struct dsi_data *dsi)
{
	/* disable interface */
	dsi_if_enable(dsi, 0);
	dsi_vc_enable(dsi, 0, 0);
	dsi_vc_enable(dsi, 1, 0);
	dsi_vc_enable(dsi, 2, 0);
	dsi_vc_enable(dsi, 3, 0);

	dss_select_dsi_clk_source(dsi->dss, dsi->module_id, DSS_CLK_SRC_FCK);
	dsi_cio_uninit(dsi);
	dss_pll_disable(&dsi->pll);

	regulator_disable(dsi->vdds_dsi_reg);
	dsi->vdds_dsi_enabled = false;
}

static void dsi_enable(struct dsi_data *dsi)
{
	int r;

	WARN_ON(!dsi_bus_is_locked(dsi));

	if (WARN_ON(dsi->iface_enabled))
		return;

	mutex_lock(&dsi->lock);

	r = dsi_runtime_get(dsi);
	if (r)
		goto err_get_dsi;

	_dsi_initialize_irq(dsi);

	r = dsi_init_dsi(dsi);
	if (r)
		goto err_init_dsi;

	dsi->iface_enabled = true;

	mutex_unlock(&dsi->lock);

	return;

err_init_dsi:
	dsi_runtime_put(dsi);
err_get_dsi:
	mutex_unlock(&dsi->lock);
	DSSDBG("dsi_enable FAILED\n");
}

static void dsi_disable(struct dsi_data *dsi)
{
	WARN_ON(!dsi_bus_is_locked(dsi));

	if (WARN_ON(!dsi->iface_enabled))
		return;

	mutex_lock(&dsi->lock);

	dsi_sync_vc(dsi, 0);
	dsi_sync_vc(dsi, 1);
	dsi_sync_vc(dsi, 2);
	dsi_sync_vc(dsi, 3);

	dsi_uninit_dsi(dsi);

	dsi_runtime_put(dsi);

	dsi->iface_enabled = false;

	mutex_unlock(&dsi->lock);
}

static int dsi_enable_te(struct dsi_data *dsi, bool enable)
{
	dsi->te_enabled = enable;

	if (dsi->te_gpio) {
		if (enable)
			enable_irq(dsi->te_irq);
		else
			disable_irq(dsi->te_irq);
	}

	return 0;
}

#ifdef PRINT_VERBOSE_VM_TIMINGS
static void print_dsi_vm(const char *str,
		const struct omap_dss_dsi_videomode_timings *t)
{
	unsigned long byteclk = t->hsclk / 4;
	int bl, wc, pps, tot;

	wc = DIV_ROUND_UP(t->hact * t->bitspp, 8);
	pps = DIV_ROUND_UP(wc + 6, t->ndl); /* pixel packet size */
	bl = t->hss + t->hsa + t->hse + t->hbp + t->hfp;
	tot = bl + pps;

#define TO_DSI_T(x) ((u32)div64_u64((u64)x * 1000000000llu, byteclk))

	pr_debug("%s bck %lu, %u/%u/%u/%u/%u/%u = %u+%u = %u, "
			"%u/%u/%u/%u/%u/%u = %u + %u = %u\n",
			str,
			byteclk,
			t->hss, t->hsa, t->hse, t->hbp, pps, t->hfp,
			bl, pps, tot,
			TO_DSI_T(t->hss),
			TO_DSI_T(t->hsa),
			TO_DSI_T(t->hse),
			TO_DSI_T(t->hbp),
			TO_DSI_T(pps),
			TO_DSI_T(t->hfp),

			TO_DSI_T(bl),
			TO_DSI_T(pps),

			TO_DSI_T(tot));
#undef TO_DSI_T
}

static void print_dispc_vm(const char *str, const struct videomode *vm)
{
	unsigned long pck = vm->pixelclock;
	int hact, bl, tot;

	hact = vm->hactive;
	bl = vm->hsync_len + vm->hback_porch + vm->hfront_porch;
	tot = hact + bl;

#define TO_DISPC_T(x) ((u32)div64_u64((u64)x * 1000000000llu, pck))

	pr_debug("%s pck %lu, %u/%u/%u/%u = %u+%u = %u, "
			"%u/%u/%u/%u = %u + %u = %u\n",
			str,
			pck,
			vm->hsync_len, vm->hback_porch, hact, vm->hfront_porch,
			bl, hact, tot,
			TO_DISPC_T(vm->hsync_len),
			TO_DISPC_T(vm->hback_porch),
			TO_DISPC_T(hact),
			TO_DISPC_T(vm->hfront_porch),
			TO_DISPC_T(bl),
			TO_DISPC_T(hact),
			TO_DISPC_T(tot));
#undef TO_DISPC_T
}

/* note: this is not quite accurate */
static void print_dsi_dispc_vm(const char *str,
		const struct omap_dss_dsi_videomode_timings *t)
{
	struct videomode vm = { 0 };
	unsigned long byteclk = t->hsclk / 4;
	unsigned long pck;
	u64 dsi_tput;
	int dsi_hact, dsi_htot;

	dsi_tput = (u64)byteclk * t->ndl * 8;
	pck = (u32)div64_u64(dsi_tput, t->bitspp);
	dsi_hact = DIV_ROUND_UP(DIV_ROUND_UP(t->hact * t->bitspp, 8) + 6, t->ndl);
	dsi_htot = t->hss + t->hsa + t->hse + t->hbp + dsi_hact + t->hfp;

	vm.pixelclock = pck;
	vm.hsync_len = div64_u64((u64)(t->hsa + t->hse) * pck, byteclk);
	vm.hback_porch = div64_u64((u64)t->hbp * pck, byteclk);
	vm.hfront_porch = div64_u64((u64)t->hfp * pck, byteclk);
	vm.hactive = t->hact;

	print_dispc_vm(str, &vm);
}
#endif /* PRINT_VERBOSE_VM_TIMINGS */

static bool dsi_cm_calc_dispc_cb(int lckd, int pckd, unsigned long lck,
		unsigned long pck, void *data)
{
	struct dsi_clk_calc_ctx *ctx = data;
	struct videomode *vm = &ctx->vm;

	ctx->dispc_cinfo.lck_div = lckd;
	ctx->dispc_cinfo.pck_div = pckd;
	ctx->dispc_cinfo.lck = lck;
	ctx->dispc_cinfo.pck = pck;

	*vm = *ctx->config->vm;
	vm->pixelclock = pck;
	vm->hactive = ctx->config->vm->hactive;
	vm->vactive = ctx->config->vm->vactive;
	vm->hsync_len = vm->hfront_porch = vm->hback_porch = vm->vsync_len = 1;
	vm->vfront_porch = vm->vback_porch = 0;

	return true;
}

static bool dsi_cm_calc_hsdiv_cb(int m_dispc, unsigned long dispc,
		void *data)
{
	struct dsi_clk_calc_ctx *ctx = data;

	ctx->dsi_cinfo.mX[HSDIV_DISPC] = m_dispc;
	ctx->dsi_cinfo.clkout[HSDIV_DISPC] = dispc;

	return dispc_div_calc(ctx->dsi->dss->dispc, dispc,
			      ctx->req_pck_min, ctx->req_pck_max,
			      dsi_cm_calc_dispc_cb, ctx);
}

static bool dsi_cm_calc_pll_cb(int n, int m, unsigned long fint,
		unsigned long clkdco, void *data)
{
	struct dsi_clk_calc_ctx *ctx = data;
	struct dsi_data *dsi = ctx->dsi;

	ctx->dsi_cinfo.n = n;
	ctx->dsi_cinfo.m = m;
	ctx->dsi_cinfo.fint = fint;
	ctx->dsi_cinfo.clkdco = clkdco;

	return dss_pll_hsdiv_calc_a(ctx->pll, clkdco, ctx->req_pck_min,
			dsi->data->max_fck_freq,
			dsi_cm_calc_hsdiv_cb, ctx);
}

static bool dsi_cm_calc(struct dsi_data *dsi,
		const struct omap_dss_dsi_config *cfg,
		struct dsi_clk_calc_ctx *ctx)
{
	unsigned long clkin;
	int bitspp, ndl;
	unsigned long pll_min, pll_max;
	unsigned long pck, txbyteclk;

	clkin = clk_get_rate(dsi->pll.clkin);
	bitspp = mipi_dsi_pixel_format_to_bpp(cfg->pixel_format);
	ndl = dsi->num_lanes_used - 1;

	/*
	 * Here we should calculate minimum txbyteclk to be able to send the
	 * frame in time, and also to handle TE. That's not very simple, though,
	 * especially as we go to LP between each pixel packet due to HW
	 * "feature". So let's just estimate very roughly and multiply by 1.5.
	 */
	pck = cfg->vm->pixelclock;
	pck = pck * 3 / 2;
	txbyteclk = pck * bitspp / 8 / ndl;

	memset(ctx, 0, sizeof(*ctx));
	ctx->dsi = dsi;
	ctx->pll = &dsi->pll;
	ctx->config = cfg;
	ctx->req_pck_min = pck;
	ctx->req_pck_nom = pck;
	ctx->req_pck_max = pck * 3 / 2;

	pll_min = max(cfg->hs_clk_min * 4, txbyteclk * 4 * 4);
	pll_max = cfg->hs_clk_max * 4;

	return dss_pll_calc_a(ctx->pll, clkin,
			pll_min, pll_max,
			dsi_cm_calc_pll_cb, ctx);
}

static bool dsi_vm_calc_blanking(struct dsi_clk_calc_ctx *ctx)
{
	struct dsi_data *dsi = ctx->dsi;
	const struct omap_dss_dsi_config *cfg = ctx->config;
	int bitspp = mipi_dsi_pixel_format_to_bpp(cfg->pixel_format);
	int ndl = dsi->num_lanes_used - 1;
	unsigned long hsclk = ctx->dsi_cinfo.clkdco / 4;
	unsigned long byteclk = hsclk / 4;

	unsigned long dispc_pck, req_pck_min, req_pck_nom, req_pck_max;
	int xres;
	int panel_htot, panel_hbl; /* pixels */
	int dispc_htot, dispc_hbl; /* pixels */
	int dsi_htot, dsi_hact, dsi_hbl, hss, hse; /* byteclks */
	int hfp, hsa, hbp;
	const struct videomode *req_vm;
	struct videomode *dispc_vm;
	struct omap_dss_dsi_videomode_timings *dsi_vm;
	u64 dsi_tput, dispc_tput;

	dsi_tput = (u64)byteclk * ndl * 8;

	req_vm = cfg->vm;
	req_pck_min = ctx->req_pck_min;
	req_pck_max = ctx->req_pck_max;
	req_pck_nom = ctx->req_pck_nom;

	dispc_pck = ctx->dispc_cinfo.pck;
	dispc_tput = (u64)dispc_pck * bitspp;

	xres = req_vm->hactive;

	panel_hbl = req_vm->hfront_porch + req_vm->hback_porch +
		    req_vm->hsync_len;
	panel_htot = xres + panel_hbl;

	dsi_hact = DIV_ROUND_UP(DIV_ROUND_UP(xres * bitspp, 8) + 6, ndl);

	/*
	 * When there are no line buffers, DISPC and DSI must have the
	 * same tput. Otherwise DISPC tput needs to be higher than DSI's.
	 */
	if (dsi->line_buffer_size < xres * bitspp / 8) {
		if (dispc_tput != dsi_tput)
			return false;
	} else {
		if (dispc_tput < dsi_tput)
			return false;
	}

	/* DSI tput must be over the min requirement */
	if (dsi_tput < (u64)bitspp * req_pck_min)
		return false;

	/* When non-burst mode, DSI tput must be below max requirement. */
	if (cfg->trans_mode != OMAP_DSS_DSI_BURST_MODE) {
		if (dsi_tput > (u64)bitspp * req_pck_max)
			return false;
	}

	hss = DIV_ROUND_UP(4, ndl);

	if (cfg->trans_mode == OMAP_DSS_DSI_PULSE_MODE) {
		if (ndl == 3 && req_vm->hsync_len == 0)
			hse = 1;
		else
			hse = DIV_ROUND_UP(4, ndl);
	} else {
		hse = 0;
	}

	/* DSI htot to match the panel's nominal pck */
	dsi_htot = div64_u64((u64)panel_htot * byteclk, req_pck_nom);

	/* fail if there would be no time for blanking */
	if (dsi_htot < hss + hse + dsi_hact)
		return false;

	/* total DSI blanking needed to achieve panel's TL */
	dsi_hbl = dsi_htot - dsi_hact;

	/* DISPC htot to match the DSI TL */
	dispc_htot = div64_u64((u64)dsi_htot * dispc_pck, byteclk);

	/* verify that the DSI and DISPC TLs are the same */
	if ((u64)dsi_htot * dispc_pck != (u64)dispc_htot * byteclk)
		return false;

	dispc_hbl = dispc_htot - xres;

	/* setup DSI videomode */

	dsi_vm = &ctx->dsi_vm;
	memset(dsi_vm, 0, sizeof(*dsi_vm));

	dsi_vm->hsclk = hsclk;

	dsi_vm->ndl = ndl;
	dsi_vm->bitspp = bitspp;

	if (cfg->trans_mode != OMAP_DSS_DSI_PULSE_MODE) {
		hsa = 0;
	} else if (ndl == 3 && req_vm->hsync_len == 0) {
		hsa = 0;
	} else {
		hsa = div64_u64((u64)req_vm->hsync_len * byteclk, req_pck_nom);
		hsa = max(hsa - hse, 1);
	}

	hbp = div64_u64((u64)req_vm->hback_porch * byteclk, req_pck_nom);
	hbp = max(hbp, 1);

	hfp = dsi_hbl - (hss + hsa + hse + hbp);
	if (hfp < 1) {
		int t;
		/* we need to take cycles from hbp */

		t = 1 - hfp;
		hbp = max(hbp - t, 1);
		hfp = dsi_hbl - (hss + hsa + hse + hbp);

		if (hfp < 1 && hsa > 0) {
			/* we need to take cycles from hsa */
			t = 1 - hfp;
			hsa = max(hsa - t, 1);
			hfp = dsi_hbl - (hss + hsa + hse + hbp);
		}
	}

	if (hfp < 1)
		return false;

	dsi_vm->hss = hss;
	dsi_vm->hsa = hsa;
	dsi_vm->hse = hse;
	dsi_vm->hbp = hbp;
	dsi_vm->hact = xres;
	dsi_vm->hfp = hfp;

	dsi_vm->vsa = req_vm->vsync_len;
	dsi_vm->vbp = req_vm->vback_porch;
	dsi_vm->vact = req_vm->vactive;
	dsi_vm->vfp = req_vm->vfront_porch;

	dsi_vm->trans_mode = cfg->trans_mode;

	dsi_vm->blanking_mode = 0;
	dsi_vm->hsa_blanking_mode = 1;
	dsi_vm->hfp_blanking_mode = 1;
	dsi_vm->hbp_blanking_mode = 1;

	dsi_vm->window_sync = 4;

	/* setup DISPC videomode */

	dispc_vm = &ctx->vm;
	*dispc_vm = *req_vm;
	dispc_vm->pixelclock = dispc_pck;

	if (cfg->trans_mode == OMAP_DSS_DSI_PULSE_MODE) {
		hsa = div64_u64((u64)req_vm->hsync_len * dispc_pck,
				req_pck_nom);
		hsa = max(hsa, 1);
	} else {
		hsa = 1;
	}

	hbp = div64_u64((u64)req_vm->hback_porch * dispc_pck, req_pck_nom);
	hbp = max(hbp, 1);

	hfp = dispc_hbl - hsa - hbp;
	if (hfp < 1) {
		int t;
		/* we need to take cycles from hbp */

		t = 1 - hfp;
		hbp = max(hbp - t, 1);
		hfp = dispc_hbl - hsa - hbp;

		if (hfp < 1) {
			/* we need to take cycles from hsa */
			t = 1 - hfp;
			hsa = max(hsa - t, 1);
			hfp = dispc_hbl - hsa - hbp;
		}
	}

	if (hfp < 1)
		return false;

	dispc_vm->hfront_porch = hfp;
	dispc_vm->hsync_len = hsa;
	dispc_vm->hback_porch = hbp;

	return true;
}


static bool dsi_vm_calc_dispc_cb(int lckd, int pckd, unsigned long lck,
		unsigned long pck, void *data)
{
	struct dsi_clk_calc_ctx *ctx = data;

	ctx->dispc_cinfo.lck_div = lckd;
	ctx->dispc_cinfo.pck_div = pckd;
	ctx->dispc_cinfo.lck = lck;
	ctx->dispc_cinfo.pck = pck;

	if (dsi_vm_calc_blanking(ctx) == false)
		return false;

#ifdef PRINT_VERBOSE_VM_TIMINGS
	print_dispc_vm("dispc", &ctx->vm);
	print_dsi_vm("dsi  ", &ctx->dsi_vm);
	print_dispc_vm("req  ", ctx->config->vm);
	print_dsi_dispc_vm("act  ", &ctx->dsi_vm);
#endif

	return true;
}

static bool dsi_vm_calc_hsdiv_cb(int m_dispc, unsigned long dispc,
		void *data)
{
	struct dsi_clk_calc_ctx *ctx = data;
	unsigned long pck_max;

	ctx->dsi_cinfo.mX[HSDIV_DISPC] = m_dispc;
	ctx->dsi_cinfo.clkout[HSDIV_DISPC] = dispc;

	/*
	 * In burst mode we can let the dispc pck be arbitrarily high, but it
	 * limits our scaling abilities. So for now, don't aim too high.
	 */

	if (ctx->config->trans_mode == OMAP_DSS_DSI_BURST_MODE)
		pck_max = ctx->req_pck_max + 10000000;
	else
		pck_max = ctx->req_pck_max;

	return dispc_div_calc(ctx->dsi->dss->dispc, dispc,
			      ctx->req_pck_min, pck_max,
			      dsi_vm_calc_dispc_cb, ctx);
}

static bool dsi_vm_calc_pll_cb(int n, int m, unsigned long fint,
		unsigned long clkdco, void *data)
{
	struct dsi_clk_calc_ctx *ctx = data;
	struct dsi_data *dsi = ctx->dsi;

	ctx->dsi_cinfo.n = n;
	ctx->dsi_cinfo.m = m;
	ctx->dsi_cinfo.fint = fint;
	ctx->dsi_cinfo.clkdco = clkdco;

	return dss_pll_hsdiv_calc_a(ctx->pll, clkdco, ctx->req_pck_min,
			dsi->data->max_fck_freq,
			dsi_vm_calc_hsdiv_cb, ctx);
}

static bool dsi_vm_calc(struct dsi_data *dsi,
		const struct omap_dss_dsi_config *cfg,
		struct dsi_clk_calc_ctx *ctx)
{
	const struct videomode *vm = cfg->vm;
	unsigned long clkin;
	unsigned long pll_min;
	unsigned long pll_max;
	int ndl = dsi->num_lanes_used - 1;
	int bitspp = mipi_dsi_pixel_format_to_bpp(cfg->pixel_format);
	unsigned long byteclk_min;

	clkin = clk_get_rate(dsi->pll.clkin);

	memset(ctx, 0, sizeof(*ctx));
	ctx->dsi = dsi;
	ctx->pll = &dsi->pll;
	ctx->config = cfg;

	/* these limits should come from the panel driver */
	ctx->req_pck_min = vm->pixelclock - 1000;
	ctx->req_pck_nom = vm->pixelclock;
	ctx->req_pck_max = vm->pixelclock + 1000;

	byteclk_min = div64_u64((u64)ctx->req_pck_min * bitspp, ndl * 8);
	pll_min = max(cfg->hs_clk_min * 4, byteclk_min * 4 * 4);

	if (cfg->trans_mode == OMAP_DSS_DSI_BURST_MODE) {
		pll_max = cfg->hs_clk_max * 4;
	} else {
		unsigned long byteclk_max;
		byteclk_max = div64_u64((u64)ctx->req_pck_max * bitspp,
				ndl * 8);

		pll_max = byteclk_max * 4 * 4;
	}

	return dss_pll_calc_a(ctx->pll, clkin,
			pll_min, pll_max,
			dsi_vm_calc_pll_cb, ctx);
}

static bool dsi_is_video_mode(struct omap_dss_device *dssdev)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);

	return dsi->mode == OMAP_DSS_DSI_VIDEO_MODE;
}

static int __dsi_calc_config(struct dsi_data *dsi,
		const struct drm_display_mode *mode,
		struct dsi_clk_calc_ctx *ctx)
{
	struct omap_dss_dsi_config cfg = dsi->config;
	struct videomode vm;
	bool ok;
	int r;

	drm_display_mode_to_videomode(mode, &vm);

	cfg.vm = &vm;
	cfg.mode = dsi->mode;
	cfg.pixel_format = dsi->pix_fmt;

	if (dsi->mode == OMAP_DSS_DSI_VIDEO_MODE)
		ok = dsi_vm_calc(dsi, &cfg, ctx);
	else
		ok = dsi_cm_calc(dsi, &cfg, ctx);

	if (!ok)
		return -EINVAL;

	dsi_pll_calc_dsi_fck(dsi, &ctx->dsi_cinfo);

	r = dsi_lp_clock_calc(ctx->dsi_cinfo.clkout[HSDIV_DSI],
		cfg.lp_clk_min, cfg.lp_clk_max, &ctx->lp_cinfo);
	if (r)
		return r;

	return 0;
}

static int dsi_set_config(struct omap_dss_device *dssdev,
		const struct drm_display_mode *mode)
{
	struct dsi_data *dsi = to_dsi_data(dssdev);
	struct dsi_clk_calc_ctx ctx;
	int r;

	mutex_lock(&dsi->lock);

	r = __dsi_calc_config(dsi, mode, &ctx);
	if (r) {
		DSSERR("failed to find suitable DSI clock settings\n");
		goto err;
	}

	dsi->user_lp_cinfo = ctx.lp_cinfo;
	dsi->user_dsi_cinfo = ctx.dsi_cinfo;
	dsi->user_dispc_cinfo = ctx.dispc_cinfo;

	dsi->vm = ctx.vm;

	/*
	 * override interlace, logic level and edge related parameters in
	 * videomode with default values
	 */
	dsi->vm.flags &= ~DISPLAY_FLAGS_INTERLACED;
	dsi->vm.flags &= ~DISPLAY_FLAGS_HSYNC_LOW;
	dsi->vm.flags |= DISPLAY_FLAGS_HSYNC_HIGH;
	dsi->vm.flags &= ~DISPLAY_FLAGS_VSYNC_LOW;
	dsi->vm.flags |= DISPLAY_FLAGS_VSYNC_HIGH;
	/*
	 * HACK: These flags should be handled through the omap_dss_device bus
	 * flags, but this will only be possible when the DSI encoder will be
	 * converted to the omapdrm-managed encoder model.
	 */
	dsi->vm.flags &= ~DISPLAY_FLAGS_PIXDATA_NEGEDGE;
	dsi->vm.flags |= DISPLAY_FLAGS_PIXDATA_POSEDGE;
	dsi->vm.flags &= ~DISPLAY_FLAGS_DE_LOW;
	dsi->vm.flags |= DISPLAY_FLAGS_DE_HIGH;
	dsi->vm.flags &= ~DISPLAY_FLAGS_SYNC_POSEDGE;
	dsi->vm.flags |= DISPLAY_FLAGS_SYNC_NEGEDGE;

	dss_mgr_set_timings(&dsi->output, &dsi->vm);

	dsi->vm_timings = ctx.dsi_vm;

	mutex_unlock(&dsi->lock);

	return 0;
err:
	mutex_unlock(&dsi->lock);

	return r;
}

/*
 * Return a hardcoded dispc channel for the DSI output. This should work for
 * current use cases, but this can be later expanded to either resolve
 * the channel in some more dynamic manner, or get the channel as a user
 * parameter.
 */
static enum omap_channel dsi_get_dispc_channel(struct dsi_data *dsi)
{
	switch (dsi->data->model) {
	case DSI_MODEL_OMAP3:
		return OMAP_DSS_CHANNEL_LCD;

	case DSI_MODEL_OMAP4:
		switch (dsi->module_id) {
		case 0:
			return OMAP_DSS_CHANNEL_LCD;
		case 1:
			return OMAP_DSS_CHANNEL_LCD2;
		default:
			DSSWARN("unsupported module id\n");
			return OMAP_DSS_CHANNEL_LCD;
		}

	case DSI_MODEL_OMAP5:
		switch (dsi->module_id) {
		case 0:
			return OMAP_DSS_CHANNEL_LCD;
		case 1:
			return OMAP_DSS_CHANNEL_LCD3;
		default:
			DSSWARN("unsupported module id\n");
			return OMAP_DSS_CHANNEL_LCD;
		}

	default:
		DSSWARN("unsupported DSS version\n");
		return OMAP_DSS_CHANNEL_LCD;
	}
}

static ssize_t _omap_dsi_host_transfer(struct dsi_data *dsi, int vc,
				       const struct mipi_dsi_msg *msg)
{
	struct omap_dss_device *dssdev = &dsi->output;
	int r;

	dsi_vc_enable_hs(dssdev, vc, !(msg->flags & MIPI_DSI_MSG_USE_LPM));

	switch (msg->type) {
	case MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM:
	case MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM:
	case MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM:
	case MIPI_DSI_GENERIC_LONG_WRITE:
	case MIPI_DSI_DCS_SHORT_WRITE:
	case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
	case MIPI_DSI_DCS_LONG_WRITE:
	case MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE:
	case MIPI_DSI_NULL_PACKET:
		r = dsi_vc_write_common(dssdev, vc, msg);
		break;
	case MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM:
	case MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM:
	case MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM:
		r = dsi_vc_generic_read(dssdev, vc, msg);
		break;
	case MIPI_DSI_DCS_READ:
		r = dsi_vc_dcs_read(dssdev, vc, msg);
		break;
	default:
		r = -EINVAL;
		break;
	}

	if (r < 0)
		return r;

	if (msg->type == MIPI_DSI_DCS_SHORT_WRITE ||
	    msg->type == MIPI_DSI_DCS_SHORT_WRITE_PARAM) {
		u8 cmd = ((u8 *)msg->tx_buf)[0];

		if (cmd == MIPI_DCS_SET_TEAR_OFF)
			dsi_enable_te(dsi, false);
		else if (cmd == MIPI_DCS_SET_TEAR_ON)
			dsi_enable_te(dsi, true);
	}

	return 0;
}

static ssize_t omap_dsi_host_transfer(struct mipi_dsi_host *host,
				      const struct mipi_dsi_msg *msg)
{
	struct dsi_data *dsi = host_to_omap(host);
	int r;
	int vc = VC_CMD;

	dsi_bus_lock(dsi);

	if (!dsi->iface_enabled) {
		dsi_enable(dsi);
		schedule_delayed_work(&dsi->dsi_disable_work, msecs_to_jiffies(2000));
	}

	r = _omap_dsi_host_transfer(dsi, vc, msg);

	dsi_bus_unlock(dsi);

	return r;
}

static int dsi_get_clocks(struct dsi_data *dsi)
{
	struct clk *clk;

	clk = devm_clk_get(dsi->dev, "fck");
	if (IS_ERR(clk)) {
		DSSERR("can't get fck\n");
		return PTR_ERR(clk);
	}

	dsi->dss_clk = clk;

	return 0;
}

static const struct omapdss_dsi_ops dsi_ops = {
	.update = dsi_update_all,
	.is_video_mode = dsi_is_video_mode,
};

static irqreturn_t omap_dsi_te_irq_handler(int irq, void *dev_id)
{
	struct dsi_data *dsi = (struct dsi_data *)dev_id;
	int old;

	old = atomic_cmpxchg(&dsi->do_ext_te_update, 1, 0);
	if (old) {
		cancel_delayed_work(&dsi->te_timeout_work);
		_dsi_update(dsi);
	}

	return IRQ_HANDLED;
}

static void omap_dsi_te_timeout_work_callback(struct work_struct *work)
{
	struct dsi_data *dsi =
		container_of(work, struct dsi_data, te_timeout_work.work);
	int old;

	old = atomic_cmpxchg(&dsi->do_ext_te_update, 1, 0);
	if (old) {
		dev_err(dsi->dev, "TE not received for 250ms!\n");
		_dsi_update(dsi);
	}
}

static int omap_dsi_register_te_irq(struct dsi_data *dsi,
				    struct mipi_dsi_device *client)
{
	int err;
	int te_irq;

	dsi->te_gpio = gpiod_get(&client->dev, "te-gpios", GPIOD_IN);
	if (IS_ERR(dsi->te_gpio)) {
		err = PTR_ERR(dsi->te_gpio);

		if (err == -ENOENT) {
			dsi->te_gpio = NULL;
			return 0;
		}

		dev_err(dsi->dev, "Could not get TE gpio: %d\n", err);
		return err;
	}

	te_irq = gpiod_to_irq(dsi->te_gpio);
	if (te_irq < 0) {
		gpiod_put(dsi->te_gpio);
		dsi->te_gpio = NULL;
		return -EINVAL;
	}

	dsi->te_irq = te_irq;

	irq_set_status_flags(te_irq, IRQ_NOAUTOEN);

	err = request_threaded_irq(te_irq, NULL, omap_dsi_te_irq_handler,
				   IRQF_TRIGGER_RISING | IRQF_ONESHOT,
				   "TE", dsi);
	if (err) {
		dev_err(dsi->dev, "request irq failed with %d\n", err);
		gpiod_put(dsi->te_gpio);
		dsi->te_gpio = NULL;
		return err;
	}

	INIT_DEFERRABLE_WORK(&dsi->te_timeout_work,
			     omap_dsi_te_timeout_work_callback);

	dev_dbg(dsi->dev, "Using GPIO TE\n");

	return 0;
}

static void omap_dsi_unregister_te_irq(struct dsi_data *dsi)
{
	if (dsi->te_gpio) {
		free_irq(dsi->te_irq, dsi);
		cancel_delayed_work(&dsi->te_timeout_work);
		gpiod_put(dsi->te_gpio);
		dsi->te_gpio = NULL;
	}
}

static int omap_dsi_host_attach(struct mipi_dsi_host *host,
				struct mipi_dsi_device *client)
{
	struct dsi_data *dsi = host_to_omap(host);
	int r;

	if (dsi->dsidev) {
		DSSERR("dsi client already attached\n");
		return -EBUSY;
	}

	if (mipi_dsi_pixel_format_to_bpp(client->format) < 0) {
		DSSERR("invalid pixel format\n");
		return -EINVAL;
	}

	atomic_set(&dsi->do_ext_te_update, 0);

	if (client->mode_flags & MIPI_DSI_MODE_VIDEO) {
		dsi->mode = OMAP_DSS_DSI_VIDEO_MODE;
	} else {
		r = omap_dsi_register_te_irq(dsi, client);
		if (r)
			return r;

		dsi->mode = OMAP_DSS_DSI_CMD_MODE;
	}

	dsi->dsidev = client;
	dsi->pix_fmt = client->format;

	dsi->config.hs_clk_min = 150000000; // TODO: get from client?
	dsi->config.hs_clk_max = client->hs_rate;
	dsi->config.lp_clk_min = 7000000; // TODO: get from client?
	dsi->config.lp_clk_max = client->lp_rate;

	if (client->mode_flags & MIPI_DSI_MODE_VIDEO_BURST)
		dsi->config.trans_mode = OMAP_DSS_DSI_BURST_MODE;
	else if (client->mode_flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE)
		dsi->config.trans_mode = OMAP_DSS_DSI_PULSE_MODE;
	else
		dsi->config.trans_mode = OMAP_DSS_DSI_EVENT_MODE;

	return 0;
}

static int omap_dsi_host_detach(struct mipi_dsi_host *host,
				struct mipi_dsi_device *client)
{
	struct dsi_data *dsi = host_to_omap(host);

	if (WARN_ON(dsi->dsidev != client))
		return -EINVAL;

	cancel_delayed_work_sync(&dsi->dsi_disable_work);

	dsi_bus_lock(dsi);

	if (dsi->iface_enabled)
		dsi_disable(dsi);

	dsi_bus_unlock(dsi);

	omap_dsi_unregister_te_irq(dsi);
	dsi->dsidev = NULL;
	return 0;
}

static const struct mipi_dsi_host_ops omap_dsi_host_ops = {
	.attach = omap_dsi_host_attach,
	.detach = omap_dsi_host_detach,
	.transfer = omap_dsi_host_transfer,
};

/* -----------------------------------------------------------------------------
 * PLL
 */

static const struct dss_pll_ops dsi_pll_ops = {
	.enable = dsi_pll_enable,
	.disable = dsi_pll_disable,
	.set_config = dss_pll_write_config_type_a,
};

static const struct dss_pll_hw dss_omap3_dsi_pll_hw = {
	.type = DSS_PLL_TYPE_A,

	.n_max = (1 << 7) - 1,
	.m_max = (1 << 11) - 1,
	.mX_max = (1 << 4) - 1,
	.fint_min = 750000,
	.fint_max = 2100000,
	.clkdco_low = 1000000000,
	.clkdco_max = 1800000000,

	.n_msb = 7,
	.n_lsb = 1,
	.m_msb = 18,
	.m_lsb = 8,

	.mX_msb[0] = 22,
	.mX_lsb[0] = 19,
	.mX_msb[1] = 26,
	.mX_lsb[1] = 23,

	.has_stopmode = true,
	.has_freqsel = true,
	.has_selfreqdco = false,
	.has_refsel = false,
};

static const struct dss_pll_hw dss_omap4_dsi_pll_hw = {
	.type = DSS_PLL_TYPE_A,

	.n_max = (1 << 8) - 1,
	.m_max = (1 << 12) - 1,
	.mX_max = (1 << 5) - 1,
	.fint_min = 500000,
	.fint_max = 2500000,
	.clkdco_low = 1000000000,
	.clkdco_max = 1800000000,

	.n_msb = 8,
	.n_lsb = 1,
	.m_msb = 20,
	.m_lsb = 9,

	.mX_msb[0] = 25,
	.mX_lsb[0] = 21,
	.mX_msb[1] = 30,
	.mX_lsb[1] = 26,

	.has_stopmode = true,
	.has_freqsel = false,
	.has_selfreqdco = false,
	.has_refsel = false,
};

static const struct dss_pll_hw dss_omap5_dsi_pll_hw = {
	.type = DSS_PLL_TYPE_A,

	.n_max = (1 << 8) - 1,
	.m_max = (1 << 12) - 1,
	.mX_max = (1 << 5) - 1,
	.fint_min = 150000,
	.fint_max = 52000000,
	.clkdco_low = 1000000000,
	.clkdco_max = 1800000000,

	.n_msb = 8,
	.n_lsb = 1,
	.m_msb = 20,
	.m_lsb = 9,

	.mX_msb[0] = 25,
	.mX_lsb[0] = 21,
	.mX_msb[1] = 30,
	.mX_lsb[1] = 26,

	.has_stopmode = true,
	.has_freqsel = false,
	.has_selfreqdco = true,
	.has_refsel = true,
};

static int dsi_init_pll_data(struct dss_device *dss, struct dsi_data *dsi)
{
	struct dss_pll *pll = &dsi->pll;
	struct clk *clk;
	int r;

	clk = devm_clk_get(dsi->dev, "sys_clk");
	if (IS_ERR(clk)) {
		DSSERR("can't get sys_clk\n");
		return PTR_ERR(clk);
	}

	pll->name = dsi->module_id == 0 ? "dsi0" : "dsi1";
	pll->id = dsi->module_id == 0 ? DSS_PLL_DSI1 : DSS_PLL_DSI2;
	pll->clkin = clk;
	pll->base = dsi->pll_base;
	pll->hw = dsi->data->pll_hw;
	pll->ops = &dsi_pll_ops;

	r = dss_pll_register(dss, pll);
	if (r)
		return r;

	return 0;
}

/* -----------------------------------------------------------------------------
 * Component Bind & Unbind
 */

static int dsi_bind(struct device *dev, struct device *master, void *data)
{
	struct dss_device *dss = dss_get_device(master);
	struct dsi_data *dsi = dev_get_drvdata(dev);
	char name[10];
	u32 rev;
	int r;

	dsi->dss = dss;

	dsi_init_pll_data(dss, dsi);

	r = dsi_runtime_get(dsi);
	if (r)
		return r;

	rev = dsi_read_reg(dsi, DSI_REVISION);
	dev_dbg(dev, "OMAP DSI rev %d.%d\n",
	       FLD_GET(rev, 7, 4), FLD_GET(rev, 3, 0));

	dsi->line_buffer_size = dsi_get_line_buf_size(dsi);

	dsi_runtime_put(dsi);

	snprintf(name, sizeof(name), "dsi%u_regs", dsi->module_id + 1);
	dsi->debugfs.regs = dss_debugfs_create_file(dss, name,
						    dsi_dump_dsi_regs, dsi);
#ifdef CONFIG_OMAP2_DSS_COLLECT_IRQ_STATS
	snprintf(name, sizeof(name), "dsi%u_irqs", dsi->module_id + 1);
	dsi->debugfs.irqs = dss_debugfs_create_file(dss, name,
						    dsi_dump_dsi_irqs, dsi);
#endif
	snprintf(name, sizeof(name), "dsi%u_clks", dsi->module_id + 1);
	dsi->debugfs.clks = dss_debugfs_create_file(dss, name,
						    dsi_dump_dsi_clocks, dsi);

	return 0;
}

static void dsi_unbind(struct device *dev, struct device *master, void *data)
{
	struct dsi_data *dsi = dev_get_drvdata(dev);

	dss_debugfs_remove_file(dsi->debugfs.clks);
	dss_debugfs_remove_file(dsi->debugfs.irqs);
	dss_debugfs_remove_file(dsi->debugfs.regs);

	WARN_ON(dsi->scp_clk_refcount > 0);

	dss_pll_unregister(&dsi->pll);
}

static const struct component_ops dsi_component_ops = {
	.bind	= dsi_bind,
	.unbind	= dsi_unbind,
};

/* -----------------------------------------------------------------------------
 * DRM Bridge Operations
 */

static int dsi_bridge_attach(struct drm_bridge *bridge,
			     enum drm_bridge_attach_flags flags)
{
	struct dsi_data *dsi = drm_bridge_to_dsi(bridge);

	if (!(flags & DRM_BRIDGE_ATTACH_NO_CONNECTOR))
		return -EINVAL;

	return drm_bridge_attach(bridge->encoder, dsi->output.next_bridge,
				 bridge, flags);
}

static enum drm_mode_status
dsi_bridge_mode_valid(struct drm_bridge *bridge,
		      const struct drm_display_info *info,
		      const struct drm_display_mode *mode)
{
	struct dsi_data *dsi = drm_bridge_to_dsi(bridge);
	struct dsi_clk_calc_ctx ctx;
	int r;

	mutex_lock(&dsi->lock);
	r = __dsi_calc_config(dsi, mode, &ctx);
	mutex_unlock(&dsi->lock);

	return r ? MODE_CLOCK_RANGE : MODE_OK;
}

static void dsi_bridge_mode_set(struct drm_bridge *bridge,
				const struct drm_display_mode *mode,
				const struct drm_display_mode *adjusted_mode)
{
	struct dsi_data *dsi = drm_bridge_to_dsi(bridge);

	dsi_set_config(&dsi->output, adjusted_mode);
}

static void dsi_bridge_enable(struct drm_bridge *bridge)
{
	struct dsi_data *dsi = drm_bridge_to_dsi(bridge);
	struct omap_dss_device *dssdev = &dsi->output;

	cancel_delayed_work_sync(&dsi->dsi_disable_work);

	dsi_bus_lock(dsi);

	if (!dsi->iface_enabled)
		dsi_enable(dsi);

	dsi_enable_video_output(dssdev, VC_VIDEO);

	dsi->video_enabled = true;

	dsi_bus_unlock(dsi);
}

static void dsi_bridge_disable(struct drm_bridge *bridge)
{
	struct dsi_data *dsi = drm_bridge_to_dsi(bridge);
	struct omap_dss_device *dssdev = &dsi->output;

	cancel_delayed_work_sync(&dsi->dsi_disable_work);

	dsi_bus_lock(dsi);

	dsi->video_enabled = false;

	dsi_disable_video_output(dssdev, VC_VIDEO);

	dsi_disable(dsi);

	dsi_bus_unlock(dsi);
}

static const struct drm_bridge_funcs dsi_bridge_funcs = {
	.attach = dsi_bridge_attach,
	.mode_valid = dsi_bridge_mode_valid,
	.mode_set = dsi_bridge_mode_set,
	.enable = dsi_bridge_enable,
	.disable = dsi_bridge_disable,
};

static void dsi_bridge_init(struct dsi_data *dsi)
{
	dsi->bridge.funcs = &dsi_bridge_funcs;
	dsi->bridge.of_node = dsi->host.dev->of_node;
	dsi->bridge.type = DRM_MODE_CONNECTOR_DSI;

	drm_bridge_add(&dsi->bridge);
}

static void dsi_bridge_cleanup(struct dsi_data *dsi)
{
	drm_bridge_remove(&dsi->bridge);
}

/* -----------------------------------------------------------------------------
 * Probe & Remove, Suspend & Resume
 */

static int dsi_init_output(struct dsi_data *dsi)
{
	struct omap_dss_device *out = &dsi->output;
	int r;

	dsi_bridge_init(dsi);

	out->dev = dsi->dev;
	out->id = dsi->module_id == 0 ?
			OMAP_DSS_OUTPUT_DSI1 : OMAP_DSS_OUTPUT_DSI2;

	out->type = OMAP_DISPLAY_TYPE_DSI;
	out->name = dsi->module_id == 0 ? "dsi.0" : "dsi.1";
	out->dispc_channel = dsi_get_dispc_channel(dsi);
	out->dsi_ops = &dsi_ops;
	out->of_port = 0;
	out->bus_flags = DRM_BUS_FLAG_PIXDATA_DRIVE_POSEDGE
		       | DRM_BUS_FLAG_DE_HIGH
		       | DRM_BUS_FLAG_SYNC_DRIVE_NEGEDGE;

	r = omapdss_device_init_output(out, &dsi->bridge);
	if (r < 0) {
		dsi_bridge_cleanup(dsi);
		return r;
	}

	omapdss_device_register(out);

	return 0;
}

static void dsi_uninit_output(struct dsi_data *dsi)
{
	struct omap_dss_device *out = &dsi->output;

	omapdss_device_unregister(out);
	omapdss_device_cleanup_output(out);
	dsi_bridge_cleanup(dsi);
}

static int dsi_probe_of(struct dsi_data *dsi)
{
	struct device_node *node = dsi->dev->of_node;
	struct property *prop;
	u32 lane_arr[10];
	int len, num_pins;
	int r;
	struct device_node *ep;

	ep = of_graph_get_endpoint_by_regs(node, 0, 0);
	if (!ep)
		return 0;

	prop = of_find_property(ep, "lanes", &len);
	if (prop == NULL) {
		dev_err(dsi->dev, "failed to find lane data\n");
		r = -EINVAL;
		goto err;
	}

	num_pins = len / sizeof(u32);

	if (num_pins < 4 || num_pins % 2 != 0 ||
		num_pins > dsi->num_lanes_supported * 2) {
		dev_err(dsi->dev, "bad number of lanes\n");
		r = -EINVAL;
		goto err;
	}

	r = of_property_read_u32_array(ep, "lanes", lane_arr, num_pins);
	if (r) {
		dev_err(dsi->dev, "failed to read lane data\n");
		goto err;
	}

	r = dsi_configure_pins(dsi, num_pins, lane_arr);
	if (r) {
		dev_err(dsi->dev, "failed to configure pins");
		goto err;
	}

	of_node_put(ep);

	return 0;

err:
	of_node_put(ep);
	return r;
}

static const struct dsi_of_data dsi_of_data_omap34xx = {
	.model = DSI_MODEL_OMAP3,
	.pll_hw = &dss_omap3_dsi_pll_hw,
	.modules = (const struct dsi_module_id_data[]) {
		{ .address = 0x4804fc00, .id = 0, },
		{ },
	},
	.max_fck_freq = 173000000,
	.max_pll_lpdiv = (1 << 13) - 1,
	.quirks = DSI_QUIRK_REVERSE_TXCLKESC,
};

static const struct dsi_of_data dsi_of_data_omap36xx = {
	.model = DSI_MODEL_OMAP3,
	.pll_hw = &dss_omap3_dsi_pll_hw,
	.modules = (const struct dsi_module_id_data[]) {
		{ .address = 0x4804fc00, .id = 0, },
		{ },
	},
	.max_fck_freq = 173000000,
	.max_pll_lpdiv = (1 << 13) - 1,
	.quirks = DSI_QUIRK_PLL_PWR_BUG,
};

static const struct dsi_of_data dsi_of_data_omap4 = {
	.model = DSI_MODEL_OMAP4,
	.pll_hw = &dss_omap4_dsi_pll_hw,
	.modules = (const struct dsi_module_id_data[]) {
		{ .address = 0x58004000, .id = 0, },
		{ .address = 0x58005000, .id = 1, },
		{ },
	},
	.max_fck_freq = 170000000,
	.max_pll_lpdiv = (1 << 13) - 1,
	.quirks = DSI_QUIRK_DCS_CMD_CONFIG_VC | DSI_QUIRK_VC_OCP_WIDTH
		| DSI_QUIRK_GNQ,
};

static const struct dsi_of_data dsi_of_data_omap5 = {
	.model = DSI_MODEL_OMAP5,
	.pll_hw = &dss_omap5_dsi_pll_hw,
	.modules = (const struct dsi_module_id_data[]) {
		{ .address = 0x58004000, .id = 0, },
		{ .address = 0x58009000, .id = 1, },
		{ },
	},
	.max_fck_freq = 209250000,
	.max_pll_lpdiv = (1 << 13) - 1,
	.quirks = DSI_QUIRK_DCS_CMD_CONFIG_VC | DSI_QUIRK_VC_OCP_WIDTH
		| DSI_QUIRK_GNQ | DSI_QUIRK_PHY_DCC,
};

static const struct of_device_id dsi_of_match[] = {
	{ .compatible = "ti,omap3-dsi", .data = &dsi_of_data_omap36xx, },
	{ .compatible = "ti,omap4-dsi", .data = &dsi_of_data_omap4, },
	{ .compatible = "ti,omap5-dsi", .data = &dsi_of_data_omap5, },
	{},
};

static const struct soc_device_attribute dsi_soc_devices[] = {
	{ .machine = "OMAP3[45]*",	.data = &dsi_of_data_omap34xx },
	{ .machine = "AM35*",		.data = &dsi_of_data_omap34xx },
	{ /* sentinel */ }
};

static void omap_dsi_disable_work_callback(struct work_struct *work)
{
	struct dsi_data *dsi = container_of(work, struct dsi_data, dsi_disable_work.work);

	dsi_bus_lock(dsi);

	if (dsi->iface_enabled && !dsi->video_enabled)
		dsi_disable(dsi);

	dsi_bus_unlock(dsi);
}

static int dsi_probe(struct platform_device *pdev)
{
	const struct soc_device_attribute *soc;
	const struct dsi_module_id_data *d;
	struct device *dev = &pdev->dev;
	struct dsi_data *dsi;
	struct resource *dsi_mem;
	struct resource *res;
	unsigned int i;
	int r;

	dsi = devm_kzalloc(dev, sizeof(*dsi), GFP_KERNEL);
	if (!dsi)
		return -ENOMEM;

	dsi->dev = dev;
	dev_set_drvdata(dev, dsi);

	spin_lock_init(&dsi->irq_lock);
	spin_lock_init(&dsi->errors_lock);
	dsi->errors = 0;

#ifdef CONFIG_OMAP2_DSS_COLLECT_IRQ_STATS
	spin_lock_init(&dsi->irq_stats_lock);
	dsi->irq_stats.last_reset = jiffies;
#endif

	mutex_init(&dsi->lock);
	sema_init(&dsi->bus_lock, 1);

	INIT_DEFERRABLE_WORK(&dsi->framedone_timeout_work,
			     dsi_framedone_timeout_work_callback);

	INIT_DEFERRABLE_WORK(&dsi->dsi_disable_work, omap_dsi_disable_work_callback);

#ifdef DSI_CATCH_MISSING_TE
	timer_setup(&dsi->te_timer, dsi_te_timeout, 0);
#endif

	dsi_mem = platform_get_resource_byname(pdev, IORESOURCE_MEM, "proto");
	dsi->proto_base = devm_ioremap_resource(dev, dsi_mem);
	if (IS_ERR(dsi->proto_base))
		return PTR_ERR(dsi->proto_base);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "phy");
	dsi->phy_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(dsi->phy_base))
		return PTR_ERR(dsi->phy_base);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pll");
	dsi->pll_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(dsi->pll_base))
		return PTR_ERR(dsi->pll_base);

	dsi->irq = platform_get_irq(pdev, 0);
	if (dsi->irq < 0) {
		DSSERR("platform_get_irq failed\n");
		return -ENODEV;
	}

	r = devm_request_irq(dev, dsi->irq, omap_dsi_irq_handler,
			     IRQF_SHARED, dev_name(dev), dsi);
	if (r < 0) {
		DSSERR("request_irq failed\n");
		return r;
	}

	dsi->vdds_dsi_reg = devm_regulator_get(dev, "vdd");
	if (IS_ERR(dsi->vdds_dsi_reg)) {
		if (PTR_ERR(dsi->vdds_dsi_reg) != -EPROBE_DEFER)
			DSSERR("can't get DSI VDD regulator\n");
		return PTR_ERR(dsi->vdds_dsi_reg);
	}

	soc = soc_device_match(dsi_soc_devices);
	if (soc)
		dsi->data = soc->data;
	else
		dsi->data = of_match_node(dsi_of_match, dev->of_node)->data;

	d = dsi->data->modules;
	while (d->address != 0 && d->address != dsi_mem->start)
		d++;

	if (d->address == 0) {
		DSSERR("unsupported DSI module\n");
		return -ENODEV;
	}

	dsi->module_id = d->id;

	if (dsi->data->model == DSI_MODEL_OMAP4 ||
	    dsi->data->model == DSI_MODEL_OMAP5) {
		struct device_node *np;

		/*
		 * The OMAP4/5 display DT bindings don't reference the padconf
		 * syscon. Our only option to retrieve it is to find it by name.
		 */
		np = of_find_node_by_name(NULL,
			dsi->data->model == DSI_MODEL_OMAP4 ?
			"omap4_padconf_global" : "omap5_padconf_global");
		if (!np)
			return -ENODEV;

		dsi->syscon = syscon_node_to_regmap(np);
		of_node_put(np);
	}

	/* DSI VCs initialization */
	for (i = 0; i < ARRAY_SIZE(dsi->vc); i++)
		dsi->vc[i].source = DSI_VC_SOURCE_L4;

	r = dsi_get_clocks(dsi);
	if (r)
		return r;

	pm_runtime_enable(dev);

	/* DSI on OMAP3 doesn't have register DSI_GNQ, set number
	 * of data to 3 by default */
	if (dsi->data->quirks & DSI_QUIRK_GNQ) {
		dsi_runtime_get(dsi);
		/* NB_DATA_LANES */
		dsi->num_lanes_supported = 1 + REG_GET(dsi, DSI_GNQ, 11, 9);
		dsi_runtime_put(dsi);
	} else {
		dsi->num_lanes_supported = 3;
	}

	dsi->host.ops = &omap_dsi_host_ops;
	dsi->host.dev = &pdev->dev;

	r = dsi_probe_of(dsi);
	if (r) {
		DSSERR("Invalid DSI DT data\n");
		goto err_pm_disable;
	}

	r = mipi_dsi_host_register(&dsi->host);
	if (r < 0) {
		dev_err(&pdev->dev, "failed to register DSI host: %d\n", r);
		goto err_pm_disable;
	}

	r = dsi_init_output(dsi);
	if (r)
		goto err_dsi_host_unregister;

	r = component_add(&pdev->dev, &dsi_component_ops);
	if (r)
		goto err_uninit_output;

	return 0;

err_uninit_output:
	dsi_uninit_output(dsi);
err_dsi_host_unregister:
	mipi_dsi_host_unregister(&dsi->host);
err_pm_disable:
	pm_runtime_disable(dev);
	return r;
}

static int dsi_remove(struct platform_device *pdev)
{
	struct dsi_data *dsi = platform_get_drvdata(pdev);

	component_del(&pdev->dev, &dsi_component_ops);

	dsi_uninit_output(dsi);

	mipi_dsi_host_unregister(&dsi->host);

	pm_runtime_disable(&pdev->dev);

	if (dsi->vdds_dsi_reg != NULL && dsi->vdds_dsi_enabled) {
		regulator_disable(dsi->vdds_dsi_reg);
		dsi->vdds_dsi_enabled = false;
	}

	return 0;
}

static int dsi_runtime_suspend(struct device *dev)
{
	struct dsi_data *dsi = dev_get_drvdata(dev);

	dsi->is_enabled = false;
	/* ensure the irq handler sees the is_enabled value */
	smp_wmb();
	/* wait for current handler to finish before turning the DSI off */
	synchronize_irq(dsi->irq);

	return 0;
}

static int dsi_runtime_resume(struct device *dev)
{
	struct dsi_data *dsi = dev_get_drvdata(dev);

	dsi->is_enabled = true;
	/* ensure the irq handler sees the is_enabled value */
	smp_wmb();

	return 0;
}

static const struct dev_pm_ops dsi_pm_ops = {
	.runtime_suspend = dsi_runtime_suspend,
	.runtime_resume = dsi_runtime_resume,
	SET_LATE_SYSTEM_SLEEP_PM_OPS(pm_runtime_force_suspend, pm_runtime_force_resume)
};

struct platform_driver omap_dsihw_driver = {
	.probe		= dsi_probe,
	.remove		= dsi_remove,
	.driver         = {
		.name   = "omapdss_dsi",
		.pm	= &dsi_pm_ops,
		.of_match_table = dsi_of_match,
		.suppress_bind_attrs = true,
	},
};
