// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	Adaptec AAC series RAID controller driver
 *	(c) Copyright 2001 Red Hat Inc.
 *
 * based on the old aacraid driver that is..
 * Adaptec aacraid device driver for Linux.
 *
 * Copyright (c) 2000-2010 Adaptec, Inc.
 *               2010-2015 PMC-Sierra, Inc. (aacraid@pmc-sierra.com)
 *		 2016-2017 Microsemi Corp. (aacraid@microsemi.com)
 *
 * Module Name:
 *  rx.c
 *
 * Abstract: Hardware miniport for Drawbridge specific hardware functions.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/spinlock.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/completion.h>
#include <linux/time.h>
#include <linux/interrupt.h>

#include <scsi/scsi_host.h>

#include "aacraid.h"

static irqreturn_t aac_rx_intr_producer(int irq, void *dev_id)
{
	struct aac_dev *dev = dev_id;
	unsigned long bellbits;
	u8 intstat = rx_readb(dev, MUnit.OISR);

	/*
	 *	Read mask and invert because drawbridge is reversed.
	 *	This allows us to only service interrupts that have
	 *	been enabled.
	 *	Check to see if this is our interrupt.  If it isn't just return
	 */
	if (likely(intstat & ~(dev->OIMR))) {
		bellbits = rx_readl(dev, OutboundDoorbellReg);
		if (unlikely(bellbits & DoorBellPrintfReady)) {
			aac_printf(dev, readl (&dev->IndexRegs->Mailbox[5]));
			rx_writel(dev, MUnit.ODR,DoorBellPrintfReady);
			rx_writel(dev, InboundDoorbellReg,DoorBellPrintfDone);
		}
		else if (unlikely(bellbits & DoorBellAdapterNormCmdReady)) {
			rx_writel(dev, MUnit.ODR, DoorBellAdapterNormCmdReady);
			aac_command_normal(&dev->queues->queue[HostNormCmdQueue]);
		}
		else if (likely(bellbits & DoorBellAdapterNormRespReady)) {
			rx_writel(dev, MUnit.ODR,DoorBellAdapterNormRespReady);
			aac_response_normal(&dev->queues->queue[HostNormRespQueue]);
		}
		else if (unlikely(bellbits & DoorBellAdapterNormCmdNotFull)) {
			rx_writel(dev, MUnit.ODR, DoorBellAdapterNormCmdNotFull);
		}
		else if (unlikely(bellbits & DoorBellAdapterNormRespNotFull)) {
			rx_writel(dev, MUnit.ODR, DoorBellAdapterNormCmdNotFull);
			rx_writel(dev, MUnit.ODR, DoorBellAdapterNormRespNotFull);
		}
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static irqreturn_t aac_rx_intr_message(int irq, void *dev_id)
{
	int isAif, isFastResponse, isSpecial;
	struct aac_dev *dev = dev_id;
	u32 Index = rx_readl(dev, MUnit.OutboundQueue);
	if (unlikely(Index == 0xFFFFFFFFL))
		Index = rx_readl(dev, MUnit.OutboundQueue);
	if (likely(Index != 0xFFFFFFFFL)) {
		do {
			isAif = isFastResponse = isSpecial = 0;
			if (Index & 0x00000002L) {
				isAif = 1;
				if (Index == 0xFFFFFFFEL)
					isSpecial = 1;
				Index &= ~0x00000002L;
			} else {
				if (Index & 0x00000001L)
					isFastResponse = 1;
				Index >>= 2;
			}
			if (!isSpecial) {
				if (unlikely(aac_intr_normal(dev,
						Index, isAif,
						isFastResponse, NULL))) {
					rx_writel(dev,
						MUnit.OutboundQueue,
						Index);
					rx_writel(dev,
						MUnit.ODR,
						DoorBellAdapterNormRespReady);
				}
			}
			Index = rx_readl(dev, MUnit.OutboundQueue);
		} while (Index != 0xFFFFFFFFL);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

/**
 *	aac_rx_disable_interrupt	-	Disable interrupts
 *	@dev: Adapter
 */

static void aac_rx_disable_interrupt(struct aac_dev *dev)
{
	rx_writeb(dev, MUnit.OIMR, dev->OIMR = 0xff);
}

/**
 *	aac_rx_enable_interrupt_producer	-	Enable interrupts
 *	@dev: Adapter
 */

static void aac_rx_enable_interrupt_producer(struct aac_dev *dev)
{
	rx_writeb(dev, MUnit.OIMR, dev->OIMR = 0xfb);
}

/**
 *	aac_rx_enable_interrupt_message	-	Enable interrupts
 *	@dev: Adapter
 */

static void aac_rx_enable_interrupt_message(struct aac_dev *dev)
{
	rx_writeb(dev, MUnit.OIMR, dev->OIMR = 0xf7);
}

/**
 *	rx_sync_cmd	-	send a command and wait
 *	@dev: Adapter
 *	@command: Command to execute
 *	@p1: first parameter
 *	@p2: second parameter
 *	@p3: third parameter
 *	@p4: forth parameter
 *	@p5: fifth parameter
 *	@p6: sixth parameter
 *	@status: adapter status
 *	@r1: first return value
 *	@r2: second return value
 *	@r3: third return value
 *	@r4: forth return value
 *
 *	This routine will send a synchronous command to the adapter and wait 
 *	for its	completion.
 */

static int rx_sync_cmd(struct aac_dev *dev, u32 command,
	u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6,
	u32 *status, u32 * r1, u32 * r2, u32 * r3, u32 * r4)
{
	unsigned long start;
	int ok;
	/*
	 *	Write the command into Mailbox 0
	 */
	writel(command, &dev->IndexRegs->Mailbox[0]);
	/*
	 *	Write the parameters into Mailboxes 1 - 6
	 */
	writel(p1, &dev->IndexRegs->Mailbox[1]);
	writel(p2, &dev->IndexRegs->Mailbox[2]);
	writel(p3, &dev->IndexRegs->Mailbox[3]);
	writel(p4, &dev->IndexRegs->Mailbox[4]);
	/*
	 *	Clear the synch command doorbell to start on a clean slate.
	 */
	rx_writel(dev, OutboundDoorbellReg, OUTBOUNDDOORBELL_0);
	/*
	 *	Disable doorbell interrupts
	 */
	rx_writeb(dev, MUnit.OIMR, dev->OIMR = 0xff);
	/*
	 *	Force the completion of the mask register write before issuing
	 *	the interrupt.
	 */
	rx_readb (dev, MUnit.OIMR);
	/*
	 *	Signal that there is a new synch command
	 */
	rx_writel(dev, InboundDoorbellReg, INBOUNDDOORBELL_0);

	ok = 0;
	start = jiffies;

	/*
	 *	Wait up to 30 seconds
	 */
	while (time_before(jiffies, start+30*HZ)) 
	{
		udelay(5);	/* Delay 5 microseconds to let Mon960 get info. */
		/*
		 *	Mon960 will set doorbell0 bit when it has completed the command.
		 */
		if (rx_readl(dev, OutboundDoorbellReg) & OUTBOUNDDOORBELL_0) {
			/*
			 *	Clear the doorbell.
			 */
			rx_writel(dev, OutboundDoorbellReg, OUTBOUNDDOORBELL_0);
			ok = 1;
			break;
		}
		/*
		 *	Yield the processor in case we are slow 
		 */
		msleep(1);
	}
	if (unlikely(ok != 1)) {
		/*
		 *	Restore interrupt mask even though we timed out
		 */
		aac_adapter_enable_int(dev);
		return -ETIMEDOUT;
	}
	/*
	 *	Pull the synch status from Mailbox 0.
	 */
	if (status)
		*status = readl(&dev->IndexRegs->Mailbox[0]);
	if (r1)
		*r1 = readl(&dev->IndexRegs->Mailbox[1]);
	if (r2)
		*r2 = readl(&dev->IndexRegs->Mailbox[2]);
	if (r3)
		*r3 = readl(&dev->IndexRegs->Mailbox[3]);
	if (r4)
		*r4 = readl(&dev->IndexRegs->Mailbox[4]);
	/*
	 *	Clear the synch command doorbell.
	 */
	rx_writel(dev, OutboundDoorbellReg, OUTBOUNDDOORBELL_0);
	/*
	 *	Restore interrupt mask
	 */
	aac_adapter_enable_int(dev);
	return 0;

}

/**
 *	aac_rx_interrupt_adapter	-	interrupt adapter
 *	@dev: Adapter
 *
 *	Send an interrupt to the i960 and breakpoint it.
 */

static void aac_rx_interrupt_adapter(struct aac_dev *dev)
{
	rx_sync_cmd(dev, BREAKPOINT_REQUEST, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL);
}

/**
 *	aac_rx_notify_adapter		-	send an event to the adapter
 *	@dev: Adapter
 *	@event: Event to send
 *
 *	Notify the i960 that something it probably cares about has
 *	happened.
 */

static void aac_rx_notify_adapter(struct aac_dev *dev, u32 event)
{
	switch (event) {

	case AdapNormCmdQue:
		rx_writel(dev, MUnit.IDR,INBOUNDDOORBELL_1);
		break;
	case HostNormRespNotFull:
		rx_writel(dev, MUnit.IDR,INBOUNDDOORBELL_4);
		break;
	case AdapNormRespQue:
		rx_writel(dev, MUnit.IDR,INBOUNDDOORBELL_2);
		break;
	case HostNormCmdNotFull:
		rx_writel(dev, MUnit.IDR,INBOUNDDOORBELL_3);
		break;
	case HostShutdown:
		break;
	case FastIo:
		rx_writel(dev, MUnit.IDR,INBOUNDDOORBELL_6);
		break;
	case AdapPrintfDone:
		rx_writel(dev, MUnit.IDR,INBOUNDDOORBELL_5);
		break;
	default:
		BUG();
		break;
	}
}

/**
 *	aac_rx_start_adapter		-	activate adapter
 *	@dev:	Adapter
 *
 *	Start up processing on an i960 based AAC adapter
 */

static void aac_rx_start_adapter(struct aac_dev *dev)
{
	union aac_init *init;

	init = dev->init;
	init->r7.host_elapsed_seconds = cpu_to_le32(ktime_get_real_seconds());
	// We can only use a 32 bit address here
	rx_sync_cmd(dev, INIT_STRUCT_BASE_ADDRESS, (u32)(ulong)dev->init_pa,
	  0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL);
}

/**
 *	aac_rx_check_health
 *	@dev: device to check if healthy
 *
 *	Will attempt to determine if the specified adapter is alive and
 *	capable of handling requests, returning 0 if alive.
 */
static int aac_rx_check_health(struct aac_dev *dev)
{
	u32 status = rx_readl(dev, MUnit.OMRx[0]);

	/*
	 *	Check to see if the board failed any self tests.
	 */
	if (unlikely(status & SELF_TEST_FAILED))
		return -1;
	/*
	 *	Check to see if the board panic'd.
	 */
	if (unlikely(status & KERNEL_PANIC)) {
		char * buffer;
		struct POSTSTATUS {
			__le32 Post_Command;
			__le32 Post_Address;
		} * post;
		dma_addr_t paddr, baddr;
		int ret;

		if (likely((status & 0xFF000000L) == 0xBC000000L))
			return (status >> 16) & 0xFF;
		buffer = dma_alloc_coherent(&dev->pdev->dev, 512, &baddr,
					    GFP_KERNEL);
		ret = -2;
		if (unlikely(buffer == NULL))
			return ret;
		post = dma_alloc_coherent(&dev->pdev->dev,
					  sizeof(struct POSTSTATUS), &paddr,
					  GFP_KERNEL);
		if (unlikely(post == NULL)) {
			dma_free_coherent(&dev->pdev->dev, 512, buffer, baddr);
			return ret;
		}
		memset(buffer, 0, 512);
		post->Post_Command = cpu_to_le32(COMMAND_POST_RESULTS);
		post->Post_Address = cpu_to_le32(baddr);
		rx_writel(dev, MUnit.IMRx[0], paddr);
		rx_sync_cmd(dev, COMMAND_POST_RESULTS, baddr, 0, 0, 0, 0, 0,
		  NULL, NULL, NULL, NULL, NULL);
		dma_free_coherent(&dev->pdev->dev, sizeof(struct POSTSTATUS),
				  post, paddr);
		if (likely((buffer[0] == '0') && ((buffer[1] == 'x') || (buffer[1] == 'X')))) {
			ret = (hex_to_bin(buffer[2]) << 4) +
				hex_to_bin(buffer[3]);
		}
		dma_free_coherent(&dev->pdev->dev, 512, buffer, baddr);
		return ret;
	}
	/*
	 *	Wait for the adapter to be up and running.
	 */
	if (unlikely(!(status & KERNEL_UP_AND_RUNNING)))
		return -3;
	/*
	 *	Everything is OK
	 */
	return 0;
}

/**
 *	aac_rx_deliver_producer
 *	@fib: fib to issue
 *
 *	Will send a fib, returning 0 if successful.
 */
int aac_rx_deliver_producer(struct fib * fib)
{
	struct aac_dev *dev = fib->dev;
	struct aac_queue *q = &dev->queues->queue[AdapNormCmdQueue];
	u32 Index;
	unsigned long nointr = 0;

	aac_queue_get( dev, &Index, AdapNormCmdQueue, fib->hw_fib_va, 1, fib, &nointr);

	atomic_inc(&q->numpending);
	*(q->headers.producer) = cpu_to_le32(Index + 1);
	if (!(nointr & aac_config.irq_mod))
		aac_adapter_notify(dev, AdapNormCmdQueue);

	return 0;
}

/**
 *	aac_rx_deliver_message
 *	@fib: fib to issue
 *
 *	Will send a fib, returning 0 if successful.
 */
static int aac_rx_deliver_message(struct fib * fib)
{
	struct aac_dev *dev = fib->dev;
	struct aac_queue *q = &dev->queues->queue[AdapNormCmdQueue];
	u32 Index;
	u64 addr;
	volatile void __iomem *device;

	unsigned long count = 10000000L; /* 50 seconds */
	atomic_inc(&q->numpending);
	for(;;) {
		Index = rx_readl(dev, MUnit.InboundQueue);
		if (unlikely(Index == 0xFFFFFFFFL))
			Index = rx_readl(dev, MUnit.InboundQueue);
		if (likely(Index != 0xFFFFFFFFL))
			break;
		if (--count == 0) {
			atomic_dec(&q->numpending);
			return -ETIMEDOUT;
		}
		udelay(5);
	}
	device = dev->base + Index;
	addr = fib->hw_fib_pa;
	writel((u32)(addr & 0xffffffff), device);
	device += sizeof(u32);
	writel((u32)(addr >> 32), device);
	device += sizeof(u32);
	writel(le16_to_cpu(fib->hw_fib_va->header.Size), device);
	rx_writel(dev, MUnit.InboundQueue, Index);
	return 0;
}

/**
 *	aac_rx_ioremap
 *	@dev: adapter
 *	@size: mapping resize request
 *
 */
static int aac_rx_ioremap(struct aac_dev * dev, u32 size)
{
	if (!size) {
		iounmap(dev->regs.rx);
		return 0;
	}
	dev->base = dev->regs.rx = ioremap(dev->base_start, size);
	if (dev->base == NULL)
		return -1;
	dev->IndexRegs = &dev->regs.rx->IndexRegs;
	return 0;
}

static int aac_rx_restart_adapter(struct aac_dev *dev, int bled, u8 reset_type)
{
	u32 var = 0;

	if (!(dev->supplement_adapter_info.supported_options2 &
	  AAC_OPTION_MU_RESET) || (bled >= 0) || (bled == -2)) {
		if (bled)
			printk(KERN_ERR "%s%d: adapter kernel panic'd %x.\n",
				dev->name, dev->id, bled);
		else {
			bled = aac_adapter_sync_cmd(dev, IOP_RESET_ALWAYS,
			  0, 0, 0, 0, 0, 0, &var, NULL, NULL, NULL, NULL);
			if (!bled && (var != 0x00000001) && (var != 0x3803000F))
				bled = -EINVAL;
		}
		if (bled && (bled != -ETIMEDOUT))
			bled = aac_adapter_sync_cmd(dev, IOP_RESET,
			  0, 0, 0, 0, 0, 0, &var, NULL, NULL, NULL, NULL);

		if (bled && (bled != -ETIMEDOUT))
			return -EINVAL;
	}
	if (bled && (var == 0x3803000F)) { /* USE_OTHER_METHOD */
		rx_writel(dev, MUnit.reserved2, 3);
		msleep(5000); /* Delay 5 seconds */
		var = 0x00000001;
	}
	if (bled && (var != 0x00000001))
		return -EINVAL;
	ssleep(5);
	if (rx_readl(dev, MUnit.OMRx[0]) & KERNEL_PANIC)
		return -ENODEV;
	if (startup_timeout < 300)
		startup_timeout = 300;
	return 0;
}

/**
 *	aac_rx_select_comm	-	Select communications method
 *	@dev: Adapter
 *	@comm: communications method
 */

int aac_rx_select_comm(struct aac_dev *dev, int comm)
{
	switch (comm) {
	case AAC_COMM_PRODUCER:
		dev->a_ops.adapter_enable_int = aac_rx_enable_interrupt_producer;
		dev->a_ops.adapter_intr = aac_rx_intr_producer;
		dev->a_ops.adapter_deliver = aac_rx_deliver_producer;
		break;
	case AAC_COMM_MESSAGE:
		dev->a_ops.adapter_enable_int = aac_rx_enable_interrupt_message;
		dev->a_ops.adapter_intr = aac_rx_intr_message;
		dev->a_ops.adapter_deliver = aac_rx_deliver_message;
		break;
	default:
		return 1;
	}
	return 0;
}

/**
 *	_aac_rx_init	-	initialize an i960 based AAC card
 *	@dev: device to configure
 *
 *	Allocate and set up resources for the i960 based AAC variants. The 
 *	device_interface in the commregion will be allocated and linked 
 *	to the comm region.
 */

int _aac_rx_init(struct aac_dev *dev)
{
	unsigned long start;
	unsigned long status;
	int restart = 0;
	int instance = dev->id;
	const char * name = dev->name;

	if (aac_adapter_ioremap(dev, dev->base_size)) {
		printk(KERN_WARNING "%s: unable to map adapter.\n", name);
		goto error_iounmap;
	}

	/* Failure to reset here is an option ... */
	dev->a_ops.adapter_sync_cmd = rx_sync_cmd;
	dev->a_ops.adapter_enable_int = aac_rx_disable_interrupt;
	dev->OIMR = status = rx_readb (dev, MUnit.OIMR);

	if (((status & 0x0c) != 0x0c) || dev->init_reset) {
		dev->init_reset = false;
		if (!aac_rx_restart_adapter(dev, 0, IOP_HWSOFT_RESET)) {
			/* Make sure the Hardware FIFO is empty */
			while ((++restart < 512) &&
			       (rx_readl(dev, MUnit.OutboundQueue) != 0xFFFFFFFFL));
		}
	}

	/*
	 *	Check to see if the board panic'd while booting.
	 */
	status = rx_readl(dev, MUnit.OMRx[0]);
	if (status & KERNEL_PANIC) {
		if (aac_rx_restart_adapter(dev,
			aac_rx_check_health(dev), IOP_HWSOFT_RESET))
			goto error_iounmap;
		++restart;
	}
	/*
	 *	Check to see if the board failed any self tests.
	 */
	status = rx_readl(dev, MUnit.OMRx[0]);
	if (status & SELF_TEST_FAILED) {
		printk(KERN_ERR "%s%d: adapter self-test failed.\n", dev->name, instance);
		goto error_iounmap;
	}
	/*
	 *	Check to see if the monitor panic'd while booting.
	 */
	if (status & MONITOR_PANIC) {
		printk(KERN_ERR "%s%d: adapter monitor panic.\n", dev->name, instance);
		goto error_iounmap;
	}
	start = jiffies;
	/*
	 *	Wait for the adapter to be up and running. Wait up to 3 minutes
	 */
	while (!((status = rx_readl(dev, MUnit.OMRx[0])) & KERNEL_UP_AND_RUNNING))
	{
		if ((restart &&
		  (status & (KERNEL_PANIC|SELF_TEST_FAILED|MONITOR_PANIC))) ||
		  time_after(jiffies, start+HZ*startup_timeout)) {
			printk(KERN_ERR "%s%d: adapter kernel failed to start, init status = %lx.\n", 
					dev->name, instance, status);
			goto error_iounmap;
		}
		if (!restart &&
		  ((status & (KERNEL_PANIC|SELF_TEST_FAILED|MONITOR_PANIC)) ||
		  time_after(jiffies, start + HZ *
		  ((startup_timeout > 60)
		    ? (startup_timeout - 60)
		    : (startup_timeout / 2))))) {
			if (likely(!aac_rx_restart_adapter(dev,
				aac_rx_check_health(dev), IOP_HWSOFT_RESET)))
				start = jiffies;
			++restart;
		}
		msleep(1);
	}
	if (restart && aac_commit)
		aac_commit = 1;
	/*
	 *	Fill in the common function dispatch table.
	 */
	dev->a_ops.adapter_interrupt = aac_rx_interrupt_adapter;
	dev->a_ops.adapter_disable_int = aac_rx_disable_interrupt;
	dev->a_ops.adapter_notify = aac_rx_notify_adapter;
	dev->a_ops.adapter_sync_cmd = rx_sync_cmd;
	dev->a_ops.adapter_check_health = aac_rx_check_health;
	dev->a_ops.adapter_restart = aac_rx_restart_adapter;
	dev->a_ops.adapter_start = aac_rx_start_adapter;

	/*
	 *	First clear out all interrupts.  Then enable the one's that we
	 *	can handle.
	 */
	aac_adapter_comm(dev, AAC_COMM_PRODUCER);
	aac_adapter_disable_int(dev);
	rx_writel(dev, MUnit.ODR, 0xffffffff);
	aac_adapter_enable_int(dev);

	if (aac_init_adapter(dev) == NULL)
		goto error_iounmap;
	aac_adapter_comm(dev, dev->comm_interface);
	dev->sync_mode = 0;	/* sync. mode not supported */
	dev->msi = aac_msi && !pci_enable_msi(dev->pdev);
	if (request_irq(dev->pdev->irq, dev->a_ops.adapter_intr,
			IRQF_SHARED, "aacraid", dev) < 0) {
		if (dev->msi)
			pci_disable_msi(dev->pdev);
		printk(KERN_ERR "%s%d: Interrupt unavailable.\n",
			name, instance);
		goto error_iounmap;
	}
	dev->dbg_base = dev->base_start;
	dev->dbg_base_mapped = dev->base;
	dev->dbg_size = dev->base_size;

	aac_adapter_enable_int(dev);
	/*
	 *	Tell the adapter that all is configured, and it can
	 * start accepting requests
	 */
	aac_rx_start_adapter(dev);

	return 0;

error_iounmap:

	return -1;
}

int aac_rx_init(struct aac_dev *dev)
{
	/*
	 *	Fill in the function dispatch table.
	 */
	dev->a_ops.adapter_ioremap = aac_rx_ioremap;
	dev->a_ops.adapter_comm = aac_rx_select_comm;

	return _aac_rx_init(dev);
}
