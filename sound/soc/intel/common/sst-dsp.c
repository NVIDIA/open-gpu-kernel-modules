// SPDX-License-Identifier: GPL-2.0-only
/*
 * Intel Smart Sound Technology (SST) DSP Core Driver
 *
 * Copyright (C) 2013, Intel Corporation. All rights reserved.
 */

#include <linux/slab.h>
#include <linux/export.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io-64-nonatomic-lo-hi.h>
#include <linux/delay.h>

#include "sst-dsp.h"
#include "sst-dsp-priv.h"

#define CREATE_TRACE_POINTS
#include <trace/events/intel-sst.h>

/* Internal generic low-level SST IO functions - can be overidden */
void sst_shim32_write(void __iomem *addr, u32 offset, u32 value)
{
	writel(value, addr + offset);
}
EXPORT_SYMBOL_GPL(sst_shim32_write);

u32 sst_shim32_read(void __iomem *addr, u32 offset)
{
	return readl(addr + offset);
}
EXPORT_SYMBOL_GPL(sst_shim32_read);

void sst_shim32_write64(void __iomem *addr, u32 offset, u64 value)
{
	writeq(value, addr + offset);
}
EXPORT_SYMBOL_GPL(sst_shim32_write64);

u64 sst_shim32_read64(void __iomem *addr, u32 offset)
{
	return readq(addr + offset);
}
EXPORT_SYMBOL_GPL(sst_shim32_read64);

/* Public API */
void sst_dsp_shim_write(struct sst_dsp *sst, u32 offset, u32 value)
{
	unsigned long flags;

	spin_lock_irqsave(&sst->spinlock, flags);
	sst->ops->write(sst->addr.shim, offset, value);
	spin_unlock_irqrestore(&sst->spinlock, flags);
}
EXPORT_SYMBOL_GPL(sst_dsp_shim_write);

u32 sst_dsp_shim_read(struct sst_dsp *sst, u32 offset)
{
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&sst->spinlock, flags);
	val = sst->ops->read(sst->addr.shim, offset);
	spin_unlock_irqrestore(&sst->spinlock, flags);

	return val;
}
EXPORT_SYMBOL_GPL(sst_dsp_shim_read);

void sst_dsp_shim_write_unlocked(struct sst_dsp *sst, u32 offset, u32 value)
{
	sst->ops->write(sst->addr.shim, offset, value);
}
EXPORT_SYMBOL_GPL(sst_dsp_shim_write_unlocked);

u32 sst_dsp_shim_read_unlocked(struct sst_dsp *sst, u32 offset)
{
	return sst->ops->read(sst->addr.shim, offset);
}
EXPORT_SYMBOL_GPL(sst_dsp_shim_read_unlocked);

int sst_dsp_shim_update_bits_unlocked(struct sst_dsp *sst, u32 offset,
				u32 mask, u32 value)
{
	bool change;
	unsigned int old, new;
	u32 ret;

	ret = sst_dsp_shim_read_unlocked(sst, offset);

	old = ret;
	new = (old & (~mask)) | (value & mask);

	change = (old != new);
	if (change)
		sst_dsp_shim_write_unlocked(sst, offset, new);

	return change;
}
EXPORT_SYMBOL_GPL(sst_dsp_shim_update_bits_unlocked);

/* This is for registers bits with attribute RWC */
void sst_dsp_shim_update_bits_forced_unlocked(struct sst_dsp *sst, u32 offset,
				u32 mask, u32 value)
{
	unsigned int old, new;
	u32 ret;

	ret = sst_dsp_shim_read_unlocked(sst, offset);

	old = ret;
	new = (old & (~mask)) | (value & mask);

	sst_dsp_shim_write_unlocked(sst, offset, new);
}
EXPORT_SYMBOL_GPL(sst_dsp_shim_update_bits_forced_unlocked);

int sst_dsp_shim_update_bits(struct sst_dsp *sst, u32 offset,
				u32 mask, u32 value)
{
	unsigned long flags;
	bool change;

	spin_lock_irqsave(&sst->spinlock, flags);
	change = sst_dsp_shim_update_bits_unlocked(sst, offset, mask, value);
	spin_unlock_irqrestore(&sst->spinlock, flags);
	return change;
}
EXPORT_SYMBOL_GPL(sst_dsp_shim_update_bits);

/* This is for registers bits with attribute RWC */
void sst_dsp_shim_update_bits_forced(struct sst_dsp *sst, u32 offset,
				u32 mask, u32 value)
{
	unsigned long flags;

	spin_lock_irqsave(&sst->spinlock, flags);
	sst_dsp_shim_update_bits_forced_unlocked(sst, offset, mask, value);
	spin_unlock_irqrestore(&sst->spinlock, flags);
}
EXPORT_SYMBOL_GPL(sst_dsp_shim_update_bits_forced);

int sst_dsp_register_poll(struct sst_dsp *ctx, u32 offset, u32 mask,
			 u32 target, u32 time, char *operation)
{
	u32 reg;
	unsigned long timeout;
	int k = 0, s = 500;

	/*
	 * split the loop into sleeps of varying resolution. more accurately,
	 * the range of wakeups are:
	 * Phase 1(first 5ms): min sleep 0.5ms; max sleep 1ms.
	 * Phase 2:( 5ms to 10ms) : min sleep 0.5ms; max sleep 10ms
	 * (usleep_range (500, 1000) and usleep_range(5000, 10000) are
	 * both possible in this phase depending on whether k > 10 or not).
	 * Phase 3: (beyond 10 ms) min sleep 5ms; max sleep 10ms.
	 */

	timeout = jiffies + msecs_to_jiffies(time);
	while ((((reg = sst_dsp_shim_read_unlocked(ctx, offset)) & mask) != target)
		&& time_before(jiffies, timeout)) {
		k++;
		if (k > 10)
			s = 5000;

		usleep_range(s, 2*s);
	}

	if ((reg & mask) == target) {
		dev_dbg(ctx->dev, "FW Poll Status: reg=%#x %s successful\n",
					reg, operation);

		return 0;
	}

	dev_dbg(ctx->dev, "FW Poll Status: reg=%#x %s timedout\n",
					reg, operation);
	return -ETIME;
}
EXPORT_SYMBOL_GPL(sst_dsp_register_poll);

int sst_dsp_mailbox_init(struct sst_dsp *sst, u32 inbox_offset, size_t inbox_size,
	u32 outbox_offset, size_t outbox_size)
{
	sst->mailbox.in_base = sst->addr.lpe + inbox_offset;
	sst->mailbox.out_base = sst->addr.lpe + outbox_offset;
	sst->mailbox.in_size = inbox_size;
	sst->mailbox.out_size = outbox_size;
	return 0;
}
EXPORT_SYMBOL_GPL(sst_dsp_mailbox_init);

void sst_dsp_outbox_write(struct sst_dsp *sst, void *message, size_t bytes)
{
	u32 i;

	trace_sst_ipc_outbox_write(bytes);

	memcpy_toio(sst->mailbox.out_base, message, bytes);

	for (i = 0; i < bytes; i += 4)
		trace_sst_ipc_outbox_wdata(i, *(u32 *)(message + i));
}
EXPORT_SYMBOL_GPL(sst_dsp_outbox_write);

void sst_dsp_outbox_read(struct sst_dsp *sst, void *message, size_t bytes)
{
	u32 i;

	trace_sst_ipc_outbox_read(bytes);

	memcpy_fromio(message, sst->mailbox.out_base, bytes);

	for (i = 0; i < bytes; i += 4)
		trace_sst_ipc_outbox_rdata(i, *(u32 *)(message + i));
}
EXPORT_SYMBOL_GPL(sst_dsp_outbox_read);

void sst_dsp_inbox_write(struct sst_dsp *sst, void *message, size_t bytes)
{
	u32 i;

	trace_sst_ipc_inbox_write(bytes);

	memcpy_toio(sst->mailbox.in_base, message, bytes);

	for (i = 0; i < bytes; i += 4)
		trace_sst_ipc_inbox_wdata(i, *(u32 *)(message + i));
}
EXPORT_SYMBOL_GPL(sst_dsp_inbox_write);

void sst_dsp_inbox_read(struct sst_dsp *sst, void *message, size_t bytes)
{
	u32 i;

	trace_sst_ipc_inbox_read(bytes);

	memcpy_fromio(message, sst->mailbox.in_base, bytes);

	for (i = 0; i < bytes; i += 4)
		trace_sst_ipc_inbox_rdata(i, *(u32 *)(message + i));
}
EXPORT_SYMBOL_GPL(sst_dsp_inbox_read);

/* Module information */
MODULE_AUTHOR("Liam Girdwood");
MODULE_DESCRIPTION("Intel SST Core");
MODULE_LICENSE("GPL v2");
