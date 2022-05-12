/* winbond-840.c: A Linux PCI network adapter device driver. */
/*
	Written 1998-2001 by Donald Becker.

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
	http://www.scyld.com/network/drivers.html

	Do not remove the copyright information.
	Do not change the version information unless an improvement has been made.
	Merely removing my name, as Compex has done in the past, does not count
	as an improvement.

	Changelog:
	* ported to 2.4
		???
	* spin lock update, memory barriers, new style dma mappings
		limit each tx buffer to < 1024 bytes
		remove DescIntr from Rx descriptors (that's an Tx flag)
		remove next pointer from Tx descriptors
		synchronize tx_q_bytes
		software reset in tx_timeout
			Copyright (C) 2000 Manfred Spraul
	* further cleanups
		power management.
		support for big endian descriptors
			Copyright (C) 2001 Manfred Spraul
  	* ethtool support (jgarzik)
	* Replace some MII-related magic numbers with constants (jgarzik)

	TODO:
	* enable pci_power_off
	* Wake-On-LAN
*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define DRV_NAME	"winbond-840"

/* Automatically extracted configuration info:
probe-func: winbond840_probe
config-in: tristate 'Winbond W89c840 Ethernet support' CONFIG_WINBOND_840

c-help-name: Winbond W89c840 PCI Ethernet support
c-help-symbol: CONFIG_WINBOND_840
c-help: This driver is for the Winbond W89c840 chip.  It also works with
c-help: the TX9882 chip on the Compex RL100-ATX board.
c-help: More specific information and updates are available from
c-help: http://www.scyld.com/network/drivers.html
*/

/* The user-configurable values.
   These may be modified when a driver module is loaded.*/

static int debug = 1;			/* 1 normal messages, 0 quiet .. 7 verbose. */
static int max_interrupt_work = 20;
/* Maximum number of multicast addresses to filter (vs. Rx-all-multicast).
   The '840 uses a 64 element hash table based on the Ethernet CRC.  */
static int multicast_filter_limit = 32;

/* Set the copy breakpoint for the copy-only-tiny-frames scheme.
   Setting to > 1518 effectively disables this feature. */
static int rx_copybreak;

/* Used to pass the media type, etc.
   Both 'options[]' and 'full_duplex[]' should exist for driver
   interoperability.
   The media type is usually passed in 'options[]'.
*/
#define MAX_UNITS 8		/* More are supported, limit only on options */
static int options[MAX_UNITS] = {-1, -1, -1, -1, -1, -1, -1, -1};
static int full_duplex[MAX_UNITS] = {-1, -1, -1, -1, -1, -1, -1, -1};

/* Operational parameters that are set at compile time. */

/* Keep the ring sizes a power of two for compile efficiency.
   The compiler will convert <unsigned>'%'<2^N> into a bit mask.
   Making the Tx ring too large decreases the effectiveness of channel
   bonding and packet priority.
   There are no ill effects from too-large receive rings. */
#define TX_QUEUE_LEN	10		/* Limit ring entries actually used.  */
#define TX_QUEUE_LEN_RESTART	5

#define TX_BUFLIMIT	(1024-128)

/* The presumed FIFO size for working around the Tx-FIFO-overflow bug.
   To avoid overflowing we don't queue again until we have room for a
   full-size packet.
 */
#define TX_FIFO_SIZE (2048)
#define TX_BUG_FIFO_LIMIT (TX_FIFO_SIZE-1514-16)


/* Operational parameters that usually are not changed. */
/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT  (2*HZ)

/* Include files, designed to support most kernel versions 2.0.0 and later. */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/rtnetlink.h>
#include <linux/crc32.h>
#include <linux/bitops.h>
#include <linux/uaccess.h>
#include <asm/processor.h>		/* Processor type for cache alignment. */
#include <asm/io.h>
#include <asm/irq.h>

#include "tulip.h"

#undef PKT_BUF_SZ			/* tulip.h also defines this */
#define PKT_BUF_SZ		1536	/* Size of each temporary Rx buffer.*/

MODULE_AUTHOR("Donald Becker <becker@scyld.com>");
MODULE_DESCRIPTION("Winbond W89c840 Ethernet driver");
MODULE_LICENSE("GPL");

module_param(max_interrupt_work, int, 0);
module_param(debug, int, 0);
module_param(rx_copybreak, int, 0);
module_param(multicast_filter_limit, int, 0);
module_param_array(options, int, NULL, 0);
module_param_array(full_duplex, int, NULL, 0);
MODULE_PARM_DESC(max_interrupt_work, "winbond-840 maximum events handled per interrupt");
MODULE_PARM_DESC(debug, "winbond-840 debug level (0-6)");
MODULE_PARM_DESC(rx_copybreak, "winbond-840 copy breakpoint for copy-only-tiny-frames");
MODULE_PARM_DESC(multicast_filter_limit, "winbond-840 maximum number of filtered multicast addresses");
MODULE_PARM_DESC(options, "winbond-840: Bits 0-3: media type, bit 17: full duplex");
MODULE_PARM_DESC(full_duplex, "winbond-840 full duplex setting(s) (1)");

/*
				Theory of Operation

I. Board Compatibility

This driver is for the Winbond w89c840 chip.

II. Board-specific settings

None.

III. Driver operation

This chip is very similar to the Digital 21*4* "Tulip" family.  The first
twelve registers and the descriptor format are nearly identical.  Read a
Tulip manual for operational details.

A significant difference is that the multicast filter and station address are
stored in registers rather than loaded through a pseudo-transmit packet.

Unlike the Tulip, transmit buffers are limited to 1KB.  To transmit a
full-sized packet we must use both data buffers in a descriptor.  Thus the
driver uses ring mode where descriptors are implicitly sequential in memory,
rather than using the second descriptor address as a chain pointer to
subsequent descriptors.

IV. Notes

If you are going to almost clone a Tulip, why not go all the way and avoid
the need for a new driver?

IVb. References

http://www.scyld.com/expert/100mbps.html
http://www.scyld.com/expert/NWay.html
http://www.winbond.com.tw/

IVc. Errata

A horrible bug exists in the transmit FIFO.  Apparently the chip doesn't
correctly detect a full FIFO, and queuing more than 2048 bytes may result in
silent data corruption.

Test with 'ping -s 10000' on a fast computer.

*/



/*
  PCI probe table.
*/
enum chip_capability_flags {
	CanHaveMII=1, HasBrokenTx=2, AlwaysFDX=4, FDXOnNoMII=8,
};

static const struct pci_device_id w840_pci_tbl[] = {
	{ 0x1050, 0x0840, PCI_ANY_ID, 0x8153,     0, 0, 0 },
	{ 0x1050, 0x0840, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 1 },
	{ 0x11f6, 0x2011, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 2 },
	{ }
};
MODULE_DEVICE_TABLE(pci, w840_pci_tbl);

enum {
	netdev_res_size		= 128,	/* size of PCI BAR resource */
};

struct pci_id_info {
        const char *name;
        int drv_flags;		/* Driver use, intended as capability flags. */
};

static const struct pci_id_info pci_id_tbl[] = {
	{ 				/* Sometime a Level-One switch card. */
	  "Winbond W89c840",	CanHaveMII | HasBrokenTx | FDXOnNoMII},
	{ "Winbond W89c840",	CanHaveMII | HasBrokenTx},
	{ "Compex RL100-ATX",	CanHaveMII | HasBrokenTx},
	{ }	/* terminate list. */
};

/* This driver was written to use PCI memory space, however some x86 systems
   work only with I/O space accesses. See CONFIG_TULIP_MMIO in .config
*/

/* Offsets to the Command and Status Registers, "CSRs".
   While similar to the Tulip, these registers are longword aligned.
   Note: It's not useful to define symbolic names for every register bit in
   the device.  The name can only partially document the semantics and make
   the driver longer and more difficult to read.
*/
enum w840_offsets {
	PCIBusCfg=0x00, TxStartDemand=0x04, RxStartDemand=0x08,
	RxRingPtr=0x0C, TxRingPtr=0x10,
	IntrStatus=0x14, NetworkConfig=0x18, IntrEnable=0x1C,
	RxMissed=0x20, EECtrl=0x24, MIICtrl=0x24, BootRom=0x28, GPTimer=0x2C,
	CurRxDescAddr=0x30, CurRxBufAddr=0x34,			/* Debug use */
	MulticastFilter0=0x38, MulticastFilter1=0x3C, StationAddr=0x40,
	CurTxDescAddr=0x4C, CurTxBufAddr=0x50,
};

/* Bits in the NetworkConfig register. */
enum rx_mode_bits {
	AcceptErr=0x80,
	RxAcceptBroadcast=0x20, AcceptMulticast=0x10,
	RxAcceptAllPhys=0x08, AcceptMyPhys=0x02,
};

enum mii_reg_bits {
	MDIO_ShiftClk=0x10000, MDIO_DataIn=0x80000, MDIO_DataOut=0x20000,
	MDIO_EnbOutput=0x40000, MDIO_EnbIn = 0x00000,
};

/* The Tulip Rx and Tx buffer descriptors. */
struct w840_rx_desc {
	s32 status;
	s32 length;
	u32 buffer1;
	u32 buffer2;
};

struct w840_tx_desc {
	s32 status;
	s32 length;
	u32 buffer1, buffer2;
};

#define MII_CNT		1 /* winbond only supports one MII */
struct netdev_private {
	struct w840_rx_desc *rx_ring;
	dma_addr_t	rx_addr[RX_RING_SIZE];
	struct w840_tx_desc *tx_ring;
	dma_addr_t	tx_addr[TX_RING_SIZE];
	dma_addr_t ring_dma_addr;
	/* The addresses of receive-in-place skbuffs. */
	struct sk_buff* rx_skbuff[RX_RING_SIZE];
	/* The saved address of a sent-in-place packet/buffer, for later free(). */
	struct sk_buff* tx_skbuff[TX_RING_SIZE];
	struct net_device_stats stats;
	struct timer_list timer;	/* Media monitoring timer. */
	/* Frequently used values: keep some adjacent for cache effect. */
	spinlock_t lock;
	int chip_id, drv_flags;
	struct pci_dev *pci_dev;
	int csr6;
	struct w840_rx_desc *rx_head_desc;
	unsigned int cur_rx, dirty_rx;		/* Producer/consumer ring indices */
	unsigned int rx_buf_sz;				/* Based on MTU+slack. */
	unsigned int cur_tx, dirty_tx;
	unsigned int tx_q_bytes;
	unsigned int tx_full;				/* The Tx queue is full. */
	/* MII transceiver section. */
	int mii_cnt;						/* MII device addresses. */
	unsigned char phys[MII_CNT];		/* MII device addresses, but only the first is used */
	u32 mii;
	struct mii_if_info mii_if;
	void __iomem *base_addr;
};

static int  eeprom_read(void __iomem *ioaddr, int location);
static int  mdio_read(struct net_device *dev, int phy_id, int location);
static void mdio_write(struct net_device *dev, int phy_id, int location, int value);
static int  netdev_open(struct net_device *dev);
static int  update_link(struct net_device *dev);
static void netdev_timer(struct timer_list *t);
static void init_rxtx_rings(struct net_device *dev);
static void free_rxtx_rings(struct netdev_private *np);
static void init_registers(struct net_device *dev);
static void tx_timeout(struct net_device *dev, unsigned int txqueue);
static int alloc_ringdesc(struct net_device *dev);
static void free_ringdesc(struct netdev_private *np);
static netdev_tx_t start_tx(struct sk_buff *skb, struct net_device *dev);
static irqreturn_t intr_handler(int irq, void *dev_instance);
static void netdev_error(struct net_device *dev, int intr_status);
static int  netdev_rx(struct net_device *dev);
static u32 __set_rx_mode(struct net_device *dev);
static void set_rx_mode(struct net_device *dev);
static struct net_device_stats *get_stats(struct net_device *dev);
static int netdev_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
static const struct ethtool_ops netdev_ethtool_ops;
static int  netdev_close(struct net_device *dev);

static const struct net_device_ops netdev_ops = {
	.ndo_open		= netdev_open,
	.ndo_stop		= netdev_close,
	.ndo_start_xmit		= start_tx,
	.ndo_get_stats		= get_stats,
	.ndo_set_rx_mode	= set_rx_mode,
	.ndo_do_ioctl		= netdev_ioctl,
	.ndo_tx_timeout		= tx_timeout,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
};

static int w840_probe1(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct net_device *dev;
	struct netdev_private *np;
	static int find_cnt;
	int chip_idx = ent->driver_data;
	int irq;
	int i, option = find_cnt < MAX_UNITS ? options[find_cnt] : 0;
	void __iomem *ioaddr;

	i = pci_enable_device(pdev);
	if (i) return i;

	pci_set_master(pdev);

	irq = pdev->irq;

	if (dma_set_mask(&pdev->dev, DMA_BIT_MASK(32))) {
		pr_warn("Device %s disabled due to DMA limitations\n",
			pci_name(pdev));
		return -EIO;
	}
	dev = alloc_etherdev(sizeof(*np));
	if (!dev)
		return -ENOMEM;
	SET_NETDEV_DEV(dev, &pdev->dev);

	if (pci_request_regions(pdev, DRV_NAME))
		goto err_out_netdev;

	ioaddr = pci_iomap(pdev, TULIP_BAR, netdev_res_size);
	if (!ioaddr)
		goto err_out_free_res;

	for (i = 0; i < 3; i++)
		((__le16 *)dev->dev_addr)[i] = cpu_to_le16(eeprom_read(ioaddr, i));

	/* Reset the chip to erase previous misconfiguration.
	   No hold time required! */
	iowrite32(0x00000001, ioaddr + PCIBusCfg);

	np = netdev_priv(dev);
	np->pci_dev = pdev;
	np->chip_id = chip_idx;
	np->drv_flags = pci_id_tbl[chip_idx].drv_flags;
	spin_lock_init(&np->lock);
	np->mii_if.dev = dev;
	np->mii_if.mdio_read = mdio_read;
	np->mii_if.mdio_write = mdio_write;
	np->base_addr = ioaddr;

	pci_set_drvdata(pdev, dev);

	if (dev->mem_start)
		option = dev->mem_start;

	/* The lower four bits are the media type. */
	if (option > 0) {
		if (option & 0x200)
			np->mii_if.full_duplex = 1;
		if (option & 15)
			dev_info(&dev->dev,
				 "ignoring user supplied media type %d",
				 option & 15);
	}
	if (find_cnt < MAX_UNITS  &&  full_duplex[find_cnt] > 0)
		np->mii_if.full_duplex = 1;

	if (np->mii_if.full_duplex)
		np->mii_if.force_media = 1;

	/* The chip-specific entries in the device structure. */
	dev->netdev_ops = &netdev_ops;
	dev->ethtool_ops = &netdev_ethtool_ops;
	dev->watchdog_timeo = TX_TIMEOUT;

	i = register_netdev(dev);
	if (i)
		goto err_out_cleardev;

	dev_info(&dev->dev, "%s at %p, %pM, IRQ %d\n",
		 pci_id_tbl[chip_idx].name, ioaddr, dev->dev_addr, irq);

	if (np->drv_flags & CanHaveMII) {
		int phy, phy_idx = 0;
		for (phy = 1; phy < 32 && phy_idx < MII_CNT; phy++) {
			int mii_status = mdio_read(dev, phy, MII_BMSR);
			if (mii_status != 0xffff  &&  mii_status != 0x0000) {
				np->phys[phy_idx++] = phy;
				np->mii_if.advertising = mdio_read(dev, phy, MII_ADVERTISE);
				np->mii = (mdio_read(dev, phy, MII_PHYSID1) << 16)+
						mdio_read(dev, phy, MII_PHYSID2);
				dev_info(&dev->dev,
					 "MII PHY %08xh found at address %d, status 0x%04x advertising %04x\n",
					 np->mii, phy, mii_status,
					 np->mii_if.advertising);
			}
		}
		np->mii_cnt = phy_idx;
		np->mii_if.phy_id = np->phys[0];
		if (phy_idx == 0) {
			dev_warn(&dev->dev,
				 "MII PHY not found -- this device may not operate correctly\n");
		}
	}

	find_cnt++;
	return 0;

err_out_cleardev:
	pci_iounmap(pdev, ioaddr);
err_out_free_res:
	pci_release_regions(pdev);
err_out_netdev:
	free_netdev (dev);
	return -ENODEV;
}


/* Read the EEPROM and MII Management Data I/O (MDIO) interfaces.  These are
   often serial bit streams generated by the host processor.
   The example below is for the common 93c46 EEPROM, 64 16 bit words. */

/* Delay between EEPROM clock transitions.
   No extra delay is needed with 33Mhz PCI, but future 66Mhz access may need
   a delay.  Note that pre-2.0.34 kernels had a cache-alignment bug that
   made udelay() unreliable.
   The old method of using an ISA access as a delay, __SLOW_DOWN_IO__, is
   deprecated.
*/
#define eeprom_delay(ee_addr)	ioread32(ee_addr)

enum EEPROM_Ctrl_Bits {
	EE_ShiftClk=0x02, EE_Write0=0x801, EE_Write1=0x805,
	EE_ChipSelect=0x801, EE_DataIn=0x08,
};

/* The EEPROM commands include the alway-set leading bit. */
enum EEPROM_Cmds {
	EE_WriteCmd=(5 << 6), EE_ReadCmd=(6 << 6), EE_EraseCmd=(7 << 6),
};

static int eeprom_read(void __iomem *addr, int location)
{
	int i;
	int retval = 0;
	void __iomem *ee_addr = addr + EECtrl;
	int read_cmd = location | EE_ReadCmd;
	iowrite32(EE_ChipSelect, ee_addr);

	/* Shift the read command bits out. */
	for (i = 10; i >= 0; i--) {
		short dataval = (read_cmd & (1 << i)) ? EE_Write1 : EE_Write0;
		iowrite32(dataval, ee_addr);
		eeprom_delay(ee_addr);
		iowrite32(dataval | EE_ShiftClk, ee_addr);
		eeprom_delay(ee_addr);
	}
	iowrite32(EE_ChipSelect, ee_addr);
	eeprom_delay(ee_addr);

	for (i = 16; i > 0; i--) {
		iowrite32(EE_ChipSelect | EE_ShiftClk, ee_addr);
		eeprom_delay(ee_addr);
		retval = (retval << 1) | ((ioread32(ee_addr) & EE_DataIn) ? 1 : 0);
		iowrite32(EE_ChipSelect, ee_addr);
		eeprom_delay(ee_addr);
	}

	/* Terminate the EEPROM access. */
	iowrite32(0, ee_addr);
	return retval;
}

/*  MII transceiver control section.
	Read and write the MII registers using software-generated serial
	MDIO protocol.  See the MII specifications or DP83840A data sheet
	for details.

	The maximum data clock rate is 2.5 Mhz.  The minimum timing is usually
	met by back-to-back 33Mhz PCI cycles. */
#define mdio_delay(mdio_addr) ioread32(mdio_addr)

/* Set iff a MII transceiver on any interface requires mdio preamble.
   This only set with older transceivers, so the extra
   code size of a per-interface flag is not worthwhile. */
static char mii_preamble_required = 1;

#define MDIO_WRITE0 (MDIO_EnbOutput)
#define MDIO_WRITE1 (MDIO_DataOut | MDIO_EnbOutput)

/* Generate the preamble required for initial synchronization and
   a few older transceivers. */
static void mdio_sync(void __iomem *mdio_addr)
{
	int bits = 32;

	/* Establish sync by sending at least 32 logic ones. */
	while (--bits >= 0) {
		iowrite32(MDIO_WRITE1, mdio_addr);
		mdio_delay(mdio_addr);
		iowrite32(MDIO_WRITE1 | MDIO_ShiftClk, mdio_addr);
		mdio_delay(mdio_addr);
	}
}

static int mdio_read(struct net_device *dev, int phy_id, int location)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *mdio_addr = np->base_addr + MIICtrl;
	int mii_cmd = (0xf6 << 10) | (phy_id << 5) | location;
	int i, retval = 0;

	if (mii_preamble_required)
		mdio_sync(mdio_addr);

	/* Shift the read command bits out. */
	for (i = 15; i >= 0; i--) {
		int dataval = (mii_cmd & (1 << i)) ? MDIO_WRITE1 : MDIO_WRITE0;

		iowrite32(dataval, mdio_addr);
		mdio_delay(mdio_addr);
		iowrite32(dataval | MDIO_ShiftClk, mdio_addr);
		mdio_delay(mdio_addr);
	}
	/* Read the two transition, 16 data, and wire-idle bits. */
	for (i = 20; i > 0; i--) {
		iowrite32(MDIO_EnbIn, mdio_addr);
		mdio_delay(mdio_addr);
		retval = (retval << 1) | ((ioread32(mdio_addr) & MDIO_DataIn) ? 1 : 0);
		iowrite32(MDIO_EnbIn | MDIO_ShiftClk, mdio_addr);
		mdio_delay(mdio_addr);
	}
	return (retval>>1) & 0xffff;
}

static void mdio_write(struct net_device *dev, int phy_id, int location, int value)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *mdio_addr = np->base_addr + MIICtrl;
	int mii_cmd = (0x5002 << 16) | (phy_id << 23) | (location<<18) | value;
	int i;

	if (location == 4  &&  phy_id == np->phys[0])
		np->mii_if.advertising = value;

	if (mii_preamble_required)
		mdio_sync(mdio_addr);

	/* Shift the command bits out. */
	for (i = 31; i >= 0; i--) {
		int dataval = (mii_cmd & (1 << i)) ? MDIO_WRITE1 : MDIO_WRITE0;

		iowrite32(dataval, mdio_addr);
		mdio_delay(mdio_addr);
		iowrite32(dataval | MDIO_ShiftClk, mdio_addr);
		mdio_delay(mdio_addr);
	}
	/* Clear out extra bits. */
	for (i = 2; i > 0; i--) {
		iowrite32(MDIO_EnbIn, mdio_addr);
		mdio_delay(mdio_addr);
		iowrite32(MDIO_EnbIn | MDIO_ShiftClk, mdio_addr);
		mdio_delay(mdio_addr);
	}
}


static int netdev_open(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;
	const int irq = np->pci_dev->irq;
	int i;

	iowrite32(0x00000001, ioaddr + PCIBusCfg);		/* Reset */

	netif_device_detach(dev);
	i = request_irq(irq, intr_handler, IRQF_SHARED, dev->name, dev);
	if (i)
		goto out_err;

	if (debug > 1)
		netdev_dbg(dev, "%s() irq %d\n", __func__, irq);

	i = alloc_ringdesc(dev);
	if (i)
		goto out_err;

	spin_lock_irq(&np->lock);
	netif_device_attach(dev);
	init_registers(dev);
	spin_unlock_irq(&np->lock);

	netif_start_queue(dev);
	if (debug > 2)
		netdev_dbg(dev, "Done %s()\n", __func__);

	/* Set the timer to check for link beat. */
	timer_setup(&np->timer, netdev_timer, 0);
	np->timer.expires = jiffies + 1*HZ;
	add_timer(&np->timer);
	return 0;
out_err:
	netif_device_attach(dev);
	return i;
}

#define MII_DAVICOM_DM9101	0x0181b800

static int update_link(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	int duplex, fasteth, result, mii_reg;

	/* BSMR */
	mii_reg = mdio_read(dev, np->phys[0], MII_BMSR);

	if (mii_reg == 0xffff)
		return np->csr6;
	/* reread: the link status bit is sticky */
	mii_reg = mdio_read(dev, np->phys[0], MII_BMSR);
	if (!(mii_reg & 0x4)) {
		if (netif_carrier_ok(dev)) {
			if (debug)
				dev_info(&dev->dev,
					 "MII #%d reports no link. Disabling watchdog\n",
					 np->phys[0]);
			netif_carrier_off(dev);
		}
		return np->csr6;
	}
	if (!netif_carrier_ok(dev)) {
		if (debug)
			dev_info(&dev->dev,
				 "MII #%d link is back. Enabling watchdog\n",
				 np->phys[0]);
		netif_carrier_on(dev);
	}

	if ((np->mii & ~0xf) == MII_DAVICOM_DM9101) {
		/* If the link partner doesn't support autonegotiation
		 * the MII detects it's abilities with the "parallel detection".
		 * Some MIIs update the LPA register to the result of the parallel
		 * detection, some don't.
		 * The Davicom PHY [at least 0181b800] doesn't.
		 * Instead bit 9 and 13 of the BMCR are updated to the result
		 * of the negotiation..
		 */
		mii_reg = mdio_read(dev, np->phys[0], MII_BMCR);
		duplex = mii_reg & BMCR_FULLDPLX;
		fasteth = mii_reg & BMCR_SPEED100;
	} else {
		int negotiated;
		mii_reg	= mdio_read(dev, np->phys[0], MII_LPA);
		negotiated = mii_reg & np->mii_if.advertising;

		duplex = (negotiated & LPA_100FULL) || ((negotiated & 0x02C0) == LPA_10FULL);
		fasteth = negotiated & 0x380;
	}
	duplex |= np->mii_if.force_media;
	/* remove fastether and fullduplex */
	result = np->csr6 & ~0x20000200;
	if (duplex)
		result |= 0x200;
	if (fasteth)
		result |= 0x20000000;
	if (result != np->csr6 && debug)
		dev_info(&dev->dev,
			 "Setting %dMBit-%s-duplex based on MII#%d\n",
			 fasteth ? 100 : 10, duplex ? "full" : "half",
			 np->phys[0]);
	return result;
}

#define RXTX_TIMEOUT	2000
static inline void update_csr6(struct net_device *dev, int new)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;
	int limit = RXTX_TIMEOUT;

	if (!netif_device_present(dev))
		new = 0;
	if (new==np->csr6)
		return;
	/* stop both Tx and Rx processes */
	iowrite32(np->csr6 & ~0x2002, ioaddr + NetworkConfig);
	/* wait until they have really stopped */
	for (;;) {
		int csr5 = ioread32(ioaddr + IntrStatus);
		int t;

		t = (csr5 >> 17) & 0x07;
		if (t==0||t==1) {
			/* rx stopped */
			t = (csr5 >> 20) & 0x07;
			if (t==0||t==1)
				break;
		}

		limit--;
		if(!limit) {
			dev_info(&dev->dev,
				 "couldn't stop rxtx, IntrStatus %xh\n", csr5);
			break;
		}
		udelay(1);
	}
	np->csr6 = new;
	/* and restart them with the new configuration */
	iowrite32(np->csr6, ioaddr + NetworkConfig);
	if (new & 0x200)
		np->mii_if.full_duplex = 1;
}

static void netdev_timer(struct timer_list *t)
{
	struct netdev_private *np = from_timer(np, t, timer);
	struct net_device *dev = pci_get_drvdata(np->pci_dev);
	void __iomem *ioaddr = np->base_addr;

	if (debug > 2)
		netdev_dbg(dev, "Media selection timer tick, status %08x config %08x\n",
			   ioread32(ioaddr + IntrStatus),
			   ioread32(ioaddr + NetworkConfig));
	spin_lock_irq(&np->lock);
	update_csr6(dev, update_link(dev));
	spin_unlock_irq(&np->lock);
	np->timer.expires = jiffies + 10*HZ;
	add_timer(&np->timer);
}

static void init_rxtx_rings(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	int i;

	np->rx_head_desc = &np->rx_ring[0];
	np->tx_ring = (struct w840_tx_desc*)&np->rx_ring[RX_RING_SIZE];

	/* Initial all Rx descriptors. */
	for (i = 0; i < RX_RING_SIZE; i++) {
		np->rx_ring[i].length = np->rx_buf_sz;
		np->rx_ring[i].status = 0;
		np->rx_skbuff[i] = NULL;
	}
	/* Mark the last entry as wrapping the ring. */
	np->rx_ring[i-1].length |= DescEndRing;

	/* Fill in the Rx buffers.  Handle allocation failure gracefully. */
	for (i = 0; i < RX_RING_SIZE; i++) {
		struct sk_buff *skb = netdev_alloc_skb(dev, np->rx_buf_sz);
		np->rx_skbuff[i] = skb;
		if (skb == NULL)
			break;
		np->rx_addr[i] = dma_map_single(&np->pci_dev->dev, skb->data,
						np->rx_buf_sz,
						DMA_FROM_DEVICE);

		np->rx_ring[i].buffer1 = np->rx_addr[i];
		np->rx_ring[i].status = DescOwned;
	}

	np->cur_rx = 0;
	np->dirty_rx = (unsigned int)(i - RX_RING_SIZE);

	/* Initialize the Tx descriptors */
	for (i = 0; i < TX_RING_SIZE; i++) {
		np->tx_skbuff[i] = NULL;
		np->tx_ring[i].status = 0;
	}
	np->tx_full = 0;
	np->tx_q_bytes = np->dirty_tx = np->cur_tx = 0;

	iowrite32(np->ring_dma_addr, np->base_addr + RxRingPtr);
	iowrite32(np->ring_dma_addr+sizeof(struct w840_rx_desc)*RX_RING_SIZE,
		np->base_addr + TxRingPtr);

}

static void free_rxtx_rings(struct netdev_private* np)
{
	int i;
	/* Free all the skbuffs in the Rx queue. */
	for (i = 0; i < RX_RING_SIZE; i++) {
		np->rx_ring[i].status = 0;
		if (np->rx_skbuff[i]) {
			dma_unmap_single(&np->pci_dev->dev, np->rx_addr[i],
					 np->rx_skbuff[i]->len,
					 DMA_FROM_DEVICE);
			dev_kfree_skb(np->rx_skbuff[i]);
		}
		np->rx_skbuff[i] = NULL;
	}
	for (i = 0; i < TX_RING_SIZE; i++) {
		if (np->tx_skbuff[i]) {
			dma_unmap_single(&np->pci_dev->dev, np->tx_addr[i],
					 np->tx_skbuff[i]->len, DMA_TO_DEVICE);
			dev_kfree_skb(np->tx_skbuff[i]);
		}
		np->tx_skbuff[i] = NULL;
	}
}

static void init_registers(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;
	int i;

	for (i = 0; i < 6; i++)
		iowrite8(dev->dev_addr[i], ioaddr + StationAddr + i);

	/* Initialize other registers. */
#ifdef __BIG_ENDIAN
	i = (1<<20);	/* Big-endian descriptors */
#else
	i = 0;
#endif
	i |= (0x04<<2);		/* skip length 4 u32 */
	i |= 0x02;		/* give Rx priority */

	/* Configure the PCI bus bursts and FIFO thresholds.
	   486: Set 8 longword cache alignment, 8 longword burst.
	   586: Set 16 longword cache alignment, no burst limit.
	   Cache alignment bits 15:14	     Burst length 13:8
		0000	<not allowed> 		0000 align to cache	0800 8 longwords
		4000	8  longwords		0100 1 longword		1000 16 longwords
		8000	16 longwords		0200 2 longwords	2000 32 longwords
		C000	32  longwords		0400 4 longwords */

#if defined (__i386__) && !defined(MODULE)
	/* When not a module we can work around broken '486 PCI boards. */
	if (boot_cpu_data.x86 <= 4) {
		i |= 0x4800;
		dev_info(&dev->dev,
			 "This is a 386/486 PCI system, setting cache alignment to 8 longwords\n");
	} else {
		i |= 0xE000;
	}
#elif defined(__powerpc__) || defined(__i386__) || defined(__alpha__) || defined(__ia64__) || defined(__x86_64__)
	i |= 0xE000;
#elif defined(CONFIG_SPARC) || defined (CONFIG_PARISC) || defined(CONFIG_ARM)
	i |= 0x4800;
#else
	dev_warn(&dev->dev, "unknown CPU architecture, using default csr0 setting\n");
	i |= 0x4800;
#endif
	iowrite32(i, ioaddr + PCIBusCfg);

	np->csr6 = 0;
	/* 128 byte Tx threshold;
		Transmit on; Receive on; */
	update_csr6(dev, 0x00022002 | update_link(dev) | __set_rx_mode(dev));

	/* Clear and Enable interrupts by setting the interrupt mask. */
	iowrite32(0x1A0F5, ioaddr + IntrStatus);
	iowrite32(0x1A0F5, ioaddr + IntrEnable);

	iowrite32(0, ioaddr + RxStartDemand);
}

static void tx_timeout(struct net_device *dev, unsigned int txqueue)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;
	const int irq = np->pci_dev->irq;

	dev_warn(&dev->dev, "Transmit timed out, status %08x, resetting...\n",
		 ioread32(ioaddr + IntrStatus));

	{
		int i;
		printk(KERN_DEBUG "  Rx ring %p: ", np->rx_ring);
		for (i = 0; i < RX_RING_SIZE; i++)
			printk(KERN_CONT " %08x", (unsigned int)np->rx_ring[i].status);
		printk(KERN_CONT "\n");
		printk(KERN_DEBUG "  Tx ring %p: ", np->tx_ring);
		for (i = 0; i < TX_RING_SIZE; i++)
			printk(KERN_CONT " %08x", np->tx_ring[i].status);
		printk(KERN_CONT "\n");
	}
	printk(KERN_DEBUG "Tx cur %d Tx dirty %d Tx Full %d, q bytes %d\n",
	       np->cur_tx, np->dirty_tx, np->tx_full, np->tx_q_bytes);
	printk(KERN_DEBUG "Tx Descriptor addr %xh\n", ioread32(ioaddr+0x4C));

	disable_irq(irq);
	spin_lock_irq(&np->lock);
	/*
	 * Under high load dirty_tx and the internal tx descriptor pointer
	 * come out of sync, thus perform a software reset and reinitialize
	 * everything.
	 */

	iowrite32(1, np->base_addr+PCIBusCfg);
	udelay(1);

	free_rxtx_rings(np);
	init_rxtx_rings(dev);
	init_registers(dev);
	spin_unlock_irq(&np->lock);
	enable_irq(irq);

	netif_wake_queue(dev);
	netif_trans_update(dev); /* prevent tx timeout */
	np->stats.tx_errors++;
}

/* Initialize the Rx and Tx rings, along with various 'dev' bits. */
static int alloc_ringdesc(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);

	np->rx_buf_sz = (dev->mtu <= 1500 ? PKT_BUF_SZ : dev->mtu + 32);

	np->rx_ring = dma_alloc_coherent(&np->pci_dev->dev,
					 sizeof(struct w840_rx_desc) * RX_RING_SIZE +
					 sizeof(struct w840_tx_desc) * TX_RING_SIZE,
					 &np->ring_dma_addr, GFP_KERNEL);
	if(!np->rx_ring)
		return -ENOMEM;
	init_rxtx_rings(dev);
	return 0;
}

static void free_ringdesc(struct netdev_private *np)
{
	dma_free_coherent(&np->pci_dev->dev,
			  sizeof(struct w840_rx_desc) * RX_RING_SIZE +
			  sizeof(struct w840_tx_desc) * TX_RING_SIZE,
			  np->rx_ring, np->ring_dma_addr);

}

static netdev_tx_t start_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	unsigned entry;

	/* Caution: the write order is important here, set the field
	   with the "ownership" bits last. */

	/* Calculate the next Tx descriptor entry. */
	entry = np->cur_tx % TX_RING_SIZE;

	np->tx_addr[entry] = dma_map_single(&np->pci_dev->dev, skb->data,
					    skb->len, DMA_TO_DEVICE);
	np->tx_skbuff[entry] = skb;

	np->tx_ring[entry].buffer1 = np->tx_addr[entry];
	if (skb->len < TX_BUFLIMIT) {
		np->tx_ring[entry].length = DescWholePkt | skb->len;
	} else {
		int len = skb->len - TX_BUFLIMIT;

		np->tx_ring[entry].buffer2 = np->tx_addr[entry]+TX_BUFLIMIT;
		np->tx_ring[entry].length = DescWholePkt | (len << 11) | TX_BUFLIMIT;
	}
	if(entry == TX_RING_SIZE-1)
		np->tx_ring[entry].length |= DescEndRing;

	/* Now acquire the irq spinlock.
	 * The difficult race is the ordering between
	 * increasing np->cur_tx and setting DescOwned:
	 * - if np->cur_tx is increased first the interrupt
	 *   handler could consider the packet as transmitted
	 *   since DescOwned is cleared.
	 * - If DescOwned is set first the NIC could report the
	 *   packet as sent, but the interrupt handler would ignore it
	 *   since the np->cur_tx was not yet increased.
	 */
	spin_lock_irq(&np->lock);
	np->cur_tx++;

	wmb(); /* flush length, buffer1, buffer2 */
	np->tx_ring[entry].status = DescOwned;
	wmb(); /* flush status and kick the hardware */
	iowrite32(0, np->base_addr + TxStartDemand);
	np->tx_q_bytes += skb->len;
	/* Work around horrible bug in the chip by marking the queue as full
	   when we do not have FIFO room for a maximum sized packet. */
	if (np->cur_tx - np->dirty_tx > TX_QUEUE_LEN ||
		((np->drv_flags & HasBrokenTx) && np->tx_q_bytes > TX_BUG_FIFO_LIMIT)) {
		netif_stop_queue(dev);
		wmb();
		np->tx_full = 1;
	}
	spin_unlock_irq(&np->lock);

	if (debug > 4) {
		netdev_dbg(dev, "Transmit frame #%d queued in slot %d\n",
			   np->cur_tx, entry);
	}
	return NETDEV_TX_OK;
}

static void netdev_tx_done(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	for (; np->cur_tx - np->dirty_tx > 0; np->dirty_tx++) {
		int entry = np->dirty_tx % TX_RING_SIZE;
		int tx_status = np->tx_ring[entry].status;

		if (tx_status < 0)
			break;
		if (tx_status & 0x8000) { 	/* There was an error, log it. */
#ifndef final_version
			if (debug > 1)
				netdev_dbg(dev, "Transmit error, Tx status %08x\n",
					   tx_status);
#endif
			np->stats.tx_errors++;
			if (tx_status & 0x0104) np->stats.tx_aborted_errors++;
			if (tx_status & 0x0C80) np->stats.tx_carrier_errors++;
			if (tx_status & 0x0200) np->stats.tx_window_errors++;
			if (tx_status & 0x0002) np->stats.tx_fifo_errors++;
			if ((tx_status & 0x0080) && np->mii_if.full_duplex == 0)
				np->stats.tx_heartbeat_errors++;
		} else {
#ifndef final_version
			if (debug > 3)
				netdev_dbg(dev, "Transmit slot %d ok, Tx status %08x\n",
					   entry, tx_status);
#endif
			np->stats.tx_bytes += np->tx_skbuff[entry]->len;
			np->stats.collisions += (tx_status >> 3) & 15;
			np->stats.tx_packets++;
		}
		/* Free the original skb. */
		dma_unmap_single(&np->pci_dev->dev, np->tx_addr[entry],
				 np->tx_skbuff[entry]->len, DMA_TO_DEVICE);
		np->tx_q_bytes -= np->tx_skbuff[entry]->len;
		dev_kfree_skb_irq(np->tx_skbuff[entry]);
		np->tx_skbuff[entry] = NULL;
	}
	if (np->tx_full &&
		np->cur_tx - np->dirty_tx < TX_QUEUE_LEN_RESTART &&
		np->tx_q_bytes < TX_BUG_FIFO_LIMIT) {
		/* The ring is no longer full, clear tbusy. */
		np->tx_full = 0;
		wmb();
		netif_wake_queue(dev);
	}
}

/* The interrupt handler does all of the Rx thread work and cleans up
   after the Tx thread. */
static irqreturn_t intr_handler(int irq, void *dev_instance)
{
	struct net_device *dev = (struct net_device *)dev_instance;
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;
	int work_limit = max_interrupt_work;
	int handled = 0;

	if (!netif_device_present(dev))
		return IRQ_NONE;
	do {
		u32 intr_status = ioread32(ioaddr + IntrStatus);

		/* Acknowledge all of the current interrupt sources ASAP. */
		iowrite32(intr_status & 0x001ffff, ioaddr + IntrStatus);

		if (debug > 4)
			netdev_dbg(dev, "Interrupt, status %04x\n", intr_status);

		if ((intr_status & (NormalIntr|AbnormalIntr)) == 0)
			break;

		handled = 1;

		if (intr_status & (RxIntr | RxNoBuf))
			netdev_rx(dev);
		if (intr_status & RxNoBuf)
			iowrite32(0, ioaddr + RxStartDemand);

		if (intr_status & (TxNoBuf | TxIntr) &&
			np->cur_tx != np->dirty_tx) {
			spin_lock(&np->lock);
			netdev_tx_done(dev);
			spin_unlock(&np->lock);
		}

		/* Abnormal error summary/uncommon events handlers. */
		if (intr_status & (AbnormalIntr | TxFIFOUnderflow | SystemError |
						   TimerInt | TxDied))
			netdev_error(dev, intr_status);

		if (--work_limit < 0) {
			dev_warn(&dev->dev,
				 "Too much work at interrupt, status=0x%04x\n",
				 intr_status);
			/* Set the timer to re-enable the other interrupts after
			   10*82usec ticks. */
			spin_lock(&np->lock);
			if (netif_device_present(dev)) {
				iowrite32(AbnormalIntr | TimerInt, ioaddr + IntrEnable);
				iowrite32(10, ioaddr + GPTimer);
			}
			spin_unlock(&np->lock);
			break;
		}
	} while (1);

	if (debug > 3)
		netdev_dbg(dev, "exiting interrupt, status=%#4.4x\n",
			   ioread32(ioaddr + IntrStatus));
	return IRQ_RETVAL(handled);
}

/* This routine is logically part of the interrupt handler, but separated
   for clarity and better register allocation. */
static int netdev_rx(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	int entry = np->cur_rx % RX_RING_SIZE;
	int work_limit = np->dirty_rx + RX_RING_SIZE - np->cur_rx;

	if (debug > 4) {
		netdev_dbg(dev, " In netdev_rx(), entry %d status %04x\n",
			   entry, np->rx_ring[entry].status);
	}

	/* If EOP is set on the next entry, it's a new packet. Send it up. */
	while (--work_limit >= 0) {
		struct w840_rx_desc *desc = np->rx_head_desc;
		s32 status = desc->status;

		if (debug > 4)
			netdev_dbg(dev, "  netdev_rx() status was %08x\n",
				   status);
		if (status < 0)
			break;
		if ((status & 0x38008300) != 0x0300) {
			if ((status & 0x38000300) != 0x0300) {
				/* Ingore earlier buffers. */
				if ((status & 0xffff) != 0x7fff) {
					dev_warn(&dev->dev,
						 "Oversized Ethernet frame spanned multiple buffers, entry %#x status %04x!\n",
						 np->cur_rx, status);
					np->stats.rx_length_errors++;
				}
			} else if (status & 0x8000) {
				/* There was a fatal error. */
				if (debug > 2)
					netdev_dbg(dev, "Receive error, Rx status %08x\n",
						   status);
				np->stats.rx_errors++; /* end of a packet.*/
				if (status & 0x0890) np->stats.rx_length_errors++;
				if (status & 0x004C) np->stats.rx_frame_errors++;
				if (status & 0x0002) np->stats.rx_crc_errors++;
			}
		} else {
			struct sk_buff *skb;
			/* Omit the four octet CRC from the length. */
			int pkt_len = ((status >> 16) & 0x7ff) - 4;

#ifndef final_version
			if (debug > 4)
				netdev_dbg(dev, "  netdev_rx() normal Rx pkt length %d status %x\n",
					   pkt_len, status);
#endif
			/* Check if the packet is long enough to accept without copying
			   to a minimally-sized skbuff. */
			if (pkt_len < rx_copybreak &&
			    (skb = netdev_alloc_skb(dev, pkt_len + 2)) != NULL) {
				skb_reserve(skb, 2);	/* 16 byte align the IP header */
				dma_sync_single_for_cpu(&np->pci_dev->dev,
							np->rx_addr[entry],
							np->rx_skbuff[entry]->len,
							DMA_FROM_DEVICE);
				skb_copy_to_linear_data(skb, np->rx_skbuff[entry]->data, pkt_len);
				skb_put(skb, pkt_len);
				dma_sync_single_for_device(&np->pci_dev->dev,
							   np->rx_addr[entry],
							   np->rx_skbuff[entry]->len,
							   DMA_FROM_DEVICE);
			} else {
				dma_unmap_single(&np->pci_dev->dev,
						 np->rx_addr[entry],
						 np->rx_skbuff[entry]->len,
						 DMA_FROM_DEVICE);
				skb_put(skb = np->rx_skbuff[entry], pkt_len);
				np->rx_skbuff[entry] = NULL;
			}
#ifndef final_version				/* Remove after testing. */
			/* You will want this info for the initial debug. */
			if (debug > 5)
				netdev_dbg(dev, "  Rx data %pM %pM %02x%02x %pI4\n",
					   &skb->data[0], &skb->data[6],
					   skb->data[12], skb->data[13],
					   &skb->data[14]);
#endif
			skb->protocol = eth_type_trans(skb, dev);
			netif_rx(skb);
			np->stats.rx_packets++;
			np->stats.rx_bytes += pkt_len;
		}
		entry = (++np->cur_rx) % RX_RING_SIZE;
		np->rx_head_desc = &np->rx_ring[entry];
	}

	/* Refill the Rx ring buffers. */
	for (; np->cur_rx - np->dirty_rx > 0; np->dirty_rx++) {
		struct sk_buff *skb;
		entry = np->dirty_rx % RX_RING_SIZE;
		if (np->rx_skbuff[entry] == NULL) {
			skb = netdev_alloc_skb(dev, np->rx_buf_sz);
			np->rx_skbuff[entry] = skb;
			if (skb == NULL)
				break;			/* Better luck next round. */
			np->rx_addr[entry] = dma_map_single(&np->pci_dev->dev,
							    skb->data,
							    np->rx_buf_sz,
							    DMA_FROM_DEVICE);
			np->rx_ring[entry].buffer1 = np->rx_addr[entry];
		}
		wmb();
		np->rx_ring[entry].status = DescOwned;
	}

	return 0;
}

static void netdev_error(struct net_device *dev, int intr_status)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;

	if (debug > 2)
		netdev_dbg(dev, "Abnormal event, %08x\n", intr_status);
	if (intr_status == 0xffffffff)
		return;
	spin_lock(&np->lock);
	if (intr_status & TxFIFOUnderflow) {
		int new;
		/* Bump up the Tx threshold */
#if 0
		/* This causes lots of dropped packets,
		 * and under high load even tx_timeouts
		 */
		new = np->csr6 + 0x4000;
#else
		new = (np->csr6 >> 14)&0x7f;
		if (new < 64)
			new *= 2;
		 else
		 	new = 127; /* load full packet before starting */
		new = (np->csr6 & ~(0x7F << 14)) | (new<<14);
#endif
		netdev_dbg(dev, "Tx underflow, new csr6 %08x\n", new);
		update_csr6(dev, new);
	}
	if (intr_status & RxDied) {		/* Missed a Rx frame. */
		np->stats.rx_errors++;
	}
	if (intr_status & TimerInt) {
		/* Re-enable other interrupts. */
		if (netif_device_present(dev))
			iowrite32(0x1A0F5, ioaddr + IntrEnable);
	}
	np->stats.rx_missed_errors += ioread32(ioaddr + RxMissed) & 0xffff;
	iowrite32(0, ioaddr + RxStartDemand);
	spin_unlock(&np->lock);
}

static struct net_device_stats *get_stats(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;

	/* The chip only need report frame silently dropped. */
	spin_lock_irq(&np->lock);
	if (netif_running(dev) && netif_device_present(dev))
		np->stats.rx_missed_errors += ioread32(ioaddr + RxMissed) & 0xffff;
	spin_unlock_irq(&np->lock);

	return &np->stats;
}


static u32 __set_rx_mode(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;
	u32 mc_filter[2];			/* Multicast hash filter */
	u32 rx_mode;

	if (dev->flags & IFF_PROMISC) {			/* Set promiscuous. */
		memset(mc_filter, 0xff, sizeof(mc_filter));
		rx_mode = RxAcceptBroadcast | AcceptMulticast | RxAcceptAllPhys
			| AcceptMyPhys;
	} else if ((netdev_mc_count(dev) > multicast_filter_limit) ||
		   (dev->flags & IFF_ALLMULTI)) {
		/* Too many to match, or accept all multicasts. */
		memset(mc_filter, 0xff, sizeof(mc_filter));
		rx_mode = RxAcceptBroadcast | AcceptMulticast | AcceptMyPhys;
	} else {
		struct netdev_hw_addr *ha;

		memset(mc_filter, 0, sizeof(mc_filter));
		netdev_for_each_mc_addr(ha, dev) {
			int filbit;

			filbit = (ether_crc(ETH_ALEN, ha->addr) >> 26) ^ 0x3F;
			filbit &= 0x3f;
			mc_filter[filbit >> 5] |= 1 << (filbit & 31);
		}
		rx_mode = RxAcceptBroadcast | AcceptMulticast | AcceptMyPhys;
	}
	iowrite32(mc_filter[0], ioaddr + MulticastFilter0);
	iowrite32(mc_filter[1], ioaddr + MulticastFilter1);
	return rx_mode;
}

static void set_rx_mode(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	u32 rx_mode = __set_rx_mode(dev);
	spin_lock_irq(&np->lock);
	update_csr6(dev, (np->csr6 & ~0x00F8) | rx_mode);
	spin_unlock_irq(&np->lock);
}

static void netdev_get_drvinfo (struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct netdev_private *np = netdev_priv(dev);

	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	strlcpy(info->bus_info, pci_name(np->pci_dev), sizeof(info->bus_info));
}

static int netdev_get_link_ksettings(struct net_device *dev,
				     struct ethtool_link_ksettings *cmd)
{
	struct netdev_private *np = netdev_priv(dev);

	spin_lock_irq(&np->lock);
	mii_ethtool_get_link_ksettings(&np->mii_if, cmd);
	spin_unlock_irq(&np->lock);

	return 0;
}

static int netdev_set_link_ksettings(struct net_device *dev,
				     const struct ethtool_link_ksettings *cmd)
{
	struct netdev_private *np = netdev_priv(dev);
	int rc;

	spin_lock_irq(&np->lock);
	rc = mii_ethtool_set_link_ksettings(&np->mii_if, cmd);
	spin_unlock_irq(&np->lock);

	return rc;
}

static int netdev_nway_reset(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	return mii_nway_restart(&np->mii_if);
}

static u32 netdev_get_link(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	return mii_link_ok(&np->mii_if);
}

static u32 netdev_get_msglevel(struct net_device *dev)
{
	return debug;
}

static void netdev_set_msglevel(struct net_device *dev, u32 value)
{
	debug = value;
}

static const struct ethtool_ops netdev_ethtool_ops = {
	.get_drvinfo		= netdev_get_drvinfo,
	.nway_reset		= netdev_nway_reset,
	.get_link		= netdev_get_link,
	.get_msglevel		= netdev_get_msglevel,
	.set_msglevel		= netdev_set_msglevel,
	.get_link_ksettings	= netdev_get_link_ksettings,
	.set_link_ksettings	= netdev_set_link_ksettings,
};

static int netdev_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct mii_ioctl_data *data = if_mii(rq);
	struct netdev_private *np = netdev_priv(dev);

	switch(cmd) {
	case SIOCGMIIPHY:		/* Get address of MII PHY in use. */
		data->phy_id = ((struct netdev_private *)netdev_priv(dev))->phys[0] & 0x1f;
		fallthrough;

	case SIOCGMIIREG:		/* Read MII PHY register. */
		spin_lock_irq(&np->lock);
		data->val_out = mdio_read(dev, data->phy_id & 0x1f, data->reg_num & 0x1f);
		spin_unlock_irq(&np->lock);
		return 0;

	case SIOCSMIIREG:		/* Write MII PHY register. */
		spin_lock_irq(&np->lock);
		mdio_write(dev, data->phy_id & 0x1f, data->reg_num & 0x1f, data->val_in);
		spin_unlock_irq(&np->lock);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static int netdev_close(struct net_device *dev)
{
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;

	netif_stop_queue(dev);

	if (debug > 1) {
		netdev_dbg(dev, "Shutting down ethercard, status was %08x Config %08x\n",
			   ioread32(ioaddr + IntrStatus),
			   ioread32(ioaddr + NetworkConfig));
		netdev_dbg(dev, "Queue pointers were Tx %d / %d,  Rx %d / %d\n",
			   np->cur_tx, np->dirty_tx,
			   np->cur_rx, np->dirty_rx);
	}

 	/* Stop the chip's Tx and Rx processes. */
	spin_lock_irq(&np->lock);
	netif_device_detach(dev);
	update_csr6(dev, 0);
	iowrite32(0x0000, ioaddr + IntrEnable);
	spin_unlock_irq(&np->lock);

	free_irq(np->pci_dev->irq, dev);
	wmb();
	netif_device_attach(dev);

	if (ioread32(ioaddr + NetworkConfig) != 0xffffffff)
		np->stats.rx_missed_errors += ioread32(ioaddr + RxMissed) & 0xffff;

#ifdef __i386__
	if (debug > 2) {
		int i;

		printk(KERN_DEBUG"  Tx ring at %p:\n", np->tx_ring);
		for (i = 0; i < TX_RING_SIZE; i++)
			printk(KERN_DEBUG " #%d desc. %04x %04x %08x\n",
			       i, np->tx_ring[i].length,
			       np->tx_ring[i].status, np->tx_ring[i].buffer1);
		printk(KERN_DEBUG "  Rx ring %p:\n", np->rx_ring);
		for (i = 0; i < RX_RING_SIZE; i++) {
			printk(KERN_DEBUG " #%d desc. %04x %04x %08x\n",
			       i, np->rx_ring[i].length,
			       np->rx_ring[i].status, np->rx_ring[i].buffer1);
		}
	}
#endif /* __i386__ debugging only */

	del_timer_sync(&np->timer);

	free_rxtx_rings(np);
	free_ringdesc(np);

	return 0;
}

static void w840_remove1(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);

	if (dev) {
		struct netdev_private *np = netdev_priv(dev);
		unregister_netdev(dev);
		pci_release_regions(pdev);
		pci_iounmap(pdev, np->base_addr);
		free_netdev(dev);
	}
}

/*
 * suspend/resume synchronization:
 * - open, close, do_ioctl:
 * 	rtnl_lock, & netif_device_detach after the rtnl_unlock.
 * - get_stats:
 * 	spin_lock_irq(np->lock), doesn't touch hw if not present
 * - start_xmit:
 * 	synchronize_irq + netif_tx_disable;
 * - tx_timeout:
 * 	netif_device_detach + netif_tx_disable;
 * - set_multicast_list
 * 	netif_device_detach + netif_tx_disable;
 * - interrupt handler
 * 	doesn't touch hw if not present, synchronize_irq waits for
 * 	running instances of the interrupt handler.
 *
 * Disabling hw requires clearing csr6 & IntrEnable.
 * update_csr6 & all function that write IntrEnable check netif_device_present
 * before settings any bits.
 *
 * Detach must occur under spin_unlock_irq(), interrupts from a detached
 * device would cause an irq storm.
 */
static int __maybe_unused w840_suspend(struct device *dev_d)
{
	struct net_device *dev = dev_get_drvdata(dev_d);
	struct netdev_private *np = netdev_priv(dev);
	void __iomem *ioaddr = np->base_addr;

	rtnl_lock();
	if (netif_running (dev)) {
		del_timer_sync(&np->timer);

		spin_lock_irq(&np->lock);
		netif_device_detach(dev);
		update_csr6(dev, 0);
		iowrite32(0, ioaddr + IntrEnable);
		spin_unlock_irq(&np->lock);

		synchronize_irq(np->pci_dev->irq);
		netif_tx_disable(dev);

		np->stats.rx_missed_errors += ioread32(ioaddr + RxMissed) & 0xffff;

		/* no more hardware accesses behind this line. */

		BUG_ON(np->csr6 || ioread32(ioaddr + IntrEnable));

		/* pci_power_off(pdev, -1); */

		free_rxtx_rings(np);
	} else {
		netif_device_detach(dev);
	}
	rtnl_unlock();
	return 0;
}

static int __maybe_unused w840_resume(struct device *dev_d)
{
	struct net_device *dev = dev_get_drvdata(dev_d);
	struct netdev_private *np = netdev_priv(dev);

	rtnl_lock();
	if (netif_device_present(dev))
		goto out; /* device not suspended */
	if (netif_running(dev)) {
		spin_lock_irq(&np->lock);
		iowrite32(1, np->base_addr+PCIBusCfg);
		ioread32(np->base_addr+PCIBusCfg);
		udelay(1);
		netif_device_attach(dev);
		init_rxtx_rings(dev);
		init_registers(dev);
		spin_unlock_irq(&np->lock);

		netif_wake_queue(dev);

		mod_timer(&np->timer, jiffies + 1*HZ);
	} else {
		netif_device_attach(dev);
	}
out:
	rtnl_unlock();
	return 0;
}

static SIMPLE_DEV_PM_OPS(w840_pm_ops, w840_suspend, w840_resume);

static struct pci_driver w840_driver = {
	.name		= DRV_NAME,
	.id_table	= w840_pci_tbl,
	.probe		= w840_probe1,
	.remove		= w840_remove1,
	.driver.pm	= &w840_pm_ops,
};

module_pci_driver(w840_driver);
