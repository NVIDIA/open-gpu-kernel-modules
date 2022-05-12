// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/arch/arm/mach-rpc/dma.c
 *
 *  Copyright (C) 1998 Russell King
 *
 *  DMA functions specific to RiscPC architecture
 */
#include <linux/mman.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>

#include <asm/page.h>
#include <asm/dma.h>
#include <asm/fiq.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <linux/uaccess.h>

#include <asm/mach/dma.h>
#include <asm/hardware/iomd.h>

struct iomd_dma {
	struct dma_struct	dma;
	void __iomem		*base;		/* Controller base address */
	int			irq;		/* Controller IRQ */
	unsigned int		state;
	dma_addr_t		cur_addr;
	unsigned int		cur_len;
	dma_addr_t		dma_addr;
	unsigned int		dma_len;
};

#if 0
typedef enum {
	dma_size_8	= 1,
	dma_size_16	= 2,
	dma_size_32	= 4,
	dma_size_128	= 16
} dma_size_t;
#endif

#define TRANSFER_SIZE	2

#define CURA	(0)
#define ENDA	(IOMD_IO0ENDA - IOMD_IO0CURA)
#define CURB	(IOMD_IO0CURB - IOMD_IO0CURA)
#define ENDB	(IOMD_IO0ENDB - IOMD_IO0CURA)
#define CR	(IOMD_IO0CR - IOMD_IO0CURA)
#define ST	(IOMD_IO0ST - IOMD_IO0CURA)

static void iomd_get_next_sg(struct iomd_dma *idma)
{
	unsigned long end, offset, flags = 0;

	if (idma->dma.sg) {
		idma->cur_addr = idma->dma_addr;
		offset = idma->cur_addr & ~PAGE_MASK;

		end = offset + idma->dma_len;

		if (end > PAGE_SIZE)
			end = PAGE_SIZE;

		if (offset + TRANSFER_SIZE >= end)
			flags |= DMA_END_L;

		idma->cur_len = end - TRANSFER_SIZE;

		idma->dma_len -= end - offset;
		idma->dma_addr += end - offset;

		if (idma->dma_len == 0) {
			if (idma->dma.sgcount > 1) {
				idma->dma.sg = sg_next(idma->dma.sg);
				idma->dma_addr = idma->dma.sg->dma_address;
				idma->dma_len = idma->dma.sg->length;
				idma->dma.sgcount--;
			} else {
				idma->dma.sg = NULL;
				flags |= DMA_END_S;
			}
		}
	} else {
		flags = DMA_END_S | DMA_END_L;
		idma->cur_addr = 0;
		idma->cur_len = 0;
	}

	idma->cur_len |= flags;
}

static irqreturn_t iomd_dma_handle(int irq, void *dev_id)
{
	struct iomd_dma *idma = dev_id;
	void __iomem *base = idma->base;
	unsigned int state = idma->state;
	unsigned int status, cur, end;

	do {
		status = readb(base + ST);
		if (!(status & DMA_ST_INT))
			goto out;

		if ((state ^ status) & DMA_ST_AB)
			iomd_get_next_sg(idma);

		// This efficiently implements state = OFL != AB ? AB : 0
		state = ((status >> 2) ^ status) & DMA_ST_AB;
		if (state) {
			cur = CURA;
			end = ENDA;
		} else {
			cur = CURB;
			end = ENDB;
		}
		writel(idma->cur_addr, base + cur);
		writel(idma->cur_len, base + end);

		if (status & DMA_ST_OFL &&
		    idma->cur_len == (DMA_END_S|DMA_END_L))
			break;
	} while (1);

	state = ~DMA_ST_AB;
	disable_irq_nosync(irq);
out:
	idma->state = state;
	return IRQ_HANDLED;
}

static int iomd_request_dma(unsigned int chan, dma_t *dma)
{
	struct iomd_dma *idma = container_of(dma, struct iomd_dma, dma);

	return request_irq(idma->irq, iomd_dma_handle,
			   0, idma->dma.device_id, idma);
}

static void iomd_free_dma(unsigned int chan, dma_t *dma)
{
	struct iomd_dma *idma = container_of(dma, struct iomd_dma, dma);

	free_irq(idma->irq, idma);
}

static struct device isa_dma_dev = {
	.init_name		= "fallback device",
	.coherent_dma_mask	= ~(dma_addr_t)0,
	.dma_mask		= &isa_dma_dev.coherent_dma_mask,
};

static void iomd_enable_dma(unsigned int chan, dma_t *dma)
{
	struct iomd_dma *idma = container_of(dma, struct iomd_dma, dma);
	void __iomem *base = idma->base;
	unsigned int ctrl = TRANSFER_SIZE | DMA_CR_E;

	if (idma->dma.invalid) {
		idma->dma.invalid = 0;

		/*
		 * Cope with ISA-style drivers which expect cache
		 * coherence.
		 */
		if (!idma->dma.sg) {
			idma->dma.sg = &idma->dma.buf;
			idma->dma.sgcount = 1;
			idma->dma.buf.length = idma->dma.count;
			idma->dma.buf.dma_address = dma_map_single(&isa_dma_dev,
				idma->dma.addr, idma->dma.count,
				idma->dma.dma_mode == DMA_MODE_READ ?
				DMA_FROM_DEVICE : DMA_TO_DEVICE);
		}

		idma->dma_addr = idma->dma.sg->dma_address;
		idma->dma_len = idma->dma.sg->length;

		writeb(DMA_CR_C, base + CR);
		idma->state = DMA_ST_AB;
	}

	if (idma->dma.dma_mode == DMA_MODE_READ)
		ctrl |= DMA_CR_D;

	writeb(ctrl, base + CR);
	enable_irq(idma->irq);
}

static void iomd_disable_dma(unsigned int chan, dma_t *dma)
{
	struct iomd_dma *idma = container_of(dma, struct iomd_dma, dma);
	void __iomem *base = idma->base;
	unsigned long flags;

	local_irq_save(flags);
	if (idma->state != ~DMA_ST_AB)
		disable_irq(idma->irq);
	writeb(0, base + CR);
	local_irq_restore(flags);
}

static int iomd_set_dma_speed(unsigned int chan, dma_t *dma, int cycle)
{
	int tcr, speed;

	if (cycle < 188)
		speed = 3;
	else if (cycle <= 250)
		speed = 2;
	else if (cycle < 438)
		speed = 1;
	else
		speed = 0;

	tcr = iomd_readb(IOMD_DMATCR);
	speed &= 3;

	switch (chan) {
	case DMA_0:
		tcr = (tcr & ~0x03) | speed;
		break;

	case DMA_1:
		tcr = (tcr & ~0x0c) | (speed << 2);
		break;

	case DMA_2:
		tcr = (tcr & ~0x30) | (speed << 4);
		break;

	case DMA_3:
		tcr = (tcr & ~0xc0) | (speed << 6);
		break;

	default:
		break;
	}

	iomd_writeb(tcr, IOMD_DMATCR);

	return speed;
}

static struct dma_ops iomd_dma_ops = {
	.type		= "IOMD",
	.request	= iomd_request_dma,
	.free		= iomd_free_dma,
	.enable		= iomd_enable_dma,
	.disable	= iomd_disable_dma,
	.setspeed	= iomd_set_dma_speed,
};

static struct fiq_handler fh = {
	.name	= "floppydma"
};

struct floppy_dma {
	struct dma_struct	dma;
	unsigned int		fiq;
};

static void floppy_enable_dma(unsigned int chan, dma_t *dma)
{
	struct floppy_dma *fdma = container_of(dma, struct floppy_dma, dma);
	void *fiqhandler_start;
	unsigned int fiqhandler_length;
	struct pt_regs regs;

	if (fdma->dma.sg)
		BUG();

	if (fdma->dma.dma_mode == DMA_MODE_READ) {
		extern unsigned char floppy_fiqin_start, floppy_fiqin_end;
		fiqhandler_start = &floppy_fiqin_start;
		fiqhandler_length = &floppy_fiqin_end - &floppy_fiqin_start;
	} else {
		extern unsigned char floppy_fiqout_start, floppy_fiqout_end;
		fiqhandler_start = &floppy_fiqout_start;
		fiqhandler_length = &floppy_fiqout_end - &floppy_fiqout_start;
	}

	regs.ARM_r9  = fdma->dma.count;
	regs.ARM_r10 = (unsigned long)fdma->dma.addr;
	regs.ARM_fp  = (unsigned long)FLOPPYDMA_BASE;

	if (claim_fiq(&fh)) {
		printk("floppydma: couldn't claim FIQ.\n");
		return;
	}

	set_fiq_handler(fiqhandler_start, fiqhandler_length);
	set_fiq_regs(&regs);
	enable_fiq(fdma->fiq);
}

static void floppy_disable_dma(unsigned int chan, dma_t *dma)
{
	struct floppy_dma *fdma = container_of(dma, struct floppy_dma, dma);
	disable_fiq(fdma->fiq);
	release_fiq(&fh);
}

static int floppy_get_residue(unsigned int chan, dma_t *dma)
{
	struct pt_regs regs;
	get_fiq_regs(&regs);
	return regs.ARM_r9;
}

static struct dma_ops floppy_dma_ops = {
	.type		= "FIQDMA",
	.enable		= floppy_enable_dma,
	.disable	= floppy_disable_dma,
	.residue	= floppy_get_residue,
};

/*
 * This is virtual DMA - we don't need anything here.
 */
static void sound_enable_disable_dma(unsigned int chan, dma_t *dma)
{
}

static struct dma_ops sound_dma_ops = {
	.type		= "VIRTUAL",
	.enable		= sound_enable_disable_dma,
	.disable	= sound_enable_disable_dma,
};

static struct iomd_dma iomd_dma[6];

static struct floppy_dma floppy_dma = {
	.dma		= {
		.d_ops	= &floppy_dma_ops,
	},
	.fiq		= FIQ_FLOPPYDATA,
};

static dma_t sound_dma = {
	.d_ops		= &sound_dma_ops,
};

static int __init rpc_dma_init(void)
{
	unsigned int i;
	int ret;

	iomd_writeb(0, IOMD_IO0CR);
	iomd_writeb(0, IOMD_IO1CR);
	iomd_writeb(0, IOMD_IO2CR);
	iomd_writeb(0, IOMD_IO3CR);

	iomd_writeb(0xa0, IOMD_DMATCR);

	/*
	 * Setup DMA channels 2,3 to be for podules
	 * and channels 0,1 for internal devices
	 */
	iomd_writeb(DMA_EXT_IO3|DMA_EXT_IO2, IOMD_DMAEXT);

	iomd_dma[DMA_0].base	= IOMD_BASE + IOMD_IO0CURA;
	iomd_dma[DMA_0].irq	= IRQ_DMA0;
	iomd_dma[DMA_1].base	= IOMD_BASE + IOMD_IO1CURA;
	iomd_dma[DMA_1].irq	= IRQ_DMA1;
	iomd_dma[DMA_2].base	= IOMD_BASE + IOMD_IO2CURA;
	iomd_dma[DMA_2].irq	= IRQ_DMA2;
	iomd_dma[DMA_3].base	= IOMD_BASE + IOMD_IO3CURA;
	iomd_dma[DMA_3].irq	= IRQ_DMA3;
	iomd_dma[DMA_S0].base	= IOMD_BASE + IOMD_SD0CURA;
	iomd_dma[DMA_S0].irq	= IRQ_DMAS0;
	iomd_dma[DMA_S1].base	= IOMD_BASE + IOMD_SD1CURA;
	iomd_dma[DMA_S1].irq	= IRQ_DMAS1;

	for (i = DMA_0; i <= DMA_S1; i++) {
		iomd_dma[i].dma.d_ops = &iomd_dma_ops;

		ret = isa_dma_add(i, &iomd_dma[i].dma);
		if (ret)
			printk("IOMDDMA%u: unable to register: %d\n", i, ret);
	}

	ret = isa_dma_add(DMA_VIRTUAL_FLOPPY, &floppy_dma.dma);
	if (ret)
		printk("IOMDFLOPPY: unable to register: %d\n", ret);
	ret = isa_dma_add(DMA_VIRTUAL_SOUND, &sound_dma);
	if (ret)
		printk("IOMDSOUND: unable to register: %d\n", ret);
	return 0;
}
core_initcall(rpc_dma_init);
