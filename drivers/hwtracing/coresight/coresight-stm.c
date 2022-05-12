// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
 *
 * Description: CoreSight System Trace Macrocell driver
 *
 * Initial implementation by Pratik Patel
 * (C) 2014-2015 Pratik Patel <pratikp@codeaurora.org>
 *
 * Serious refactoring, code cleanup and upgrading to the Coresight upstream
 * framework by Mathieu Poirier
 * (C) 2015-2016 Mathieu Poirier <mathieu.poirier@linaro.org>
 *
 * Guaranteed timing and support for various packet type coming from the
 * generic STM API by Chunyan Zhang
 * (C) 2015-2016 Chunyan Zhang <zhang.chunyan@linaro.org>
 */
#include <asm/local.h>
#include <linux/acpi.h>
#include <linux/amba/bus.h>
#include <linux/bitmap.h>
#include <linux/clk.h>
#include <linux/coresight.h>
#include <linux/coresight-stm.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/of_address.h>
#include <linux/perf_event.h>
#include <linux/pm_runtime.h>
#include <linux/stm.h>

#include "coresight-priv.h"

#define STMDMASTARTR			0xc04
#define STMDMASTOPR			0xc08
#define STMDMASTATR			0xc0c
#define STMDMACTLR			0xc10
#define STMDMAIDR			0xcfc
#define STMHEER				0xd00
#define STMHETER			0xd20
#define STMHEBSR			0xd60
#define STMHEMCR			0xd64
#define STMHEMASTR			0xdf4
#define STMHEFEAT1R			0xdf8
#define STMHEIDR			0xdfc
#define STMSPER				0xe00
#define STMSPTER			0xe20
#define STMPRIVMASKR			0xe40
#define STMSPSCR			0xe60
#define STMSPMSCR			0xe64
#define STMSPOVERRIDER			0xe68
#define STMSPMOVERRIDER			0xe6c
#define STMSPTRIGCSR			0xe70
#define STMTCSR				0xe80
#define STMTSSTIMR			0xe84
#define STMTSFREQR			0xe8c
#define STMSYNCR			0xe90
#define STMAUXCR			0xe94
#define STMSPFEAT1R			0xea0
#define STMSPFEAT2R			0xea4
#define STMSPFEAT3R			0xea8
#define STMITTRIGGER			0xee8
#define STMITATBDATA0			0xeec
#define STMITATBCTR2			0xef0
#define STMITATBID			0xef4
#define STMITATBCTR0			0xef8

#define STM_32_CHANNEL			32
#define BYTES_PER_CHANNEL		256
#define STM_TRACE_BUF_SIZE		4096
#define STM_SW_MASTER_END		127

/* Register bit definition */
#define STMTCSR_BUSY_BIT		23
/* Reserve the first 10 channels for kernel usage */
#define STM_CHANNEL_OFFSET		0

enum stm_pkt_type {
	STM_PKT_TYPE_DATA	= 0x98,
	STM_PKT_TYPE_FLAG	= 0xE8,
	STM_PKT_TYPE_TRIG	= 0xF8,
};

#define stm_channel_addr(drvdata, ch)	(drvdata->chs.base +	\
					(ch * BYTES_PER_CHANNEL))
#define stm_channel_off(type, opts)	(type & ~opts)

static int boot_nr_channel;

/*
 * Not really modular but using module_param is the easiest way to
 * remain consistent with existing use cases for now.
 */
module_param_named(
	boot_nr_channel, boot_nr_channel, int, S_IRUGO
);

/*
 * struct channel_space - central management entity for extended ports
 * @base:		memory mapped base address where channels start.
 * @phys:		physical base address of channel region.
 * @guaraneed:		is the channel delivery guaranteed.
 */
struct channel_space {
	void __iomem		*base;
	phys_addr_t		phys;
	unsigned long		*guaranteed;
};

DEFINE_CORESIGHT_DEVLIST(stm_devs, "stm");

/**
 * struct stm_drvdata - specifics associated to an STM component
 * @base:		memory mapped base address for this component.
 * @atclk:		optional clock for the core parts of the STM.
 * @csdev:		component vitals needed by the framework.
 * @spinlock:		only one at a time pls.
 * @chs:		the channels accociated to this STM.
 * @stm:		structure associated to the generic STM interface.
 * @mode:		this tracer's mode, i.e sysFS, or disabled.
 * @traceid:		value of the current ID for this component.
 * @write_bytes:	Maximus bytes this STM can write at a time.
 * @stmsper:		settings for register STMSPER.
 * @stmspscr:		settings for register STMSPSCR.
 * @numsp:		the total number of stimulus port support by this STM.
 * @stmheer:		settings for register STMHEER.
 * @stmheter:		settings for register STMHETER.
 * @stmhebsr:		settings for register STMHEBSR.
 */
struct stm_drvdata {
	void __iomem		*base;
	struct clk		*atclk;
	struct coresight_device	*csdev;
	spinlock_t		spinlock;
	struct channel_space	chs;
	struct stm_data		stm;
	local_t			mode;
	u8			traceid;
	u32			write_bytes;
	u32			stmsper;
	u32			stmspscr;
	u32			numsp;
	u32			stmheer;
	u32			stmheter;
	u32			stmhebsr;
};

static void stm_hwevent_enable_hw(struct stm_drvdata *drvdata)
{
	CS_UNLOCK(drvdata->base);

	writel_relaxed(drvdata->stmhebsr, drvdata->base + STMHEBSR);
	writel_relaxed(drvdata->stmheter, drvdata->base + STMHETER);
	writel_relaxed(drvdata->stmheer, drvdata->base + STMHEER);
	writel_relaxed(0x01 |	/* Enable HW event tracing */
		       0x04,	/* Error detection on event tracing */
		       drvdata->base + STMHEMCR);

	CS_LOCK(drvdata->base);
}

static void stm_port_enable_hw(struct stm_drvdata *drvdata)
{
	CS_UNLOCK(drvdata->base);
	/* ATB trigger enable on direct writes to TRIG locations */
	writel_relaxed(0x10,
		       drvdata->base + STMSPTRIGCSR);
	writel_relaxed(drvdata->stmspscr, drvdata->base + STMSPSCR);
	writel_relaxed(drvdata->stmsper, drvdata->base + STMSPER);

	CS_LOCK(drvdata->base);
}

static void stm_enable_hw(struct stm_drvdata *drvdata)
{
	if (drvdata->stmheer)
		stm_hwevent_enable_hw(drvdata);

	stm_port_enable_hw(drvdata);

	CS_UNLOCK(drvdata->base);

	/* 4096 byte between synchronisation packets */
	writel_relaxed(0xFFF, drvdata->base + STMSYNCR);
	writel_relaxed((drvdata->traceid << 16 | /* trace id */
			0x02 |			 /* timestamp enable */
			0x01),			 /* global STM enable */
			drvdata->base + STMTCSR);

	CS_LOCK(drvdata->base);
}

static int stm_enable(struct coresight_device *csdev,
		      struct perf_event *event, u32 mode)
{
	u32 val;
	struct stm_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	if (mode != CS_MODE_SYSFS)
		return -EINVAL;

	val = local_cmpxchg(&drvdata->mode, CS_MODE_DISABLED, mode);

	/* Someone is already using the tracer */
	if (val)
		return -EBUSY;

	pm_runtime_get_sync(csdev->dev.parent);

	spin_lock(&drvdata->spinlock);
	stm_enable_hw(drvdata);
	spin_unlock(&drvdata->spinlock);

	dev_dbg(&csdev->dev, "STM tracing enabled\n");
	return 0;
}

static void stm_hwevent_disable_hw(struct stm_drvdata *drvdata)
{
	CS_UNLOCK(drvdata->base);

	writel_relaxed(0x0, drvdata->base + STMHEMCR);
	writel_relaxed(0x0, drvdata->base + STMHEER);
	writel_relaxed(0x0, drvdata->base + STMHETER);

	CS_LOCK(drvdata->base);
}

static void stm_port_disable_hw(struct stm_drvdata *drvdata)
{
	CS_UNLOCK(drvdata->base);

	writel_relaxed(0x0, drvdata->base + STMSPER);
	writel_relaxed(0x0, drvdata->base + STMSPTRIGCSR);

	CS_LOCK(drvdata->base);
}

static void stm_disable_hw(struct stm_drvdata *drvdata)
{
	u32 val;

	CS_UNLOCK(drvdata->base);

	val = readl_relaxed(drvdata->base + STMTCSR);
	val &= ~0x1; /* clear global STM enable [0] */
	writel_relaxed(val, drvdata->base + STMTCSR);

	CS_LOCK(drvdata->base);

	stm_port_disable_hw(drvdata);
	if (drvdata->stmheer)
		stm_hwevent_disable_hw(drvdata);
}

static void stm_disable(struct coresight_device *csdev,
			struct perf_event *event)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);
	struct csdev_access *csa = &csdev->access;

	/*
	 * For as long as the tracer isn't disabled another entity can't
	 * change its status.  As such we can read the status here without
	 * fearing it will change under us.
	 */
	if (local_read(&drvdata->mode) == CS_MODE_SYSFS) {
		spin_lock(&drvdata->spinlock);
		stm_disable_hw(drvdata);
		spin_unlock(&drvdata->spinlock);

		/* Wait until the engine has completely stopped */
		coresight_timeout(csa, STMTCSR, STMTCSR_BUSY_BIT, 0);

		pm_runtime_put(csdev->dev.parent);

		local_set(&drvdata->mode, CS_MODE_DISABLED);
		dev_dbg(&csdev->dev, "STM tracing disabled\n");
	}
}

static int stm_trace_id(struct coresight_device *csdev)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	return drvdata->traceid;
}

static const struct coresight_ops_source stm_source_ops = {
	.trace_id	= stm_trace_id,
	.enable		= stm_enable,
	.disable	= stm_disable,
};

static const struct coresight_ops stm_cs_ops = {
	.source_ops	= &stm_source_ops,
};

static inline bool stm_addr_unaligned(const void *addr, u8 write_bytes)
{
	return ((unsigned long)addr & (write_bytes - 1));
}

static void stm_send(void __iomem *addr, const void *data,
		     u32 size, u8 write_bytes)
{
	u8 paload[8];

	if (stm_addr_unaligned(data, write_bytes)) {
		memcpy(paload, data, size);
		data = paload;
	}

	/* now we are 64bit/32bit aligned */
	switch (size) {
#ifdef CONFIG_64BIT
	case 8:
		writeq_relaxed(*(u64 *)data, addr);
		break;
#endif
	case 4:
		writel_relaxed(*(u32 *)data, addr);
		break;
	case 2:
		writew_relaxed(*(u16 *)data, addr);
		break;
	case 1:
		writeb_relaxed(*(u8 *)data, addr);
		break;
	default:
		break;
	}
}

static int stm_generic_link(struct stm_data *stm_data,
			    unsigned int master,  unsigned int channel)
{
	struct stm_drvdata *drvdata = container_of(stm_data,
						   struct stm_drvdata, stm);
	if (!drvdata || !drvdata->csdev)
		return -EINVAL;

	return coresight_enable(drvdata->csdev);
}

static void stm_generic_unlink(struct stm_data *stm_data,
			       unsigned int master,  unsigned int channel)
{
	struct stm_drvdata *drvdata = container_of(stm_data,
						   struct stm_drvdata, stm);
	if (!drvdata || !drvdata->csdev)
		return;

	coresight_disable(drvdata->csdev);
}

static phys_addr_t
stm_mmio_addr(struct stm_data *stm_data, unsigned int master,
	      unsigned int channel, unsigned int nr_chans)
{
	struct stm_drvdata *drvdata = container_of(stm_data,
						   struct stm_drvdata, stm);
	phys_addr_t addr;

	addr = drvdata->chs.phys + channel * BYTES_PER_CHANNEL;

	if (offset_in_page(addr) ||
	    offset_in_page(nr_chans * BYTES_PER_CHANNEL))
		return 0;

	return addr;
}

static long stm_generic_set_options(struct stm_data *stm_data,
				    unsigned int master,
				    unsigned int channel,
				    unsigned int nr_chans,
				    unsigned long options)
{
	struct stm_drvdata *drvdata = container_of(stm_data,
						   struct stm_drvdata, stm);
	if (!(drvdata && local_read(&drvdata->mode)))
		return -EINVAL;

	if (channel >= drvdata->numsp)
		return -EINVAL;

	switch (options) {
	case STM_OPTION_GUARANTEED:
		set_bit(channel, drvdata->chs.guaranteed);
		break;

	case STM_OPTION_INVARIANT:
		clear_bit(channel, drvdata->chs.guaranteed);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static ssize_t notrace stm_generic_packet(struct stm_data *stm_data,
				  unsigned int master,
				  unsigned int channel,
				  unsigned int packet,
				  unsigned int flags,
				  unsigned int size,
				  const unsigned char *payload)
{
	void __iomem *ch_addr;
	struct stm_drvdata *drvdata = container_of(stm_data,
						   struct stm_drvdata, stm);
	unsigned int stm_flags;

	if (!(drvdata && local_read(&drvdata->mode)))
		return -EACCES;

	if (channel >= drvdata->numsp)
		return -EINVAL;

	ch_addr = stm_channel_addr(drvdata, channel);

	stm_flags = (flags & STP_PACKET_TIMESTAMPED) ?
			STM_FLAG_TIMESTAMPED : 0;
	stm_flags |= test_bit(channel, drvdata->chs.guaranteed) ?
			   STM_FLAG_GUARANTEED : 0;

	if (size > drvdata->write_bytes)
		size = drvdata->write_bytes;
	else
		size = rounddown_pow_of_two(size);

	switch (packet) {
	case STP_PACKET_FLAG:
		ch_addr += stm_channel_off(STM_PKT_TYPE_FLAG, stm_flags);

		/*
		 * The generic STM core sets a size of '0' on flag packets.
		 * As such send a flag packet of size '1' and tell the
		 * core we did so.
		 */
		stm_send(ch_addr, payload, 1, drvdata->write_bytes);
		size = 1;
		break;

	case STP_PACKET_DATA:
		stm_flags |= (flags & STP_PACKET_MARKED) ? STM_FLAG_MARKED : 0;
		ch_addr += stm_channel_off(STM_PKT_TYPE_DATA, stm_flags);
		stm_send(ch_addr, payload, size,
				drvdata->write_bytes);
		break;

	default:
		return -ENOTSUPP;
	}

	return size;
}

static ssize_t hwevent_enable_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val = drvdata->stmheer;

	return scnprintf(buf, PAGE_SIZE, "%#lx\n", val);
}

static ssize_t hwevent_enable_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t size)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val;
	int ret = 0;

	ret = kstrtoul(buf, 16, &val);
	if (ret)
		return -EINVAL;

	drvdata->stmheer = val;
	/* HW event enable and trigger go hand in hand */
	drvdata->stmheter = val;

	return size;
}
static DEVICE_ATTR_RW(hwevent_enable);

static ssize_t hwevent_select_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val = drvdata->stmhebsr;

	return scnprintf(buf, PAGE_SIZE, "%#lx\n", val);
}

static ssize_t hwevent_select_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t size)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val;
	int ret = 0;

	ret = kstrtoul(buf, 16, &val);
	if (ret)
		return -EINVAL;

	drvdata->stmhebsr = val;

	return size;
}
static DEVICE_ATTR_RW(hwevent_select);

static ssize_t port_select_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val;

	if (!local_read(&drvdata->mode)) {
		val = drvdata->stmspscr;
	} else {
		spin_lock(&drvdata->spinlock);
		val = readl_relaxed(drvdata->base + STMSPSCR);
		spin_unlock(&drvdata->spinlock);
	}

	return scnprintf(buf, PAGE_SIZE, "%#lx\n", val);
}

static ssize_t port_select_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t size)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val, stmsper;
	int ret = 0;

	ret = kstrtoul(buf, 16, &val);
	if (ret)
		return ret;

	spin_lock(&drvdata->spinlock);
	drvdata->stmspscr = val;

	if (local_read(&drvdata->mode)) {
		CS_UNLOCK(drvdata->base);
		/* Process as per ARM's TRM recommendation */
		stmsper = readl_relaxed(drvdata->base + STMSPER);
		writel_relaxed(0x0, drvdata->base + STMSPER);
		writel_relaxed(drvdata->stmspscr, drvdata->base + STMSPSCR);
		writel_relaxed(stmsper, drvdata->base + STMSPER);
		CS_LOCK(drvdata->base);
	}
	spin_unlock(&drvdata->spinlock);

	return size;
}
static DEVICE_ATTR_RW(port_select);

static ssize_t port_enable_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val;

	if (!local_read(&drvdata->mode)) {
		val = drvdata->stmsper;
	} else {
		spin_lock(&drvdata->spinlock);
		val = readl_relaxed(drvdata->base + STMSPER);
		spin_unlock(&drvdata->spinlock);
	}

	return scnprintf(buf, PAGE_SIZE, "%#lx\n", val);
}

static ssize_t port_enable_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t size)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);
	unsigned long val;
	int ret = 0;

	ret = kstrtoul(buf, 16, &val);
	if (ret)
		return ret;

	spin_lock(&drvdata->spinlock);
	drvdata->stmsper = val;

	if (local_read(&drvdata->mode)) {
		CS_UNLOCK(drvdata->base);
		writel_relaxed(drvdata->stmsper, drvdata->base + STMSPER);
		CS_LOCK(drvdata->base);
	}
	spin_unlock(&drvdata->spinlock);

	return size;
}
static DEVICE_ATTR_RW(port_enable);

static ssize_t traceid_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	unsigned long val;
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);

	val = drvdata->traceid;
	return sprintf(buf, "%#lx\n", val);
}

static ssize_t traceid_store(struct device *dev,
			     struct device_attribute *attr,
			     const char *buf, size_t size)
{
	int ret;
	unsigned long val;
	struct stm_drvdata *drvdata = dev_get_drvdata(dev->parent);

	ret = kstrtoul(buf, 16, &val);
	if (ret)
		return ret;

	/* traceid field is 7bit wide on STM32 */
	drvdata->traceid = val & 0x7f;
	return size;
}
static DEVICE_ATTR_RW(traceid);

#define coresight_stm_reg(name, offset)	\
	coresight_simple_reg32(struct stm_drvdata, name, offset)

coresight_stm_reg(tcsr, STMTCSR);
coresight_stm_reg(tsfreqr, STMTSFREQR);
coresight_stm_reg(syncr, STMSYNCR);
coresight_stm_reg(sper, STMSPER);
coresight_stm_reg(spter, STMSPTER);
coresight_stm_reg(privmaskr, STMPRIVMASKR);
coresight_stm_reg(spscr, STMSPSCR);
coresight_stm_reg(spmscr, STMSPMSCR);
coresight_stm_reg(spfeat1r, STMSPFEAT1R);
coresight_stm_reg(spfeat2r, STMSPFEAT2R);
coresight_stm_reg(spfeat3r, STMSPFEAT3R);
coresight_stm_reg(devid, CORESIGHT_DEVID);

static struct attribute *coresight_stm_attrs[] = {
	&dev_attr_hwevent_enable.attr,
	&dev_attr_hwevent_select.attr,
	&dev_attr_port_enable.attr,
	&dev_attr_port_select.attr,
	&dev_attr_traceid.attr,
	NULL,
};

static struct attribute *coresight_stm_mgmt_attrs[] = {
	&dev_attr_tcsr.attr,
	&dev_attr_tsfreqr.attr,
	&dev_attr_syncr.attr,
	&dev_attr_sper.attr,
	&dev_attr_spter.attr,
	&dev_attr_privmaskr.attr,
	&dev_attr_spscr.attr,
	&dev_attr_spmscr.attr,
	&dev_attr_spfeat1r.attr,
	&dev_attr_spfeat2r.attr,
	&dev_attr_spfeat3r.attr,
	&dev_attr_devid.attr,
	NULL,
};

static const struct attribute_group coresight_stm_group = {
	.attrs = coresight_stm_attrs,
};

static const struct attribute_group coresight_stm_mgmt_group = {
	.attrs = coresight_stm_mgmt_attrs,
	.name = "mgmt",
};

static const struct attribute_group *coresight_stm_groups[] = {
	&coresight_stm_group,
	&coresight_stm_mgmt_group,
	NULL,
};

#ifdef CONFIG_OF
static int of_stm_get_stimulus_area(struct device *dev, struct resource *res)
{
	const char *name = NULL;
	int index = 0, found = 0;
	struct device_node *np = dev->of_node;

	while (!of_property_read_string_index(np, "reg-names", index, &name)) {
		if (strcmp("stm-stimulus-base", name)) {
			index++;
			continue;
		}

		/* We have a match and @index is where it's at */
		found = 1;
		break;
	}

	if (!found)
		return -EINVAL;

	return of_address_to_resource(np, index, res);
}
#else
static inline int of_stm_get_stimulus_area(struct device *dev,
					   struct resource *res)
{
	return -ENOENT;
}
#endif

#ifdef CONFIG_ACPI
static int acpi_stm_get_stimulus_area(struct device *dev, struct resource *res)
{
	int rc;
	bool found_base = false;
	struct resource_entry *rent;
	LIST_HEAD(res_list);

	struct acpi_device *adev = ACPI_COMPANION(dev);

	rc = acpi_dev_get_resources(adev, &res_list, NULL, NULL);
	if (rc < 0)
		return rc;

	/*
	 * The stimulus base for STM device must be listed as the second memory
	 * resource, followed by the programming base address as described in
	 * "Section 2.3 Resources" in ACPI for CoreSightTM 1.0 Platform Design
	 * document (DEN0067).
	 */
	rc = -ENOENT;
	list_for_each_entry(rent, &res_list, node) {
		if (resource_type(rent->res) != IORESOURCE_MEM)
			continue;
		if (found_base) {
			*res = *rent->res;
			rc = 0;
			break;
		}

		found_base = true;
	}

	acpi_dev_free_resource_list(&res_list);
	return rc;
}
#else
static inline int acpi_stm_get_stimulus_area(struct device *dev,
					     struct resource *res)
{
	return -ENOENT;
}
#endif

static int stm_get_stimulus_area(struct device *dev, struct resource *res)
{
	struct fwnode_handle *fwnode = dev_fwnode(dev);

	if (is_of_node(fwnode))
		return of_stm_get_stimulus_area(dev, res);
	else if (is_acpi_node(fwnode))
		return acpi_stm_get_stimulus_area(dev, res);
	return -ENOENT;
}

static u32 stm_fundamental_data_size(struct stm_drvdata *drvdata)
{
	u32 stmspfeat2r;

	if (!IS_ENABLED(CONFIG_64BIT))
		return 4;

	stmspfeat2r = readl_relaxed(drvdata->base + STMSPFEAT2R);

	/*
	 * bit[15:12] represents the fundamental data size
	 * 0 - 32-bit data
	 * 1 - 64-bit data
	 */
	return BMVAL(stmspfeat2r, 12, 15) ? 8 : 4;
}

static u32 stm_num_stimulus_port(struct stm_drvdata *drvdata)
{
	u32 numsp;

	numsp = readl_relaxed(drvdata->base + CORESIGHT_DEVID);
	/*
	 * NUMPS in STMDEVID is 17 bit long and if equal to 0x0,
	 * 32 stimulus ports are supported.
	 */
	numsp &= 0x1ffff;
	if (!numsp)
		numsp = STM_32_CHANNEL;
	return numsp;
}

static void stm_init_default_data(struct stm_drvdata *drvdata)
{
	/* Don't use port selection */
	drvdata->stmspscr = 0x0;
	/*
	 * Enable all channel regardless of their number.  When port
	 * selection isn't used (see above) STMSPER applies to all
	 * 32 channel group available, hence setting all 32 bits to 1
	 */
	drvdata->stmsper = ~0x0;

	/*
	 * The trace ID value for *ETM* tracers start at CPU_ID * 2 + 0x10 and
	 * anything equal to or higher than 0x70 is reserved.  Since 0x00 is
	 * also reserved the STM trace ID needs to be higher than 0x00 and
	 * lowner than 0x10.
	 */
	drvdata->traceid = 0x1;

	/* Set invariant transaction timing on all channels */
	bitmap_clear(drvdata->chs.guaranteed, 0, drvdata->numsp);
}

static void stm_init_generic_data(struct stm_drvdata *drvdata,
				  const char *name)
{
	drvdata->stm.name = name;

	/*
	 * MasterIDs are assigned at HW design phase. As such the core is
	 * using a single master for interaction with this device.
	 */
	drvdata->stm.sw_start = 1;
	drvdata->stm.sw_end = 1;
	drvdata->stm.hw_override = true;
	drvdata->stm.sw_nchannels = drvdata->numsp;
	drvdata->stm.sw_mmiosz = BYTES_PER_CHANNEL;
	drvdata->stm.packet = stm_generic_packet;
	drvdata->stm.mmio_addr = stm_mmio_addr;
	drvdata->stm.link = stm_generic_link;
	drvdata->stm.unlink = stm_generic_unlink;
	drvdata->stm.set_options = stm_generic_set_options;
}

static int stm_probe(struct amba_device *adev, const struct amba_id *id)
{
	int ret;
	void __iomem *base;
	unsigned long *guaranteed;
	struct device *dev = &adev->dev;
	struct coresight_platform_data *pdata = NULL;
	struct stm_drvdata *drvdata;
	struct resource *res = &adev->res;
	struct resource ch_res;
	size_t bitmap_size;
	struct coresight_desc desc = { 0 };

	desc.name = coresight_alloc_device_name(&stm_devs, dev);
	if (!desc.name)
		return -ENOMEM;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->atclk = devm_clk_get(&adev->dev, "atclk"); /* optional */
	if (!IS_ERR(drvdata->atclk)) {
		ret = clk_prepare_enable(drvdata->atclk);
		if (ret)
			return ret;
	}
	dev_set_drvdata(dev, drvdata);

	base = devm_ioremap_resource(dev, res);
	if (IS_ERR(base))
		return PTR_ERR(base);
	drvdata->base = base;
	desc.access = CSDEV_ACCESS_IOMEM(base);

	ret = stm_get_stimulus_area(dev, &ch_res);
	if (ret)
		return ret;
	drvdata->chs.phys = ch_res.start;

	base = devm_ioremap_resource(dev, &ch_res);
	if (IS_ERR(base))
		return PTR_ERR(base);
	drvdata->chs.base = base;

	drvdata->write_bytes = stm_fundamental_data_size(drvdata);

	if (boot_nr_channel)
		drvdata->numsp = boot_nr_channel;
	else
		drvdata->numsp = stm_num_stimulus_port(drvdata);

	bitmap_size = BITS_TO_LONGS(drvdata->numsp) * sizeof(long);

	guaranteed = devm_kzalloc(dev, bitmap_size, GFP_KERNEL);
	if (!guaranteed)
		return -ENOMEM;
	drvdata->chs.guaranteed = guaranteed;

	spin_lock_init(&drvdata->spinlock);

	stm_init_default_data(drvdata);
	stm_init_generic_data(drvdata, desc.name);

	if (stm_register_device(dev, &drvdata->stm, THIS_MODULE)) {
		dev_info(dev,
			 "%s : stm_register_device failed, probing deferred\n",
			 desc.name);
		return -EPROBE_DEFER;
	}

	pdata = coresight_get_platform_data(dev);
	if (IS_ERR(pdata)) {
		ret = PTR_ERR(pdata);
		goto stm_unregister;
	}
	adev->dev.platform_data = pdata;

	desc.type = CORESIGHT_DEV_TYPE_SOURCE;
	desc.subtype.source_subtype = CORESIGHT_DEV_SUBTYPE_SOURCE_SOFTWARE;
	desc.ops = &stm_cs_ops;
	desc.pdata = pdata;
	desc.dev = dev;
	desc.groups = coresight_stm_groups;
	drvdata->csdev = coresight_register(&desc);
	if (IS_ERR(drvdata->csdev)) {
		ret = PTR_ERR(drvdata->csdev);
		goto stm_unregister;
	}

	pm_runtime_put(&adev->dev);

	dev_info(&drvdata->csdev->dev, "%s initialized\n",
		 (char *)coresight_get_uci_data(id));
	return 0;

stm_unregister:
	stm_unregister_device(&drvdata->stm);
	return ret;
}

static void stm_remove(struct amba_device *adev)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(&adev->dev);

	coresight_unregister(drvdata->csdev);

	stm_unregister_device(&drvdata->stm);
}

#ifdef CONFIG_PM
static int stm_runtime_suspend(struct device *dev)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev);

	if (drvdata && !IS_ERR(drvdata->atclk))
		clk_disable_unprepare(drvdata->atclk);

	return 0;
}

static int stm_runtime_resume(struct device *dev)
{
	struct stm_drvdata *drvdata = dev_get_drvdata(dev);

	if (drvdata && !IS_ERR(drvdata->atclk))
		clk_prepare_enable(drvdata->atclk);

	return 0;
}
#endif

static const struct dev_pm_ops stm_dev_pm_ops = {
	SET_RUNTIME_PM_OPS(stm_runtime_suspend, stm_runtime_resume, NULL)
};

static const struct amba_id stm_ids[] = {
	CS_AMBA_ID_DATA(0x000bb962, "STM32"),
	CS_AMBA_ID_DATA(0x000bb963, "STM500"),
	{ 0, 0},
};

MODULE_DEVICE_TABLE(amba, stm_ids);

static struct amba_driver stm_driver = {
	.drv = {
		.name   = "coresight-stm",
		.owner	= THIS_MODULE,
		.pm	= &stm_dev_pm_ops,
		.suppress_bind_attrs = true,
	},
	.probe          = stm_probe,
	.remove         = stm_remove,
	.id_table	= stm_ids,
};

module_amba_driver(stm_driver);

MODULE_AUTHOR("Pratik Patel <pratikp@codeaurora.org>");
MODULE_DESCRIPTION("Arm CoreSight System Trace Macrocell driver");
MODULE_LICENSE("GPL v2");
