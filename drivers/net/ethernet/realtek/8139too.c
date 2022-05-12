/*

	8139too.c: A RealTek RTL-8139 Fast Ethernet driver for Linux.

	Maintained by Jeff Garzik <jgarzik@pobox.com>
	Copyright 2000-2002 Jeff Garzik

	Much code comes from Donald Becker's rtl8139.c driver,
	versions 1.13 and older.  This driver was originally based
	on rtl8139.c version 1.07.  Header of rtl8139.c version 1.13:

	-----<snip>-----

        	Written 1997-2001 by Donald Becker.
		This software may be used and distributed according to the
		terms of the GNU General Public License (GPL), incorporated
		herein by reference.  Drivers based on or derived from this
		code fall under the GPL and must retain the authorship,
		copyright and license notice.  This file is not a complete
		program and may only be used when the entire operating
		system is licensed under the GPL.

		This driver is for boards based on the RTL8129 and RTL8139
		PCI ethernet chips.

		The author may be reached as becker@scyld.com, or C/O Scyld
		Computing Corporation 410 Severn Ave., Suite 210 Annapolis
		MD 21403

		Support and updates available at
		http://www.scyld.com/network/rtl8139.html

		Twister-tuning table provided by Kinston
		<shangh@realtek.com.tw>.

	-----<snip>-----

	This software may be used and distributed according to the terms
	of the GNU General Public License, incorporated herein by reference.

	Contributors:

		Donald Becker - he wrote the original driver, kudos to him!
		(but please don't e-mail him for support, this isn't his driver)

		Tigran Aivazian - bug fixes, skbuff free cleanup

		Martin Mares - suggestions for PCI cleanup

		David S. Miller - PCI DMA and softnet updates

		Ernst Gill - fixes ported from BSD driver

		Daniel Kobras - identified specific locations of
			posted MMIO write bugginess

		Gerard Sharp - bug fix, testing and feedback

		David Ford - Rx ring wrap fix

		Dan DeMaggio - swapped RTL8139 cards with me, and allowed me
		to find and fix a crucial bug on older chipsets.

		Donald Becker/Chris Butterworth/Marcus Westergren -
		Noticed various Rx packet size-related buglets.

		Santiago Garcia Mantinan - testing and feedback

		Jens David - 2.2.x kernel backports

		Martin Dennett - incredibly helpful insight on undocumented
		features of the 8139 chips

		Jean-Jacques Michel - bug fix

		Tobias Ringström - Rx interrupt status checking suggestion

		Andrew Morton - Clear blocked signals, avoid
		buffer overrun setting current->comm.

		Kalle Olavi Niemitalo - Wake-on-LAN ioctls

		Robert Kuebel - Save kernel thread from dying on any signal.

	Submitting bug reports:

		"rtl8139-diag -mmmaaavvveefN" output
		enable RTL8139_DEBUG below, and look at 'dmesg' or kernel log

*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define DRV_NAME	"8139too"
#define DRV_VERSION	"0.9.28"


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h>
#include <linux/delay.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/completion.h>
#include <linux/crc32.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/gfp.h>
#include <linux/if_vlan.h>
#include <asm/irq.h>

#define RTL8139_DRIVER_NAME   DRV_NAME " Fast Ethernet driver " DRV_VERSION

/* Default Message level */
#define RTL8139_DEF_MSG_ENABLE   (NETIF_MSG_DRV   | \
                                 NETIF_MSG_PROBE  | \
                                 NETIF_MSG_LINK)


/* define to 1, 2 or 3 to enable copious debugging info */
#define RTL8139_DEBUG 0

/* define to 1 to disable lightweight runtime debugging checks */
#undef RTL8139_NDEBUG


#ifdef RTL8139_NDEBUG
#  define assert(expr) do {} while (0)
#else
#  define assert(expr) \
        if (unlikely(!(expr))) {				\
		pr_err("Assertion failed! %s,%s,%s,line=%d\n",	\
		       #expr, __FILE__, __func__, __LINE__);	\
        }
#endif


/* A few user-configurable values. */
/* media options */
#define MAX_UNITS 8
static int media[MAX_UNITS] = {-1, -1, -1, -1, -1, -1, -1, -1};
static int full_duplex[MAX_UNITS] = {-1, -1, -1, -1, -1, -1, -1, -1};

/* Whether to use MMIO or PIO. Default to MMIO. */
#ifdef CONFIG_8139TOO_PIO
static bool use_io = true;
#else
static bool use_io = false;
#endif

/* Maximum number of multicast addresses to filter (vs. Rx-all-multicast).
   The RTL chips use a 64 element hash table based on the Ethernet CRC.  */
static int multicast_filter_limit = 32;

/* bitmapped message enable number */
static int debug = -1;

/*
 * Receive ring size
 * Warning: 64K ring has hardware issues and may lock up.
 */
#if defined(CONFIG_SH_DREAMCAST)
#define RX_BUF_IDX 0	/* 8K ring */
#else
#define RX_BUF_IDX	2	/* 32K ring */
#endif
#define RX_BUF_LEN	(8192 << RX_BUF_IDX)
#define RX_BUF_PAD	16
#define RX_BUF_WRAP_PAD 2048 /* spare padding to handle lack of packet wrap */

#if RX_BUF_LEN == 65536
#define RX_BUF_TOT_LEN	RX_BUF_LEN
#else
#define RX_BUF_TOT_LEN	(RX_BUF_LEN + RX_BUF_PAD + RX_BUF_WRAP_PAD)
#endif

/* Number of Tx descriptor registers. */
#define NUM_TX_DESC	4

/* max supported ethernet frame size -- must be at least (dev->mtu+18+4).*/
#define MAX_ETH_FRAME_SIZE	1792

/* max supported payload size */
#define MAX_ETH_DATA_SIZE (MAX_ETH_FRAME_SIZE - VLAN_ETH_HLEN - ETH_FCS_LEN)

/* Size of the Tx bounce buffers -- must be at least (dev->mtu+18+4). */
#define TX_BUF_SIZE	MAX_ETH_FRAME_SIZE
#define TX_BUF_TOT_LEN	(TX_BUF_SIZE * NUM_TX_DESC)

/* PCI Tuning Parameters
   Threshold is bytes transferred to chip before transmission starts. */
#define TX_FIFO_THRESH 256	/* In bytes, rounded down to 32 byte units. */

/* The following settings are log_2(bytes)-4:  0 == 16 bytes .. 6==1024, 7==end of packet. */
#define RX_FIFO_THRESH	7	/* Rx buffer level before first PCI xfer.  */
#define RX_DMA_BURST	7	/* Maximum PCI burst, '6' is 1024 */
#define TX_DMA_BURST	6	/* Maximum PCI burst, '6' is 1024 */
#define TX_RETRY	8	/* 0-15.  retries = 16 + (TX_RETRY * 16) */

/* Operational parameters that usually are not changed. */
/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT  (6*HZ)


enum {
	HAS_MII_XCVR = 0x010000,
	HAS_CHIP_XCVR = 0x020000,
	HAS_LNK_CHNG = 0x040000,
};

#define RTL_NUM_STATS 4		/* number of ETHTOOL_GSTATS u64's */
#define RTL_REGS_VER 1		/* version of reg. data in ETHTOOL_GREGS */
#define RTL_MIN_IO_SIZE 0x80
#define RTL8139B_IO_SIZE 256

#define RTL8129_CAPS	HAS_MII_XCVR
#define RTL8139_CAPS	(HAS_CHIP_XCVR|HAS_LNK_CHNG)

typedef enum {
	RTL8139 = 0,
	RTL8129,
} board_t;


/* indexed by board_t, above */
static const struct {
	const char *name;
	u32 hw_flags;
} board_info[] = {
	{ "RealTek RTL8139", RTL8139_CAPS },
	{ "RealTek RTL8129", RTL8129_CAPS },
};


static const struct pci_device_id rtl8139_pci_tbl[] = {
	{0x10ec, 0x8139, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x10ec, 0x8138, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x1113, 0x1211, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x1500, 0x1360, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x4033, 0x1360, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x1186, 0x1300, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x1186, 0x1340, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x13d1, 0xab06, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x1259, 0xa117, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x1259, 0xa11e, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x14ea, 0xab06, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x14ea, 0xab07, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x11db, 0x1234, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x1432, 0x9130, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x02ac, 0x1012, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x018a, 0x0106, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x126c, 0x1211, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x1743, 0x8139, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x021b, 0x8139, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
	{0x16ec, 0xab06, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },

#ifdef CONFIG_SH_SECUREEDGE5410
	/* Bogus 8139 silicon reports 8129 without external PROM :-( */
	{0x10ec, 0x8129, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },
#endif
#ifdef CONFIG_8139TOO_8129
	{0x10ec, 0x8129, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8129 },
#endif

	/* some crazy cards report invalid vendor ids like
	 * 0x0001 here.  The other ids are valid and constant,
	 * so we simply don't match on the main vendor id.
	 */
	{PCI_ANY_ID, 0x8139, 0x10ec, 0x8139, 0, 0, RTL8139 },
	{PCI_ANY_ID, 0x8139, 0x1186, 0x1300, 0, 0, RTL8139 },
	{PCI_ANY_ID, 0x8139, 0x13d1, 0xab06, 0, 0, RTL8139 },

	{0,}
};
MODULE_DEVICE_TABLE (pci, rtl8139_pci_tbl);

static struct {
	const char str[ETH_GSTRING_LEN];
} ethtool_stats_keys[] = {
	{ "early_rx" },
	{ "tx_buf_mapped" },
	{ "tx_timeouts" },
	{ "rx_lost_in_ring" },
};

/* The rest of these values should never change. */

/* Symbolic offsets to registers. */
enum RTL8139_registers {
	MAC0		= 0,	 /* Ethernet hardware address. */
	MAR0		= 8,	 /* Multicast filter. */
	TxStatus0	= 0x10,	 /* Transmit status (Four 32bit registers). */
	TxAddr0		= 0x20,	 /* Tx descriptors (also four 32bit). */
	RxBuf		= 0x30,
	ChipCmd		= 0x37,
	RxBufPtr	= 0x38,
	RxBufAddr	= 0x3A,
	IntrMask	= 0x3C,
	IntrStatus	= 0x3E,
	TxConfig	= 0x40,
	RxConfig	= 0x44,
	Timer		= 0x48,	 /* A general-purpose counter. */
	RxMissed	= 0x4C,  /* 24 bits valid, write clears. */
	Cfg9346		= 0x50,
	Config0		= 0x51,
	Config1		= 0x52,
	TimerInt	= 0x54,
	MediaStatus	= 0x58,
	Config3		= 0x59,
	Config4		= 0x5A,	 /* absent on RTL-8139A */
	HltClk		= 0x5B,
	MultiIntr	= 0x5C,
	TxSummary	= 0x60,
	BasicModeCtrl	= 0x62,
	BasicModeStatus	= 0x64,
	NWayAdvert	= 0x66,
	NWayLPAR	= 0x68,
	NWayExpansion	= 0x6A,
	/* Undocumented registers, but required for proper operation. */
	FIFOTMS		= 0x70,	 /* FIFO Control and test. */
	CSCR		= 0x74,	 /* Chip Status and Configuration Register. */
	PARA78		= 0x78,
	FlashReg	= 0xD4,	/* Communication with Flash ROM, four bytes. */
	PARA7c		= 0x7c,	 /* Magic transceiver parameter register. */
	Config5		= 0xD8,	 /* absent on RTL-8139A */
};

enum ClearBitMasks {
	MultiIntrClear	= 0xF000,
	ChipCmdClear	= 0xE2,
	Config1Clear	= (1<<7)|(1<<6)|(1<<3)|(1<<2)|(1<<1),
};

enum ChipCmdBits {
	CmdReset	= 0x10,
	CmdRxEnb	= 0x08,
	CmdTxEnb	= 0x04,
	RxBufEmpty	= 0x01,
};

/* Interrupt register bits, using my own meaningful names. */
enum IntrStatusBits {
	PCIErr		= 0x8000,
	PCSTimeout	= 0x4000,
	RxFIFOOver	= 0x40,
	RxUnderrun	= 0x20,
	RxOverflow	= 0x10,
	TxErr		= 0x08,
	TxOK		= 0x04,
	RxErr		= 0x02,
	RxOK		= 0x01,

	RxAckBits	= RxFIFOOver | RxOverflow | RxOK,
};

enum TxStatusBits {
	TxHostOwns	= 0x2000,
	TxUnderrun	= 0x4000,
	TxStatOK	= 0x8000,
	TxOutOfWindow	= 0x20000000,
	TxAborted	= 0x40000000,
	TxCarrierLost	= 0x80000000,
};
enum RxStatusBits {
	RxMulticast	= 0x8000,
	RxPhysical	= 0x4000,
	RxBroadcast	= 0x2000,
	RxBadSymbol	= 0x0020,
	RxRunt		= 0x0010,
	RxTooLong	= 0x0008,
	RxCRCErr	= 0x0004,
	RxBadAlign	= 0x0002,
	RxStatusOK	= 0x0001,
};

/* Bits in RxConfig. */
enum rx_mode_bits {
	AcceptErr	= 0x20,
	AcceptRunt	= 0x10,
	AcceptBroadcast	= 0x08,
	AcceptMulticast	= 0x04,
	AcceptMyPhys	= 0x02,
	AcceptAllPhys	= 0x01,
};

/* Bits in TxConfig. */
enum tx_config_bits {
        /* Interframe Gap Time. Only TxIFG96 doesn't violate IEEE 802.3 */
        TxIFGShift	= 24,
        TxIFG84		= (0 << TxIFGShift), /* 8.4us / 840ns (10 / 100Mbps) */
        TxIFG88		= (1 << TxIFGShift), /* 8.8us / 880ns (10 / 100Mbps) */
        TxIFG92		= (2 << TxIFGShift), /* 9.2us / 920ns (10 / 100Mbps) */
        TxIFG96		= (3 << TxIFGShift), /* 9.6us / 960ns (10 / 100Mbps) */

	TxLoopBack	= (1 << 18) | (1 << 17), /* enable loopback test mode */
	TxCRC		= (1 << 16),	/* DISABLE Tx pkt CRC append */
	TxClearAbt	= (1 << 0),	/* Clear abort (WO) */
	TxDMAShift	= 8, /* DMA burst value (0-7) is shifted X many bits */
	TxRetryShift	= 4, /* TXRR value (0-15) is shifted X many bits */

	TxVersionMask	= 0x7C800000, /* mask out version bits 30-26, 23 */
};

/* Bits in Config1 */
enum Config1Bits {
	Cfg1_PM_Enable	= 0x01,
	Cfg1_VPD_Enable	= 0x02,
	Cfg1_PIO	= 0x04,
	Cfg1_MMIO	= 0x08,
	LWAKE		= 0x10,		/* not on 8139, 8139A */
	Cfg1_Driver_Load = 0x20,
	Cfg1_LED0	= 0x40,
	Cfg1_LED1	= 0x80,
	SLEEP		= (1 << 1),	/* only on 8139, 8139A */
	PWRDN		= (1 << 0),	/* only on 8139, 8139A */
};

/* Bits in Config3 */
enum Config3Bits {
	Cfg3_FBtBEn   	= (1 << 0), /* 1	= Fast Back to Back */
	Cfg3_FuncRegEn	= (1 << 1), /* 1	= enable CardBus Function registers */
	Cfg3_CLKRUN_En	= (1 << 2), /* 1	= enable CLKRUN */
	Cfg3_CardB_En 	= (1 << 3), /* 1	= enable CardBus registers */
	Cfg3_LinkUp   	= (1 << 4), /* 1	= wake up on link up */
	Cfg3_Magic    	= (1 << 5), /* 1	= wake up on Magic Packet (tm) */
	Cfg3_PARM_En  	= (1 << 6), /* 0	= software can set twister parameters */
	Cfg3_GNTSel   	= (1 << 7), /* 1	= delay 1 clock from PCI GNT signal */
};

/* Bits in Config4 */
enum Config4Bits {
	LWPTN	= (1 << 2),	/* not on 8139, 8139A */
};

/* Bits in Config5 */
enum Config5Bits {
	Cfg5_PME_STS   	= (1 << 0), /* 1	= PCI reset resets PME_Status */
	Cfg5_LANWake   	= (1 << 1), /* 1	= enable LANWake signal */
	Cfg5_LDPS      	= (1 << 2), /* 0	= save power when link is down */
	Cfg5_FIFOAddrPtr= (1 << 3), /* Realtek internal SRAM testing */
	Cfg5_UWF        = (1 << 4), /* 1 = accept unicast wakeup frame */
	Cfg5_MWF        = (1 << 5), /* 1 = accept multicast wakeup frame */
	Cfg5_BWF        = (1 << 6), /* 1 = accept broadcast wakeup frame */
};

enum RxConfigBits {
	/* rx fifo threshold */
	RxCfgFIFOShift	= 13,
	RxCfgFIFONone	= (7 << RxCfgFIFOShift),

	/* Max DMA burst */
	RxCfgDMAShift	= 8,
	RxCfgDMAUnlimited = (7 << RxCfgDMAShift),

	/* rx ring buffer length */
	RxCfgRcv8K	= 0,
	RxCfgRcv16K	= (1 << 11),
	RxCfgRcv32K	= (1 << 12),
	RxCfgRcv64K	= (1 << 11) | (1 << 12),

	/* Disable packet wrap at end of Rx buffer. (not possible with 64k) */
	RxNoWrap	= (1 << 7),
};

/* Twister tuning parameters from RealTek.
   Completely undocumented, but required to tune bad links on some boards. */
enum CSCRBits {
	CSCR_LinkOKBit		= 0x0400,
	CSCR_LinkChangeBit	= 0x0800,
	CSCR_LinkStatusBits	= 0x0f000,
	CSCR_LinkDownOffCmd	= 0x003c0,
	CSCR_LinkDownCmd	= 0x0f3c0,
};

enum Cfg9346Bits {
	Cfg9346_Lock	= 0x00,
	Cfg9346_Unlock	= 0xC0,
};

typedef enum {
	CH_8139	= 0,
	CH_8139_K,
	CH_8139A,
	CH_8139A_G,
	CH_8139B,
	CH_8130,
	CH_8139C,
	CH_8100,
	CH_8100B_8139D,
	CH_8101,
} chip_t;

enum chip_flags {
	HasHltClk	= (1 << 0),
	HasLWake	= (1 << 1),
};

#define HW_REVID(b30, b29, b28, b27, b26, b23, b22) \
	(b30<<30 | b29<<29 | b28<<28 | b27<<27 | b26<<26 | b23<<23 | b22<<22)
#define HW_REVID_MASK	HW_REVID(1, 1, 1, 1, 1, 1, 1)

/* directly indexed by chip_t, above */
static const struct {
	const char *name;
	u32 version; /* from RTL8139C/RTL8139D docs */
	u32 flags;
} rtl_chip_info[] = {
	{ "RTL-8139",
	  HW_REVID(1, 0, 0, 0, 0, 0, 0),
	  HasHltClk,
	},

	{ "RTL-8139 rev K",
	  HW_REVID(1, 1, 0, 0, 0, 0, 0),
	  HasHltClk,
	},

	{ "RTL-8139A",
	  HW_REVID(1, 1, 1, 0, 0, 0, 0),
	  HasHltClk, /* XXX undocumented? */
	},

	{ "RTL-8139A rev G",
	  HW_REVID(1, 1, 1, 0, 0, 1, 0),
	  HasHltClk, /* XXX undocumented? */
	},

	{ "RTL-8139B",
	  HW_REVID(1, 1, 1, 1, 0, 0, 0),
	  HasLWake,
	},

	{ "RTL-8130",
	  HW_REVID(1, 1, 1, 1, 1, 0, 0),
	  HasLWake,
	},

	{ "RTL-8139C",
	  HW_REVID(1, 1, 1, 0, 1, 0, 0),
	  HasLWake,
	},

	{ "RTL-8100",
	  HW_REVID(1, 1, 1, 1, 0, 1, 0),
 	  HasLWake,
 	},

	{ "RTL-8100B/8139D",
	  HW_REVID(1, 1, 1, 0, 1, 0, 1),
	  HasHltClk /* XXX undocumented? */
	| HasLWake,
	},

	{ "RTL-8101",
	  HW_REVID(1, 1, 1, 0, 1, 1, 1),
	  HasLWake,
	},
};

struct rtl_extra_stats {
	unsigned long early_rx;
	unsigned long tx_buf_mapped;
	unsigned long tx_timeouts;
	unsigned long rx_lost_in_ring;
};

struct rtl8139_stats {
	u64	packets;
	u64	bytes;
	struct u64_stats_sync	syncp;
};

struct rtl8139_private {
	void __iomem		*mmio_addr;
	int			drv_flags;
	struct pci_dev		*pci_dev;
	u32			msg_enable;
	struct napi_struct	napi;
	struct net_device	*dev;

	unsigned char		*rx_ring;
	unsigned int		cur_rx;	/* RX buf index of next pkt */
	struct rtl8139_stats	rx_stats;
	dma_addr_t		rx_ring_dma;

	unsigned int		tx_flag;
	unsigned long		cur_tx;
	unsigned long		dirty_tx;
	struct rtl8139_stats	tx_stats;
	unsigned char		*tx_buf[NUM_TX_DESC];	/* Tx bounce buffers */
	unsigned char		*tx_bufs;	/* Tx bounce buffer region. */
	dma_addr_t		tx_bufs_dma;

	signed char		phys[4];	/* MII device addresses. */

				/* Twister tune state. */
	char			twistie, twist_row, twist_col;

	unsigned int		watchdog_fired : 1;
	unsigned int		default_port : 4; /* Last dev->if_port value. */
	unsigned int		have_thread : 1;

	spinlock_t		lock;
	spinlock_t		rx_lock;

	chip_t			chipset;
	u32			rx_config;
	struct rtl_extra_stats	xstats;

	struct delayed_work	thread;

	struct mii_if_info	mii;
	unsigned int		regs_len;
	unsigned long		fifo_copy_timeout;
};

MODULE_AUTHOR ("Jeff Garzik <jgarzik@pobox.com>");
MODULE_DESCRIPTION ("RealTek RTL-8139 Fast Ethernet driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_param(use_io, bool, 0);
MODULE_PARM_DESC(use_io, "Force use of I/O access mode. 0=MMIO 1=PIO");
module_param(multicast_filter_limit, int, 0);
module_param_array(media, int, NULL, 0);
module_param_array(full_duplex, int, NULL, 0);
module_param(debug, int, 0);
MODULE_PARM_DESC (debug, "8139too bitmapped message enable number");
MODULE_PARM_DESC (multicast_filter_limit, "8139too maximum number of filtered multicast addresses");
MODULE_PARM_DESC (media, "8139too: Bits 4+9: force full duplex, bit 5: 100Mbps");
MODULE_PARM_DESC (full_duplex, "8139too: Force full duplex for board(s) (1)");

static int read_eeprom (void __iomem *ioaddr, int location, int addr_len);
static int rtl8139_open (struct net_device *dev);
static int mdio_read (struct net_device *dev, int phy_id, int location);
static void mdio_write (struct net_device *dev, int phy_id, int location,
			int val);
static void rtl8139_start_thread(struct rtl8139_private *tp);
static void rtl8139_tx_timeout (struct net_device *dev, unsigned int txqueue);
static void rtl8139_init_ring (struct net_device *dev);
static netdev_tx_t rtl8139_start_xmit (struct sk_buff *skb,
				       struct net_device *dev);
#ifdef CONFIG_NET_POLL_CONTROLLER
static void rtl8139_poll_controller(struct net_device *dev);
#endif
static int rtl8139_set_mac_address(struct net_device *dev, void *p);
static int rtl8139_poll(struct napi_struct *napi, int budget);
static irqreturn_t rtl8139_interrupt (int irq, void *dev_instance);
static int rtl8139_close (struct net_device *dev);
static int netdev_ioctl (struct net_device *dev, struct ifreq *rq, int cmd);
static void rtl8139_get_stats64(struct net_device *dev,
				struct rtnl_link_stats64 *stats);
static void rtl8139_set_rx_mode (struct net_device *dev);
static void __set_rx_mode (struct net_device *dev);
static void rtl8139_hw_start (struct net_device *dev);
static void rtl8139_thread (struct work_struct *work);
static void rtl8139_tx_timeout_task(struct work_struct *work);
static const struct ethtool_ops rtl8139_ethtool_ops;

/* write MMIO register, with flush */
/* Flush avoids rtl8139 bug w/ posted MMIO writes */
#define RTL_W8_F(reg, val8)	do { iowrite8 ((val8), ioaddr + (reg)); ioread8 (ioaddr + (reg)); } while (0)
#define RTL_W16_F(reg, val16)	do { iowrite16 ((val16), ioaddr + (reg)); ioread16 (ioaddr + (reg)); } while (0)
#define RTL_W32_F(reg, val32)	do { iowrite32 ((val32), ioaddr + (reg)); ioread32 (ioaddr + (reg)); } while (0)

/* write MMIO register */
#define RTL_W8(reg, val8)	iowrite8 ((val8), ioaddr + (reg))
#define RTL_W16(reg, val16)	iowrite16 ((val16), ioaddr + (reg))
#define RTL_W32(reg, val32)	iowrite32 ((val32), ioaddr + (reg))

/* read MMIO register */
#define RTL_R8(reg)		ioread8 (ioaddr + (reg))
#define RTL_R16(reg)		ioread16 (ioaddr + (reg))
#define RTL_R32(reg)		ioread32 (ioaddr + (reg))


static const u16 rtl8139_intr_mask =
	PCIErr | PCSTimeout | RxUnderrun | RxOverflow | RxFIFOOver |
	TxErr | TxOK | RxErr | RxOK;

static const u16 rtl8139_norx_intr_mask =
	PCIErr | PCSTimeout | RxUnderrun |
	TxErr | TxOK | RxErr ;

#if RX_BUF_IDX == 0
static const unsigned int rtl8139_rx_config =
	RxCfgRcv8K | RxNoWrap |
	(RX_FIFO_THRESH << RxCfgFIFOShift) |
	(RX_DMA_BURST << RxCfgDMAShift);
#elif RX_BUF_IDX == 1
static const unsigned int rtl8139_rx_config =
	RxCfgRcv16K | RxNoWrap |
	(RX_FIFO_THRESH << RxCfgFIFOShift) |
	(RX_DMA_BURST << RxCfgDMAShift);
#elif RX_BUF_IDX == 2
static const unsigned int rtl8139_rx_config =
	RxCfgRcv32K | RxNoWrap |
	(RX_FIFO_THRESH << RxCfgFIFOShift) |
	(RX_DMA_BURST << RxCfgDMAShift);
#elif RX_BUF_IDX == 3
static const unsigned int rtl8139_rx_config =
	RxCfgRcv64K |
	(RX_FIFO_THRESH << RxCfgFIFOShift) |
	(RX_DMA_BURST << RxCfgDMAShift);
#else
#error "Invalid configuration for 8139_RXBUF_IDX"
#endif

static const unsigned int rtl8139_tx_config =
	TxIFG96 | (TX_DMA_BURST << TxDMAShift) | (TX_RETRY << TxRetryShift);

static void __rtl8139_cleanup_dev (struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	struct pci_dev *pdev;

	assert (dev != NULL);
	assert (tp->pci_dev != NULL);
	pdev = tp->pci_dev;

	if (tp->mmio_addr)
		pci_iounmap (pdev, tp->mmio_addr);

	/* it's ok to call this even if we have no regions to free */
	pci_release_regions (pdev);

	free_netdev(dev);
}


static void rtl8139_chip_reset (void __iomem *ioaddr)
{
	int i;

	/* Soft reset the chip. */
	RTL_W8 (ChipCmd, CmdReset);

	/* Check that the chip has finished the reset. */
	for (i = 1000; i > 0; i--) {
		barrier();
		if ((RTL_R8 (ChipCmd) & CmdReset) == 0)
			break;
		udelay (10);
	}
}


static struct net_device *rtl8139_init_board(struct pci_dev *pdev)
{
	struct device *d = &pdev->dev;
	void __iomem *ioaddr;
	struct net_device *dev;
	struct rtl8139_private *tp;
	u8 tmp8;
	int rc, disable_dev_on_err = 0;
	unsigned int i, bar;
	unsigned long io_len;
	u32 version;
	static const struct {
		unsigned long mask;
		char *type;
	} res[] = {
		{ IORESOURCE_IO,  "PIO" },
		{ IORESOURCE_MEM, "MMIO" }
	};

	assert (pdev != NULL);

	/* dev and priv zeroed in alloc_etherdev */
	dev = alloc_etherdev (sizeof (*tp));
	if (dev == NULL)
		return ERR_PTR(-ENOMEM);

	SET_NETDEV_DEV(dev, &pdev->dev);

	tp = netdev_priv(dev);
	tp->pci_dev = pdev;

	/* enable device (incl. PCI PM wakeup and hotplug setup) */
	rc = pci_enable_device (pdev);
	if (rc)
		goto err_out;

	disable_dev_on_err = 1;
	rc = pci_request_regions (pdev, DRV_NAME);
	if (rc)
		goto err_out;

	pci_set_master (pdev);

	u64_stats_init(&tp->rx_stats.syncp);
	u64_stats_init(&tp->tx_stats.syncp);

retry:
	/* PIO bar register comes first. */
	bar = !use_io;

	io_len = pci_resource_len(pdev, bar);

	dev_dbg(d, "%s region size = 0x%02lX\n", res[bar].type, io_len);

	if (!(pci_resource_flags(pdev, bar) & res[bar].mask)) {
		dev_err(d, "region #%d not a %s resource, aborting\n", bar,
			res[bar].type);
		rc = -ENODEV;
		goto err_out;
	}
	if (io_len < RTL_MIN_IO_SIZE) {
		dev_err(d, "Invalid PCI %s region size(s), aborting\n",
			res[bar].type);
		rc = -ENODEV;
		goto err_out;
	}

	ioaddr = pci_iomap(pdev, bar, 0);
	if (!ioaddr) {
		dev_err(d, "cannot map %s\n", res[bar].type);
		if (!use_io) {
			use_io = true;
			goto retry;
		}
		rc = -ENODEV;
		goto err_out;
	}
	tp->regs_len = io_len;
	tp->mmio_addr = ioaddr;

	/* Bring old chips out of low-power mode. */
	RTL_W8 (HltClk, 'R');

	/* check for missing/broken hardware */
	if (RTL_R32 (TxConfig) == 0xFFFFFFFF) {
		dev_err(&pdev->dev, "Chip not responding, ignoring board\n");
		rc = -EIO;
		goto err_out;
	}

	/* identify chip attached to board */
	version = RTL_R32 (TxConfig) & HW_REVID_MASK;
	for (i = 0; i < ARRAY_SIZE (rtl_chip_info); i++)
		if (version == rtl_chip_info[i].version) {
			tp->chipset = i;
			goto match;
		}

	/* if unknown chip, assume array element #0, original RTL-8139 in this case */
	i = 0;
	dev_dbg(&pdev->dev, "unknown chip version, assuming RTL-8139\n");
	dev_dbg(&pdev->dev, "TxConfig = 0x%x\n", RTL_R32 (TxConfig));
	tp->chipset = 0;

match:
	pr_debug("chipset id (%d) == index %d, '%s'\n",
		 version, i, rtl_chip_info[i].name);

	if (tp->chipset >= CH_8139B) {
		u8 new_tmp8 = tmp8 = RTL_R8 (Config1);
		pr_debug("PCI PM wakeup\n");
		if ((rtl_chip_info[tp->chipset].flags & HasLWake) &&
		    (tmp8 & LWAKE))
			new_tmp8 &= ~LWAKE;
		new_tmp8 |= Cfg1_PM_Enable;
		if (new_tmp8 != tmp8) {
			RTL_W8 (Cfg9346, Cfg9346_Unlock);
			RTL_W8 (Config1, tmp8);
			RTL_W8 (Cfg9346, Cfg9346_Lock);
		}
		if (rtl_chip_info[tp->chipset].flags & HasLWake) {
			tmp8 = RTL_R8 (Config4);
			if (tmp8 & LWPTN) {
				RTL_W8 (Cfg9346, Cfg9346_Unlock);
				RTL_W8 (Config4, tmp8 & ~LWPTN);
				RTL_W8 (Cfg9346, Cfg9346_Lock);
			}
		}
	} else {
		pr_debug("Old chip wakeup\n");
		tmp8 = RTL_R8 (Config1);
		tmp8 &= ~(SLEEP | PWRDN);
		RTL_W8 (Config1, tmp8);
	}

	rtl8139_chip_reset (ioaddr);

	return dev;

err_out:
	__rtl8139_cleanup_dev (dev);
	if (disable_dev_on_err)
		pci_disable_device (pdev);
	return ERR_PTR(rc);
}

static int rtl8139_set_features(struct net_device *dev, netdev_features_t features)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	unsigned long flags;
	netdev_features_t changed = features ^ dev->features;
	void __iomem *ioaddr = tp->mmio_addr;

	if (!(changed & (NETIF_F_RXALL)))
		return 0;

	spin_lock_irqsave(&tp->lock, flags);

	if (changed & NETIF_F_RXALL) {
		int rx_mode = tp->rx_config;
		if (features & NETIF_F_RXALL)
			rx_mode |= (AcceptErr | AcceptRunt);
		else
			rx_mode &= ~(AcceptErr | AcceptRunt);
		tp->rx_config = rtl8139_rx_config | rx_mode;
		RTL_W32_F(RxConfig, tp->rx_config);
	}

	spin_unlock_irqrestore(&tp->lock, flags);

	return 0;
}

static const struct net_device_ops rtl8139_netdev_ops = {
	.ndo_open		= rtl8139_open,
	.ndo_stop		= rtl8139_close,
	.ndo_get_stats64	= rtl8139_get_stats64,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_mac_address 	= rtl8139_set_mac_address,
	.ndo_start_xmit		= rtl8139_start_xmit,
	.ndo_set_rx_mode	= rtl8139_set_rx_mode,
	.ndo_do_ioctl		= netdev_ioctl,
	.ndo_tx_timeout		= rtl8139_tx_timeout,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= rtl8139_poll_controller,
#endif
	.ndo_set_features	= rtl8139_set_features,
};

static int rtl8139_init_one(struct pci_dev *pdev,
			    const struct pci_device_id *ent)
{
	struct net_device *dev = NULL;
	struct rtl8139_private *tp;
	int i, addr_len, option;
	void __iomem *ioaddr;
	static int board_idx = -1;

	assert (pdev != NULL);
	assert (ent != NULL);

	board_idx++;

	/* when we're built into the kernel, the driver version message
	 * is only printed if at least one 8139 board has been found
	 */
#ifndef MODULE
	{
		static int printed_version;
		if (!printed_version++)
			pr_info(RTL8139_DRIVER_NAME "\n");
	}
#endif

	if (pdev->vendor == PCI_VENDOR_ID_REALTEK &&
	    pdev->device == PCI_DEVICE_ID_REALTEK_8139 && pdev->revision >= 0x20) {
		dev_info(&pdev->dev,
			   "This (id %04x:%04x rev %02x) is an enhanced 8139C+ chip, use 8139cp\n",
		       	   pdev->vendor, pdev->device, pdev->revision);
		return -ENODEV;
	}

	if (pdev->vendor == PCI_VENDOR_ID_REALTEK &&
	    pdev->device == PCI_DEVICE_ID_REALTEK_8139 &&
	    pdev->subsystem_vendor == PCI_VENDOR_ID_ATHEROS &&
	    pdev->subsystem_device == PCI_DEVICE_ID_REALTEK_8139) {
		pr_info("OQO Model 2 detected. Forcing PIO\n");
		use_io = true;
	}

	dev = rtl8139_init_board (pdev);
	if (IS_ERR(dev))
		return PTR_ERR(dev);

	assert (dev != NULL);
	tp = netdev_priv(dev);
	tp->dev = dev;

	ioaddr = tp->mmio_addr;
	assert (ioaddr != NULL);

	addr_len = read_eeprom (ioaddr, 0, 8) == 0x8129 ? 8 : 6;
	for (i = 0; i < 3; i++)
		((__le16 *) (dev->dev_addr))[i] =
		    cpu_to_le16(read_eeprom (ioaddr, i + 7, addr_len));

	/* The Rtl8139-specific entries in the device structure. */
	dev->netdev_ops = &rtl8139_netdev_ops;
	dev->ethtool_ops = &rtl8139_ethtool_ops;
	dev->watchdog_timeo = TX_TIMEOUT;
	netif_napi_add(dev, &tp->napi, rtl8139_poll, 64);

	/* note: the hardware is not capable of sg/csum/highdma, however
	 * through the use of skb_copy_and_csum_dev we enable these
	 * features
	 */
	dev->features |= NETIF_F_SG | NETIF_F_HW_CSUM | NETIF_F_HIGHDMA;
	dev->vlan_features = dev->features;

	dev->hw_features |= NETIF_F_RXALL;
	dev->hw_features |= NETIF_F_RXFCS;

	/* MTU range: 68 - 1770 */
	dev->min_mtu = ETH_MIN_MTU;
	dev->max_mtu = MAX_ETH_DATA_SIZE;

	/* tp zeroed and aligned in alloc_etherdev */
	tp = netdev_priv(dev);

	/* note: tp->chipset set in rtl8139_init_board */
	tp->drv_flags = board_info[ent->driver_data].hw_flags;
	tp->mmio_addr = ioaddr;
	tp->msg_enable =
		(debug < 0 ? RTL8139_DEF_MSG_ENABLE : ((1 << debug) - 1));
	spin_lock_init (&tp->lock);
	spin_lock_init (&tp->rx_lock);
	INIT_DELAYED_WORK(&tp->thread, rtl8139_thread);
	tp->mii.dev = dev;
	tp->mii.mdio_read = mdio_read;
	tp->mii.mdio_write = mdio_write;
	tp->mii.phy_id_mask = 0x3f;
	tp->mii.reg_num_mask = 0x1f;

	/* dev is fully set up and ready to use now */
	pr_debug("about to register device named %s (%p)...\n",
		 dev->name, dev);
	i = register_netdev (dev);
	if (i) goto err_out;

	pci_set_drvdata (pdev, dev);

	netdev_info(dev, "%s at 0x%p, %pM, IRQ %d\n",
		    board_info[ent->driver_data].name,
		    ioaddr, dev->dev_addr, pdev->irq);

	netdev_dbg(dev, "Identified 8139 chip type '%s'\n",
		   rtl_chip_info[tp->chipset].name);

	/* Find the connected MII xcvrs.
	   Doing this in open() would allow detecting external xcvrs later, but
	   takes too much time. */
#ifdef CONFIG_8139TOO_8129
	if (tp->drv_flags & HAS_MII_XCVR) {
		int phy, phy_idx = 0;
		for (phy = 0; phy < 32 && phy_idx < sizeof(tp->phys); phy++) {
			int mii_status = mdio_read(dev, phy, 1);
			if (mii_status != 0xffff  &&  mii_status != 0x0000) {
				u16 advertising = mdio_read(dev, phy, 4);
				tp->phys[phy_idx++] = phy;
				netdev_info(dev, "MII transceiver %d status 0x%04x advertising %04x\n",
					    phy, mii_status, advertising);
			}
		}
		if (phy_idx == 0) {
			netdev_info(dev, "No MII transceivers found! Assuming SYM transceiver\n");
			tp->phys[0] = 32;
		}
	} else
#endif
		tp->phys[0] = 32;
	tp->mii.phy_id = tp->phys[0];

	/* The lower four bits are the media type. */
	option = (board_idx >= MAX_UNITS) ? 0 : media[board_idx];
	if (option > 0) {
		tp->mii.full_duplex = (option & 0x210) ? 1 : 0;
		tp->default_port = option & 0xFF;
		if (tp->default_port)
			tp->mii.force_media = 1;
	}
	if (board_idx < MAX_UNITS  &&  full_duplex[board_idx] > 0)
		tp->mii.full_duplex = full_duplex[board_idx];
	if (tp->mii.full_duplex) {
		netdev_info(dev, "Media type forced to Full Duplex\n");
		/* Changing the MII-advertised media because might prevent
		   re-connection. */
		tp->mii.force_media = 1;
	}
	if (tp->default_port) {
		netdev_info(dev, "  Forcing %dMbps %s-duplex operation\n",
			    (option & 0x20 ? 100 : 10),
			    (option & 0x10 ? "full" : "half"));
		mdio_write(dev, tp->phys[0], 0,
				   ((option & 0x20) ? 0x2000 : 0) | 	/* 100Mbps? */
				   ((option & 0x10) ? 0x0100 : 0)); /* Full duplex? */
	}

	/* Put the chip into low-power mode. */
	if (rtl_chip_info[tp->chipset].flags & HasHltClk)
		RTL_W8 (HltClk, 'H');	/* 'R' would leave the clock running. */

	return 0;

err_out:
	__rtl8139_cleanup_dev (dev);
	pci_disable_device (pdev);
	return i;
}


static void rtl8139_remove_one(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata (pdev);
	struct rtl8139_private *tp = netdev_priv(dev);

	assert (dev != NULL);

	cancel_delayed_work_sync(&tp->thread);

	unregister_netdev (dev);

	__rtl8139_cleanup_dev (dev);
	pci_disable_device (pdev);
}


/* Serial EEPROM section. */

/*  EEPROM_Ctrl bits. */
#define EE_SHIFT_CLK	0x04	/* EEPROM shift clock. */
#define EE_CS			0x08	/* EEPROM chip select. */
#define EE_DATA_WRITE	0x02	/* EEPROM chip data in. */
#define EE_WRITE_0		0x00
#define EE_WRITE_1		0x02
#define EE_DATA_READ	0x01	/* EEPROM chip data out. */
#define EE_ENB			(0x80 | EE_CS)

/* Delay between EEPROM clock transitions.
   No extra delay is needed with 33Mhz PCI, but 66Mhz may change this.
 */

#define eeprom_delay()	(void)RTL_R8(Cfg9346)

/* The EEPROM commands include the alway-set leading bit. */
#define EE_WRITE_CMD	(5)
#define EE_READ_CMD		(6)
#define EE_ERASE_CMD	(7)

static int read_eeprom(void __iomem *ioaddr, int location, int addr_len)
{
	int i;
	unsigned retval = 0;
	int read_cmd = location | (EE_READ_CMD << addr_len);

	RTL_W8 (Cfg9346, EE_ENB & ~EE_CS);
	RTL_W8 (Cfg9346, EE_ENB);
	eeprom_delay ();

	/* Shift the read command bits out. */
	for (i = 4 + addr_len; i >= 0; i--) {
		int dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		RTL_W8 (Cfg9346, EE_ENB | dataval);
		eeprom_delay ();
		RTL_W8 (Cfg9346, EE_ENB | dataval | EE_SHIFT_CLK);
		eeprom_delay ();
	}
	RTL_W8 (Cfg9346, EE_ENB);
	eeprom_delay ();

	for (i = 16; i > 0; i--) {
		RTL_W8 (Cfg9346, EE_ENB | EE_SHIFT_CLK);
		eeprom_delay ();
		retval =
		    (retval << 1) | ((RTL_R8 (Cfg9346) & EE_DATA_READ) ? 1 :
				     0);
		RTL_W8 (Cfg9346, EE_ENB);
		eeprom_delay ();
	}

	/* Terminate the EEPROM access. */
	RTL_W8(Cfg9346, 0);
	eeprom_delay ();

	return retval;
}

/* MII serial management: mostly bogus for now. */
/* Read and write the MII management registers using software-generated
   serial MDIO protocol.
   The maximum data clock rate is 2.5 Mhz.  The minimum timing is usually
   met by back-to-back PCI I/O cycles, but we insert a delay to avoid
   "overclocking" issues. */
#define MDIO_DIR		0x80
#define MDIO_DATA_OUT	0x04
#define MDIO_DATA_IN	0x02
#define MDIO_CLK		0x01
#define MDIO_WRITE0 (MDIO_DIR)
#define MDIO_WRITE1 (MDIO_DIR | MDIO_DATA_OUT)

#define mdio_delay()	RTL_R8(Config4)


static const char mii_2_8139_map[8] = {
	BasicModeCtrl,
	BasicModeStatus,
	0,
	0,
	NWayAdvert,
	NWayLPAR,
	NWayExpansion,
	0
};


#ifdef CONFIG_8139TOO_8129
/* Syncronize the MII management interface by shifting 32 one bits out. */
static void mdio_sync (void __iomem *ioaddr)
{
	int i;

	for (i = 32; i >= 0; i--) {
		RTL_W8 (Config4, MDIO_WRITE1);
		mdio_delay ();
		RTL_W8 (Config4, MDIO_WRITE1 | MDIO_CLK);
		mdio_delay ();
	}
}
#endif

static int mdio_read (struct net_device *dev, int phy_id, int location)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	int retval = 0;
#ifdef CONFIG_8139TOO_8129
	void __iomem *ioaddr = tp->mmio_addr;
	int mii_cmd = (0xf6 << 10) | (phy_id << 5) | location;
	int i;
#endif

	if (phy_id > 31) {	/* Really a 8139.  Use internal registers. */
		void __iomem *ioaddr = tp->mmio_addr;
		return location < 8 && mii_2_8139_map[location] ?
		    RTL_R16 (mii_2_8139_map[location]) : 0;
	}

#ifdef CONFIG_8139TOO_8129
	mdio_sync (ioaddr);
	/* Shift the read command bits out. */
	for (i = 15; i >= 0; i--) {
		int dataval = (mii_cmd & (1 << i)) ? MDIO_DATA_OUT : 0;

		RTL_W8 (Config4, MDIO_DIR | dataval);
		mdio_delay ();
		RTL_W8 (Config4, MDIO_DIR | dataval | MDIO_CLK);
		mdio_delay ();
	}

	/* Read the two transition, 16 data, and wire-idle bits. */
	for (i = 19; i > 0; i--) {
		RTL_W8 (Config4, 0);
		mdio_delay ();
		retval = (retval << 1) | ((RTL_R8 (Config4) & MDIO_DATA_IN) ? 1 : 0);
		RTL_W8 (Config4, MDIO_CLK);
		mdio_delay ();
	}
#endif

	return (retval >> 1) & 0xffff;
}


static void mdio_write (struct net_device *dev, int phy_id, int location,
			int value)
{
	struct rtl8139_private *tp = netdev_priv(dev);
#ifdef CONFIG_8139TOO_8129
	void __iomem *ioaddr = tp->mmio_addr;
	int mii_cmd = (0x5002 << 16) | (phy_id << 23) | (location << 18) | value;
	int i;
#endif

	if (phy_id > 31) {	/* Really a 8139.  Use internal registers. */
		void __iomem *ioaddr = tp->mmio_addr;
		if (location == 0) {
			RTL_W8 (Cfg9346, Cfg9346_Unlock);
			RTL_W16 (BasicModeCtrl, value);
			RTL_W8 (Cfg9346, Cfg9346_Lock);
		} else if (location < 8 && mii_2_8139_map[location])
			RTL_W16 (mii_2_8139_map[location], value);
		return;
	}

#ifdef CONFIG_8139TOO_8129
	mdio_sync (ioaddr);

	/* Shift the command bits out. */
	for (i = 31; i >= 0; i--) {
		int dataval =
		    (mii_cmd & (1 << i)) ? MDIO_WRITE1 : MDIO_WRITE0;
		RTL_W8 (Config4, dataval);
		mdio_delay ();
		RTL_W8 (Config4, dataval | MDIO_CLK);
		mdio_delay ();
	}
	/* Clear out extra bits. */
	for (i = 2; i > 0; i--) {
		RTL_W8 (Config4, 0);
		mdio_delay ();
		RTL_W8 (Config4, MDIO_CLK);
		mdio_delay ();
	}
#endif
}


static int rtl8139_open (struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	const int irq = tp->pci_dev->irq;
	int retval;

	retval = request_irq(irq, rtl8139_interrupt, IRQF_SHARED, dev->name, dev);
	if (retval)
		return retval;

	tp->tx_bufs = dma_alloc_coherent(&tp->pci_dev->dev, TX_BUF_TOT_LEN,
					   &tp->tx_bufs_dma, GFP_KERNEL);
	tp->rx_ring = dma_alloc_coherent(&tp->pci_dev->dev, RX_BUF_TOT_LEN,
					   &tp->rx_ring_dma, GFP_KERNEL);
	if (tp->tx_bufs == NULL || tp->rx_ring == NULL) {
		free_irq(irq, dev);

		if (tp->tx_bufs)
			dma_free_coherent(&tp->pci_dev->dev, TX_BUF_TOT_LEN,
					    tp->tx_bufs, tp->tx_bufs_dma);
		if (tp->rx_ring)
			dma_free_coherent(&tp->pci_dev->dev, RX_BUF_TOT_LEN,
					    tp->rx_ring, tp->rx_ring_dma);

		return -ENOMEM;

	}

	napi_enable(&tp->napi);

	tp->mii.full_duplex = tp->mii.force_media;
	tp->tx_flag = (TX_FIFO_THRESH << 11) & 0x003f0000;

	rtl8139_init_ring (dev);
	rtl8139_hw_start (dev);
	netif_start_queue (dev);

	netif_dbg(tp, ifup, dev,
		  "%s() ioaddr %#llx IRQ %d GP Pins %02x %s-duplex\n",
		  __func__,
		  (unsigned long long)pci_resource_start (tp->pci_dev, 1),
		  irq, RTL_R8 (MediaStatus),
		  tp->mii.full_duplex ? "full" : "half");

	rtl8139_start_thread(tp);

	return 0;
}


static void rtl_check_media (struct net_device *dev, unsigned int init_media)
{
	struct rtl8139_private *tp = netdev_priv(dev);

	if (tp->phys[0] >= 0) {
		mii_check_media(&tp->mii, netif_msg_link(tp), init_media);
	}
}

/* Start the hardware at open or resume. */
static void rtl8139_hw_start (struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	u32 i;
	u8 tmp;

	/* Bring old chips out of low-power mode. */
	if (rtl_chip_info[tp->chipset].flags & HasHltClk)
		RTL_W8 (HltClk, 'R');

	rtl8139_chip_reset (ioaddr);

	/* unlock Config[01234] and BMCR register writes */
	RTL_W8_F (Cfg9346, Cfg9346_Unlock);
	/* Restore our idea of the MAC address. */
	RTL_W32_F (MAC0 + 0, le32_to_cpu (*(__le32 *) (dev->dev_addr + 0)));
	RTL_W32_F (MAC0 + 4, le16_to_cpu (*(__le16 *) (dev->dev_addr + 4)));

	tp->cur_rx = 0;

	/* init Rx ring buffer DMA address */
	RTL_W32_F (RxBuf, tp->rx_ring_dma);

	/* Must enable Tx/Rx before setting transfer thresholds! */
	RTL_W8 (ChipCmd, CmdRxEnb | CmdTxEnb);

	tp->rx_config = rtl8139_rx_config | AcceptBroadcast | AcceptMyPhys;
	RTL_W32 (RxConfig, tp->rx_config);
	RTL_W32 (TxConfig, rtl8139_tx_config);

	rtl_check_media (dev, 1);

	if (tp->chipset >= CH_8139B) {
		/* Disable magic packet scanning, which is enabled
		 * when PM is enabled in Config1.  It can be reenabled
		 * via ETHTOOL_SWOL if desired.  */
		RTL_W8 (Config3, RTL_R8 (Config3) & ~Cfg3_Magic);
	}

	netdev_dbg(dev, "init buffer addresses\n");

	/* Lock Config[01234] and BMCR register writes */
	RTL_W8 (Cfg9346, Cfg9346_Lock);

	/* init Tx buffer DMA addresses */
	for (i = 0; i < NUM_TX_DESC; i++)
		RTL_W32_F (TxAddr0 + (i * 4), tp->tx_bufs_dma + (tp->tx_buf[i] - tp->tx_bufs));

	RTL_W32 (RxMissed, 0);

	rtl8139_set_rx_mode (dev);

	/* no early-rx interrupts */
	RTL_W16 (MultiIntr, RTL_R16 (MultiIntr) & MultiIntrClear);

	/* make sure RxTx has started */
	tmp = RTL_R8 (ChipCmd);
	if ((!(tmp & CmdRxEnb)) || (!(tmp & CmdTxEnb)))
		RTL_W8 (ChipCmd, CmdRxEnb | CmdTxEnb);

	/* Enable all known interrupts by setting the interrupt mask. */
	RTL_W16 (IntrMask, rtl8139_intr_mask);
}


/* Initialize the Rx and Tx rings, along with various 'dev' bits. */
static void rtl8139_init_ring (struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	int i;

	tp->cur_rx = 0;
	tp->cur_tx = 0;
	tp->dirty_tx = 0;

	for (i = 0; i < NUM_TX_DESC; i++)
		tp->tx_buf[i] = &tp->tx_bufs[i * TX_BUF_SIZE];
}


/* This must be global for CONFIG_8139TOO_TUNE_TWISTER case */
static int next_tick = 3 * HZ;

#ifndef CONFIG_8139TOO_TUNE_TWISTER
static inline void rtl8139_tune_twister (struct net_device *dev,
				  struct rtl8139_private *tp) {}
#else
enum TwisterParamVals {
	PARA78_default	= 0x78fa8388,
	PARA7c_default	= 0xcb38de43,	/* param[0][3] */
	PARA7c_xxx	= 0xcb38de43,
};

static const unsigned long param[4][4] = {
	{0xcb39de43, 0xcb39ce43, 0xfb38de03, 0xcb38de43},
	{0xcb39de43, 0xcb39ce43, 0xcb39ce83, 0xcb39ce83},
	{0xcb39de43, 0xcb39ce43, 0xcb39ce83, 0xcb39ce83},
	{0xbb39de43, 0xbb39ce43, 0xbb39ce83, 0xbb39ce83}
};

static void rtl8139_tune_twister (struct net_device *dev,
				  struct rtl8139_private *tp)
{
	int linkcase;
	void __iomem *ioaddr = tp->mmio_addr;

	/* This is a complicated state machine to configure the "twister" for
	   impedance/echos based on the cable length.
	   All of this is magic and undocumented.
	 */
	switch (tp->twistie) {
	case 1:
		if (RTL_R16 (CSCR) & CSCR_LinkOKBit) {
			/* We have link beat, let us tune the twister. */
			RTL_W16 (CSCR, CSCR_LinkDownOffCmd);
			tp->twistie = 2;	/* Change to state 2. */
			next_tick = HZ / 10;
		} else {
			/* Just put in some reasonable defaults for when beat returns. */
			RTL_W16 (CSCR, CSCR_LinkDownCmd);
			RTL_W32 (FIFOTMS, 0x20);	/* Turn on cable test mode. */
			RTL_W32 (PARA78, PARA78_default);
			RTL_W32 (PARA7c, PARA7c_default);
			tp->twistie = 0;	/* Bail from future actions. */
		}
		break;
	case 2:
		/* Read how long it took to hear the echo. */
		linkcase = RTL_R16 (CSCR) & CSCR_LinkStatusBits;
		if (linkcase == 0x7000)
			tp->twist_row = 3;
		else if (linkcase == 0x3000)
			tp->twist_row = 2;
		else if (linkcase == 0x1000)
			tp->twist_row = 1;
		else
			tp->twist_row = 0;
		tp->twist_col = 0;
		tp->twistie = 3;	/* Change to state 2. */
		next_tick = HZ / 10;
		break;
	case 3:
		/* Put out four tuning parameters, one per 100msec. */
		if (tp->twist_col == 0)
			RTL_W16 (FIFOTMS, 0);
		RTL_W32 (PARA7c, param[(int) tp->twist_row]
			 [(int) tp->twist_col]);
		next_tick = HZ / 10;
		if (++tp->twist_col >= 4) {
			/* For short cables we are done.
			   For long cables (row == 3) check for mistune. */
			tp->twistie =
			    (tp->twist_row == 3) ? 4 : 0;
		}
		break;
	case 4:
		/* Special case for long cables: check for mistune. */
		if ((RTL_R16 (CSCR) &
		     CSCR_LinkStatusBits) == 0x7000) {
			tp->twistie = 0;
			break;
		} else {
			RTL_W32 (PARA7c, 0xfb38de03);
			tp->twistie = 5;
			next_tick = HZ / 10;
		}
		break;
	case 5:
		/* Retune for shorter cable (column 2). */
		RTL_W32 (FIFOTMS, 0x20);
		RTL_W32 (PARA78, PARA78_default);
		RTL_W32 (PARA7c, PARA7c_default);
		RTL_W32 (FIFOTMS, 0x00);
		tp->twist_row = 2;
		tp->twist_col = 0;
		tp->twistie = 3;
		next_tick = HZ / 10;
		break;

	default:
		/* do nothing */
		break;
	}
}
#endif /* CONFIG_8139TOO_TUNE_TWISTER */

static inline void rtl8139_thread_iter (struct net_device *dev,
				 struct rtl8139_private *tp,
				 void __iomem *ioaddr)
{
	int mii_lpa;

	mii_lpa = mdio_read (dev, tp->phys[0], MII_LPA);

	if (!tp->mii.force_media && mii_lpa != 0xffff) {
		int duplex = ((mii_lpa & LPA_100FULL) ||
			      (mii_lpa & 0x01C0) == 0x0040);
		if (tp->mii.full_duplex != duplex) {
			tp->mii.full_duplex = duplex;

			if (mii_lpa) {
				netdev_info(dev, "Setting %s-duplex based on MII #%d link partner ability of %04x\n",
					    tp->mii.full_duplex ? "full" : "half",
					    tp->phys[0], mii_lpa);
			} else {
				netdev_info(dev, "media is unconnected, link down, or incompatible connection\n");
			}
#if 0
			RTL_W8 (Cfg9346, Cfg9346_Unlock);
			RTL_W8 (Config1, tp->mii.full_duplex ? 0x60 : 0x20);
			RTL_W8 (Cfg9346, Cfg9346_Lock);
#endif
		}
	}

	next_tick = HZ * 60;

	rtl8139_tune_twister (dev, tp);

	netdev_dbg(dev, "Media selection tick, Link partner %04x\n",
		   RTL_R16(NWayLPAR));
	netdev_dbg(dev, "Other registers are IntMask %04x IntStatus %04x\n",
		   RTL_R16(IntrMask), RTL_R16(IntrStatus));
	netdev_dbg(dev, "Chip config %02x %02x\n",
		   RTL_R8(Config0), RTL_R8(Config1));
}

static void rtl8139_thread (struct work_struct *work)
{
	struct rtl8139_private *tp =
		container_of(work, struct rtl8139_private, thread.work);
	struct net_device *dev = tp->mii.dev;
	unsigned long thr_delay = next_tick;

	rtnl_lock();

	if (!netif_running(dev))
		goto out_unlock;

	if (tp->watchdog_fired) {
		tp->watchdog_fired = 0;
		rtl8139_tx_timeout_task(work);
	} else
		rtl8139_thread_iter(dev, tp, tp->mmio_addr);

	if (tp->have_thread)
		schedule_delayed_work(&tp->thread, thr_delay);
out_unlock:
	rtnl_unlock ();
}

static void rtl8139_start_thread(struct rtl8139_private *tp)
{
	tp->twistie = 0;
	if (tp->chipset == CH_8139_K)
		tp->twistie = 1;
	else if (tp->drv_flags & HAS_LNK_CHNG)
		return;

	tp->have_thread = 1;
	tp->watchdog_fired = 0;

	schedule_delayed_work(&tp->thread, next_tick);
}

static inline void rtl8139_tx_clear (struct rtl8139_private *tp)
{
	tp->cur_tx = 0;
	tp->dirty_tx = 0;

	/* XXX account for unsent Tx packets in tp->stats.tx_dropped */
}

static void rtl8139_tx_timeout_task (struct work_struct *work)
{
	struct rtl8139_private *tp =
		container_of(work, struct rtl8139_private, thread.work);
	struct net_device *dev = tp->mii.dev;
	void __iomem *ioaddr = tp->mmio_addr;
	int i;
	u8 tmp8;

	napi_disable(&tp->napi);
	netif_stop_queue(dev);
	synchronize_rcu();

	netdev_dbg(dev, "Transmit timeout, status %02x %04x %04x media %02x\n",
		   RTL_R8(ChipCmd), RTL_R16(IntrStatus),
		   RTL_R16(IntrMask), RTL_R8(MediaStatus));
	/* Emit info to figure out what went wrong. */
	netdev_dbg(dev, "Tx queue start entry %ld  dirty entry %ld\n",
		   tp->cur_tx, tp->dirty_tx);
	for (i = 0; i < NUM_TX_DESC; i++)
		netdev_dbg(dev, "Tx descriptor %d is %08x%s\n",
			   i, RTL_R32(TxStatus0 + (i * 4)),
			   i == tp->dirty_tx % NUM_TX_DESC ?
			   " (queue head)" : "");

	tp->xstats.tx_timeouts++;

	/* disable Tx ASAP, if not already */
	tmp8 = RTL_R8 (ChipCmd);
	if (tmp8 & CmdTxEnb)
		RTL_W8 (ChipCmd, CmdRxEnb);

	spin_lock_bh(&tp->rx_lock);
	/* Disable interrupts by clearing the interrupt mask. */
	RTL_W16 (IntrMask, 0x0000);

	/* Stop a shared interrupt from scavenging while we are. */
	spin_lock_irq(&tp->lock);
	rtl8139_tx_clear (tp);
	spin_unlock_irq(&tp->lock);

	/* ...and finally, reset everything */
	napi_enable(&tp->napi);
	rtl8139_hw_start(dev);
	netif_wake_queue(dev);

	spin_unlock_bh(&tp->rx_lock);
}

static void rtl8139_tx_timeout(struct net_device *dev, unsigned int txqueue)
{
	struct rtl8139_private *tp = netdev_priv(dev);

	tp->watchdog_fired = 1;
	if (!tp->have_thread) {
		INIT_DELAYED_WORK(&tp->thread, rtl8139_thread);
		schedule_delayed_work(&tp->thread, next_tick);
	}
}

static netdev_tx_t rtl8139_start_xmit (struct sk_buff *skb,
					     struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	unsigned int entry;
	unsigned int len = skb->len;
	unsigned long flags;

	/* Calculate the next Tx descriptor entry. */
	entry = tp->cur_tx % NUM_TX_DESC;

	/* Note: the chip doesn't have auto-pad! */
	if (likely(len < TX_BUF_SIZE)) {
		if (len < ETH_ZLEN)
			memset(tp->tx_buf[entry], 0, ETH_ZLEN);
		skb_copy_and_csum_dev(skb, tp->tx_buf[entry]);
		dev_kfree_skb_any(skb);
	} else {
		dev_kfree_skb_any(skb);
		dev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	spin_lock_irqsave(&tp->lock, flags);
	/*
	 * Writing to TxStatus triggers a DMA transfer of the data
	 * copied to tp->tx_buf[entry] above. Use a memory barrier
	 * to make sure that the device sees the updated data.
	 */
	wmb();
	RTL_W32_F (TxStatus0 + (entry * sizeof (u32)),
		   tp->tx_flag | max(len, (unsigned int)ETH_ZLEN));

	tp->cur_tx++;

	if ((tp->cur_tx - NUM_TX_DESC) == tp->dirty_tx)
		netif_stop_queue (dev);
	spin_unlock_irqrestore(&tp->lock, flags);

	netif_dbg(tp, tx_queued, dev, "Queued Tx packet size %u to slot %d\n",
		  len, entry);

	return NETDEV_TX_OK;
}


static void rtl8139_tx_interrupt (struct net_device *dev,
				  struct rtl8139_private *tp,
				  void __iomem *ioaddr)
{
	unsigned long dirty_tx, tx_left;

	assert (dev != NULL);
	assert (ioaddr != NULL);

	dirty_tx = tp->dirty_tx;
	tx_left = tp->cur_tx - dirty_tx;
	while (tx_left > 0) {
		int entry = dirty_tx % NUM_TX_DESC;
		int txstatus;

		txstatus = RTL_R32 (TxStatus0 + (entry * sizeof (u32)));

		if (!(txstatus & (TxStatOK | TxUnderrun | TxAborted)))
			break;	/* It still hasn't been Txed */

		/* Note: TxCarrierLost is always asserted at 100mbps. */
		if (txstatus & (TxOutOfWindow | TxAborted)) {
			/* There was an major error, log it. */
			netif_dbg(tp, tx_err, dev, "Transmit error, Tx status %08x\n",
				  txstatus);
			dev->stats.tx_errors++;
			if (txstatus & TxAborted) {
				dev->stats.tx_aborted_errors++;
				RTL_W32 (TxConfig, TxClearAbt);
				RTL_W16 (IntrStatus, TxErr);
				wmb();
			}
			if (txstatus & TxCarrierLost)
				dev->stats.tx_carrier_errors++;
			if (txstatus & TxOutOfWindow)
				dev->stats.tx_window_errors++;
		} else {
			if (txstatus & TxUnderrun) {
				/* Add 64 to the Tx FIFO threshold. */
				if (tp->tx_flag < 0x00300000)
					tp->tx_flag += 0x00020000;
				dev->stats.tx_fifo_errors++;
			}
			dev->stats.collisions += (txstatus >> 24) & 15;
			u64_stats_update_begin(&tp->tx_stats.syncp);
			tp->tx_stats.packets++;
			tp->tx_stats.bytes += txstatus & 0x7ff;
			u64_stats_update_end(&tp->tx_stats.syncp);
		}

		dirty_tx++;
		tx_left--;
	}

#ifndef RTL8139_NDEBUG
	if (tp->cur_tx - dirty_tx > NUM_TX_DESC) {
		netdev_err(dev, "Out-of-sync dirty pointer, %ld vs. %ld\n",
			   dirty_tx, tp->cur_tx);
		dirty_tx += NUM_TX_DESC;
	}
#endif /* RTL8139_NDEBUG */

	/* only wake the queue if we did work, and the queue is stopped */
	if (tp->dirty_tx != dirty_tx) {
		tp->dirty_tx = dirty_tx;
		mb();
		netif_wake_queue (dev);
	}
}


/* TODO: clean this up!  Rx reset need not be this intensive */
static void rtl8139_rx_err (u32 rx_status, struct net_device *dev,
			    struct rtl8139_private *tp, void __iomem *ioaddr)
{
	u8 tmp8;
#ifdef CONFIG_8139_OLD_RX_RESET
	int tmp_work;
#endif

	netif_dbg(tp, rx_err, dev, "Ethernet frame had errors, status %08x\n",
		  rx_status);
	dev->stats.rx_errors++;
	if (!(rx_status & RxStatusOK)) {
		if (rx_status & RxTooLong) {
			netdev_dbg(dev, "Oversized Ethernet frame, status %04x!\n",
				   rx_status);
			/* A.C.: The chip hangs here. */
		}
		if (rx_status & (RxBadSymbol | RxBadAlign))
			dev->stats.rx_frame_errors++;
		if (rx_status & (RxRunt | RxTooLong))
			dev->stats.rx_length_errors++;
		if (rx_status & RxCRCErr)
			dev->stats.rx_crc_errors++;
	} else {
		tp->xstats.rx_lost_in_ring++;
	}

#ifndef CONFIG_8139_OLD_RX_RESET
	tmp8 = RTL_R8 (ChipCmd);
	RTL_W8 (ChipCmd, tmp8 & ~CmdRxEnb);
	RTL_W8 (ChipCmd, tmp8);
	RTL_W32 (RxConfig, tp->rx_config);
	tp->cur_rx = 0;
#else
	/* Reset the receiver, based on RealTek recommendation. (Bug?) */

	/* disable receive */
	RTL_W8_F (ChipCmd, CmdTxEnb);
	tmp_work = 200;
	while (--tmp_work > 0) {
		udelay(1);
		tmp8 = RTL_R8 (ChipCmd);
		if (!(tmp8 & CmdRxEnb))
			break;
	}
	if (tmp_work <= 0)
		netdev_warn(dev, "rx stop wait too long\n");
	/* restart receive */
	tmp_work = 200;
	while (--tmp_work > 0) {
		RTL_W8_F (ChipCmd, CmdRxEnb | CmdTxEnb);
		udelay(1);
		tmp8 = RTL_R8 (ChipCmd);
		if ((tmp8 & CmdRxEnb) && (tmp8 & CmdTxEnb))
			break;
	}
	if (tmp_work <= 0)
		netdev_warn(dev, "tx/rx enable wait too long\n");

	/* and reinitialize all rx related registers */
	RTL_W8_F (Cfg9346, Cfg9346_Unlock);
	/* Must enable Tx/Rx before setting transfer thresholds! */
	RTL_W8 (ChipCmd, CmdRxEnb | CmdTxEnb);

	tp->rx_config = rtl8139_rx_config | AcceptBroadcast | AcceptMyPhys;
	RTL_W32 (RxConfig, tp->rx_config);
	tp->cur_rx = 0;

	netdev_dbg(dev, "init buffer addresses\n");

	/* Lock Config[01234] and BMCR register writes */
	RTL_W8 (Cfg9346, Cfg9346_Lock);

	/* init Rx ring buffer DMA address */
	RTL_W32_F (RxBuf, tp->rx_ring_dma);

	/* A.C.: Reset the multicast list. */
	__set_rx_mode (dev);
#endif
}

#if RX_BUF_IDX == 3
static inline void wrap_copy(struct sk_buff *skb, const unsigned char *ring,
				 u32 offset, unsigned int size)
{
	u32 left = RX_BUF_LEN - offset;

	if (size > left) {
		skb_copy_to_linear_data(skb, ring + offset, left);
		skb_copy_to_linear_data_offset(skb, left, ring, size - left);
	} else
		skb_copy_to_linear_data(skb, ring + offset, size);
}
#endif

static void rtl8139_isr_ack(struct rtl8139_private *tp)
{
	void __iomem *ioaddr = tp->mmio_addr;
	u16 status;

	status = RTL_R16 (IntrStatus) & RxAckBits;

	/* Clear out errors and receive interrupts */
	if (likely(status != 0)) {
		if (unlikely(status & (RxFIFOOver | RxOverflow))) {
			tp->dev->stats.rx_errors++;
			if (status & RxFIFOOver)
				tp->dev->stats.rx_fifo_errors++;
		}
		RTL_W16_F (IntrStatus, RxAckBits);
	}
}

static int rtl8139_rx(struct net_device *dev, struct rtl8139_private *tp,
		      int budget)
{
	void __iomem *ioaddr = tp->mmio_addr;
	int received = 0;
	unsigned char *rx_ring = tp->rx_ring;
	unsigned int cur_rx = tp->cur_rx;
	unsigned int rx_size = 0;

	netdev_dbg(dev, "In %s(), current %04x BufAddr %04x, free to %04x, Cmd %02x\n",
		   __func__, (u16)cur_rx,
		   RTL_R16(RxBufAddr), RTL_R16(RxBufPtr), RTL_R8(ChipCmd));

	while (netif_running(dev) && received < budget &&
	       (RTL_R8 (ChipCmd) & RxBufEmpty) == 0) {
		u32 ring_offset = cur_rx % RX_BUF_LEN;
		u32 rx_status;
		unsigned int pkt_size;
		struct sk_buff *skb;

		rmb();

		/* read size+status of next frame from DMA ring buffer */
		rx_status = le32_to_cpu (*(__le32 *) (rx_ring + ring_offset));
		rx_size = rx_status >> 16;
		if (likely(!(dev->features & NETIF_F_RXFCS)))
			pkt_size = rx_size - 4;
		else
			pkt_size = rx_size;

		netif_dbg(tp, rx_status, dev, "%s() status %04x, size %04x, cur %04x\n",
			  __func__, rx_status, rx_size, cur_rx);
#if RTL8139_DEBUG > 2
		print_hex_dump(KERN_DEBUG, "Frame contents: ",
			       DUMP_PREFIX_OFFSET, 16, 1,
			       &rx_ring[ring_offset], 70, true);
#endif

		/* Packet copy from FIFO still in progress.
		 * Theoretically, this should never happen
		 * since EarlyRx is disabled.
		 */
		if (unlikely(rx_size == 0xfff0)) {
			if (!tp->fifo_copy_timeout)
				tp->fifo_copy_timeout = jiffies + 2;
			else if (time_after(jiffies, tp->fifo_copy_timeout)) {
				netdev_dbg(dev, "hung FIFO. Reset\n");
				rx_size = 0;
				goto no_early_rx;
			}
			netif_dbg(tp, intr, dev, "fifo copy in progress\n");
			tp->xstats.early_rx++;
			break;
		}

no_early_rx:
		tp->fifo_copy_timeout = 0;

		/* If Rx err or invalid rx_size/rx_status received
		 * (which happens if we get lost in the ring),
		 * Rx process gets reset, so we abort any further
		 * Rx processing.
		 */
		if (unlikely((rx_size > (MAX_ETH_FRAME_SIZE+4)) ||
			     (rx_size < 8) ||
			     (!(rx_status & RxStatusOK)))) {
			if ((dev->features & NETIF_F_RXALL) &&
			    (rx_size <= (MAX_ETH_FRAME_SIZE + 4)) &&
			    (rx_size >= 8) &&
			    (!(rx_status & RxStatusOK))) {
				/* Length is at least mostly OK, but pkt has
				 * error.  I'm hoping we can handle some of these
				 * errors without resetting the chip. --Ben
				 */
				dev->stats.rx_errors++;
				if (rx_status & RxCRCErr) {
					dev->stats.rx_crc_errors++;
					goto keep_pkt;
				}
				if (rx_status & RxRunt) {
					dev->stats.rx_length_errors++;
					goto keep_pkt;
				}
			}
			rtl8139_rx_err (rx_status, dev, tp, ioaddr);
			received = -1;
			goto out;
		}

keep_pkt:
		/* Malloc up new buffer, compatible with net-2e. */
		/* Omit the four octet CRC from the length. */

		skb = napi_alloc_skb(&tp->napi, pkt_size);
		if (likely(skb)) {
#if RX_BUF_IDX == 3
			wrap_copy(skb, rx_ring, ring_offset+4, pkt_size);
#else
			skb_copy_to_linear_data (skb, &rx_ring[ring_offset + 4], pkt_size);
#endif
			skb_put (skb, pkt_size);

			skb->protocol = eth_type_trans (skb, dev);

			u64_stats_update_begin(&tp->rx_stats.syncp);
			tp->rx_stats.packets++;
			tp->rx_stats.bytes += pkt_size;
			u64_stats_update_end(&tp->rx_stats.syncp);

			netif_receive_skb (skb);
		} else {
			dev->stats.rx_dropped++;
		}
		received++;

		cur_rx = (cur_rx + rx_size + 4 + 3) & ~3;
		RTL_W16 (RxBufPtr, (u16) (cur_rx - 16));

		rtl8139_isr_ack(tp);
	}

	if (unlikely(!received || rx_size == 0xfff0))
		rtl8139_isr_ack(tp);

	netdev_dbg(dev, "Done %s(), current %04x BufAddr %04x, free to %04x, Cmd %02x\n",
		   __func__, cur_rx,
		   RTL_R16(RxBufAddr), RTL_R16(RxBufPtr), RTL_R8(ChipCmd));

	tp->cur_rx = cur_rx;

	/*
	 * The receive buffer should be mostly empty.
	 * Tell NAPI to reenable the Rx irq.
	 */
	if (tp->fifo_copy_timeout)
		received = budget;

out:
	return received;
}


static void rtl8139_weird_interrupt (struct net_device *dev,
				     struct rtl8139_private *tp,
				     void __iomem *ioaddr,
				     int status, int link_changed)
{
	netdev_dbg(dev, "Abnormal interrupt, status %08x\n", status);

	assert (dev != NULL);
	assert (tp != NULL);
	assert (ioaddr != NULL);

	/* Update the error count. */
	dev->stats.rx_missed_errors += RTL_R32 (RxMissed);
	RTL_W32 (RxMissed, 0);

	if ((status & RxUnderrun) && link_changed &&
	    (tp->drv_flags & HAS_LNK_CHNG)) {
		rtl_check_media(dev, 0);
		status &= ~RxUnderrun;
	}

	if (status & (RxUnderrun | RxErr))
		dev->stats.rx_errors++;

	if (status & PCSTimeout)
		dev->stats.rx_length_errors++;
	if (status & RxUnderrun)
		dev->stats.rx_fifo_errors++;
	if (status & PCIErr) {
		u16 pci_cmd_status;
		pci_read_config_word (tp->pci_dev, PCI_STATUS, &pci_cmd_status);
		pci_write_config_word (tp->pci_dev, PCI_STATUS, pci_cmd_status);

		netdev_err(dev, "PCI Bus error %04x\n", pci_cmd_status);
	}
}

static int rtl8139_poll(struct napi_struct *napi, int budget)
{
	struct rtl8139_private *tp = container_of(napi, struct rtl8139_private, napi);
	struct net_device *dev = tp->dev;
	void __iomem *ioaddr = tp->mmio_addr;
	int work_done;

	spin_lock(&tp->rx_lock);
	work_done = 0;
	if (likely(RTL_R16(IntrStatus) & RxAckBits))
		work_done += rtl8139_rx(dev, tp, budget);

	if (work_done < budget) {
		unsigned long flags;

		spin_lock_irqsave(&tp->lock, flags);
		if (napi_complete_done(napi, work_done))
			RTL_W16_F(IntrMask, rtl8139_intr_mask);
		spin_unlock_irqrestore(&tp->lock, flags);
	}
	spin_unlock(&tp->rx_lock);

	return work_done;
}

/* The interrupt handler does all of the Rx thread work and cleans up
   after the Tx thread. */
static irqreturn_t rtl8139_interrupt (int irq, void *dev_instance)
{
	struct net_device *dev = (struct net_device *) dev_instance;
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	u16 status, ackstat;
	int link_changed = 0; /* avoid bogus "uninit" warning */
	int handled = 0;

	spin_lock (&tp->lock);
	status = RTL_R16 (IntrStatus);

	/* shared irq? */
	if (unlikely((status & rtl8139_intr_mask) == 0))
		goto out;

	handled = 1;

	/* h/w no longer present (hotplug?) or major error, bail */
	if (unlikely(status == 0xFFFF))
		goto out;

	/* close possible race's with dev_close */
	if (unlikely(!netif_running(dev))) {
		RTL_W16 (IntrMask, 0);
		goto out;
	}

	/* Acknowledge all of the current interrupt sources ASAP, but
	   an first get an additional status bit from CSCR. */
	if (unlikely(status & RxUnderrun))
		link_changed = RTL_R16 (CSCR) & CSCR_LinkChangeBit;

	ackstat = status & ~(RxAckBits | TxErr);
	if (ackstat)
		RTL_W16 (IntrStatus, ackstat);

	/* Receive packets are processed by poll routine.
	   If not running start it now. */
	if (status & RxAckBits){
		if (napi_schedule_prep(&tp->napi)) {
			RTL_W16_F (IntrMask, rtl8139_norx_intr_mask);
			__napi_schedule(&tp->napi);
		}
	}

	/* Check uncommon events with one test. */
	if (unlikely(status & (PCIErr | PCSTimeout | RxUnderrun | RxErr)))
		rtl8139_weird_interrupt (dev, tp, ioaddr,
					 status, link_changed);

	if (status & (TxOK | TxErr)) {
		rtl8139_tx_interrupt (dev, tp, ioaddr);
		if (status & TxErr)
			RTL_W16 (IntrStatus, TxErr);
	}
 out:
	spin_unlock (&tp->lock);

	netdev_dbg(dev, "exiting interrupt, intr_status=%#4.4x\n",
		   RTL_R16(IntrStatus));
	return IRQ_RETVAL(handled);
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/*
 * Polling receive - used by netconsole and other diagnostic tools
 * to allow network i/o with interrupts disabled.
 */
static void rtl8139_poll_controller(struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	const int irq = tp->pci_dev->irq;

	disable_irq_nosync(irq);
	rtl8139_interrupt(irq, dev);
	enable_irq(irq);
}
#endif

static int rtl8139_set_mac_address(struct net_device *dev, void *p)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	spin_lock_irq(&tp->lock);

	RTL_W8_F(Cfg9346, Cfg9346_Unlock);
	RTL_W32_F(MAC0 + 0, cpu_to_le32 (*(u32 *) (dev->dev_addr + 0)));
	RTL_W32_F(MAC0 + 4, cpu_to_le32 (*(u32 *) (dev->dev_addr + 4)));
	RTL_W8_F(Cfg9346, Cfg9346_Lock);

	spin_unlock_irq(&tp->lock);

	return 0;
}

static int rtl8139_close (struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	unsigned long flags;

	netif_stop_queue(dev);
	napi_disable(&tp->napi);

	netif_dbg(tp, ifdown, dev, "Shutting down ethercard, status was 0x%04x\n",
		  RTL_R16(IntrStatus));

	spin_lock_irqsave (&tp->lock, flags);

	/* Stop the chip's Tx and Rx DMA processes. */
	RTL_W8 (ChipCmd, 0);

	/* Disable interrupts by clearing the interrupt mask. */
	RTL_W16 (IntrMask, 0);

	/* Update the error counts. */
	dev->stats.rx_missed_errors += RTL_R32 (RxMissed);
	RTL_W32 (RxMissed, 0);

	spin_unlock_irqrestore (&tp->lock, flags);

	free_irq(tp->pci_dev->irq, dev);

	rtl8139_tx_clear (tp);

	dma_free_coherent(&tp->pci_dev->dev, RX_BUF_TOT_LEN,
			  tp->rx_ring, tp->rx_ring_dma);
	dma_free_coherent(&tp->pci_dev->dev, TX_BUF_TOT_LEN,
			  tp->tx_bufs, tp->tx_bufs_dma);
	tp->rx_ring = NULL;
	tp->tx_bufs = NULL;

	/* Green! Put the chip in low-power mode. */
	RTL_W8 (Cfg9346, Cfg9346_Unlock);

	if (rtl_chip_info[tp->chipset].flags & HasHltClk)
		RTL_W8 (HltClk, 'H');	/* 'R' would leave the clock running. */

	return 0;
}


/* Get the ethtool Wake-on-LAN settings.  Assumes that wol points to
   kernel memory, *wol has been initialized as {ETHTOOL_GWOL}, and
   other threads or interrupts aren't messing with the 8139.  */
static void rtl8139_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;

	spin_lock_irq(&tp->lock);
	if (rtl_chip_info[tp->chipset].flags & HasLWake) {
		u8 cfg3 = RTL_R8 (Config3);
		u8 cfg5 = RTL_R8 (Config5);

		wol->supported = WAKE_PHY | WAKE_MAGIC
			| WAKE_UCAST | WAKE_MCAST | WAKE_BCAST;

		wol->wolopts = 0;
		if (cfg3 & Cfg3_LinkUp)
			wol->wolopts |= WAKE_PHY;
		if (cfg3 & Cfg3_Magic)
			wol->wolopts |= WAKE_MAGIC;
		/* (KON)FIXME: See how netdev_set_wol() handles the
		   following constants.  */
		if (cfg5 & Cfg5_UWF)
			wol->wolopts |= WAKE_UCAST;
		if (cfg5 & Cfg5_MWF)
			wol->wolopts |= WAKE_MCAST;
		if (cfg5 & Cfg5_BWF)
			wol->wolopts |= WAKE_BCAST;
	}
	spin_unlock_irq(&tp->lock);
}


/* Set the ethtool Wake-on-LAN settings.  Return 0 or -errno.  Assumes
   that wol points to kernel memory and other threads or interrupts
   aren't messing with the 8139.  */
static int rtl8139_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	u32 support;
	u8 cfg3, cfg5;

	support = ((rtl_chip_info[tp->chipset].flags & HasLWake)
		   ? (WAKE_PHY | WAKE_MAGIC
		      | WAKE_UCAST | WAKE_MCAST | WAKE_BCAST)
		   : 0);
	if (wol->wolopts & ~support)
		return -EINVAL;

	spin_lock_irq(&tp->lock);
	cfg3 = RTL_R8 (Config3) & ~(Cfg3_LinkUp | Cfg3_Magic);
	if (wol->wolopts & WAKE_PHY)
		cfg3 |= Cfg3_LinkUp;
	if (wol->wolopts & WAKE_MAGIC)
		cfg3 |= Cfg3_Magic;
	RTL_W8 (Cfg9346, Cfg9346_Unlock);
	RTL_W8 (Config3, cfg3);
	RTL_W8 (Cfg9346, Cfg9346_Lock);

	cfg5 = RTL_R8 (Config5) & ~(Cfg5_UWF | Cfg5_MWF | Cfg5_BWF);
	/* (KON)FIXME: These are untested.  We may have to set the
	   CRC0, Wakeup0 and LSBCRC0 registers too, but I have no
	   documentation.  */
	if (wol->wolopts & WAKE_UCAST)
		cfg5 |= Cfg5_UWF;
	if (wol->wolopts & WAKE_MCAST)
		cfg5 |= Cfg5_MWF;
	if (wol->wolopts & WAKE_BCAST)
		cfg5 |= Cfg5_BWF;
	RTL_W8 (Config5, cfg5);	/* need not unlock via Cfg9346 */
	spin_unlock_irq(&tp->lock);

	return 0;
}

static void rtl8139_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	strlcpy(info->version, DRV_VERSION, sizeof(info->version));
	strlcpy(info->bus_info, pci_name(tp->pci_dev), sizeof(info->bus_info));
}

static int rtl8139_get_link_ksettings(struct net_device *dev,
				      struct ethtool_link_ksettings *cmd)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	spin_lock_irq(&tp->lock);
	mii_ethtool_get_link_ksettings(&tp->mii, cmd);
	spin_unlock_irq(&tp->lock);
	return 0;
}

static int rtl8139_set_link_ksettings(struct net_device *dev,
				      const struct ethtool_link_ksettings *cmd)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	int rc;
	spin_lock_irq(&tp->lock);
	rc = mii_ethtool_set_link_ksettings(&tp->mii, cmd);
	spin_unlock_irq(&tp->lock);
	return rc;
}

static int rtl8139_nway_reset(struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	return mii_nway_restart(&tp->mii);
}

static u32 rtl8139_get_link(struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	return mii_link_ok(&tp->mii);
}

static u32 rtl8139_get_msglevel(struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	return tp->msg_enable;
}

static void rtl8139_set_msglevel(struct net_device *dev, u32 datum)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	tp->msg_enable = datum;
}

static int rtl8139_get_regs_len(struct net_device *dev)
{
	struct rtl8139_private *tp;
	/* TODO: we are too slack to do reg dumping for pio, for now */
	if (use_io)
		return 0;
	tp = netdev_priv(dev);
	return tp->regs_len;
}

static void rtl8139_get_regs(struct net_device *dev, struct ethtool_regs *regs, void *regbuf)
{
	struct rtl8139_private *tp;

	/* TODO: we are too slack to do reg dumping for pio, for now */
	if (use_io)
		return;
	tp = netdev_priv(dev);

	regs->version = RTL_REGS_VER;

	spin_lock_irq(&tp->lock);
	memcpy_fromio(regbuf, tp->mmio_addr, regs->len);
	spin_unlock_irq(&tp->lock);
}

static int rtl8139_get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return RTL_NUM_STATS;
	default:
		return -EOPNOTSUPP;
	}
}

static void rtl8139_get_ethtool_stats(struct net_device *dev, struct ethtool_stats *stats, u64 *data)
{
	struct rtl8139_private *tp = netdev_priv(dev);

	data[0] = tp->xstats.early_rx;
	data[1] = tp->xstats.tx_buf_mapped;
	data[2] = tp->xstats.tx_timeouts;
	data[3] = tp->xstats.rx_lost_in_ring;
}

static void rtl8139_get_strings(struct net_device *dev, u32 stringset, u8 *data)
{
	memcpy(data, ethtool_stats_keys, sizeof(ethtool_stats_keys));
}

static const struct ethtool_ops rtl8139_ethtool_ops = {
	.get_drvinfo		= rtl8139_get_drvinfo,
	.get_regs_len		= rtl8139_get_regs_len,
	.get_regs		= rtl8139_get_regs,
	.nway_reset		= rtl8139_nway_reset,
	.get_link		= rtl8139_get_link,
	.get_msglevel		= rtl8139_get_msglevel,
	.set_msglevel		= rtl8139_set_msglevel,
	.get_wol		= rtl8139_get_wol,
	.set_wol		= rtl8139_set_wol,
	.get_strings		= rtl8139_get_strings,
	.get_sset_count		= rtl8139_get_sset_count,
	.get_ethtool_stats	= rtl8139_get_ethtool_stats,
	.get_link_ksettings	= rtl8139_get_link_ksettings,
	.set_link_ksettings	= rtl8139_set_link_ksettings,
};

static int netdev_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	int rc;

	if (!netif_running(dev))
		return -EINVAL;

	spin_lock_irq(&tp->lock);
	rc = generic_mii_ioctl(&tp->mii, if_mii(rq), cmd, NULL);
	spin_unlock_irq(&tp->lock);

	return rc;
}


static void
rtl8139_get_stats64(struct net_device *dev, struct rtnl_link_stats64 *stats)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	unsigned long flags;
	unsigned int start;

	if (netif_running(dev)) {
		spin_lock_irqsave (&tp->lock, flags);
		dev->stats.rx_missed_errors += RTL_R32 (RxMissed);
		RTL_W32 (RxMissed, 0);
		spin_unlock_irqrestore (&tp->lock, flags);
	}

	netdev_stats_to_stats64(stats, &dev->stats);

	do {
		start = u64_stats_fetch_begin_irq(&tp->rx_stats.syncp);
		stats->rx_packets = tp->rx_stats.packets;
		stats->rx_bytes = tp->rx_stats.bytes;
	} while (u64_stats_fetch_retry_irq(&tp->rx_stats.syncp, start));

	do {
		start = u64_stats_fetch_begin_irq(&tp->tx_stats.syncp);
		stats->tx_packets = tp->tx_stats.packets;
		stats->tx_bytes = tp->tx_stats.bytes;
	} while (u64_stats_fetch_retry_irq(&tp->tx_stats.syncp, start));
}

/* Set or clear the multicast filter for this adaptor.
   This routine is not state sensitive and need not be SMP locked. */

static void __set_rx_mode (struct net_device *dev)
{
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	u32 mc_filter[2];	/* Multicast hash filter */
	int rx_mode;
	u32 tmp;

	netdev_dbg(dev, "rtl8139_set_rx_mode(%04x) done -- Rx config %08x\n",
		   dev->flags, RTL_R32(RxConfig));

	/* Note: do not reorder, GCC is clever about common statements. */
	if (dev->flags & IFF_PROMISC) {
		rx_mode =
		    AcceptBroadcast | AcceptMulticast | AcceptMyPhys |
		    AcceptAllPhys;
		mc_filter[1] = mc_filter[0] = 0xffffffff;
	} else if ((netdev_mc_count(dev) > multicast_filter_limit) ||
		   (dev->flags & IFF_ALLMULTI)) {
		/* Too many to filter perfectly -- accept all multicasts. */
		rx_mode = AcceptBroadcast | AcceptMulticast | AcceptMyPhys;
		mc_filter[1] = mc_filter[0] = 0xffffffff;
	} else {
		struct netdev_hw_addr *ha;
		rx_mode = AcceptBroadcast | AcceptMyPhys;
		mc_filter[1] = mc_filter[0] = 0;
		netdev_for_each_mc_addr(ha, dev) {
			int bit_nr = ether_crc(ETH_ALEN, ha->addr) >> 26;

			mc_filter[bit_nr >> 5] |= 1 << (bit_nr & 31);
			rx_mode |= AcceptMulticast;
		}
	}

	if (dev->features & NETIF_F_RXALL)
		rx_mode |= (AcceptErr | AcceptRunt);

	/* We can safely update without stopping the chip. */
	tmp = rtl8139_rx_config | rx_mode;
	if (tp->rx_config != tmp) {
		RTL_W32_F (RxConfig, tmp);
		tp->rx_config = tmp;
	}
	RTL_W32_F (MAR0 + 0, mc_filter[0]);
	RTL_W32_F (MAR0 + 4, mc_filter[1]);
}

static void rtl8139_set_rx_mode (struct net_device *dev)
{
	unsigned long flags;
	struct rtl8139_private *tp = netdev_priv(dev);

	spin_lock_irqsave (&tp->lock, flags);
	__set_rx_mode(dev);
	spin_unlock_irqrestore (&tp->lock, flags);
}

static int __maybe_unused rtl8139_suspend(struct device *device)
{
	struct net_device *dev = dev_get_drvdata(device);
	struct rtl8139_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->mmio_addr;
	unsigned long flags;

	if (!netif_running (dev))
		return 0;

	netif_device_detach (dev);

	spin_lock_irqsave (&tp->lock, flags);

	/* Disable interrupts, stop Tx and Rx. */
	RTL_W16 (IntrMask, 0);
	RTL_W8 (ChipCmd, 0);

	/* Update the error counts. */
	dev->stats.rx_missed_errors += RTL_R32 (RxMissed);
	RTL_W32 (RxMissed, 0);

	spin_unlock_irqrestore (&tp->lock, flags);

	return 0;
}

static int __maybe_unused rtl8139_resume(struct device *device)
{
	struct net_device *dev = dev_get_drvdata(device);

	if (!netif_running (dev))
		return 0;

	rtl8139_init_ring (dev);
	rtl8139_hw_start (dev);
	netif_device_attach (dev);
	return 0;
}

static SIMPLE_DEV_PM_OPS(rtl8139_pm_ops, rtl8139_suspend, rtl8139_resume);

static struct pci_driver rtl8139_pci_driver = {
	.name		= DRV_NAME,
	.id_table	= rtl8139_pci_tbl,
	.probe		= rtl8139_init_one,
	.remove		= rtl8139_remove_one,
	.driver.pm	= &rtl8139_pm_ops,
};


static int __init rtl8139_init_module (void)
{
	/* when we're a module, we always print a version message,
	 * even if no 8139 board is found.
	 */
#ifdef MODULE
	pr_info(RTL8139_DRIVER_NAME "\n");
#endif

	return pci_register_driver(&rtl8139_pci_driver);
}


static void __exit rtl8139_cleanup_module (void)
{
	pci_unregister_driver (&rtl8139_pci_driver);
}


module_init(rtl8139_init_module);
module_exit(rtl8139_cleanup_module);
