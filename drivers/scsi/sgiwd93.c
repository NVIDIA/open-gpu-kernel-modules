/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996 David S. Miller (davem@davemloft.net)
 * Copyright (C) 1999 Andrew R. Baker (andrewb@uab.edu)
 * Copyright (C) 2001 Florian Lohoff (flo@rfc822.org)
 * Copyright (C) 2003, 07 Ralf Baechle (ralf@linux-mips.org)
 * 
 * (In all truth, Jed Schimmel wrote all this code.)
 */

#undef DEBUG

#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/gfp.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#include <asm/sgi/hpc3.h>
#include <asm/sgi/ip22.h>
#include <asm/sgi/wd.h>

#include "scsi.h"
#include "wd33c93.h"

struct ip22_hostdata {
	struct WD33C93_hostdata wh;
	dma_addr_t dma;
	void *cpu;
	struct device *dev;
};

#define host_to_hostdata(host) ((struct ip22_hostdata *)((host)->hostdata))

struct hpc_chunk {
	struct hpc_dma_desc desc;
	u32 _padding;	/* align to quadword boundary */
};

/* space for hpc dma descriptors */
#define HPC_DMA_SIZE   PAGE_SIZE

#define DMA_DIR(d)   ((d == DATA_OUT_DIR) ? DMA_TO_DEVICE : DMA_FROM_DEVICE)

static irqreturn_t sgiwd93_intr(int irq, void *dev_id)
{
	struct Scsi_Host * host = dev_id;
	unsigned long flags;

	spin_lock_irqsave(host->host_lock, flags);
	wd33c93_intr(host);
	spin_unlock_irqrestore(host->host_lock, flags);

	return IRQ_HANDLED;
}

static inline
void fill_hpc_entries(struct ip22_hostdata *hd, struct scsi_cmnd *cmd, int din)
{
	unsigned long len = cmd->SCp.this_residual;
	void *addr = cmd->SCp.ptr;
	dma_addr_t physaddr;
	unsigned long count;
	struct hpc_chunk *hcp;

	physaddr = dma_map_single(hd->dev, addr, len, DMA_DIR(din));
	cmd->SCp.dma_handle = physaddr;
	hcp = hd->cpu;

	while (len) {
		/*
		 * even cntinfo could be up to 16383, without
		 * magic only 8192 works correctly
		 */
		count = len > 8192 ? 8192 : len;
		hcp->desc.pbuf = physaddr;
		hcp->desc.cntinfo = count;
		hcp++;
		len -= count;
		physaddr += count;
	}

	/*
	 * To make sure, if we trip an HPC bug, that we transfer every single
	 * byte, we tag on an extra zero length dma descriptor at the end of
	 * the chain.
	 */
	hcp->desc.pbuf = 0;
	hcp->desc.cntinfo = HPCDMA_EOX;
	dma_sync_single_for_device(hd->dev, hd->dma,
		       (unsigned long)(hcp + 1) - (unsigned long)hd->cpu,
		       DMA_TO_DEVICE);
}

static int dma_setup(struct scsi_cmnd *cmd, int datainp)
{
	struct ip22_hostdata *hdata = host_to_hostdata(cmd->device->host);
	struct hpc3_scsiregs *hregs =
		(struct hpc3_scsiregs *) cmd->device->host->base;

	pr_debug("dma_setup: datainp<%d> hcp<%p> ", datainp, hdata->cpu);

	hdata->wh.dma_dir = datainp;

	/*
	 * wd33c93 shouldn't pass us bogus dma_setups, but it does:-(  The
	 * other wd33c93 drivers deal with it the same way (which isn't that
	 * obvious).  IMHO a better fix would be, not to do these dma setups
	 * in the first place.
	 */
	if (cmd->SCp.ptr == NULL || cmd->SCp.this_residual == 0)
		return 1;

	fill_hpc_entries(hdata, cmd, datainp);

	pr_debug(" HPCGO\n");

	/* Start up the HPC. */
	hregs->ndptr = hdata->dma;
	if (datainp)
		hregs->ctrl = HPC3_SCTRL_ACTIVE;
	else
		hregs->ctrl = HPC3_SCTRL_ACTIVE | HPC3_SCTRL_DIR;

	return 0;
}

static void dma_stop(struct Scsi_Host *instance, struct scsi_cmnd *SCpnt,
		     int status)
{
	struct ip22_hostdata *hdata = host_to_hostdata(instance);
	struct hpc3_scsiregs *hregs;

	if (!SCpnt)
		return;

	if (SCpnt->SCp.ptr == NULL || SCpnt->SCp.this_residual == 0)
		return;

	hregs = (struct hpc3_scsiregs *) SCpnt->device->host->base;

	pr_debug("dma_stop: status<%d> ", status);

	/* First stop the HPC and flush it's FIFO. */
	if (hdata->wh.dma_dir) {
		hregs->ctrl |= HPC3_SCTRL_FLUSH;
		while (hregs->ctrl & HPC3_SCTRL_ACTIVE)
			barrier();
	}
	hregs->ctrl = 0;
	dma_unmap_single(hdata->dev, SCpnt->SCp.dma_handle,
			 SCpnt->SCp.this_residual,
			 DMA_DIR(hdata->wh.dma_dir));

	pr_debug("\n");
}

void sgiwd93_reset(unsigned long base)
{
	struct hpc3_scsiregs *hregs = (struct hpc3_scsiregs *) base;

	hregs->ctrl = HPC3_SCTRL_CRESET;
	udelay(50);
	hregs->ctrl = 0;
}
EXPORT_SYMBOL_GPL(sgiwd93_reset);

static inline void init_hpc_chain(struct ip22_hostdata *hdata)
{
	struct hpc_chunk *hcp = (struct hpc_chunk *)hdata->cpu;
	dma_addr_t dma = hdata->dma;
	unsigned long start, end;

	start = (unsigned long) hcp;
	end = start + HPC_DMA_SIZE;
	while (start < end) {
		hcp->desc.pnext = (u32) (dma + sizeof(struct hpc_chunk));
		hcp->desc.cntinfo = HPCDMA_EOX;
		hcp++;
		dma += sizeof(struct hpc_chunk);
		start += sizeof(struct hpc_chunk);
	}
	hcp--;
	hcp->desc.pnext = hdata->dma;
}

/*
 * Kludge alert - the SCSI code calls the abort and reset method with int
 * arguments not with pointers.  So this is going to blow up beautyfully
 * on 64-bit systems with memory outside the compat address spaces.
 */
static struct scsi_host_template sgiwd93_template = {
	.module			= THIS_MODULE,
	.proc_name		= "SGIWD93",
	.name			= "SGI WD93",
	.queuecommand		= wd33c93_queuecommand,
	.eh_abort_handler	= wd33c93_abort,
	.eh_host_reset_handler	= wd33c93_host_reset,
	.can_queue		= 16,
	.this_id		= 7,
	.sg_tablesize		= SG_ALL,
	.cmd_per_lun		= 8,
	.dma_boundary		= PAGE_SIZE - 1,
};

static int sgiwd93_probe(struct platform_device *pdev)
{
	struct sgiwd93_platform_data *pd = pdev->dev.platform_data;
	unsigned char *wdregs = pd->wdregs;
	struct hpc3_scsiregs *hregs = pd->hregs;
	struct ip22_hostdata *hdata;
	struct Scsi_Host *host;
	wd33c93_regs regs;
	unsigned int unit = pd->unit;
	unsigned int irq = pd->irq;
	int err;

	host = scsi_host_alloc(&sgiwd93_template, sizeof(struct ip22_hostdata));
	if (!host) {
		err = -ENOMEM;
		goto out;
	}

	host->base = (unsigned long) hregs;
	host->irq = irq;

	hdata = host_to_hostdata(host);
	hdata->dev = &pdev->dev;
	hdata->cpu = dma_alloc_noncoherent(&pdev->dev, HPC_DMA_SIZE,
				&hdata->dma, DMA_TO_DEVICE, GFP_KERNEL);
	if (!hdata->cpu) {
		printk(KERN_WARNING "sgiwd93: Could not allocate memory for "
		       "host %d buffer.\n", unit);
		err = -ENOMEM;
		goto out_put;
	}

	init_hpc_chain(hdata);

	regs.SASR = wdregs + 3;
	regs.SCMD = wdregs + 7;

	hdata->wh.no_sync = 0;
	hdata->wh.fast = 1;
	hdata->wh.dma_mode = CTRL_BURST;

	wd33c93_init(host, regs, dma_setup, dma_stop, WD33C93_FS_MHZ(20));

	err = request_irq(irq, sgiwd93_intr, 0, "SGI WD93", host);
	if (err) {
		printk(KERN_WARNING "sgiwd93: Could not register irq %d "
		       "for host %d.\n", irq, unit);
		goto out_free;
	}

	platform_set_drvdata(pdev, host);

	err = scsi_add_host(host, NULL);
	if (err)
		goto out_irq;

	scsi_scan_host(host);

	return 0;

out_irq:
	free_irq(irq, host);
out_free:
	dma_free_noncoherent(&pdev->dev, HPC_DMA_SIZE, hdata->cpu, hdata->dma,
			DMA_TO_DEVICE);
out_put:
	scsi_host_put(host);
out:

	return err;
}

static int sgiwd93_remove(struct platform_device *pdev)
{
	struct Scsi_Host *host = platform_get_drvdata(pdev);
	struct ip22_hostdata *hdata = (struct ip22_hostdata *) host->hostdata;
	struct sgiwd93_platform_data *pd = pdev->dev.platform_data;

	scsi_remove_host(host);
	free_irq(pd->irq, host);
	dma_free_noncoherent(&pdev->dev, HPC_DMA_SIZE, hdata->cpu, hdata->dma,
			DMA_TO_DEVICE);
	scsi_host_put(host);
	return 0;
}

static struct platform_driver sgiwd93_driver = {
	.probe  = sgiwd93_probe,
	.remove = sgiwd93_remove,
	.driver = {
		.name   = "sgiwd93",
	}
};

static int __init sgiwd93_module_init(void)
{
	return platform_driver_register(&sgiwd93_driver);
}

static void __exit sgiwd93_module_exit(void)
{
	return platform_driver_unregister(&sgiwd93_driver);
}

module_init(sgiwd93_module_init);
module_exit(sgiwd93_module_exit);

MODULE_DESCRIPTION("SGI WD33C93 driver");
MODULE_AUTHOR("Ralf Baechle <ralf@linux-mips.org>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:sgiwd93");
