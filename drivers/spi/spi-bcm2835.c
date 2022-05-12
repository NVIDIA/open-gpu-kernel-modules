// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Broadcom BCM2835 SPI Controllers
 *
 * Copyright (C) 2012 Chris Boot
 * Copyright (C) 2013 Stephen Warren
 * Copyright (C) 2015 Martin Sperl
 *
 * This driver is inspired by:
 * spi-ath79.c, Copyright (C) 2009-2011 Gabor Juhos <juhosg@openwrt.org>
 * spi-atmel.c, Copyright (C) 2006 Atmel Corporation
 */

#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/machine.h> /* FIXME: using chip internals */
#include <linux/gpio/driver.h> /* FIXME: using chip internals */
#include <linux/of_irq.h>
#include <linux/spi/spi.h>

/* SPI register offsets */
#define BCM2835_SPI_CS			0x00
#define BCM2835_SPI_FIFO		0x04
#define BCM2835_SPI_CLK			0x08
#define BCM2835_SPI_DLEN		0x0c
#define BCM2835_SPI_LTOH		0x10
#define BCM2835_SPI_DC			0x14

/* Bitfields in CS */
#define BCM2835_SPI_CS_LEN_LONG		0x02000000
#define BCM2835_SPI_CS_DMA_LEN		0x01000000
#define BCM2835_SPI_CS_CSPOL2		0x00800000
#define BCM2835_SPI_CS_CSPOL1		0x00400000
#define BCM2835_SPI_CS_CSPOL0		0x00200000
#define BCM2835_SPI_CS_RXF		0x00100000
#define BCM2835_SPI_CS_RXR		0x00080000
#define BCM2835_SPI_CS_TXD		0x00040000
#define BCM2835_SPI_CS_RXD		0x00020000
#define BCM2835_SPI_CS_DONE		0x00010000
#define BCM2835_SPI_CS_LEN		0x00002000
#define BCM2835_SPI_CS_REN		0x00001000
#define BCM2835_SPI_CS_ADCS		0x00000800
#define BCM2835_SPI_CS_INTR		0x00000400
#define BCM2835_SPI_CS_INTD		0x00000200
#define BCM2835_SPI_CS_DMAEN		0x00000100
#define BCM2835_SPI_CS_TA		0x00000080
#define BCM2835_SPI_CS_CSPOL		0x00000040
#define BCM2835_SPI_CS_CLEAR_RX		0x00000020
#define BCM2835_SPI_CS_CLEAR_TX		0x00000010
#define BCM2835_SPI_CS_CPOL		0x00000008
#define BCM2835_SPI_CS_CPHA		0x00000004
#define BCM2835_SPI_CS_CS_10		0x00000002
#define BCM2835_SPI_CS_CS_01		0x00000001

#define BCM2835_SPI_FIFO_SIZE		64
#define BCM2835_SPI_FIFO_SIZE_3_4	48
#define BCM2835_SPI_DMA_MIN_LENGTH	96
#define BCM2835_SPI_NUM_CS		24  /* raise as necessary */
#define BCM2835_SPI_MODE_BITS	(SPI_CPOL | SPI_CPHA | SPI_CS_HIGH \
				| SPI_NO_CS | SPI_3WIRE)

#define DRV_NAME	"spi-bcm2835"

/* define polling limits */
static unsigned int polling_limit_us = 30;
module_param(polling_limit_us, uint, 0664);
MODULE_PARM_DESC(polling_limit_us,
		 "time in us to run a transfer in polling mode\n");

/**
 * struct bcm2835_spi - BCM2835 SPI controller
 * @regs: base address of register map
 * @clk: core clock, divided to calculate serial clock
 * @irq: interrupt, signals TX FIFO empty or RX FIFO ¾ full
 * @tfr: SPI transfer currently processed
 * @ctlr: SPI controller reverse lookup
 * @tx_buf: pointer whence next transmitted byte is read
 * @rx_buf: pointer where next received byte is written
 * @tx_len: remaining bytes to transmit
 * @rx_len: remaining bytes to receive
 * @tx_prologue: bytes transmitted without DMA if first TX sglist entry's
 *	length is not a multiple of 4 (to overcome hardware limitation)
 * @rx_prologue: bytes received without DMA if first RX sglist entry's
 *	length is not a multiple of 4 (to overcome hardware limitation)
 * @tx_spillover: whether @tx_prologue spills over to second TX sglist entry
 * @prepare_cs: precalculated CS register value for ->prepare_message()
 *	(uses slave-specific clock polarity and phase settings)
 * @debugfs_dir: the debugfs directory - neede to remove debugfs when
 *      unloading the module
 * @count_transfer_polling: count of how often polling mode is used
 * @count_transfer_irq: count of how often interrupt mode is used
 * @count_transfer_irq_after_polling: count of how often we fall back to
 *      interrupt mode after starting in polling mode.
 *      These are counted as well in @count_transfer_polling and
 *      @count_transfer_irq
 * @count_transfer_dma: count how often dma mode is used
 * @chip_select: SPI slave currently selected
 *	(used by bcm2835_spi_dma_tx_done() to write @clear_rx_cs)
 * @tx_dma_active: whether a TX DMA descriptor is in progress
 * @rx_dma_active: whether a RX DMA descriptor is in progress
 *	(used by bcm2835_spi_dma_tx_done() to handle a race)
 * @fill_tx_desc: preallocated TX DMA descriptor used for RX-only transfers
 *	(cyclically copies from zero page to TX FIFO)
 * @fill_tx_addr: bus address of zero page
 * @clear_rx_desc: preallocated RX DMA descriptor used for TX-only transfers
 *	(cyclically clears RX FIFO by writing @clear_rx_cs to CS register)
 * @clear_rx_addr: bus address of @clear_rx_cs
 * @clear_rx_cs: precalculated CS register value to clear RX FIFO
 *	(uses slave-specific clock polarity and phase settings)
 */
struct bcm2835_spi {
	void __iomem *regs;
	struct clk *clk;
	int irq;
	struct spi_transfer *tfr;
	struct spi_controller *ctlr;
	const u8 *tx_buf;
	u8 *rx_buf;
	int tx_len;
	int rx_len;
	int tx_prologue;
	int rx_prologue;
	unsigned int tx_spillover;
	u32 prepare_cs[BCM2835_SPI_NUM_CS];

	struct dentry *debugfs_dir;
	u64 count_transfer_polling;
	u64 count_transfer_irq;
	u64 count_transfer_irq_after_polling;
	u64 count_transfer_dma;

	u8 chip_select;
	unsigned int tx_dma_active;
	unsigned int rx_dma_active;
	struct dma_async_tx_descriptor *fill_tx_desc;
	dma_addr_t fill_tx_addr;
	struct dma_async_tx_descriptor *clear_rx_desc[BCM2835_SPI_NUM_CS];
	dma_addr_t clear_rx_addr;
	u32 clear_rx_cs[BCM2835_SPI_NUM_CS] ____cacheline_aligned;
};

#if defined(CONFIG_DEBUG_FS)
static void bcm2835_debugfs_create(struct bcm2835_spi *bs,
				   const char *dname)
{
	char name[64];
	struct dentry *dir;

	/* get full name */
	snprintf(name, sizeof(name), "spi-bcm2835-%s", dname);

	/* the base directory */
	dir = debugfs_create_dir(name, NULL);
	bs->debugfs_dir = dir;

	/* the counters */
	debugfs_create_u64("count_transfer_polling", 0444, dir,
			   &bs->count_transfer_polling);
	debugfs_create_u64("count_transfer_irq", 0444, dir,
			   &bs->count_transfer_irq);
	debugfs_create_u64("count_transfer_irq_after_polling", 0444, dir,
			   &bs->count_transfer_irq_after_polling);
	debugfs_create_u64("count_transfer_dma", 0444, dir,
			   &bs->count_transfer_dma);
}

static void bcm2835_debugfs_remove(struct bcm2835_spi *bs)
{
	debugfs_remove_recursive(bs->debugfs_dir);
	bs->debugfs_dir = NULL;
}
#else
static void bcm2835_debugfs_create(struct bcm2835_spi *bs,
				   const char *dname)
{
}

static void bcm2835_debugfs_remove(struct bcm2835_spi *bs)
{
}
#endif /* CONFIG_DEBUG_FS */

static inline u32 bcm2835_rd(struct bcm2835_spi *bs, unsigned int reg)
{
	return readl(bs->regs + reg);
}

static inline void bcm2835_wr(struct bcm2835_spi *bs, unsigned int reg, u32 val)
{
	writel(val, bs->regs + reg);
}

static inline void bcm2835_rd_fifo(struct bcm2835_spi *bs)
{
	u8 byte;

	while ((bs->rx_len) &&
	       (bcm2835_rd(bs, BCM2835_SPI_CS) & BCM2835_SPI_CS_RXD)) {
		byte = bcm2835_rd(bs, BCM2835_SPI_FIFO);
		if (bs->rx_buf)
			*bs->rx_buf++ = byte;
		bs->rx_len--;
	}
}

static inline void bcm2835_wr_fifo(struct bcm2835_spi *bs)
{
	u8 byte;

	while ((bs->tx_len) &&
	       (bcm2835_rd(bs, BCM2835_SPI_CS) & BCM2835_SPI_CS_TXD)) {
		byte = bs->tx_buf ? *bs->tx_buf++ : 0;
		bcm2835_wr(bs, BCM2835_SPI_FIFO, byte);
		bs->tx_len--;
	}
}

/**
 * bcm2835_rd_fifo_count() - blindly read exactly @count bytes from RX FIFO
 * @bs: BCM2835 SPI controller
 * @count: bytes to read from RX FIFO
 *
 * The caller must ensure that @bs->rx_len is greater than or equal to @count,
 * that the RX FIFO contains at least @count bytes and that the DMA Enable flag
 * in the CS register is set (such that a read from the FIFO register receives
 * 32-bit instead of just 8-bit).  Moreover @bs->rx_buf must not be %NULL.
 */
static inline void bcm2835_rd_fifo_count(struct bcm2835_spi *bs, int count)
{
	u32 val;
	int len;

	bs->rx_len -= count;

	do {
		val = bcm2835_rd(bs, BCM2835_SPI_FIFO);
		len = min(count, 4);
		memcpy(bs->rx_buf, &val, len);
		bs->rx_buf += len;
		count -= 4;
	} while (count > 0);
}

/**
 * bcm2835_wr_fifo_count() - blindly write exactly @count bytes to TX FIFO
 * @bs: BCM2835 SPI controller
 * @count: bytes to write to TX FIFO
 *
 * The caller must ensure that @bs->tx_len is greater than or equal to @count,
 * that the TX FIFO can accommodate @count bytes and that the DMA Enable flag
 * in the CS register is set (such that a write to the FIFO register transmits
 * 32-bit instead of just 8-bit).
 */
static inline void bcm2835_wr_fifo_count(struct bcm2835_spi *bs, int count)
{
	u32 val;
	int len;

	bs->tx_len -= count;

	do {
		if (bs->tx_buf) {
			len = min(count, 4);
			memcpy(&val, bs->tx_buf, len);
			bs->tx_buf += len;
		} else {
			val = 0;
		}
		bcm2835_wr(bs, BCM2835_SPI_FIFO, val);
		count -= 4;
	} while (count > 0);
}

/**
 * bcm2835_wait_tx_fifo_empty() - busy-wait for TX FIFO to empty
 * @bs: BCM2835 SPI controller
 *
 * The caller must ensure that the RX FIFO can accommodate as many bytes
 * as have been written to the TX FIFO:  Transmission is halted once the
 * RX FIFO is full, causing this function to spin forever.
 */
static inline void bcm2835_wait_tx_fifo_empty(struct bcm2835_spi *bs)
{
	while (!(bcm2835_rd(bs, BCM2835_SPI_CS) & BCM2835_SPI_CS_DONE))
		cpu_relax();
}

/**
 * bcm2835_rd_fifo_blind() - blindly read up to @count bytes from RX FIFO
 * @bs: BCM2835 SPI controller
 * @count: bytes available for reading in RX FIFO
 */
static inline void bcm2835_rd_fifo_blind(struct bcm2835_spi *bs, int count)
{
	u8 val;

	count = min(count, bs->rx_len);
	bs->rx_len -= count;

	do {
		val = bcm2835_rd(bs, BCM2835_SPI_FIFO);
		if (bs->rx_buf)
			*bs->rx_buf++ = val;
	} while (--count);
}

/**
 * bcm2835_wr_fifo_blind() - blindly write up to @count bytes to TX FIFO
 * @bs: BCM2835 SPI controller
 * @count: bytes available for writing in TX FIFO
 */
static inline void bcm2835_wr_fifo_blind(struct bcm2835_spi *bs, int count)
{
	u8 val;

	count = min(count, bs->tx_len);
	bs->tx_len -= count;

	do {
		val = bs->tx_buf ? *bs->tx_buf++ : 0;
		bcm2835_wr(bs, BCM2835_SPI_FIFO, val);
	} while (--count);
}

static void bcm2835_spi_reset_hw(struct bcm2835_spi *bs)
{
	u32 cs = bcm2835_rd(bs, BCM2835_SPI_CS);

	/* Disable SPI interrupts and transfer */
	cs &= ~(BCM2835_SPI_CS_INTR |
		BCM2835_SPI_CS_INTD |
		BCM2835_SPI_CS_DMAEN |
		BCM2835_SPI_CS_TA);
	/*
	 * Transmission sometimes breaks unless the DONE bit is written at the
	 * end of every transfer.  The spec says it's a RO bit.  Either the
	 * spec is wrong and the bit is actually of type RW1C, or it's a
	 * hardware erratum.
	 */
	cs |= BCM2835_SPI_CS_DONE;
	/* and reset RX/TX FIFOS */
	cs |= BCM2835_SPI_CS_CLEAR_RX | BCM2835_SPI_CS_CLEAR_TX;

	/* and reset the SPI_HW */
	bcm2835_wr(bs, BCM2835_SPI_CS, cs);
	/* as well as DLEN */
	bcm2835_wr(bs, BCM2835_SPI_DLEN, 0);
}

static irqreturn_t bcm2835_spi_interrupt(int irq, void *dev_id)
{
	struct bcm2835_spi *bs = dev_id;
	u32 cs = bcm2835_rd(bs, BCM2835_SPI_CS);

	/*
	 * An interrupt is signaled either if DONE is set (TX FIFO empty)
	 * or if RXR is set (RX FIFO >= ¾ full).
	 */
	if (cs & BCM2835_SPI_CS_RXF)
		bcm2835_rd_fifo_blind(bs, BCM2835_SPI_FIFO_SIZE);
	else if (cs & BCM2835_SPI_CS_RXR)
		bcm2835_rd_fifo_blind(bs, BCM2835_SPI_FIFO_SIZE_3_4);

	if (bs->tx_len && cs & BCM2835_SPI_CS_DONE)
		bcm2835_wr_fifo_blind(bs, BCM2835_SPI_FIFO_SIZE);

	/* Read as many bytes as possible from FIFO */
	bcm2835_rd_fifo(bs);
	/* Write as many bytes as possible to FIFO */
	bcm2835_wr_fifo(bs);

	if (!bs->rx_len) {
		/* Transfer complete - reset SPI HW */
		bcm2835_spi_reset_hw(bs);
		/* wake up the framework */
		spi_finalize_current_transfer(bs->ctlr);
	}

	return IRQ_HANDLED;
}

static int bcm2835_spi_transfer_one_irq(struct spi_controller *ctlr,
					struct spi_device *spi,
					struct spi_transfer *tfr,
					u32 cs, bool fifo_empty)
{
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);

	/* update usage statistics */
	bs->count_transfer_irq++;

	/*
	 * Enable HW block, but with interrupts still disabled.
	 * Otherwise the empty TX FIFO would immediately trigger an interrupt.
	 */
	bcm2835_wr(bs, BCM2835_SPI_CS, cs | BCM2835_SPI_CS_TA);

	/* fill TX FIFO as much as possible */
	if (fifo_empty)
		bcm2835_wr_fifo_blind(bs, BCM2835_SPI_FIFO_SIZE);
	bcm2835_wr_fifo(bs);

	/* enable interrupts */
	cs |= BCM2835_SPI_CS_INTR | BCM2835_SPI_CS_INTD | BCM2835_SPI_CS_TA;
	bcm2835_wr(bs, BCM2835_SPI_CS, cs);

	/* signal that we need to wait for completion */
	return 1;
}

/**
 * bcm2835_spi_transfer_prologue() - transfer first few bytes without DMA
 * @ctlr: SPI master controller
 * @tfr: SPI transfer
 * @bs: BCM2835 SPI controller
 * @cs: CS register
 *
 * A limitation in DMA mode is that the FIFO must be accessed in 4 byte chunks.
 * Only the final write access is permitted to transmit less than 4 bytes, the
 * SPI controller deduces its intended size from the DLEN register.
 *
 * If a TX or RX sglist contains multiple entries, one per page, and the first
 * entry starts in the middle of a page, that first entry's length may not be
 * a multiple of 4.  Subsequent entries are fine because they span an entire
 * page, hence do have a length that's a multiple of 4.
 *
 * This cannot happen with kmalloc'ed buffers (which is what most clients use)
 * because they are contiguous in physical memory and therefore not split on
 * page boundaries by spi_map_buf().  But it *can* happen with vmalloc'ed
 * buffers.
 *
 * The DMA engine is incapable of combining sglist entries into a continuous
 * stream of 4 byte chunks, it treats every entry separately:  A TX entry is
 * rounded up a to a multiple of 4 bytes by transmitting surplus bytes, an RX
 * entry is rounded up by throwing away received bytes.
 *
 * Overcome this limitation by transferring the first few bytes without DMA:
 * E.g. if the first TX sglist entry's length is 23 and the first RX's is 42,
 * write 3 bytes to the TX FIFO but read only 2 bytes from the RX FIFO.
 * The residue of 1 byte in the RX FIFO is picked up by DMA.  Together with
 * the rest of the first RX sglist entry it makes up a multiple of 4 bytes.
 *
 * Should the RX prologue be larger, say, 3 vis-à-vis a TX prologue of 1,
 * write 1 + 4 = 5 bytes to the TX FIFO and read 3 bytes from the RX FIFO.
 * Caution, the additional 4 bytes spill over to the second TX sglist entry
 * if the length of the first is *exactly* 1.
 *
 * At most 6 bytes are written and at most 3 bytes read.  Do we know the
 * transfer has this many bytes?  Yes, see BCM2835_SPI_DMA_MIN_LENGTH.
 *
 * The FIFO is normally accessed with 8-bit width by the CPU and 32-bit width
 * by the DMA engine.  Toggling the DMA Enable flag in the CS register switches
 * the width but also garbles the FIFO's contents.  The prologue must therefore
 * be transmitted in 32-bit width to ensure that the following DMA transfer can
 * pick up the residue in the RX FIFO in ungarbled form.
 */
static void bcm2835_spi_transfer_prologue(struct spi_controller *ctlr,
					  struct spi_transfer *tfr,
					  struct bcm2835_spi *bs,
					  u32 cs)
{
	int tx_remaining;

	bs->tfr		 = tfr;
	bs->tx_prologue  = 0;
	bs->rx_prologue  = 0;
	bs->tx_spillover = false;

	if (bs->tx_buf && !sg_is_last(&tfr->tx_sg.sgl[0]))
		bs->tx_prologue = sg_dma_len(&tfr->tx_sg.sgl[0]) & 3;

	if (bs->rx_buf && !sg_is_last(&tfr->rx_sg.sgl[0])) {
		bs->rx_prologue = sg_dma_len(&tfr->rx_sg.sgl[0]) & 3;

		if (bs->rx_prologue > bs->tx_prologue) {
			if (!bs->tx_buf || sg_is_last(&tfr->tx_sg.sgl[0])) {
				bs->tx_prologue  = bs->rx_prologue;
			} else {
				bs->tx_prologue += 4;
				bs->tx_spillover =
					!(sg_dma_len(&tfr->tx_sg.sgl[0]) & ~3);
			}
		}
	}

	/* rx_prologue > 0 implies tx_prologue > 0, so check only the latter */
	if (!bs->tx_prologue)
		return;

	/* Write and read RX prologue.  Adjust first entry in RX sglist. */
	if (bs->rx_prologue) {
		bcm2835_wr(bs, BCM2835_SPI_DLEN, bs->rx_prologue);
		bcm2835_wr(bs, BCM2835_SPI_CS, cs | BCM2835_SPI_CS_TA
						  | BCM2835_SPI_CS_DMAEN);
		bcm2835_wr_fifo_count(bs, bs->rx_prologue);
		bcm2835_wait_tx_fifo_empty(bs);
		bcm2835_rd_fifo_count(bs, bs->rx_prologue);
		bcm2835_wr(bs, BCM2835_SPI_CS, cs | BCM2835_SPI_CS_CLEAR_RX
						  | BCM2835_SPI_CS_CLEAR_TX
						  | BCM2835_SPI_CS_DONE);

		dma_sync_single_for_device(ctlr->dma_rx->device->dev,
					   sg_dma_address(&tfr->rx_sg.sgl[0]),
					   bs->rx_prologue, DMA_FROM_DEVICE);

		sg_dma_address(&tfr->rx_sg.sgl[0]) += bs->rx_prologue;
		sg_dma_len(&tfr->rx_sg.sgl[0])     -= bs->rx_prologue;
	}

	if (!bs->tx_buf)
		return;

	/*
	 * Write remaining TX prologue.  Adjust first entry in TX sglist.
	 * Also adjust second entry if prologue spills over to it.
	 */
	tx_remaining = bs->tx_prologue - bs->rx_prologue;
	if (tx_remaining) {
		bcm2835_wr(bs, BCM2835_SPI_DLEN, tx_remaining);
		bcm2835_wr(bs, BCM2835_SPI_CS, cs | BCM2835_SPI_CS_TA
						  | BCM2835_SPI_CS_DMAEN);
		bcm2835_wr_fifo_count(bs, tx_remaining);
		bcm2835_wait_tx_fifo_empty(bs);
		bcm2835_wr(bs, BCM2835_SPI_CS, cs | BCM2835_SPI_CS_CLEAR_TX
						  | BCM2835_SPI_CS_DONE);
	}

	if (likely(!bs->tx_spillover)) {
		sg_dma_address(&tfr->tx_sg.sgl[0]) += bs->tx_prologue;
		sg_dma_len(&tfr->tx_sg.sgl[0])     -= bs->tx_prologue;
	} else {
		sg_dma_len(&tfr->tx_sg.sgl[0])      = 0;
		sg_dma_address(&tfr->tx_sg.sgl[1]) += 4;
		sg_dma_len(&tfr->tx_sg.sgl[1])     -= 4;
	}
}

/**
 * bcm2835_spi_undo_prologue() - reconstruct original sglist state
 * @bs: BCM2835 SPI controller
 *
 * Undo changes which were made to an SPI transfer's sglist when transmitting
 * the prologue.  This is necessary to ensure the same memory ranges are
 * unmapped that were originally mapped.
 */
static void bcm2835_spi_undo_prologue(struct bcm2835_spi *bs)
{
	struct spi_transfer *tfr = bs->tfr;

	if (!bs->tx_prologue)
		return;

	if (bs->rx_prologue) {
		sg_dma_address(&tfr->rx_sg.sgl[0]) -= bs->rx_prologue;
		sg_dma_len(&tfr->rx_sg.sgl[0])     += bs->rx_prologue;
	}

	if (!bs->tx_buf)
		goto out;

	if (likely(!bs->tx_spillover)) {
		sg_dma_address(&tfr->tx_sg.sgl[0]) -= bs->tx_prologue;
		sg_dma_len(&tfr->tx_sg.sgl[0])     += bs->tx_prologue;
	} else {
		sg_dma_len(&tfr->tx_sg.sgl[0])      = bs->tx_prologue - 4;
		sg_dma_address(&tfr->tx_sg.sgl[1]) -= 4;
		sg_dma_len(&tfr->tx_sg.sgl[1])     += 4;
	}
out:
	bs->tx_prologue = 0;
}

/**
 * bcm2835_spi_dma_rx_done() - callback for DMA RX channel
 * @data: SPI master controller
 *
 * Used for bidirectional and RX-only transfers.
 */
static void bcm2835_spi_dma_rx_done(void *data)
{
	struct spi_controller *ctlr = data;
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);

	/* terminate tx-dma as we do not have an irq for it
	 * because when the rx dma will terminate and this callback
	 * is called the tx-dma must have finished - can't get to this
	 * situation otherwise...
	 */
	dmaengine_terminate_async(ctlr->dma_tx);
	bs->tx_dma_active = false;
	bs->rx_dma_active = false;
	bcm2835_spi_undo_prologue(bs);

	/* reset fifo and HW */
	bcm2835_spi_reset_hw(bs);

	/* and mark as completed */;
	spi_finalize_current_transfer(ctlr);
}

/**
 * bcm2835_spi_dma_tx_done() - callback for DMA TX channel
 * @data: SPI master controller
 *
 * Used for TX-only transfers.
 */
static void bcm2835_spi_dma_tx_done(void *data)
{
	struct spi_controller *ctlr = data;
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);

	/* busy-wait for TX FIFO to empty */
	while (!(bcm2835_rd(bs, BCM2835_SPI_CS) & BCM2835_SPI_CS_DONE))
		bcm2835_wr(bs, BCM2835_SPI_CS,
			   bs->clear_rx_cs[bs->chip_select]);

	bs->tx_dma_active = false;
	smp_wmb();

	/*
	 * In case of a very short transfer, RX DMA may not have been
	 * issued yet.  The onus is then on bcm2835_spi_transfer_one_dma()
	 * to terminate it immediately after issuing.
	 */
	if (cmpxchg(&bs->rx_dma_active, true, false))
		dmaengine_terminate_async(ctlr->dma_rx);

	bcm2835_spi_undo_prologue(bs);
	bcm2835_spi_reset_hw(bs);
	spi_finalize_current_transfer(ctlr);
}

/**
 * bcm2835_spi_prepare_sg() - prepare and submit DMA descriptor for sglist
 * @ctlr: SPI master controller
 * @spi: SPI slave
 * @tfr: SPI transfer
 * @bs: BCM2835 SPI controller
 * @is_tx: whether to submit DMA descriptor for TX or RX sglist
 *
 * Prepare and submit a DMA descriptor for the TX or RX sglist of @tfr.
 * Return 0 on success or a negative error number.
 */
static int bcm2835_spi_prepare_sg(struct spi_controller *ctlr,
				  struct spi_device *spi,
				  struct spi_transfer *tfr,
				  struct bcm2835_spi *bs,
				  bool is_tx)
{
	struct dma_chan *chan;
	struct scatterlist *sgl;
	unsigned int nents;
	enum dma_transfer_direction dir;
	unsigned long flags;

	struct dma_async_tx_descriptor *desc;
	dma_cookie_t cookie;

	if (is_tx) {
		dir   = DMA_MEM_TO_DEV;
		chan  = ctlr->dma_tx;
		nents = tfr->tx_sg.nents;
		sgl   = tfr->tx_sg.sgl;
		flags = tfr->rx_buf ? 0 : DMA_PREP_INTERRUPT;
	} else {
		dir   = DMA_DEV_TO_MEM;
		chan  = ctlr->dma_rx;
		nents = tfr->rx_sg.nents;
		sgl   = tfr->rx_sg.sgl;
		flags = DMA_PREP_INTERRUPT;
	}
	/* prepare the channel */
	desc = dmaengine_prep_slave_sg(chan, sgl, nents, dir, flags);
	if (!desc)
		return -EINVAL;

	/*
	 * Completion is signaled by the RX channel for bidirectional and
	 * RX-only transfers; else by the TX channel for TX-only transfers.
	 */
	if (!is_tx) {
		desc->callback = bcm2835_spi_dma_rx_done;
		desc->callback_param = ctlr;
	} else if (!tfr->rx_buf) {
		desc->callback = bcm2835_spi_dma_tx_done;
		desc->callback_param = ctlr;
		bs->chip_select = spi->chip_select;
	}

	/* submit it to DMA-engine */
	cookie = dmaengine_submit(desc);

	return dma_submit_error(cookie);
}

/**
 * bcm2835_spi_transfer_one_dma() - perform SPI transfer using DMA engine
 * @ctlr: SPI master controller
 * @spi: SPI slave
 * @tfr: SPI transfer
 * @cs: CS register
 *
 * For *bidirectional* transfers (both tx_buf and rx_buf are non-%NULL), set up
 * the TX and RX DMA channel to copy between memory and FIFO register.
 *
 * For *TX-only* transfers (rx_buf is %NULL), copying the RX FIFO's contents to
 * memory is pointless.  However not reading the RX FIFO isn't an option either
 * because transmission is halted once it's full.  As a workaround, cyclically
 * clear the RX FIFO by setting the CLEAR_RX bit in the CS register.
 *
 * The CS register value is precalculated in bcm2835_spi_setup().  Normally
 * this is called only once, on slave registration.  A DMA descriptor to write
 * this value is preallocated in bcm2835_dma_init().  All that's left to do
 * when performing a TX-only transfer is to submit this descriptor to the RX
 * DMA channel.  Latency is thereby minimized.  The descriptor does not
 * generate any interrupts while running.  It must be terminated once the
 * TX DMA channel is done.
 *
 * Clearing the RX FIFO is paced by the DREQ signal.  The signal is asserted
 * when the RX FIFO becomes half full, i.e. 32 bytes.  (Tuneable with the DC
 * register.)  Reading 32 bytes from the RX FIFO would normally require 8 bus
 * accesses, whereas clearing it requires only 1 bus access.  So an 8-fold
 * reduction in bus traffic and thus energy consumption is achieved.
 *
 * For *RX-only* transfers (tx_buf is %NULL), fill the TX FIFO by cyclically
 * copying from the zero page.  The DMA descriptor to do this is preallocated
 * in bcm2835_dma_init().  It must be terminated once the RX DMA channel is
 * done and can then be reused.
 *
 * The BCM2835 DMA driver autodetects when a transaction copies from the zero
 * page and utilizes the DMA controller's ability to synthesize zeroes instead
 * of copying them from memory.  This reduces traffic on the memory bus.  The
 * feature is not available on so-called "lite" channels, but normally TX DMA
 * is backed by a full-featured channel.
 *
 * Zero-filling the TX FIFO is paced by the DREQ signal.  Unfortunately the
 * BCM2835 SPI controller continues to assert DREQ even after the DLEN register
 * has been counted down to zero (hardware erratum).  Thus, when the transfer
 * has finished, the DMA engine zero-fills the TX FIFO until it is half full.
 * (Tuneable with the DC register.)  So up to 9 gratuitous bus accesses are
 * performed at the end of an RX-only transfer.
 */
static int bcm2835_spi_transfer_one_dma(struct spi_controller *ctlr,
					struct spi_device *spi,
					struct spi_transfer *tfr,
					u32 cs)
{
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);
	dma_cookie_t cookie;
	int ret;

	/* update usage statistics */
	bs->count_transfer_dma++;

	/*
	 * Transfer first few bytes without DMA if length of first TX or RX
	 * sglist entry is not a multiple of 4 bytes (hardware limitation).
	 */
	bcm2835_spi_transfer_prologue(ctlr, tfr, bs, cs);

	/* setup tx-DMA */
	if (bs->tx_buf) {
		ret = bcm2835_spi_prepare_sg(ctlr, spi, tfr, bs, true);
	} else {
		cookie = dmaengine_submit(bs->fill_tx_desc);
		ret = dma_submit_error(cookie);
	}
	if (ret)
		goto err_reset_hw;

	/* set the DMA length */
	bcm2835_wr(bs, BCM2835_SPI_DLEN, bs->tx_len);

	/* start the HW */
	bcm2835_wr(bs, BCM2835_SPI_CS,
		   cs | BCM2835_SPI_CS_TA | BCM2835_SPI_CS_DMAEN);

	bs->tx_dma_active = true;
	smp_wmb();

	/* start TX early */
	dma_async_issue_pending(ctlr->dma_tx);

	/* setup rx-DMA late - to run transfers while
	 * mapping of the rx buffers still takes place
	 * this saves 10us or more.
	 */
	if (bs->rx_buf) {
		ret = bcm2835_spi_prepare_sg(ctlr, spi, tfr, bs, false);
	} else {
		cookie = dmaengine_submit(bs->clear_rx_desc[spi->chip_select]);
		ret = dma_submit_error(cookie);
	}
	if (ret) {
		/* need to reset on errors */
		dmaengine_terminate_sync(ctlr->dma_tx);
		bs->tx_dma_active = false;
		goto err_reset_hw;
	}

	/* start rx dma late */
	dma_async_issue_pending(ctlr->dma_rx);
	bs->rx_dma_active = true;
	smp_mb();

	/*
	 * In case of a very short TX-only transfer, bcm2835_spi_dma_tx_done()
	 * may run before RX DMA is issued.  Terminate RX DMA if so.
	 */
	if (!bs->rx_buf && !bs->tx_dma_active &&
	    cmpxchg(&bs->rx_dma_active, true, false)) {
		dmaengine_terminate_async(ctlr->dma_rx);
		bcm2835_spi_reset_hw(bs);
	}

	/* wait for wakeup in framework */
	return 1;

err_reset_hw:
	bcm2835_spi_reset_hw(bs);
	bcm2835_spi_undo_prologue(bs);
	return ret;
}

static bool bcm2835_spi_can_dma(struct spi_controller *ctlr,
				struct spi_device *spi,
				struct spi_transfer *tfr)
{
	/* we start DMA efforts only on bigger transfers */
	if (tfr->len < BCM2835_SPI_DMA_MIN_LENGTH)
		return false;

	/* return OK */
	return true;
}

static void bcm2835_dma_release(struct spi_controller *ctlr,
				struct bcm2835_spi *bs)
{
	int i;

	if (ctlr->dma_tx) {
		dmaengine_terminate_sync(ctlr->dma_tx);

		if (bs->fill_tx_desc)
			dmaengine_desc_free(bs->fill_tx_desc);

		if (bs->fill_tx_addr)
			dma_unmap_page_attrs(ctlr->dma_tx->device->dev,
					     bs->fill_tx_addr, sizeof(u32),
					     DMA_TO_DEVICE,
					     DMA_ATTR_SKIP_CPU_SYNC);

		dma_release_channel(ctlr->dma_tx);
		ctlr->dma_tx = NULL;
	}

	if (ctlr->dma_rx) {
		dmaengine_terminate_sync(ctlr->dma_rx);

		for (i = 0; i < BCM2835_SPI_NUM_CS; i++)
			if (bs->clear_rx_desc[i])
				dmaengine_desc_free(bs->clear_rx_desc[i]);

		if (bs->clear_rx_addr)
			dma_unmap_single(ctlr->dma_rx->device->dev,
					 bs->clear_rx_addr,
					 sizeof(bs->clear_rx_cs),
					 DMA_TO_DEVICE);

		dma_release_channel(ctlr->dma_rx);
		ctlr->dma_rx = NULL;
	}
}

static int bcm2835_dma_init(struct spi_controller *ctlr, struct device *dev,
			    struct bcm2835_spi *bs)
{
	struct dma_slave_config slave_config;
	const __be32 *addr;
	dma_addr_t dma_reg_base;
	int ret, i;

	/* base address in dma-space */
	addr = of_get_address(ctlr->dev.of_node, 0, NULL, NULL);
	if (!addr) {
		dev_err(dev, "could not get DMA-register address - not using dma mode\n");
		/* Fall back to interrupt mode */
		return 0;
	}
	dma_reg_base = be32_to_cpup(addr);

	/* get tx/rx dma */
	ctlr->dma_tx = dma_request_chan(dev, "tx");
	if (IS_ERR(ctlr->dma_tx)) {
		dev_err(dev, "no tx-dma configuration found - not using dma mode\n");
		ret = PTR_ERR(ctlr->dma_tx);
		ctlr->dma_tx = NULL;
		goto err;
	}
	ctlr->dma_rx = dma_request_chan(dev, "rx");
	if (IS_ERR(ctlr->dma_rx)) {
		dev_err(dev, "no rx-dma configuration found - not using dma mode\n");
		ret = PTR_ERR(ctlr->dma_rx);
		ctlr->dma_rx = NULL;
		goto err_release;
	}

	/*
	 * The TX DMA channel either copies a transfer's TX buffer to the FIFO
	 * or, in case of an RX-only transfer, cyclically copies from the zero
	 * page to the FIFO using a preallocated, reusable descriptor.
	 */
	slave_config.dst_addr = (u32)(dma_reg_base + BCM2835_SPI_FIFO);
	slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;

	ret = dmaengine_slave_config(ctlr->dma_tx, &slave_config);
	if (ret)
		goto err_config;

	bs->fill_tx_addr = dma_map_page_attrs(ctlr->dma_tx->device->dev,
					      ZERO_PAGE(0), 0, sizeof(u32),
					      DMA_TO_DEVICE,
					      DMA_ATTR_SKIP_CPU_SYNC);
	if (dma_mapping_error(ctlr->dma_tx->device->dev, bs->fill_tx_addr)) {
		dev_err(dev, "cannot map zero page - not using DMA mode\n");
		bs->fill_tx_addr = 0;
		ret = -ENOMEM;
		goto err_release;
	}

	bs->fill_tx_desc = dmaengine_prep_dma_cyclic(ctlr->dma_tx,
						     bs->fill_tx_addr,
						     sizeof(u32), 0,
						     DMA_MEM_TO_DEV, 0);
	if (!bs->fill_tx_desc) {
		dev_err(dev, "cannot prepare fill_tx_desc - not using DMA mode\n");
		ret = -ENOMEM;
		goto err_release;
	}

	ret = dmaengine_desc_set_reuse(bs->fill_tx_desc);
	if (ret) {
		dev_err(dev, "cannot reuse fill_tx_desc - not using DMA mode\n");
		goto err_release;
	}

	/*
	 * The RX DMA channel is used bidirectionally:  It either reads the
	 * RX FIFO or, in case of a TX-only transfer, cyclically writes a
	 * precalculated value to the CS register to clear the RX FIFO.
	 */
	slave_config.src_addr = (u32)(dma_reg_base + BCM2835_SPI_FIFO);
	slave_config.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	slave_config.dst_addr = (u32)(dma_reg_base + BCM2835_SPI_CS);
	slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;

	ret = dmaengine_slave_config(ctlr->dma_rx, &slave_config);
	if (ret)
		goto err_config;

	bs->clear_rx_addr = dma_map_single(ctlr->dma_rx->device->dev,
					   bs->clear_rx_cs,
					   sizeof(bs->clear_rx_cs),
					   DMA_TO_DEVICE);
	if (dma_mapping_error(ctlr->dma_rx->device->dev, bs->clear_rx_addr)) {
		dev_err(dev, "cannot map clear_rx_cs - not using DMA mode\n");
		bs->clear_rx_addr = 0;
		ret = -ENOMEM;
		goto err_release;
	}

	for (i = 0; i < BCM2835_SPI_NUM_CS; i++) {
		bs->clear_rx_desc[i] = dmaengine_prep_dma_cyclic(ctlr->dma_rx,
					   bs->clear_rx_addr + i * sizeof(u32),
					   sizeof(u32), 0,
					   DMA_MEM_TO_DEV, 0);
		if (!bs->clear_rx_desc[i]) {
			dev_err(dev, "cannot prepare clear_rx_desc - not using DMA mode\n");
			ret = -ENOMEM;
			goto err_release;
		}

		ret = dmaengine_desc_set_reuse(bs->clear_rx_desc[i]);
		if (ret) {
			dev_err(dev, "cannot reuse clear_rx_desc - not using DMA mode\n");
			goto err_release;
		}
	}

	/* all went well, so set can_dma */
	ctlr->can_dma = bcm2835_spi_can_dma;

	return 0;

err_config:
	dev_err(dev, "issue configuring dma: %d - not using DMA mode\n",
		ret);
err_release:
	bcm2835_dma_release(ctlr, bs);
err:
	/*
	 * Only report error for deferred probing, otherwise fall back to
	 * interrupt mode
	 */
	if (ret != -EPROBE_DEFER)
		ret = 0;

	return ret;
}

static int bcm2835_spi_transfer_one_poll(struct spi_controller *ctlr,
					 struct spi_device *spi,
					 struct spi_transfer *tfr,
					 u32 cs)
{
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);
	unsigned long timeout;

	/* update usage statistics */
	bs->count_transfer_polling++;

	/* enable HW block without interrupts */
	bcm2835_wr(bs, BCM2835_SPI_CS, cs | BCM2835_SPI_CS_TA);

	/* fill in the fifo before timeout calculations
	 * if we are interrupted here, then the data is
	 * getting transferred by the HW while we are interrupted
	 */
	bcm2835_wr_fifo_blind(bs, BCM2835_SPI_FIFO_SIZE);

	/* set the timeout to at least 2 jiffies */
	timeout = jiffies + 2 + HZ * polling_limit_us / 1000000;

	/* loop until finished the transfer */
	while (bs->rx_len) {
		/* fill in tx fifo with remaining data */
		bcm2835_wr_fifo(bs);

		/* read from fifo as much as possible */
		bcm2835_rd_fifo(bs);

		/* if there is still data pending to read
		 * then check the timeout
		 */
		if (bs->rx_len && time_after(jiffies, timeout)) {
			dev_dbg_ratelimited(&spi->dev,
					    "timeout period reached: jiffies: %lu remaining tx/rx: %d/%d - falling back to interrupt mode\n",
					    jiffies - timeout,
					    bs->tx_len, bs->rx_len);
			/* fall back to interrupt mode */

			/* update usage statistics */
			bs->count_transfer_irq_after_polling++;

			return bcm2835_spi_transfer_one_irq(ctlr, spi,
							    tfr, cs, false);
		}
	}

	/* Transfer complete - reset SPI HW */
	bcm2835_spi_reset_hw(bs);
	/* and return without waiting for completion */
	return 0;
}

static int bcm2835_spi_transfer_one(struct spi_controller *ctlr,
				    struct spi_device *spi,
				    struct spi_transfer *tfr)
{
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);
	unsigned long spi_hz, clk_hz, cdiv;
	unsigned long hz_per_byte, byte_limit;
	u32 cs = bs->prepare_cs[spi->chip_select];

	/* set clock */
	spi_hz = tfr->speed_hz;
	clk_hz = clk_get_rate(bs->clk);

	if (spi_hz >= clk_hz / 2) {
		cdiv = 2; /* clk_hz/2 is the fastest we can go */
	} else if (spi_hz) {
		/* CDIV must be a multiple of two */
		cdiv = DIV_ROUND_UP(clk_hz, spi_hz);
		cdiv += (cdiv % 2);

		if (cdiv >= 65536)
			cdiv = 0; /* 0 is the slowest we can go */
	} else {
		cdiv = 0; /* 0 is the slowest we can go */
	}
	tfr->effective_speed_hz = cdiv ? (clk_hz / cdiv) : (clk_hz / 65536);
	bcm2835_wr(bs, BCM2835_SPI_CLK, cdiv);

	/* handle all the 3-wire mode */
	if (spi->mode & SPI_3WIRE && tfr->rx_buf)
		cs |= BCM2835_SPI_CS_REN;

	/* set transmit buffers and length */
	bs->tx_buf = tfr->tx_buf;
	bs->rx_buf = tfr->rx_buf;
	bs->tx_len = tfr->len;
	bs->rx_len = tfr->len;

	/* Calculate the estimated time in us the transfer runs.  Note that
	 * there is 1 idle clocks cycles after each byte getting transferred
	 * so we have 9 cycles/byte.  This is used to find the number of Hz
	 * per byte per polling limit.  E.g., we can transfer 1 byte in 30 us
	 * per 300,000 Hz of bus clock.
	 */
	hz_per_byte = polling_limit_us ? (9 * 1000000) / polling_limit_us : 0;
	byte_limit = hz_per_byte ? tfr->effective_speed_hz / hz_per_byte : 1;

	/* run in polling mode for short transfers */
	if (tfr->len < byte_limit)
		return bcm2835_spi_transfer_one_poll(ctlr, spi, tfr, cs);

	/* run in dma mode if conditions are right
	 * Note that unlike poll or interrupt mode DMA mode does not have
	 * this 1 idle clock cycle pattern but runs the spi clock without gaps
	 */
	if (ctlr->can_dma && bcm2835_spi_can_dma(ctlr, spi, tfr))
		return bcm2835_spi_transfer_one_dma(ctlr, spi, tfr, cs);

	/* run in interrupt-mode */
	return bcm2835_spi_transfer_one_irq(ctlr, spi, tfr, cs, true);
}

static int bcm2835_spi_prepare_message(struct spi_controller *ctlr,
				       struct spi_message *msg)
{
	struct spi_device *spi = msg->spi;
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);
	int ret;

	if (ctlr->can_dma) {
		/*
		 * DMA transfers are limited to 16 bit (0 to 65535 bytes) by
		 * the SPI HW due to DLEN. Split up transfers (32-bit FIFO
		 * aligned) if the limit is exceeded.
		 */
		ret = spi_split_transfers_maxsize(ctlr, msg, 65532,
						  GFP_KERNEL | GFP_DMA);
		if (ret)
			return ret;
	}

	/*
	 * Set up clock polarity before spi_transfer_one_message() asserts
	 * chip select to avoid a gratuitous clock signal edge.
	 */
	bcm2835_wr(bs, BCM2835_SPI_CS, bs->prepare_cs[spi->chip_select]);

	return 0;
}

static void bcm2835_spi_handle_err(struct spi_controller *ctlr,
				   struct spi_message *msg)
{
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);

	/* if an error occurred and we have an active dma, then terminate */
	dmaengine_terminate_sync(ctlr->dma_tx);
	bs->tx_dma_active = false;
	dmaengine_terminate_sync(ctlr->dma_rx);
	bs->rx_dma_active = false;
	bcm2835_spi_undo_prologue(bs);

	/* and reset */
	bcm2835_spi_reset_hw(bs);
}

static int chip_match_name(struct gpio_chip *chip, void *data)
{
	return !strcmp(chip->label, data);
}

static int bcm2835_spi_setup(struct spi_device *spi)
{
	struct spi_controller *ctlr = spi->controller;
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);
	struct gpio_chip *chip;
	u32 cs;

	if (spi->chip_select >= BCM2835_SPI_NUM_CS) {
		dev_err(&spi->dev, "only %d chip-selects supported\n",
			BCM2835_SPI_NUM_CS - 1);
		return -EINVAL;
	}

	/*
	 * Precalculate SPI slave's CS register value for ->prepare_message():
	 * The driver always uses software-controlled GPIO chip select, hence
	 * set the hardware-controlled native chip select to an invalid value
	 * to prevent it from interfering.
	 */
	cs = BCM2835_SPI_CS_CS_10 | BCM2835_SPI_CS_CS_01;
	if (spi->mode & SPI_CPOL)
		cs |= BCM2835_SPI_CS_CPOL;
	if (spi->mode & SPI_CPHA)
		cs |= BCM2835_SPI_CS_CPHA;
	bs->prepare_cs[spi->chip_select] = cs;

	/*
	 * Precalculate SPI slave's CS register value to clear RX FIFO
	 * in case of a TX-only DMA transfer.
	 */
	if (ctlr->dma_rx) {
		bs->clear_rx_cs[spi->chip_select] = cs |
						    BCM2835_SPI_CS_TA |
						    BCM2835_SPI_CS_DMAEN |
						    BCM2835_SPI_CS_CLEAR_RX;
		dma_sync_single_for_device(ctlr->dma_rx->device->dev,
					   bs->clear_rx_addr,
					   sizeof(bs->clear_rx_cs),
					   DMA_TO_DEVICE);
	}

	/*
	 * sanity checking the native-chipselects
	 */
	if (spi->mode & SPI_NO_CS)
		return 0;
	/*
	 * The SPI core has successfully requested the CS GPIO line from the
	 * device tree, so we are done.
	 */
	if (spi->cs_gpiod)
		return 0;
	if (spi->chip_select > 1) {
		/* error in the case of native CS requested with CS > 1
		 * officially there is a CS2, but it is not documented
		 * which GPIO is connected with that...
		 */
		dev_err(&spi->dev,
			"setup: only two native chip-selects are supported\n");
		return -EINVAL;
	}

	/*
	 * Translate native CS to GPIO
	 *
	 * FIXME: poking around in the gpiolib internals like this is
	 * not very good practice. Find a way to locate the real problem
	 * and fix it. Why is the GPIO descriptor in spi->cs_gpiod
	 * sometimes not assigned correctly? Erroneous device trees?
	 */

	/* get the gpio chip for the base */
	chip = gpiochip_find("pinctrl-bcm2835", chip_match_name);
	if (!chip)
		return 0;

	spi->cs_gpiod = gpiochip_request_own_desc(chip, 8 - spi->chip_select,
						  DRV_NAME,
						  GPIO_LOOKUP_FLAGS_DEFAULT,
						  GPIOD_OUT_LOW);
	if (IS_ERR(spi->cs_gpiod))
		return PTR_ERR(spi->cs_gpiod);

	/* and set up the "mode" and level */
	dev_info(&spi->dev, "setting up native-CS%i to use GPIO\n",
		 spi->chip_select);

	return 0;
}

static int bcm2835_spi_probe(struct platform_device *pdev)
{
	struct spi_controller *ctlr;
	struct bcm2835_spi *bs;
	int err;

	ctlr = devm_spi_alloc_master(&pdev->dev, ALIGN(sizeof(*bs),
						  dma_get_cache_alignment()));
	if (!ctlr)
		return -ENOMEM;

	platform_set_drvdata(pdev, ctlr);

	ctlr->use_gpio_descriptors = true;
	ctlr->mode_bits = BCM2835_SPI_MODE_BITS;
	ctlr->bits_per_word_mask = SPI_BPW_MASK(8);
	ctlr->num_chipselect = 3;
	ctlr->setup = bcm2835_spi_setup;
	ctlr->transfer_one = bcm2835_spi_transfer_one;
	ctlr->handle_err = bcm2835_spi_handle_err;
	ctlr->prepare_message = bcm2835_spi_prepare_message;
	ctlr->dev.of_node = pdev->dev.of_node;

	bs = spi_controller_get_devdata(ctlr);
	bs->ctlr = ctlr;

	bs->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(bs->regs))
		return PTR_ERR(bs->regs);

	bs->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(bs->clk))
		return dev_err_probe(&pdev->dev, PTR_ERR(bs->clk),
				     "could not get clk\n");

	ctlr->max_speed_hz = clk_get_rate(bs->clk) / 2;

	bs->irq = platform_get_irq(pdev, 0);
	if (bs->irq <= 0)
		return bs->irq ? bs->irq : -ENODEV;

	clk_prepare_enable(bs->clk);

	err = bcm2835_dma_init(ctlr, &pdev->dev, bs);
	if (err)
		goto out_clk_disable;

	/* initialise the hardware with the default polarities */
	bcm2835_wr(bs, BCM2835_SPI_CS,
		   BCM2835_SPI_CS_CLEAR_RX | BCM2835_SPI_CS_CLEAR_TX);

	err = devm_request_irq(&pdev->dev, bs->irq, bcm2835_spi_interrupt, 0,
			       dev_name(&pdev->dev), bs);
	if (err) {
		dev_err(&pdev->dev, "could not request IRQ: %d\n", err);
		goto out_dma_release;
	}

	err = spi_register_controller(ctlr);
	if (err) {
		dev_err(&pdev->dev, "could not register SPI controller: %d\n",
			err);
		goto out_dma_release;
	}

	bcm2835_debugfs_create(bs, dev_name(&pdev->dev));

	return 0;

out_dma_release:
	bcm2835_dma_release(ctlr, bs);
out_clk_disable:
	clk_disable_unprepare(bs->clk);
	return err;
}

static int bcm2835_spi_remove(struct platform_device *pdev)
{
	struct spi_controller *ctlr = platform_get_drvdata(pdev);
	struct bcm2835_spi *bs = spi_controller_get_devdata(ctlr);

	bcm2835_debugfs_remove(bs);

	spi_unregister_controller(ctlr);

	bcm2835_dma_release(ctlr, bs);

	/* Clear FIFOs, and disable the HW block */
	bcm2835_wr(bs, BCM2835_SPI_CS,
		   BCM2835_SPI_CS_CLEAR_RX | BCM2835_SPI_CS_CLEAR_TX);

	clk_disable_unprepare(bs->clk);

	return 0;
}

static void bcm2835_spi_shutdown(struct platform_device *pdev)
{
	int ret;

	ret = bcm2835_spi_remove(pdev);
	if (ret)
		dev_err(&pdev->dev, "failed to shutdown\n");
}

static const struct of_device_id bcm2835_spi_match[] = {
	{ .compatible = "brcm,bcm2835-spi", },
	{}
};
MODULE_DEVICE_TABLE(of, bcm2835_spi_match);

static struct platform_driver bcm2835_spi_driver = {
	.driver		= {
		.name		= DRV_NAME,
		.of_match_table	= bcm2835_spi_match,
	},
	.probe		= bcm2835_spi_probe,
	.remove		= bcm2835_spi_remove,
	.shutdown	= bcm2835_spi_shutdown,
};
module_platform_driver(bcm2835_spi_driver);

MODULE_DESCRIPTION("SPI controller driver for Broadcom BCM2835");
MODULE_AUTHOR("Chris Boot <bootc@bootc.net>");
MODULE_LICENSE("GPL");
