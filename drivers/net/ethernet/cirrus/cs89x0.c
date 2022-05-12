/* cs89x0.c: A Crystal Semiconductor (Now Cirrus Logic) CS89[02]0
 *           driver for linux.
 * Written 1996 by Russell Nelson, with reference to skeleton.c
 * written 1993-1994 by Donald Becker.
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 *
 * The author may be reached at nelson@crynwr.com, Crynwr
 * Software, 521 Pleasant Valley Rd., Potsdam, NY 13676
 *
 * Other contributors:
 * Mike Cruse        : mcruse@cti-ltd.com
 * Russ Nelson
 * Melody Lee        : ethernet@crystal.cirrus.com
 * Alan Cox
 * Andrew Morton
 * Oskar Schirmer    : oskar@scara.com
 * Deepak Saxena     : dsaxena@plexity.net
 * Dmitry Pervushin  : dpervushin@ru.mvista.com
 * Deepak Saxena     : dsaxena@plexity.net
 * Domenico Andreoli : cavokz@gmail.com
 */


/*
 * Set this to zero to disable DMA code
 *
 * Note that even if DMA is turned off we still support the 'dma' and  'use_dma'
 * module options so we don't break any startup scripts.
 */
#ifndef CONFIG_ISA_DMA_API
#define ALLOW_DMA	0
#else
#define ALLOW_DMA	1
#endif

/*
 * Set this to zero to remove all the debug statements via
 * dead code elimination
 */
#define DEBUGGING	1

/* Sources:
 *	Crynwr packet driver epktisa.
 *	Crystal Semiconductor data sheets.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/printk.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/in.h>
#include <linux/jiffies.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/gfp.h>
#include <linux/io.h>

#include <asm/irq.h>
#include <linux/atomic.h>
#if ALLOW_DMA
#include <asm/dma.h>
#endif

#include "cs89x0.h"

#define cs89_dbg(val, level, fmt, ...)				\
do {								\
	if (val <= net_debug)					\
		pr_##level(fmt, ##__VA_ARGS__);			\
} while (0)

static char version[] __initdata =
	"v2.4.3-pre1 Russell Nelson <nelson@crynwr.com>, Andrew Morton";

#define DRV_NAME "cs89x0"

/* First, a few definitions that the brave might change.
 * A zero-terminated list of I/O addresses to be probed. Some special flags..
 * Addr & 1 = Read back the address port, look for signature and reset
 * the page window before probing
 * Addr & 3 = Reset the page window and probe
 * The CLPS eval board has the Cirrus chip at 0x80090300, in ARM IO space,
 * but it is possible that a Cirrus board could be plugged into the ISA
 * slots.
 */
/* The cs8900 has 4 IRQ pins, software selectable. cs8900_irq_map maps
 * them to system IRQ numbers. This mapping is card specific and is set to
 * the configuration of the Cirrus Eval board for this chip.
 */
#ifndef CONFIG_CS89x0_PLATFORM
static unsigned int netcard_portlist[] __used __initdata = {
	0x300, 0x320, 0x340, 0x360, 0x200, 0x220, 0x240,
	0x260, 0x280, 0x2a0, 0x2c0, 0x2e0, 0
};
static unsigned int cs8900_irq_map[] = {
	10, 11, 12, 5
};
#endif

#if DEBUGGING
static unsigned int net_debug = DEBUGGING;
#else
#define net_debug 0	/* gcc will remove all the debug code for us */
#endif

/* The number of low I/O ports used by the ethercard. */
#define NETCARD_IO_EXTENT	16

/* we allow the user to override various values normally set in the EEPROM */
#define FORCE_RJ45	0x0001    /* pick one of these three */
#define FORCE_AUI	0x0002
#define FORCE_BNC	0x0004

#define FORCE_AUTO	0x0010    /* pick one of these three */
#define FORCE_HALF	0x0020
#define FORCE_FULL	0x0030

/* Information that need to be kept for each board. */
struct net_local {
	int chip_type;		/* one of: CS8900, CS8920, CS8920M */
	char chip_revision;	/* revision letter of the chip ('A'...) */
	int send_cmd;		/* the proper send command: TX_NOW, TX_AFTER_381, or TX_AFTER_ALL */
	int auto_neg_cnf;	/* auto-negotiation word from EEPROM */
	int adapter_cnf;	/* adapter configuration from EEPROM */
	int isa_config;		/* ISA configuration from EEPROM */
	int irq_map;		/* IRQ map from EEPROM */
	int rx_mode;		/* what mode are we in? 0, RX_MULTCAST_ACCEPT, or RX_ALL_ACCEPT */
	int curr_rx_cfg;	/* a copy of PP_RxCFG */
	int linectl;		/* either 0 or LOW_RX_SQUELCH, depending on configuration. */
	int send_underrun;	/* keep track of how many underruns in a row we get */
	int force;		/* force various values; see FORCE* above. */
	spinlock_t lock;
	void __iomem *virt_addr;/* CS89x0 virtual address. */
#if ALLOW_DMA
	int use_dma;		/* Flag: we're using dma */
	int dma;		/* DMA channel */
	int dmasize;		/* 16 or 64 */
	unsigned char *dma_buff;	/* points to the beginning of the buffer */
	unsigned char *end_dma_buff;	/* points to the end of the buffer */
	unsigned char *rx_dma_ptr;	/* points to the next packet  */
#endif
};

/* Example routines you must write ;->. */
#define tx_done(dev) 1

/*
 * Permit 'cs89x0_dma=N' in the kernel boot environment
 */
#if !defined(MODULE)
#if ALLOW_DMA
static int g_cs89x0_dma;

static int __init dma_fn(char *str)
{
	g_cs89x0_dma = simple_strtol(str, NULL, 0);
	return 1;
}

__setup("cs89x0_dma=", dma_fn);
#endif	/* ALLOW_DMA */

static int g_cs89x0_media__force;

static int __init media_fn(char *str)
{
	if (!strcmp(str, "rj45"))
		g_cs89x0_media__force = FORCE_RJ45;
	else if (!strcmp(str, "aui"))
		g_cs89x0_media__force = FORCE_AUI;
	else if (!strcmp(str, "bnc"))
		g_cs89x0_media__force = FORCE_BNC;

	return 1;
}

__setup("cs89x0_media=", media_fn);
#endif

static void readwords(struct net_local *lp, int portno, void *buf, int length)
{
	u8 *buf8 = (u8 *)buf;

	do {
		u16 tmp16;

		tmp16 = ioread16(lp->virt_addr + portno);
		*buf8++ = (u8)tmp16;
		*buf8++ = (u8)(tmp16 >> 8);
	} while (--length);
}

static void writewords(struct net_local *lp, int portno, void *buf, int length)
{
	u8 *buf8 = (u8 *)buf;

	do {
		u16 tmp16;

		tmp16 = *buf8++;
		tmp16 |= (*buf8++) << 8;
		iowrite16(tmp16, lp->virt_addr + portno);
	} while (--length);
}

static u16
readreg(struct net_device *dev, u16 regno)
{
	struct net_local *lp = netdev_priv(dev);

	iowrite16(regno, lp->virt_addr + ADD_PORT);
	return ioread16(lp->virt_addr + DATA_PORT);
}

static void
writereg(struct net_device *dev, u16 regno, u16 value)
{
	struct net_local *lp = netdev_priv(dev);

	iowrite16(regno, lp->virt_addr + ADD_PORT);
	iowrite16(value, lp->virt_addr + DATA_PORT);
}

static int __init
wait_eeprom_ready(struct net_device *dev)
{
	unsigned long timeout = jiffies;
	/* check to see if the EEPROM is ready,
	 * a timeout is used just in case EEPROM is ready when
	 * SI_BUSY in the PP_SelfST is clear
	 */
	while (readreg(dev, PP_SelfST) & SI_BUSY)
		if (time_after_eq(jiffies, timeout + 40))
			return -1;
	return 0;
}

static int __init
get_eeprom_data(struct net_device *dev, int off, int len, int *buffer)
{
	int i;

	cs89_dbg(3, info, "EEPROM data from %x for %x:", off, len);
	for (i = 0; i < len; i++) {
		if (wait_eeprom_ready(dev) < 0)
			return -1;
		/* Now send the EEPROM read command and EEPROM location to read */
		writereg(dev, PP_EECMD, (off + i) | EEPROM_READ_CMD);
		if (wait_eeprom_ready(dev) < 0)
			return -1;
		buffer[i] = readreg(dev, PP_EEData);
		cs89_dbg(3, cont, " %04x", buffer[i]);
	}
	cs89_dbg(3, cont, "\n");
	return 0;
}

static int  __init
get_eeprom_cksum(int off, int len, int *buffer)
{
	int i, cksum;

	cksum = 0;
	for (i = 0; i < len; i++)
		cksum += buffer[i];
	cksum &= 0xffff;
	if (cksum == 0)
		return 0;
	return -1;
}

static void
write_irq(struct net_device *dev, int chip_type, int irq)
{
	int i;

	if (chip_type == CS8900) {
#ifndef CONFIG_CS89x0_PLATFORM
		/* Search the mapping table for the corresponding IRQ pin. */
		for (i = 0; i != ARRAY_SIZE(cs8900_irq_map); i++)
			if (cs8900_irq_map[i] == irq)
				break;
		/* Not found */
		if (i == ARRAY_SIZE(cs8900_irq_map))
			i = 3;
#else
		/* INTRQ0 pin is used for interrupt generation. */
		i = 0;
#endif
		writereg(dev, PP_CS8900_ISAINT, i);
	} else {
		writereg(dev, PP_CS8920_ISAINT, irq);
	}
}

static void
count_rx_errors(int status, struct net_device *dev)
{
	dev->stats.rx_errors++;
	if (status & RX_RUNT)
		dev->stats.rx_length_errors++;
	if (status & RX_EXTRA_DATA)
		dev->stats.rx_length_errors++;
	if ((status & RX_CRC_ERROR) && !(status & (RX_EXTRA_DATA | RX_RUNT)))
		/* per str 172 */
		dev->stats.rx_crc_errors++;
	if (status & RX_DRIBBLE)
		dev->stats.rx_frame_errors++;
}

/*********************************
 * This page contains DMA routines
 *********************************/

#if ALLOW_DMA

#define dma_page_eq(ptr1, ptr2) ((long)(ptr1) >> 17 == (long)(ptr2) >> 17)

static void
get_dma_channel(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);

	if (lp->dma) {
		dev->dma = lp->dma;
		lp->isa_config |= ISA_RxDMA;
	} else {
		if ((lp->isa_config & ANY_ISA_DMA) == 0)
			return;
		dev->dma = lp->isa_config & DMA_NO_MASK;
		if (lp->chip_type == CS8900)
			dev->dma += 5;
		if (dev->dma < 5 || dev->dma > 7) {
			lp->isa_config &= ~ANY_ISA_DMA;
			return;
		}
	}
}

static void
write_dma(struct net_device *dev, int chip_type, int dma)
{
	struct net_local *lp = netdev_priv(dev);
	if ((lp->isa_config & ANY_ISA_DMA) == 0)
		return;
	if (chip_type == CS8900)
		writereg(dev, PP_CS8900_ISADMA, dma - 5);
	else
		writereg(dev, PP_CS8920_ISADMA, dma);
}

static void
set_dma_cfg(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);

	if (lp->use_dma) {
		if ((lp->isa_config & ANY_ISA_DMA) == 0) {
			cs89_dbg(3, err, "set_dma_cfg(): no DMA\n");
			return;
		}
		if (lp->isa_config & ISA_RxDMA) {
			lp->curr_rx_cfg |= RX_DMA_ONLY;
			cs89_dbg(3, info, "set_dma_cfg(): RX_DMA_ONLY\n");
		} else {
			lp->curr_rx_cfg |= AUTO_RX_DMA;	/* not that we support it... */
			cs89_dbg(3, info, "set_dma_cfg(): AUTO_RX_DMA\n");
		}
	}
}

static int
dma_bufcfg(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	if (lp->use_dma)
		return (lp->isa_config & ANY_ISA_DMA) ? RX_DMA_ENBL : 0;
	else
		return 0;
}

static int
dma_busctl(struct net_device *dev)
{
	int retval = 0;
	struct net_local *lp = netdev_priv(dev);
	if (lp->use_dma) {
		if (lp->isa_config & ANY_ISA_DMA)
			retval |= RESET_RX_DMA; /* Reset the DMA pointer */
		if (lp->isa_config & DMA_BURST)
			retval |= DMA_BURST_MODE; /* Does ISA config specify DMA burst ? */
		if (lp->dmasize == 64)
			retval |= RX_DMA_SIZE_64K; /* did they ask for 64K? */
		retval |= MEMORY_ON;	/* we need memory enabled to use DMA. */
	}
	return retval;
}

static void
dma_rx(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	struct sk_buff *skb;
	int status, length;
	unsigned char *bp = lp->rx_dma_ptr;

	status = bp[0] + (bp[1] << 8);
	length = bp[2] + (bp[3] << 8);
	bp += 4;

	cs89_dbg(5, debug, "%s: receiving DMA packet at %lx, status %x, length %x\n",
		 dev->name, (unsigned long)bp, status, length);

	if ((status & RX_OK) == 0) {
		count_rx_errors(status, dev);
		goto skip_this_frame;
	}

	/* Malloc up new buffer. */
	skb = netdev_alloc_skb(dev, length + 2);
	if (skb == NULL) {
		dev->stats.rx_dropped++;

		/* AKPM: advance bp to the next frame */
skip_this_frame:
		bp += (length + 3) & ~3;
		if (bp >= lp->end_dma_buff)
			bp -= lp->dmasize * 1024;
		lp->rx_dma_ptr = bp;
		return;
	}
	skb_reserve(skb, 2);	/* longword align L3 header */

	if (bp + length > lp->end_dma_buff) {
		int semi_cnt = lp->end_dma_buff - bp;
		skb_put_data(skb, bp, semi_cnt);
		skb_put_data(skb, lp->dma_buff, length - semi_cnt);
	} else {
		skb_put_data(skb, bp, length);
	}
	bp += (length + 3) & ~3;
	if (bp >= lp->end_dma_buff)
		bp -= lp->dmasize*1024;
	lp->rx_dma_ptr = bp;

	cs89_dbg(3, info, "%s: received %d byte DMA packet of type %x\n",
		 dev->name, length,
		 ((skb->data[ETH_ALEN + ETH_ALEN] << 8) |
		  skb->data[ETH_ALEN + ETH_ALEN + 1]));

	skb->protocol = eth_type_trans(skb, dev);
	netif_rx(skb);
	dev->stats.rx_packets++;
	dev->stats.rx_bytes += length;
}

static void release_dma_buff(struct net_local *lp)
{
	if (lp->dma_buff) {
		free_pages((unsigned long)(lp->dma_buff),
			   get_order(lp->dmasize * 1024));
		lp->dma_buff = NULL;
	}
}

#endif	/* ALLOW_DMA */

static void
control_dc_dc(struct net_device *dev, int on_not_off)
{
	struct net_local *lp = netdev_priv(dev);
	unsigned int selfcontrol;
	unsigned long timenow = jiffies;
	/* control the DC to DC convertor in the SelfControl register.
	 * Note: This is hooked up to a general purpose pin, might not
	 * always be a DC to DC convertor.
	 */

	selfcontrol = HCB1_ENBL; /* Enable the HCB1 bit as an output */
	if (((lp->adapter_cnf & A_CNF_DC_DC_POLARITY) != 0) ^ on_not_off)
		selfcontrol |= HCB1;
	else
		selfcontrol &= ~HCB1;
	writereg(dev, PP_SelfCTL, selfcontrol);

	/* Wait for the DC/DC converter to power up - 500ms */
	while (time_before(jiffies, timenow + HZ))
		;
}

/* send a test packet - return true if carrier bits are ok */
static int
send_test_pkt(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	char test_packet[] = {
		0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
		0, 46,		/* A 46 in network order */
		0, 0,		/* DSAP=0 & SSAP=0 fields */
		0xf3, 0		/* Control (Test Req + P bit set) */
	};
	unsigned long timenow = jiffies;

	writereg(dev, PP_LineCTL, readreg(dev, PP_LineCTL) | SERIAL_TX_ON);

	memcpy(test_packet,            dev->dev_addr, ETH_ALEN);
	memcpy(test_packet + ETH_ALEN, dev->dev_addr, ETH_ALEN);

	iowrite16(TX_AFTER_ALL, lp->virt_addr + TX_CMD_PORT);
	iowrite16(ETH_ZLEN, lp->virt_addr + TX_LEN_PORT);

	/* Test to see if the chip has allocated memory for the packet */
	while (time_before(jiffies, timenow + 5))
		if (readreg(dev, PP_BusST) & READY_FOR_TX_NOW)
			break;
	if (time_after_eq(jiffies, timenow + 5))
		return 0;	/* this shouldn't happen */

	/* Write the contents of the packet */
	writewords(lp, TX_FRAME_PORT, test_packet, (ETH_ZLEN + 1) >> 1);

	cs89_dbg(1, debug, "Sending test packet ");
	/* wait a couple of jiffies for packet to be received */
	for (timenow = jiffies; time_before(jiffies, timenow + 3);)
		;
	if ((readreg(dev, PP_TxEvent) & TX_SEND_OK_BITS) == TX_OK) {
		cs89_dbg(1, cont, "succeeded\n");
		return 1;
	}
	cs89_dbg(1, cont, "failed\n");
	return 0;
}

#define DETECTED_NONE  0
#define DETECTED_RJ45H 1
#define DETECTED_RJ45F 2
#define DETECTED_AUI   3
#define DETECTED_BNC   4

static int
detect_tp(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	unsigned long timenow = jiffies;
	int fdx;

	cs89_dbg(1, debug, "%s: Attempting TP\n", dev->name);

	/* If connected to another full duplex capable 10-Base-T card
	 * the link pulses seem to be lost when the auto detect bit in
	 * the LineCTL is set.  To overcome this the auto detect bit will
	 * be cleared whilst testing the 10-Base-T interface.  This would
	 * not be necessary for the sparrow chip but is simpler to do it
	 * anyway.
	 */
	writereg(dev, PP_LineCTL, lp->linectl & ~AUI_ONLY);
	control_dc_dc(dev, 0);

	/* Delay for the hardware to work out if the TP cable is present
	 * - 150ms
	 */
	for (timenow = jiffies; time_before(jiffies, timenow + 15);)
		;
	if ((readreg(dev, PP_LineST) & LINK_OK) == 0)
		return DETECTED_NONE;

	if (lp->chip_type == CS8900) {
		switch (lp->force & 0xf0) {
#if 0
		case FORCE_AUTO:
			pr_info("%s: cs8900 doesn't autonegotiate\n",
				dev->name);
			return DETECTED_NONE;
#endif
			/* CS8900 doesn't support AUTO, change to HALF*/
		case FORCE_AUTO:
			lp->force &= ~FORCE_AUTO;
			lp->force |= FORCE_HALF;
			break;
		case FORCE_HALF:
			break;
		case FORCE_FULL:
			writereg(dev, PP_TestCTL,
				 readreg(dev, PP_TestCTL) | FDX_8900);
			break;
		}
		fdx = readreg(dev, PP_TestCTL) & FDX_8900;
	} else {
		switch (lp->force & 0xf0) {
		case FORCE_AUTO:
			lp->auto_neg_cnf = AUTO_NEG_ENABLE;
			break;
		case FORCE_HALF:
			lp->auto_neg_cnf = 0;
			break;
		case FORCE_FULL:
			lp->auto_neg_cnf = RE_NEG_NOW | ALLOW_FDX;
			break;
		}

		writereg(dev, PP_AutoNegCTL, lp->auto_neg_cnf & AUTO_NEG_MASK);

		if ((lp->auto_neg_cnf & AUTO_NEG_BITS) == AUTO_NEG_ENABLE) {
			pr_info("%s: negotiating duplex...\n", dev->name);
			while (readreg(dev, PP_AutoNegST) & AUTO_NEG_BUSY) {
				if (time_after(jiffies, timenow + 4000)) {
					pr_err("**** Full / half duplex auto-negotiation timed out ****\n");
					break;
				}
			}
		}
		fdx = readreg(dev, PP_AutoNegST) & FDX_ACTIVE;
	}
	if (fdx)
		return DETECTED_RJ45F;
	else
		return DETECTED_RJ45H;
}

static int
detect_bnc(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);

	cs89_dbg(1, debug, "%s: Attempting BNC\n", dev->name);
	control_dc_dc(dev, 1);

	writereg(dev, PP_LineCTL, (lp->linectl & ~AUTO_AUI_10BASET) | AUI_ONLY);

	if (send_test_pkt(dev))
		return DETECTED_BNC;
	else
		return DETECTED_NONE;
}

static int
detect_aui(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);

	cs89_dbg(1, debug, "%s: Attempting AUI\n", dev->name);
	control_dc_dc(dev, 0);

	writereg(dev, PP_LineCTL, (lp->linectl & ~AUTO_AUI_10BASET) | AUI_ONLY);

	if (send_test_pkt(dev))
		return DETECTED_AUI;
	else
		return DETECTED_NONE;
}

/* We have a good packet(s), get it/them out of the buffers. */
static void
net_rx(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	struct sk_buff *skb;
	int status, length;

	status = ioread16(lp->virt_addr + RX_FRAME_PORT);
	length = ioread16(lp->virt_addr + RX_FRAME_PORT);

	if ((status & RX_OK) == 0) {
		count_rx_errors(status, dev);
		return;
	}

	/* Malloc up new buffer. */
	skb = netdev_alloc_skb(dev, length + 2);
	if (skb == NULL) {
		dev->stats.rx_dropped++;
		return;
	}
	skb_reserve(skb, 2);	/* longword align L3 header */

	readwords(lp, RX_FRAME_PORT, skb_put(skb, length), length >> 1);
	if (length & 1)
		skb->data[length-1] = ioread16(lp->virt_addr + RX_FRAME_PORT);

	cs89_dbg(3, debug, "%s: received %d byte packet of type %x\n",
		 dev->name, length,
		 (skb->data[ETH_ALEN + ETH_ALEN] << 8) |
		 skb->data[ETH_ALEN + ETH_ALEN + 1]);

	skb->protocol = eth_type_trans(skb, dev);
	netif_rx(skb);
	dev->stats.rx_packets++;
	dev->stats.rx_bytes += length;
}

/* The typical workload of the driver:
 * Handle the network interface interrupts.
 */

static irqreturn_t net_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct net_local *lp;
	int status;
	int handled = 0;

	lp = netdev_priv(dev);

	/* we MUST read all the events out of the ISQ, otherwise we'll never
	 * get interrupted again.  As a consequence, we can't have any limit
	 * on the number of times we loop in the interrupt handler.  The
	 * hardware guarantees that eventually we'll run out of events.  Of
	 * course, if you're on a slow machine, and packets are arriving
	 * faster than you can read them off, you're screwed.  Hasta la
	 * vista, baby!
	 */
	while ((status = ioread16(lp->virt_addr + ISQ_PORT))) {
		cs89_dbg(4, debug, "%s: event=%04x\n", dev->name, status);
		handled = 1;
		switch (status & ISQ_EVENT_MASK) {
		case ISQ_RECEIVER_EVENT:
			/* Got a packet(s). */
			net_rx(dev);
			break;
		case ISQ_TRANSMITTER_EVENT:
			dev->stats.tx_packets++;
			netif_wake_queue(dev);	/* Inform upper layers. */
			if ((status & (TX_OK |
				       TX_LOST_CRS |
				       TX_SQE_ERROR |
				       TX_LATE_COL |
				       TX_16_COL)) != TX_OK) {
				if ((status & TX_OK) == 0)
					dev->stats.tx_errors++;
				if (status & TX_LOST_CRS)
					dev->stats.tx_carrier_errors++;
				if (status & TX_SQE_ERROR)
					dev->stats.tx_heartbeat_errors++;
				if (status & TX_LATE_COL)
					dev->stats.tx_window_errors++;
				if (status & TX_16_COL)
					dev->stats.tx_aborted_errors++;
			}
			break;
		case ISQ_BUFFER_EVENT:
			if (status & READY_FOR_TX) {
				/* we tried to transmit a packet earlier,
				 * but inexplicably ran out of buffers.
				 * That shouldn't happen since we only ever
				 * load one packet.  Shrug.  Do the right
				 * thing anyway.
				 */
				netif_wake_queue(dev);	/* Inform upper layers. */
			}
			if (status & TX_UNDERRUN) {
				cs89_dbg(0, err, "%s: transmit underrun\n",
					 dev->name);
				lp->send_underrun++;
				if (lp->send_underrun == 3)
					lp->send_cmd = TX_AFTER_381;
				else if (lp->send_underrun == 6)
					lp->send_cmd = TX_AFTER_ALL;
				/* transmit cycle is done, although
				 * frame wasn't transmitted - this
				 * avoids having to wait for the upper
				 * layers to timeout on us, in the
				 * event of a tx underrun
				 */
				netif_wake_queue(dev);	/* Inform upper layers. */
			}
#if ALLOW_DMA
			if (lp->use_dma && (status & RX_DMA)) {
				int count = readreg(dev, PP_DmaFrameCnt);
				while (count) {
					cs89_dbg(5, debug,
						 "%s: receiving %d DMA frames\n",
						 dev->name, count);
					if (count > 1)
						cs89_dbg(2, debug,
							 "%s: receiving %d DMA frames\n",
							 dev->name, count);
					dma_rx(dev);
					if (--count == 0)
						count = readreg(dev, PP_DmaFrameCnt);
					if (count > 0)
						cs89_dbg(2, debug,
							 "%s: continuing with %d DMA frames\n",
							 dev->name, count);
				}
			}
#endif
			break;
		case ISQ_RX_MISS_EVENT:
			dev->stats.rx_missed_errors += (status >> 6);
			break;
		case ISQ_TX_COL_EVENT:
			dev->stats.collisions += (status >> 6);
			break;
		}
	}
	return IRQ_RETVAL(handled);
}

/* Open/initialize the board.  This is called (in the current kernel)
   sometime after booting when the 'ifconfig' program is run.

   This routine should set everything up anew at each open, even
   registers that "should" only need to be set once at boot, so that
   there is non-reboot way to recover if something goes wrong.
*/

/* AKPM: do we need to do any locking here? */

static int
net_open(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	int result = 0;
	int i;
	int ret;

	if (dev->irq < 2) {
		/* Allow interrupts to be generated by the chip */
/* Cirrus' release had this: */
#if 0
		writereg(dev, PP_BusCTL, readreg(dev, PP_BusCTL) | ENABLE_IRQ);
#endif
/* And 2.3.47 had this: */
		writereg(dev, PP_BusCTL, ENABLE_IRQ | MEMORY_ON);

		for (i = 2; i < CS8920_NO_INTS; i++) {
			if ((1 << i) & lp->irq_map) {
				if (request_irq(i, net_interrupt, 0, dev->name,
						dev) == 0) {
					dev->irq = i;
					write_irq(dev, lp->chip_type, i);
					/* writereg(dev, PP_BufCFG, GENERATE_SW_INTERRUPT); */
					break;
				}
			}
		}

		if (i >= CS8920_NO_INTS) {
			writereg(dev, PP_BusCTL, 0);	/* disable interrupts. */
			pr_err("can't get an interrupt\n");
			ret = -EAGAIN;
			goto bad_out;
		}
	} else {
#if !defined(CONFIG_CS89x0_PLATFORM)
		if (((1 << dev->irq) & lp->irq_map) == 0) {
			pr_err("%s: IRQ %d is not in our map of allowable IRQs, which is %x\n",
			       dev->name, dev->irq, lp->irq_map);
			ret = -EAGAIN;
			goto bad_out;
		}
#endif
/* FIXME: Cirrus' release had this: */
		writereg(dev, PP_BusCTL, readreg(dev, PP_BusCTL)|ENABLE_IRQ);
/* And 2.3.47 had this: */
#if 0
		writereg(dev, PP_BusCTL, ENABLE_IRQ | MEMORY_ON);
#endif
		write_irq(dev, lp->chip_type, dev->irq);
		ret = request_irq(dev->irq, net_interrupt, 0, dev->name, dev);
		if (ret) {
			pr_err("request_irq(%d) failed\n", dev->irq);
			goto bad_out;
		}
	}

#if ALLOW_DMA
	if (lp->use_dma && (lp->isa_config & ANY_ISA_DMA)) {
		unsigned long flags;
		lp->dma_buff = (unsigned char *)__get_dma_pages(GFP_KERNEL,
								get_order(lp->dmasize * 1024));
		if (!lp->dma_buff) {
			pr_err("%s: cannot get %dK memory for DMA\n",
			       dev->name, lp->dmasize);
			goto release_irq;
		}
		cs89_dbg(1, debug, "%s: dma %lx %lx\n",
			 dev->name,
			 (unsigned long)lp->dma_buff,
			 (unsigned long)isa_virt_to_bus(lp->dma_buff));
		if ((unsigned long)lp->dma_buff >= MAX_DMA_ADDRESS ||
		    !dma_page_eq(lp->dma_buff,
				 lp->dma_buff + lp->dmasize * 1024 - 1)) {
			pr_err("%s: not usable as DMA buffer\n", dev->name);
			goto release_irq;
		}
		memset(lp->dma_buff, 0, lp->dmasize * 1024);	/* Why? */
		if (request_dma(dev->dma, dev->name)) {
			pr_err("%s: cannot get dma channel %d\n",
			       dev->name, dev->dma);
			goto release_irq;
		}
		write_dma(dev, lp->chip_type, dev->dma);
		lp->rx_dma_ptr = lp->dma_buff;
		lp->end_dma_buff = lp->dma_buff + lp->dmasize * 1024;
		spin_lock_irqsave(&lp->lock, flags);
		disable_dma(dev->dma);
		clear_dma_ff(dev->dma);
		set_dma_mode(dev->dma, DMA_RX_MODE); /* auto_init as well */
		set_dma_addr(dev->dma, isa_virt_to_bus(lp->dma_buff));
		set_dma_count(dev->dma, lp->dmasize * 1024);
		enable_dma(dev->dma);
		spin_unlock_irqrestore(&lp->lock, flags);
	}
#endif	/* ALLOW_DMA */

	/* set the Ethernet address */
	for (i = 0; i < ETH_ALEN / 2; i++)
		writereg(dev, PP_IA + i * 2,
			 (dev->dev_addr[i * 2] |
			  (dev->dev_addr[i * 2 + 1] << 8)));

	/* while we're testing the interface, leave interrupts disabled */
	writereg(dev, PP_BusCTL, MEMORY_ON);

	/* Set the LineCTL quintuplet based on adapter configuration read from EEPROM */
	if ((lp->adapter_cnf & A_CNF_EXTND_10B_2) &&
	    (lp->adapter_cnf & A_CNF_LOW_RX_SQUELCH))
		lp->linectl = LOW_RX_SQUELCH;
	else
		lp->linectl = 0;

	/* check to make sure that they have the "right" hardware available */
	switch (lp->adapter_cnf & A_CNF_MEDIA_TYPE) {
	case A_CNF_MEDIA_10B_T:
		result = lp->adapter_cnf & A_CNF_10B_T;
		break;
	case A_CNF_MEDIA_AUI:
		result = lp->adapter_cnf & A_CNF_AUI;
		break;
	case A_CNF_MEDIA_10B_2:
		result = lp->adapter_cnf & A_CNF_10B_2;
		break;
	default:
		result = lp->adapter_cnf & (A_CNF_10B_T |
					    A_CNF_AUI |
					    A_CNF_10B_2);
	}
	if (!result) {
		pr_err("%s: EEPROM is configured for unavailable media\n",
		       dev->name);
release_dma:
#if ALLOW_DMA
		free_dma(dev->dma);
release_irq:
		release_dma_buff(lp);
#endif
		writereg(dev, PP_LineCTL,
			 readreg(dev, PP_LineCTL) & ~(SERIAL_TX_ON | SERIAL_RX_ON));
		free_irq(dev->irq, dev);
		ret = -EAGAIN;
		goto bad_out;
	}

	/* set the hardware to the configured choice */
	switch (lp->adapter_cnf & A_CNF_MEDIA_TYPE) {
	case A_CNF_MEDIA_10B_T:
		result = detect_tp(dev);
		if (result == DETECTED_NONE) {
			pr_warn("%s: 10Base-T (RJ-45) has no cable\n",
				dev->name);
			if (lp->auto_neg_cnf & IMM_BIT) /* check "ignore missing media" bit */
				result = DETECTED_RJ45H; /* Yes! I don't care if I see a link pulse */
		}
		break;
	case A_CNF_MEDIA_AUI:
		result = detect_aui(dev);
		if (result == DETECTED_NONE) {
			pr_warn("%s: 10Base-5 (AUI) has no cable\n", dev->name);
			if (lp->auto_neg_cnf & IMM_BIT) /* check "ignore missing media" bit */
				result = DETECTED_AUI; /* Yes! I don't care if I see a carrrier */
		}
		break;
	case A_CNF_MEDIA_10B_2:
		result = detect_bnc(dev);
		if (result == DETECTED_NONE) {
			pr_warn("%s: 10Base-2 (BNC) has no cable\n", dev->name);
			if (lp->auto_neg_cnf & IMM_BIT) /* check "ignore missing media" bit */
				result = DETECTED_BNC; /* Yes! I don't care if I can xmit a packet */
		}
		break;
	case A_CNF_MEDIA_AUTO:
		writereg(dev, PP_LineCTL, lp->linectl | AUTO_AUI_10BASET);
		if (lp->adapter_cnf & A_CNF_10B_T) {
			result = detect_tp(dev);
			if (result != DETECTED_NONE)
				break;
		}
		if (lp->adapter_cnf & A_CNF_AUI) {
			result = detect_aui(dev);
			if (result != DETECTED_NONE)
				break;
		}
		if (lp->adapter_cnf & A_CNF_10B_2) {
			result = detect_bnc(dev);
			if (result != DETECTED_NONE)
				break;
		}
		pr_err("%s: no media detected\n", dev->name);
		goto release_dma;
	}
	switch (result) {
	case DETECTED_NONE:
		pr_err("%s: no network cable attached to configured media\n",
		       dev->name);
		goto release_dma;
	case DETECTED_RJ45H:
		pr_info("%s: using half-duplex 10Base-T (RJ-45)\n", dev->name);
		break;
	case DETECTED_RJ45F:
		pr_info("%s: using full-duplex 10Base-T (RJ-45)\n", dev->name);
		break;
	case DETECTED_AUI:
		pr_info("%s: using 10Base-5 (AUI)\n", dev->name);
		break;
	case DETECTED_BNC:
		pr_info("%s: using 10Base-2 (BNC)\n", dev->name);
		break;
	}

	/* Turn on both receive and transmit operations */
	writereg(dev, PP_LineCTL,
		 readreg(dev, PP_LineCTL) | SERIAL_RX_ON | SERIAL_TX_ON);

	/* Receive only error free packets addressed to this card */
	lp->rx_mode = 0;
	writereg(dev, PP_RxCTL, DEF_RX_ACCEPT);

	lp->curr_rx_cfg = RX_OK_ENBL | RX_CRC_ERROR_ENBL;

	if (lp->isa_config & STREAM_TRANSFER)
		lp->curr_rx_cfg |= RX_STREAM_ENBL;
#if ALLOW_DMA
	set_dma_cfg(dev);
#endif
	writereg(dev, PP_RxCFG, lp->curr_rx_cfg);

	writereg(dev, PP_TxCFG, (TX_LOST_CRS_ENBL |
				 TX_SQE_ERROR_ENBL |
				 TX_OK_ENBL |
				 TX_LATE_COL_ENBL |
				 TX_JBR_ENBL |
				 TX_ANY_COL_ENBL |
				 TX_16_COL_ENBL));

	writereg(dev, PP_BufCFG, (READY_FOR_TX_ENBL |
				  RX_MISS_COUNT_OVRFLOW_ENBL |
#if ALLOW_DMA
				  dma_bufcfg(dev) |
#endif
				  TX_COL_COUNT_OVRFLOW_ENBL |
				  TX_UNDERRUN_ENBL));

	/* now that we've got our act together, enable everything */
	writereg(dev, PP_BusCTL, (ENABLE_IRQ
				  | (dev->mem_start ? MEMORY_ON : 0) /* turn memory on */
#if ALLOW_DMA
				  | dma_busctl(dev)
#endif
			 ));
	netif_start_queue(dev);
	cs89_dbg(1, debug, "net_open() succeeded\n");
	return 0;
bad_out:
	return ret;
}

/* The inverse routine to net_open(). */
static int
net_close(struct net_device *dev)
{
#if ALLOW_DMA
	struct net_local *lp = netdev_priv(dev);
#endif

	netif_stop_queue(dev);

	writereg(dev, PP_RxCFG, 0);
	writereg(dev, PP_TxCFG, 0);
	writereg(dev, PP_BufCFG, 0);
	writereg(dev, PP_BusCTL, 0);

	free_irq(dev->irq, dev);

#if ALLOW_DMA
	if (lp->use_dma && lp->dma) {
		free_dma(dev->dma);
		release_dma_buff(lp);
	}
#endif

	/* Update the statistics here. */
	return 0;
}

/* Get the current statistics.
 * This may be called with the card open or closed.
 */
static struct net_device_stats *
net_get_stats(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&lp->lock, flags);
	/* Update the statistics from the device registers. */
	dev->stats.rx_missed_errors += (readreg(dev, PP_RxMiss) >> 6);
	dev->stats.collisions += (readreg(dev, PP_TxCol) >> 6);
	spin_unlock_irqrestore(&lp->lock, flags);

	return &dev->stats;
}

static void net_timeout(struct net_device *dev, unsigned int txqueue)
{
	/* If we get here, some higher level has decided we are broken.
	   There should really be a "kick me" function call instead. */
	cs89_dbg(0, err, "%s: transmit timed out, %s?\n",
		 dev->name,
		 tx_done(dev) ? "IRQ conflict" : "network cable problem");
	/* Try to restart the adaptor. */
	netif_wake_queue(dev);
}

static netdev_tx_t net_send_packet(struct sk_buff *skb, struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	unsigned long flags;

	cs89_dbg(3, debug, "%s: sent %d byte packet of type %x\n",
		 dev->name, skb->len,
		 ((skb->data[ETH_ALEN + ETH_ALEN] << 8) |
		  skb->data[ETH_ALEN + ETH_ALEN + 1]));

	/* keep the upload from being interrupted, since we
	 * ask the chip to start transmitting before the
	 * whole packet has been completely uploaded.
	 */

	spin_lock_irqsave(&lp->lock, flags);
	netif_stop_queue(dev);

	/* initiate a transmit sequence */
	iowrite16(lp->send_cmd, lp->virt_addr + TX_CMD_PORT);
	iowrite16(skb->len, lp->virt_addr + TX_LEN_PORT);

	/* Test to see if the chip has allocated memory for the packet */
	if ((readreg(dev, PP_BusST) & READY_FOR_TX_NOW) == 0) {
		/* Gasp!  It hasn't.  But that shouldn't happen since
		 * we're waiting for TxOk, so return 1 and requeue this packet.
		 */

		spin_unlock_irqrestore(&lp->lock, flags);
		cs89_dbg(0, err, "Tx buffer not free!\n");
		return NETDEV_TX_BUSY;
	}
	/* Write the contents of the packet */
	writewords(lp, TX_FRAME_PORT, skb->data, (skb->len + 1) >> 1);
	spin_unlock_irqrestore(&lp->lock, flags);
	dev->stats.tx_bytes += skb->len;
	dev_consume_skb_any(skb);

	/* We DO NOT call netif_wake_queue() here.
	 * We also DO NOT call netif_start_queue().
	 *
	 * Either of these would cause another bottom half run through
	 * net_send_packet() before this packet has fully gone out.
	 * That causes us to hit the "Gasp!" above and the send is rescheduled.
	 * it runs like a dog.  We just return and wait for the Tx completion
	 * interrupt handler to restart the netdevice layer
	 */

	return NETDEV_TX_OK;
}

static void set_multicast_list(struct net_device *dev)
{
	struct net_local *lp = netdev_priv(dev);
	unsigned long flags;
	u16 cfg;

	spin_lock_irqsave(&lp->lock, flags);
	if (dev->flags & IFF_PROMISC)
		lp->rx_mode = RX_ALL_ACCEPT;
	else if ((dev->flags & IFF_ALLMULTI) || !netdev_mc_empty(dev))
		/* The multicast-accept list is initialized to accept-all,
		 * and we rely on higher-level filtering for now.
		 */
		lp->rx_mode = RX_MULTCAST_ACCEPT;
	else
		lp->rx_mode = 0;

	writereg(dev, PP_RxCTL, DEF_RX_ACCEPT | lp->rx_mode);

	/* in promiscuous mode, we accept errored packets,
	 * so we have to enable interrupts on them also
	 */
	cfg = lp->curr_rx_cfg;
	if (lp->rx_mode == RX_ALL_ACCEPT)
		cfg |= RX_CRC_ERROR_ENBL | RX_RUNT_ENBL | RX_EXTRA_DATA_ENBL;
	writereg(dev, PP_RxCFG, cfg);
	spin_unlock_irqrestore(&lp->lock, flags);
}

static int set_mac_address(struct net_device *dev, void *p)
{
	int i;
	struct sockaddr *addr = p;

	if (netif_running(dev))
		return -EBUSY;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	cs89_dbg(0, debug, "%s: Setting MAC address to %pM\n",
		 dev->name, dev->dev_addr);

	/* set the Ethernet address */
	for (i = 0; i < ETH_ALEN / 2; i++)
		writereg(dev, PP_IA + i * 2,
			 (dev->dev_addr[i * 2] |
			  (dev->dev_addr[i * 2 + 1] << 8)));

	return 0;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/*
 * Polling receive - used by netconsole and other diagnostic tools
 * to allow network i/o with interrupts disabled.
 */
static void net_poll_controller(struct net_device *dev)
{
	disable_irq(dev->irq);
	net_interrupt(dev->irq, dev);
	enable_irq(dev->irq);
}
#endif

static const struct net_device_ops net_ops = {
	.ndo_open		= net_open,
	.ndo_stop		= net_close,
	.ndo_tx_timeout		= net_timeout,
	.ndo_start_xmit		= net_send_packet,
	.ndo_get_stats		= net_get_stats,
	.ndo_set_rx_mode	= set_multicast_list,
	.ndo_set_mac_address	= set_mac_address,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= net_poll_controller,
#endif
	.ndo_validate_addr	= eth_validate_addr,
};

static void __init reset_chip(struct net_device *dev)
{
#if !defined(CONFIG_MACH_MX31ADS)
	struct net_local *lp = netdev_priv(dev);
	unsigned long reset_start_time;

	writereg(dev, PP_SelfCTL, readreg(dev, PP_SelfCTL) | POWER_ON_RESET);

	/* wait 30 ms */
	msleep(30);

	if (lp->chip_type != CS8900) {
		/* Hardware problem requires PNP registers to be reconfigured after a reset */
		iowrite16(PP_CS8920_ISAINT, lp->virt_addr + ADD_PORT);
		iowrite8(dev->irq, lp->virt_addr + DATA_PORT);
		iowrite8(0, lp->virt_addr + DATA_PORT + 1);

		iowrite16(PP_CS8920_ISAMemB, lp->virt_addr + ADD_PORT);
		iowrite8((dev->mem_start >> 16) & 0xff,
			 lp->virt_addr + DATA_PORT);
		iowrite8((dev->mem_start >> 8) & 0xff,
			 lp->virt_addr + DATA_PORT + 1);
	}

	/* Wait until the chip is reset */
	reset_start_time = jiffies;
	while ((readreg(dev, PP_SelfST) & INIT_DONE) == 0 &&
	       time_before(jiffies, reset_start_time + 2))
		;
#endif /* !CONFIG_MACH_MX31ADS */
}

/* This is the real probe routine.
 * Linux has a history of friendly device probes on the ISA bus.
 * A good device probes avoids doing writes, and
 * verifies that the correct device exists and functions.
 * Return 0 on success.
 */
static int __init
cs89x0_probe1(struct net_device *dev, void __iomem *ioaddr, int modular)
{
	struct net_local *lp = netdev_priv(dev);
	int i;
	int tmp;
	unsigned rev_type = 0;
	int eeprom_buff[CHKSUM_LEN];
	int retval;

	/* Initialize the device structure. */
	if (!modular) {
		memset(lp, 0, sizeof(*lp));
		spin_lock_init(&lp->lock);
#ifndef MODULE
#if ALLOW_DMA
		if (g_cs89x0_dma) {
			lp->use_dma = 1;
			lp->dma = g_cs89x0_dma;
			lp->dmasize = 16;	/* Could make this an option... */
		}
#endif
		lp->force = g_cs89x0_media__force;
#endif
	}

	pr_debug("PP_addr at %p[%x]: 0x%x\n",
		 ioaddr, ADD_PORT, ioread16(ioaddr + ADD_PORT));
	iowrite16(PP_ChipID, ioaddr + ADD_PORT);

	tmp = ioread16(ioaddr + DATA_PORT);
	if (tmp != CHIP_EISA_ID_SIG) {
		pr_debug("%s: incorrect signature at %p[%x]: 0x%x!="
			 CHIP_EISA_ID_SIG_STR "\n",
			 dev->name, ioaddr, DATA_PORT, tmp);
		retval = -ENODEV;
		goto out1;
	}

	lp->virt_addr = ioaddr;

	/* get the chip type */
	rev_type = readreg(dev, PRODUCT_ID_ADD);
	lp->chip_type = rev_type & ~REVISON_BITS;
	lp->chip_revision = ((rev_type & REVISON_BITS) >> 8) + 'A';

	/* Check the chip type and revision in order to set the correct
	 * send command.  CS8920 revision C and CS8900 revision F can use
	 * the faster send.
	 */
	lp->send_cmd = TX_AFTER_381;
	if (lp->chip_type == CS8900 && lp->chip_revision >= 'F')
		lp->send_cmd = TX_NOW;
	if (lp->chip_type != CS8900 && lp->chip_revision >= 'C')
		lp->send_cmd = TX_NOW;

	pr_info_once("%s\n", version);

	pr_info("%s: cs89%c0%s rev %c found at %p ",
		dev->name,
		lp->chip_type == CS8900  ? '0' : '2',
		lp->chip_type == CS8920M ? "M" : "",
		lp->chip_revision,
		lp->virt_addr);

	reset_chip(dev);

	/* Here we read the current configuration of the chip.
	 * If there is no Extended EEPROM then the idea is to not disturb
	 * the chip configuration, it should have been correctly setup by
	 * automatic EEPROM read on reset. So, if the chip says it read
	 * the EEPROM the driver will always do *something* instead of
	 * complain that adapter_cnf is 0.
	 */

	if ((readreg(dev, PP_SelfST) & (EEPROM_OK | EEPROM_PRESENT)) ==
	    (EEPROM_OK | EEPROM_PRESENT)) {
		/* Load the MAC. */
		for (i = 0; i < ETH_ALEN / 2; i++) {
			unsigned int Addr;
			Addr = readreg(dev, PP_IA + i * 2);
			dev->dev_addr[i * 2] = Addr & 0xFF;
			dev->dev_addr[i * 2 + 1] = Addr >> 8;
		}

		/* Load the Adapter Configuration.
		 * Note:  Barring any more specific information from some
		 * other source (ie EEPROM+Schematics), we would not know
		 * how to operate a 10Base2 interface on the AUI port.
		 * However, since we  do read the status of HCB1 and use
		 * settings that always result in calls to control_dc_dc(dev,0)
		 * a BNC interface should work if the enable pin
		 * (dc/dc converter) is on HCB1.
		 * It will be called AUI however.
		 */

		lp->adapter_cnf = 0;
		i = readreg(dev, PP_LineCTL);
		/* Preserve the setting of the HCB1 pin. */
		if ((i & (HCB1 | HCB1_ENBL)) == (HCB1 | HCB1_ENBL))
			lp->adapter_cnf |= A_CNF_DC_DC_POLARITY;
		/* Save the sqelch bit */
		if ((i & LOW_RX_SQUELCH) == LOW_RX_SQUELCH)
			lp->adapter_cnf |= A_CNF_EXTND_10B_2 | A_CNF_LOW_RX_SQUELCH;
		/* Check if the card is in 10Base-t only mode */
		if ((i & (AUI_ONLY | AUTO_AUI_10BASET)) == 0)
			lp->adapter_cnf |=  A_CNF_10B_T | A_CNF_MEDIA_10B_T;
		/* Check if the card is in AUI only mode */
		if ((i & (AUI_ONLY | AUTO_AUI_10BASET)) == AUI_ONLY)
			lp->adapter_cnf |=  A_CNF_AUI | A_CNF_MEDIA_AUI;
		/* Check if the card is in Auto mode. */
		if ((i & (AUI_ONLY | AUTO_AUI_10BASET)) == AUTO_AUI_10BASET)
			lp->adapter_cnf |=  A_CNF_AUI | A_CNF_10B_T |
				A_CNF_MEDIA_AUI | A_CNF_MEDIA_10B_T | A_CNF_MEDIA_AUTO;

		cs89_dbg(1, info, "%s: PP_LineCTL=0x%x, adapter_cnf=0x%x\n",
			 dev->name, i, lp->adapter_cnf);

		/* IRQ. Other chips already probe, see below. */
		if (lp->chip_type == CS8900)
			lp->isa_config = readreg(dev, PP_CS8900_ISAINT) & INT_NO_MASK;

		pr_cont("[Cirrus EEPROM] ");
	}

	pr_cont("\n");

	/* First check to see if an EEPROM is attached. */

	if ((readreg(dev, PP_SelfST) & EEPROM_PRESENT) == 0)
		pr_warn("No EEPROM, relying on command line....\n");
	else if (get_eeprom_data(dev, START_EEPROM_DATA, CHKSUM_LEN, eeprom_buff) < 0) {
		pr_warn("EEPROM read failed, relying on command line\n");
	} else if (get_eeprom_cksum(START_EEPROM_DATA, CHKSUM_LEN, eeprom_buff) < 0) {
		/* Check if the chip was able to read its own configuration starting
		   at 0 in the EEPROM*/
		if ((readreg(dev, PP_SelfST) & (EEPROM_OK | EEPROM_PRESENT)) !=
		    (EEPROM_OK | EEPROM_PRESENT))
			pr_warn("Extended EEPROM checksum bad and no Cirrus EEPROM, relying on command line\n");

	} else {
		/* This reads an extended EEPROM that is not documented
		 * in the CS8900 datasheet.
		 */

		/* get transmission control word  but keep the autonegotiation bits */
		if (!lp->auto_neg_cnf)
			lp->auto_neg_cnf = eeprom_buff[AUTO_NEG_CNF_OFFSET / 2];
		/* Store adapter configuration */
		if (!lp->adapter_cnf)
			lp->adapter_cnf = eeprom_buff[ADAPTER_CNF_OFFSET / 2];
		/* Store ISA configuration */
		lp->isa_config = eeprom_buff[ISA_CNF_OFFSET / 2];
		dev->mem_start = eeprom_buff[PACKET_PAGE_OFFSET / 2] << 8;

		/* eeprom_buff has 32-bit ints, so we can't just memcpy it */
		/* store the initial memory base address */
		for (i = 0; i < ETH_ALEN / 2; i++) {
			dev->dev_addr[i * 2] = eeprom_buff[i];
			dev->dev_addr[i * 2 + 1] = eeprom_buff[i] >> 8;
		}
		cs89_dbg(1, debug, "%s: new adapter_cnf: 0x%x\n",
			 dev->name, lp->adapter_cnf);
	}

	/* allow them to force multiple transceivers.  If they force multiple, autosense */
	{
		int count = 0;
		if (lp->force & FORCE_RJ45) {
			lp->adapter_cnf |= A_CNF_10B_T;
			count++;
		}
		if (lp->force & FORCE_AUI) {
			lp->adapter_cnf |= A_CNF_AUI;
			count++;
		}
		if (lp->force & FORCE_BNC) {
			lp->adapter_cnf |= A_CNF_10B_2;
			count++;
		}
		if (count > 1)
			lp->adapter_cnf |= A_CNF_MEDIA_AUTO;
		else if (lp->force & FORCE_RJ45)
			lp->adapter_cnf |= A_CNF_MEDIA_10B_T;
		else if (lp->force & FORCE_AUI)
			lp->adapter_cnf |= A_CNF_MEDIA_AUI;
		else if (lp->force & FORCE_BNC)
			lp->adapter_cnf |= A_CNF_MEDIA_10B_2;
	}

	cs89_dbg(1, debug, "%s: after force 0x%x, adapter_cnf=0x%x\n",
		 dev->name, lp->force, lp->adapter_cnf);

	/* FIXME: We don't let you set dc-dc polarity or low RX squelch from the command line: add it here */

	/* FIXME: We don't let you set the IMM bit from the command line: add it to lp->auto_neg_cnf here */

	/* FIXME: we don't set the Ethernet address on the command line.  Use
	 * ifconfig IFACE hw ether AABBCCDDEEFF
	 */

	pr_info("media %s%s%s",
		(lp->adapter_cnf & A_CNF_10B_T) ? "RJ-45," : "",
		(lp->adapter_cnf & A_CNF_AUI) ? "AUI," : "",
		(lp->adapter_cnf & A_CNF_10B_2) ? "BNC," : "");

	lp->irq_map = 0xffff;

	/* If this is a CS8900 then no pnp soft */
	if (lp->chip_type != CS8900 &&
	    /* Check if the ISA IRQ has been set  */
	    (i = readreg(dev, PP_CS8920_ISAINT) & 0xff,
	     (i != 0 && i < CS8920_NO_INTS))) {
		if (!dev->irq)
			dev->irq = i;
	} else {
		i = lp->isa_config & INT_NO_MASK;
#ifndef CONFIG_CS89x0_PLATFORM
		if (lp->chip_type == CS8900) {
			/* Translate the IRQ using the IRQ mapping table. */
			if (i >= ARRAY_SIZE(cs8900_irq_map))
				pr_err("invalid ISA interrupt number %d\n", i);
			else
				i = cs8900_irq_map[i];

			lp->irq_map = CS8900_IRQ_MAP; /* fixed IRQ map for CS8900 */
		} else {
			int irq_map_buff[IRQ_MAP_LEN/2];

			if (get_eeprom_data(dev, IRQ_MAP_EEPROM_DATA,
					    IRQ_MAP_LEN / 2,
					    irq_map_buff) >= 0) {
				if ((irq_map_buff[0] & 0xff) == PNP_IRQ_FRMT)
					lp->irq_map = ((irq_map_buff[0] >> 8) |
						       (irq_map_buff[1] << 8));
			}
		}
#endif
		if (!dev->irq)
			dev->irq = i;
	}

	pr_cont(" IRQ %d", dev->irq);

#if ALLOW_DMA
	if (lp->use_dma) {
		get_dma_channel(dev);
		pr_cont(", DMA %d", dev->dma);
	} else
#endif
		pr_cont(", programmed I/O");

	/* print the ethernet address. */
	pr_cont(", MAC %pM\n", dev->dev_addr);

	dev->netdev_ops	= &net_ops;
	dev->watchdog_timeo = HZ;

	cs89_dbg(0, info, "cs89x0_probe1() successful\n");

	retval = register_netdev(dev);
	if (retval)
		goto out2;
	return 0;
out2:
	iowrite16(PP_ChipID, lp->virt_addr + ADD_PORT);
out1:
	return retval;
}

#ifndef CONFIG_CS89x0_PLATFORM
/*
 * This function converts the I/O port address used by the cs89x0_probe() and
 * init_module() functions to the I/O memory address used by the
 * cs89x0_probe1() function.
 */
static int __init
cs89x0_ioport_probe(struct net_device *dev, unsigned long ioport, int modular)
{
	struct net_local *lp = netdev_priv(dev);
	int ret;
	void __iomem *io_mem;

	if (!lp)
		return -ENOMEM;

	dev->base_addr = ioport;

	if (!request_region(ioport, NETCARD_IO_EXTENT, DRV_NAME)) {
		ret = -EBUSY;
		goto out;
	}

	io_mem = ioport_map(ioport & ~3, NETCARD_IO_EXTENT);
	if (!io_mem) {
		ret = -ENOMEM;
		goto release;
	}

	/* if they give us an odd I/O address, then do ONE write to
	 * the address port, to get it back to address zero, where we
	 * expect to find the EISA signature word. An IO with a base of 0x3
	 * will skip the test for the ADD_PORT.
	 */
	if (ioport & 1) {
		cs89_dbg(1, info, "%s: odd ioaddr 0x%lx\n", dev->name, ioport);
		if ((ioport & 2) != 2) {
			if ((ioread16(io_mem + ADD_PORT) & ADD_MASK) !=
			    ADD_SIG) {
				pr_err("%s: bad signature 0x%x\n",
				       dev->name, ioread16(io_mem + ADD_PORT));
				ret = -ENODEV;
				goto unmap;
			}
		}
	}

	ret = cs89x0_probe1(dev, io_mem, modular);
	if (!ret)
		goto out;
unmap:
	ioport_unmap(io_mem);
release:
	release_region(ioport, NETCARD_IO_EXTENT);
out:
	return ret;
}

#ifndef MODULE
/* Check for a network adaptor of this type, and return '0' iff one exists.
 * If dev->base_addr == 0, probe all likely locations.
 * If dev->base_addr == 1, always return failure.
 * If dev->base_addr == 2, allocate space for the device and return success
 * (detachable devices only).
 * Return 0 on success.
 */

struct net_device * __init cs89x0_probe(int unit)
{
	struct net_device *dev = alloc_etherdev(sizeof(struct net_local));
	unsigned *port;
	int err = 0;
	int irq;
	int io;

	if (!dev)
		return ERR_PTR(-ENODEV);

	sprintf(dev->name, "eth%d", unit);
	netdev_boot_setup_check(dev);
	io = dev->base_addr;
	irq = dev->irq;

	cs89_dbg(0, info, "cs89x0_probe(0x%x)\n", io);

	if (io > 0x1ff)	{	/* Check a single specified location. */
		err = cs89x0_ioport_probe(dev, io, 0);
	} else if (io != 0) {	/* Don't probe at all. */
		err = -ENXIO;
	} else {
		for (port = netcard_portlist; *port; port++) {
			if (cs89x0_ioport_probe(dev, *port, 0) == 0)
				break;
			dev->irq = irq;
		}
		if (!*port)
			err = -ENODEV;
	}
	if (err)
		goto out;
	return dev;
out:
	free_netdev(dev);
	pr_warn("no cs8900 or cs8920 detected.  Be sure to disable PnP with SETUP\n");
	return ERR_PTR(err);
}
#endif
#endif

#if defined(MODULE) && !defined(CONFIG_CS89x0_PLATFORM)

static struct net_device *dev_cs89x0;

/* Support the 'debug' module parm even if we're compiled for non-debug to
 * avoid breaking someone's startup scripts
 */

static int io;
static int irq;
static int debug;
static char media[8];
static int duplex = -1;

static int use_dma;			/* These generate unused var warnings if ALLOW_DMA = 0 */
static int dma;
static int dmasize = 16;		/* or 64 */

module_param_hw(io, int, ioport, 0);
module_param_hw(irq, int, irq, 0);
module_param(debug, int, 0);
module_param_string(media, media, sizeof(media), 0);
module_param(duplex, int, 0);
module_param_hw(dma , int, dma, 0);
module_param(dmasize , int, 0);
module_param(use_dma , int, 0);
MODULE_PARM_DESC(io, "cs89x0 I/O base address");
MODULE_PARM_DESC(irq, "cs89x0 IRQ number");
#if DEBUGGING
MODULE_PARM_DESC(debug, "cs89x0 debug level (0-6)");
#else
MODULE_PARM_DESC(debug, "(ignored)");
#endif
MODULE_PARM_DESC(media, "Set cs89x0 adapter(s) media type(s) (rj45,bnc,aui)");
/* No other value than -1 for duplex seems to be currently interpreted */
MODULE_PARM_DESC(duplex, "(ignored)");
#if ALLOW_DMA
MODULE_PARM_DESC(dma , "cs89x0 ISA DMA channel; ignored if use_dma=0");
MODULE_PARM_DESC(dmasize , "cs89x0 DMA size in kB (16,64); ignored if use_dma=0");
MODULE_PARM_DESC(use_dma , "cs89x0 using DMA (0-1)");
#else
MODULE_PARM_DESC(dma , "(ignored)");
MODULE_PARM_DESC(dmasize , "(ignored)");
MODULE_PARM_DESC(use_dma , "(ignored)");
#endif

MODULE_AUTHOR("Mike Cruse, Russwll Nelson <nelson@crynwr.com>, Andrew Morton");
MODULE_LICENSE("GPL");

/*
 * media=t             - specify media type
 * or media=2
 * or media=aui
 * or medai=auto
 * duplex=0            - specify forced half/full/autonegotiate duplex
 * debug=#             - debug level
 *
 * Default Chip Configuration:
 * DMA Burst = enabled
 * IOCHRDY Enabled = enabled
 * UseSA = enabled
 * CS8900 defaults to half-duplex if not specified on command-line
 * CS8920 defaults to autoneg if not specified on command-line
 * Use reset defaults for other config parameters
 *
 * Assumptions:
 * media type specified is supported (circuitry is present)
 * if memory address is > 1MB, then required mem decode hw is present
 * if 10B-2, then agent other than driver will enable DC/DC converter
 * (hw or software util)
 */

int __init init_module(void)
{
	struct net_device *dev = alloc_etherdev(sizeof(struct net_local));
	struct net_local *lp;
	int ret = 0;

#if DEBUGGING
	net_debug = debug;
#else
	debug = 0;
#endif
	if (!dev)
		return -ENOMEM;

	dev->irq = irq;
	dev->base_addr = io;
	lp = netdev_priv(dev);

#if ALLOW_DMA
	if (use_dma) {
		lp->use_dma = use_dma;
		lp->dma = dma;
		lp->dmasize = dmasize;
	}
#endif

	spin_lock_init(&lp->lock);

	/* boy, they'd better get these right */
	if (!strcmp(media, "rj45"))
		lp->adapter_cnf = A_CNF_MEDIA_10B_T | A_CNF_10B_T;
	else if (!strcmp(media, "aui"))
		lp->adapter_cnf = A_CNF_MEDIA_AUI   | A_CNF_AUI;
	else if (!strcmp(media, "bnc"))
		lp->adapter_cnf = A_CNF_MEDIA_10B_2 | A_CNF_10B_2;
	else
		lp->adapter_cnf = A_CNF_MEDIA_10B_T | A_CNF_10B_T;

	if (duplex == -1)
		lp->auto_neg_cnf = AUTO_NEG_ENABLE;

	if (io == 0) {
		pr_err("Module autoprobing not allowed\n");
		pr_err("Append io=0xNNN\n");
		ret = -EPERM;
		goto out;
	} else if (io <= 0x1ff) {
		ret = -ENXIO;
		goto out;
	}

#if ALLOW_DMA
	if (use_dma && dmasize != 16 && dmasize != 64) {
		pr_err("dma size must be either 16K or 64K, not %dK\n",
		       dmasize);
		ret = -EPERM;
		goto out;
	}
#endif
	ret = cs89x0_ioport_probe(dev, io, 1);
	if (ret)
		goto out;

	dev_cs89x0 = dev;
	return 0;
out:
	free_netdev(dev);
	return ret;
}

void __exit
cleanup_module(void)
{
	struct net_local *lp = netdev_priv(dev_cs89x0);

	unregister_netdev(dev_cs89x0);
	iowrite16(PP_ChipID, lp->virt_addr + ADD_PORT);
	ioport_unmap(lp->virt_addr);
	release_region(dev_cs89x0->base_addr, NETCARD_IO_EXTENT);
	free_netdev(dev_cs89x0);
}
#endif /* MODULE && !CONFIG_CS89x0_PLATFORM */

#ifdef CONFIG_CS89x0_PLATFORM
static int __init cs89x0_platform_probe(struct platform_device *pdev)
{
	struct net_device *dev = alloc_etherdev(sizeof(struct net_local));
	void __iomem *virt_addr;
	int err;

	if (!dev)
		return -ENOMEM;

	dev->irq = platform_get_irq(pdev, 0);
	if (dev->irq <= 0) {
		dev_warn(&dev->dev, "interrupt resource missing\n");
		err = -ENXIO;
		goto free;
	}

	virt_addr = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(virt_addr)) {
		err = PTR_ERR(virt_addr);
		goto free;
	}

	err = cs89x0_probe1(dev, virt_addr, 0);
	if (err) {
		dev_warn(&dev->dev, "no cs8900 or cs8920 detected\n");
		goto free;
	}

	platform_set_drvdata(pdev, dev);
	return 0;

free:
	free_netdev(dev);
	return err;
}

static int cs89x0_platform_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);

	/* This platform_get_resource() call will not return NULL, because
	 * the same call in cs89x0_platform_probe() has returned a non NULL
	 * value.
	 */
	unregister_netdev(dev);
	free_netdev(dev);
	return 0;
}

static const struct of_device_id __maybe_unused cs89x0_match[] = {
	{ .compatible = "cirrus,cs8900", },
	{ .compatible = "cirrus,cs8920", },
	{ },
};
MODULE_DEVICE_TABLE(of, cs89x0_match);

static struct platform_driver cs89x0_driver = {
	.driver	= {
		.name		= DRV_NAME,
		.of_match_table	= of_match_ptr(cs89x0_match),
	},
	.remove	= cs89x0_platform_remove,
};

module_platform_driver_probe(cs89x0_driver, cs89x0_platform_probe);

#endif /* CONFIG_CS89x0_PLATFORM */

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Crystal Semiconductor (Now Cirrus Logic) CS89[02]0 network driver");
MODULE_AUTHOR("Russell Nelson <nelson@crynwr.com>");
