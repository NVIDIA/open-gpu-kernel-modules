/* sundance.c: A Linux device driver for the Sundance ST201 "Alta". */
/*
	Written 1999-2000 by Donald Becker.

	This software may be used and distributed according to the terms of
	the GNU General Public License (GPL), incorporated herein by reference.
	Drivers based on or derived from this code fall under the GPL and must
	retain the authorship, copyright and license notice.  This file is not
	a complete program and may only be used when the entire operating
	system is licensed under the GPL.

	The author may be reached as becker@scyld.com, or C/O
	Scyld Computing Corporation
	410 Severn Ave., Suite 210
	Annapolis MD 21403

	Support and updates available at
	http://www.scyld.com/network/sundance.html
	[link no longer provides useful info -jgarzik]
	Archives of the mailing list are still available at
	https://www.beowulf.org/pipermail/netdrivers/

*/

#define DRV_NAME	"sundance"

/* The user-configurable values.
   These may be modified when a driver module is loaded.*/
static int debug = 1;			/* 1 normal messages, 0 quiet .. 7 verbose. */
/* Maximum number of multicast addresses to filter (vs. rx-all-multicast).
   Typical is a 64 element hash table based on the Ethernet CRC.  */
static const int multicast_filter_limit = 32;

/* Set the copy breakpoint for the copy-only-tiny-frames scheme.
   Setting to > 1518 effectively disables this feature.
   This chip can receive into offset buffers, so the Alpha does not
   need a copy-align. */
static int rx_copybreak;
static int flowctrl=1;

/* media[] specifies the media type the NIC operates at.
		 autosense	Autosensing active media.
		 10mbps_hd 	10Mbps half duplex.
		 10mbps_fd 	10Mbps full duplex.
		 100mbps_hd 	100Mbps half duplex.
		 100mbps_fd 	100Mbps full duplex.
		 0		Autosensing active media.
		 1	 	10Mbps half duplex.
		 2	 	10Mbps full duplex.
		 3	 	100Mbps half duplex.
		 4	 	100Mbps full duplex.
*/
#define MAX_UNITS 8
static char *media[MAX_UNITS];


/* Operational parameters that are set at compile time. */

/* Keep the ring sizes a power of two for compile efficiency.
   The compiler will convert <unsigned>'%'<2^N> into a bit mask.
   Making the Tx ring too large decreases the effectiveness of channel
   bonding and packet priority, and more than 128 requires modifying the
   Tx error recovery.
   Large receive rings merely waste memory. */
#define TX_RING_SIZE	32
#define TX_QUEUE_LEN	(TX_RING_SIZE - 1) /* Limit ring entries actually used.  */
#define RX_RING_SIZE	64
#define RX_BUDGET	32
#define TX_TOTAL_SIZE	TX_RING_SIZE*sizeof(struct netdev_desc)
#define RX_TOTAL_SIZE	RX_RING_SIZE*sizeof(struct netdev_desc)

/* Operational parameters that usually are not changed. */
/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT  (4*HZ)
#define PKT_BUF_SZ		1536	/* Size of each temporary Rx buffer.*/

/* Include files, designed to support most kernel versions 2.0.0 and later. */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/uaccess.h>
#include <asm/processor.h>		/* Processor type for cache alignment. */
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/crc32.h>
#include <linux/ethtool.h>
#include <linux/mii.h>

MODULE_AUTHOR("Donald Becker <becker@scyld.com>");
MODULE_DESCRIPTION("Sundance Alta Ethernet driver");
MODULE_LICENSE("GPL");

module_param(debug, int, 0);
module_param(rx_copybreak, int, 0);
module_param_array(media, charp, NULL, 0);
module_param(flowctrl, int, 0);
MODULE_PARM_DESC(debug, "Sundance Alta debug level (0-5)");
MODULE_PARM_DESC(rx_copybreak, "Sundance Alta copy breakpoint for copy-only-tiny-frames");
MODULE_PARM_DESC(flowctrl, "Sundance Alta flow control [0|1]");

/*
				Theory of Operation

I. Board Compatibility

This driver is designed for the Sundance Technologies "Alta" ST201 chip.

II. Board-specific settings

III. Driver operation

IIIa. Ring buffers

This driver uses two statically allocated fixed-size descriptor lists
formed into rings by a branch from the final descriptor to the beginning of
the list.  The ring sizes are set at compile time by RX/TX_RING_SIZE.
Some chips explicitly use only 2^N sized rings, while others use a
'next descriptor' pointer that the driver forms into rings.

IIIb/c. Transmit/Receive Structure

This driver uses a zero-copy receive and transmit scheme.
The driver allocates full frame size skbuffs for the Rx ring buffers at
open() time and passes the skb->data field to the chip as receive data
buffers.  When an incoming frame is less than RX_COPYBREAK bytes long,
a fresh skbuff is allocated and the frame is copied to the new skbuff.
When the incoming frame is larger, the skbuff is passed directly up the
protocol stack.  Buffers consumed this way are replaced by newly allocated
skbuffs in a later phase of receives.

The RX_COPYBREAK value is chosen to trade-off the memory wasted by
using a full-sized skbuff for small frames vs. the copying costs of larger
frames.  New boards are typically used in generously configured machines
and the underfilled buffers have negligible impact compared to the benefit of
a single allocation size, so the default value of zero results in never
copying packets.  When copying is done, the cost is usually mitigated by using
a combined copy/checksum routine.  Copying also preloads the cache, which is
most useful with small frames.

A subtle aspect of the operation is that the IP header at offset 14 in an
ethernet frame isn't longword aligned for further processing.
Unaligned buffers are permitted by the Sundance hardware, so
frames are received into the skbuff at an offset of "+2", 16-byte aligning
the IP header.

IIId. Synchronization

The driver runs as two independent, single-threaded flows of control.  One
is the send-packet routine, which enforces single-threaded use by the
dev->tbusy flag.  The other thread is the interrupt handler, which is single
threaded by the hardware and interrupt handling software.

The send packet thread has partial control over the Tx ring and 'dev->tbusy'
flag.  It sets the tbusy flag whenever it's queuing a Tx packet. If the next
queue slot is empty, it clears the tbusy flag when finished otherwise it sets
the 'lp->tx_full' flag.

The interrupt handler has exclusive control over the Rx ring and records stats
from the Tx ring.  After reaping the stats, it marks the Tx queue entry as
empty by incrementing the dirty_tx mark. Iff the 'lp->tx_full' flag is set, it
clears both the tx_full and tbusy flags.

IV. Notes

IVb. References

The Sundance ST201 datasheet, preliminary version.
The Kendin KS8723 datasheet, preliminary version.
The ICplus IP100 datasheet, preliminary version.
http://www.scyld.com/expert/100mbps.html
http://www.scyld.com/expert/NWay.html

IVc. Errata

*/

/* Work-around for Kendin chip bugs. */
#ifndef CONFIG_SUNDANCE_MMIO
#define USE_IO_OPS 1
#endif

static const struct pci_device_id sundance_pci_tbl[] = {
	{ 0x1186, 0x1002, 0x1186, 0x1002, 0, 0, 0 },
	{ 0x1186, 0x1002, 0x1186, 0x1003, 0, 0, 1 },
	{ 0x1186, 0x1002, 0x1186, 0x1012, 0, 0, 2 },
	{ 0x1186, 0x1002, 0x1186, 0x1040, 0, 0, 3 },
	{ 0x1186, 0x1002, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 4 },
	{ 0x13F0, 0x0201, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 5 },
	{ 0x13F0, 0x0200, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 6 },
	{ }
};
MODULE_DEVICE_TABLE(pci, sundance_pci_tbl);

enum {
	netdev_io_size = 128
};

struct pci_id_info {
        const char *name;
};
static const struct pci_id_info pci_id_tbl[] = {
	{"D-Link DFE-550TX FAST Ethernet Adapter"},
	{"D-Link DFE-550FX 100Mbps Fiber-optics Adapter"},
	{"D-Link DFE-580TX 4 port Server Adapter"},
	{"D-Link DFE-530TXS FAST Ethernet Adapter"},
	{"D-Link DL10050-based FAST Ethernet Adapter"},
	{"Sundance Technology Alta"},
	{"IC Plus Corporation IP100A FAST Ethernet Adapter"},
	{ }	/* terminate list. */
};

/* This driver was written to use PCI memory space, however x86-oriented
   hardware often uses I/O space accesses. */

/* Offsets to the device registers.
   Unlike software-only systems, device drivers interact with complex hardware.
   It's not useful to define symbolic names for every register bit in the
   device.  The name can only partially document the semantics and make
   the driver longer and more difficult to read.
   In general, only the important configuration values or bits changed
   multiple times should be defined symbolically.
*/
enum alta_offsets {
	DMACtrl = 0x00,
	TxListPtr = 0x04,
	TxDMABurstThresh = 0x08,
	TxDMAUrgentThresh = 0x09,
	TxDMAPollPeriod = 0x0a,
	RxDMAStatus = 0x0c,
	RxListPtr = 0x10,
	DebugCtrl0 = 0x1a,
	DebugCtrl1 = 0x1c,
	RxDMABurstThresh = 0x14,
	RxDMAUrgentThresh = 0x15,
	RxDMAPollPeriod = 0x16,
	LEDCtrl = 0x1a,
	ASICCtrl = 0x30,
	EEData = 0x34,
	EECtrl = 0x36,
	FlashAddr = 0x40,
	FlashData = 0x44,
	WakeEvent = 0x45,
	TxStatus = 0x46,
	TxFrameId = 0x47,
	DownCounter = 0x18,
	IntrClear = 0x4a,
	IntrEnable = 0x4c,
	IntrStatus = 0x4e,
	MACCtrl0 = 0x50,
	MACCtrl1 = 0x52,
	StationAddr = 0x54,
	MaxFrameSize = 0x5A,
	RxMode = 0x5c,
	MIICtrl = 0x5e,
	MulticastFilter0 = 0x60,
	MulticastFilter1 = 0x64,
	RxOctetsLow = 0x68,
	RxOctetsHigh = 0x6a,
	TxOctetsLow = 0x6c,
	TxOctetsHigh = 0x6e,
	TxFramesOK = 0x70,
	RxFramesOK = 0x72,
	StatsCarrierError = 0x74,
	StatsLateColl = 0x75,
	StatsMultiColl = 0x76,
	StatsOneColl = 0x77,
	StatsTxDefer = 0x78,
	RxMissed = 0x79,
	StatsTxXSDefer = 0x7a,
	StatsTxAbort = 0x7b,
	StatsBcastTx = 0x7c,
	StatsBcastRx = 0x7d,
	StatsMcastTx = 0x7e,
	StatsMcastRx = 0x7f,
	/* Aliased and bogus values! */
	RxStatus = 0x0c,
};

#define ASIC_HI_WORD(x)	((x) + 2)

enum ASICCtrl_HiWord_bit {
	GlobalReset = 0x0001,
	RxReset = 0x0002,
	TxReset = 0x0004,
	DMAReset = 0x0008,
	FIFOReset = 0x0010,
	NetworkReset = 0x0020,
	HostReset = 0x0040,
	ResetBusy = 0x0400,
};

/* Bits in the interrupt status/mask registers. */
enum intr_status_bits {
	IntrSummary=0x0001, IntrPCIErr=0x0002, IntrMACCtrl=0x0008,
	IntrTxDone=0x0004, IntrRxDone=0x0010, IntrRxStart=0x0020,
	IntrDrvRqst=0x0040,
	StatsMax=0x0080, LinkChange=0x0100,
	IntrTxDMADone=0x0200, IntrRxDMADone=0x0400,
};

/* Bits in the RxMode register. */
enum rx_mode_bits {
	AcceptAllIPMulti=0x20, AcceptMultiHash=0x10, AcceptAll=0x08,
	AcceptBroadcast=0x04, AcceptMulticast=0x02, AcceptMyPhys=0x01,
};
/* Bits in MACCtrl. */
enum mac_ctrl0_bits {
	EnbFullDuplex=0x20, EnbRcvLargeFrame=0x40,
	EnbFlowCtrl=0x100, EnbPassRxCRC=0x200,
};
enum mac_ctrl1_bits {
	StatsEnable=0x0020,	StatsDisable=0x0040, StatsEnabled=0x0080,
	TxEnable=0x0100, TxDisable=0x0200, TxEnabled=0x0400,
	RxEnable=0x0800, RxDisable=0x1000, RxEnabled=0x2000,
};

/* Bits in WakeEvent register. */
enum wake_event_bits {
	WakePktEnable = 0x01,
	MagicPktEnable = 0x02,
	LinkEventEnable = 0x04,
	WolEnable = 0x80,
};

/* The Rx and Tx buffer descriptors. */
/* Note that using only 32 bit fields simplifies conversion to big-endian
   architectures. */
struct netdev_desc {
	__le32 next_desc;
	__le32 status;
	struct desc_frag { __le32 addr, length; } frag[1];
};

/* Bits in netdev_desc.status */
enum desc_status_bits {
	DescOwn=0x8000,
	DescEndPacket=0x4000,
	DescEndRing=0x2000,
	LastFrag=0x80000000,
	DescIntrOnTx=0x8000,
	DescIntrOnDMADone=0x80000000,
	DisableAlign = 0x00000001,
};

#define PRIV_ALIGN	15 	/* Required alignment mask */
/* Use  __attribute__((aligned (L1_CACHE_BYTES)))  to maintain alignment
   within the structure. */
#define MII_CNT		4
struct netdev_private {
	/* Descriptor rings first for alignment. */
	struct netdev_desc *rx_ring;
	struct netdev_desc *tx_ring;
	struct sk_buff* rx_skbuff[RX_RING_SIZE];
	struct sk_buff* tx_skbuff[TX_RING_SIZE];
        dma_addr_t tx_ring_dma;
        dma_addr_t rx_ring_dma;
	struct timer_list timer;		/* Media monitoring timer. */
	struct net_device *ndev;		/* backpointer */
	/* ethtool extra stats */
	struct {
		u64 tx_multiple_collisions;
		u64 tx_single_collisions;
		u64 tx_late_collisions;
		u64 tx_deferred;
		u64 tx_deferred_excessive;
		u64 tx_aborted;
		u64 tx_bcasts;
		u64 rx_bcasts;
		u64 tx_mcasts;
		u64 rx_mcasts;
	} xstats;
	/* Frequently used values: keep some adjacent for cache effect. */
	spinlock_t lock;
	int msg_enable;
	int chip_id;
	unsigned int cur_rx, dirty_rx;		/* Producer/consumer ring indices */
	unsigned int rx_buf_sz;			/* Based on MTU+slack. */
	struct netdev_desc *last_tx;		/* Last Tx descriptor used. */
	unsigned int cur_tx, dirty_tx;
	/* These values are keep track of the transceiver/media in use. */
	unsigned int flowctrl:1;
	unsigned int default_port:4;		/* Last dev->if_port value. */
	unsigned int an_enable:1;
	unsigned int speed;
	unsigned int wol_enabled:1;			/* Wake on LAN enabled */
	struct tasklet_struct rx_tasklet;
	struct tasklet_struct tx_tasklet;
	int budget;
	int cur_task;
	/* Multicast and receive mode. */
	spinlock_t mcastlock;			/* SMP lock multicast updates. */
	u16 mcast_filter[4];
	/* MII transceiver section. */
	struct mii_if_info mii_if;
	int mii_preamble_required;
	unsigned char phys[MII_CNT];		/* MII device addresses, only first one used. */
	struct pci_dev *pci_dev;
	void __iomem *base;
	spinlock_t statlock;
};

/* The station address location in the EEPROM. */
#define EEPROM_SA_OFFSET	0x10
#define DEFAULT_INTR (IntrRxDMADone | IntrPCIErr | \
			IntrDrvRqst | IntrTxDone | StatsMax | \
			LinkChange)

static int  change_mtu(struct net_device *dev, int new_mtu);
static int  eeprom_read(void __iomem *ioaddr, int location);
static int  mdio_read(struct net_device *dev, int phy_id, int location);
static void mdio_write(struct net_device *dev, int phy_id, int location, int value);
static int  mdio_wait_link(struct net_device *dev, int wait);
static int  netdev_open(struct net_device *dev);
static void check_duplex(struct net_device *dev);
static void netdev_timer(struct timer_list *t);
static void tx_timeout(struct net_device *dev, unsigned int txqueue);
static void init_ring(struct net_device *dev);
static netdev_tx_t start_tx(struct sk_buff *skb, struct net_device *dev);
static int reset_tx (struct net_device *dev);
static irqreturn_t intr_handler(int irq, void *dev_instance);
static void rx_poll(struct tasklet_struct *t);
static void tx_poll(struct tasklet_struct *t);
static void refill_rx (struct net_device *dev);
static void netdev_error(struct net_device *dev, int intr_status);
static void netdev_error(struct net_device *dev, int intr_status);
static void set_rx_mode(struct net_device *dev);
static int __set_mac_addr(struct net_device *dev);
static int sundance_set_mac_addr(struct net_device *dev, void *data);
static struct net_device_stats *get_stats(struct net_device *dev);
static int netdev_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
static int  netdev_close(struct net_device *dev);
static const struct ethtool_ops ethtool_ops;

static void sundance_reset(struct net_device *dev, unsigned long reset_cmd)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base + ASICCtrl;
	int countdown;

	/* ST201 documentation states ASICCtrl is a 32bit register */
	iowrite32 (reset_cmd | ioread32 (ioaddr), ioaddr);
	/* ST201 documentation states reset can take up to 1 ms */
	countdown = 10 + 1;
	while (ioread32 (ioaddr) & (ResetBusy << 16)) {
		if (--countdown == 0) {
			printk(KERN_WARNING "%s : reset not completed !!\n", dev->name);
			break;
		}
		udelay(100);
	}
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void sundance_poll_controller(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);

	disable_irq(np->pci_dev->irq);
	intr_handler(np->pci_dev->irq, dev);
	enable_irq(np->pci_dev->irq);
}
#endif

static const struct net_device_ops netdev_ops = {
	.ndo_open		= netdev_open,
	.ndo_stop		= netdev_close,
	.ndo_start_xmit		= start_tx,
	.ndo_get_stats 		= get_stats,
	.ndo_set_rx_mode	= set_rx_mode,
	.ndo_do_ioctl 		= netdev_ioctl,
	.ndo_tx_timeout		= tx_timeout,
	.ndo_change_mtu		= change_mtu,
	.ndo_set_mac_address 	= sundance_set_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller 	= sundance_poll_controller,
#endif
};

static int sundance_probe1(struct pci_dev *pdev,
			   const struct pci_device_id *ent)
{
	struct net_device *dev;
	struct netdev_private *np;
	static int card_idx;
	int chip_idx = ent->driver_data;
	int irq;
	int i;
	void __iomem *ioaddr;
	u16 mii_ctl;
	void *ring_space;
	dma_addr_t ring_dma;
#ifdef USE_IO_OPS
	int bar = 0;
#else
	int bar = 1;
#endif
	int phy, phy_end, phy_idx = 0;

	if (pci_enable_device(pdev))
		return -EIO;
	pci_set_master(pdev);

	irq = pdev->irq;

	dev = alloc_etherdev(sizeof(*np));
	if (!dev)
		return -ENOMEM;
	SET_NETDEV_DEV(dev, &pdev->dev);

	if (pci_request_regions(pdev, DRV_NAME))
		goto err_out_netdev;

	ioaddr = pci_iomap(pdev, bar, netdev_io_size);
	if (!ioaddr)
		goto err_out_res;

	for (i = 0; i < 3; i++)
		((__le16 *)dev->dev_addr)[i] =
			cpu_to_le16(eeprom_read(ioaddr, i + EEPROM_SA_OFFSET));

	np = netdev_priv(dev);
	np->ndev = dev;
	np->base = ioaddr;
	np->pci_dev = pdev;
	np->chip_id = chip_idx;
	np->msg_enable = (1 << debug) - 1;
	spin_lock_init(&np->lock);
	spin_lock_init(&np->statlock);
	tasklet_setup(&np->rx_tasklet, rx_poll);
	tasklet_setup(&np->tx_tasklet, tx_poll);

	ring_space = dma_alloc_coherent(&pdev->dev, TX_TOTAL_SIZE,
			&ring_dma, GFP_KERNEL);
	if (!ring_space)
		goto err_out_cleardev;
	np->tx_ring = (struct netdev_desc *)ring_space;
	np->tx_ring_dma = ring_dma;

	ring_space = dma_alloc_coherent(&pdev->dev, RX_TOTAL_SIZE,
			&ring_dma, GFP_KERNEL);
	if (!ring_space)
		goto err_out_unmap_tx;
	np->rx_ring = (struct netdev_desc *)ring_space;
	np->rx_ring_dma = ring_dma;

	np->mii_if.dev = dev;
	np->mii_if.mdio_read = mdio_read;
	np->mii_if.mdio_write = mdio_write;
	np->mii_if.phy_id_mask = 0x1f;
	np->mii_if.reg_num_mask = 0x1f;

	/* The chip-specific entries in the device structure. */
	dev->netdev_ops = &netdev_ops;
	dev->ethtool_ops = &ethtool_ops;
	dev->watchdog_timeo = TX_TIMEOUT;

	/* MTU range: 68 - 8191 */
	dev->min_mtu = ETH_MIN_MTU;
	dev->max_mtu = 8191;

	pci_set_drvdata(pdev, dev);

	i = register_netdev(dev);
	if (i)
		goto err_out_unmap_rx;

	printk(KERN_INFO "%s: %s at %p, %pM, IRQ %d.\n",
	       dev->name, pci_id_tbl[chip_idx].name, ioaddr,
	       dev->dev_addr, irq);

	np->phys[0] = 1;		/* Default setting */
	np->mii_preamble_required++;

	/*
	 * It seems some phys doesn't deal well with address 0 being accessed
	 * first
	 */
	if (sundance_pci_tbl[np->chip_id].device == 0x0200) {
		phy = 0;
		phy_end = 31;
	} else {
		phy = 1;
		phy_end = 32;	/* wraps to zero, due to 'phy & 0x1f' */
	}
	for (; phy <= phy_end && phy_idx < MII_CNT; phy++) {
		int phyx = phy & 0x1f;
		int mii_status = mdio_read(dev, phyx, MII_BMSR);
		if (mii_status != 0xffff  &&  mii_status != 0x0000) {
			np->phys[phy_idx++] = phyx;
			np->mii_if.advertising = mdio_read(dev, phyx, MII_ADVERTISE);
			if ((mii_status & 0x0040) == 0)
				np->mii_preamble_required++;
			printk(KERN_INFO "%s: MII PHY found at address %d, status "
				   "0x%4.4x advertising %4.4x.\n",
				   dev->name, phyx, mii_status, np->mii_if.advertising);
		}
	}
	np->mii_preamble_required--;

	if (phy_idx == 0) {
		printk(KERN_INFO "%s: No MII transceiver found, aborting.  ASIC status %x\n",
			   dev->name, ioread32(ioaddr + ASICCtrl));
		goto err_out_unregister;
	}

	np->mii_if.phy_id = np->phys[0];

	/* Parse override configuration */
	np->an_enable = 1;
	if (card_idx < MAX_UNITS) {
		if (media[card_idx] != NULL) {
			np->an_enable = 0;
			if (strcmp (media[card_idx], "100mbps_fd") == 0 ||
			    strcmp (media[card_idx], "4") == 0) {
				np->speed = 100;
				np->mii_if.full_duplex = 1;
			} else if (strcmp (media[card_idx], "100mbps_hd") == 0 ||
				   strcmp (media[card_idx], "3") == 0) {
				np->speed = 100;
				np->mii_if.full_duplex = 0;
			} else if (strcmp (media[card_idx], "10mbps_fd") == 0 ||
				   strcmp (media[card_idx], "2") == 0) {
				np->speed = 10;
				np->mii_if.full_duplex = 1;
			} else if (strcmp (media[card_idx], "10mbps_hd") == 0 ||
				   strcmp (media[card_idx], "1") == 0) {
				np->speed = 10;
				np->mii_if.full_duplex = 0;
			} else {
				np->an_enable = 1;
			}
		}
		if (flowctrl == 1)
			np->flowctrl = 1;
	}

	/* Fibre PHY? */
	if (ioread32 (ioaddr + ASICCtrl) & 0x80) {
		/* Default 100Mbps Full */
		if (np->an_enable) {
			np->speed = 100;
			np->mii_if.full_duplex = 1;
			np->an_enable = 0;
		}
	}
	/* Reset PHY */
	mdio_write (dev, np->phys[0], MII_BMCR, BMCR_RESET);
	mdelay (300);
	/* If flow control enabled, we need to advertise it.*/
	if (np->flowctrl)
		mdio_write (dev, np->phys[0], MII_ADVERTISE, np->mii_if.advertising | 0x0400);
	mdio_write (dev, np->phys[0], MII_BMCR, BMCR_ANENABLE|BMCR_ANRESTART);
	/* Force media type */
	if (!np->an_enable) {
		mii_ctl = 0;
		mii_ctl |= (np->speed == 100) ? BMCR_SPEED100 : 0;
		mii_ctl |= (np->mii_if.full_duplex) ? BMCR_FULLDPLX : 0;
		mdio_write (dev, np->phys[0], MII_BMCR, mii_ctl);
		printk (KERN_INFO "Override speed=%d, %s duplex\n",
			np->speed, np->mii_if.full_duplex ? "Full" : "Half");

	}

	/* Perhaps move the reset here? */
	/* Reset the chip to erase previous misconfiguration. */
	if (netif_msg_hw(np))
		printk("ASIC Control is %x.\n", ioread32(ioaddr + ASICCtrl));
	sundance_reset(dev, 0x00ff << 16);
	if (netif_msg_hw(np))
		printk("ASIC Control is now %x.\n", ioread32(ioaddr + ASICCtrl));

	card_idx++;
	return 0;

err_out_unregister:
	unregister_netdev(dev);
err_out_unmap_rx:
	dma_free_coherent(&pdev->dev, RX_TOTAL_SIZE,
		np->rx_ring, np->rx_ring_dma);
err_out_unmap_tx:
	dma_free_coherent(&pdev->dev, TX_TOTAL_SIZE,
		np->tx_ring, np->tx_ring_dma);
err_out_cleardev:
	pci_iounmap(pdev, ioaddr);
err_out_res:
	pci_release_regions(pdev);
err_out_netdev:
	free_netdev (dev);
	return -ENODEV;
}

static int change_mtu(struct net_device *dev, int new_mtu)
{
	if (netif_running(dev))
		return -EBUSY;
	dev->mtu = new_mtu;
	return 0;
}

#define eeprom_delay(ee_addr)	ioread32(ee_addr)
/* Read the EEPROM and MII Management Data I/O (MDIO) interfaces. */
static int eeprom_read(void __iomem *ioaddr, int location)
{
	int boguscnt = 10000;		/* Typical 1900 ticks. */
	iowrite16(0x0200 | (location & 0xff), ioaddr + EECtrl);
	do {
		eeprom_delay(ioaddr + EECtrl);
		if (! (ioread16(ioaddr + EECtrl) & 0x8000)) {
			return ioread16(ioaddr + EEData);
		}
	} while (--boguscnt > 0);
	return 0;
}

/*  MII transceiver control section.
	Read and write the MII registers using software-generated serial
	MDIO protocol.  See the MII specifications or DP83840A data sheet
	for details.

	The maximum data clock rate is 2.5 Mhz.  The minimum timing is usually
	met by back-to-back 33Mhz PCI cycles. */
#define mdio_delay() ioread8(mdio_addr)

enum mii_reg_bits {
	MDIO_ShiftClk=0x0001, MDIO_Data=0x0002, MDIO_EnbOutput=0x0004,
};
#define MDIO_EnbIn  (0)
#define MDIO_WRITE0 (MDIO_EnbOutput)
#define MDIO_WRITE1 (MDIO_Data | MDIO_EnbOutput)

/* Generate the preamble required for initial synchronization and
   a few older transceivers. */
static void mdio_sync(void __iomem *mdio_addr)
{
	int bits = 32;

	/* Establish sync by sending at least 32 logic ones. */
	while (--bits >= 0) {
		iowrite8(MDIO_WRITE1, mdio_addr);
		mdio_delay();
		iowrite8(MDIO_WRITE1 | MDIO_ShiftClk, mdio_addr);
		mdio_delay();
	}
}

static int mdio_read(struct net_device *dev, int phy_id, int location)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *mdio_addr = np->base + MIICtrl;
	int mii_cmd = (0xf6 << 10) | (phy_id << 5) | location;
	int i, retval = 0;

	if (np->mii_preamble_required)
		mdio_sync(mdio_addr);

	/* Shift the read command bits out. */
	for (i = 15; i >= 0; i--) {
		int dataval = (mii_cmd & (1 << i)) ? MDIO_WRITE1 : MDIO_WRITE0;

		iowrite8(dataval, mdio_addr);
		mdio_delay();
		iowrite8(dataval | MDIO_ShiftClk, mdio_addr);
		mdio_delay();
	}
	/* Read the two transition, 16 data, and wire-idle bits. */
	for (i = 19; i > 0; i--) {
		iowrite8(MDIO_EnbIn, mdio_addr);
		mdio_delay();
		retval = (retval << 1) | ((ioread8(mdio_addr) & MDIO_Data) ? 1 : 0);
		iowrite8(MDIO_EnbIn | MDIO_ShiftClk, mdio_addr);
		mdio_delay();
	}
	return (retval>>1) & 0xffff;
}

static void mdio_write(struct net_device *dev, int phy_id, int location, int value)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *mdio_addr = np->base + MIICtrl;
	int mii_cmd = (0x5002 << 16) | (phy_id << 23) | (location<<18) | value;
	int i;

	if (np->mii_preamble_required)
		mdio_sync(mdio_addr);

	/* Shift the command bits out. */
	for (i = 31; i >= 0; i--) {
		int dataval = (mii_cmd & (1 << i)) ? MDIO_WRITE1 : MDIO_WRITE0;

		iowrite8(dataval, mdio_addr);
		mdio_delay();
		iowrite8(dataval | MDIO_ShiftClk, mdio_addr);
		mdio_delay();
	}
	/* Clear out extra bits. */
	for (i = 2; i > 0; i--) {
		iowrite8(MDIO_EnbIn, mdio_addr);
		mdio_delay();
		iowrite8(MDIO_EnbIn | MDIO_ShiftClk, mdio_addr);
		mdio_delay();
	}
}

static int mdio_wait_link(struct net_device *dev, int wait)
{
	int bmsr;
	int phy_id;
	struct netdev_private *np;

	np = netdev_priv(dev);
	phy_id = np->phys[0];

	do {
		bmsr = mdio_read(dev, phy_id, MII_BMSR);
		if (bmsr & 0x0004)
			return 0;
		mdelay(1);
	} while (--wait > 0);
	return -1;
}

static int netdev_open(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	const int irq = np->pci_dev->irq;
	unsigned long flags;
	int i;

	sundance_reset(dev, 0x00ff << 16);

	i = request_irq(irq, intr_handler, IRQF_SHARED, dev->name, dev);
	if (i)
		return i;

	if (netif_msg_ifup(np))
		printk(KERN_DEBUG "%s: netdev_open() irq %d\n", dev->name, irq);

	init_ring(dev);

	iowrite32(np->rx_ring_dma, ioaddr + RxListPtr);
	/* The Tx list pointer is written as packets are queued. */

	/* Initialize other registers. */
	__set_mac_addr(dev);
#if IS_ENABLED(CONFIG_VLAN_8021Q)
	iowrite16(dev->mtu + 18, ioaddr + MaxFrameSize);
#else
	iowrite16(dev->mtu + 14, ioaddr + MaxFrameSize);
#endif
	if (dev->mtu > 2047)
		iowrite32(ioread32(ioaddr + ASICCtrl) | 0x0C, ioaddr + ASICCtrl);

	/* Configure the PCI bus bursts and FIFO thresholds. */

	if (dev->if_port == 0)
		dev->if_port = np->default_port;

	spin_lock_init(&np->mcastlock);

	set_rx_mode(dev);
	iowrite16(0, ioaddr + IntrEnable);
	iowrite16(0, ioaddr + DownCounter);
	/* Set the chip to poll every N*320nsec. */
	iowrite8(100, ioaddr + RxDMAPollPeriod);
	iowrite8(127, ioaddr + TxDMAPollPeriod);
	/* Fix DFE-580TX packet drop issue */
	if (np->pci_dev->revision >= 0x14)
		iowrite8(0x01, ioaddr + DebugCtrl1);
	netif_start_queue(dev);

	spin_lock_irqsave(&np->lock, flags);
	reset_tx(dev);
	spin_unlock_irqrestore(&np->lock, flags);

	iowrite16 (StatsEnable | RxEnable | TxEnable, ioaddr + MACCtrl1);

	/* Disable Wol */
	iowrite8(ioread8(ioaddr + WakeEvent) | 0x00, ioaddr + WakeEvent);
	np->wol_enabled = 0;

	if (netif_msg_ifup(np))
		printk(KERN_DEBUG "%s: Done netdev_open(), status: Rx %x Tx %x "
			   "MAC Control %x, %4.4x %4.4x.\n",
			   dev->name, ioread32(ioaddr + RxStatus), ioread8(ioaddr + TxStatus),
			   ioread32(ioaddr + MACCtrl0),
			   ioread16(ioaddr + MACCtrl1), ioread16(ioaddr + MACCtrl0));

	/* Set the timer to check for link beat. */
	timer_setup(&np->timer, netdev_timer, 0);
	np->timer.expires = jiffies + 3*HZ;
	add_timer(&np->timer);

	/* Enable interrupts by setting the interrupt mask. */
	iowrite16(DEFAULT_INTR, ioaddr + IntrEnable);

	return 0;
}

static void check_duplex(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	int mii_lpa = mdio_read(dev, np->phys[0], MII_LPA);
	int negotiated = mii_lpa & np->mii_if.advertising;
	int duplex;

	/* Force media */
	if (!np->an_enable || mii_lpa == 0xffff) {
		if (np->mii_if.full_duplex)
			iowrite16 (ioread16 (ioaddr + MACCtrl0) | EnbFullDuplex,
				ioaddr + MACCtrl0);
		return;
	}

	/* Autonegotiation */
	duplex = (negotiated & 0x0100) || (negotiated & 0x01C0) == 0x0040;
	if (np->mii_if.full_duplex != duplex) {
		np->mii_if.full_duplex = duplex;
		if (netif_msg_link(np))
			printk(KERN_INFO "%s: Setting %s-duplex based on MII #%d "
				   "negotiated capability %4.4x.\n", dev->name,
				   duplex ? "full" : "half", np->phys[0], negotiated);
		iowrite16(ioread16(ioaddr + MACCtrl0) | (duplex ? 0x20 : 0), ioaddr + MACCtrl0);
	}
}

static void netdev_timer(struct timer_list *t)
{
	struct netdev_private *np = from_timer(np, t, timer);
	struct net_device *dev = np->mii_if.dev;
	void __iomem *ioaddr = np->base;
	int next_tick = 10*HZ;

	if (netif_msg_timer(np)) {
		printk(KERN_DEBUG "%s: Media selection timer tick, intr status %4.4x, "
			   "Tx %x Rx %x.\n",
			   dev->name, ioread16(ioaddr + IntrEnable),
			   ioread8(ioaddr + TxStatus), ioread32(ioaddr + RxStatus));
	}
	check_duplex(dev);
	np->timer.expires = jiffies + next_tick;
	add_timer(&np->timer);
}

static void tx_timeout(struct net_device *dev, unsigned int txqueue)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	unsigned long flag;

	netif_stop_queue(dev);
	tasklet_disable_in_atomic(&np->tx_tasklet);
	iowrite16(0, ioaddr + IntrEnable);
	printk(KERN_WARNING "%s: Transmit timed out, TxStatus %2.2x "
		   "TxFrameId %2.2x,"
		   " resetting...\n", dev->name, ioread8(ioaddr + TxStatus),
		   ioread8(ioaddr + TxFrameId));

	{
		int i;
		for (i=0; i<TX_RING_SIZE; i++) {
			printk(KERN_DEBUG "%02x %08llx %08x %08x(%02x) %08x %08x\n", i,
				(unsigned long long)(np->tx_ring_dma + i*sizeof(*np->tx_ring)),
				le32_to_cpu(np->tx_ring[i].next_desc),
				le32_to_cpu(np->tx_ring[i].status),
				(le32_to_cpu(np->tx_ring[i].status) >> 2) & 0xff,
				le32_to_cpu(np->tx_ring[i].frag[0].addr),
				le32_to_cpu(np->tx_ring[i].frag[0].length));
		}
		printk(KERN_DEBUG "TxListPtr=%08x netif_queue_stopped=%d\n",
			ioread32(np->base + TxListPtr),
			netif_queue_stopped(dev));
		printk(KERN_DEBUG "cur_tx=%d(%02x) dirty_tx=%d(%02x)\n",
			np->cur_tx, np->cur_tx % TX_RING_SIZE,
			np->dirty_tx, np->dirty_tx % TX_RING_SIZE);
		printk(KERN_DEBUG "cur_rx=%d dirty_rx=%d\n", np->cur_rx, np->dirty_rx);
		printk(KERN_DEBUG "cur_task=%d\n", np->cur_task);
	}
	spin_lock_irqsave(&np->lock, flag);

	/* Stop and restart the chip's Tx processes . */
	reset_tx(dev);
	spin_unlock_irqrestore(&np->lock, flag);

	dev->if_port = 0;

	netif_trans_update(dev); /* prevent tx timeout */
	dev->stats.tx_errors++;
	if (np->cur_tx - np->dirty_tx < TX_QUEUE_LEN - 4) {
		netif_wake_queue(dev);
	}
	iowrite16(DEFAULT_INTR, ioaddr + IntrEnable);
	tasklet_enable(&np->tx_tasklet);
}


/* Initialize the Rx and Tx rings, along with various 'dev' bits. */
static void init_ring(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	int i;

	np->cur_rx = np->cur_tx = 0;
	np->dirty_rx = np->dirty_tx = 0;
	np->cur_task = 0;

	np->rx_buf_sz = (dev->mtu <= 1520 ? PKT_BUF_SZ : dev->mtu + 16);

	/* Initialize all Rx descriptors. */
	for (i = 0; i < RX_RING_SIZE; i++) {
		np->rx_ring[i].next_desc = cpu_to_le32(np->rx_ring_dma +
			((i+1)%RX_RING_SIZE)*sizeof(*np->rx_ring));
		np->rx_ring[i].status = 0;
		np->rx_ring[i].frag[0].length = 0;
		np->rx_skbuff[i] = NULL;
	}

	/* Fill in the Rx buffers.  Handle allocation failure gracefully. */
	for (i = 0; i < RX_RING_SIZE; i++) {
		struct sk_buff *skb =
			netdev_alloc_skb(dev, np->rx_buf_sz + 2);
		np->rx_skbuff[i] = skb;
		if (skb == NULL)
			break;
		skb_reserve(skb, 2);	/* 16 byte align the IP header. */
		np->rx_ring[i].frag[0].addr = cpu_to_le32(
			dma_map_single(&np->pci_dev->dev, skb->data,
				np->rx_buf_sz, DMA_FROM_DEVICE));
		if (dma_mapping_error(&np->pci_dev->dev,
					np->rx_ring[i].frag[0].addr)) {
			dev_kfree_skb(skb);
			np->rx_skbuff[i] = NULL;
			break;
		}
		np->rx_ring[i].frag[0].length = cpu_to_le32(np->rx_buf_sz | LastFrag);
	}
	np->dirty_rx = (unsigned int)(i - RX_RING_SIZE);

	for (i = 0; i < TX_RING_SIZE; i++) {
		np->tx_skbuff[i] = NULL;
		np->tx_ring[i].status = 0;
	}
}

static void tx_poll(struct tasklet_struct *t)
{
	struct netdev_private *np = from_tasklet(np, t, tx_tasklet);
	unsigned head = np->cur_task % TX_RING_SIZE;
	struct netdev_desc *txdesc =
		&np->tx_ring[(np->cur_tx - 1) % TX_RING_SIZE];

	/* Chain the next pointer */
	for (; np->cur_tx - np->cur_task > 0; np->cur_task++) {
		int entry = np->cur_task % TX_RING_SIZE;
		txdesc = &np->tx_ring[entry];
		if (np->last_tx) {
			np->last_tx->next_desc = cpu_to_le32(np->tx_ring_dma +
				entry*sizeof(struct netdev_desc));
		}
		np->last_tx = txdesc;
	}
	/* Indicate the latest descriptor of tx ring */
	txdesc->status |= cpu_to_le32(DescIntrOnTx);

	if (ioread32 (np->base + TxListPtr) == 0)
		iowrite32 (np->tx_ring_dma + head * sizeof(struct netdev_desc),
			np->base + TxListPtr);
}

static netdev_tx_t
start_tx (struct sk_buff *skb, struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	struct netdev_desc *txdesc;
	unsigned entry;

	/* Calculate the next Tx descriptor entry. */
	entry = np->cur_tx % TX_RING_SIZE;
	np->tx_skbuff[entry] = skb;
	txdesc = &np->tx_ring[entry];

	txdesc->next_desc = 0;
	txdesc->status = cpu_to_le32 ((entry << 2) | DisableAlign);
	txdesc->frag[0].addr = cpu_to_le32(dma_map_single(&np->pci_dev->dev,
				skb->data, skb->len, DMA_TO_DEVICE));
	if (dma_mapping_error(&np->pci_dev->dev,
				txdesc->frag[0].addr))
			goto drop_frame;
	txdesc->frag[0].length = cpu_to_le32 (skb->len | LastFrag);

	/* Increment cur_tx before tasklet_schedule() */
	np->cur_tx++;
	mb();
	/* Schedule a tx_poll() task */
	tasklet_schedule(&np->tx_tasklet);

	/* On some architectures: explicitly flush cache lines here. */
	if (np->cur_tx - np->dirty_tx < TX_QUEUE_LEN - 1 &&
	    !netif_queue_stopped(dev)) {
		/* do nothing */
	} else {
		netif_stop_queue (dev);
	}
	if (netif_msg_tx_queued(np)) {
		printk (KERN_DEBUG
			"%s: Transmit frame #%d queued in slot %d.\n",
			dev->name, np->cur_tx, entry);
	}
	return NETDEV_TX_OK;

drop_frame:
	dev_kfree_skb_any(skb);
	np->tx_skbuff[entry] = NULL;
	dev->stats.tx_dropped++;
	return NETDEV_TX_OK;
}

/* Reset hardware tx and free all of tx buffers */
static int
reset_tx (struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	struct sk_buff *skb;
	int i;

	/* Reset tx logic, TxListPtr will be cleaned */
	iowrite16 (TxDisable, ioaddr + MACCtrl1);
	sundance_reset(dev, (NetworkReset|FIFOReset|DMAReset|TxReset) << 16);

	/* free all tx skbuff */
	for (i = 0; i < TX_RING_SIZE; i++) {
		np->tx_ring[i].next_desc = 0;

		skb = np->tx_skbuff[i];
		if (skb) {
			dma_unmap_single(&np->pci_dev->dev,
				le32_to_cpu(np->tx_ring[i].frag[0].addr),
				skb->len, DMA_TO_DEVICE);
			dev_kfree_skb_any(skb);
			np->tx_skbuff[i] = NULL;
			dev->stats.tx_dropped++;
		}
	}
	np->cur_tx = np->dirty_tx = 0;
	np->cur_task = 0;

	np->last_tx = NULL;
	iowrite8(127, ioaddr + TxDMAPollPeriod);

	iowrite16 (StatsEnable | RxEnable | TxEnable, ioaddr + MACCtrl1);
	return 0;
}

/* The interrupt handler cleans up after the Tx thread,
   and schedule a Rx thread work */
static irqreturn_t intr_handler(int irq, void *dev_instance)
{
	struct net_device *dev = (struct net_device *)dev_instance;
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	int hw_frame_id;
	int tx_cnt;
	int tx_status;
	int handled = 0;
	int i;

	do {
		int intr_status = ioread16(ioaddr + IntrStatus);
		iowrite16(intr_status, ioaddr + IntrStatus);

		if (netif_msg_intr(np))
			printk(KERN_DEBUG "%s: Interrupt, status %4.4x.\n",
				   dev->name, intr_status);

		if (!(intr_status & DEFAULT_INTR))
			break;

		handled = 1;

		if (intr_status & (IntrRxDMADone)) {
			iowrite16(DEFAULT_INTR & ~(IntrRxDone|IntrRxDMADone),
					ioaddr + IntrEnable);
			if (np->budget < 0)
				np->budget = RX_BUDGET;
			tasklet_schedule(&np->rx_tasklet);
		}
		if (intr_status & (IntrTxDone | IntrDrvRqst)) {
			tx_status = ioread16 (ioaddr + TxStatus);
			for (tx_cnt=32; tx_status & 0x80; --tx_cnt) {
				if (netif_msg_tx_done(np))
					printk
					    ("%s: Transmit status is %2.2x.\n",
				     	dev->name, tx_status);
				if (tx_status & 0x1e) {
					if (netif_msg_tx_err(np))
						printk("%s: Transmit error status %4.4x.\n",
							   dev->name, tx_status);
					dev->stats.tx_errors++;
					if (tx_status & 0x10)
						dev->stats.tx_fifo_errors++;
					if (tx_status & 0x08)
						dev->stats.collisions++;
					if (tx_status & 0x04)
						dev->stats.tx_fifo_errors++;
					if (tx_status & 0x02)
						dev->stats.tx_window_errors++;

					/*
					** This reset has been verified on
					** DFE-580TX boards ! phdm@macqel.be.
					*/
					if (tx_status & 0x10) {	/* TxUnderrun */
						/* Restart Tx FIFO and transmitter */
						sundance_reset(dev, (NetworkReset|FIFOReset|TxReset) << 16);
						/* No need to reset the Tx pointer here */
					}
					/* Restart the Tx. Need to make sure tx enabled */
					i = 10;
					do {
						iowrite16(ioread16(ioaddr + MACCtrl1) | TxEnable, ioaddr + MACCtrl1);
						if (ioread16(ioaddr + MACCtrl1) & TxEnabled)
							break;
						mdelay(1);
					} while (--i);
				}
				/* Yup, this is a documentation bug.  It cost me *hours*. */
				iowrite16 (0, ioaddr + TxStatus);
				if (tx_cnt < 0) {
					iowrite32(5000, ioaddr + DownCounter);
					break;
				}
				tx_status = ioread16 (ioaddr + TxStatus);
			}
			hw_frame_id = (tx_status >> 8) & 0xff;
		} else 	{
			hw_frame_id = ioread8(ioaddr + TxFrameId);
		}

		if (np->pci_dev->revision >= 0x14) {
			spin_lock(&np->lock);
			for (; np->cur_tx - np->dirty_tx > 0; np->dirty_tx++) {
				int entry = np->dirty_tx % TX_RING_SIZE;
				struct sk_buff *skb;
				int sw_frame_id;
				sw_frame_id = (le32_to_cpu(
					np->tx_ring[entry].status) >> 2) & 0xff;
				if (sw_frame_id == hw_frame_id &&
					!(le32_to_cpu(np->tx_ring[entry].status)
					& 0x00010000))
						break;
				if (sw_frame_id == (hw_frame_id + 1) %
					TX_RING_SIZE)
						break;
				skb = np->tx_skbuff[entry];
				/* Free the original skb. */
				dma_unmap_single(&np->pci_dev->dev,
					le32_to_cpu(np->tx_ring[entry].frag[0].addr),
					skb->len, DMA_TO_DEVICE);
				dev_consume_skb_irq(np->tx_skbuff[entry]);
				np->tx_skbuff[entry] = NULL;
				np->tx_ring[entry].frag[0].addr = 0;
				np->tx_ring[entry].frag[0].length = 0;
			}
			spin_unlock(&np->lock);
		} else {
			spin_lock(&np->lock);
			for (; np->cur_tx - np->dirty_tx > 0; np->dirty_tx++) {
				int entry = np->dirty_tx % TX_RING_SIZE;
				struct sk_buff *skb;
				if (!(le32_to_cpu(np->tx_ring[entry].status)
							& 0x00010000))
					break;
				skb = np->tx_skbuff[entry];
				/* Free the original skb. */
				dma_unmap_single(&np->pci_dev->dev,
					le32_to_cpu(np->tx_ring[entry].frag[0].addr),
					skb->len, DMA_TO_DEVICE);
				dev_consume_skb_irq(np->tx_skbuff[entry]);
				np->tx_skbuff[entry] = NULL;
				np->tx_ring[entry].frag[0].addr = 0;
				np->tx_ring[entry].frag[0].length = 0;
			}
			spin_unlock(&np->lock);
		}

		if (netif_queue_stopped(dev) &&
			np->cur_tx - np->dirty_tx < TX_QUEUE_LEN - 4) {
			/* The ring is no longer full, clear busy flag. */
			netif_wake_queue (dev);
		}
		/* Abnormal error summary/uncommon events handlers. */
		if (intr_status & (IntrPCIErr | LinkChange | StatsMax))
			netdev_error(dev, intr_status);
	} while (0);
	if (netif_msg_intr(np))
		printk(KERN_DEBUG "%s: exiting interrupt, status=%#4.4x.\n",
			   dev->name, ioread16(ioaddr + IntrStatus));
	return IRQ_RETVAL(handled);
}

static void rx_poll(struct tasklet_struct *t)
{
	struct netdev_private *np = from_tasklet(np, t, rx_tasklet);
	struct net_device *dev = np->ndev;
	int entry = np->cur_rx % RX_RING_SIZE;
	int boguscnt = np->budget;
	void __iomem *ioaddr = np->base;
	int received = 0;

	/* If EOP is set on the next entry, it's a new packet. Send it up. */
	while (1) {
		struct netdev_desc *desc = &(np->rx_ring[entry]);
		u32 frame_status = le32_to_cpu(desc->status);
		int pkt_len;

		if (--boguscnt < 0) {
			goto not_done;
		}
		if (!(frame_status & DescOwn))
			break;
		pkt_len = frame_status & 0x1fff;	/* Chip omits the CRC. */
		if (netif_msg_rx_status(np))
			printk(KERN_DEBUG "  netdev_rx() status was %8.8x.\n",
				   frame_status);
		if (frame_status & 0x001f4000) {
			/* There was a error. */
			if (netif_msg_rx_err(np))
				printk(KERN_DEBUG "  netdev_rx() Rx error was %8.8x.\n",
					   frame_status);
			dev->stats.rx_errors++;
			if (frame_status & 0x00100000)
				dev->stats.rx_length_errors++;
			if (frame_status & 0x00010000)
				dev->stats.rx_fifo_errors++;
			if (frame_status & 0x00060000)
				dev->stats.rx_frame_errors++;
			if (frame_status & 0x00080000)
				dev->stats.rx_crc_errors++;
			if (frame_status & 0x00100000) {
				printk(KERN_WARNING "%s: Oversized Ethernet frame,"
					   " status %8.8x.\n",
					   dev->name, frame_status);
			}
		} else {
			struct sk_buff *skb;
#ifndef final_version
			if (netif_msg_rx_status(np))
				printk(KERN_DEBUG "  netdev_rx() normal Rx pkt length %d"
					   ", bogus_cnt %d.\n",
					   pkt_len, boguscnt);
#endif
			/* Check if the packet is long enough to accept without copying
			   to a minimally-sized skbuff. */
			if (pkt_len < rx_copybreak &&
			    (skb = netdev_alloc_skb(dev, pkt_len + 2)) != NULL) {
				skb_reserve(skb, 2);	/* 16 byte align the IP header */
				dma_sync_single_for_cpu(&np->pci_dev->dev,
						le32_to_cpu(desc->frag[0].addr),
						np->rx_buf_sz, DMA_FROM_DEVICE);
				skb_copy_to_linear_data(skb, np->rx_skbuff[entry]->data, pkt_len);
				dma_sync_single_for_device(&np->pci_dev->dev,
						le32_to_cpu(desc->frag[0].addr),
						np->rx_buf_sz, DMA_FROM_DEVICE);
				skb_put(skb, pkt_len);
			} else {
				dma_unmap_single(&np->pci_dev->dev,
					le32_to_cpu(desc->frag[0].addr),
					np->rx_buf_sz, DMA_FROM_DEVICE);
				skb_put(skb = np->rx_skbuff[entry], pkt_len);
				np->rx_skbuff[entry] = NULL;
			}
			skb->protocol = eth_type_trans(skb, dev);
			/* Note: checksum -> skb->ip_summed = CHECKSUM_UNNECESSARY; */
			netif_rx(skb);
		}
		entry = (entry + 1) % RX_RING_SIZE;
		received++;
	}
	np->cur_rx = entry;
	refill_rx (dev);
	np->budget -= received;
	iowrite16(DEFAULT_INTR, ioaddr + IntrEnable);
	return;

not_done:
	np->cur_rx = entry;
	refill_rx (dev);
	if (!received)
		received = 1;
	np->budget -= received;
	if (np->budget <= 0)
		np->budget = RX_BUDGET;
	tasklet_schedule(&np->rx_tasklet);
}

static void refill_rx (struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	int entry;
	int cnt = 0;

	/* Refill the Rx ring buffers. */
	for (;(np->cur_rx - np->dirty_rx + RX_RING_SIZE) % RX_RING_SIZE > 0;
		np->dirty_rx = (np->dirty_rx + 1) % RX_RING_SIZE) {
		struct sk_buff *skb;
		entry = np->dirty_rx % RX_RING_SIZE;
		if (np->rx_skbuff[entry] == NULL) {
			skb = netdev_alloc_skb(dev, np->rx_buf_sz + 2);
			np->rx_skbuff[entry] = skb;
			if (skb == NULL)
				break;		/* Better luck next round. */
			skb_reserve(skb, 2);	/* Align IP on 16 byte boundaries */
			np->rx_ring[entry].frag[0].addr = cpu_to_le32(
				dma_map_single(&np->pci_dev->dev, skb->data,
					np->rx_buf_sz, DMA_FROM_DEVICE));
			if (dma_mapping_error(&np->pci_dev->dev,
				    np->rx_ring[entry].frag[0].addr)) {
			    dev_kfree_skb_irq(skb);
			    np->rx_skbuff[entry] = NULL;
			    break;
			}
		}
		/* Perhaps we need not reset this field. */
		np->rx_ring[entry].frag[0].length =
			cpu_to_le32(np->rx_buf_sz | LastFrag);
		np->rx_ring[entry].status = 0;
		cnt++;
	}
}
static void netdev_error(struct net_device *dev, int intr_status)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	u16 mii_ctl, mii_advertise, mii_lpa;
	int speed;

	if (intr_status & LinkChange) {
		if (mdio_wait_link(dev, 10) == 0) {
			printk(KERN_INFO "%s: Link up\n", dev->name);
			if (np->an_enable) {
				mii_advertise = mdio_read(dev, np->phys[0],
							   MII_ADVERTISE);
				mii_lpa = mdio_read(dev, np->phys[0], MII_LPA);
				mii_advertise &= mii_lpa;
				printk(KERN_INFO "%s: Link changed: ",
					dev->name);
				if (mii_advertise & ADVERTISE_100FULL) {
					np->speed = 100;
					printk("100Mbps, full duplex\n");
				} else if (mii_advertise & ADVERTISE_100HALF) {
					np->speed = 100;
					printk("100Mbps, half duplex\n");
				} else if (mii_advertise & ADVERTISE_10FULL) {
					np->speed = 10;
					printk("10Mbps, full duplex\n");
				} else if (mii_advertise & ADVERTISE_10HALF) {
					np->speed = 10;
					printk("10Mbps, half duplex\n");
				} else
					printk("\n");

			} else {
				mii_ctl = mdio_read(dev, np->phys[0], MII_BMCR);
				speed = (mii_ctl & BMCR_SPEED100) ? 100 : 10;
				np->speed = speed;
				printk(KERN_INFO "%s: Link changed: %dMbps ,",
					dev->name, speed);
				printk("%s duplex.\n",
					(mii_ctl & BMCR_FULLDPLX) ?
						"full" : "half");
			}
			check_duplex(dev);
			if (np->flowctrl && np->mii_if.full_duplex) {
				iowrite16(ioread16(ioaddr + MulticastFilter1+2) | 0x0200,
					ioaddr + MulticastFilter1+2);
				iowrite16(ioread16(ioaddr + MACCtrl0) | EnbFlowCtrl,
					ioaddr + MACCtrl0);
			}
			netif_carrier_on(dev);
		} else {
			printk(KERN_INFO "%s: Link down\n", dev->name);
			netif_carrier_off(dev);
		}
	}
	if (intr_status & StatsMax) {
		get_stats(dev);
	}
	if (intr_status & IntrPCIErr) {
		printk(KERN_ERR "%s: Something Wicked happened! %4.4x.\n",
			   dev->name, intr_status);
		/* We must do a global reset of DMA to continue. */
	}
}

static struct net_device_stats *get_stats(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	unsigned long flags;
	u8 late_coll, single_coll, mult_coll;

	spin_lock_irqsave(&np->statlock, flags);
	/* The chip only need report frame silently dropped. */
	dev->stats.rx_missed_errors	+= ioread8(ioaddr + RxMissed);
	dev->stats.tx_packets += ioread16(ioaddr + TxFramesOK);
	dev->stats.rx_packets += ioread16(ioaddr + RxFramesOK);
	dev->stats.tx_carrier_errors += ioread8(ioaddr + StatsCarrierError);

	mult_coll = ioread8(ioaddr + StatsMultiColl);
	np->xstats.tx_multiple_collisions += mult_coll;
	single_coll = ioread8(ioaddr + StatsOneColl);
	np->xstats.tx_single_collisions += single_coll;
	late_coll = ioread8(ioaddr + StatsLateColl);
	np->xstats.tx_late_collisions += late_coll;
	dev->stats.collisions += mult_coll
		+ single_coll
		+ late_coll;

	np->xstats.tx_deferred += ioread8(ioaddr + StatsTxDefer);
	np->xstats.tx_deferred_excessive += ioread8(ioaddr + StatsTxXSDefer);
	np->xstats.tx_aborted += ioread8(ioaddr + StatsTxAbort);
	np->xstats.tx_bcasts += ioread8(ioaddr + StatsBcastTx);
	np->xstats.rx_bcasts += ioread8(ioaddr + StatsBcastRx);
	np->xstats.tx_mcasts += ioread8(ioaddr + StatsMcastTx);
	np->xstats.rx_mcasts += ioread8(ioaddr + StatsMcastRx);

	dev->stats.tx_bytes += ioread16(ioaddr + TxOctetsLow);
	dev->stats.tx_bytes += ioread16(ioaddr + TxOctetsHigh) << 16;
	dev->stats.rx_bytes += ioread16(ioaddr + RxOctetsLow);
	dev->stats.rx_bytes += ioread16(ioaddr + RxOctetsHigh) << 16;

	spin_unlock_irqrestore(&np->statlock, flags);

	return &dev->stats;
}

static void set_rx_mode(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	u16 mc_filter[4];			/* Multicast hash filter */
	u32 rx_mode;
	int i;

	if (dev->flags & IFF_PROMISC) {			/* Set promiscuous. */
		memset(mc_filter, 0xff, sizeof(mc_filter));
		rx_mode = AcceptBroadcast | AcceptMulticast | AcceptAll | AcceptMyPhys;
	} else if ((netdev_mc_count(dev) > multicast_filter_limit) ||
		   (dev->flags & IFF_ALLMULTI)) {
		/* Too many to match, or accept all multicasts. */
		memset(mc_filter, 0xff, sizeof(mc_filter));
		rx_mode = AcceptBroadcast | AcceptMulticast | AcceptMyPhys;
	} else if (!netdev_mc_empty(dev)) {
		struct netdev_hw_addr *ha;
		int bit;
		int index;
		int crc;
		memset (mc_filter, 0, sizeof (mc_filter));
		netdev_for_each_mc_addr(ha, dev) {
			crc = ether_crc_le(ETH_ALEN, ha->addr);
			for (index=0, bit=0; bit < 6; bit++, crc <<= 1)
				if (crc & 0x80000000) index |= 1 << bit;
			mc_filter[index/16] |= (1 << (index % 16));
		}
		rx_mode = AcceptBroadcast | AcceptMultiHash | AcceptMyPhys;
	} else {
		iowrite8(AcceptBroadcast | AcceptMyPhys, ioaddr + RxMode);
		return;
	}
	if (np->mii_if.full_duplex && np->flowctrl)
		mc_filter[3] |= 0x0200;

	for (i = 0; i < 4; i++)
		iowrite16(mc_filter[i], ioaddr + MulticastFilter0 + i*2);
	iowrite8(rx_mode, ioaddr + RxMode);
}

static int __set_mac_addr(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	u16 addr16;

	addr16 = (dev->dev_addr[0] | (dev->dev_addr[1] << 8));
	iowrite16(addr16, np->base + StationAddr);
	addr16 = (dev->dev_addr[2] | (dev->dev_addr[3] << 8));
	iowrite16(addr16, np->base + StationAddr+2);
	addr16 = (dev->dev_addr[4] | (dev->dev_addr[5] << 8));
	iowrite16(addr16, np->base + StationAddr+4);
	return 0;
}

/* Invoked with rtnl_lock held */
static int sundance_set_mac_addr(struct net_device *dev, void *data)
{
	const struct sockaddr *addr = data;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;
	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	__set_mac_addr(dev);

	return 0;
}

static const struct {
	const char name[ETH_GSTRING_LEN];
} sundance_stats[] = {
	{ "tx_multiple_collisions" },
	{ "tx_single_collisions" },
	{ "tx_late_collisions" },
	{ "tx_deferred" },
	{ "tx_deferred_excessive" },
	{ "tx_aborted" },
	{ "tx_bcasts" },
	{ "rx_bcasts" },
	{ "tx_mcasts" },
	{ "rx_mcasts" },
};

static int check_if_running(struct net_device *dev)
{
	if (!netif_running(dev))
		return -EINVAL;
	return 0;
}

static void get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct netdev_private *np = netdev_priv(dev);
	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	strlcpy(info->bus_info, pci_name(np->pci_dev), sizeof(info->bus_info));
}

static int get_link_ksettings(struct net_device *dev,
			      struct ethtool_link_ksettings *cmd)
{
	struct netdev_private *np = netdev_priv(dev);
	spin_lock_irq(&np->lock);
	mii_ethtool_get_link_ksettings(&np->mii_if, cmd);
	spin_unlock_irq(&np->lock);
	return 0;
}

static int set_link_ksettings(struct net_device *dev,
			      const struct ethtool_link_ksettings *cmd)
{
	struct netdev_private *np = netdev_priv(dev);
	int res;
	spin_lock_irq(&np->lock);
	res = mii_ethtool_set_link_ksettings(&np->mii_if, cmd);
	spin_unlock_irq(&np->lock);
	return res;
}

static int nway_reset(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	return mii_nway_restart(&np->mii_if);
}

static u32 get_link(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	return mii_link_ok(&np->mii_if);
}

static u32 get_msglevel(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	return np->msg_enable;
}

static void set_msglevel(struct net_device *dev, u32 val)
{
	struct netdev_private *np = netdev_priv(dev);
	np->msg_enable = val;
}

static void get_strings(struct net_device *dev, u32 stringset,
		u8 *data)
{
	if (stringset == ETH_SS_STATS)
		memcpy(data, sundance_stats, sizeof(sundance_stats));
}

static int get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return ARRAY_SIZE(sundance_stats);
	default:
		return -EOPNOTSUPP;
	}
}

static void get_ethtool_stats(struct net_device *dev,
		struct ethtool_stats *stats, u64 *data)
{
	struct netdev_private *np = netdev_priv(dev);
	int i = 0;

	get_stats(dev);
	data[i++] = np->xstats.tx_multiple_collisions;
	data[i++] = np->xstats.tx_single_collisions;
	data[i++] = np->xstats.tx_late_collisions;
	data[i++] = np->xstats.tx_deferred;
	data[i++] = np->xstats.tx_deferred_excessive;
	data[i++] = np->xstats.tx_aborted;
	data[i++] = np->xstats.tx_bcasts;
	data[i++] = np->xstats.rx_bcasts;
	data[i++] = np->xstats.tx_mcasts;
	data[i++] = np->xstats.rx_mcasts;
}

#ifdef CONFIG_PM

static void sundance_get_wol(struct net_device *dev,
		struct ethtool_wolinfo *wol)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	u8 wol_bits;

	wol->wolopts = 0;

	wol->supported = (WAKE_PHY | WAKE_MAGIC);
	if (!np->wol_enabled)
		return;

	wol_bits = ioread8(ioaddr + WakeEvent);
	if (wol_bits & MagicPktEnable)
		wol->wolopts |= WAKE_MAGIC;
	if (wol_bits & LinkEventEnable)
		wol->wolopts |= WAKE_PHY;
}

static int sundance_set_wol(struct net_device *dev,
	struct ethtool_wolinfo *wol)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	u8 wol_bits;

	if (!device_can_wakeup(&np->pci_dev->dev))
		return -EOPNOTSUPP;

	np->wol_enabled = !!(wol->wolopts);
	wol_bits = ioread8(ioaddr + WakeEvent);
	wol_bits &= ~(WakePktEnable | MagicPktEnable |
			LinkEventEnable | WolEnable);

	if (np->wol_enabled) {
		if (wol->wolopts & WAKE_MAGIC)
			wol_bits |= (MagicPktEnable | WolEnable);
		if (wol->wolopts & WAKE_PHY)
			wol_bits |= (LinkEventEnable | WolEnable);
	}
	iowrite8(wol_bits, ioaddr + WakeEvent);

	device_set_wakeup_enable(&np->pci_dev->dev, np->wol_enabled);

	return 0;
}
#else
#define sundance_get_wol NULL
#define sundance_set_wol NULL
#endif /* CONFIG_PM */

static const struct ethtool_ops ethtool_ops = {
	.begin = check_if_running,
	.get_drvinfo = get_drvinfo,
	.nway_reset = nway_reset,
	.get_link = get_link,
	.get_wol = sundance_get_wol,
	.set_wol = sundance_set_wol,
	.get_msglevel = get_msglevel,
	.set_msglevel = set_msglevel,
	.get_strings = get_strings,
	.get_sset_count = get_sset_count,
	.get_ethtool_stats = get_ethtool_stats,
	.get_link_ksettings = get_link_ksettings,
	.set_link_ksettings = set_link_ksettings,
};

static int netdev_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct netdev_private *np = netdev_priv(dev);
	int rc;

	if (!netif_running(dev))
		return -EINVAL;

	spin_lock_irq(&np->lock);
	rc = generic_mii_ioctl(&np->mii_if, if_mii(rq), cmd, NULL);
	spin_unlock_irq(&np->lock);

	return rc;
}

static int netdev_close(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;
	struct sk_buff *skb;
	int i;

	/* Wait and kill tasklet */
	tasklet_kill(&np->rx_tasklet);
	tasklet_kill(&np->tx_tasklet);
	np->cur_tx = 0;
	np->dirty_tx = 0;
	np->cur_task = 0;
	np->last_tx = NULL;

	netif_stop_queue(dev);

	if (netif_msg_ifdown(np)) {
		printk(KERN_DEBUG "%s: Shutting down ethercard, status was Tx %2.2x "
			   "Rx %4.4x Int %2.2x.\n",
			   dev->name, ioread8(ioaddr + TxStatus),
			   ioread32(ioaddr + RxStatus), ioread16(ioaddr + IntrStatus));
		printk(KERN_DEBUG "%s: Queue pointers were Tx %d / %d,  Rx %d / %d.\n",
			   dev->name, np->cur_tx, np->dirty_tx, np->cur_rx, np->dirty_rx);
	}

	/* Disable interrupts by clearing the interrupt mask. */
	iowrite16(0x0000, ioaddr + IntrEnable);

	/* Disable Rx and Tx DMA for safely release resource */
	iowrite32(0x500, ioaddr + DMACtrl);

	/* Stop the chip's Tx and Rx processes. */
	iowrite16(TxDisable | RxDisable | StatsDisable, ioaddr + MACCtrl1);

    	for (i = 2000; i > 0; i--) {
 		if ((ioread32(ioaddr + DMACtrl) & 0xc000) == 0)
			break;
		mdelay(1);
    	}

    	iowrite16(GlobalReset | DMAReset | FIFOReset | NetworkReset,
			ioaddr + ASIC_HI_WORD(ASICCtrl));

    	for (i = 2000; i > 0; i--) {
		if ((ioread16(ioaddr + ASIC_HI_WORD(ASICCtrl)) & ResetBusy) == 0)
			break;
		mdelay(1);
    	}

#ifdef __i386__
	if (netif_msg_hw(np)) {
		printk(KERN_DEBUG "  Tx ring at %8.8x:\n",
			   (int)(np->tx_ring_dma));
		for (i = 0; i < TX_RING_SIZE; i++)
			printk(KERN_DEBUG " #%d desc. %4.4x %8.8x %8.8x.\n",
				   i, np->tx_ring[i].status, np->tx_ring[i].frag[0].addr,
				   np->tx_ring[i].frag[0].length);
		printk(KERN_DEBUG "  Rx ring %8.8x:\n",
			   (int)(np->rx_ring_dma));
		for (i = 0; i < /*RX_RING_SIZE*/4 ; i++) {
			printk(KERN_DEBUG " #%d desc. %4.4x %4.4x %8.8x\n",
				   i, np->rx_ring[i].status, np->rx_ring[i].frag[0].addr,
				   np->rx_ring[i].frag[0].length);
		}
	}
#endif /* __i386__ debugging only */

	free_irq(np->pci_dev->irq, dev);

	del_timer_sync(&np->timer);

	/* Free all the skbuffs in the Rx queue. */
	for (i = 0; i < RX_RING_SIZE; i++) {
		np->rx_ring[i].status = 0;
		skb = np->rx_skbuff[i];
		if (skb) {
			dma_unmap_single(&np->pci_dev->dev,
				le32_to_cpu(np->rx_ring[i].frag[0].addr),
				np->rx_buf_sz, DMA_FROM_DEVICE);
			dev_kfree_skb(skb);
			np->rx_skbuff[i] = NULL;
		}
		np->rx_ring[i].frag[0].addr = cpu_to_le32(0xBADF00D0); /* poison */
	}
	for (i = 0; i < TX_RING_SIZE; i++) {
		np->tx_ring[i].next_desc = 0;
		skb = np->tx_skbuff[i];
		if (skb) {
			dma_unmap_single(&np->pci_dev->dev,
				le32_to_cpu(np->tx_ring[i].frag[0].addr),
				skb->len, DMA_TO_DEVICE);
			dev_kfree_skb(skb);
			np->tx_skbuff[i] = NULL;
		}
	}

	return 0;
}

static void sundance_remove1(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);

	if (dev) {
	    struct netdev_private *np = netdev_priv(dev);
	    unregister_netdev(dev);
	    dma_free_coherent(&pdev->dev, RX_TOTAL_SIZE,
		    np->rx_ring, np->rx_ring_dma);
	    dma_free_coherent(&pdev->dev, TX_TOTAL_SIZE,
		    np->tx_ring, np->tx_ring_dma);
	    pci_iounmap(pdev, np->base);
	    pci_release_regions(pdev);
	    free_netdev(dev);
	}
}

static int __maybe_unused sundance_suspend(struct device *dev_d)
{
	struct net_device *dev = dev_get_drvdata(dev_d);
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base;

	if (!netif_running(dev))
		return 0;

	netdev_close(dev);
	netif_device_detach(dev);

	if (np->wol_enabled) {
		iowrite8(AcceptBroadcast | AcceptMyPhys, ioaddr + RxMode);
		iowrite16(RxEnable, ioaddr + MACCtrl1);
	}

	device_set_wakeup_enable(dev_d, np->wol_enabled);

	return 0;
}

static int __maybe_unused sundance_resume(struct device *dev_d)
{
	struct net_device *dev = dev_get_drvdata(dev_d);
	int err = 0;

	if (!netif_running(dev))
		return 0;

	err = netdev_open(dev);
	if (err) {
		printk(KERN_ERR "%s: Can't resume interface!\n",
				dev->name);
		goto out;
	}

	netif_device_attach(dev);

out:
	return err;
}

static SIMPLE_DEV_PM_OPS(sundance_pm_ops, sundance_suspend, sundance_resume);

static struct pci_driver sundance_driver = {
	.name		= DRV_NAME,
	.id_table	= sundance_pci_tbl,
	.probe		= sundance_probe1,
	.remove		= sundance_remove1,
	.driver.pm	= &sundance_pm_ops,
};

module_pci_driver(sundance_driver);
