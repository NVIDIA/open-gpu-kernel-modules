/* EtherLinkXL.c: A 3Com EtherLink PCI III/XL ethernet driver for linux. */
/*
	Written 1996-1999 by Donald Becker.

	This software may be used and distributed according to the terms
	of the GNU General Public License, incorporated herein by reference.

	This driver is for the 3Com "Vortex" and "Boomerang" series ethercards.
	Members of the series include Fast EtherLink 3c590/3c592/3c595/3c597
	and the EtherLink XL 3c900 and 3c905 cards.

	Problem reports and questions should be directed to
	vortex@scyld.com

	The author may be reached as becker@scyld.com, or C/O
	Scyld Computing Corporation
	410 Severn Ave., Suite 210
	Annapolis MD 21403

*/

/*
 * FIXME: This driver _could_ support MTU changing, but doesn't.  See Don's hamachi.c implementation
 * as well as other drivers
 *
 * NOTE: If you make 'vortex_debug' a constant (#define vortex_debug 0) the driver shrinks by 2k
 * due to dead code elimination.  There will be some performance benefits from this due to
 * elimination of all the tests and reduced cache footprint.
 */


#define DRV_NAME	"3c59x"



/* A few values that may be tweaked. */
/* Keep the ring sizes a power of two for efficiency. */
#define TX_RING_SIZE	16
#define RX_RING_SIZE	32
#define PKT_BUF_SZ		1536			/* Size of each temporary Rx buffer.*/

/* "Knobs" that adjust features and parameters. */
/* Set the copy breakpoint for the copy-only-tiny-frames scheme.
   Setting to > 1512 effectively disables this feature. */
#ifndef __arm__
static int rx_copybreak = 200;
#else
/* ARM systems perform better by disregarding the bus-master
   transfer capability of these cards. -- rmk */
static int rx_copybreak = 1513;
#endif
/* Allow setting MTU to a larger size, bypassing the normal ethernet setup. */
static const int mtu = 1500;
/* Maximum events (Rx packets, etc.) to handle at each interrupt. */
static int max_interrupt_work = 32;
/* Tx timeout interval (millisecs) */
static int watchdog = 5000;

/* Allow aggregation of Tx interrupts.  Saves CPU load at the cost
 * of possible Tx stalls if the system is blocking interrupts
 * somewhere else.  Undefine this to disable.
 */
#define tx_interrupt_mitigation 1

/* Put out somewhat more debugging messages. (0: no msg, 1 minimal .. 6). */
#define vortex_debug debug
#ifdef VORTEX_DEBUG
static int vortex_debug = VORTEX_DEBUG;
#else
static int vortex_debug = 1;
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/in.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/mii.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <linux/highmem.h>
#include <linux/eisa.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/gfp.h>
#include <asm/irq.h>			/* For nr_irqs only. */
#include <asm/io.h>
#include <linux/uaccess.h>

/* Kernel compatibility defines, some common to David Hinds' PCMCIA package.
   This is only in the support-all-kernels source code. */

#define RUN_AT(x) (jiffies + (x))

#include <linux/delay.h>


static const char version[] =
	DRV_NAME ": Donald Becker and others.\n";

MODULE_AUTHOR("Donald Becker <becker@scyld.com>");
MODULE_DESCRIPTION("3Com 3c59x/3c9xx ethernet driver ");
MODULE_LICENSE("GPL");


/* Operational parameter that usually are not changed. */

/* The Vortex size is twice that of the original EtherLinkIII series: the
   runtime register window, window 1, is now always mapped in.
   The Boomerang size is twice as large as the Vortex -- it has additional
   bus master control registers. */
#define VORTEX_TOTAL_SIZE 0x20
#define BOOMERANG_TOTAL_SIZE 0x40

/* Set iff a MII transceiver on any interface requires mdio preamble.
   This only set with the original DP83840 on older 3c905 boards, so the extra
   code size of a per-interface flag is not worthwhile. */
static char mii_preamble_required;

#define PFX DRV_NAME ": "



/*
				Theory of Operation

I. Board Compatibility

This device driver is designed for the 3Com FastEtherLink and FastEtherLink
XL, 3Com's PCI to 10/100baseT adapters.  It also works with the 10Mbs
versions of the FastEtherLink cards.  The supported product IDs are
  3c590, 3c592, 3c595, 3c597, 3c900, 3c905

The related ISA 3c515 is supported with a separate driver, 3c515.c, included
with the kernel source or available from
    cesdis.gsfc.nasa.gov:/pub/linux/drivers/3c515.html

II. Board-specific settings

PCI bus devices are configured by the system at boot time, so no jumpers
need to be set on the board.  The system BIOS should be set to assign the
PCI INTA signal to an otherwise unused system IRQ line.

The EEPROM settings for media type and forced-full-duplex are observed.
The EEPROM media type should be left at the default "autoselect" unless using
10base2 or AUI connections which cannot be reliably detected.

III. Driver operation

The 3c59x series use an interface that's very similar to the previous 3c5x9
series.  The primary interface is two programmed-I/O FIFOs, with an
alternate single-contiguous-region bus-master transfer (see next).

The 3c900 "Boomerang" series uses a full-bus-master interface with separate
lists of transmit and receive descriptors, similar to the AMD LANCE/PCnet,
DEC Tulip and Intel Speedo3.  The first chip version retains a compatible
programmed-I/O interface that has been removed in 'B' and subsequent board
revisions.

One extension that is advertised in a very large font is that the adapters
are capable of being bus masters.  On the Vortex chip this capability was
only for a single contiguous region making it far less useful than the full
bus master capability.  There is a significant performance impact of taking
an extra interrupt or polling for the completion of each transfer, as well
as difficulty sharing the single transfer engine between the transmit and
receive threads.  Using DMA transfers is a win only with large blocks or
with the flawed versions of the Intel Orion motherboard PCI controller.

The Boomerang chip's full-bus-master interface is useful, and has the
currently-unused advantages over other similar chips that queued transmit
packets may be reordered and receive buffer groups are associated with a
single frame.

With full-bus-master support, this driver uses a "RX_COPYBREAK" scheme.
Rather than a fixed intermediate receive buffer, this scheme allocates
full-sized skbuffs as receive buffers.  The value RX_COPYBREAK is used as
the copying breakpoint: it is chosen to trade-off the memory wasted by
passing the full-sized skbuff to the queue layer for all frames vs. the
copying cost of copying a frame to a correctly-sized skbuff.

IIIC. Synchronization
The driver runs as two independent, single-threaded flows of control.  One
is the send-packet routine, which enforces single-threaded use by the
dev->tbusy flag.  The other thread is the interrupt handler, which is single
threaded by the hardware and other software.

IV. Notes

Thanks to Cameron Spitzer and Terry Murphy of 3Com for providing development
3c590, 3c595, and 3c900 boards.
The name "Vortex" is the internal 3Com project name for the PCI ASIC, and
the EISA version is called "Demon".  According to Terry these names come
from rides at the local amusement park.

The new chips support both ethernet (1.5K) and FDDI (4.5K) packet sizes!
This driver only supports ethernet packets because of the skbuff allocation
limit of 4K.
*/

/* This table drives the PCI probe routines.  It's mostly boilerplate in all
   of the drivers, and will likely be provided by some future kernel.
*/
enum pci_flags_bit {
	PCI_USES_MASTER=4,
};

enum {	IS_VORTEX=1, IS_BOOMERANG=2, IS_CYCLONE=4, IS_TORNADO=8,
	EEPROM_8BIT=0x10,	/* AKPM: Uses 0x230 as the base bitmaps for EEPROM reads */
	HAS_PWR_CTRL=0x20, HAS_MII=0x40, HAS_NWAY=0x80, HAS_CB_FNS=0x100,
	INVERT_MII_PWR=0x200, INVERT_LED_PWR=0x400, MAX_COLLISION_RESET=0x800,
	EEPROM_OFFSET=0x1000, HAS_HWCKSM=0x2000, WNO_XCVR_PWR=0x4000,
	EXTRA_PREAMBLE=0x8000, EEPROM_RESET=0x10000, };

enum vortex_chips {
	CH_3C590 = 0,
	CH_3C592,
	CH_3C597,
	CH_3C595_1,
	CH_3C595_2,

	CH_3C595_3,
	CH_3C900_1,
	CH_3C900_2,
	CH_3C900_3,
	CH_3C900_4,

	CH_3C900_5,
	CH_3C900B_FL,
	CH_3C905_1,
	CH_3C905_2,
	CH_3C905B_TX,
	CH_3C905B_1,

	CH_3C905B_2,
	CH_3C905B_FX,
	CH_3C905C,
	CH_3C9202,
	CH_3C980,
	CH_3C9805,

	CH_3CSOHO100_TX,
	CH_3C555,
	CH_3C556,
	CH_3C556B,
	CH_3C575,

	CH_3C575_1,
	CH_3CCFE575,
	CH_3CCFE575CT,
	CH_3CCFE656,
	CH_3CCFEM656,

	CH_3CCFEM656_1,
	CH_3C450,
	CH_3C920,
	CH_3C982A,
	CH_3C982B,

	CH_905BT4,
	CH_920B_EMB_WNM,
};


/* note: this array directly indexed by above enums, and MUST
 * be kept in sync with both the enums above, and the PCI device
 * table below
 */
static struct vortex_chip_info {
	const char *name;
	int flags;
	int drv_flags;
	int io_size;
} vortex_info_tbl[] = {
	{"3c590 Vortex 10Mbps",
	 PCI_USES_MASTER, IS_VORTEX, 32, },
	{"3c592 EISA 10Mbps Demon/Vortex",					/* AKPM: from Don's 3c59x_cb.c 0.49H */
	 PCI_USES_MASTER, IS_VORTEX, 32, },
	{"3c597 EISA Fast Demon/Vortex",					/* AKPM: from Don's 3c59x_cb.c 0.49H */
	 PCI_USES_MASTER, IS_VORTEX, 32, },
	{"3c595 Vortex 100baseTx",
	 PCI_USES_MASTER, IS_VORTEX, 32, },
	{"3c595 Vortex 100baseT4",
	 PCI_USES_MASTER, IS_VORTEX, 32, },

	{"3c595 Vortex 100base-MII",
	 PCI_USES_MASTER, IS_VORTEX, 32, },
	{"3c900 Boomerang 10baseT",
	 PCI_USES_MASTER, IS_BOOMERANG|EEPROM_RESET, 64, },
	{"3c900 Boomerang 10Mbps Combo",
	 PCI_USES_MASTER, IS_BOOMERANG|EEPROM_RESET, 64, },
	{"3c900 Cyclone 10Mbps TPO",						/* AKPM: from Don's 0.99M */
	 PCI_USES_MASTER, IS_CYCLONE|HAS_HWCKSM, 128, },
	{"3c900 Cyclone 10Mbps Combo",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_HWCKSM, 128, },

	{"3c900 Cyclone 10Mbps TPC",						/* AKPM: from Don's 0.99M */
	 PCI_USES_MASTER, IS_CYCLONE|HAS_HWCKSM, 128, },
	{"3c900B-FL Cyclone 10base-FL",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_HWCKSM, 128, },
	{"3c905 Boomerang 100baseTx",
	 PCI_USES_MASTER, IS_BOOMERANG|HAS_MII|EEPROM_RESET, 64, },
	{"3c905 Boomerang 100baseT4",
	 PCI_USES_MASTER, IS_BOOMERANG|HAS_MII|EEPROM_RESET, 64, },
	{"3C905B-TX Fast Etherlink XL PCI",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_HWCKSM|EXTRA_PREAMBLE, 128, },
	{"3c905B Cyclone 100baseTx",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_HWCKSM|EXTRA_PREAMBLE, 128, },

	{"3c905B Cyclone 10/100/BNC",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_HWCKSM, 128, },
	{"3c905B-FX Cyclone 100baseFx",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_HWCKSM, 128, },
	{"3c905C Tornado",
	PCI_USES_MASTER, IS_TORNADO|HAS_NWAY|HAS_HWCKSM|EXTRA_PREAMBLE, 128, },
	{"3c920B-EMB-WNM (ATI Radeon 9100 IGP)",
	 PCI_USES_MASTER, IS_TORNADO|HAS_MII|HAS_HWCKSM, 128, },
	{"3c980 Cyclone",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_HWCKSM|EXTRA_PREAMBLE, 128, },

	{"3c980C Python-T",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_HWCKSM, 128, },
	{"3cSOHO100-TX Hurricane",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_HWCKSM|EXTRA_PREAMBLE, 128, },
	{"3c555 Laptop Hurricane",
	 PCI_USES_MASTER, IS_CYCLONE|EEPROM_8BIT|HAS_HWCKSM, 128, },
	{"3c556 Laptop Tornado",
	 PCI_USES_MASTER, IS_TORNADO|HAS_NWAY|EEPROM_8BIT|HAS_CB_FNS|INVERT_MII_PWR|
									HAS_HWCKSM, 128, },
	{"3c556B Laptop Hurricane",
	 PCI_USES_MASTER, IS_TORNADO|HAS_NWAY|EEPROM_OFFSET|HAS_CB_FNS|INVERT_MII_PWR|
	                                WNO_XCVR_PWR|HAS_HWCKSM, 128, },

	{"3c575 [Megahertz] 10/100 LAN 	CardBus",
	PCI_USES_MASTER, IS_BOOMERANG|HAS_MII|EEPROM_8BIT, 128, },
	{"3c575 Boomerang CardBus",
	 PCI_USES_MASTER, IS_BOOMERANG|HAS_MII|EEPROM_8BIT, 128, },
	{"3CCFE575BT Cyclone CardBus",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_CB_FNS|EEPROM_8BIT|
									INVERT_LED_PWR|HAS_HWCKSM, 128, },
	{"3CCFE575CT Tornado CardBus",
	 PCI_USES_MASTER, IS_TORNADO|HAS_NWAY|HAS_CB_FNS|EEPROM_8BIT|INVERT_MII_PWR|
									MAX_COLLISION_RESET|HAS_HWCKSM, 128, },
	{"3CCFE656 Cyclone CardBus",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_CB_FNS|EEPROM_8BIT|INVERT_MII_PWR|
									INVERT_LED_PWR|HAS_HWCKSM, 128, },

	{"3CCFEM656B Cyclone+Winmodem CardBus",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_CB_FNS|EEPROM_8BIT|INVERT_MII_PWR|
									INVERT_LED_PWR|HAS_HWCKSM, 128, },
	{"3CXFEM656C Tornado+Winmodem CardBus",			/* From pcmcia-cs-3.1.5 */
	 PCI_USES_MASTER, IS_TORNADO|HAS_NWAY|HAS_CB_FNS|EEPROM_8BIT|INVERT_MII_PWR|
									MAX_COLLISION_RESET|HAS_HWCKSM, 128, },
	{"3c450 HomePNA Tornado",						/* AKPM: from Don's 0.99Q */
	 PCI_USES_MASTER, IS_TORNADO|HAS_NWAY|HAS_HWCKSM, 128, },
	{"3c920 Tornado",
	 PCI_USES_MASTER, IS_TORNADO|HAS_NWAY|HAS_HWCKSM, 128, },
	{"3c982 Hydra Dual Port A",
	 PCI_USES_MASTER, IS_TORNADO|HAS_HWCKSM|HAS_NWAY, 128, },

	{"3c982 Hydra Dual Port B",
	 PCI_USES_MASTER, IS_TORNADO|HAS_HWCKSM|HAS_NWAY, 128, },
	{"3c905B-T4",
	 PCI_USES_MASTER, IS_CYCLONE|HAS_NWAY|HAS_HWCKSM|EXTRA_PREAMBLE, 128, },
	{"3c920B-EMB-WNM Tornado",
	 PCI_USES_MASTER, IS_TORNADO|HAS_NWAY|HAS_HWCKSM, 128, },

	{NULL,}, /* NULL terminated list. */
};


static const struct pci_device_id vortex_pci_tbl[] = {
	{ 0x10B7, 0x5900, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C590 },
	{ 0x10B7, 0x5920, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C592 },
	{ 0x10B7, 0x5970, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C597 },
	{ 0x10B7, 0x5950, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C595_1 },
	{ 0x10B7, 0x5951, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C595_2 },

	{ 0x10B7, 0x5952, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C595_3 },
	{ 0x10B7, 0x9000, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C900_1 },
	{ 0x10B7, 0x9001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C900_2 },
	{ 0x10B7, 0x9004, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C900_3 },
	{ 0x10B7, 0x9005, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C900_4 },

	{ 0x10B7, 0x9006, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C900_5 },
	{ 0x10B7, 0x900A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C900B_FL },
	{ 0x10B7, 0x9050, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C905_1 },
	{ 0x10B7, 0x9051, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C905_2 },
	{ 0x10B7, 0x9054, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C905B_TX },
	{ 0x10B7, 0x9055, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C905B_1 },

	{ 0x10B7, 0x9058, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C905B_2 },
	{ 0x10B7, 0x905A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C905B_FX },
	{ 0x10B7, 0x9200, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C905C },
	{ 0x10B7, 0x9202, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C9202 },
	{ 0x10B7, 0x9800, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C980 },
	{ 0x10B7, 0x9805, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C9805 },

	{ 0x10B7, 0x7646, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3CSOHO100_TX },
	{ 0x10B7, 0x5055, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C555 },
	{ 0x10B7, 0x6055, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C556 },
	{ 0x10B7, 0x6056, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C556B },
	{ 0x10B7, 0x5b57, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C575 },

	{ 0x10B7, 0x5057, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C575_1 },
	{ 0x10B7, 0x5157, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3CCFE575 },
	{ 0x10B7, 0x5257, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3CCFE575CT },
	{ 0x10B7, 0x6560, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3CCFE656 },
	{ 0x10B7, 0x6562, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3CCFEM656 },

	{ 0x10B7, 0x6564, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3CCFEM656_1 },
	{ 0x10B7, 0x4500, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C450 },
	{ 0x10B7, 0x9201, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C920 },
	{ 0x10B7, 0x1201, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C982A },
	{ 0x10B7, 0x1202, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_3C982B },

	{ 0x10B7, 0x9056, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_905BT4 },
	{ 0x10B7, 0x9210, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH_920B_EMB_WNM },

	{0,}						/* 0 terminated list. */
};
MODULE_DEVICE_TABLE(pci, vortex_pci_tbl);


/* Operational definitions.
   These are not used by other compilation units and thus are not
   exported in a ".h" file.

   First the windows.  There are eight register windows, with the command
   and status registers available in each.
   */
#define EL3_CMD 0x0e
#define EL3_STATUS 0x0e

/* The top five bits written to EL3_CMD are a command, the lower
   11 bits are the parameter, if applicable.
   Note that 11 parameters bits was fine for ethernet, but the new chip
   can handle FDDI length frames (~4500 octets) and now parameters count
   32-bit 'Dwords' rather than octets. */

enum vortex_cmd {
	TotalReset = 0<<11, SelectWindow = 1<<11, StartCoax = 2<<11,
	RxDisable = 3<<11, RxEnable = 4<<11, RxReset = 5<<11,
	UpStall = 6<<11, UpUnstall = (6<<11)+1,
	DownStall = (6<<11)+2, DownUnstall = (6<<11)+3,
	RxDiscard = 8<<11, TxEnable = 9<<11, TxDisable = 10<<11, TxReset = 11<<11,
	FakeIntr = 12<<11, AckIntr = 13<<11, SetIntrEnb = 14<<11,
	SetStatusEnb = 15<<11, SetRxFilter = 16<<11, SetRxThreshold = 17<<11,
	SetTxThreshold = 18<<11, SetTxStart = 19<<11,
	StartDMAUp = 20<<11, StartDMADown = (20<<11)+1, StatsEnable = 21<<11,
	StatsDisable = 22<<11, StopCoax = 23<<11, SetFilterBit = 25<<11,};

/* The SetRxFilter command accepts the following classes: */
enum RxFilter {
	RxStation = 1, RxMulticast = 2, RxBroadcast = 4, RxProm = 8 };

/* Bits in the general status register. */
enum vortex_status {
	IntLatch = 0x0001, HostError = 0x0002, TxComplete = 0x0004,
	TxAvailable = 0x0008, RxComplete = 0x0010, RxEarly = 0x0020,
	IntReq = 0x0040, StatsFull = 0x0080,
	DMADone = 1<<8, DownComplete = 1<<9, UpComplete = 1<<10,
	DMAInProgress = 1<<11,			/* DMA controller is still busy.*/
	CmdInProgress = 1<<12,			/* EL3_CMD is still busy.*/
};

/* Register window 1 offsets, the window used in normal operation.
   On the Vortex this window is always mapped at offsets 0x10-0x1f. */
enum Window1 {
	TX_FIFO = 0x10,  RX_FIFO = 0x10,  RxErrors = 0x14,
	RxStatus = 0x18,  Timer=0x1A, TxStatus = 0x1B,
	TxFree = 0x1C, /* Remaining free bytes in Tx buffer. */
};
enum Window0 {
	Wn0EepromCmd = 10,		/* Window 0: EEPROM command register. */
	Wn0EepromData = 12,		/* Window 0: EEPROM results register. */
	IntrStatus=0x0E,		/* Valid in all windows. */
};
enum Win0_EEPROM_bits {
	EEPROM_Read = 0x80, EEPROM_WRITE = 0x40, EEPROM_ERASE = 0xC0,
	EEPROM_EWENB = 0x30,		/* Enable erasing/writing for 10 msec. */
	EEPROM_EWDIS = 0x00,		/* Disable EWENB before 10 msec timeout. */
};
/* EEPROM locations. */
enum eeprom_offset {
	PhysAddr01=0, PhysAddr23=1, PhysAddr45=2, ModelID=3,
	EtherLink3ID=7, IFXcvrIO=8, IRQLine=9,
	NodeAddr01=10, NodeAddr23=11, NodeAddr45=12,
	DriverTune=13, Checksum=15};

enum Window2 {			/* Window 2. */
	Wn2_ResetOptions=12,
};
enum Window3 {			/* Window 3: MAC/config bits. */
	Wn3_Config=0, Wn3_MaxPktSize=4, Wn3_MAC_Ctrl=6, Wn3_Options=8,
};

#define BFEXT(value, offset, bitcount)  \
    ((((unsigned long)(value)) >> (offset)) & ((1 << (bitcount)) - 1))

#define BFINS(lhs, rhs, offset, bitcount)					\
	(((lhs) & ~((((1 << (bitcount)) - 1)) << (offset))) |	\
	(((rhs) & ((1 << (bitcount)) - 1)) << (offset)))

#define RAM_SIZE(v)		BFEXT(v, 0, 3)
#define RAM_WIDTH(v)	BFEXT(v, 3, 1)
#define RAM_SPEED(v)	BFEXT(v, 4, 2)
#define ROM_SIZE(v)		BFEXT(v, 6, 2)
#define RAM_SPLIT(v)	BFEXT(v, 16, 2)
#define XCVR(v)			BFEXT(v, 20, 4)
#define AUTOSELECT(v)	BFEXT(v, 24, 1)

enum Window4 {		/* Window 4: Xcvr/media bits. */
	Wn4_FIFODiag = 4, Wn4_NetDiag = 6, Wn4_PhysicalMgmt=8, Wn4_Media = 10,
};
enum Win4_Media_bits {
	Media_SQE = 0x0008,		/* Enable SQE error counting for AUI. */
	Media_10TP = 0x00C0,	/* Enable link beat and jabber for 10baseT. */
	Media_Lnk = 0x0080,		/* Enable just link beat for 100TX/100FX. */
	Media_LnkBeat = 0x0800,
};
enum Window7 {					/* Window 7: Bus Master control. */
	Wn7_MasterAddr = 0, Wn7_VlanEtherType=4, Wn7_MasterLen = 6,
	Wn7_MasterStatus = 12,
};
/* Boomerang bus master control registers. */
enum MasterCtrl {
	PktStatus = 0x20, DownListPtr = 0x24, FragAddr = 0x28, FragLen = 0x2c,
	TxFreeThreshold = 0x2f, UpPktStatus = 0x30, UpListPtr = 0x38,
};

/* The Rx and Tx descriptor lists.
   Caution Alpha hackers: these types are 32 bits!  Note also the 8 byte
   alignment contraint on tx_ring[] and rx_ring[]. */
#define LAST_FRAG 	0x80000000			/* Last Addr/Len pair in descriptor. */
#define DN_COMPLETE	0x00010000			/* This packet has been downloaded */
struct boom_rx_desc {
	__le32 next;					/* Last entry points to 0.   */
	__le32 status;
	__le32 addr;					/* Up to 63 addr/len pairs possible. */
	__le32 length;					/* Set LAST_FRAG to indicate last pair. */
};
/* Values for the Rx status entry. */
enum rx_desc_status {
	RxDComplete=0x00008000, RxDError=0x4000,
	/* See boomerang_rx() for actual error bits */
	IPChksumErr=1<<25, TCPChksumErr=1<<26, UDPChksumErr=1<<27,
	IPChksumValid=1<<29, TCPChksumValid=1<<30, UDPChksumValid=1<<31,
};

#ifdef MAX_SKB_FRAGS
#define DO_ZEROCOPY 1
#else
#define DO_ZEROCOPY 0
#endif

struct boom_tx_desc {
	__le32 next;					/* Last entry points to 0.   */
	__le32 status;					/* bits 0:12 length, others see below.  */
#if DO_ZEROCOPY
	struct {
		__le32 addr;
		__le32 length;
	} frag[1+MAX_SKB_FRAGS];
#else
		__le32 addr;
		__le32 length;
#endif
};

/* Values for the Tx status entry. */
enum tx_desc_status {
	CRCDisable=0x2000, TxDComplete=0x8000,
	AddIPChksum=0x02000000, AddTCPChksum=0x04000000, AddUDPChksum=0x08000000,
	TxIntrUploaded=0x80000000,		/* IRQ when in FIFO, but maybe not sent. */
};

/* Chip features we care about in vp->capabilities, read from the EEPROM. */
enum ChipCaps { CapBusMaster=0x20, CapPwrMgmt=0x2000 };

struct vortex_extra_stats {
	unsigned long tx_deferred;
	unsigned long tx_max_collisions;
	unsigned long tx_multiple_collisions;
	unsigned long tx_single_collisions;
	unsigned long rx_bad_ssd;
};

struct vortex_private {
	/* The Rx and Tx rings should be quad-word-aligned. */
	struct boom_rx_desc* rx_ring;
	struct boom_tx_desc* tx_ring;
	dma_addr_t rx_ring_dma;
	dma_addr_t tx_ring_dma;
	/* The addresses of transmit- and receive-in-place skbuffs. */
	struct sk_buff* rx_skbuff[RX_RING_SIZE];
	struct sk_buff* tx_skbuff[TX_RING_SIZE];
	unsigned int cur_rx, cur_tx;		/* The next free ring entry */
	unsigned int dirty_tx;	/* The ring entries to be free()ed. */
	struct vortex_extra_stats xstats;	/* NIC-specific extra stats */
	struct sk_buff *tx_skb;				/* Packet being eaten by bus master ctrl.  */
	dma_addr_t tx_skb_dma;				/* Allocated DMA address for bus master ctrl DMA.   */

	/* PCI configuration space information. */
	struct device *gendev;
	void __iomem *ioaddr;			/* IO address space */
	void __iomem *cb_fn_base;		/* CardBus function status addr space. */

	/* Some values here only for performance evaluation and path-coverage */
	int rx_nocopy, rx_copy, queued_packet, rx_csumhits;
	int card_idx;

	/* The remainder are related to chip state, mostly media selection. */
	struct timer_list timer;			/* Media selection timer. */
	int options;						/* User-settable misc. driver options. */
	unsigned int media_override:4, 		/* Passed-in media type. */
		default_media:4,				/* Read from the EEPROM/Wn3_Config. */
		full_duplex:1, autoselect:1,
		bus_master:1,					/* Vortex can only do a fragment bus-m. */
		full_bus_master_tx:1, full_bus_master_rx:2, /* Boomerang  */
		flow_ctrl:1,					/* Use 802.3x flow control (PAUSE only) */
		partner_flow_ctrl:1,			/* Partner supports flow control */
		has_nway:1,
		enable_wol:1,					/* Wake-on-LAN is enabled */
		pm_state_valid:1,				/* pci_dev->saved_config_space has sane contents */
		open:1,
		medialock:1,
		large_frames:1,			/* accept large frames */
		handling_irq:1;			/* private in_irq indicator */
	/* {get|set}_wol operations are already serialized by rtnl.
	 * no additional locking is required for the enable_wol and acpi_set_WOL()
	 */
	int drv_flags;
	u16 status_enable;
	u16 intr_enable;
	u16 available_media;				/* From Wn3_Options. */
	u16 capabilities, info1, info2;		/* Various, from EEPROM. */
	u16 advertising;					/* NWay media advertisement */
	unsigned char phys[2];				/* MII device addresses. */
	u16 deferred;						/* Resend these interrupts when we
										 * bale from the ISR */
	u16 io_size;						/* Size of PCI region (for release_region) */

	/* Serialises access to hardware other than MII and variables below.
	 * The lock hierarchy is rtnl_lock > {lock, mii_lock} > window_lock. */
	spinlock_t lock;

	spinlock_t mii_lock;		/* Serialises access to MII */
	struct mii_if_info mii;		/* MII lib hooks/info */
	spinlock_t window_lock;		/* Serialises access to windowed regs */
	int window;			/* Register window */
};

static void window_set(struct vortex_private *vp, int window)
{
	if (window != vp->window) {
		iowrite16(SelectWindow + window, vp->ioaddr + EL3_CMD);
		vp->window = window;
	}
}

#define DEFINE_WINDOW_IO(size)						\
static u ## size							\
window_read ## size(struct vortex_private *vp, int window, int addr)	\
{									\
	unsigned long flags;						\
	u ## size ret;							\
	spin_lock_irqsave(&vp->window_lock, flags);			\
	window_set(vp, window);						\
	ret = ioread ## size(vp->ioaddr + addr);			\
	spin_unlock_irqrestore(&vp->window_lock, flags);		\
	return ret;							\
}									\
static void								\
window_write ## size(struct vortex_private *vp, u ## size value,	\
		     int window, int addr)				\
{									\
	unsigned long flags;						\
	spin_lock_irqsave(&vp->window_lock, flags);			\
	window_set(vp, window);						\
	iowrite ## size(value, vp->ioaddr + addr);			\
	spin_unlock_irqrestore(&vp->window_lock, flags);		\
}
DEFINE_WINDOW_IO(8)
DEFINE_WINDOW_IO(16)
DEFINE_WINDOW_IO(32)

#ifdef CONFIG_PCI
#define DEVICE_PCI(dev) ((dev_is_pci(dev)) ? to_pci_dev((dev)) : NULL)
#else
#define DEVICE_PCI(dev) NULL
#endif

#define VORTEX_PCI(vp)							\
	((struct pci_dev *) (((vp)->gendev) ? DEVICE_PCI((vp)->gendev) : NULL))

#ifdef CONFIG_EISA
#define DEVICE_EISA(dev) (((dev)->bus == &eisa_bus_type) ? to_eisa_device((dev)) : NULL)
#else
#define DEVICE_EISA(dev) NULL
#endif

#define VORTEX_EISA(vp)							\
	((struct eisa_device *) (((vp)->gendev) ? DEVICE_EISA((vp)->gendev) : NULL))

/* The action to take with a media selection timer tick.
   Note that we deviate from the 3Com order by checking 10base2 before AUI.
 */
enum xcvr_types {
	XCVR_10baseT=0, XCVR_AUI, XCVR_10baseTOnly, XCVR_10base2, XCVR_100baseTx,
	XCVR_100baseFx, XCVR_MII=6, XCVR_NWAY=8, XCVR_ExtMII=9, XCVR_Default=10,
};

static const struct media_table {
	char *name;
	unsigned int media_bits:16,		/* Bits to set in Wn4_Media register. */
		mask:8,						/* The transceiver-present bit in Wn3_Config.*/
		next:8;						/* The media type to try next. */
	int wait;						/* Time before we check media status. */
} media_tbl[] = {
  {	"10baseT",   Media_10TP,0x08, XCVR_10base2, (14*HZ)/10},
  { "10Mbs AUI", Media_SQE, 0x20, XCVR_Default, (1*HZ)/10},
  { "undefined", 0,			0x80, XCVR_10baseT, 10000},
  { "10base2",   0,			0x10, XCVR_AUI,		(1*HZ)/10},
  { "100baseTX", Media_Lnk, 0x02, XCVR_100baseFx, (14*HZ)/10},
  { "100baseFX", Media_Lnk, 0x04, XCVR_MII,		(14*HZ)/10},
  { "MII",		 0,			0x41, XCVR_10baseT, 3*HZ },
  { "undefined", 0,			0x01, XCVR_10baseT, 10000},
  { "Autonegotiate", 0,		0x41, XCVR_10baseT, 3*HZ},
  { "MII-External",	 0,		0x41, XCVR_10baseT, 3*HZ },
  { "Default",	 0,			0xFF, XCVR_10baseT, 10000},
};

static struct {
	const char str[ETH_GSTRING_LEN];
} ethtool_stats_keys[] = {
	{ "tx_deferred" },
	{ "tx_max_collisions" },
	{ "tx_multiple_collisions" },
	{ "tx_single_collisions" },
	{ "rx_bad_ssd" },
};

/* number of ETHTOOL_GSTATS u64's */
#define VORTEX_NUM_STATS    5

static int vortex_probe1(struct device *gendev, void __iomem *ioaddr, int irq,
				   int chip_idx, int card_idx);
static int vortex_up(struct net_device *dev);
static void vortex_down(struct net_device *dev, int final);
static int vortex_open(struct net_device *dev);
static void mdio_sync(struct vortex_private *vp, int bits);
static int mdio_read(struct net_device *dev, int phy_id, int location);
static void mdio_write(struct net_device *vp, int phy_id, int location, int value);
static void vortex_timer(struct timer_list *t);
static netdev_tx_t vortex_start_xmit(struct sk_buff *skb,
				     struct net_device *dev);
static netdev_tx_t boomerang_start_xmit(struct sk_buff *skb,
					struct net_device *dev);
static int vortex_rx(struct net_device *dev);
static int boomerang_rx(struct net_device *dev);
static irqreturn_t vortex_boomerang_interrupt(int irq, void *dev_id);
static irqreturn_t _vortex_interrupt(int irq, struct net_device *dev);
static irqreturn_t _boomerang_interrupt(int irq, struct net_device *dev);
static int vortex_close(struct net_device *dev);
static void dump_tx_ring(struct net_device *dev);
static void update_stats(void __iomem *ioaddr, struct net_device *dev);
static struct net_device_stats *vortex_get_stats(struct net_device *dev);
static void set_rx_mode(struct net_device *dev);
#ifdef CONFIG_PCI
static int vortex_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
#endif
static void vortex_tx_timeout(struct net_device *dev, unsigned int txqueue);
static void acpi_set_WOL(struct net_device *dev);
static const struct ethtool_ops vortex_ethtool_ops;
static void set_8021q_mode(struct net_device *dev, int enable);

/* This driver uses 'options' to pass the media type, full-duplex flag, etc. */
/* Option count limit only -- unlimited interfaces are supported. */
#define MAX_UNITS 8
static int options[MAX_UNITS] = { [0 ... MAX_UNITS-1] = -1 };
static int full_duplex[MAX_UNITS] = {[0 ... MAX_UNITS-1] = -1 };
static int hw_checksums[MAX_UNITS] = {[0 ... MAX_UNITS-1] = -1 };
static int flow_ctrl[MAX_UNITS] = {[0 ... MAX_UNITS-1] = -1 };
static int enable_wol[MAX_UNITS] = {[0 ... MAX_UNITS-1] = -1 };
static int use_mmio[MAX_UNITS] = {[0 ... MAX_UNITS-1] = -1 };
static int global_options = -1;
static int global_full_duplex = -1;
static int global_enable_wol = -1;
static int global_use_mmio = -1;

/* Variables to work-around the Compaq PCI BIOS32 problem. */
static int compaq_ioaddr, compaq_irq, compaq_device_id = 0x5900;
static struct net_device *compaq_net_device;

static int vortex_cards_found;

module_param(debug, int, 0);
module_param(global_options, int, 0);
module_param_array(options, int, NULL, 0);
module_param(global_full_duplex, int, 0);
module_param_array(full_duplex, int, NULL, 0);
module_param_array(hw_checksums, int, NULL, 0);
module_param_array(flow_ctrl, int, NULL, 0);
module_param(global_enable_wol, int, 0);
module_param_array(enable_wol, int, NULL, 0);
module_param(rx_copybreak, int, 0);
module_param(max_interrupt_work, int, 0);
module_param_hw(compaq_ioaddr, int, ioport, 0);
module_param_hw(compaq_irq, int, irq, 0);
module_param(compaq_device_id, int, 0);
module_param(watchdog, int, 0);
module_param(global_use_mmio, int, 0);
module_param_array(use_mmio, int, NULL, 0);
MODULE_PARM_DESC(debug, "3c59x debug level (0-6)");
MODULE_PARM_DESC(options, "3c59x: Bits 0-3: media type, bit 4: bus mastering, bit 9: full duplex");
MODULE_PARM_DESC(global_options, "3c59x: same as options, but applies to all NICs if options is unset");
MODULE_PARM_DESC(full_duplex, "3c59x full duplex setting(s) (1)");
MODULE_PARM_DESC(global_full_duplex, "3c59x: same as full_duplex, but applies to all NICs if full_duplex is unset");
MODULE_PARM_DESC(hw_checksums, "3c59x Hardware checksum checking by adapter(s) (0-1)");
MODULE_PARM_DESC(flow_ctrl, "3c59x 802.3x flow control usage (PAUSE only) (0-1)");
MODULE_PARM_DESC(enable_wol, "3c59x: Turn on Wake-on-LAN for adapter(s) (0-1)");
MODULE_PARM_DESC(global_enable_wol, "3c59x: same as enable_wol, but applies to all NICs if enable_wol is unset");
MODULE_PARM_DESC(rx_copybreak, "3c59x copy breakpoint for copy-only-tiny-frames");
MODULE_PARM_DESC(max_interrupt_work, "3c59x maximum events handled per interrupt");
MODULE_PARM_DESC(compaq_ioaddr, "3c59x PCI I/O base address (Compaq BIOS problem workaround)");
MODULE_PARM_DESC(compaq_irq, "3c59x PCI IRQ number (Compaq BIOS problem workaround)");
MODULE_PARM_DESC(compaq_device_id, "3c59x PCI device ID (Compaq BIOS problem workaround)");
MODULE_PARM_DESC(watchdog, "3c59x transmit timeout in milliseconds");
MODULE_PARM_DESC(global_use_mmio, "3c59x: same as use_mmio, but applies to all NICs if options is unset");
MODULE_PARM_DESC(use_mmio, "3c59x: use memory-mapped PCI I/O resource (0-1)");

#ifdef CONFIG_NET_POLL_CONTROLLER
static void poll_vortex(struct net_device *dev)
{
	vortex_boomerang_interrupt(dev->irq, dev);
}
#endif

#ifdef CONFIG_PM

static int vortex_suspend(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);

	if (!ndev || !netif_running(ndev))
		return 0;

	netif_device_detach(ndev);
	vortex_down(ndev, 1);

	return 0;
}

static int vortex_resume(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);
	int err;

	if (!ndev || !netif_running(ndev))
		return 0;

	err = vortex_up(ndev);
	if (err)
		return err;

	netif_device_attach(ndev);

	return 0;
}

static const struct dev_pm_ops vortex_pm_ops = {
	.suspend = vortex_suspend,
	.resume = vortex_resume,
	.freeze = vortex_suspend,
	.thaw = vortex_resume,
	.poweroff = vortex_suspend,
	.restore = vortex_resume,
};

#define VORTEX_PM_OPS (&vortex_pm_ops)

#else /* !CONFIG_PM */

#define VORTEX_PM_OPS NULL

#endif /* !CONFIG_PM */

#ifdef CONFIG_EISA
static const struct eisa_device_id vortex_eisa_ids[] = {
	{ "TCM5920", CH_3C592 },
	{ "TCM5970", CH_3C597 },
	{ "" }
};
MODULE_DEVICE_TABLE(eisa, vortex_eisa_ids);

static int vortex_eisa_probe(struct device *device)
{
	void __iomem *ioaddr;
	struct eisa_device *edev;

	edev = to_eisa_device(device);

	if (!request_region(edev->base_addr, VORTEX_TOTAL_SIZE, DRV_NAME))
		return -EBUSY;

	ioaddr = ioport_map(edev->base_addr, VORTEX_TOTAL_SIZE);

	if (vortex_probe1(device, ioaddr, ioread16(ioaddr + 0xC88) >> 12,
					  edev->id.driver_data, vortex_cards_found)) {
		release_region(edev->base_addr, VORTEX_TOTAL_SIZE);
		return -ENODEV;
	}

	vortex_cards_found++;

	return 0;
}

static int vortex_eisa_remove(struct device *device)
{
	struct eisa_device *edev;
	struct net_device *dev;
	struct vortex_private *vp;
	void __iomem *ioaddr;

	edev = to_eisa_device(device);
	dev = eisa_get_drvdata(edev);

	if (!dev) {
		pr_err("vortex_eisa_remove called for Compaq device!\n");
		BUG();
	}

	vp = netdev_priv(dev);
	ioaddr = vp->ioaddr;

	unregister_netdev(dev);
	iowrite16(TotalReset|0x14, ioaddr + EL3_CMD);
	release_region(edev->base_addr, VORTEX_TOTAL_SIZE);

	free_netdev(dev);
	return 0;
}

static struct eisa_driver vortex_eisa_driver = {
	.id_table = vortex_eisa_ids,
	.driver   = {
		.name    = "3c59x",
		.probe   = vortex_eisa_probe,
		.remove  = vortex_eisa_remove
	}
};

#endif /* CONFIG_EISA */

/* returns count found (>= 0), or negative on error */
static int __init vortex_eisa_init(void)
{
	int eisa_found = 0;
	int orig_cards_found = vortex_cards_found;

#ifdef CONFIG_EISA
	int err;

	err = eisa_driver_register (&vortex_eisa_driver);
	if (!err) {
		/*
		 * Because of the way EISA bus is probed, we cannot assume
		 * any device have been found when we exit from
		 * eisa_driver_register (the bus root driver may not be
		 * initialized yet). So we blindly assume something was
		 * found, and let the sysfs magic happened...
		 */
		eisa_found = 1;
	}
#endif

	/* Special code to work-around the Compaq PCI BIOS32 problem. */
	if (compaq_ioaddr) {
		vortex_probe1(NULL, ioport_map(compaq_ioaddr, VORTEX_TOTAL_SIZE),
			      compaq_irq, compaq_device_id, vortex_cards_found++);
	}

	return vortex_cards_found - orig_cards_found + eisa_found;
}

/* returns count (>= 0), or negative on error */
static int vortex_init_one(struct pci_dev *pdev,
			   const struct pci_device_id *ent)
{
	int rc, unit, pci_bar;
	struct vortex_chip_info *vci;
	void __iomem *ioaddr;

	/* wake up and enable device */
	rc = pci_enable_device(pdev);
	if (rc < 0)
		goto out;

	rc = pci_request_regions(pdev, DRV_NAME);
	if (rc < 0)
		goto out_disable;

	unit = vortex_cards_found;

	if (global_use_mmio < 0 && (unit >= MAX_UNITS || use_mmio[unit] < 0)) {
		/* Determine the default if the user didn't override us */
		vci = &vortex_info_tbl[ent->driver_data];
		pci_bar = vci->drv_flags & (IS_CYCLONE | IS_TORNADO) ? 1 : 0;
	} else if (unit < MAX_UNITS && use_mmio[unit] >= 0)
		pci_bar = use_mmio[unit] ? 1 : 0;
	else
		pci_bar = global_use_mmio ? 1 : 0;

	ioaddr = pci_iomap(pdev, pci_bar, 0);
	if (!ioaddr) /* If mapping fails, fall-back to BAR 0... */
		ioaddr = pci_iomap(pdev, 0, 0);
	if (!ioaddr) {
		rc = -ENOMEM;
		goto out_release;
	}

	rc = vortex_probe1(&pdev->dev, ioaddr, pdev->irq,
			   ent->driver_data, unit);
	if (rc < 0)
		goto out_iounmap;

	vortex_cards_found++;
	goto out;

out_iounmap:
	pci_iounmap(pdev, ioaddr);
out_release:
	pci_release_regions(pdev);
out_disable:
	pci_disable_device(pdev);
out:
	return rc;
}

static const struct net_device_ops boomrang_netdev_ops = {
	.ndo_open		= vortex_open,
	.ndo_stop		= vortex_close,
	.ndo_start_xmit		= boomerang_start_xmit,
	.ndo_tx_timeout		= vortex_tx_timeout,
	.ndo_get_stats		= vortex_get_stats,
#ifdef CONFIG_PCI
	.ndo_do_ioctl 		= vortex_ioctl,
#endif
	.ndo_set_rx_mode	= set_rx_mode,
	.ndo_set_mac_address 	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= poll_vortex,
#endif
};

static const struct net_device_ops vortex_netdev_ops = {
	.ndo_open		= vortex_open,
	.ndo_stop		= vortex_close,
	.ndo_start_xmit		= vortex_start_xmit,
	.ndo_tx_timeout		= vortex_tx_timeout,
	.ndo_get_stats		= vortex_get_stats,
#ifdef CONFIG_PCI
	.ndo_do_ioctl 		= vortex_ioctl,
#endif
	.ndo_set_rx_mode	= set_rx_mode,
	.ndo_set_mac_address 	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= poll_vortex,
#endif
};

/*
 * Start up the PCI/EISA device which is described by *gendev.
 * Return 0 on success.
 *
 * NOTE: pdev can be NULL, for the case of a Compaq device
 */
static int vortex_probe1(struct device *gendev, void __iomem *ioaddr, int irq,
			 int chip_idx, int card_idx)
{
	struct vortex_private *vp;
	int option;
	unsigned int eeprom[0x40], checksum = 0;		/* EEPROM contents */
	int i, step;
	struct net_device *dev;
	static int printed_version;
	int retval, print_info;
	struct vortex_chip_info * const vci = &vortex_info_tbl[chip_idx];
	const char *print_name = "3c59x";
	struct pci_dev *pdev = NULL;
	struct eisa_device *edev = NULL;

	if (!printed_version) {
		pr_info("%s", version);
		printed_version = 1;
	}

	if (gendev) {
		if ((pdev = DEVICE_PCI(gendev))) {
			print_name = pci_name(pdev);
		}

		if ((edev = DEVICE_EISA(gendev))) {
			print_name = dev_name(&edev->dev);
		}
	}

	dev = alloc_etherdev(sizeof(*vp));
	retval = -ENOMEM;
	if (!dev)
		goto out;

	SET_NETDEV_DEV(dev, gendev);
	vp = netdev_priv(dev);

	option = global_options;

	/* The lower four bits are the media type. */
	if (dev->mem_start) {
		/*
		 * The 'options' param is passed in as the third arg to the
		 * LILO 'ether=' argument for non-modular use
		 */
		option = dev->mem_start;
	}
	else if (card_idx < MAX_UNITS) {
		if (options[card_idx] >= 0)
			option = options[card_idx];
	}

	if (option > 0) {
		if (option & 0x8000)
			vortex_debug = 7;
		if (option & 0x4000)
			vortex_debug = 2;
		if (option & 0x0400)
			vp->enable_wol = 1;
	}

	print_info = (vortex_debug > 1);
	if (print_info)
		pr_info("See Documentation/networking/device_drivers/ethernet/3com/vortex.rst\n");

	pr_info("%s: 3Com %s %s at %p.\n",
	       print_name,
	       pdev ? "PCI" : "EISA",
	       vci->name,
	       ioaddr);

	dev->base_addr = (unsigned long)ioaddr;
	dev->irq = irq;
	dev->mtu = mtu;
	vp->ioaddr = ioaddr;
	vp->large_frames = mtu > 1500;
	vp->drv_flags = vci->drv_flags;
	vp->has_nway = (vci->drv_flags & HAS_NWAY) ? 1 : 0;
	vp->io_size = vci->io_size;
	vp->card_idx = card_idx;
	vp->window = -1;

	/* module list only for Compaq device */
	if (gendev == NULL) {
		compaq_net_device = dev;
	}

	/* PCI-only startup logic */
	if (pdev) {
		/* enable bus-mastering if necessary */
		if (vci->flags & PCI_USES_MASTER)
			pci_set_master(pdev);

		if (vci->drv_flags & IS_VORTEX) {
			u8 pci_latency;
			u8 new_latency = 248;

			/* Check the PCI latency value.  On the 3c590 series the latency timer
			   must be set to the maximum value to avoid data corruption that occurs
			   when the timer expires during a transfer.  This bug exists the Vortex
			   chip only. */
			pci_read_config_byte(pdev, PCI_LATENCY_TIMER, &pci_latency);
			if (pci_latency < new_latency) {
				pr_info("%s: Overriding PCI latency timer (CFLT) setting of %d, new value is %d.\n",
					print_name, pci_latency, new_latency);
				pci_write_config_byte(pdev, PCI_LATENCY_TIMER, new_latency);
			}
		}
	}

	spin_lock_init(&vp->lock);
	spin_lock_init(&vp->mii_lock);
	spin_lock_init(&vp->window_lock);
	vp->gendev = gendev;
	vp->mii.dev = dev;
	vp->mii.mdio_read = mdio_read;
	vp->mii.mdio_write = mdio_write;
	vp->mii.phy_id_mask = 0x1f;
	vp->mii.reg_num_mask = 0x1f;

	/* Makes sure rings are at least 16 byte aligned. */
	vp->rx_ring = dma_alloc_coherent(gendev, sizeof(struct boom_rx_desc) * RX_RING_SIZE
					   + sizeof(struct boom_tx_desc) * TX_RING_SIZE,
					   &vp->rx_ring_dma, GFP_KERNEL);
	retval = -ENOMEM;
	if (!vp->rx_ring)
		goto free_device;

	vp->tx_ring = (struct boom_tx_desc *)(vp->rx_ring + RX_RING_SIZE);
	vp->tx_ring_dma = vp->rx_ring_dma + sizeof(struct boom_rx_desc) * RX_RING_SIZE;

	/* if we are a PCI driver, we store info in pdev->driver_data
	 * instead of a module list */
	if (pdev)
		pci_set_drvdata(pdev, dev);
	if (edev)
		eisa_set_drvdata(edev, dev);

	vp->media_override = 7;
	if (option >= 0) {
		vp->media_override = ((option & 7) == 2)  ?  0  :  option & 15;
		if (vp->media_override != 7)
			vp->medialock = 1;
		vp->full_duplex = (option & 0x200) ? 1 : 0;
		vp->bus_master = (option & 16) ? 1 : 0;
	}

	if (global_full_duplex > 0)
		vp->full_duplex = 1;
	if (global_enable_wol > 0)
		vp->enable_wol = 1;

	if (card_idx < MAX_UNITS) {
		if (full_duplex[card_idx] > 0)
			vp->full_duplex = 1;
		if (flow_ctrl[card_idx] > 0)
			vp->flow_ctrl = 1;
		if (enable_wol[card_idx] > 0)
			vp->enable_wol = 1;
	}

	vp->mii.force_media = vp->full_duplex;
	vp->options = option;
	/* Read the station address from the EEPROM. */
	{
		int base;

		if (vci->drv_flags & EEPROM_8BIT)
			base = 0x230;
		else if (vci->drv_flags & EEPROM_OFFSET)
			base = EEPROM_Read + 0x30;
		else
			base = EEPROM_Read;

		for (i = 0; i < 0x40; i++) {
			int timer;
			window_write16(vp, base + i, 0, Wn0EepromCmd);
			/* Pause for at least 162 us. for the read to take place. */
			for (timer = 10; timer >= 0; timer--) {
				udelay(162);
				if ((window_read16(vp, 0, Wn0EepromCmd) &
				     0x8000) == 0)
					break;
			}
			eeprom[i] = window_read16(vp, 0, Wn0EepromData);
		}
	}
	for (i = 0; i < 0x18; i++)
		checksum ^= eeprom[i];
	checksum = (checksum ^ (checksum >> 8)) & 0xff;
	if (checksum != 0x00) {		/* Grrr, needless incompatible change 3Com. */
		while (i < 0x21)
			checksum ^= eeprom[i++];
		checksum = (checksum ^ (checksum >> 8)) & 0xff;
	}
	if ((checksum != 0x00) && !(vci->drv_flags & IS_TORNADO))
		pr_cont(" ***INVALID CHECKSUM %4.4x*** ", checksum);
	for (i = 0; i < 3; i++)
		((__be16 *)dev->dev_addr)[i] = htons(eeprom[i + 10]);
	if (print_info)
		pr_cont(" %pM", dev->dev_addr);
	/* Unfortunately an all zero eeprom passes the checksum and this
	   gets found in the wild in failure cases. Crypto is hard 8) */
	if (!is_valid_ether_addr(dev->dev_addr)) {
		retval = -EINVAL;
		pr_err("*** EEPROM MAC address is invalid.\n");
		goto free_ring;	/* With every pack */
	}
	for (i = 0; i < 6; i++)
		window_write8(vp, dev->dev_addr[i], 2, i);

	if (print_info)
		pr_cont(", IRQ %d\n", dev->irq);
	/* Tell them about an invalid IRQ. */
	if (dev->irq <= 0 || dev->irq >= nr_irqs)
		pr_warn(" *** Warning: IRQ %d is unlikely to work! ***\n",
			dev->irq);

	step = (window_read8(vp, 4, Wn4_NetDiag) & 0x1e) >> 1;
	if (print_info) {
		pr_info("  product code %02x%02x rev %02x.%d date %02d-%02d-%02d\n",
			eeprom[6]&0xff, eeprom[6]>>8, eeprom[0x14],
			step, (eeprom[4]>>5) & 15, eeprom[4] & 31, eeprom[4]>>9);
	}


	if (pdev && vci->drv_flags & HAS_CB_FNS) {
		unsigned short n;

		vp->cb_fn_base = pci_iomap(pdev, 2, 0);
		if (!vp->cb_fn_base) {
			retval = -ENOMEM;
			goto free_ring;
		}

		if (print_info) {
			pr_info("%s: CardBus functions mapped %16.16llx->%p\n",
				print_name,
				(unsigned long long)pci_resource_start(pdev, 2),
				vp->cb_fn_base);
		}

		n = window_read16(vp, 2, Wn2_ResetOptions) & ~0x4010;
		if (vp->drv_flags & INVERT_LED_PWR)
			n |= 0x10;
		if (vp->drv_flags & INVERT_MII_PWR)
			n |= 0x4000;
		window_write16(vp, n, 2, Wn2_ResetOptions);
		if (vp->drv_flags & WNO_XCVR_PWR) {
			window_write16(vp, 0x0800, 0, 0);
		}
	}

	/* Extract our information from the EEPROM data. */
	vp->info1 = eeprom[13];
	vp->info2 = eeprom[15];
	vp->capabilities = eeprom[16];

	if (vp->info1 & 0x8000) {
		vp->full_duplex = 1;
		if (print_info)
			pr_info("Full duplex capable\n");
	}

	{
		static const char * const ram_split[] = {"5:3", "3:1", "1:1", "3:5"};
		unsigned int config;
		vp->available_media = window_read16(vp, 3, Wn3_Options);
		if ((vp->available_media & 0xff) == 0)		/* Broken 3c916 */
			vp->available_media = 0x40;
		config = window_read32(vp, 3, Wn3_Config);
		if (print_info) {
			pr_debug("  Internal config register is %4.4x, transceivers %#x.\n",
				config, window_read16(vp, 3, Wn3_Options));
			pr_info("  %dK %s-wide RAM %s Rx:Tx split, %s%s interface.\n",
				   8 << RAM_SIZE(config),
				   RAM_WIDTH(config) ? "word" : "byte",
				   ram_split[RAM_SPLIT(config)],
				   AUTOSELECT(config) ? "autoselect/" : "",
				   XCVR(config) > XCVR_ExtMII ? "<invalid transceiver>" :
				   media_tbl[XCVR(config)].name);
		}
		vp->default_media = XCVR(config);
		if (vp->default_media == XCVR_NWAY)
			vp->has_nway = 1;
		vp->autoselect = AUTOSELECT(config);
	}

	if (vp->media_override != 7) {
		pr_info("%s:  Media override to transceiver type %d (%s).\n",
				print_name, vp->media_override,
				media_tbl[vp->media_override].name);
		dev->if_port = vp->media_override;
	} else
		dev->if_port = vp->default_media;

	if ((vp->available_media & 0x40) || (vci->drv_flags & HAS_NWAY) ||
		dev->if_port == XCVR_MII || dev->if_port == XCVR_NWAY) {
		int phy, phy_idx = 0;
		mii_preamble_required++;
		if (vp->drv_flags & EXTRA_PREAMBLE)
			mii_preamble_required++;
		mdio_sync(vp, 32);
		mdio_read(dev, 24, MII_BMSR);
		for (phy = 0; phy < 32 && phy_idx < 1; phy++) {
			int mii_status, phyx;

			/*
			 * For the 3c905CX we look at index 24 first, because it bogusly
			 * reports an external PHY at all indices
			 */
			if (phy == 0)
				phyx = 24;
			else if (phy <= 24)
				phyx = phy - 1;
			else
				phyx = phy;
			mii_status = mdio_read(dev, phyx, MII_BMSR);
			if (mii_status  &&  mii_status != 0xffff) {
				vp->phys[phy_idx++] = phyx;
				if (print_info) {
					pr_info("  MII transceiver found at address %d, status %4x.\n",
						phyx, mii_status);
				}
				if ((mii_status & 0x0040) == 0)
					mii_preamble_required++;
			}
		}
		mii_preamble_required--;
		if (phy_idx == 0) {
			pr_warn("  ***WARNING*** No MII transceivers found!\n");
			vp->phys[0] = 24;
		} else {
			vp->advertising = mdio_read(dev, vp->phys[0], MII_ADVERTISE);
			if (vp->full_duplex) {
				/* Only advertise the FD media types. */
				vp->advertising &= ~0x02A0;
				mdio_write(dev, vp->phys[0], 4, vp->advertising);
			}
		}
		vp->mii.phy_id = vp->phys[0];
	}

	if (vp->capabilities & CapBusMaster) {
		vp->full_bus_master_tx = 1;
		if (print_info) {
			pr_info("  Enabling bus-master transmits and %s receives.\n",
			(vp->info2 & 1) ? "early" : "whole-frame" );
		}
		vp->full_bus_master_rx = (vp->info2 & 1) ? 1 : 2;
		vp->bus_master = 0;		/* AKPM: vortex only */
	}

	/* The 3c59x-specific entries in the device structure. */
	if (vp->full_bus_master_tx) {
		dev->netdev_ops = &boomrang_netdev_ops;
		/* Actually, it still should work with iommu. */
		if (card_idx < MAX_UNITS &&
		    ((hw_checksums[card_idx] == -1 && (vp->drv_flags & HAS_HWCKSM)) ||
				hw_checksums[card_idx] == 1)) {
			dev->features |= NETIF_F_IP_CSUM | NETIF_F_SG;
		}
	} else
		dev->netdev_ops =  &vortex_netdev_ops;

	if (print_info) {
		pr_info("%s: scatter/gather %sabled. h/w checksums %sabled\n",
				print_name,
				(dev->features & NETIF_F_SG) ? "en":"dis",
				(dev->features & NETIF_F_IP_CSUM) ? "en":"dis");
	}

	dev->ethtool_ops = &vortex_ethtool_ops;
	dev->watchdog_timeo = (watchdog * HZ) / 1000;

	if (pdev) {
		vp->pm_state_valid = 1;
		pci_save_state(pdev);
 		acpi_set_WOL(dev);
	}
	retval = register_netdev(dev);
	if (retval == 0)
		return 0;

free_ring:
	dma_free_coherent(&pdev->dev,
		sizeof(struct boom_rx_desc) * RX_RING_SIZE +
		sizeof(struct boom_tx_desc) * TX_RING_SIZE,
		vp->rx_ring, vp->rx_ring_dma);
free_device:
	free_netdev(dev);
	pr_err(PFX "vortex_probe1 fails.  Returns %d\n", retval);
out:
	return retval;
}

static void
issue_and_wait(struct net_device *dev, int cmd)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	int i;

	iowrite16(cmd, ioaddr + EL3_CMD);
	for (i = 0; i < 2000; i++) {
		if (!(ioread16(ioaddr + EL3_STATUS) & CmdInProgress))
			return;
	}

	/* OK, that didn't work.  Do it the slow way.  One second */
	for (i = 0; i < 100000; i++) {
		if (!(ioread16(ioaddr + EL3_STATUS) & CmdInProgress)) {
			if (vortex_debug > 1)
				pr_info("%s: command 0x%04x took %d usecs\n",
					   dev->name, cmd, i * 10);
			return;
		}
		udelay(10);
	}
	pr_err("%s: command 0x%04x did not complete! Status=0x%x\n",
			   dev->name, cmd, ioread16(ioaddr + EL3_STATUS));
}

static void
vortex_set_duplex(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);

	pr_info("%s:  setting %s-duplex.\n",
		dev->name, (vp->full_duplex) ? "full" : "half");

	/* Set the full-duplex bit. */
	window_write16(vp,
		       ((vp->info1 & 0x8000) || vp->full_duplex ? 0x20 : 0) |
		       (vp->large_frames ? 0x40 : 0) |
		       ((vp->full_duplex && vp->flow_ctrl && vp->partner_flow_ctrl) ?
			0x100 : 0),
		       3, Wn3_MAC_Ctrl);
}

static void vortex_check_media(struct net_device *dev, unsigned int init)
{
	struct vortex_private *vp = netdev_priv(dev);
	unsigned int ok_to_print = 0;

	if (vortex_debug > 3)
		ok_to_print = 1;

	if (mii_check_media(&vp->mii, ok_to_print, init)) {
		vp->full_duplex = vp->mii.full_duplex;
		vortex_set_duplex(dev);
	} else if (init) {
		vortex_set_duplex(dev);
	}
}

static int
vortex_up(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	unsigned int config;
	int i, mii_reg5, err = 0;

	if (VORTEX_PCI(vp)) {
		pci_set_power_state(VORTEX_PCI(vp), PCI_D0);	/* Go active */
		if (vp->pm_state_valid)
			pci_restore_state(VORTEX_PCI(vp));
		err = pci_enable_device(VORTEX_PCI(vp));
		if (err) {
			pr_warn("%s: Could not enable device\n", dev->name);
			goto err_out;
		}
	}

	/* Before initializing select the active media port. */
	config = window_read32(vp, 3, Wn3_Config);

	if (vp->media_override != 7) {
		pr_info("%s: Media override to transceiver %d (%s).\n",
			   dev->name, vp->media_override,
			   media_tbl[vp->media_override].name);
		dev->if_port = vp->media_override;
	} else if (vp->autoselect) {
		if (vp->has_nway) {
			if (vortex_debug > 1)
				pr_info("%s: using NWAY device table, not %d\n",
								dev->name, dev->if_port);
			dev->if_port = XCVR_NWAY;
		} else {
			/* Find first available media type, starting with 100baseTx. */
			dev->if_port = XCVR_100baseTx;
			while (! (vp->available_media & media_tbl[dev->if_port].mask))
				dev->if_port = media_tbl[dev->if_port].next;
			if (vortex_debug > 1)
				pr_info("%s: first available media type: %s\n",
					dev->name, media_tbl[dev->if_port].name);
		}
	} else {
		dev->if_port = vp->default_media;
		if (vortex_debug > 1)
			pr_info("%s: using default media %s\n",
				dev->name, media_tbl[dev->if_port].name);
	}

	timer_setup(&vp->timer, vortex_timer, 0);
	mod_timer(&vp->timer, RUN_AT(media_tbl[dev->if_port].wait));

	if (vortex_debug > 1)
		pr_debug("%s: Initial media type %s.\n",
			   dev->name, media_tbl[dev->if_port].name);

	vp->full_duplex = vp->mii.force_media;
	config = BFINS(config, dev->if_port, 20, 4);
	if (vortex_debug > 6)
		pr_debug("vortex_up(): writing 0x%x to InternalConfig\n", config);
	window_write32(vp, config, 3, Wn3_Config);

	if (dev->if_port == XCVR_MII || dev->if_port == XCVR_NWAY) {
		mdio_read(dev, vp->phys[0], MII_BMSR);
		mii_reg5 = mdio_read(dev, vp->phys[0], MII_LPA);
		vp->partner_flow_ctrl = ((mii_reg5 & 0x0400) != 0);
		vp->mii.full_duplex = vp->full_duplex;

		vortex_check_media(dev, 1);
	}
	else
		vortex_set_duplex(dev);

	issue_and_wait(dev, TxReset);
	/*
	 * Don't reset the PHY - that upsets autonegotiation during DHCP operations.
	 */
	issue_and_wait(dev, RxReset|0x04);


	iowrite16(SetStatusEnb | 0x00, ioaddr + EL3_CMD);

	if (vortex_debug > 1) {
		pr_debug("%s: vortex_up() irq %d media status %4.4x.\n",
			   dev->name, dev->irq, window_read16(vp, 4, Wn4_Media));
	}

	/* Set the station address and mask in window 2 each time opened. */
	for (i = 0; i < 6; i++)
		window_write8(vp, dev->dev_addr[i], 2, i);
	for (; i < 12; i+=2)
		window_write16(vp, 0, 2, i);

	if (vp->cb_fn_base) {
		unsigned short n = window_read16(vp, 2, Wn2_ResetOptions) & ~0x4010;
		if (vp->drv_flags & INVERT_LED_PWR)
			n |= 0x10;
		if (vp->drv_flags & INVERT_MII_PWR)
			n |= 0x4000;
		window_write16(vp, n, 2, Wn2_ResetOptions);
	}

	if (dev->if_port == XCVR_10base2)
		/* Start the thinnet transceiver. We should really wait 50ms...*/
		iowrite16(StartCoax, ioaddr + EL3_CMD);
	if (dev->if_port != XCVR_NWAY) {
		window_write16(vp,
			       (window_read16(vp, 4, Wn4_Media) &
				~(Media_10TP|Media_SQE)) |
			       media_tbl[dev->if_port].media_bits,
			       4, Wn4_Media);
	}

	/* Switch to the stats window, and clear all stats by reading. */
	iowrite16(StatsDisable, ioaddr + EL3_CMD);
	for (i = 0; i < 10; i++)
		window_read8(vp, 6, i);
	window_read16(vp, 6, 10);
	window_read16(vp, 6, 12);
	/* New: On the Vortex we must also clear the BadSSD counter. */
	window_read8(vp, 4, 12);
	/* ..and on the Boomerang we enable the extra statistics bits. */
	window_write16(vp, 0x0040, 4, Wn4_NetDiag);

	if (vp->full_bus_master_rx) { /* Boomerang bus master. */
		vp->cur_rx = 0;
		/* Initialize the RxEarly register as recommended. */
		iowrite16(SetRxThreshold + (1536>>2), ioaddr + EL3_CMD);
		iowrite32(0x0020, ioaddr + PktStatus);
		iowrite32(vp->rx_ring_dma, ioaddr + UpListPtr);
	}
	if (vp->full_bus_master_tx) { 		/* Boomerang bus master Tx. */
		vp->cur_tx = vp->dirty_tx = 0;
		if (vp->drv_flags & IS_BOOMERANG)
			iowrite8(PKT_BUF_SZ>>8, ioaddr + TxFreeThreshold); /* Room for a packet. */
		/* Clear the Rx, Tx rings. */
		for (i = 0; i < RX_RING_SIZE; i++)	/* AKPM: this is done in vortex_open, too */
			vp->rx_ring[i].status = 0;
		for (i = 0; i < TX_RING_SIZE; i++)
			vp->tx_skbuff[i] = NULL;
		iowrite32(0, ioaddr + DownListPtr);
	}
	/* Set receiver mode: presumably accept b-case and phys addr only. */
	set_rx_mode(dev);
	/* enable 802.1q tagged frames */
	set_8021q_mode(dev, 1);
	iowrite16(StatsEnable, ioaddr + EL3_CMD); /* Turn on statistics. */

	iowrite16(RxEnable, ioaddr + EL3_CMD); /* Enable the receiver. */
	iowrite16(TxEnable, ioaddr + EL3_CMD); /* Enable transmitter. */
	/* Allow status bits to be seen. */
	vp->status_enable = SetStatusEnb | HostError|IntReq|StatsFull|TxComplete|
		(vp->full_bus_master_tx ? DownComplete : TxAvailable) |
		(vp->full_bus_master_rx ? UpComplete : RxComplete) |
		(vp->bus_master ? DMADone : 0);
	vp->intr_enable = SetIntrEnb | IntLatch | TxAvailable |
		(vp->full_bus_master_rx ? 0 : RxComplete) |
		StatsFull | HostError | TxComplete | IntReq
		| (vp->bus_master ? DMADone : 0) | UpComplete | DownComplete;
	iowrite16(vp->status_enable, ioaddr + EL3_CMD);
	/* Ack all pending events, and set active indicator mask. */
	iowrite16(AckIntr | IntLatch | TxAvailable | RxEarly | IntReq,
		 ioaddr + EL3_CMD);
	iowrite16(vp->intr_enable, ioaddr + EL3_CMD);
	if (vp->cb_fn_base)			/* The PCMCIA people are idiots.  */
		iowrite32(0x8000, vp->cb_fn_base + 4);
	netif_start_queue (dev);
	netdev_reset_queue(dev);
err_out:
	return err;
}

static int
vortex_open(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	int i;
	int retval;
	dma_addr_t dma;

	/* Use the now-standard shared IRQ implementation. */
	if ((retval = request_irq(dev->irq, vortex_boomerang_interrupt, IRQF_SHARED, dev->name, dev))) {
		pr_err("%s: Could not reserve IRQ %d\n", dev->name, dev->irq);
		goto err;
	}

	if (vp->full_bus_master_rx) { /* Boomerang bus master. */
		if (vortex_debug > 2)
			pr_debug("%s:  Filling in the Rx ring.\n", dev->name);
		for (i = 0; i < RX_RING_SIZE; i++) {
			struct sk_buff *skb;
			vp->rx_ring[i].next = cpu_to_le32(vp->rx_ring_dma + sizeof(struct boom_rx_desc) * (i+1));
			vp->rx_ring[i].status = 0;	/* Clear complete bit. */
			vp->rx_ring[i].length = cpu_to_le32(PKT_BUF_SZ | LAST_FRAG);

			skb = __netdev_alloc_skb(dev, PKT_BUF_SZ + NET_IP_ALIGN,
						 GFP_KERNEL);
			vp->rx_skbuff[i] = skb;
			if (skb == NULL)
				break;			/* Bad news!  */

			skb_reserve(skb, NET_IP_ALIGN);	/* Align IP on 16 byte boundaries */
			dma = dma_map_single(vp->gendev, skb->data,
					     PKT_BUF_SZ, DMA_FROM_DEVICE);
			if (dma_mapping_error(vp->gendev, dma))
				break;
			vp->rx_ring[i].addr = cpu_to_le32(dma);
		}
		if (i != RX_RING_SIZE) {
			pr_emerg("%s: no memory for rx ring\n", dev->name);
			retval = -ENOMEM;
			goto err_free_skb;
		}
		/* Wrap the ring. */
		vp->rx_ring[i-1].next = cpu_to_le32(vp->rx_ring_dma);
	}

	retval = vortex_up(dev);
	if (!retval)
		goto out;

err_free_skb:
	for (i = 0; i < RX_RING_SIZE; i++) {
		if (vp->rx_skbuff[i]) {
			dev_kfree_skb(vp->rx_skbuff[i]);
			vp->rx_skbuff[i] = NULL;
		}
	}
	free_irq(dev->irq, dev);
err:
	if (vortex_debug > 1)
		pr_err("%s: vortex_open() fails: returning %d\n", dev->name, retval);
out:
	return retval;
}

static void
vortex_timer(struct timer_list *t)
{
	struct vortex_private *vp = from_timer(vp, t, timer);
	struct net_device *dev = vp->mii.dev;
	void __iomem *ioaddr = vp->ioaddr;
	int next_tick = 60*HZ;
	int ok = 0;
	int media_status;

	if (vortex_debug > 2) {
		pr_debug("%s: Media selection timer tick happened, %s.\n",
			   dev->name, media_tbl[dev->if_port].name);
		pr_debug("dev->watchdog_timeo=%d\n", dev->watchdog_timeo);
	}

	media_status = window_read16(vp, 4, Wn4_Media);
	switch (dev->if_port) {
	case XCVR_10baseT:  case XCVR_100baseTx:  case XCVR_100baseFx:
		if (media_status & Media_LnkBeat) {
			netif_carrier_on(dev);
			ok = 1;
			if (vortex_debug > 1)
				pr_debug("%s: Media %s has link beat, %x.\n",
					   dev->name, media_tbl[dev->if_port].name, media_status);
		} else {
			netif_carrier_off(dev);
			if (vortex_debug > 1) {
				pr_debug("%s: Media %s has no link beat, %x.\n",
					   dev->name, media_tbl[dev->if_port].name, media_status);
			}
		}
		break;
	case XCVR_MII: case XCVR_NWAY:
		{
			ok = 1;
			vortex_check_media(dev, 0);
		}
		break;
	  default:					/* Other media types handled by Tx timeouts. */
		if (vortex_debug > 1)
		  pr_debug("%s: Media %s has no indication, %x.\n",
				 dev->name, media_tbl[dev->if_port].name, media_status);
		ok = 1;
	}

	if (dev->flags & IFF_SLAVE || !netif_carrier_ok(dev))
		next_tick = 5*HZ;

	if (vp->medialock)
		goto leave_media_alone;

	if (!ok) {
		unsigned int config;

		spin_lock_irq(&vp->lock);

		do {
			dev->if_port = media_tbl[dev->if_port].next;
		} while ( ! (vp->available_media & media_tbl[dev->if_port].mask));
		if (dev->if_port == XCVR_Default) { /* Go back to default. */
		  dev->if_port = vp->default_media;
		  if (vortex_debug > 1)
			pr_debug("%s: Media selection failing, using default %s port.\n",
				   dev->name, media_tbl[dev->if_port].name);
		} else {
			if (vortex_debug > 1)
				pr_debug("%s: Media selection failed, now trying %s port.\n",
					   dev->name, media_tbl[dev->if_port].name);
			next_tick = media_tbl[dev->if_port].wait;
		}
		window_write16(vp,
			       (media_status & ~(Media_10TP|Media_SQE)) |
			       media_tbl[dev->if_port].media_bits,
			       4, Wn4_Media);

		config = window_read32(vp, 3, Wn3_Config);
		config = BFINS(config, dev->if_port, 20, 4);
		window_write32(vp, config, 3, Wn3_Config);

		iowrite16(dev->if_port == XCVR_10base2 ? StartCoax : StopCoax,
			 ioaddr + EL3_CMD);
		if (vortex_debug > 1)
			pr_debug("wrote 0x%08x to Wn3_Config\n", config);
		/* AKPM: FIXME: Should reset Rx & Tx here.  P60 of 3c90xc.pdf */

		spin_unlock_irq(&vp->lock);
	}

leave_media_alone:
	if (vortex_debug > 2)
	  pr_debug("%s: Media selection timer finished, %s.\n",
			 dev->name, media_tbl[dev->if_port].name);

	mod_timer(&vp->timer, RUN_AT(next_tick));
	if (vp->deferred)
		iowrite16(FakeIntr, ioaddr + EL3_CMD);
}

static void vortex_tx_timeout(struct net_device *dev, unsigned int txqueue)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;

	pr_err("%s: transmit timed out, tx_status %2.2x status %4.4x.\n",
		   dev->name, ioread8(ioaddr + TxStatus),
		   ioread16(ioaddr + EL3_STATUS));
	pr_err("  diagnostics: net %04x media %04x dma %08x fifo %04x\n",
			window_read16(vp, 4, Wn4_NetDiag),
			window_read16(vp, 4, Wn4_Media),
			ioread32(ioaddr + PktStatus),
			window_read16(vp, 4, Wn4_FIFODiag));
	/* Slight code bloat to be user friendly. */
	if ((ioread8(ioaddr + TxStatus) & 0x88) == 0x88)
		pr_err("%s: Transmitter encountered 16 collisions --"
			   " network cable problem?\n", dev->name);
	if (ioread16(ioaddr + EL3_STATUS) & IntLatch) {
		pr_err("%s: Interrupt posted but not delivered --"
			   " IRQ blocked by another device?\n", dev->name);
		/* Bad idea here.. but we might as well handle a few events. */
		vortex_boomerang_interrupt(dev->irq, dev);
	}

	if (vortex_debug > 0)
		dump_tx_ring(dev);

	issue_and_wait(dev, TxReset);

	dev->stats.tx_errors++;
	if (vp->full_bus_master_tx) {
		pr_debug("%s: Resetting the Tx ring pointer.\n", dev->name);
		if (vp->cur_tx - vp->dirty_tx > 0  &&  ioread32(ioaddr + DownListPtr) == 0)
			iowrite32(vp->tx_ring_dma + (vp->dirty_tx % TX_RING_SIZE) * sizeof(struct boom_tx_desc),
				 ioaddr + DownListPtr);
		if (vp->cur_tx - vp->dirty_tx < TX_RING_SIZE) {
			netif_wake_queue (dev);
			netdev_reset_queue (dev);
		}
		if (vp->drv_flags & IS_BOOMERANG)
			iowrite8(PKT_BUF_SZ>>8, ioaddr + TxFreeThreshold);
		iowrite16(DownUnstall, ioaddr + EL3_CMD);
	} else {
		dev->stats.tx_dropped++;
		netif_wake_queue(dev);
		netdev_reset_queue(dev);
	}
	/* Issue Tx Enable */
	iowrite16(TxEnable, ioaddr + EL3_CMD);
	netif_trans_update(dev); /* prevent tx timeout */
}

/*
 * Handle uncommon interrupt sources.  This is a separate routine to minimize
 * the cache impact.
 */
static void
vortex_error(struct net_device *dev, int status)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	int do_tx_reset = 0, reset_mask = 0;
	unsigned char tx_status = 0;

	if (vortex_debug > 2) {
		pr_err("%s: vortex_error(), status=0x%x\n", dev->name, status);
	}

	if (status & TxComplete) {			/* Really "TxError" for us. */
		tx_status = ioread8(ioaddr + TxStatus);
		/* Presumably a tx-timeout. We must merely re-enable. */
		if (vortex_debug > 2 ||
		    (tx_status != 0x88 && vortex_debug > 0)) {
			pr_err("%s: Transmit error, Tx status register %2.2x.\n",
				   dev->name, tx_status);
			if (tx_status == 0x82) {
				pr_err("Probably a duplex mismatch.  See "
						"Documentation/networking/device_drivers/ethernet/3com/vortex.rst\n");
			}
			dump_tx_ring(dev);
		}
		if (tx_status & 0x14)  dev->stats.tx_fifo_errors++;
		if (tx_status & 0x38)  dev->stats.tx_aborted_errors++;
		if (tx_status & 0x08)  vp->xstats.tx_max_collisions++;
		iowrite8(0, ioaddr + TxStatus);
		if (tx_status & 0x30) {			/* txJabber or txUnderrun */
			do_tx_reset = 1;
		} else if ((tx_status & 0x08) && (vp->drv_flags & MAX_COLLISION_RESET))  {	/* maxCollisions */
			do_tx_reset = 1;
			reset_mask = 0x0108;		/* Reset interface logic, but not download logic */
		} else {				/* Merely re-enable the transmitter. */
			iowrite16(TxEnable, ioaddr + EL3_CMD);
		}
	}

	if (status & RxEarly)				/* Rx early is unused. */
		iowrite16(AckIntr | RxEarly, ioaddr + EL3_CMD);

	if (status & StatsFull) {			/* Empty statistics. */
		static int DoneDidThat;
		if (vortex_debug > 4)
			pr_debug("%s: Updating stats.\n", dev->name);
		update_stats(ioaddr, dev);
		/* HACK: Disable statistics as an interrupt source. */
		/* This occurs when we have the wrong media type! */
		if (DoneDidThat == 0  &&
			ioread16(ioaddr + EL3_STATUS) & StatsFull) {
			pr_warn("%s: Updating statistics failed, disabling stats as an interrupt source\n",
				dev->name);
			iowrite16(SetIntrEnb |
				  (window_read16(vp, 5, 10) & ~StatsFull),
				  ioaddr + EL3_CMD);
			vp->intr_enable &= ~StatsFull;
			DoneDidThat++;
		}
	}
	if (status & IntReq) {		/* Restore all interrupt sources.  */
		iowrite16(vp->status_enable, ioaddr + EL3_CMD);
		iowrite16(vp->intr_enable, ioaddr + EL3_CMD);
	}
	if (status & HostError) {
		u16 fifo_diag;
		fifo_diag = window_read16(vp, 4, Wn4_FIFODiag);
		pr_err("%s: Host error, FIFO diagnostic register %4.4x.\n",
			   dev->name, fifo_diag);
		/* Adapter failure requires Tx/Rx reset and reinit. */
		if (vp->full_bus_master_tx) {
			int bus_status = ioread32(ioaddr + PktStatus);
			/* 0x80000000 PCI master abort. */
			/* 0x40000000 PCI target abort. */
			if (vortex_debug)
				pr_err("%s: PCI bus error, bus status %8.8x\n", dev->name, bus_status);

			/* In this case, blow the card away */
			/* Must not enter D3 or we can't legally issue the reset! */
			vortex_down(dev, 0);
			issue_and_wait(dev, TotalReset | 0xff);
			vortex_up(dev);		/* AKPM: bug.  vortex_up() assumes that the rx ring is full. It may not be. */
		} else if (fifo_diag & 0x0400)
			do_tx_reset = 1;
		if (fifo_diag & 0x3000) {
			/* Reset Rx fifo and upload logic */
			issue_and_wait(dev, RxReset|0x07);
			/* Set the Rx filter to the current state. */
			set_rx_mode(dev);
			/* enable 802.1q VLAN tagged frames */
			set_8021q_mode(dev, 1);
			iowrite16(RxEnable, ioaddr + EL3_CMD); /* Re-enable the receiver. */
			iowrite16(AckIntr | HostError, ioaddr + EL3_CMD);
		}
	}

	if (do_tx_reset) {
		issue_and_wait(dev, TxReset|reset_mask);
		iowrite16(TxEnable, ioaddr + EL3_CMD);
		if (!vp->full_bus_master_tx)
			netif_wake_queue(dev);
	}
}

static netdev_tx_t
vortex_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	int skblen = skb->len;

	/* Put out the doubleword header... */
	iowrite32(skb->len, ioaddr + TX_FIFO);
	if (vp->bus_master) {
		/* Set the bus-master controller to transfer the packet. */
		int len = (skb->len + 3) & ~3;
		vp->tx_skb_dma = dma_map_single(vp->gendev, skb->data, len,
						DMA_TO_DEVICE);
		if (dma_mapping_error(vp->gendev, vp->tx_skb_dma)) {
			dev_kfree_skb_any(skb);
			dev->stats.tx_dropped++;
			return NETDEV_TX_OK;
		}

		spin_lock_irq(&vp->window_lock);
		window_set(vp, 7);
		iowrite32(vp->tx_skb_dma, ioaddr + Wn7_MasterAddr);
		iowrite16(len, ioaddr + Wn7_MasterLen);
		spin_unlock_irq(&vp->window_lock);
		vp->tx_skb = skb;
		skb_tx_timestamp(skb);
		iowrite16(StartDMADown, ioaddr + EL3_CMD);
		/* netif_wake_queue() will be called at the DMADone interrupt. */
	} else {
		/* ... and the packet rounded to a doubleword. */
		skb_tx_timestamp(skb);
		iowrite32_rep(ioaddr + TX_FIFO, skb->data, (skb->len + 3) >> 2);
		dev_consume_skb_any (skb);
		if (ioread16(ioaddr + TxFree) > 1536) {
			netif_start_queue (dev);	/* AKPM: redundant? */
		} else {
			/* Interrupt us when the FIFO has room for max-sized packet. */
			netif_stop_queue(dev);
			iowrite16(SetTxThreshold + (1536>>2), ioaddr + EL3_CMD);
		}
	}

	netdev_sent_queue(dev, skblen);

	/* Clear the Tx status stack. */
	{
		int tx_status;
		int i = 32;

		while (--i > 0	&&	(tx_status = ioread8(ioaddr + TxStatus)) > 0) {
			if (tx_status & 0x3C) {		/* A Tx-disabling error occurred.  */
				if (vortex_debug > 2)
				  pr_debug("%s: Tx error, status %2.2x.\n",
						 dev->name, tx_status);
				if (tx_status & 0x04) dev->stats.tx_fifo_errors++;
				if (tx_status & 0x38) dev->stats.tx_aborted_errors++;
				if (tx_status & 0x30) {
					issue_and_wait(dev, TxReset);
				}
				iowrite16(TxEnable, ioaddr + EL3_CMD);
			}
			iowrite8(0x00, ioaddr + TxStatus); /* Pop the status stack. */
		}
	}
	return NETDEV_TX_OK;
}

static netdev_tx_t
boomerang_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	/* Calculate the next Tx descriptor entry. */
	int entry = vp->cur_tx % TX_RING_SIZE;
	int skblen = skb->len;
	struct boom_tx_desc *prev_entry = &vp->tx_ring[(vp->cur_tx-1) % TX_RING_SIZE];
	unsigned long flags;
	dma_addr_t dma_addr;

	if (vortex_debug > 6) {
		pr_debug("boomerang_start_xmit()\n");
		pr_debug("%s: Trying to send a packet, Tx index %d.\n",
			   dev->name, vp->cur_tx);
	}

	/*
	 * We can't allow a recursion from our interrupt handler back into the
	 * tx routine, as they take the same spin lock, and that causes
	 * deadlock.  Just return NETDEV_TX_BUSY and let the stack try again in
	 * a bit
	 */
	if (vp->handling_irq)
		return NETDEV_TX_BUSY;

	if (vp->cur_tx - vp->dirty_tx >= TX_RING_SIZE) {
		if (vortex_debug > 0)
			pr_warn("%s: BUG! Tx Ring full, refusing to send buffer\n",
				dev->name);
		netif_stop_queue(dev);
		return NETDEV_TX_BUSY;
	}

	vp->tx_skbuff[entry] = skb;

	vp->tx_ring[entry].next = 0;
#if DO_ZEROCOPY
	if (skb->ip_summed != CHECKSUM_PARTIAL)
			vp->tx_ring[entry].status = cpu_to_le32(skb->len | TxIntrUploaded);
	else
			vp->tx_ring[entry].status = cpu_to_le32(skb->len | TxIntrUploaded | AddTCPChksum | AddUDPChksum);

	if (!skb_shinfo(skb)->nr_frags) {
		dma_addr = dma_map_single(vp->gendev, skb->data, skb->len,
					  DMA_TO_DEVICE);
		if (dma_mapping_error(vp->gendev, dma_addr))
			goto out_dma_err;

		vp->tx_ring[entry].frag[0].addr = cpu_to_le32(dma_addr);
		vp->tx_ring[entry].frag[0].length = cpu_to_le32(skb->len | LAST_FRAG);
	} else {
		int i;

		dma_addr = dma_map_single(vp->gendev, skb->data,
					  skb_headlen(skb), DMA_TO_DEVICE);
		if (dma_mapping_error(vp->gendev, dma_addr))
			goto out_dma_err;

		vp->tx_ring[entry].frag[0].addr = cpu_to_le32(dma_addr);
		vp->tx_ring[entry].frag[0].length = cpu_to_le32(skb_headlen(skb));

		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

			dma_addr = skb_frag_dma_map(vp->gendev, frag,
						    0,
						    skb_frag_size(frag),
						    DMA_TO_DEVICE);
			if (dma_mapping_error(vp->gendev, dma_addr)) {
				for(i = i-1; i >= 0; i--)
					dma_unmap_page(vp->gendev,
						       le32_to_cpu(vp->tx_ring[entry].frag[i+1].addr),
						       le32_to_cpu(vp->tx_ring[entry].frag[i+1].length),
						       DMA_TO_DEVICE);

				dma_unmap_single(vp->gendev,
						 le32_to_cpu(vp->tx_ring[entry].frag[0].addr),
						 le32_to_cpu(vp->tx_ring[entry].frag[0].length),
						 DMA_TO_DEVICE);

				goto out_dma_err;
			}

			vp->tx_ring[entry].frag[i+1].addr =
						cpu_to_le32(dma_addr);

			if (i == skb_shinfo(skb)->nr_frags-1)
					vp->tx_ring[entry].frag[i+1].length = cpu_to_le32(skb_frag_size(frag)|LAST_FRAG);
			else
					vp->tx_ring[entry].frag[i+1].length = cpu_to_le32(skb_frag_size(frag));
		}
	}
#else
	dma_addr = dma_map_single(vp->gendev, skb->data, skb->len, DMA_TO_DEVICE);
	if (dma_mapping_error(vp->gendev, dma_addr))
		goto out_dma_err;
	vp->tx_ring[entry].addr = cpu_to_le32(dma_addr);
	vp->tx_ring[entry].length = cpu_to_le32(skb->len | LAST_FRAG);
	vp->tx_ring[entry].status = cpu_to_le32(skb->len | TxIntrUploaded);
#endif

	spin_lock_irqsave(&vp->lock, flags);
	/* Wait for the stall to complete. */
	issue_and_wait(dev, DownStall);
	prev_entry->next = cpu_to_le32(vp->tx_ring_dma + entry * sizeof(struct boom_tx_desc));
	if (ioread32(ioaddr + DownListPtr) == 0) {
		iowrite32(vp->tx_ring_dma + entry * sizeof(struct boom_tx_desc), ioaddr + DownListPtr);
		vp->queued_packet++;
	}

	vp->cur_tx++;
	netdev_sent_queue(dev, skblen);

	if (vp->cur_tx - vp->dirty_tx > TX_RING_SIZE - 1) {
		netif_stop_queue (dev);
	} else {					/* Clear previous interrupt enable. */
#if defined(tx_interrupt_mitigation)
		/* Dubious. If in boomeang_interrupt "faster" cyclone ifdef
		 * were selected, this would corrupt DN_COMPLETE. No?
		 */
		prev_entry->status &= cpu_to_le32(~TxIntrUploaded);
#endif
	}
	skb_tx_timestamp(skb);
	iowrite16(DownUnstall, ioaddr + EL3_CMD);
	spin_unlock_irqrestore(&vp->lock, flags);
out:
	return NETDEV_TX_OK;
out_dma_err:
	dev_err(vp->gendev, "Error mapping dma buffer\n");
	goto out;
}

/* The interrupt handler does all of the Rx thread work and cleans up
   after the Tx thread. */

/*
 * This is the ISR for the vortex series chips.
 * full_bus_master_tx == 0 && full_bus_master_rx == 0
 */

static irqreturn_t
_vortex_interrupt(int irq, struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr;
	int status;
	int work_done = max_interrupt_work;
	int handled = 0;
	unsigned int bytes_compl = 0, pkts_compl = 0;

	ioaddr = vp->ioaddr;

	status = ioread16(ioaddr + EL3_STATUS);

	if (vortex_debug > 6)
		pr_debug("vortex_interrupt(). status=0x%4x\n", status);

	if ((status & IntLatch) == 0)
		goto handler_exit;		/* No interrupt: shared IRQs cause this */
	handled = 1;

	if (status & IntReq) {
		status |= vp->deferred;
		vp->deferred = 0;
	}

	if (status == 0xffff)		/* h/w no longer present (hotplug)? */
		goto handler_exit;

	if (vortex_debug > 4)
		pr_debug("%s: interrupt, status %4.4x, latency %d ticks.\n",
			   dev->name, status, ioread8(ioaddr + Timer));

	spin_lock(&vp->window_lock);
	window_set(vp, 7);

	do {
		if (vortex_debug > 5)
				pr_debug("%s: In interrupt loop, status %4.4x.\n",
					   dev->name, status);
		if (status & RxComplete)
			vortex_rx(dev);

		if (status & TxAvailable) {
			if (vortex_debug > 5)
				pr_debug("	TX room bit was handled.\n");
			/* There's room in the FIFO for a full-sized packet. */
			iowrite16(AckIntr | TxAvailable, ioaddr + EL3_CMD);
			netif_wake_queue (dev);
		}

		if (status & DMADone) {
			if (ioread16(ioaddr + Wn7_MasterStatus) & 0x1000) {
				iowrite16(0x1000, ioaddr + Wn7_MasterStatus); /* Ack the event. */
				dma_unmap_single(vp->gendev, vp->tx_skb_dma, (vp->tx_skb->len + 3) & ~3, DMA_TO_DEVICE);
				pkts_compl++;
				bytes_compl += vp->tx_skb->len;
				dev_consume_skb_irq(vp->tx_skb); /* Release the transferred buffer */
				if (ioread16(ioaddr + TxFree) > 1536) {
					/*
					 * AKPM: FIXME: I don't think we need this.  If the queue was stopped due to
					 * insufficient FIFO room, the TxAvailable test will succeed and call
					 * netif_wake_queue()
					 */
					netif_wake_queue(dev);
				} else { /* Interrupt when FIFO has room for max-sized packet. */
					iowrite16(SetTxThreshold + (1536>>2), ioaddr + EL3_CMD);
					netif_stop_queue(dev);
				}
			}
		}
		/* Check for all uncommon interrupts at once. */
		if (status & (HostError | RxEarly | StatsFull | TxComplete | IntReq)) {
			if (status == 0xffff)
				break;
			if (status & RxEarly)
				vortex_rx(dev);
			spin_unlock(&vp->window_lock);
			vortex_error(dev, status);
			spin_lock(&vp->window_lock);
			window_set(vp, 7);
		}

		if (--work_done < 0) {
			pr_warn("%s: Too much work in interrupt, status %4.4x\n",
				dev->name, status);
			/* Disable all pending interrupts. */
			do {
				vp->deferred |= status;
				iowrite16(SetStatusEnb | (~vp->deferred & vp->status_enable),
					 ioaddr + EL3_CMD);
				iowrite16(AckIntr | (vp->deferred & 0x7ff), ioaddr + EL3_CMD);
			} while ((status = ioread16(ioaddr + EL3_CMD)) & IntLatch);
			/* The timer will reenable interrupts. */
			mod_timer(&vp->timer, jiffies + 1*HZ);
			break;
		}
		/* Acknowledge the IRQ. */
		iowrite16(AckIntr | IntReq | IntLatch, ioaddr + EL3_CMD);
	} while ((status = ioread16(ioaddr + EL3_STATUS)) & (IntLatch | RxComplete));

	netdev_completed_queue(dev, pkts_compl, bytes_compl);
	spin_unlock(&vp->window_lock);

	if (vortex_debug > 4)
		pr_debug("%s: exiting interrupt, status %4.4x.\n",
			   dev->name, status);
handler_exit:
	return IRQ_RETVAL(handled);
}

/*
 * This is the ISR for the boomerang series chips.
 * full_bus_master_tx == 1 && full_bus_master_rx == 1
 */

static irqreturn_t
_boomerang_interrupt(int irq, struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr;
	int status;
	int work_done = max_interrupt_work;
	int handled = 0;
	unsigned int bytes_compl = 0, pkts_compl = 0;

	ioaddr = vp->ioaddr;

	vp->handling_irq = 1;

	status = ioread16(ioaddr + EL3_STATUS);

	if (vortex_debug > 6)
		pr_debug("boomerang_interrupt. status=0x%4x\n", status);

	if ((status & IntLatch) == 0)
		goto handler_exit;		/* No interrupt: shared IRQs can cause this */
	handled = 1;

	if (status == 0xffff) {		/* h/w no longer present (hotplug)? */
		if (vortex_debug > 1)
			pr_debug("boomerang_interrupt(1): status = 0xffff\n");
		goto handler_exit;
	}

	if (status & IntReq) {
		status |= vp->deferred;
		vp->deferred = 0;
	}

	if (vortex_debug > 4)
		pr_debug("%s: interrupt, status %4.4x, latency %d ticks.\n",
			   dev->name, status, ioread8(ioaddr + Timer));
	do {
		if (vortex_debug > 5)
				pr_debug("%s: In interrupt loop, status %4.4x.\n",
					   dev->name, status);
		if (status & UpComplete) {
			iowrite16(AckIntr | UpComplete, ioaddr + EL3_CMD);
			if (vortex_debug > 5)
				pr_debug("boomerang_interrupt->boomerang_rx\n");
			boomerang_rx(dev);
		}

		if (status & DownComplete) {
			unsigned int dirty_tx = vp->dirty_tx;

			iowrite16(AckIntr | DownComplete, ioaddr + EL3_CMD);
			while (vp->cur_tx - dirty_tx > 0) {
				int entry = dirty_tx % TX_RING_SIZE;
#if 1	/* AKPM: the latter is faster, but cyclone-only */
				if (ioread32(ioaddr + DownListPtr) ==
					vp->tx_ring_dma + entry * sizeof(struct boom_tx_desc))
					break;			/* It still hasn't been processed. */
#else
				if ((vp->tx_ring[entry].status & DN_COMPLETE) == 0)
					break;			/* It still hasn't been processed. */
#endif

				if (vp->tx_skbuff[entry]) {
					struct sk_buff *skb = vp->tx_skbuff[entry];
#if DO_ZEROCOPY
					int i;
					dma_unmap_single(vp->gendev,
							le32_to_cpu(vp->tx_ring[entry].frag[0].addr),
							le32_to_cpu(vp->tx_ring[entry].frag[0].length)&0xFFF,
							DMA_TO_DEVICE);

					for (i=1; i<=skb_shinfo(skb)->nr_frags; i++)
							dma_unmap_page(vp->gendev,
											 le32_to_cpu(vp->tx_ring[entry].frag[i].addr),
											 le32_to_cpu(vp->tx_ring[entry].frag[i].length)&0xFFF,
											 DMA_TO_DEVICE);
#else
					dma_unmap_single(vp->gendev,
						le32_to_cpu(vp->tx_ring[entry].addr), skb->len, DMA_TO_DEVICE);
#endif
					pkts_compl++;
					bytes_compl += skb->len;
					dev_consume_skb_irq(skb);
					vp->tx_skbuff[entry] = NULL;
				} else {
					pr_debug("boomerang_interrupt: no skb!\n");
				}
				/* dev->stats.tx_packets++;  Counted below. */
				dirty_tx++;
			}
			vp->dirty_tx = dirty_tx;
			if (vp->cur_tx - dirty_tx <= TX_RING_SIZE - 1) {
				if (vortex_debug > 6)
					pr_debug("boomerang_interrupt: wake queue\n");
				netif_wake_queue (dev);
			}
		}

		/* Check for all uncommon interrupts at once. */
		if (status & (HostError | RxEarly | StatsFull | TxComplete | IntReq))
			vortex_error(dev, status);

		if (--work_done < 0) {
			pr_warn("%s: Too much work in interrupt, status %4.4x\n",
				dev->name, status);
			/* Disable all pending interrupts. */
			do {
				vp->deferred |= status;
				iowrite16(SetStatusEnb | (~vp->deferred & vp->status_enable),
					 ioaddr + EL3_CMD);
				iowrite16(AckIntr | (vp->deferred & 0x7ff), ioaddr + EL3_CMD);
			} while ((status = ioread16(ioaddr + EL3_CMD)) & IntLatch);
			/* The timer will reenable interrupts. */
			mod_timer(&vp->timer, jiffies + 1*HZ);
			break;
		}
		/* Acknowledge the IRQ. */
		iowrite16(AckIntr | IntReq | IntLatch, ioaddr + EL3_CMD);
		if (vp->cb_fn_base)			/* The PCMCIA people are idiots.  */
			iowrite32(0x8000, vp->cb_fn_base + 4);

	} while ((status = ioread16(ioaddr + EL3_STATUS)) & IntLatch);
	netdev_completed_queue(dev, pkts_compl, bytes_compl);

	if (vortex_debug > 4)
		pr_debug("%s: exiting interrupt, status %4.4x.\n",
			   dev->name, status);
handler_exit:
	vp->handling_irq = 0;
	return IRQ_RETVAL(handled);
}

static irqreturn_t
vortex_boomerang_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct vortex_private *vp = netdev_priv(dev);
	unsigned long flags;
	irqreturn_t ret;

	spin_lock_irqsave(&vp->lock, flags);

	if (vp->full_bus_master_rx)
		ret = _boomerang_interrupt(dev->irq, dev);
	else
		ret = _vortex_interrupt(dev->irq, dev);

	spin_unlock_irqrestore(&vp->lock, flags);

	return ret;
}

static int vortex_rx(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	int i;
	short rx_status;

	if (vortex_debug > 5)
		pr_debug("vortex_rx(): status %4.4x, rx_status %4.4x.\n",
			   ioread16(ioaddr+EL3_STATUS), ioread16(ioaddr+RxStatus));
	while ((rx_status = ioread16(ioaddr + RxStatus)) > 0) {
		if (rx_status & 0x4000) { /* Error, update stats. */
			unsigned char rx_error = ioread8(ioaddr + RxErrors);
			if (vortex_debug > 2)
				pr_debug(" Rx error: status %2.2x.\n", rx_error);
			dev->stats.rx_errors++;
			if (rx_error & 0x01)  dev->stats.rx_over_errors++;
			if (rx_error & 0x02)  dev->stats.rx_length_errors++;
			if (rx_error & 0x04)  dev->stats.rx_frame_errors++;
			if (rx_error & 0x08)  dev->stats.rx_crc_errors++;
			if (rx_error & 0x10)  dev->stats.rx_length_errors++;
		} else {
			/* The packet length: up to 4.5K!. */
			int pkt_len = rx_status & 0x1fff;
			struct sk_buff *skb;

			skb = netdev_alloc_skb(dev, pkt_len + 5);
			if (vortex_debug > 4)
				pr_debug("Receiving packet size %d status %4.4x.\n",
					   pkt_len, rx_status);
			if (skb != NULL) {
				skb_reserve(skb, 2);	/* Align IP on 16 byte boundaries */
				/* 'skb_put()' points to the start of sk_buff data area. */
				if (vp->bus_master &&
					! (ioread16(ioaddr + Wn7_MasterStatus) & 0x8000)) {
					dma_addr_t dma = dma_map_single(vp->gendev, skb_put(skb, pkt_len),
									   pkt_len, DMA_FROM_DEVICE);
					iowrite32(dma, ioaddr + Wn7_MasterAddr);
					iowrite16((skb->len + 3) & ~3, ioaddr + Wn7_MasterLen);
					iowrite16(StartDMAUp, ioaddr + EL3_CMD);
					while (ioread16(ioaddr + Wn7_MasterStatus) & 0x8000)
						;
					dma_unmap_single(vp->gendev, dma, pkt_len, DMA_FROM_DEVICE);
				} else {
					ioread32_rep(ioaddr + RX_FIFO,
					             skb_put(skb, pkt_len),
						     (pkt_len + 3) >> 2);
				}
				iowrite16(RxDiscard, ioaddr + EL3_CMD); /* Pop top Rx packet. */
				skb->protocol = eth_type_trans(skb, dev);
				netif_rx(skb);
				dev->stats.rx_packets++;
				/* Wait a limited time to go to next packet. */
				for (i = 200; i >= 0; i--)
					if ( ! (ioread16(ioaddr + EL3_STATUS) & CmdInProgress))
						break;
				continue;
			} else if (vortex_debug > 0)
				pr_notice("%s: No memory to allocate a sk_buff of size %d.\n",
					dev->name, pkt_len);
			dev->stats.rx_dropped++;
		}
		issue_and_wait(dev, RxDiscard);
	}

	return 0;
}

static int
boomerang_rx(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	int entry = vp->cur_rx % RX_RING_SIZE;
	void __iomem *ioaddr = vp->ioaddr;
	int rx_status;
	int rx_work_limit = RX_RING_SIZE;

	if (vortex_debug > 5)
		pr_debug("boomerang_rx(): status %4.4x\n", ioread16(ioaddr+EL3_STATUS));

	while ((rx_status = le32_to_cpu(vp->rx_ring[entry].status)) & RxDComplete){
		if (--rx_work_limit < 0)
			break;
		if (rx_status & RxDError) { /* Error, update stats. */
			unsigned char rx_error = rx_status >> 16;
			if (vortex_debug > 2)
				pr_debug(" Rx error: status %2.2x.\n", rx_error);
			dev->stats.rx_errors++;
			if (rx_error & 0x01)  dev->stats.rx_over_errors++;
			if (rx_error & 0x02)  dev->stats.rx_length_errors++;
			if (rx_error & 0x04)  dev->stats.rx_frame_errors++;
			if (rx_error & 0x08)  dev->stats.rx_crc_errors++;
			if (rx_error & 0x10)  dev->stats.rx_length_errors++;
		} else {
			/* The packet length: up to 4.5K!. */
			int pkt_len = rx_status & 0x1fff;
			struct sk_buff *skb, *newskb;
			dma_addr_t newdma;
			dma_addr_t dma = le32_to_cpu(vp->rx_ring[entry].addr);

			if (vortex_debug > 4)
				pr_debug("Receiving packet size %d status %4.4x.\n",
					   pkt_len, rx_status);

			/* Check if the packet is long enough to just accept without
			   copying to a properly sized skbuff. */
			if (pkt_len < rx_copybreak &&
			    (skb = netdev_alloc_skb(dev, pkt_len + 2)) != NULL) {
				skb_reserve(skb, 2);	/* Align IP on 16 byte boundaries */
				dma_sync_single_for_cpu(vp->gendev, dma, PKT_BUF_SZ, DMA_FROM_DEVICE);
				/* 'skb_put()' points to the start of sk_buff data area. */
				skb_put_data(skb, vp->rx_skbuff[entry]->data,
					     pkt_len);
				dma_sync_single_for_device(vp->gendev, dma, PKT_BUF_SZ, DMA_FROM_DEVICE);
				vp->rx_copy++;
			} else {
				/* Pre-allocate the replacement skb.  If it or its
				 * mapping fails then recycle the buffer thats already
				 * in place
				 */
				newskb = netdev_alloc_skb_ip_align(dev, PKT_BUF_SZ);
				if (!newskb) {
					dev->stats.rx_dropped++;
					goto clear_complete;
				}
				newdma = dma_map_single(vp->gendev, newskb->data,
							PKT_BUF_SZ, DMA_FROM_DEVICE);
				if (dma_mapping_error(vp->gendev, newdma)) {
					dev->stats.rx_dropped++;
					consume_skb(newskb);
					goto clear_complete;
				}

				/* Pass up the skbuff already on the Rx ring. */
				skb = vp->rx_skbuff[entry];
				vp->rx_skbuff[entry] = newskb;
				vp->rx_ring[entry].addr = cpu_to_le32(newdma);
				skb_put(skb, pkt_len);
				dma_unmap_single(vp->gendev, dma, PKT_BUF_SZ, DMA_FROM_DEVICE);
				vp->rx_nocopy++;
			}
			skb->protocol = eth_type_trans(skb, dev);
			{					/* Use hardware checksum info. */
				int csum_bits = rx_status & 0xee000000;
				if (csum_bits &&
					(csum_bits == (IPChksumValid | TCPChksumValid) ||
					 csum_bits == (IPChksumValid | UDPChksumValid))) {
					skb->ip_summed = CHECKSUM_UNNECESSARY;
					vp->rx_csumhits++;
				}
			}
			netif_rx(skb);
			dev->stats.rx_packets++;
		}

clear_complete:
		vp->rx_ring[entry].status = 0;	/* Clear complete bit. */
		iowrite16(UpUnstall, ioaddr + EL3_CMD);
		entry = (++vp->cur_rx) % RX_RING_SIZE;
	}
	return 0;
}

static void
vortex_down(struct net_device *dev, int final_down)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;

	netdev_reset_queue(dev);
	netif_stop_queue(dev);

	del_timer_sync(&vp->timer);

	/* Turn off statistics ASAP.  We update dev->stats below. */
	iowrite16(StatsDisable, ioaddr + EL3_CMD);

	/* Disable the receiver and transmitter. */
	iowrite16(RxDisable, ioaddr + EL3_CMD);
	iowrite16(TxDisable, ioaddr + EL3_CMD);

	/* Disable receiving 802.1q tagged frames */
	set_8021q_mode(dev, 0);

	if (dev->if_port == XCVR_10base2)
		/* Turn off thinnet power.  Green! */
		iowrite16(StopCoax, ioaddr + EL3_CMD);

	iowrite16(SetIntrEnb | 0x0000, ioaddr + EL3_CMD);

	update_stats(ioaddr, dev);
	if (vp->full_bus_master_rx)
		iowrite32(0, ioaddr + UpListPtr);
	if (vp->full_bus_master_tx)
		iowrite32(0, ioaddr + DownListPtr);

	if (final_down && VORTEX_PCI(vp)) {
		vp->pm_state_valid = 1;
		pci_save_state(VORTEX_PCI(vp));
		acpi_set_WOL(dev);
	}
}

static int
vortex_close(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	int i;

	if (netif_device_present(dev))
		vortex_down(dev, 1);

	if (vortex_debug > 1) {
		pr_debug("%s: vortex_close() status %4.4x, Tx status %2.2x.\n",
			   dev->name, ioread16(ioaddr + EL3_STATUS), ioread8(ioaddr + TxStatus));
		pr_debug("%s: vortex close stats: rx_nocopy %d rx_copy %d"
			   " tx_queued %d Rx pre-checksummed %d.\n",
			   dev->name, vp->rx_nocopy, vp->rx_copy, vp->queued_packet, vp->rx_csumhits);
	}

#if DO_ZEROCOPY
	if (vp->rx_csumhits &&
	    (vp->drv_flags & HAS_HWCKSM) == 0 &&
	    (vp->card_idx >= MAX_UNITS || hw_checksums[vp->card_idx] == -1)) {
		pr_warn("%s supports hardware checksums, and we're not using them!\n",
			dev->name);
	}
#endif

	free_irq(dev->irq, dev);

	if (vp->full_bus_master_rx) { /* Free Boomerang bus master Rx buffers. */
		for (i = 0; i < RX_RING_SIZE; i++)
			if (vp->rx_skbuff[i]) {
				dma_unmap_single(vp->gendev, le32_to_cpu(vp->rx_ring[i].addr),
									PKT_BUF_SZ, DMA_FROM_DEVICE);
				dev_kfree_skb(vp->rx_skbuff[i]);
				vp->rx_skbuff[i] = NULL;
			}
	}
	if (vp->full_bus_master_tx) { /* Free Boomerang bus master Tx buffers. */
		for (i = 0; i < TX_RING_SIZE; i++) {
			if (vp->tx_skbuff[i]) {
				struct sk_buff *skb = vp->tx_skbuff[i];
#if DO_ZEROCOPY
				int k;

				for (k=0; k<=skb_shinfo(skb)->nr_frags; k++)
						dma_unmap_single(vp->gendev,
										 le32_to_cpu(vp->tx_ring[i].frag[k].addr),
										 le32_to_cpu(vp->tx_ring[i].frag[k].length)&0xFFF,
										 DMA_TO_DEVICE);
#else
				dma_unmap_single(vp->gendev, le32_to_cpu(vp->tx_ring[i].addr), skb->len, DMA_TO_DEVICE);
#endif
				dev_kfree_skb(skb);
				vp->tx_skbuff[i] = NULL;
			}
		}
	}

	return 0;
}

static void
dump_tx_ring(struct net_device *dev)
{
	if (vortex_debug > 0) {
	struct vortex_private *vp = netdev_priv(dev);
		void __iomem *ioaddr = vp->ioaddr;

		if (vp->full_bus_master_tx) {
			int i;
			int stalled = ioread32(ioaddr + PktStatus) & 0x04;	/* Possible racy. But it's only debug stuff */

			pr_err("  Flags; bus-master %d, dirty %d(%d) current %d(%d)\n",
					vp->full_bus_master_tx,
					vp->dirty_tx, vp->dirty_tx % TX_RING_SIZE,
					vp->cur_tx, vp->cur_tx % TX_RING_SIZE);
			pr_err("  Transmit list %8.8x vs. %p.\n",
				   ioread32(ioaddr + DownListPtr),
				   &vp->tx_ring[vp->dirty_tx % TX_RING_SIZE]);
			issue_and_wait(dev, DownStall);
			for (i = 0; i < TX_RING_SIZE; i++) {
				unsigned int length;

#if DO_ZEROCOPY
				length = le32_to_cpu(vp->tx_ring[i].frag[0].length);
#else
				length = le32_to_cpu(vp->tx_ring[i].length);
#endif
				pr_err("  %d: @%p  length %8.8x status %8.8x\n",
					   i, &vp->tx_ring[i], length,
					   le32_to_cpu(vp->tx_ring[i].status));
			}
			if (!stalled)
				iowrite16(DownUnstall, ioaddr + EL3_CMD);
		}
	}
}

static struct net_device_stats *vortex_get_stats(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	unsigned long flags;

	if (netif_device_present(dev)) {	/* AKPM: Used to be netif_running */
		spin_lock_irqsave (&vp->lock, flags);
		update_stats(ioaddr, dev);
		spin_unlock_irqrestore (&vp->lock, flags);
	}
	return &dev->stats;
}

/*  Update statistics.
	Unlike with the EL3 we need not worry about interrupts changing
	the window setting from underneath us, but we must still guard
	against a race condition with a StatsUpdate interrupt updating the
	table.  This is done by checking that the ASM (!) code generated uses
	atomic updates with '+='.
	*/
static void update_stats(void __iomem *ioaddr, struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);

	/* Unlike the 3c5x9 we need not turn off stats updates while reading. */
	/* Switch to the stats window, and read everything. */
	dev->stats.tx_carrier_errors		+= window_read8(vp, 6, 0);
	dev->stats.tx_heartbeat_errors		+= window_read8(vp, 6, 1);
	dev->stats.tx_window_errors		+= window_read8(vp, 6, 4);
	dev->stats.rx_fifo_errors		+= window_read8(vp, 6, 5);
	dev->stats.tx_packets			+= window_read8(vp, 6, 6);
	dev->stats.tx_packets			+= (window_read8(vp, 6, 9) &
						    0x30) << 4;
	/* Rx packets	*/			window_read8(vp, 6, 7);   /* Must read to clear */
	/* Don't bother with register 9, an extension of registers 6&7.
	   If we do use the 6&7 values the atomic update assumption above
	   is invalid. */
	dev->stats.rx_bytes 			+= window_read16(vp, 6, 10);
	dev->stats.tx_bytes 			+= window_read16(vp, 6, 12);
	/* Extra stats for get_ethtool_stats() */
	vp->xstats.tx_multiple_collisions	+= window_read8(vp, 6, 2);
	vp->xstats.tx_single_collisions         += window_read8(vp, 6, 3);
	vp->xstats.tx_deferred			+= window_read8(vp, 6, 8);
	vp->xstats.rx_bad_ssd			+= window_read8(vp, 4, 12);

	dev->stats.collisions = vp->xstats.tx_multiple_collisions
		+ vp->xstats.tx_single_collisions
		+ vp->xstats.tx_max_collisions;

	{
		u8 up = window_read8(vp, 4, 13);
		dev->stats.rx_bytes += (up & 0x0f) << 16;
		dev->stats.tx_bytes += (up & 0xf0) << 12;
	}
}

static int vortex_nway_reset(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);

	return mii_nway_restart(&vp->mii);
}

static int vortex_get_link_ksettings(struct net_device *dev,
				     struct ethtool_link_ksettings *cmd)
{
	struct vortex_private *vp = netdev_priv(dev);

	mii_ethtool_get_link_ksettings(&vp->mii, cmd);

	return 0;
}

static int vortex_set_link_ksettings(struct net_device *dev,
				     const struct ethtool_link_ksettings *cmd)
{
	struct vortex_private *vp = netdev_priv(dev);

	return mii_ethtool_set_link_ksettings(&vp->mii, cmd);
}

static u32 vortex_get_msglevel(struct net_device *dev)
{
	return vortex_debug;
}

static void vortex_set_msglevel(struct net_device *dev, u32 dbg)
{
	vortex_debug = dbg;
}

static int vortex_get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return VORTEX_NUM_STATS;
	default:
		return -EOPNOTSUPP;
	}
}

static void vortex_get_ethtool_stats(struct net_device *dev,
	struct ethtool_stats *stats, u64 *data)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	unsigned long flags;

	spin_lock_irqsave(&vp->lock, flags);
	update_stats(ioaddr, dev);
	spin_unlock_irqrestore(&vp->lock, flags);

	data[0] = vp->xstats.tx_deferred;
	data[1] = vp->xstats.tx_max_collisions;
	data[2] = vp->xstats.tx_multiple_collisions;
	data[3] = vp->xstats.tx_single_collisions;
	data[4] = vp->xstats.rx_bad_ssd;
}


static void vortex_get_strings(struct net_device *dev, u32 stringset, u8 *data)
{
	switch (stringset) {
	case ETH_SS_STATS:
		memcpy(data, &ethtool_stats_keys, sizeof(ethtool_stats_keys));
		break;
	default:
		WARN_ON(1);
		break;
	}
}

static void vortex_get_drvinfo(struct net_device *dev,
					struct ethtool_drvinfo *info)
{
	struct vortex_private *vp = netdev_priv(dev);

	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	if (VORTEX_PCI(vp)) {
		strlcpy(info->bus_info, pci_name(VORTEX_PCI(vp)),
			sizeof(info->bus_info));
	} else {
		if (VORTEX_EISA(vp))
			strlcpy(info->bus_info, dev_name(vp->gendev),
				sizeof(info->bus_info));
		else
			snprintf(info->bus_info, sizeof(info->bus_info),
				"EISA 0x%lx %d", dev->base_addr, dev->irq);
	}
}

static void vortex_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct vortex_private *vp = netdev_priv(dev);

	if (!VORTEX_PCI(vp))
		return;

	wol->supported = WAKE_MAGIC;

	wol->wolopts = 0;
	if (vp->enable_wol)
		wol->wolopts |= WAKE_MAGIC;
}

static int vortex_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct vortex_private *vp = netdev_priv(dev);

	if (!VORTEX_PCI(vp))
		return -EOPNOTSUPP;

	if (wol->wolopts & ~WAKE_MAGIC)
		return -EINVAL;

	if (wol->wolopts & WAKE_MAGIC)
		vp->enable_wol = 1;
	else
		vp->enable_wol = 0;
	acpi_set_WOL(dev);

	return 0;
}

static const struct ethtool_ops vortex_ethtool_ops = {
	.get_drvinfo		= vortex_get_drvinfo,
	.get_strings            = vortex_get_strings,
	.get_msglevel           = vortex_get_msglevel,
	.set_msglevel           = vortex_set_msglevel,
	.get_ethtool_stats      = vortex_get_ethtool_stats,
	.get_sset_count		= vortex_get_sset_count,
	.get_link               = ethtool_op_get_link,
	.nway_reset             = vortex_nway_reset,
	.get_wol                = vortex_get_wol,
	.set_wol                = vortex_set_wol,
	.get_ts_info		= ethtool_op_get_ts_info,
	.get_link_ksettings     = vortex_get_link_ksettings,
	.set_link_ksettings     = vortex_set_link_ksettings,
};

#ifdef CONFIG_PCI
/*
 *	Must power the device up to do MDIO operations
 */
static int vortex_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	int err;
	struct vortex_private *vp = netdev_priv(dev);
	pci_power_t state = 0;

	if(VORTEX_PCI(vp))
		state = VORTEX_PCI(vp)->current_state;

	/* The kernel core really should have pci_get_power_state() */

	if(state != 0)
		pci_set_power_state(VORTEX_PCI(vp), PCI_D0);
	err = generic_mii_ioctl(&vp->mii, if_mii(rq), cmd, NULL);
	if(state != 0)
		pci_set_power_state(VORTEX_PCI(vp), state);

	return err;
}
#endif


/* Pre-Cyclone chips have no documented multicast filter, so the only
   multicast setting is to receive all multicast frames.  At least
   the chip has a very clean way to set the mode, unlike many others. */
static void set_rx_mode(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;
	int new_mode;

	if (dev->flags & IFF_PROMISC) {
		if (vortex_debug > 3)
			pr_notice("%s: Setting promiscuous mode.\n", dev->name);
		new_mode = SetRxFilter|RxStation|RxMulticast|RxBroadcast|RxProm;
	} else	if (!netdev_mc_empty(dev) || dev->flags & IFF_ALLMULTI) {
		new_mode = SetRxFilter|RxStation|RxMulticast|RxBroadcast;
	} else
		new_mode = SetRxFilter | RxStation | RxBroadcast;

	iowrite16(new_mode, ioaddr + EL3_CMD);
}

#if IS_ENABLED(CONFIG_VLAN_8021Q)
/* Setup the card so that it can receive frames with an 802.1q VLAN tag.
   Note that this must be done after each RxReset due to some backwards
   compatibility logic in the Cyclone and Tornado ASICs */

/* The Ethernet Type used for 802.1q tagged frames */
#define VLAN_ETHER_TYPE 0x8100

static void set_8021q_mode(struct net_device *dev, int enable)
{
	struct vortex_private *vp = netdev_priv(dev);
	int mac_ctrl;

	if ((vp->drv_flags&IS_CYCLONE) || (vp->drv_flags&IS_TORNADO)) {
		/* cyclone and tornado chipsets can recognize 802.1q
		 * tagged frames and treat them correctly */

		int max_pkt_size = dev->mtu+14;	/* MTU+Ethernet header */
		if (enable)
			max_pkt_size += 4;	/* 802.1Q VLAN tag */

		window_write16(vp, max_pkt_size, 3, Wn3_MaxPktSize);

		/* set VlanEtherType to let the hardware checksumming
		   treat tagged frames correctly */
		window_write16(vp, VLAN_ETHER_TYPE, 7, Wn7_VlanEtherType);
	} else {
		/* on older cards we have to enable large frames */

		vp->large_frames = dev->mtu > 1500 || enable;

		mac_ctrl = window_read16(vp, 3, Wn3_MAC_Ctrl);
		if (vp->large_frames)
			mac_ctrl |= 0x40;
		else
			mac_ctrl &= ~0x40;
		window_write16(vp, mac_ctrl, 3, Wn3_MAC_Ctrl);
	}
}
#else

static void set_8021q_mode(struct net_device *dev, int enable)
{
}


#endif

/* MII transceiver control section.
   Read and write the MII registers using software-generated serial
   MDIO protocol.  See the MII specifications or DP83840A data sheet
   for details. */

/* The maximum data clock rate is 2.5 Mhz.  The minimum timing is usually
   met by back-to-back PCI I/O cycles, but we insert a delay to avoid
   "overclocking" issues. */
static void mdio_delay(struct vortex_private *vp)
{
	window_read32(vp, 4, Wn4_PhysicalMgmt);
}

#define MDIO_SHIFT_CLK	0x01
#define MDIO_DIR_WRITE	0x04
#define MDIO_DATA_WRITE0 (0x00 | MDIO_DIR_WRITE)
#define MDIO_DATA_WRITE1 (0x02 | MDIO_DIR_WRITE)
#define MDIO_DATA_READ	0x02
#define MDIO_ENB_IN		0x00

/* Generate the preamble required for initial synchronization and
   a few older transceivers. */
static void mdio_sync(struct vortex_private *vp, int bits)
{
	/* Establish sync by sending at least 32 logic ones. */
	while (-- bits >= 0) {
		window_write16(vp, MDIO_DATA_WRITE1, 4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
		window_write16(vp, MDIO_DATA_WRITE1 | MDIO_SHIFT_CLK,
			       4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
	}
}

static int mdio_read(struct net_device *dev, int phy_id, int location)
{
	int i;
	struct vortex_private *vp = netdev_priv(dev);
	int read_cmd = (0xf6 << 10) | (phy_id << 5) | location;
	unsigned int retval = 0;

	spin_lock_bh(&vp->mii_lock);

	if (mii_preamble_required)
		mdio_sync(vp, 32);

	/* Shift the read command bits out. */
	for (i = 14; i >= 0; i--) {
		int dataval = (read_cmd&(1<<i)) ? MDIO_DATA_WRITE1 : MDIO_DATA_WRITE0;
		window_write16(vp, dataval, 4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
		window_write16(vp, dataval | MDIO_SHIFT_CLK,
			       4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
	}
	/* Read the two transition, 16 data, and wire-idle bits. */
	for (i = 19; i > 0; i--) {
		window_write16(vp, MDIO_ENB_IN, 4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
		retval = (retval << 1) |
			((window_read16(vp, 4, Wn4_PhysicalMgmt) &
			  MDIO_DATA_READ) ? 1 : 0);
		window_write16(vp, MDIO_ENB_IN | MDIO_SHIFT_CLK,
			       4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
	}

	spin_unlock_bh(&vp->mii_lock);

	return retval & 0x20000 ? 0xffff : retval>>1 & 0xffff;
}

static void mdio_write(struct net_device *dev, int phy_id, int location, int value)
{
	struct vortex_private *vp = netdev_priv(dev);
	int write_cmd = 0x50020000 | (phy_id << 23) | (location << 18) | value;
	int i;

	spin_lock_bh(&vp->mii_lock);

	if (mii_preamble_required)
		mdio_sync(vp, 32);

	/* Shift the command bits out. */
	for (i = 31; i >= 0; i--) {
		int dataval = (write_cmd&(1<<i)) ? MDIO_DATA_WRITE1 : MDIO_DATA_WRITE0;
		window_write16(vp, dataval, 4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
		window_write16(vp, dataval | MDIO_SHIFT_CLK,
			       4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
	}
	/* Leave the interface idle. */
	for (i = 1; i >= 0; i--) {
		window_write16(vp, MDIO_ENB_IN, 4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
		window_write16(vp, MDIO_ENB_IN | MDIO_SHIFT_CLK,
			       4, Wn4_PhysicalMgmt);
		mdio_delay(vp);
	}

	spin_unlock_bh(&vp->mii_lock);
}

/* ACPI: Advanced Configuration and Power Interface. */
/* Set Wake-On-LAN mode and put the board into D3 (power-down) state. */
static void acpi_set_WOL(struct net_device *dev)
{
	struct vortex_private *vp = netdev_priv(dev);
	void __iomem *ioaddr = vp->ioaddr;

	device_set_wakeup_enable(vp->gendev, vp->enable_wol);

	if (vp->enable_wol) {
		/* Power up on: 1==Downloaded Filter, 2==Magic Packets, 4==Link Status. */
		window_write16(vp, 2, 7, 0x0c);
		/* The RxFilter must accept the WOL frames. */
		iowrite16(SetRxFilter|RxStation|RxMulticast|RxBroadcast, ioaddr + EL3_CMD);
		iowrite16(RxEnable, ioaddr + EL3_CMD);

		if (pci_enable_wake(VORTEX_PCI(vp), PCI_D3hot, 1)) {
			pr_info("%s: WOL not supported.\n", pci_name(VORTEX_PCI(vp)));

			vp->enable_wol = 0;
			return;
		}

		if (VORTEX_PCI(vp)->current_state < PCI_D3hot)
			return;

		/* Change the power state to D3; RxEnable doesn't take effect. */
		pci_set_power_state(VORTEX_PCI(vp), PCI_D3hot);
	}
}


static void vortex_remove_one(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	struct vortex_private *vp;

	if (!dev) {
		pr_err("vortex_remove_one called for Compaq device!\n");
		BUG();
	}

	vp = netdev_priv(dev);

	if (vp->cb_fn_base)
		pci_iounmap(pdev, vp->cb_fn_base);

	unregister_netdev(dev);

	pci_set_power_state(pdev, PCI_D0);	/* Go active */
	if (vp->pm_state_valid)
		pci_restore_state(pdev);
	pci_disable_device(pdev);

	/* Should really use issue_and_wait() here */
	iowrite16(TotalReset | ((vp->drv_flags & EEPROM_RESET) ? 0x04 : 0x14),
	     vp->ioaddr + EL3_CMD);

	pci_iounmap(pdev, vp->ioaddr);

	dma_free_coherent(&pdev->dev,
			sizeof(struct boom_rx_desc) * RX_RING_SIZE +
			sizeof(struct boom_tx_desc) * TX_RING_SIZE,
			vp->rx_ring, vp->rx_ring_dma);

	pci_release_regions(pdev);

	free_netdev(dev);
}


static struct pci_driver vortex_driver = {
	.name		= "3c59x",
	.probe		= vortex_init_one,
	.remove		= vortex_remove_one,
	.id_table	= vortex_pci_tbl,
	.driver.pm	= VORTEX_PM_OPS,
};


static int vortex_have_pci;
static int vortex_have_eisa;


static int __init vortex_init(void)
{
	int pci_rc, eisa_rc;

	pci_rc = pci_register_driver(&vortex_driver);
	eisa_rc = vortex_eisa_init();

	if (pci_rc == 0)
		vortex_have_pci = 1;
	if (eisa_rc > 0)
		vortex_have_eisa = 1;

	return (vortex_have_pci + vortex_have_eisa) ? 0 : -ENODEV;
}


static void __exit vortex_eisa_cleanup(void)
{
	void __iomem *ioaddr;

#ifdef CONFIG_EISA
	/* Take care of the EISA devices */
	eisa_driver_unregister(&vortex_eisa_driver);
#endif

	if (compaq_net_device) {
		ioaddr = ioport_map(compaq_net_device->base_addr,
		                    VORTEX_TOTAL_SIZE);

		unregister_netdev(compaq_net_device);
		iowrite16(TotalReset, ioaddr + EL3_CMD);
		release_region(compaq_net_device->base_addr,
		               VORTEX_TOTAL_SIZE);

		free_netdev(compaq_net_device);
	}
}


static void __exit vortex_cleanup(void)
{
	if (vortex_have_pci)
		pci_unregister_driver(&vortex_driver);
	if (vortex_have_eisa)
		vortex_eisa_cleanup();
}


module_init(vortex_init);
module_exit(vortex_cleanup);
