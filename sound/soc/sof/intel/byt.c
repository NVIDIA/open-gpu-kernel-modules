// SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause)
//
// This file is provided under a dual BSD/GPLv2 license.  When using or
// redistributing this file, you may do so under either license.
//
// Copyright(c) 2018 Intel Corporation. All rights reserved.
//
// Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
//

/*
 * Hardware interface for audio DSP on Baytrail, Braswell and Cherrytrail.
 */

#include <linux/module.h>
#include <sound/sof.h>
#include <sound/sof/xtensa.h>
#include <sound/soc-acpi.h>
#include <sound/soc-acpi-intel-match.h>
#include <sound/intel-dsp-config.h>
#include "../ops.h"
#include "shim.h"
#include "../sof-acpi-dev.h"
#include "../sof-audio.h"
#include "../../intel/common/soc-intel-quirks.h"

/* DSP memories */
#define IRAM_OFFSET		0x0C0000
#define IRAM_SIZE		(80 * 1024)
#define DRAM_OFFSET		0x100000
#define DRAM_SIZE		(160 * 1024)
#define SHIM_OFFSET		0x140000
#define SHIM_SIZE_BYT		0x100
#define SHIM_SIZE_CHT		0x118
#define MBOX_OFFSET		0x144000
#define MBOX_SIZE		0x1000
#define EXCEPT_OFFSET		0x800
#define EXCEPT_MAX_HDR_SIZE	0x400

/* DSP peripherals */
#define DMAC0_OFFSET		0x098000
#define DMAC1_OFFSET		0x09c000
#define DMAC2_OFFSET		0x094000
#define DMAC_SIZE		0x420
#define SSP0_OFFSET		0x0a0000
#define SSP1_OFFSET		0x0a1000
#define SSP2_OFFSET		0x0a2000
#define SSP3_OFFSET		0x0a4000
#define SSP4_OFFSET		0x0a5000
#define SSP5_OFFSET		0x0a6000
#define SSP_SIZE		0x100

#define BYT_STACK_DUMP_SIZE	32

#define BYT_PCI_BAR_SIZE	0x200000

#define BYT_PANIC_OFFSET(x)	(((x) & GENMASK_ULL(47, 32)) >> 32)

/*
 * Debug
 */

#define MBOX_DUMP_SIZE	0x30

/* BARs */
#define BYT_DSP_BAR		0
#define BYT_PCI_BAR		1
#define BYT_IMR_BAR		2

static const struct snd_sof_debugfs_map byt_debugfs[] = {
	{"dmac0", BYT_DSP_BAR, DMAC0_OFFSET, DMAC_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"dmac1", BYT_DSP_BAR,  DMAC1_OFFSET, DMAC_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp0",  BYT_DSP_BAR, SSP0_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp1", BYT_DSP_BAR, SSP1_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp2", BYT_DSP_BAR, SSP2_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"iram", BYT_DSP_BAR, IRAM_OFFSET, IRAM_SIZE,
	 SOF_DEBUGFS_ACCESS_D0_ONLY},
	{"dram", BYT_DSP_BAR, DRAM_OFFSET, DRAM_SIZE,
	 SOF_DEBUGFS_ACCESS_D0_ONLY},
	{"shim", BYT_DSP_BAR, SHIM_OFFSET, SHIM_SIZE_BYT,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
};

static void byt_host_done(struct snd_sof_dev *sdev);
static void byt_dsp_done(struct snd_sof_dev *sdev);
static void byt_get_reply(struct snd_sof_dev *sdev);

/*
 * Debug
 */

static void byt_get_registers(struct snd_sof_dev *sdev,
			      struct sof_ipc_dsp_oops_xtensa *xoops,
			      struct sof_ipc_panic_info *panic_info,
			      u32 *stack, size_t stack_words)
{
	u32 offset = sdev->dsp_oops_offset;

	/* first read regsisters */
	sof_mailbox_read(sdev, offset, xoops, sizeof(*xoops));

	/* note: variable AR register array is not read */

	/* then get panic info */
	if (xoops->arch_hdr.totalsize > EXCEPT_MAX_HDR_SIZE) {
		dev_err(sdev->dev, "invalid header size 0x%x. FW oops is bogus\n",
			xoops->arch_hdr.totalsize);
		return;
	}
	offset += xoops->arch_hdr.totalsize;
	sof_mailbox_read(sdev, offset, panic_info, sizeof(*panic_info));

	/* then get the stack */
	offset += sizeof(*panic_info);
	sof_mailbox_read(sdev, offset, stack, stack_words * sizeof(u32));
}

static void byt_dump(struct snd_sof_dev *sdev, u32 flags)
{
	struct sof_ipc_dsp_oops_xtensa xoops;
	struct sof_ipc_panic_info panic_info;
	u32 stack[BYT_STACK_DUMP_SIZE];
	u64 status, panic, imrd, imrx;

	/* now try generic SOF status messages */
	status = snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_IPCD);
	panic = snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_IPCX);
	byt_get_registers(sdev, &xoops, &panic_info, stack,
			  BYT_STACK_DUMP_SIZE);
	snd_sof_get_status(sdev, status, panic, &xoops, &panic_info, stack,
			   BYT_STACK_DUMP_SIZE);

	/* provide some context for firmware debug */
	imrx = snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_IMRX);
	imrd = snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_IMRD);
	dev_err(sdev->dev,
		"error: ipc host -> DSP: pending %s complete %s raw 0x%llx\n",
		(panic & SHIM_IPCX_BUSY) ? "yes" : "no",
		(panic & SHIM_IPCX_DONE) ? "yes" : "no", panic);
	dev_err(sdev->dev,
		"error: mask host: pending %s complete %s raw 0x%llx\n",
		(imrx & SHIM_IMRX_BUSY) ? "yes" : "no",
		(imrx & SHIM_IMRX_DONE) ? "yes" : "no", imrx);
	dev_err(sdev->dev,
		"error: ipc DSP -> host: pending %s complete %s raw 0x%llx\n",
		(status & SHIM_IPCD_BUSY) ? "yes" : "no",
		(status & SHIM_IPCD_DONE) ? "yes" : "no", status);
	dev_err(sdev->dev,
		"error: mask DSP: pending %s complete %s raw 0x%llx\n",
		(imrd & SHIM_IMRD_BUSY) ? "yes" : "no",
		(imrd & SHIM_IMRD_DONE) ? "yes" : "no", imrd);

}

/*
 * IPC Doorbell IRQ handler and thread.
 */

static irqreturn_t byt_irq_handler(int irq, void *context)
{
	struct snd_sof_dev *sdev = context;
	u64 ipcx, ipcd;
	int ret = IRQ_NONE;

	ipcx = snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_IPCX);
	ipcd = snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_IPCD);

	if (ipcx & SHIM_BYT_IPCX_DONE) {

		/* reply message from DSP, Mask Done interrupt first */
		snd_sof_dsp_update_bits64_unlocked(sdev, BYT_DSP_BAR,
						   SHIM_IMRX,
						   SHIM_IMRX_DONE,
						   SHIM_IMRX_DONE);
		ret = IRQ_WAKE_THREAD;
	}

	if (ipcd & SHIM_BYT_IPCD_BUSY) {

		/* new message from DSP, Mask Busy interrupt first */
		snd_sof_dsp_update_bits64_unlocked(sdev, BYT_DSP_BAR,
						   SHIM_IMRX,
						   SHIM_IMRX_BUSY,
						   SHIM_IMRX_BUSY);
		ret = IRQ_WAKE_THREAD;
	}

	return ret;
}

static irqreturn_t byt_irq_thread(int irq, void *context)
{
	struct snd_sof_dev *sdev = context;
	u64 ipcx, ipcd;

	ipcx = snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_IPCX);
	ipcd = snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_IPCD);

	/* reply message from DSP */
	if (ipcx & SHIM_BYT_IPCX_DONE) {

		spin_lock_irq(&sdev->ipc_lock);

		/*
		 * handle immediate reply from DSP core. If the msg is
		 * found, set done bit in cmd_done which is called at the
		 * end of message processing function, else set it here
		 * because the done bit can't be set in cmd_done function
		 * which is triggered by msg
		 */
		byt_get_reply(sdev);
		snd_sof_ipc_reply(sdev, ipcx);

		byt_dsp_done(sdev);

		spin_unlock_irq(&sdev->ipc_lock);
	}

	/* new message from DSP */
	if (ipcd & SHIM_BYT_IPCD_BUSY) {

		/* Handle messages from DSP Core */
		if ((ipcd & SOF_IPC_PANIC_MAGIC_MASK) == SOF_IPC_PANIC_MAGIC) {
			snd_sof_dsp_panic(sdev, BYT_PANIC_OFFSET(ipcd) +
					  MBOX_OFFSET);
		} else {
			snd_sof_ipc_msgs_rx(sdev);
		}

		byt_host_done(sdev);
	}

	return IRQ_HANDLED;
}

static int byt_send_msg(struct snd_sof_dev *sdev, struct snd_sof_ipc_msg *msg)
{
	/* unmask and prepare to receive Done interrupt */
	snd_sof_dsp_update_bits64_unlocked(sdev, BYT_DSP_BAR, SHIM_IMRX,
					   SHIM_IMRX_DONE, 0);

	/* send the message */
	sof_mailbox_write(sdev, sdev->host_box.offset, msg->msg_data,
			  msg->msg_size);
	snd_sof_dsp_write64(sdev, BYT_DSP_BAR, SHIM_IPCX, SHIM_BYT_IPCX_BUSY);

	return 0;
}

static void byt_get_reply(struct snd_sof_dev *sdev)
{
	struct snd_sof_ipc_msg *msg = sdev->msg;
	struct sof_ipc_reply reply;
	int ret = 0;

	/*
	 * Sometimes, there is unexpected reply ipc arriving. The reply
	 * ipc belongs to none of the ipcs sent from driver.
	 * In this case, the driver must ignore the ipc.
	 */
	if (!msg) {
		dev_warn(sdev->dev, "unexpected ipc interrupt raised!\n");
		return;
	}

	/* get reply */
	sof_mailbox_read(sdev, sdev->host_box.offset, &reply, sizeof(reply));

	if (reply.error < 0) {
		memcpy(msg->reply_data, &reply, sizeof(reply));
		ret = reply.error;
	} else {
		/* reply correct size ? */
		if (reply.hdr.size != msg->reply_size) {
			dev_err(sdev->dev, "error: reply expected %zu got %u bytes\n",
				msg->reply_size, reply.hdr.size);
			ret = -EINVAL;
		}

		/* read the message */
		if (msg->reply_size > 0)
			sof_mailbox_read(sdev, sdev->host_box.offset,
					 msg->reply_data, msg->reply_size);
	}

	msg->reply_error = ret;
}

static int byt_get_mailbox_offset(struct snd_sof_dev *sdev)
{
	return MBOX_OFFSET;
}

static int byt_get_window_offset(struct snd_sof_dev *sdev, u32 id)
{
	return MBOX_OFFSET;
}

static void byt_host_done(struct snd_sof_dev *sdev)
{
	/* clear BUSY bit and set DONE bit - accept new messages */
	snd_sof_dsp_update_bits64_unlocked(sdev, BYT_DSP_BAR, SHIM_IPCD,
					   SHIM_BYT_IPCD_BUSY |
					   SHIM_BYT_IPCD_DONE,
					   SHIM_BYT_IPCD_DONE);

	/* unmask and prepare to receive next new message */
	snd_sof_dsp_update_bits64_unlocked(sdev, BYT_DSP_BAR, SHIM_IMRX,
					   SHIM_IMRX_BUSY, 0);
}

static void byt_dsp_done(struct snd_sof_dev *sdev)
{
	/* clear DONE bit - tell DSP we have completed */
	snd_sof_dsp_update_bits64_unlocked(sdev, BYT_DSP_BAR, SHIM_IPCX,
					   SHIM_BYT_IPCX_DONE, 0);
}

/*
 * DSP control.
 */

static int byt_run(struct snd_sof_dev *sdev)
{
	int tries = 10;

	/* release stall and wait to unstall */
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_CSR,
				  SHIM_BYT_CSR_STALL, 0x0);
	while (tries--) {
		if (!(snd_sof_dsp_read64(sdev, BYT_DSP_BAR, SHIM_CSR) &
		      SHIM_BYT_CSR_PWAITMODE))
			break;
		msleep(100);
	}
	if (tries < 0) {
		dev_err(sdev->dev, "error:  unable to run DSP firmware\n");
		byt_dump(sdev, SOF_DBG_DUMP_REGS | SOF_DBG_DUMP_MBOX);
		return -ENODEV;
	}

	/* return init core mask */
	return 1;
}

static int byt_reset(struct snd_sof_dev *sdev)
{
	/* put DSP into reset, set reset vector and stall */
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_CSR,
				  SHIM_BYT_CSR_RST | SHIM_BYT_CSR_VECTOR_SEL |
				  SHIM_BYT_CSR_STALL,
				  SHIM_BYT_CSR_RST | SHIM_BYT_CSR_VECTOR_SEL |
				  SHIM_BYT_CSR_STALL);

	usleep_range(10, 15);

	/* take DSP out of reset and keep stalled for FW loading */
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_CSR,
				  SHIM_BYT_CSR_RST, 0);

	return 0;
}

static const char *fixup_tplg_name(struct snd_sof_dev *sdev,
				   const char *sof_tplg_filename,
				   const char *ssp_str)
{
	const char *tplg_filename = NULL;
	char *filename;
	char *split_ext;

	filename = devm_kstrdup(sdev->dev, sof_tplg_filename, GFP_KERNEL);
	if (!filename)
		return NULL;

	/* this assumes a .tplg extension */
	split_ext = strsep(&filename, ".");
	if (split_ext) {
		tplg_filename = devm_kasprintf(sdev->dev, GFP_KERNEL,
					       "%s-%s.tplg",
					       split_ext, ssp_str);
		if (!tplg_filename)
			return NULL;
	}
	return tplg_filename;
}

static void byt_machine_select(struct snd_sof_dev *sdev)
{
	struct snd_sof_pdata *sof_pdata = sdev->pdata;
	const struct sof_dev_desc *desc = sof_pdata->desc;
	struct snd_soc_acpi_mach *mach;
	struct platform_device *pdev;
	const char *tplg_filename;

	mach = snd_soc_acpi_find_machine(desc->machines);
	if (!mach) {
		dev_warn(sdev->dev, "warning: No matching ASoC machine driver found\n");
		return;
	}

	pdev = to_platform_device(sdev->dev);
	if (soc_intel_is_byt_cr(pdev)) {
		dev_dbg(sdev->dev,
			"BYT-CR detected, SSP0 used instead of SSP2\n");

		tplg_filename = fixup_tplg_name(sdev,
						mach->sof_tplg_filename,
						"ssp0");
	} else {
		tplg_filename = mach->sof_tplg_filename;
	}

	if (!tplg_filename) {
		dev_dbg(sdev->dev,
			"error: no topology filename\n");
		return;
	}

	sof_pdata->tplg_filename = tplg_filename;
	mach->mach_params.acpi_ipc_irq_index = desc->irqindex_host_ipc;
	sof_pdata->machine = mach;
}

/* Baytrail DAIs */
static struct snd_soc_dai_driver byt_dai[] = {
{
	.name = "ssp0-port",
	.playback = {
		.channels_min = 1,
		.channels_max = 8,
	},
	.capture = {
		.channels_min = 1,
		.channels_max = 8,
	},
},
{
	.name = "ssp1-port",
	.playback = {
		.channels_min = 1,
		.channels_max = 8,
	},
	.capture = {
		.channels_min = 1,
		.channels_max = 8,
	},
},
{
	.name = "ssp2-port",
	.playback = {
		.channels_min = 1,
		.channels_max = 8,
	},
	.capture = {
		.channels_min = 1,
		.channels_max = 8,
	}
},
{
	.name = "ssp3-port",
	.playback = {
		.channels_min = 1,
		.channels_max = 8,
	},
	.capture = {
		.channels_min = 1,
		.channels_max = 8,
	},
},
{
	.name = "ssp4-port",
	.playback = {
		.channels_min = 1,
		.channels_max = 8,
	},
	.capture = {
		.channels_min = 1,
		.channels_max = 8,
	},
},
{
	.name = "ssp5-port",
	.playback = {
		.channels_min = 1,
		.channels_max = 8,
	},
	.capture = {
		.channels_min = 1,
		.channels_max = 8,
	},
},
};

static void byt_set_mach_params(const struct snd_soc_acpi_mach *mach,
				struct snd_sof_dev *sdev)
{
	struct snd_sof_pdata *pdata = sdev->pdata;
	const struct sof_dev_desc *desc = pdata->desc;
	struct snd_soc_acpi_mach_params *mach_params;

	mach_params = (struct snd_soc_acpi_mach_params *)&mach->mach_params;
	mach_params->platform = dev_name(sdev->dev);
	mach_params->num_dai_drivers = desc->ops->num_drv;
	mach_params->dai_drivers = desc->ops->drv;
}

/*
 * Probe and remove.
 */

#if IS_ENABLED(CONFIG_SND_SOC_SOF_MERRIFIELD)

static int tangier_pci_probe(struct snd_sof_dev *sdev)
{
	struct snd_sof_pdata *pdata = sdev->pdata;
	const struct sof_dev_desc *desc = pdata->desc;
	struct pci_dev *pci = to_pci_dev(sdev->dev);
	u32 base, size;
	int ret;

	/* DSP DMA can only access low 31 bits of host memory */
	ret = dma_coerce_mask_and_coherent(&pci->dev, DMA_BIT_MASK(31));
	if (ret < 0) {
		dev_err(sdev->dev, "error: failed to set DMA mask %d\n", ret);
		return ret;
	}

	/* LPE base */
	base = pci_resource_start(pci, desc->resindex_lpe_base) - IRAM_OFFSET;
	size = BYT_PCI_BAR_SIZE;

	dev_dbg(sdev->dev, "LPE PHY base at 0x%x size 0x%x", base, size);
	sdev->bar[BYT_DSP_BAR] = devm_ioremap(sdev->dev, base, size);
	if (!sdev->bar[BYT_DSP_BAR]) {
		dev_err(sdev->dev, "error: failed to ioremap LPE base 0x%x size 0x%x\n",
			base, size);
		return -ENODEV;
	}
	dev_dbg(sdev->dev, "LPE VADDR %p\n", sdev->bar[BYT_DSP_BAR]);

	/* IMR base - optional */
	if (desc->resindex_imr_base == -1)
		goto irq;

	base = pci_resource_start(pci, desc->resindex_imr_base);
	size = pci_resource_len(pci, desc->resindex_imr_base);

	/* some BIOSes don't map IMR */
	if (base == 0x55aa55aa || base == 0x0) {
		dev_info(sdev->dev, "IMR not set by BIOS. Ignoring\n");
		goto irq;
	}

	dev_dbg(sdev->dev, "IMR base at 0x%x size 0x%x", base, size);
	sdev->bar[BYT_IMR_BAR] = devm_ioremap(sdev->dev, base, size);
	if (!sdev->bar[BYT_IMR_BAR]) {
		dev_err(sdev->dev, "error: failed to ioremap IMR base 0x%x size 0x%x\n",
			base, size);
		return -ENODEV;
	}
	dev_dbg(sdev->dev, "IMR VADDR %p\n", sdev->bar[BYT_IMR_BAR]);

irq:
	/* register our IRQ */
	sdev->ipc_irq = pci->irq;
	dev_dbg(sdev->dev, "using IRQ %d\n", sdev->ipc_irq);
	ret = devm_request_threaded_irq(sdev->dev, sdev->ipc_irq,
					byt_irq_handler, byt_irq_thread,
					0, "AudioDSP", sdev);
	if (ret < 0) {
		dev_err(sdev->dev, "error: failed to register IRQ %d\n",
			sdev->ipc_irq);
		return ret;
	}

	/* enable BUSY and disable DONE Interrupt by default */
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_IMRX,
				  SHIM_IMRX_BUSY | SHIM_IMRX_DONE,
				  SHIM_IMRX_DONE);

	/* set default mailbox offset for FW ready message */
	sdev->dsp_box.offset = MBOX_OFFSET;

	return ret;
}

const struct snd_sof_dsp_ops sof_tng_ops = {
	/* device init */
	.probe		= tangier_pci_probe,

	/* DSP core boot / reset */
	.run		= byt_run,
	.reset		= byt_reset,

	/* Register IO */
	.write		= sof_io_write,
	.read		= sof_io_read,
	.write64	= sof_io_write64,
	.read64		= sof_io_read64,

	/* Block IO */
	.block_read	= sof_block_read,
	.block_write	= sof_block_write,

	/* doorbell */
	.irq_handler	= byt_irq_handler,
	.irq_thread	= byt_irq_thread,

	/* ipc */
	.send_msg	= byt_send_msg,
	.fw_ready	= sof_fw_ready,
	.get_mailbox_offset = byt_get_mailbox_offset,
	.get_window_offset = byt_get_window_offset,

	.ipc_msg_data	= intel_ipc_msg_data,
	.ipc_pcm_params	= intel_ipc_pcm_params,

	/* machine driver */
	.machine_select = byt_machine_select,
	.machine_register = sof_machine_register,
	.machine_unregister = sof_machine_unregister,
	.set_mach_params = byt_set_mach_params,

	/* debug */
	.debug_map	= byt_debugfs,
	.debug_map_count	= ARRAY_SIZE(byt_debugfs),
	.dbg_dump	= byt_dump,

	/* stream callbacks */
	.pcm_open	= intel_pcm_open,
	.pcm_close	= intel_pcm_close,

	/* module loading */
	.load_module	= snd_sof_parse_module_memcpy,

	/*Firmware loading */
	.load_firmware	= snd_sof_load_firmware_memcpy,

	/* DAI drivers */
	.drv = byt_dai,
	.num_drv = 3, /* we have only 3 SSPs on byt*/

	/* ALSA HW info flags */
	.hw_info =	SNDRV_PCM_INFO_MMAP |
			SNDRV_PCM_INFO_MMAP_VALID |
			SNDRV_PCM_INFO_INTERLEAVED |
			SNDRV_PCM_INFO_PAUSE |
			SNDRV_PCM_INFO_BATCH,

	.arch_ops = &sof_xtensa_arch_ops,
};
EXPORT_SYMBOL_NS(sof_tng_ops, SND_SOC_SOF_MERRIFIELD);

const struct sof_intel_dsp_desc tng_chip_info = {
	.cores_num = 1,
	.host_managed_cores_mask = 1,
};
EXPORT_SYMBOL_NS(tng_chip_info, SND_SOC_SOF_MERRIFIELD);

#endif /* CONFIG_SND_SOC_SOF_MERRIFIELD */

#if IS_ENABLED(CONFIG_SND_SOC_SOF_BAYTRAIL)

static void byt_reset_dsp_disable_int(struct snd_sof_dev *sdev)
{
	/* Disable Interrupt from both sides */
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_IMRX, 0x3, 0x3);
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_IMRD, 0x3, 0x3);

	/* Put DSP into reset, set reset vector */
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_CSR,
				  SHIM_BYT_CSR_RST | SHIM_BYT_CSR_VECTOR_SEL,
				  SHIM_BYT_CSR_RST | SHIM_BYT_CSR_VECTOR_SEL);
}

static int byt_suspend(struct snd_sof_dev *sdev, u32 target_state)
{
	byt_reset_dsp_disable_int(sdev);

	return 0;
}

static int byt_resume(struct snd_sof_dev *sdev)
{
	/* enable BUSY and disable DONE Interrupt by default */
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_IMRX,
				  SHIM_IMRX_BUSY | SHIM_IMRX_DONE,
				  SHIM_IMRX_DONE);

	return 0;
}

static int byt_remove(struct snd_sof_dev *sdev)
{
	byt_reset_dsp_disable_int(sdev);

	return 0;
}

static const struct snd_sof_debugfs_map cht_debugfs[] = {
	{"dmac0", BYT_DSP_BAR, DMAC0_OFFSET, DMAC_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"dmac1", BYT_DSP_BAR,  DMAC1_OFFSET, DMAC_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"dmac2", BYT_DSP_BAR,  DMAC2_OFFSET, DMAC_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp0",  BYT_DSP_BAR, SSP0_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp1", BYT_DSP_BAR, SSP1_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp2", BYT_DSP_BAR, SSP2_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp3", BYT_DSP_BAR, SSP3_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp4", BYT_DSP_BAR, SSP4_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"ssp5", BYT_DSP_BAR, SSP5_OFFSET, SSP_SIZE,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
	{"iram", BYT_DSP_BAR, IRAM_OFFSET, IRAM_SIZE,
	 SOF_DEBUGFS_ACCESS_D0_ONLY},
	{"dram", BYT_DSP_BAR, DRAM_OFFSET, DRAM_SIZE,
	 SOF_DEBUGFS_ACCESS_D0_ONLY},
	{"shim", BYT_DSP_BAR, SHIM_OFFSET, SHIM_SIZE_CHT,
	 SOF_DEBUGFS_ACCESS_ALWAYS},
};

static int byt_acpi_probe(struct snd_sof_dev *sdev)
{
	struct snd_sof_pdata *pdata = sdev->pdata;
	const struct sof_dev_desc *desc = pdata->desc;
	struct platform_device *pdev =
		container_of(sdev->dev, struct platform_device, dev);
	struct resource *mmio;
	u32 base, size;
	int ret;

	/* DSP DMA can only access low 31 bits of host memory */
	ret = dma_coerce_mask_and_coherent(sdev->dev, DMA_BIT_MASK(31));
	if (ret < 0) {
		dev_err(sdev->dev, "error: failed to set DMA mask %d\n", ret);
		return ret;
	}

	/* LPE base */
	mmio = platform_get_resource(pdev, IORESOURCE_MEM,
				     desc->resindex_lpe_base);
	if (mmio) {
		base = mmio->start;
		size = resource_size(mmio);
	} else {
		dev_err(sdev->dev, "error: failed to get LPE base at idx %d\n",
			desc->resindex_lpe_base);
		return -EINVAL;
	}

	dev_dbg(sdev->dev, "LPE PHY base at 0x%x size 0x%x", base, size);
	sdev->bar[BYT_DSP_BAR] = devm_ioremap(sdev->dev, base, size);
	if (!sdev->bar[BYT_DSP_BAR]) {
		dev_err(sdev->dev, "error: failed to ioremap LPE base 0x%x size 0x%x\n",
			base, size);
		return -ENODEV;
	}
	dev_dbg(sdev->dev, "LPE VADDR %p\n", sdev->bar[BYT_DSP_BAR]);

	/* TODO: add offsets */
	sdev->mmio_bar = BYT_DSP_BAR;
	sdev->mailbox_bar = BYT_DSP_BAR;

	/* IMR base - optional */
	if (desc->resindex_imr_base == -1)
		goto irq;

	mmio = platform_get_resource(pdev, IORESOURCE_MEM,
				     desc->resindex_imr_base);
	if (mmio) {
		base = mmio->start;
		size = resource_size(mmio);
	} else {
		dev_err(sdev->dev, "error: failed to get IMR base at idx %d\n",
			desc->resindex_imr_base);
		return -ENODEV;
	}

	/* some BIOSes don't map IMR */
	if (base == 0x55aa55aa || base == 0x0) {
		dev_info(sdev->dev, "IMR not set by BIOS. Ignoring\n");
		goto irq;
	}

	dev_dbg(sdev->dev, "IMR base at 0x%x size 0x%x", base, size);
	sdev->bar[BYT_IMR_BAR] = devm_ioremap(sdev->dev, base, size);
	if (!sdev->bar[BYT_IMR_BAR]) {
		dev_err(sdev->dev, "error: failed to ioremap IMR base 0x%x size 0x%x\n",
			base, size);
		return -ENODEV;
	}
	dev_dbg(sdev->dev, "IMR VADDR %p\n", sdev->bar[BYT_IMR_BAR]);

irq:
	/* register our IRQ */
	sdev->ipc_irq = platform_get_irq(pdev, desc->irqindex_host_ipc);
	if (sdev->ipc_irq < 0)
		return sdev->ipc_irq;

	dev_dbg(sdev->dev, "using IRQ %d\n", sdev->ipc_irq);
	ret = devm_request_threaded_irq(sdev->dev, sdev->ipc_irq,
					byt_irq_handler, byt_irq_thread,
					IRQF_SHARED, "AudioDSP", sdev);
	if (ret < 0) {
		dev_err(sdev->dev, "error: failed to register IRQ %d\n",
			sdev->ipc_irq);
		return ret;
	}

	/* enable BUSY and disable DONE Interrupt by default */
	snd_sof_dsp_update_bits64(sdev, BYT_DSP_BAR, SHIM_IMRX,
				  SHIM_IMRX_BUSY | SHIM_IMRX_DONE,
				  SHIM_IMRX_DONE);

	/* set default mailbox offset for FW ready message */
	sdev->dsp_box.offset = MBOX_OFFSET;

	return ret;
}

/* baytrail ops */
static const struct snd_sof_dsp_ops sof_byt_ops = {
	/* device init */
	.probe		= byt_acpi_probe,
	.remove		= byt_remove,

	/* DSP core boot / reset */
	.run		= byt_run,
	.reset		= byt_reset,

	/* Register IO */
	.write		= sof_io_write,
	.read		= sof_io_read,
	.write64	= sof_io_write64,
	.read64		= sof_io_read64,

	/* Block IO */
	.block_read	= sof_block_read,
	.block_write	= sof_block_write,

	/* doorbell */
	.irq_handler	= byt_irq_handler,
	.irq_thread	= byt_irq_thread,

	/* ipc */
	.send_msg	= byt_send_msg,
	.fw_ready	= sof_fw_ready,
	.get_mailbox_offset = byt_get_mailbox_offset,
	.get_window_offset = byt_get_window_offset,

	.ipc_msg_data	= intel_ipc_msg_data,
	.ipc_pcm_params	= intel_ipc_pcm_params,

	/* machine driver */
	.machine_select = byt_machine_select,
	.machine_register = sof_machine_register,
	.machine_unregister = sof_machine_unregister,
	.set_mach_params = byt_set_mach_params,

	/* debug */
	.debug_map	= byt_debugfs,
	.debug_map_count	= ARRAY_SIZE(byt_debugfs),
	.dbg_dump	= byt_dump,

	/* stream callbacks */
	.pcm_open	= intel_pcm_open,
	.pcm_close	= intel_pcm_close,

	/* module loading */
	.load_module	= snd_sof_parse_module_memcpy,

	/*Firmware loading */
	.load_firmware	= snd_sof_load_firmware_memcpy,

	/* PM */
	.suspend = byt_suspend,
	.resume = byt_resume,

	/* DAI drivers */
	.drv = byt_dai,
	.num_drv = 3, /* we have only 3 SSPs on byt*/

	/* ALSA HW info flags */
	.hw_info =	SNDRV_PCM_INFO_MMAP |
			SNDRV_PCM_INFO_MMAP_VALID |
			SNDRV_PCM_INFO_INTERLEAVED |
			SNDRV_PCM_INFO_PAUSE |
			SNDRV_PCM_INFO_BATCH,

	.arch_ops = &sof_xtensa_arch_ops,
};

static const struct sof_intel_dsp_desc byt_chip_info = {
	.cores_num = 1,
	.host_managed_cores_mask = 1,
};

/* cherrytrail and braswell ops */
static const struct snd_sof_dsp_ops sof_cht_ops = {
	/* device init */
	.probe		= byt_acpi_probe,
	.remove		= byt_remove,

	/* DSP core boot / reset */
	.run		= byt_run,
	.reset		= byt_reset,

	/* Register IO */
	.write		= sof_io_write,
	.read		= sof_io_read,
	.write64	= sof_io_write64,
	.read64		= sof_io_read64,

	/* Block IO */
	.block_read	= sof_block_read,
	.block_write	= sof_block_write,

	/* doorbell */
	.irq_handler	= byt_irq_handler,
	.irq_thread	= byt_irq_thread,

	/* ipc */
	.send_msg	= byt_send_msg,
	.fw_ready	= sof_fw_ready,
	.get_mailbox_offset = byt_get_mailbox_offset,
	.get_window_offset = byt_get_window_offset,

	.ipc_msg_data	= intel_ipc_msg_data,
	.ipc_pcm_params	= intel_ipc_pcm_params,

	/* machine driver */
	.machine_select = byt_machine_select,
	.machine_register = sof_machine_register,
	.machine_unregister = sof_machine_unregister,
	.set_mach_params = byt_set_mach_params,

	/* debug */
	.debug_map	= cht_debugfs,
	.debug_map_count	= ARRAY_SIZE(cht_debugfs),
	.dbg_dump	= byt_dump,

	/* stream callbacks */
	.pcm_open	= intel_pcm_open,
	.pcm_close	= intel_pcm_close,

	/* module loading */
	.load_module	= snd_sof_parse_module_memcpy,

	/*Firmware loading */
	.load_firmware	= snd_sof_load_firmware_memcpy,

	/* PM */
	.suspend = byt_suspend,
	.resume = byt_resume,

	/* DAI drivers */
	.drv = byt_dai,
	/* all 6 SSPs may be available for cherrytrail */
	.num_drv = ARRAY_SIZE(byt_dai),

	/* ALSA HW info flags */
	.hw_info =	SNDRV_PCM_INFO_MMAP |
			SNDRV_PCM_INFO_MMAP_VALID |
			SNDRV_PCM_INFO_INTERLEAVED |
			SNDRV_PCM_INFO_PAUSE |
			SNDRV_PCM_INFO_BATCH,

	.arch_ops = &sof_xtensa_arch_ops,
};

static const struct sof_intel_dsp_desc cht_chip_info = {
	.cores_num = 1,
	.host_managed_cores_mask = 1,
};

/* BYTCR uses different IRQ index */
static const struct sof_dev_desc sof_acpi_baytrailcr_desc = {
	.machines = snd_soc_acpi_intel_baytrail_machines,
	.resindex_lpe_base = 0,
	.resindex_pcicfg_base = 1,
	.resindex_imr_base = 2,
	.irqindex_host_ipc = 0,
	.chip_info = &byt_chip_info,
	.default_fw_path = "intel/sof",
	.default_tplg_path = "intel/sof-tplg",
	.default_fw_filename = "sof-byt.ri",
	.nocodec_tplg_filename = "sof-byt-nocodec.tplg",
	.ops = &sof_byt_ops,
};

static const struct sof_dev_desc sof_acpi_baytrail_desc = {
	.machines = snd_soc_acpi_intel_baytrail_machines,
	.resindex_lpe_base = 0,
	.resindex_pcicfg_base = 1,
	.resindex_imr_base = 2,
	.irqindex_host_ipc = 5,
	.chip_info = &byt_chip_info,
	.default_fw_path = "intel/sof",
	.default_tplg_path = "intel/sof-tplg",
	.default_fw_filename = "sof-byt.ri",
	.nocodec_tplg_filename = "sof-byt-nocodec.tplg",
	.ops = &sof_byt_ops,
};

static const struct sof_dev_desc sof_acpi_cherrytrail_desc = {
	.machines = snd_soc_acpi_intel_cherrytrail_machines,
	.resindex_lpe_base = 0,
	.resindex_pcicfg_base = 1,
	.resindex_imr_base = 2,
	.irqindex_host_ipc = 5,
	.chip_info = &cht_chip_info,
	.default_fw_path = "intel/sof",
	.default_tplg_path = "intel/sof-tplg",
	.default_fw_filename = "sof-cht.ri",
	.nocodec_tplg_filename = "sof-cht-nocodec.tplg",
	.ops = &sof_cht_ops,
};

static const struct acpi_device_id sof_baytrail_match[] = {
	{ "80860F28", (unsigned long)&sof_acpi_baytrail_desc },
	{ "808622A8", (unsigned long)&sof_acpi_cherrytrail_desc },
	{ }
};
MODULE_DEVICE_TABLE(acpi, sof_baytrail_match);

static int sof_baytrail_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct sof_dev_desc *desc;
	const struct acpi_device_id *id;
	int ret;

	id = acpi_match_device(dev->driver->acpi_match_table, dev);
	if (!id)
		return -ENODEV;

	ret = snd_intel_acpi_dsp_driver_probe(dev, id->id);
	if (ret != SND_INTEL_DSP_DRIVER_ANY && ret != SND_INTEL_DSP_DRIVER_SOF) {
		dev_dbg(dev, "SOF ACPI driver not selected, aborting probe\n");
		return -ENODEV;
	}

	desc = device_get_match_data(&pdev->dev);
	if (!desc)
		return -ENODEV;

	if (desc == &sof_acpi_baytrail_desc && soc_intel_is_byt_cr(pdev))
		desc = &sof_acpi_baytrailcr_desc;

	return sof_acpi_probe(pdev, desc);
}

/* acpi_driver definition */
static struct platform_driver snd_sof_acpi_intel_byt_driver = {
	.probe = sof_baytrail_probe,
	.remove = sof_acpi_remove,
	.driver = {
		.name = "sof-audio-acpi-intel-byt",
		.pm = &sof_acpi_pm,
		.acpi_match_table = sof_baytrail_match,
	},
};
module_platform_driver(snd_sof_acpi_intel_byt_driver);

#endif /* CONFIG_SND_SOC_SOF_BAYTRAIL */

MODULE_LICENSE("Dual BSD/GPL");
MODULE_IMPORT_NS(SND_SOC_SOF_INTEL_HIFI_EP_IPC);
MODULE_IMPORT_NS(SND_SOC_SOF_XTENSA);
MODULE_IMPORT_NS(SND_SOC_SOF_ACPI_DEV);
