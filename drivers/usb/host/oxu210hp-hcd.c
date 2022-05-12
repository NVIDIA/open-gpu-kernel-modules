// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2008 Rodolfo Giometti <giometti@linux.it>
 * Copyright (c) 2008 Eurotech S.p.A. <info@eurtech.it>
 *
 * This code is *strongly* based on EHCI-HCD code by David Brownell since
 * the chip is a quasi-EHCI compatible.
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/dmapool.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/moduleparam.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/iopoll.h>

#include <asm/irq.h>
#include <asm/unaligned.h>

#include <linux/irq.h>
#include <linux/platform_device.h>

#define DRIVER_VERSION "0.0.50"

#define OXU_DEVICEID			0x00
	#define OXU_REV_MASK		0xffff0000
	#define OXU_REV_SHIFT		16
	#define OXU_REV_2100		0x2100
	#define OXU_BO_SHIFT		8
	#define OXU_BO_MASK		(0x3 << OXU_BO_SHIFT)
	#define OXU_MAJ_REV_SHIFT	4
	#define OXU_MAJ_REV_MASK	(0xf << OXU_MAJ_REV_SHIFT)
	#define OXU_MIN_REV_SHIFT	0
	#define OXU_MIN_REV_MASK	(0xf << OXU_MIN_REV_SHIFT)
#define OXU_HOSTIFCONFIG		0x04
#define OXU_SOFTRESET			0x08
	#define OXU_SRESET		(1 << 0)

#define OXU_PIOBURSTREADCTRL		0x0C

#define OXU_CHIPIRQSTATUS		0x10
#define OXU_CHIPIRQEN_SET		0x14
#define OXU_CHIPIRQEN_CLR		0x18
	#define OXU_USBSPHLPWUI		0x00000080
	#define OXU_USBOTGLPWUI		0x00000040
	#define OXU_USBSPHI		0x00000002
	#define OXU_USBOTGI		0x00000001

#define OXU_CLKCTRL_SET			0x1C
	#define OXU_SYSCLKEN		0x00000008
	#define OXU_USBSPHCLKEN		0x00000002
	#define OXU_USBOTGCLKEN		0x00000001

#define OXU_ASO				0x68
	#define OXU_SPHPOEN		0x00000100
	#define OXU_OVRCCURPUPDEN	0x00000800
	#define OXU_ASO_OP		(1 << 10)
	#define OXU_COMPARATOR		0x000004000

#define OXU_USBMODE			0x1A8
	#define OXU_VBPS		0x00000020
	#define OXU_ES_LITTLE		0x00000000
	#define OXU_CM_HOST_ONLY	0x00000003

/*
 * Proper EHCI structs & defines
 */

/* Magic numbers that can affect system performance */
#define EHCI_TUNE_CERR		3	/* 0-3 qtd retries; 0 == don't stop */
#define EHCI_TUNE_RL_HS		4	/* nak throttle; see 4.9 */
#define EHCI_TUNE_RL_TT		0
#define EHCI_TUNE_MULT_HS	1	/* 1-3 transactions/uframe; 4.10.3 */
#define EHCI_TUNE_MULT_TT	1
#define EHCI_TUNE_FLS		2	/* (small) 256 frame schedule */

struct oxu_hcd;

/* EHCI register interface, corresponds to EHCI Revision 0.95 specification */

/* Section 2.2 Host Controller Capability Registers */
struct ehci_caps {
	/* these fields are specified as 8 and 16 bit registers,
	 * but some hosts can't perform 8 or 16 bit PCI accesses.
	 */
	u32		hc_capbase;
#define HC_LENGTH(p)		(((p)>>00)&0x00ff)	/* bits 7:0 */
#define HC_VERSION(p)		(((p)>>16)&0xffff)	/* bits 31:16 */
	u32		hcs_params;     /* HCSPARAMS - offset 0x4 */
#define HCS_DEBUG_PORT(p)	(((p)>>20)&0xf)	/* bits 23:20, debug port? */
#define HCS_INDICATOR(p)	((p)&(1 << 16))	/* true: has port indicators */
#define HCS_N_CC(p)		(((p)>>12)&0xf)	/* bits 15:12, #companion HCs */
#define HCS_N_PCC(p)		(((p)>>8)&0xf)	/* bits 11:8, ports per CC */
#define HCS_PORTROUTED(p)	((p)&(1 << 7))	/* true: port routing */
#define HCS_PPC(p)		((p)&(1 << 4))	/* true: port power control */
#define HCS_N_PORTS(p)		(((p)>>0)&0xf)	/* bits 3:0, ports on HC */

	u32		hcc_params;      /* HCCPARAMS - offset 0x8 */
#define HCC_EXT_CAPS(p)		(((p)>>8)&0xff)	/* for pci extended caps */
#define HCC_ISOC_CACHE(p)       ((p)&(1 << 7))  /* true: can cache isoc frame */
#define HCC_ISOC_THRES(p)       (((p)>>4)&0x7)  /* bits 6:4, uframes cached */
#define HCC_CANPARK(p)		((p)&(1 << 2))  /* true: can park on async qh */
#define HCC_PGM_FRAMELISTLEN(p) ((p)&(1 << 1))  /* true: periodic_size changes*/
#define HCC_64BIT_ADDR(p)       ((p)&(1))       /* true: can use 64-bit addr */
	u8		portroute[8];	 /* nibbles for routing - offset 0xC */
} __packed;


/* Section 2.3 Host Controller Operational Registers */
struct ehci_regs {
	/* USBCMD: offset 0x00 */
	u32		command;
/* 23:16 is r/w intr rate, in microframes; default "8" == 1/msec */
#define CMD_PARK	(1<<11)		/* enable "park" on async qh */
#define CMD_PARK_CNT(c)	(((c)>>8)&3)	/* how many transfers to park for */
#define CMD_LRESET	(1<<7)		/* partial reset (no ports, etc) */
#define CMD_IAAD	(1<<6)		/* "doorbell" interrupt async advance */
#define CMD_ASE		(1<<5)		/* async schedule enable */
#define CMD_PSE		(1<<4)		/* periodic schedule enable */
/* 3:2 is periodic frame list size */
#define CMD_RESET	(1<<1)		/* reset HC not bus */
#define CMD_RUN		(1<<0)		/* start/stop HC */

	/* USBSTS: offset 0x04 */
	u32		status;
#define STS_ASS		(1<<15)		/* Async Schedule Status */
#define STS_PSS		(1<<14)		/* Periodic Schedule Status */
#define STS_RECL	(1<<13)		/* Reclamation */
#define STS_HALT	(1<<12)		/* Not running (any reason) */
/* some bits reserved */
	/* these STS_* flags are also intr_enable bits (USBINTR) */
#define STS_IAA		(1<<5)		/* Interrupted on async advance */
#define STS_FATAL	(1<<4)		/* such as some PCI access errors */
#define STS_FLR		(1<<3)		/* frame list rolled over */
#define STS_PCD		(1<<2)		/* port change detect */
#define STS_ERR		(1<<1)		/* "error" completion (overflow, ...) */
#define STS_INT		(1<<0)		/* "normal" completion (short, ...) */

#define INTR_MASK (STS_IAA | STS_FATAL | STS_PCD | STS_ERR | STS_INT)

	/* USBINTR: offset 0x08 */
	u32		intr_enable;

	/* FRINDEX: offset 0x0C */
	u32		frame_index;	/* current microframe number */
	/* CTRLDSSEGMENT: offset 0x10 */
	u32		segment;	/* address bits 63:32 if needed */
	/* PERIODICLISTBASE: offset 0x14 */
	u32		frame_list;	/* points to periodic list */
	/* ASYNCLISTADDR: offset 0x18 */
	u32		async_next;	/* address of next async queue head */

	u32		reserved[9];

	/* CONFIGFLAG: offset 0x40 */
	u32		configured_flag;
#define FLAG_CF		(1<<0)		/* true: we'll support "high speed" */

	/* PORTSC: offset 0x44 */
	u32		port_status[0];	/* up to N_PORTS */
/* 31:23 reserved */
#define PORT_WKOC_E	(1<<22)		/* wake on overcurrent (enable) */
#define PORT_WKDISC_E	(1<<21)		/* wake on disconnect (enable) */
#define PORT_WKCONN_E	(1<<20)		/* wake on connect (enable) */
/* 19:16 for port testing */
#define PORT_LED_OFF	(0<<14)
#define PORT_LED_AMBER	(1<<14)
#define PORT_LED_GREEN	(2<<14)
#define PORT_LED_MASK	(3<<14)
#define PORT_OWNER	(1<<13)		/* true: companion hc owns this port */
#define PORT_POWER	(1<<12)		/* true: has power (see PPC) */
#define PORT_USB11(x) (((x)&(3<<10)) == (1<<10))	/* USB 1.1 device */
/* 11:10 for detecting lowspeed devices (reset vs release ownership) */
/* 9 reserved */
#define PORT_RESET	(1<<8)		/* reset port */
#define PORT_SUSPEND	(1<<7)		/* suspend port */
#define PORT_RESUME	(1<<6)		/* resume it */
#define PORT_OCC	(1<<5)		/* over current change */
#define PORT_OC		(1<<4)		/* over current active */
#define PORT_PEC	(1<<3)		/* port enable change */
#define PORT_PE		(1<<2)		/* port enable */
#define PORT_CSC	(1<<1)		/* connect status change */
#define PORT_CONNECT	(1<<0)		/* device connected */
#define PORT_RWC_BITS   (PORT_CSC | PORT_PEC | PORT_OCC)
} __packed;

/* Appendix C, Debug port ... intended for use with special "debug devices"
 * that can help if there's no serial console.  (nonstandard enumeration.)
 */
struct ehci_dbg_port {
	u32	control;
#define DBGP_OWNER	(1<<30)
#define DBGP_ENABLED	(1<<28)
#define DBGP_DONE	(1<<16)
#define DBGP_INUSE	(1<<10)
#define DBGP_ERRCODE(x)	(((x)>>7)&0x07)
#	define DBGP_ERR_BAD	1
#	define DBGP_ERR_SIGNAL	2
#define DBGP_ERROR	(1<<6)
#define DBGP_GO		(1<<5)
#define DBGP_OUT	(1<<4)
#define DBGP_LEN(x)	(((x)>>0)&0x0f)
	u32	pids;
#define DBGP_PID_GET(x)		(((x)>>16)&0xff)
#define DBGP_PID_SET(data, tok)	(((data)<<8)|(tok))
	u32	data03;
	u32	data47;
	u32	address;
#define DBGP_EPADDR(dev, ep)	(((dev)<<8)|(ep))
} __packed;

#define	QTD_NEXT(dma)	cpu_to_le32((u32)dma)

/*
 * EHCI Specification 0.95 Section 3.5
 * QTD: describe data transfer components (buffer, direction, ...)
 * See Fig 3-6 "Queue Element Transfer Descriptor Block Diagram".
 *
 * These are associated only with "QH" (Queue Head) structures,
 * used with control, bulk, and interrupt transfers.
 */
struct ehci_qtd {
	/* first part defined by EHCI spec */
	__le32			hw_next;		/* see EHCI 3.5.1 */
	__le32			hw_alt_next;		/* see EHCI 3.5.2 */
	__le32			hw_token;		/* see EHCI 3.5.3 */
#define	QTD_TOGGLE	(1 << 31)	/* data toggle */
#define	QTD_LENGTH(tok)	(((tok)>>16) & 0x7fff)
#define	QTD_IOC		(1 << 15)	/* interrupt on complete */
#define	QTD_CERR(tok)	(((tok)>>10) & 0x3)
#define	QTD_PID(tok)	(((tok)>>8) & 0x3)
#define	QTD_STS_ACTIVE	(1 << 7)	/* HC may execute this */
#define	QTD_STS_HALT	(1 << 6)	/* halted on error */
#define	QTD_STS_DBE	(1 << 5)	/* data buffer error (in HC) */
#define	QTD_STS_BABBLE	(1 << 4)	/* device was babbling (qtd halted) */
#define	QTD_STS_XACT	(1 << 3)	/* device gave illegal response */
#define	QTD_STS_MMF	(1 << 2)	/* incomplete split transaction */
#define	QTD_STS_STS	(1 << 1)	/* split transaction state */
#define	QTD_STS_PING	(1 << 0)	/* issue PING? */
	__le32			hw_buf[5];		/* see EHCI 3.5.4 */
	__le32			hw_buf_hi[5];		/* Appendix B */

	/* the rest is HCD-private */
	dma_addr_t		qtd_dma;		/* qtd address */
	struct list_head	qtd_list;		/* sw qtd list */
	struct urb		*urb;			/* qtd's urb */
	size_t			length;			/* length of buffer */

	u32			qtd_buffer_len;
	void			*buffer;
	dma_addr_t		buffer_dma;
	void			*transfer_buffer;
	void			*transfer_dma;
} __aligned(32);

/* mask NakCnt+T in qh->hw_alt_next */
#define QTD_MASK cpu_to_le32 (~0x1f)

#define IS_SHORT_READ(token) (QTD_LENGTH(token) != 0 && QTD_PID(token) == 1)

/* Type tag from {qh, itd, sitd, fstn}->hw_next */
#define Q_NEXT_TYPE(dma) ((dma) & cpu_to_le32 (3 << 1))

/* values for that type tag */
#define Q_TYPE_QH	cpu_to_le32 (1 << 1)

/* next async queue entry, or pointer to interrupt/periodic QH */
#define	QH_NEXT(dma)	(cpu_to_le32(((u32)dma)&~0x01f)|Q_TYPE_QH)

/* for periodic/async schedules and qtd lists, mark end of list */
#define	EHCI_LIST_END	cpu_to_le32(1) /* "null pointer" to hw */

/*
 * Entries in periodic shadow table are pointers to one of four kinds
 * of data structure.  That's dictated by the hardware; a type tag is
 * encoded in the low bits of the hardware's periodic schedule.  Use
 * Q_NEXT_TYPE to get the tag.
 *
 * For entries in the async schedule, the type tag always says "qh".
 */
union ehci_shadow {
	struct ehci_qh		*qh;		/* Q_TYPE_QH */
	__le32			*hw_next;	/* (all types) */
	void			*ptr;
};

/*
 * EHCI Specification 0.95 Section 3.6
 * QH: describes control/bulk/interrupt endpoints
 * See Fig 3-7 "Queue Head Structure Layout".
 *
 * These appear in both the async and (for interrupt) periodic schedules.
 */

struct ehci_qh {
	/* first part defined by EHCI spec */
	__le32			hw_next;	 /* see EHCI 3.6.1 */
	__le32			hw_info1;	/* see EHCI 3.6.2 */
#define	QH_HEAD		0x00008000
	__le32			hw_info2;	/* see EHCI 3.6.2 */
#define	QH_SMASK	0x000000ff
#define	QH_CMASK	0x0000ff00
#define	QH_HUBADDR	0x007f0000
#define	QH_HUBPORT	0x3f800000
#define	QH_MULT		0xc0000000
	__le32			hw_current;	 /* qtd list - see EHCI 3.6.4 */

	/* qtd overlay (hardware parts of a struct ehci_qtd) */
	__le32			hw_qtd_next;
	__le32			hw_alt_next;
	__le32			hw_token;
	__le32			hw_buf[5];
	__le32			hw_buf_hi[5];

	/* the rest is HCD-private */
	dma_addr_t		qh_dma;		/* address of qh */
	union ehci_shadow	qh_next;	/* ptr to qh; or periodic */
	struct list_head	qtd_list;	/* sw qtd list */
	struct ehci_qtd		*dummy;
	struct ehci_qh		*reclaim;	/* next to reclaim */

	struct oxu_hcd		*oxu;
	struct kref		kref;
	unsigned int		stamp;

	u8			qh_state;
#define	QH_STATE_LINKED		1		/* HC sees this */
#define	QH_STATE_UNLINK		2		/* HC may still see this */
#define	QH_STATE_IDLE		3		/* HC doesn't see this */
#define	QH_STATE_UNLINK_WAIT	4		/* LINKED and on reclaim q */
#define	QH_STATE_COMPLETING	5		/* don't touch token.HALT */

	/* periodic schedule info */
	u8			usecs;		/* intr bandwidth */
	u8			gap_uf;		/* uframes split/csplit gap */
	u8			c_usecs;	/* ... split completion bw */
	u16			tt_usecs;	/* tt downstream bandwidth */
	unsigned short		period;		/* polling interval */
	unsigned short		start;		/* where polling starts */
#define NO_FRAME ((unsigned short)~0)			/* pick new start */
	struct usb_device	*dev;		/* access to TT */
} __aligned(32);

/*
 * Proper OXU210HP structs
 */

#define OXU_OTG_CORE_OFFSET	0x00400
#define OXU_OTG_CAP_OFFSET	(OXU_OTG_CORE_OFFSET + 0x100)
#define OXU_SPH_CORE_OFFSET	0x00800
#define OXU_SPH_CAP_OFFSET	(OXU_SPH_CORE_OFFSET + 0x100)

#define OXU_OTG_MEM		0xE000
#define OXU_SPH_MEM		0x16000

/* Only how many elements & element structure are specifies here. */
/* 2 host controllers are enabled - total size <= 28 kbytes */
#define	DEFAULT_I_TDPS		1024
#define QHEAD_NUM		16
#define QTD_NUM			32
#define SITD_NUM		8
#define MURB_NUM		8

#define BUFFER_NUM		8
#define BUFFER_SIZE		512

struct oxu_info {
	struct usb_hcd *hcd[2];
};

struct oxu_buf {
	u8			buffer[BUFFER_SIZE];
} __aligned(BUFFER_SIZE);

struct oxu_onchip_mem {
	struct oxu_buf		db_pool[BUFFER_NUM];

	u32			frame_list[DEFAULT_I_TDPS];
	struct ehci_qh		qh_pool[QHEAD_NUM];
	struct ehci_qtd		qtd_pool[QTD_NUM];
} __aligned(4 << 10);

#define	EHCI_MAX_ROOT_PORTS	15		/* see HCS_N_PORTS */

struct oxu_murb {
	struct urb		urb;
	struct urb		*main;
	u8			last;
};

struct oxu_hcd {				/* one per controller */
	unsigned int		is_otg:1;

	u8			qh_used[QHEAD_NUM];
	u8			qtd_used[QTD_NUM];
	u8			db_used[BUFFER_NUM];
	u8			murb_used[MURB_NUM];

	struct oxu_onchip_mem	__iomem *mem;
	spinlock_t		mem_lock;

	struct timer_list	urb_timer;

	struct ehci_caps __iomem *caps;
	struct ehci_regs __iomem *regs;

	u32			hcs_params;	/* cached register copy */
	spinlock_t		lock;

	/* async schedule support */
	struct ehci_qh		*async;
	struct ehci_qh		*reclaim;
	unsigned int		reclaim_ready:1;
	unsigned int		scanning:1;

	/* periodic schedule support */
	unsigned int		periodic_size;
	__le32			*periodic;	/* hw periodic table */
	dma_addr_t		periodic_dma;
	unsigned int		i_thresh;	/* uframes HC might cache */

	union ehci_shadow	*pshadow;	/* mirror hw periodic table */
	int			next_uframe;	/* scan periodic, start here */
	unsigned int		periodic_sched;	/* periodic activity count */

	/* per root hub port */
	unsigned long		reset_done[EHCI_MAX_ROOT_PORTS];
	/* bit vectors (one bit per port) */
	unsigned long		bus_suspended;	/* which ports were
						 * already suspended at the
						 * start of a bus suspend
						 */
	unsigned long		companion_ports;/* which ports are dedicated
						 * to the companion controller
						 */

	struct timer_list	watchdog;
	unsigned long		actions;
	unsigned int		stamp;
	unsigned long		next_statechange;
	u32			command;

	/* SILICON QUIRKS */
	struct list_head	urb_list;	/* this is the head to urb
						 * queue that didn't get enough
						 * resources
						 */
	struct oxu_murb		*murb_pool;	/* murb per split big urb */
	unsigned int		urb_len;

	u8			sbrn;		/* packed release number */
};

#define EHCI_IAA_JIFFIES	(HZ/100)	/* arbitrary; ~10 msec */
#define EHCI_IO_JIFFIES		(HZ/10)		/* io watchdog > irq_thresh */
#define EHCI_ASYNC_JIFFIES      (HZ/20)		/* async idle timeout */
#define EHCI_SHRINK_JIFFIES     (HZ/200)	/* async qh unlink delay */

enum ehci_timer_action {
	TIMER_IO_WATCHDOG,
	TIMER_IAA_WATCHDOG,
	TIMER_ASYNC_SHRINK,
	TIMER_ASYNC_OFF,
};

/*
 * Main defines
 */

#define oxu_dbg(oxu, fmt, args...) \
		dev_dbg(oxu_to_hcd(oxu)->self.controller , fmt , ## args)
#define oxu_err(oxu, fmt, args...) \
		dev_err(oxu_to_hcd(oxu)->self.controller , fmt , ## args)
#define oxu_info(oxu, fmt, args...) \
		dev_info(oxu_to_hcd(oxu)->self.controller , fmt , ## args)

#ifdef CONFIG_DYNAMIC_DEBUG
#define DEBUG
#endif

static inline struct usb_hcd *oxu_to_hcd(struct oxu_hcd *oxu)
{
	return container_of((void *) oxu, struct usb_hcd, hcd_priv);
}

static inline struct oxu_hcd *hcd_to_oxu(struct usb_hcd *hcd)
{
	return (struct oxu_hcd *) (hcd->hcd_priv);
}

/*
 * Debug stuff
 */

#undef OXU_URB_TRACE
#undef OXU_VERBOSE_DEBUG

#ifdef OXU_VERBOSE_DEBUG
#define oxu_vdbg			oxu_dbg
#else
#define oxu_vdbg(oxu, fmt, args...)	/* Nop */
#endif

#ifdef DEBUG

static int __attribute__((__unused__))
dbg_status_buf(char *buf, unsigned len, const char *label, u32 status)
{
	return scnprintf(buf, len, "%s%sstatus %04x%s%s%s%s%s%s%s%s%s%s",
		label, label[0] ? " " : "", status,
		(status & STS_ASS) ? " Async" : "",
		(status & STS_PSS) ? " Periodic" : "",
		(status & STS_RECL) ? " Recl" : "",
		(status & STS_HALT) ? " Halt" : "",
		(status & STS_IAA) ? " IAA" : "",
		(status & STS_FATAL) ? " FATAL" : "",
		(status & STS_FLR) ? " FLR" : "",
		(status & STS_PCD) ? " PCD" : "",
		(status & STS_ERR) ? " ERR" : "",
		(status & STS_INT) ? " INT" : ""
		);
}

static int __attribute__((__unused__))
dbg_intr_buf(char *buf, unsigned len, const char *label, u32 enable)
{
	return scnprintf(buf, len, "%s%sintrenable %02x%s%s%s%s%s%s",
		label, label[0] ? " " : "", enable,
		(enable & STS_IAA) ? " IAA" : "",
		(enable & STS_FATAL) ? " FATAL" : "",
		(enable & STS_FLR) ? " FLR" : "",
		(enable & STS_PCD) ? " PCD" : "",
		(enable & STS_ERR) ? " ERR" : "",
		(enable & STS_INT) ? " INT" : ""
		);
}

static const char *const fls_strings[] =
    { "1024", "512", "256", "??" };

static int dbg_command_buf(char *buf, unsigned len,
				const char *label, u32 command)
{
	return scnprintf(buf, len,
		"%s%scommand %06x %s=%d ithresh=%d%s%s%s%s period=%s%s %s",
		label, label[0] ? " " : "", command,
		(command & CMD_PARK) ? "park" : "(park)",
		CMD_PARK_CNT(command),
		(command >> 16) & 0x3f,
		(command & CMD_LRESET) ? " LReset" : "",
		(command & CMD_IAAD) ? " IAAD" : "",
		(command & CMD_ASE) ? " Async" : "",
		(command & CMD_PSE) ? " Periodic" : "",
		fls_strings[(command >> 2) & 0x3],
		(command & CMD_RESET) ? " Reset" : "",
		(command & CMD_RUN) ? "RUN" : "HALT"
		);
}

static int dbg_port_buf(char *buf, unsigned len, const char *label,
				int port, u32 status)
{
	char	*sig;

	/* signaling state */
	switch (status & (3 << 10)) {
	case 0 << 10:
		sig = "se0";
		break;
	case 1 << 10:
		sig = "k";	/* low speed */
		break;
	case 2 << 10:
		sig = "j";
		break;
	default:
		sig = "?";
		break;
	}

	return scnprintf(buf, len,
		"%s%sport %d status %06x%s%s sig=%s%s%s%s%s%s%s%s%s%s",
		label, label[0] ? " " : "", port, status,
		(status & PORT_POWER) ? " POWER" : "",
		(status & PORT_OWNER) ? " OWNER" : "",
		sig,
		(status & PORT_RESET) ? " RESET" : "",
		(status & PORT_SUSPEND) ? " SUSPEND" : "",
		(status & PORT_RESUME) ? " RESUME" : "",
		(status & PORT_OCC) ? " OCC" : "",
		(status & PORT_OC) ? " OC" : "",
		(status & PORT_PEC) ? " PEC" : "",
		(status & PORT_PE) ? " PE" : "",
		(status & PORT_CSC) ? " CSC" : "",
		(status & PORT_CONNECT) ? " CONNECT" : ""
	    );
}

#else

static inline int __attribute__((__unused__))
dbg_status_buf(char *buf, unsigned len, const char *label, u32 status)
{ return 0; }

static inline int __attribute__((__unused__))
dbg_command_buf(char *buf, unsigned len, const char *label, u32 command)
{ return 0; }

static inline int __attribute__((__unused__))
dbg_intr_buf(char *buf, unsigned len, const char *label, u32 enable)
{ return 0; }

static inline int __attribute__((__unused__))
dbg_port_buf(char *buf, unsigned len, const char *label, int port, u32 status)
{ return 0; }

#endif /* DEBUG */

/* functions have the "wrong" filename when they're output... */
#define dbg_status(oxu, label, status) { \
	char _buf[80]; \
	dbg_status_buf(_buf, sizeof _buf, label, status); \
	oxu_dbg(oxu, "%s\n", _buf); \
}

#define dbg_cmd(oxu, label, command) { \
	char _buf[80]; \
	dbg_command_buf(_buf, sizeof _buf, label, command); \
	oxu_dbg(oxu, "%s\n", _buf); \
}

#define dbg_port(oxu, label, port, status) { \
	char _buf[80]; \
	dbg_port_buf(_buf, sizeof _buf, label, port, status); \
	oxu_dbg(oxu, "%s\n", _buf); \
}

/*
 * Module parameters
 */

/* Initial IRQ latency: faster than hw default */
static int log2_irq_thresh;			/* 0 to 6 */
module_param(log2_irq_thresh, int, S_IRUGO);
MODULE_PARM_DESC(log2_irq_thresh, "log2 IRQ latency, 1-64 microframes");

/* Initial park setting: slower than hw default */
static unsigned park;
module_param(park, uint, S_IRUGO);
MODULE_PARM_DESC(park, "park setting; 1-3 back-to-back async packets");

/* For flakey hardware, ignore overcurrent indicators */
static bool ignore_oc;
module_param(ignore_oc, bool, S_IRUGO);
MODULE_PARM_DESC(ignore_oc, "ignore bogus hardware overcurrent indications");


static void ehci_work(struct oxu_hcd *oxu);
static int oxu_hub_control(struct usb_hcd *hcd,
				u16 typeReq, u16 wValue, u16 wIndex,
				char *buf, u16 wLength);

/*
 * Local functions
 */

/* Low level read/write registers functions */
static inline u32 oxu_readl(void __iomem *base, u32 reg)
{
	return readl(base + reg);
}

static inline void oxu_writel(void __iomem *base, u32 reg, u32 val)
{
	writel(val, base + reg);
}

static inline void timer_action_done(struct oxu_hcd *oxu,
					enum ehci_timer_action action)
{
	clear_bit(action, &oxu->actions);
}

static inline void timer_action(struct oxu_hcd *oxu,
					enum ehci_timer_action action)
{
	if (!test_and_set_bit(action, &oxu->actions)) {
		unsigned long t;

		switch (action) {
		case TIMER_IAA_WATCHDOG:
			t = EHCI_IAA_JIFFIES;
			break;
		case TIMER_IO_WATCHDOG:
			t = EHCI_IO_JIFFIES;
			break;
		case TIMER_ASYNC_OFF:
			t = EHCI_ASYNC_JIFFIES;
			break;
		case TIMER_ASYNC_SHRINK:
		default:
			t = EHCI_SHRINK_JIFFIES;
			break;
		}
		t += jiffies;
		/* all timings except IAA watchdog can be overridden.
		 * async queue SHRINK often precedes IAA.  while it's ready
		 * to go OFF neither can matter, and afterwards the IO
		 * watchdog stops unless there's still periodic traffic.
		 */
		if (action != TIMER_IAA_WATCHDOG
				&& t > oxu->watchdog.expires
				&& timer_pending(&oxu->watchdog))
			return;
		mod_timer(&oxu->watchdog, t);
	}
}

/*
 * handshake - spin reading hc until handshake completes or fails
 * @ptr: address of hc register to be read
 * @mask: bits to look at in result of read
 * @done: value of those bits when handshake succeeds
 * @usec: timeout in microseconds
 *
 * Returns negative errno, or zero on success
 *
 * Success happens when the "mask" bits have the specified value (hardware
 * handshake done).  There are two failure modes:  "usec" have passed (major
 * hardware flakeout), or the register reads as all-ones (hardware removed).
 *
 * That last failure should_only happen in cases like physical cardbus eject
 * before driver shutdown. But it also seems to be caused by bugs in cardbus
 * bridge shutdown:  shutting down the bridge before the devices using it.
 */
static int handshake(struct oxu_hcd *oxu, void __iomem *ptr,
					u32 mask, u32 done, int usec)
{
	u32 result;
	int ret;

	ret = readl_poll_timeout_atomic(ptr, result,
					((result & mask) == done ||
					 result == U32_MAX),
					1, usec);
	if (result == U32_MAX)		/* card removed */
		return -ENODEV;

	return ret;
}

/* Force HC to halt state from unknown (EHCI spec section 2.3) */
static int ehci_halt(struct oxu_hcd *oxu)
{
	u32	temp = readl(&oxu->regs->status);

	/* disable any irqs left enabled by previous code */
	writel(0, &oxu->regs->intr_enable);

	if ((temp & STS_HALT) != 0)
		return 0;

	temp = readl(&oxu->regs->command);
	temp &= ~CMD_RUN;
	writel(temp, &oxu->regs->command);
	return handshake(oxu, &oxu->regs->status,
			  STS_HALT, STS_HALT, 16 * 125);
}

/* Put TDI/ARC silicon into EHCI mode */
static void tdi_reset(struct oxu_hcd *oxu)
{
	u32 __iomem *reg_ptr;
	u32 tmp;

	reg_ptr = (u32 __iomem *)(((u8 __iomem *)oxu->regs) + 0x68);
	tmp = readl(reg_ptr);
	tmp |= 0x3;
	writel(tmp, reg_ptr);
}

/* Reset a non-running (STS_HALT == 1) controller */
static int ehci_reset(struct oxu_hcd *oxu)
{
	int	retval;
	u32	command = readl(&oxu->regs->command);

	command |= CMD_RESET;
	dbg_cmd(oxu, "reset", command);
	writel(command, &oxu->regs->command);
	oxu_to_hcd(oxu)->state = HC_STATE_HALT;
	oxu->next_statechange = jiffies;
	retval = handshake(oxu, &oxu->regs->command,
			    CMD_RESET, 0, 250 * 1000);

	if (retval)
		return retval;

	tdi_reset(oxu);

	return retval;
}

/* Idle the controller (from running) */
static void ehci_quiesce(struct oxu_hcd *oxu)
{
	u32	temp;

#ifdef DEBUG
	BUG_ON(!HC_IS_RUNNING(oxu_to_hcd(oxu)->state));
#endif

	/* wait for any schedule enables/disables to take effect */
	temp = readl(&oxu->regs->command) << 10;
	temp &= STS_ASS | STS_PSS;
	if (handshake(oxu, &oxu->regs->status, STS_ASS | STS_PSS,
				temp, 16 * 125) != 0) {
		oxu_to_hcd(oxu)->state = HC_STATE_HALT;
		return;
	}

	/* then disable anything that's still active */
	temp = readl(&oxu->regs->command);
	temp &= ~(CMD_ASE | CMD_IAAD | CMD_PSE);
	writel(temp, &oxu->regs->command);

	/* hardware can take 16 microframes to turn off ... */
	if (handshake(oxu, &oxu->regs->status, STS_ASS | STS_PSS,
				0, 16 * 125) != 0) {
		oxu_to_hcd(oxu)->state = HC_STATE_HALT;
		return;
	}
}

static int check_reset_complete(struct oxu_hcd *oxu, int index,
				u32 __iomem *status_reg, int port_status)
{
	if (!(port_status & PORT_CONNECT)) {
		oxu->reset_done[index] = 0;
		return port_status;
	}

	/* if reset finished and it's still not enabled -- handoff */
	if (!(port_status & PORT_PE)) {
		oxu_dbg(oxu, "Failed to enable port %d on root hub TT\n",
				index+1);
		return port_status;
	} else
		oxu_dbg(oxu, "port %d high speed\n", index + 1);

	return port_status;
}

static void ehci_hub_descriptor(struct oxu_hcd *oxu,
				struct usb_hub_descriptor *desc)
{
	int ports = HCS_N_PORTS(oxu->hcs_params);
	u16 temp;

	desc->bDescriptorType = USB_DT_HUB;
	desc->bPwrOn2PwrGood = 10;	/* oxu 1.0, 2.3.9 says 20ms max */
	desc->bHubContrCurrent = 0;

	desc->bNbrPorts = ports;
	temp = 1 + (ports / 8);
	desc->bDescLength = 7 + 2 * temp;

	/* ports removable, and usb 1.0 legacy PortPwrCtrlMask */
	memset(&desc->u.hs.DeviceRemovable[0], 0, temp);
	memset(&desc->u.hs.DeviceRemovable[temp], 0xff, temp);

	temp = HUB_CHAR_INDV_PORT_OCPM;	/* per-port overcurrent reporting */
	if (HCS_PPC(oxu->hcs_params))
		temp |= HUB_CHAR_INDV_PORT_LPSM; /* per-port power control */
	else
		temp |= HUB_CHAR_NO_LPSM; /* no power switching */
	desc->wHubCharacteristics = (__force __u16)cpu_to_le16(temp);
}


/* Allocate an OXU210HP on-chip memory data buffer
 *
 * An on-chip memory data buffer is required for each OXU210HP USB transfer.
 * Each transfer descriptor has one or more on-chip memory data buffers.
 *
 * Data buffers are allocated from a fix sized pool of data blocks.
 * To minimise fragmentation and give reasonable memory utlisation,
 * data buffers are allocated with sizes the power of 2 multiples of
 * the block size, starting on an address a multiple of the allocated size.
 *
 * FIXME: callers of this function require a buffer to be allocated for
 * len=0. This is a waste of on-chip memory and should be fix. Then this
 * function should be changed to not allocate a buffer for len=0.
 */
static int oxu_buf_alloc(struct oxu_hcd *oxu, struct ehci_qtd *qtd, int len)
{
	int n_blocks;	/* minium blocks needed to hold len */
	int a_blocks;	/* blocks allocated */
	int i, j;

	/* Don't allocte bigger than supported */
	if (len > BUFFER_SIZE * BUFFER_NUM) {
		oxu_err(oxu, "buffer too big (%d)\n", len);
		return -ENOMEM;
	}

	spin_lock(&oxu->mem_lock);

	/* Number of blocks needed to hold len */
	n_blocks = (len + BUFFER_SIZE - 1) / BUFFER_SIZE;

	/* Round the number of blocks up to the power of 2 */
	for (a_blocks = 1; a_blocks < n_blocks; a_blocks <<= 1)
		;

	/* Find a suitable available data buffer */
	for (i = 0; i < BUFFER_NUM;
			i += max(a_blocks, (int)oxu->db_used[i])) {

		/* Check all the required blocks are available */
		for (j = 0; j < a_blocks; j++)
			if (oxu->db_used[i + j])
				break;

		if (j != a_blocks)
			continue;

		/* Allocate blocks found! */
		qtd->buffer = (void *) &oxu->mem->db_pool[i];
		qtd->buffer_dma = virt_to_phys(qtd->buffer);

		qtd->qtd_buffer_len = BUFFER_SIZE * a_blocks;
		oxu->db_used[i] = a_blocks;

		spin_unlock(&oxu->mem_lock);

		return 0;
	}

	/* Failed */

	spin_unlock(&oxu->mem_lock);

	return -ENOMEM;
}

static void oxu_buf_free(struct oxu_hcd *oxu, struct ehci_qtd *qtd)
{
	int index;

	spin_lock(&oxu->mem_lock);

	index = (qtd->buffer - (void *) &oxu->mem->db_pool[0])
							 / BUFFER_SIZE;
	oxu->db_used[index] = 0;
	qtd->qtd_buffer_len = 0;
	qtd->buffer_dma = 0;
	qtd->buffer = NULL;

	spin_unlock(&oxu->mem_lock);
}

static inline void ehci_qtd_init(struct ehci_qtd *qtd, dma_addr_t dma)
{
	memset(qtd, 0, sizeof *qtd);
	qtd->qtd_dma = dma;
	qtd->hw_token = cpu_to_le32(QTD_STS_HALT);
	qtd->hw_next = EHCI_LIST_END;
	qtd->hw_alt_next = EHCI_LIST_END;
	INIT_LIST_HEAD(&qtd->qtd_list);
}

static inline void oxu_qtd_free(struct oxu_hcd *oxu, struct ehci_qtd *qtd)
{
	int index;

	if (qtd->buffer)
		oxu_buf_free(oxu, qtd);

	spin_lock(&oxu->mem_lock);

	index = qtd - &oxu->mem->qtd_pool[0];
	oxu->qtd_used[index] = 0;

	spin_unlock(&oxu->mem_lock);
}

static struct ehci_qtd *ehci_qtd_alloc(struct oxu_hcd *oxu)
{
	int i;
	struct ehci_qtd *qtd = NULL;

	spin_lock(&oxu->mem_lock);

	for (i = 0; i < QTD_NUM; i++)
		if (!oxu->qtd_used[i])
			break;

	if (i < QTD_NUM) {
		qtd = (struct ehci_qtd *) &oxu->mem->qtd_pool[i];
		memset(qtd, 0, sizeof *qtd);

		qtd->hw_token = cpu_to_le32(QTD_STS_HALT);
		qtd->hw_next = EHCI_LIST_END;
		qtd->hw_alt_next = EHCI_LIST_END;
		INIT_LIST_HEAD(&qtd->qtd_list);

		qtd->qtd_dma = virt_to_phys(qtd);

		oxu->qtd_used[i] = 1;
	}

	spin_unlock(&oxu->mem_lock);

	return qtd;
}

static void oxu_qh_free(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	int index;

	spin_lock(&oxu->mem_lock);

	index = qh - &oxu->mem->qh_pool[0];
	oxu->qh_used[index] = 0;

	spin_unlock(&oxu->mem_lock);
}

static void qh_destroy(struct kref *kref)
{
	struct ehci_qh *qh = container_of(kref, struct ehci_qh, kref);
	struct oxu_hcd *oxu = qh->oxu;

	/* clean qtds first, and know this is not linked */
	if (!list_empty(&qh->qtd_list) || qh->qh_next.ptr) {
		oxu_dbg(oxu, "unused qh not empty!\n");
		BUG();
	}
	if (qh->dummy)
		oxu_qtd_free(oxu, qh->dummy);
	oxu_qh_free(oxu, qh);
}

static struct ehci_qh *oxu_qh_alloc(struct oxu_hcd *oxu)
{
	int i;
	struct ehci_qh *qh = NULL;

	spin_lock(&oxu->mem_lock);

	for (i = 0; i < QHEAD_NUM; i++)
		if (!oxu->qh_used[i])
			break;

	if (i < QHEAD_NUM) {
		qh = (struct ehci_qh *) &oxu->mem->qh_pool[i];
		memset(qh, 0, sizeof *qh);

		kref_init(&qh->kref);
		qh->oxu = oxu;
		qh->qh_dma = virt_to_phys(qh);
		INIT_LIST_HEAD(&qh->qtd_list);

		/* dummy td enables safe urb queuing */
		qh->dummy = ehci_qtd_alloc(oxu);
		if (qh->dummy == NULL) {
			oxu_dbg(oxu, "no dummy td\n");
			oxu->qh_used[i] = 0;
			qh = NULL;
			goto unlock;
		}

		oxu->qh_used[i] = 1;
	}
unlock:
	spin_unlock(&oxu->mem_lock);

	return qh;
}

/* to share a qh (cpu threads, or hc) */
static inline struct ehci_qh *qh_get(struct ehci_qh *qh)
{
	kref_get(&qh->kref);
	return qh;
}

static inline void qh_put(struct ehci_qh *qh)
{
	kref_put(&qh->kref, qh_destroy);
}

static void oxu_murb_free(struct oxu_hcd *oxu, struct oxu_murb *murb)
{
	int index;

	spin_lock(&oxu->mem_lock);

	index = murb - &oxu->murb_pool[0];
	oxu->murb_used[index] = 0;

	spin_unlock(&oxu->mem_lock);
}

static struct oxu_murb *oxu_murb_alloc(struct oxu_hcd *oxu)

{
	int i;
	struct oxu_murb *murb = NULL;

	spin_lock(&oxu->mem_lock);

	for (i = 0; i < MURB_NUM; i++)
		if (!oxu->murb_used[i])
			break;

	if (i < MURB_NUM) {
		murb = &(oxu->murb_pool)[i];

		oxu->murb_used[i] = 1;
	}

	spin_unlock(&oxu->mem_lock);

	return murb;
}

/* The queue heads and transfer descriptors are managed from pools tied
 * to each of the "per device" structures.
 * This is the initialisation and cleanup code.
 */
static void ehci_mem_cleanup(struct oxu_hcd *oxu)
{
	kfree(oxu->murb_pool);
	oxu->murb_pool = NULL;

	if (oxu->async)
		qh_put(oxu->async);
	oxu->async = NULL;

	del_timer(&oxu->urb_timer);

	oxu->periodic = NULL;

	/* shadow periodic table */
	kfree(oxu->pshadow);
	oxu->pshadow = NULL;
}

/* Remember to add cleanup code (above) if you add anything here.
 */
static int ehci_mem_init(struct oxu_hcd *oxu, gfp_t flags)
{
	int i;

	for (i = 0; i < oxu->periodic_size; i++)
		oxu->mem->frame_list[i] = EHCI_LIST_END;
	for (i = 0; i < QHEAD_NUM; i++)
		oxu->qh_used[i] = 0;
	for (i = 0; i < QTD_NUM; i++)
		oxu->qtd_used[i] = 0;

	oxu->murb_pool = kcalloc(MURB_NUM, sizeof(struct oxu_murb), flags);
	if (!oxu->murb_pool)
		goto fail;

	for (i = 0; i < MURB_NUM; i++)
		oxu->murb_used[i] = 0;

	oxu->async = oxu_qh_alloc(oxu);
	if (!oxu->async)
		goto fail;

	oxu->periodic = (__le32 *) &oxu->mem->frame_list;
	oxu->periodic_dma = virt_to_phys(oxu->periodic);

	for (i = 0; i < oxu->periodic_size; i++)
		oxu->periodic[i] = EHCI_LIST_END;

	/* software shadow of hardware table */
	oxu->pshadow = kcalloc(oxu->periodic_size, sizeof(void *), flags);
	if (oxu->pshadow != NULL)
		return 0;

fail:
	oxu_dbg(oxu, "couldn't init memory\n");
	ehci_mem_cleanup(oxu);
	return -ENOMEM;
}

/* Fill a qtd, returning how much of the buffer we were able to queue up.
 */
static int qtd_fill(struct ehci_qtd *qtd, dma_addr_t buf, size_t len,
				int token, int maxpacket)
{
	int i, count;
	u64 addr = buf;

	/* one buffer entry per 4K ... first might be short or unaligned */
	qtd->hw_buf[0] = cpu_to_le32((u32)addr);
	qtd->hw_buf_hi[0] = cpu_to_le32((u32)(addr >> 32));
	count = 0x1000 - (buf & 0x0fff);	/* rest of that page */
	if (likely(len < count))		/* ... iff needed */
		count = len;
	else {
		buf +=  0x1000;
		buf &= ~0x0fff;

		/* per-qtd limit: from 16K to 20K (best alignment) */
		for (i = 1; count < len && i < 5; i++) {
			addr = buf;
			qtd->hw_buf[i] = cpu_to_le32((u32)addr);
			qtd->hw_buf_hi[i] = cpu_to_le32((u32)(addr >> 32));
			buf += 0x1000;
			if ((count + 0x1000) < len)
				count += 0x1000;
			else
				count = len;
		}

		/* short packets may only terminate transfers */
		if (count != len)
			count -= (count % maxpacket);
	}
	qtd->hw_token = cpu_to_le32((count << 16) | token);
	qtd->length = count;

	return count;
}

static inline void qh_update(struct oxu_hcd *oxu,
				struct ehci_qh *qh, struct ehci_qtd *qtd)
{
	/* writes to an active overlay are unsafe */
	BUG_ON(qh->qh_state != QH_STATE_IDLE);

	qh->hw_qtd_next = QTD_NEXT(qtd->qtd_dma);
	qh->hw_alt_next = EHCI_LIST_END;

	/* Except for control endpoints, we make hardware maintain data
	 * toggle (like OHCI) ... here (re)initialize the toggle in the QH,
	 * and set the pseudo-toggle in udev. Only usb_clear_halt() will
	 * ever clear it.
	 */
	if (!(qh->hw_info1 & cpu_to_le32(1 << 14))) {
		unsigned	is_out, epnum;

		is_out = !(qtd->hw_token & cpu_to_le32(1 << 8));
		epnum = (le32_to_cpup(&qh->hw_info1) >> 8) & 0x0f;
		if (unlikely(!usb_gettoggle(qh->dev, epnum, is_out))) {
			qh->hw_token &= ~cpu_to_le32(QTD_TOGGLE);
			usb_settoggle(qh->dev, epnum, is_out, 1);
		}
	}

	/* HC must see latest qtd and qh data before we clear ACTIVE+HALT */
	wmb();
	qh->hw_token &= cpu_to_le32(QTD_TOGGLE | QTD_STS_PING);
}

/* If it weren't for a common silicon quirk (writing the dummy into the qh
 * overlay, so qh->hw_token wrongly becomes inactive/halted), only fault
 * recovery (including urb dequeue) would need software changes to a QH...
 */
static void qh_refresh(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	struct ehci_qtd *qtd;

	if (list_empty(&qh->qtd_list))
		qtd = qh->dummy;
	else {
		qtd = list_entry(qh->qtd_list.next,
				struct ehci_qtd, qtd_list);
		/* first qtd may already be partially processed */
		if (cpu_to_le32(qtd->qtd_dma) == qh->hw_current)
			qtd = NULL;
	}

	if (qtd)
		qh_update(oxu, qh, qtd);
}

static void qtd_copy_status(struct oxu_hcd *oxu, struct urb *urb,
				size_t length, u32 token)
{
	/* count IN/OUT bytes, not SETUP (even short packets) */
	if (likely(QTD_PID(token) != 2))
		urb->actual_length += length - QTD_LENGTH(token);

	/* don't modify error codes */
	if (unlikely(urb->status != -EINPROGRESS))
		return;

	/* force cleanup after short read; not always an error */
	if (unlikely(IS_SHORT_READ(token)))
		urb->status = -EREMOTEIO;

	/* serious "can't proceed" faults reported by the hardware */
	if (token & QTD_STS_HALT) {
		if (token & QTD_STS_BABBLE) {
			/* FIXME "must" disable babbling device's port too */
			urb->status = -EOVERFLOW;
		} else if (token & QTD_STS_MMF) {
			/* fs/ls interrupt xfer missed the complete-split */
			urb->status = -EPROTO;
		} else if (token & QTD_STS_DBE) {
			urb->status = (QTD_PID(token) == 1) /* IN ? */
				? -ENOSR  /* hc couldn't read data */
				: -ECOMM; /* hc couldn't write data */
		} else if (token & QTD_STS_XACT) {
			/* timeout, bad crc, wrong PID, etc; retried */
			if (QTD_CERR(token))
				urb->status = -EPIPE;
			else {
				oxu_dbg(oxu, "devpath %s ep%d%s 3strikes\n",
					urb->dev->devpath,
					usb_pipeendpoint(urb->pipe),
					usb_pipein(urb->pipe) ? "in" : "out");
				urb->status = -EPROTO;
			}
		/* CERR nonzero + no errors + halt --> stall */
		} else if (QTD_CERR(token))
			urb->status = -EPIPE;
		else	/* unknown */
			urb->status = -EPROTO;

		oxu_vdbg(oxu, "dev%d ep%d%s qtd token %08x --> status %d\n",
			usb_pipedevice(urb->pipe),
			usb_pipeendpoint(urb->pipe),
			usb_pipein(urb->pipe) ? "in" : "out",
			token, urb->status);
	}
}

static void ehci_urb_done(struct oxu_hcd *oxu, struct urb *urb)
__releases(oxu->lock)
__acquires(oxu->lock)
{
	if (likely(urb->hcpriv != NULL)) {
		struct ehci_qh	*qh = (struct ehci_qh *) urb->hcpriv;

		/* S-mask in a QH means it's an interrupt urb */
		if ((qh->hw_info2 & cpu_to_le32(QH_SMASK)) != 0) {

			/* ... update hc-wide periodic stats (for usbfs) */
			oxu_to_hcd(oxu)->self.bandwidth_int_reqs--;
		}
		qh_put(qh);
	}

	urb->hcpriv = NULL;
	switch (urb->status) {
	case -EINPROGRESS:		/* success */
		urb->status = 0;
		break;
	default:			/* fault */
		break;
	case -EREMOTEIO:		/* fault or normal */
		if (!(urb->transfer_flags & URB_SHORT_NOT_OK))
			urb->status = 0;
		break;
	case -ECONNRESET:		/* canceled */
	case -ENOENT:
		break;
	}

#ifdef OXU_URB_TRACE
	oxu_dbg(oxu, "%s %s urb %p ep%d%s status %d len %d/%d\n",
		__func__, urb->dev->devpath, urb,
		usb_pipeendpoint(urb->pipe),
		usb_pipein(urb->pipe) ? "in" : "out",
		urb->status,
		urb->actual_length, urb->transfer_buffer_length);
#endif

	/* complete() can reenter this HCD */
	spin_unlock(&oxu->lock);
	usb_hcd_giveback_urb(oxu_to_hcd(oxu), urb, urb->status);
	spin_lock(&oxu->lock);
}

static void start_unlink_async(struct oxu_hcd *oxu, struct ehci_qh *qh);
static void unlink_async(struct oxu_hcd *oxu, struct ehci_qh *qh);

static void intr_deschedule(struct oxu_hcd *oxu, struct ehci_qh *qh);
static int qh_schedule(struct oxu_hcd *oxu, struct ehci_qh *qh);

#define HALT_BIT cpu_to_le32(QTD_STS_HALT)

/* Process and free completed qtds for a qh, returning URBs to drivers.
 * Chases up to qh->hw_current.  Returns number of completions called,
 * indicating how much "real" work we did.
 */
static unsigned qh_completions(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	struct ehci_qtd *last = NULL, *end = qh->dummy;
	struct ehci_qtd	*qtd, *tmp;
	int stopped;
	unsigned count = 0;
	int do_status = 0;
	u8 state;
	struct oxu_murb *murb = NULL;

	if (unlikely(list_empty(&qh->qtd_list)))
		return count;

	/* completions (or tasks on other cpus) must never clobber HALT
	 * till we've gone through and cleaned everything up, even when
	 * they add urbs to this qh's queue or mark them for unlinking.
	 *
	 * NOTE:  unlinking expects to be done in queue order.
	 */
	state = qh->qh_state;
	qh->qh_state = QH_STATE_COMPLETING;
	stopped = (state == QH_STATE_IDLE);

	/* remove de-activated QTDs from front of queue.
	 * after faults (including short reads), cleanup this urb
	 * then let the queue advance.
	 * if queue is stopped, handles unlinks.
	 */
	list_for_each_entry_safe(qtd, tmp, &qh->qtd_list, qtd_list) {
		struct urb *urb;
		u32 token = 0;

		urb = qtd->urb;

		/* Clean up any state from previous QTD ...*/
		if (last) {
			if (likely(last->urb != urb)) {
				if (last->urb->complete == NULL) {
					murb = (struct oxu_murb *) last->urb;
					last->urb = murb->main;
					if (murb->last) {
						ehci_urb_done(oxu, last->urb);
						count++;
					}
					oxu_murb_free(oxu, murb);
				} else {
					ehci_urb_done(oxu, last->urb);
					count++;
				}
			}
			oxu_qtd_free(oxu, last);
			last = NULL;
		}

		/* ignore urbs submitted during completions we reported */
		if (qtd == end)
			break;

		/* hardware copies qtd out of qh overlay */
		rmb();
		token = le32_to_cpu(qtd->hw_token);

		/* always clean up qtds the hc de-activated */
		if ((token & QTD_STS_ACTIVE) == 0) {

			if ((token & QTD_STS_HALT) != 0) {
				stopped = 1;

			/* magic dummy for some short reads; qh won't advance.
			 * that silicon quirk can kick in with this dummy too.
			 */
			} else if (IS_SHORT_READ(token) &&
					!(qtd->hw_alt_next & EHCI_LIST_END)) {
				stopped = 1;
				goto halt;
			}

		/* stop scanning when we reach qtds the hc is using */
		} else if (likely(!stopped &&
				HC_IS_RUNNING(oxu_to_hcd(oxu)->state))) {
			break;

		} else {
			stopped = 1;

			if (unlikely(!HC_IS_RUNNING(oxu_to_hcd(oxu)->state)))
				urb->status = -ESHUTDOWN;

			/* ignore active urbs unless some previous qtd
			 * for the urb faulted (including short read) or
			 * its urb was canceled.  we may patch qh or qtds.
			 */
			if (likely(urb->status == -EINPROGRESS))
				continue;

			/* issue status after short control reads */
			if (unlikely(do_status != 0)
					&& QTD_PID(token) == 0 /* OUT */) {
				do_status = 0;
				continue;
			}

			/* token in overlay may be most current */
			if (state == QH_STATE_IDLE
					&& cpu_to_le32(qtd->qtd_dma)
						== qh->hw_current)
				token = le32_to_cpu(qh->hw_token);

			/* force halt for unlinked or blocked qh, so we'll
			 * patch the qh later and so that completions can't
			 * activate it while we "know" it's stopped.
			 */
			if ((HALT_BIT & qh->hw_token) == 0) {
halt:
				qh->hw_token |= HALT_BIT;
				wmb();
			}
		}

		/* Remove it from the queue */
		qtd_copy_status(oxu, urb->complete ?
					urb : ((struct oxu_murb *) urb)->main,
				qtd->length, token);
		if ((usb_pipein(qtd->urb->pipe)) &&
				(NULL != qtd->transfer_buffer))
			memcpy(qtd->transfer_buffer, qtd->buffer, qtd->length);
		do_status = (urb->status == -EREMOTEIO)
				&& usb_pipecontrol(urb->pipe);

		if (stopped && qtd->qtd_list.prev != &qh->qtd_list) {
			last = list_entry(qtd->qtd_list.prev,
					struct ehci_qtd, qtd_list);
			last->hw_next = qtd->hw_next;
		}
		list_del(&qtd->qtd_list);
		last = qtd;
	}

	/* last urb's completion might still need calling */
	if (likely(last != NULL)) {
		if (last->urb->complete == NULL) {
			murb = (struct oxu_murb *) last->urb;
			last->urb = murb->main;
			if (murb->last) {
				ehci_urb_done(oxu, last->urb);
				count++;
			}
			oxu_murb_free(oxu, murb);
		} else {
			ehci_urb_done(oxu, last->urb);
			count++;
		}
		oxu_qtd_free(oxu, last);
	}

	/* restore original state; caller must unlink or relink */
	qh->qh_state = state;

	/* be sure the hardware's done with the qh before refreshing
	 * it after fault cleanup, or recovering from silicon wrongly
	 * overlaying the dummy qtd (which reduces DMA chatter).
	 */
	if (stopped != 0 || qh->hw_qtd_next == EHCI_LIST_END) {
		switch (state) {
		case QH_STATE_IDLE:
			qh_refresh(oxu, qh);
			break;
		case QH_STATE_LINKED:
			/* should be rare for periodic transfers,
			 * except maybe high bandwidth ...
			 */
			if ((cpu_to_le32(QH_SMASK)
					& qh->hw_info2) != 0) {
				intr_deschedule(oxu, qh);
				(void) qh_schedule(oxu, qh);
			} else
				unlink_async(oxu, qh);
			break;
		/* otherwise, unlink already started */
		}
	}

	return count;
}

/* High bandwidth multiplier, as encoded in highspeed endpoint descriptors */
#define hb_mult(wMaxPacketSize)		(1 + (((wMaxPacketSize) >> 11) & 0x03))
/* ... and packet size, for any kind of endpoint descriptor */
#define max_packet(wMaxPacketSize)	((wMaxPacketSize) & 0x07ff)

/* Reverse of qh_urb_transaction: free a list of TDs.
 * used for cleanup after errors, before HC sees an URB's TDs.
 */
static void qtd_list_free(struct oxu_hcd *oxu,
				struct urb *urb, struct list_head *head)
{
	struct ehci_qtd	*qtd, *temp;

	list_for_each_entry_safe(qtd, temp, head, qtd_list) {
		list_del(&qtd->qtd_list);
		oxu_qtd_free(oxu, qtd);
	}
}

/* Create a list of filled qtds for this URB; won't link into qh.
 */
static struct list_head *qh_urb_transaction(struct oxu_hcd *oxu,
						struct urb *urb,
						struct list_head *head,
						gfp_t flags)
{
	struct ehci_qtd	*qtd, *qtd_prev;
	dma_addr_t buf;
	int len, maxpacket;
	int is_input;
	u32 token;
	void *transfer_buf = NULL;
	int ret;

	/*
	 * URBs map to sequences of QTDs: one logical transaction
	 */
	qtd = ehci_qtd_alloc(oxu);
	if (unlikely(!qtd))
		return NULL;
	list_add_tail(&qtd->qtd_list, head);
	qtd->urb = urb;

	token = QTD_STS_ACTIVE;
	token |= (EHCI_TUNE_CERR << 10);
	/* for split transactions, SplitXState initialized to zero */

	len = urb->transfer_buffer_length;
	is_input = usb_pipein(urb->pipe);
	if (!urb->transfer_buffer && urb->transfer_buffer_length && is_input)
		urb->transfer_buffer = phys_to_virt(urb->transfer_dma);

	if (usb_pipecontrol(urb->pipe)) {
		/* SETUP pid */
		ret = oxu_buf_alloc(oxu, qtd, sizeof(struct usb_ctrlrequest));
		if (ret)
			goto cleanup;

		qtd_fill(qtd, qtd->buffer_dma, sizeof(struct usb_ctrlrequest),
				token | (2 /* "setup" */ << 8), 8);
		memcpy(qtd->buffer, qtd->urb->setup_packet,
				sizeof(struct usb_ctrlrequest));

		/* ... and always at least one more pid */
		token ^= QTD_TOGGLE;
		qtd_prev = qtd;
		qtd = ehci_qtd_alloc(oxu);
		if (unlikely(!qtd))
			goto cleanup;
		qtd->urb = urb;
		qtd_prev->hw_next = QTD_NEXT(qtd->qtd_dma);
		list_add_tail(&qtd->qtd_list, head);

		/* for zero length DATA stages, STATUS is always IN */
		if (len == 0)
			token |= (1 /* "in" */ << 8);
	}

	/*
	 * Data transfer stage: buffer setup
	 */

	ret = oxu_buf_alloc(oxu, qtd, len);
	if (ret)
		goto cleanup;

	buf = qtd->buffer_dma;
	transfer_buf = urb->transfer_buffer;

	if (!is_input)
		memcpy(qtd->buffer, qtd->urb->transfer_buffer, len);

	if (is_input)
		token |= (1 /* "in" */ << 8);
	/* else it's already initted to "out" pid (0 << 8) */

	maxpacket = max_packet(usb_maxpacket(urb->dev, urb->pipe, !is_input));

	/*
	 * buffer gets wrapped in one or more qtds;
	 * last one may be "short" (including zero len)
	 * and may serve as a control status ack
	 */
	for (;;) {
		int this_qtd_len;

		this_qtd_len = qtd_fill(qtd, buf, len, token, maxpacket);
		qtd->transfer_buffer = transfer_buf;
		len -= this_qtd_len;
		buf += this_qtd_len;
		transfer_buf += this_qtd_len;
		if (is_input)
			qtd->hw_alt_next = oxu->async->hw_alt_next;

		/* qh makes control packets use qtd toggle; maybe switch it */
		if ((maxpacket & (this_qtd_len + (maxpacket - 1))) == 0)
			token ^= QTD_TOGGLE;

		if (likely(len <= 0))
			break;

		qtd_prev = qtd;
		qtd = ehci_qtd_alloc(oxu);
		if (unlikely(!qtd))
			goto cleanup;
		if (likely(len > 0)) {
			ret = oxu_buf_alloc(oxu, qtd, len);
			if (ret)
				goto cleanup;
		}
		qtd->urb = urb;
		qtd_prev->hw_next = QTD_NEXT(qtd->qtd_dma);
		list_add_tail(&qtd->qtd_list, head);
	}

	/* unless the bulk/interrupt caller wants a chance to clean
	 * up after short reads, hc should advance qh past this urb
	 */
	if (likely((urb->transfer_flags & URB_SHORT_NOT_OK) == 0
				|| usb_pipecontrol(urb->pipe)))
		qtd->hw_alt_next = EHCI_LIST_END;

	/*
	 * control requests may need a terminating data "status" ack;
	 * bulk ones may need a terminating short packet (zero length).
	 */
	if (likely(urb->transfer_buffer_length != 0)) {
		int	one_more = 0;

		if (usb_pipecontrol(urb->pipe)) {
			one_more = 1;
			token ^= 0x0100;	/* "in" <--> "out"  */
			token |= QTD_TOGGLE;	/* force DATA1 */
		} else if (usb_pipebulk(urb->pipe)
				&& (urb->transfer_flags & URB_ZERO_PACKET)
				&& !(urb->transfer_buffer_length % maxpacket)) {
			one_more = 1;
		}
		if (one_more) {
			qtd_prev = qtd;
			qtd = ehci_qtd_alloc(oxu);
			if (unlikely(!qtd))
				goto cleanup;
			qtd->urb = urb;
			qtd_prev->hw_next = QTD_NEXT(qtd->qtd_dma);
			list_add_tail(&qtd->qtd_list, head);

			/* never any data in such packets */
			qtd_fill(qtd, 0, 0, token, 0);
		}
	}

	/* by default, enable interrupt on urb completion */
	qtd->hw_token |= cpu_to_le32(QTD_IOC);
	return head;

cleanup:
	qtd_list_free(oxu, urb, head);
	return NULL;
}

/* Each QH holds a qtd list; a QH is used for everything except iso.
 *
 * For interrupt urbs, the scheduler must set the microframe scheduling
 * mask(s) each time the QH gets scheduled.  For highspeed, that's
 * just one microframe in the s-mask.  For split interrupt transactions
 * there are additional complications: c-mask, maybe FSTNs.
 */
static struct ehci_qh *qh_make(struct oxu_hcd *oxu,
				struct urb *urb, gfp_t flags)
{
	struct ehci_qh *qh = oxu_qh_alloc(oxu);
	u32 info1 = 0, info2 = 0;
	int is_input, type;
	int maxp = 0;

	if (!qh)
		return qh;

	/*
	 * init endpoint/device data for this QH
	 */
	info1 |= usb_pipeendpoint(urb->pipe) << 8;
	info1 |= usb_pipedevice(urb->pipe) << 0;

	is_input = usb_pipein(urb->pipe);
	type = usb_pipetype(urb->pipe);
	maxp = usb_maxpacket(urb->dev, urb->pipe, !is_input);

	/* Compute interrupt scheduling parameters just once, and save.
	 * - allowing for high bandwidth, how many nsec/uframe are used?
	 * - split transactions need a second CSPLIT uframe; same question
	 * - splits also need a schedule gap (for full/low speed I/O)
	 * - qh has a polling interval
	 *
	 * For control/bulk requests, the HC or TT handles these.
	 */
	if (type == PIPE_INTERRUPT) {
		qh->usecs = NS_TO_US(usb_calc_bus_time(USB_SPEED_HIGH,
								is_input, 0,
				hb_mult(maxp) * max_packet(maxp)));
		qh->start = NO_FRAME;

		if (urb->dev->speed == USB_SPEED_HIGH) {
			qh->c_usecs = 0;
			qh->gap_uf = 0;

			qh->period = urb->interval >> 3;
			if (qh->period == 0 && urb->interval != 1) {
				/* NOTE interval 2 or 4 uframes could work.
				 * But interval 1 scheduling is simpler, and
				 * includes high bandwidth.
				 */
				oxu_dbg(oxu, "intr period %d uframes, NYET!\n",
					urb->interval);
				goto done;
			}
		} else {
			struct usb_tt	*tt = urb->dev->tt;
			int		think_time;

			/* gap is f(FS/LS transfer times) */
			qh->gap_uf = 1 + usb_calc_bus_time(urb->dev->speed,
					is_input, 0, maxp) / (125 * 1000);

			/* FIXME this just approximates SPLIT/CSPLIT times */
			if (is_input) {		/* SPLIT, gap, CSPLIT+DATA */
				qh->c_usecs = qh->usecs + HS_USECS(0);
				qh->usecs = HS_USECS(1);
			} else {		/* SPLIT+DATA, gap, CSPLIT */
				qh->usecs += HS_USECS(1);
				qh->c_usecs = HS_USECS(0);
			}

			think_time = tt ? tt->think_time : 0;
			qh->tt_usecs = NS_TO_US(think_time +
					usb_calc_bus_time(urb->dev->speed,
					is_input, 0, max_packet(maxp)));
			qh->period = urb->interval;
		}
	}

	/* support for tt scheduling, and access to toggles */
	qh->dev = urb->dev;

	/* using TT? */
	switch (urb->dev->speed) {
	case USB_SPEED_LOW:
		info1 |= (1 << 12);	/* EPS "low" */
		fallthrough;

	case USB_SPEED_FULL:
		/* EPS 0 means "full" */
		if (type != PIPE_INTERRUPT)
			info1 |= (EHCI_TUNE_RL_TT << 28);
		if (type == PIPE_CONTROL) {
			info1 |= (1 << 27);	/* for TT */
			info1 |= 1 << 14;	/* toggle from qtd */
		}
		info1 |= maxp << 16;

		info2 |= (EHCI_TUNE_MULT_TT << 30);
		info2 |= urb->dev->ttport << 23;

		/* NOTE:  if (PIPE_INTERRUPT) { scheduler sets c-mask } */

		break;

	case USB_SPEED_HIGH:		/* no TT involved */
		info1 |= (2 << 12);	/* EPS "high" */
		if (type == PIPE_CONTROL) {
			info1 |= (EHCI_TUNE_RL_HS << 28);
			info1 |= 64 << 16;	/* usb2 fixed maxpacket */
			info1 |= 1 << 14;	/* toggle from qtd */
			info2 |= (EHCI_TUNE_MULT_HS << 30);
		} else if (type == PIPE_BULK) {
			info1 |= (EHCI_TUNE_RL_HS << 28);
			info1 |= 512 << 16;	/* usb2 fixed maxpacket */
			info2 |= (EHCI_TUNE_MULT_HS << 30);
		} else {		/* PIPE_INTERRUPT */
			info1 |= max_packet(maxp) << 16;
			info2 |= hb_mult(maxp) << 30;
		}
		break;
	default:
		oxu_dbg(oxu, "bogus dev %p speed %d\n", urb->dev, urb->dev->speed);
done:
		qh_put(qh);
		return NULL;
	}

	/* NOTE:  if (PIPE_INTERRUPT) { scheduler sets s-mask } */

	/* init as live, toggle clear, advance to dummy */
	qh->qh_state = QH_STATE_IDLE;
	qh->hw_info1 = cpu_to_le32(info1);
	qh->hw_info2 = cpu_to_le32(info2);
	usb_settoggle(urb->dev, usb_pipeendpoint(urb->pipe), !is_input, 1);
	qh_refresh(oxu, qh);
	return qh;
}

/* Move qh (and its qtds) onto async queue; maybe enable queue.
 */
static void qh_link_async(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	__le32 dma = QH_NEXT(qh->qh_dma);
	struct ehci_qh *head;

	/* (re)start the async schedule? */
	head = oxu->async;
	timer_action_done(oxu, TIMER_ASYNC_OFF);
	if (!head->qh_next.qh) {
		u32	cmd = readl(&oxu->regs->command);

		if (!(cmd & CMD_ASE)) {
			/* in case a clear of CMD_ASE didn't take yet */
			(void)handshake(oxu, &oxu->regs->status,
					STS_ASS, 0, 150);
			cmd |= CMD_ASE | CMD_RUN;
			writel(cmd, &oxu->regs->command);
			oxu_to_hcd(oxu)->state = HC_STATE_RUNNING;
			/* posted write need not be known to HC yet ... */
		}
	}

	/* clear halt and/or toggle; and maybe recover from silicon quirk */
	if (qh->qh_state == QH_STATE_IDLE)
		qh_refresh(oxu, qh);

	/* splice right after start */
	qh->qh_next = head->qh_next;
	qh->hw_next = head->hw_next;
	wmb();

	head->qh_next.qh = qh;
	head->hw_next = dma;

	qh->qh_state = QH_STATE_LINKED;
	/* qtd completions reported later by interrupt */
}

#define	QH_ADDR_MASK	cpu_to_le32(0x7f)

/*
 * For control/bulk/interrupt, return QH with these TDs appended.
 * Allocates and initializes the QH if necessary.
 * Returns null if it can't allocate a QH it needs to.
 * If the QH has TDs (urbs) already, that's great.
 */
static struct ehci_qh *qh_append_tds(struct oxu_hcd *oxu,
				struct urb *urb, struct list_head *qtd_list,
				int epnum, void	**ptr)
{
	struct ehci_qh *qh = NULL;

	qh = (struct ehci_qh *) *ptr;
	if (unlikely(qh == NULL)) {
		/* can't sleep here, we have oxu->lock... */
		qh = qh_make(oxu, urb, GFP_ATOMIC);
		*ptr = qh;
	}
	if (likely(qh != NULL)) {
		struct ehci_qtd	*qtd;

		if (unlikely(list_empty(qtd_list)))
			qtd = NULL;
		else
			qtd = list_entry(qtd_list->next, struct ehci_qtd,
					qtd_list);

		/* control qh may need patching ... */
		if (unlikely(epnum == 0)) {

			/* usb_reset_device() briefly reverts to address 0 */
			if (usb_pipedevice(urb->pipe) == 0)
				qh->hw_info1 &= ~QH_ADDR_MASK;
		}

		/* just one way to queue requests: swap with the dummy qtd.
		 * only hc or qh_refresh() ever modify the overlay.
		 */
		if (likely(qtd != NULL)) {
			struct ehci_qtd	*dummy;
			dma_addr_t dma;
			__le32 token;

			/* to avoid racing the HC, use the dummy td instead of
			 * the first td of our list (becomes new dummy).  both
			 * tds stay deactivated until we're done, when the
			 * HC is allowed to fetch the old dummy (4.10.2).
			 */
			token = qtd->hw_token;
			qtd->hw_token = HALT_BIT;
			wmb();
			dummy = qh->dummy;

			dma = dummy->qtd_dma;
			*dummy = *qtd;
			dummy->qtd_dma = dma;

			list_del(&qtd->qtd_list);
			list_add(&dummy->qtd_list, qtd_list);
			list_splice(qtd_list, qh->qtd_list.prev);

			ehci_qtd_init(qtd, qtd->qtd_dma);
			qh->dummy = qtd;

			/* hc must see the new dummy at list end */
			dma = qtd->qtd_dma;
			qtd = list_entry(qh->qtd_list.prev,
					struct ehci_qtd, qtd_list);
			qtd->hw_next = QTD_NEXT(dma);

			/* let the hc process these next qtds */
			dummy->hw_token = (token & ~(0x80));
			wmb();
			dummy->hw_token = token;

			urb->hcpriv = qh_get(qh);
		}
	}
	return qh;
}

static int submit_async(struct oxu_hcd	*oxu, struct urb *urb,
			struct list_head *qtd_list, gfp_t mem_flags)
{
	int epnum = urb->ep->desc.bEndpointAddress;
	unsigned long flags;
	struct ehci_qh *qh = NULL;
	int rc = 0;
#ifdef OXU_URB_TRACE
	struct ehci_qtd	*qtd;

	qtd = list_entry(qtd_list->next, struct ehci_qtd, qtd_list);

	oxu_dbg(oxu, "%s %s urb %p ep%d%s len %d, qtd %p [qh %p]\n",
		__func__, urb->dev->devpath, urb,
		epnum & 0x0f, (epnum & USB_DIR_IN) ? "in" : "out",
		urb->transfer_buffer_length,
		qtd, urb->ep->hcpriv);
#endif

	spin_lock_irqsave(&oxu->lock, flags);
	if (unlikely(!HCD_HW_ACCESSIBLE(oxu_to_hcd(oxu)))) {
		rc = -ESHUTDOWN;
		goto done;
	}

	qh = qh_append_tds(oxu, urb, qtd_list, epnum, &urb->ep->hcpriv);
	if (unlikely(qh == NULL)) {
		rc = -ENOMEM;
		goto done;
	}

	/* Control/bulk operations through TTs don't need scheduling,
	 * the HC and TT handle it when the TT has a buffer ready.
	 */
	if (likely(qh->qh_state == QH_STATE_IDLE))
		qh_link_async(oxu, qh_get(qh));
done:
	spin_unlock_irqrestore(&oxu->lock, flags);
	if (unlikely(qh == NULL))
		qtd_list_free(oxu, urb, qtd_list);
	return rc;
}

/* The async qh for the qtds being reclaimed are now unlinked from the HC */

static void end_unlink_async(struct oxu_hcd *oxu)
{
	struct ehci_qh *qh = oxu->reclaim;
	struct ehci_qh *next;

	timer_action_done(oxu, TIMER_IAA_WATCHDOG);

	qh->qh_state = QH_STATE_IDLE;
	qh->qh_next.qh = NULL;
	qh_put(qh);			/* refcount from reclaim */

	/* other unlink(s) may be pending (in QH_STATE_UNLINK_WAIT) */
	next = qh->reclaim;
	oxu->reclaim = next;
	oxu->reclaim_ready = 0;
	qh->reclaim = NULL;

	qh_completions(oxu, qh);

	if (!list_empty(&qh->qtd_list)
			&& HC_IS_RUNNING(oxu_to_hcd(oxu)->state))
		qh_link_async(oxu, qh);
	else {
		qh_put(qh);		/* refcount from async list */

		/* it's not free to turn the async schedule on/off; leave it
		 * active but idle for a while once it empties.
		 */
		if (HC_IS_RUNNING(oxu_to_hcd(oxu)->state)
				&& oxu->async->qh_next.qh == NULL)
			timer_action(oxu, TIMER_ASYNC_OFF);
	}

	if (next) {
		oxu->reclaim = NULL;
		start_unlink_async(oxu, next);
	}
}

/* makes sure the async qh will become idle */
/* caller must own oxu->lock */

static void start_unlink_async(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	int cmd = readl(&oxu->regs->command);
	struct ehci_qh *prev;

#ifdef DEBUG
	assert_spin_locked(&oxu->lock);
	BUG_ON(oxu->reclaim || (qh->qh_state != QH_STATE_LINKED
				&& qh->qh_state != QH_STATE_UNLINK_WAIT));
#endif

	/* stop async schedule right now? */
	if (unlikely(qh == oxu->async)) {
		/* can't get here without STS_ASS set */
		if (oxu_to_hcd(oxu)->state != HC_STATE_HALT
				&& !oxu->reclaim) {
			/* ... and CMD_IAAD clear */
			writel(cmd & ~CMD_ASE, &oxu->regs->command);
			wmb();
			/* handshake later, if we need to */
			timer_action_done(oxu, TIMER_ASYNC_OFF);
		}
		return;
	}

	qh->qh_state = QH_STATE_UNLINK;
	oxu->reclaim = qh = qh_get(qh);

	prev = oxu->async;
	while (prev->qh_next.qh != qh)
		prev = prev->qh_next.qh;

	prev->hw_next = qh->hw_next;
	prev->qh_next = qh->qh_next;
	wmb();

	if (unlikely(oxu_to_hcd(oxu)->state == HC_STATE_HALT)) {
		/* if (unlikely(qh->reclaim != 0))
		 *	this will recurse, probably not much
		 */
		end_unlink_async(oxu);
		return;
	}

	oxu->reclaim_ready = 0;
	cmd |= CMD_IAAD;
	writel(cmd, &oxu->regs->command);
	(void) readl(&oxu->regs->command);
	timer_action(oxu, TIMER_IAA_WATCHDOG);
}

static void scan_async(struct oxu_hcd *oxu)
{
	struct ehci_qh *qh;
	enum ehci_timer_action action = TIMER_IO_WATCHDOG;

	if (!++(oxu->stamp))
		oxu->stamp++;
	timer_action_done(oxu, TIMER_ASYNC_SHRINK);
rescan:
	qh = oxu->async->qh_next.qh;
	if (likely(qh != NULL)) {
		do {
			/* clean any finished work for this qh */
			if (!list_empty(&qh->qtd_list)
					&& qh->stamp != oxu->stamp) {
				int temp;

				/* unlinks could happen here; completion
				 * reporting drops the lock.  rescan using
				 * the latest schedule, but don't rescan
				 * qhs we already finished (no looping).
				 */
				qh = qh_get(qh);
				qh->stamp = oxu->stamp;
				temp = qh_completions(oxu, qh);
				qh_put(qh);
				if (temp != 0)
					goto rescan;
			}

			/* unlink idle entries, reducing HC PCI usage as well
			 * as HCD schedule-scanning costs.  delay for any qh
			 * we just scanned, there's a not-unusual case that it
			 * doesn't stay idle for long.
			 * (plus, avoids some kind of re-activation race.)
			 */
			if (list_empty(&qh->qtd_list)) {
				if (qh->stamp == oxu->stamp)
					action = TIMER_ASYNC_SHRINK;
				else if (!oxu->reclaim
					    && qh->qh_state == QH_STATE_LINKED)
					start_unlink_async(oxu, qh);
			}

			qh = qh->qh_next.qh;
		} while (qh);
	}
	if (action == TIMER_ASYNC_SHRINK)
		timer_action(oxu, TIMER_ASYNC_SHRINK);
}

/*
 * periodic_next_shadow - return "next" pointer on shadow list
 * @periodic: host pointer to qh/itd/sitd
 * @tag: hardware tag for type of this record
 */
static union ehci_shadow *periodic_next_shadow(union ehci_shadow *periodic,
						__le32 tag)
{
	switch (tag) {
	default:
	case Q_TYPE_QH:
		return &periodic->qh->qh_next;
	}
}

/* caller must hold oxu->lock */
static void periodic_unlink(struct oxu_hcd *oxu, unsigned frame, void *ptr)
{
	union ehci_shadow *prev_p = &oxu->pshadow[frame];
	__le32 *hw_p = &oxu->periodic[frame];
	union ehci_shadow here = *prev_p;

	/* find predecessor of "ptr"; hw and shadow lists are in sync */
	while (here.ptr && here.ptr != ptr) {
		prev_p = periodic_next_shadow(prev_p, Q_NEXT_TYPE(*hw_p));
		hw_p = here.hw_next;
		here = *prev_p;
	}
	/* an interrupt entry (at list end) could have been shared */
	if (!here.ptr)
		return;

	/* update shadow and hardware lists ... the old "next" pointers
	 * from ptr may still be in use, the caller updates them.
	 */
	*prev_p = *periodic_next_shadow(&here, Q_NEXT_TYPE(*hw_p));
	*hw_p = *here.hw_next;
}

/* how many of the uframe's 125 usecs are allocated? */
static unsigned short periodic_usecs(struct oxu_hcd *oxu,
					unsigned frame, unsigned uframe)
{
	__le32 *hw_p = &oxu->periodic[frame];
	union ehci_shadow *q = &oxu->pshadow[frame];
	unsigned usecs = 0;

	while (q->ptr) {
		switch (Q_NEXT_TYPE(*hw_p)) {
		case Q_TYPE_QH:
		default:
			/* is it in the S-mask? */
			if (q->qh->hw_info2 & cpu_to_le32(1 << uframe))
				usecs += q->qh->usecs;
			/* ... or C-mask? */
			if (q->qh->hw_info2 & cpu_to_le32(1 << (8 + uframe)))
				usecs += q->qh->c_usecs;
			hw_p = &q->qh->hw_next;
			q = &q->qh->qh_next;
			break;
		}
	}
#ifdef DEBUG
	if (usecs > 100)
		oxu_err(oxu, "uframe %d sched overrun: %d usecs\n",
						frame * 8 + uframe, usecs);
#endif
	return usecs;
}

static int enable_periodic(struct oxu_hcd *oxu)
{
	u32 cmd;
	int status;

	/* did clearing PSE did take effect yet?
	 * takes effect only at frame boundaries...
	 */
	status = handshake(oxu, &oxu->regs->status, STS_PSS, 0, 9 * 125);
	if (status != 0) {
		oxu_to_hcd(oxu)->state = HC_STATE_HALT;
		usb_hc_died(oxu_to_hcd(oxu));
		return status;
	}

	cmd = readl(&oxu->regs->command) | CMD_PSE;
	writel(cmd, &oxu->regs->command);
	/* posted write ... PSS happens later */
	oxu_to_hcd(oxu)->state = HC_STATE_RUNNING;

	/* make sure ehci_work scans these */
	oxu->next_uframe = readl(&oxu->regs->frame_index)
		% (oxu->periodic_size << 3);
	return 0;
}

static int disable_periodic(struct oxu_hcd *oxu)
{
	u32 cmd;
	int status;

	/* did setting PSE not take effect yet?
	 * takes effect only at frame boundaries...
	 */
	status = handshake(oxu, &oxu->regs->status, STS_PSS, STS_PSS, 9 * 125);
	if (status != 0) {
		oxu_to_hcd(oxu)->state = HC_STATE_HALT;
		usb_hc_died(oxu_to_hcd(oxu));
		return status;
	}

	cmd = readl(&oxu->regs->command) & ~CMD_PSE;
	writel(cmd, &oxu->regs->command);
	/* posted write ... */

	oxu->next_uframe = -1;
	return 0;
}

/* periodic schedule slots have iso tds (normal or split) first, then a
 * sparse tree for active interrupt transfers.
 *
 * this just links in a qh; caller guarantees uframe masks are set right.
 * no FSTN support (yet; oxu 0.96+)
 */
static int qh_link_periodic(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	unsigned i;
	unsigned period = qh->period;

	dev_dbg(&qh->dev->dev,
		"link qh%d-%04x/%p start %d [%d/%d us]\n",
		period, le32_to_cpup(&qh->hw_info2) & (QH_CMASK | QH_SMASK),
		qh, qh->start, qh->usecs, qh->c_usecs);

	/* high bandwidth, or otherwise every microframe */
	if (period == 0)
		period = 1;

	for (i = qh->start; i < oxu->periodic_size; i += period) {
		union ehci_shadow	*prev = &oxu->pshadow[i];
		__le32			*hw_p = &oxu->periodic[i];
		union ehci_shadow	here = *prev;
		__le32			type = 0;

		/* skip the iso nodes at list head */
		while (here.ptr) {
			type = Q_NEXT_TYPE(*hw_p);
			if (type == Q_TYPE_QH)
				break;
			prev = periodic_next_shadow(prev, type);
			hw_p = &here.qh->hw_next;
			here = *prev;
		}

		/* sorting each branch by period (slow-->fast)
		 * enables sharing interior tree nodes
		 */
		while (here.ptr && qh != here.qh) {
			if (qh->period > here.qh->period)
				break;
			prev = &here.qh->qh_next;
			hw_p = &here.qh->hw_next;
			here = *prev;
		}
		/* link in this qh, unless some earlier pass did that */
		if (qh != here.qh) {
			qh->qh_next = here;
			if (here.qh)
				qh->hw_next = *hw_p;
			wmb();
			prev->qh = qh;
			*hw_p = QH_NEXT(qh->qh_dma);
		}
	}
	qh->qh_state = QH_STATE_LINKED;
	qh_get(qh);

	/* update per-qh bandwidth for usbfs */
	oxu_to_hcd(oxu)->self.bandwidth_allocated += qh->period
		? ((qh->usecs + qh->c_usecs) / qh->period)
		: (qh->usecs * 8);

	/* maybe enable periodic schedule processing */
	if (!oxu->periodic_sched++)
		return enable_periodic(oxu);

	return 0;
}

static void qh_unlink_periodic(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	unsigned i;
	unsigned period;

	/* FIXME:
	 *   IF this isn't high speed
	 *   and this qh is active in the current uframe
	 *   (and overlay token SplitXstate is false?)
	 * THEN
	 *   qh->hw_info1 |= cpu_to_le32(1 << 7 "ignore");
	 */

	/* high bandwidth, or otherwise part of every microframe */
	period = qh->period;
	if (period == 0)
		period = 1;

	for (i = qh->start; i < oxu->periodic_size; i += period)
		periodic_unlink(oxu, i, qh);

	/* update per-qh bandwidth for usbfs */
	oxu_to_hcd(oxu)->self.bandwidth_allocated -= qh->period
		? ((qh->usecs + qh->c_usecs) / qh->period)
		: (qh->usecs * 8);

	dev_dbg(&qh->dev->dev,
		"unlink qh%d-%04x/%p start %d [%d/%d us]\n",
		qh->period,
		le32_to_cpup(&qh->hw_info2) & (QH_CMASK | QH_SMASK),
		qh, qh->start, qh->usecs, qh->c_usecs);

	/* qh->qh_next still "live" to HC */
	qh->qh_state = QH_STATE_UNLINK;
	qh->qh_next.ptr = NULL;
	qh_put(qh);

	/* maybe turn off periodic schedule */
	oxu->periodic_sched--;
	if (!oxu->periodic_sched)
		(void) disable_periodic(oxu);
}

static void intr_deschedule(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	unsigned wait;

	qh_unlink_periodic(oxu, qh);

	/* simple/paranoid:  always delay, expecting the HC needs to read
	 * qh->hw_next or finish a writeback after SPLIT/CSPLIT ... and
	 * expect hub_wq to clean up after any CSPLITs we won't issue.
	 * active high speed queues may need bigger delays...
	 */
	if (list_empty(&qh->qtd_list)
		|| (cpu_to_le32(QH_CMASK) & qh->hw_info2) != 0)
		wait = 2;
	else
		wait = 55;	/* worst case: 3 * 1024 */

	udelay(wait);
	qh->qh_state = QH_STATE_IDLE;
	qh->hw_next = EHCI_LIST_END;
	wmb();
}

static int check_period(struct oxu_hcd *oxu,
			unsigned frame, unsigned uframe,
			unsigned period, unsigned usecs)
{
	int claimed;

	/* complete split running into next frame?
	 * given FSTN support, we could sometimes check...
	 */
	if (uframe >= 8)
		return 0;

	/*
	 * 80% periodic == 100 usec/uframe available
	 * convert "usecs we need" to "max already claimed"
	 */
	usecs = 100 - usecs;

	/* we "know" 2 and 4 uframe intervals were rejected; so
	 * for period 0, check _every_ microframe in the schedule.
	 */
	if (unlikely(period == 0)) {
		do {
			for (uframe = 0; uframe < 7; uframe++) {
				claimed = periodic_usecs(oxu, frame, uframe);
				if (claimed > usecs)
					return 0;
			}
		} while ((frame += 1) < oxu->periodic_size);

	/* just check the specified uframe, at that period */
	} else {
		do {
			claimed = periodic_usecs(oxu, frame, uframe);
			if (claimed > usecs)
				return 0;
		} while ((frame += period) < oxu->periodic_size);
	}

	return 1;
}

static int check_intr_schedule(struct oxu_hcd	*oxu,
				unsigned frame, unsigned uframe,
				const struct ehci_qh *qh, __le32 *c_maskp)
{
	int retval = -ENOSPC;

	if (qh->c_usecs && uframe >= 6)		/* FSTN territory? */
		goto done;

	if (!check_period(oxu, frame, uframe, qh->period, qh->usecs))
		goto done;
	if (!qh->c_usecs) {
		retval = 0;
		*c_maskp = 0;
		goto done;
	}

done:
	return retval;
}

/* "first fit" scheduling policy used the first time through,
 * or when the previous schedule slot can't be re-used.
 */
static int qh_schedule(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	int		status;
	unsigned	uframe;
	__le32		c_mask;
	unsigned	frame;		/* 0..(qh->period - 1), or NO_FRAME */

	qh_refresh(oxu, qh);
	qh->hw_next = EHCI_LIST_END;
	frame = qh->start;

	/* reuse the previous schedule slots, if we can */
	if (frame < qh->period) {
		uframe = ffs(le32_to_cpup(&qh->hw_info2) & QH_SMASK);
		status = check_intr_schedule(oxu, frame, --uframe,
				qh, &c_mask);
	} else {
		uframe = 0;
		c_mask = 0;
		status = -ENOSPC;
	}

	/* else scan the schedule to find a group of slots such that all
	 * uframes have enough periodic bandwidth available.
	 */
	if (status) {
		/* "normal" case, uframing flexible except with splits */
		if (qh->period) {
			frame = qh->period - 1;
			do {
				for (uframe = 0; uframe < 8; uframe++) {
					status = check_intr_schedule(oxu,
							frame, uframe, qh,
							&c_mask);
					if (status == 0)
						break;
				}
			} while (status && frame--);

		/* qh->period == 0 means every uframe */
		} else {
			frame = 0;
			status = check_intr_schedule(oxu, 0, 0, qh, &c_mask);
		}
		if (status)
			goto done;
		qh->start = frame;

		/* reset S-frame and (maybe) C-frame masks */
		qh->hw_info2 &= cpu_to_le32(~(QH_CMASK | QH_SMASK));
		qh->hw_info2 |= qh->period
			? cpu_to_le32(1 << uframe)
			: cpu_to_le32(QH_SMASK);
		qh->hw_info2 |= c_mask;
	} else
		oxu_dbg(oxu, "reused qh %p schedule\n", qh);

	/* stuff into the periodic schedule */
	status = qh_link_periodic(oxu, qh);
done:
	return status;
}

static int intr_submit(struct oxu_hcd *oxu, struct urb *urb,
			struct list_head *qtd_list, gfp_t mem_flags)
{
	unsigned epnum;
	unsigned long flags;
	struct ehci_qh *qh;
	int status = 0;
	struct list_head	empty;

	/* get endpoint and transfer/schedule data */
	epnum = urb->ep->desc.bEndpointAddress;

	spin_lock_irqsave(&oxu->lock, flags);

	if (unlikely(!HCD_HW_ACCESSIBLE(oxu_to_hcd(oxu)))) {
		status = -ESHUTDOWN;
		goto done;
	}

	/* get qh and force any scheduling errors */
	INIT_LIST_HEAD(&empty);
	qh = qh_append_tds(oxu, urb, &empty, epnum, &urb->ep->hcpriv);
	if (qh == NULL) {
		status = -ENOMEM;
		goto done;
	}
	if (qh->qh_state == QH_STATE_IDLE) {
		status = qh_schedule(oxu, qh);
		if (status != 0)
			goto done;
	}

	/* then queue the urb's tds to the qh */
	qh = qh_append_tds(oxu, urb, qtd_list, epnum, &urb->ep->hcpriv);
	BUG_ON(qh == NULL);

	/* ... update usbfs periodic stats */
	oxu_to_hcd(oxu)->self.bandwidth_int_reqs++;

done:
	spin_unlock_irqrestore(&oxu->lock, flags);
	if (status)
		qtd_list_free(oxu, urb, qtd_list);

	return status;
}

static inline int itd_submit(struct oxu_hcd *oxu, struct urb *urb,
						gfp_t mem_flags)
{
	oxu_dbg(oxu, "iso support is missing!\n");
	return -ENOSYS;
}

static inline int sitd_submit(struct oxu_hcd *oxu, struct urb *urb,
						gfp_t mem_flags)
{
	oxu_dbg(oxu, "split iso support is missing!\n");
	return -ENOSYS;
}

static void scan_periodic(struct oxu_hcd *oxu)
{
	unsigned frame, clock, now_uframe, mod;
	unsigned modified;

	mod = oxu->periodic_size << 3;

	/*
	 * When running, scan from last scan point up to "now"
	 * else clean up by scanning everything that's left.
	 * Touches as few pages as possible:  cache-friendly.
	 */
	now_uframe = oxu->next_uframe;
	if (HC_IS_RUNNING(oxu_to_hcd(oxu)->state))
		clock = readl(&oxu->regs->frame_index);
	else
		clock = now_uframe + mod - 1;
	clock %= mod;

	for (;;) {
		union ehci_shadow	q, *q_p;
		__le32			type, *hw_p;

		/* don't scan past the live uframe */
		frame = now_uframe >> 3;
		if (frame != (clock >> 3)) {
			/* safe to scan the whole frame at once */
			now_uframe |= 0x07;
		}

restart:
		/* scan each element in frame's queue for completions */
		q_p = &oxu->pshadow[frame];
		hw_p = &oxu->periodic[frame];
		q.ptr = q_p->ptr;
		type = Q_NEXT_TYPE(*hw_p);
		modified = 0;

		while (q.ptr != NULL) {
			union ehci_shadow temp;

			switch (type) {
			case Q_TYPE_QH:
				/* handle any completions */
				temp.qh = qh_get(q.qh);
				type = Q_NEXT_TYPE(q.qh->hw_next);
				q = q.qh->qh_next;
				modified = qh_completions(oxu, temp.qh);
				if (unlikely(list_empty(&temp.qh->qtd_list)))
					intr_deschedule(oxu, temp.qh);
				qh_put(temp.qh);
				break;
			default:
				oxu_dbg(oxu, "corrupt type %d frame %d shadow %p\n",
					type, frame, q.ptr);
				q.ptr = NULL;
			}

			/* assume completion callbacks modify the queue */
			if (unlikely(modified))
				goto restart;
		}

		/* Stop when we catch up to the HC */

		/* FIXME:  this assumes we won't get lapped when
		 * latencies climb; that should be rare, but...
		 * detect it, and just go all the way around.
		 * FLR might help detect this case, so long as latencies
		 * don't exceed periodic_size msec (default 1.024 sec).
		 */

		/* FIXME: likewise assumes HC doesn't halt mid-scan */

		if (now_uframe == clock) {
			unsigned	now;

			if (!HC_IS_RUNNING(oxu_to_hcd(oxu)->state))
				break;
			oxu->next_uframe = now_uframe;
			now = readl(&oxu->regs->frame_index) % mod;
			if (now_uframe == now)
				break;

			/* rescan the rest of this frame, then ... */
			clock = now;
		} else {
			now_uframe++;
			now_uframe %= mod;
		}
	}
}

/* On some systems, leaving remote wakeup enabled prevents system shutdown.
 * The firmware seems to think that powering off is a wakeup event!
 * This routine turns off remote wakeup and everything else, on all ports.
 */
static void ehci_turn_off_all_ports(struct oxu_hcd *oxu)
{
	int port = HCS_N_PORTS(oxu->hcs_params);

	while (port--)
		writel(PORT_RWC_BITS, &oxu->regs->port_status[port]);
}

static void ehci_port_power(struct oxu_hcd *oxu, int is_on)
{
	unsigned port;

	if (!HCS_PPC(oxu->hcs_params))
		return;

	oxu_dbg(oxu, "...power%s ports...\n", is_on ? "up" : "down");
	for (port = HCS_N_PORTS(oxu->hcs_params); port > 0; ) {
		if (is_on)
			oxu_hub_control(oxu_to_hcd(oxu), SetPortFeature,
				USB_PORT_FEAT_POWER, port--, NULL, 0);
		else
			oxu_hub_control(oxu_to_hcd(oxu), ClearPortFeature,
				USB_PORT_FEAT_POWER, port--, NULL, 0);
	}

	msleep(20);
}

/* Called from some interrupts, timers, and so on.
 * It calls driver completion functions, after dropping oxu->lock.
 */
static void ehci_work(struct oxu_hcd *oxu)
{
	timer_action_done(oxu, TIMER_IO_WATCHDOG);
	if (oxu->reclaim_ready)
		end_unlink_async(oxu);

	/* another CPU may drop oxu->lock during a schedule scan while
	 * it reports urb completions.  this flag guards against bogus
	 * attempts at re-entrant schedule scanning.
	 */
	if (oxu->scanning)
		return;
	oxu->scanning = 1;
	scan_async(oxu);
	if (oxu->next_uframe != -1)
		scan_periodic(oxu);
	oxu->scanning = 0;

	/* the IO watchdog guards against hardware or driver bugs that
	 * misplace IRQs, and should let us run completely without IRQs.
	 * such lossage has been observed on both VT6202 and VT8235.
	 */
	if (HC_IS_RUNNING(oxu_to_hcd(oxu)->state) &&
			(oxu->async->qh_next.ptr != NULL ||
			 oxu->periodic_sched != 0))
		timer_action(oxu, TIMER_IO_WATCHDOG);
}

static void unlink_async(struct oxu_hcd *oxu, struct ehci_qh *qh)
{
	/* if we need to use IAA and it's busy, defer */
	if (qh->qh_state == QH_STATE_LINKED
			&& oxu->reclaim
			&& HC_IS_RUNNING(oxu_to_hcd(oxu)->state)) {
		struct ehci_qh		*last;

		for (last = oxu->reclaim;
				last->reclaim;
				last = last->reclaim)
			continue;
		qh->qh_state = QH_STATE_UNLINK_WAIT;
		last->reclaim = qh;

	/* bypass IAA if the hc can't care */
	} else if (!HC_IS_RUNNING(oxu_to_hcd(oxu)->state) && oxu->reclaim)
		end_unlink_async(oxu);

	/* something else might have unlinked the qh by now */
	if (qh->qh_state == QH_STATE_LINKED)
		start_unlink_async(oxu, qh);
}

/*
 * USB host controller methods
 */

static irqreturn_t oxu210_hcd_irq(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	u32 status, pcd_status = 0;
	int bh;

	spin_lock(&oxu->lock);

	status = readl(&oxu->regs->status);

	/* e.g. cardbus physical eject */
	if (status == ~(u32) 0) {
		oxu_dbg(oxu, "device removed\n");
		goto dead;
	}

	/* Shared IRQ? */
	status &= INTR_MASK;
	if (!status || unlikely(hcd->state == HC_STATE_HALT)) {
		spin_unlock(&oxu->lock);
		return IRQ_NONE;
	}

	/* clear (just) interrupts */
	writel(status, &oxu->regs->status);
	readl(&oxu->regs->command);	/* unblock posted write */
	bh = 0;

#ifdef OXU_VERBOSE_DEBUG
	/* unrequested/ignored: Frame List Rollover */
	dbg_status(oxu, "irq", status);
#endif

	/* INT, ERR, and IAA interrupt rates can be throttled */

	/* normal [4.15.1.2] or error [4.15.1.1] completion */
	if (likely((status & (STS_INT|STS_ERR)) != 0))
		bh = 1;

	/* complete the unlinking of some qh [4.15.2.3] */
	if (status & STS_IAA) {
		oxu->reclaim_ready = 1;
		bh = 1;
	}

	/* remote wakeup [4.3.1] */
	if (status & STS_PCD) {
		unsigned i = HCS_N_PORTS(oxu->hcs_params);
		pcd_status = status;

		/* resume root hub? */
		if (!(readl(&oxu->regs->command) & CMD_RUN))
			usb_hcd_resume_root_hub(hcd);

		while (i--) {
			int pstatus = readl(&oxu->regs->port_status[i]);

			if (pstatus & PORT_OWNER)
				continue;
			if (!(pstatus & PORT_RESUME)
					|| oxu->reset_done[i] != 0)
				continue;

			/* start USB_RESUME_TIMEOUT resume signaling from this
			 * port, and make hub_wq collect PORT_STAT_C_SUSPEND to
			 * stop that signaling.
			 */
			oxu->reset_done[i] = jiffies +
				msecs_to_jiffies(USB_RESUME_TIMEOUT);
			oxu_dbg(oxu, "port %d remote wakeup\n", i + 1);
			mod_timer(&hcd->rh_timer, oxu->reset_done[i]);
		}
	}

	/* PCI errors [4.15.2.4] */
	if (unlikely((status & STS_FATAL) != 0)) {
		/* bogus "fatal" IRQs appear on some chips... why?  */
		status = readl(&oxu->regs->status);
		dbg_cmd(oxu, "fatal", readl(&oxu->regs->command));
		dbg_status(oxu, "fatal", status);
		if (status & STS_HALT) {
			oxu_err(oxu, "fatal error\n");
dead:
			ehci_reset(oxu);
			writel(0, &oxu->regs->configured_flag);
			usb_hc_died(hcd);
			/* generic layer kills/unlinks all urbs, then
			 * uses oxu_stop to clean up the rest
			 */
			bh = 1;
		}
	}

	if (bh)
		ehci_work(oxu);
	spin_unlock(&oxu->lock);
	if (pcd_status & STS_PCD)
		usb_hcd_poll_rh_status(hcd);
	return IRQ_HANDLED;
}

static irqreturn_t oxu_irq(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	int ret = IRQ_HANDLED;

	u32 status = oxu_readl(hcd->regs, OXU_CHIPIRQSTATUS);
	u32 enable = oxu_readl(hcd->regs, OXU_CHIPIRQEN_SET);

	/* Disable all interrupt */
	oxu_writel(hcd->regs, OXU_CHIPIRQEN_CLR, enable);

	if ((oxu->is_otg && (status & OXU_USBOTGI)) ||
		(!oxu->is_otg && (status & OXU_USBSPHI)))
		oxu210_hcd_irq(hcd);
	else
		ret = IRQ_NONE;

	/* Enable all interrupt back */
	oxu_writel(hcd->regs, OXU_CHIPIRQEN_SET, enable);

	return ret;
}

static void oxu_watchdog(struct timer_list *t)
{
	struct oxu_hcd	*oxu = from_timer(oxu, t, watchdog);
	unsigned long flags;

	spin_lock_irqsave(&oxu->lock, flags);

	/* lost IAA irqs wedge things badly; seen with a vt8235 */
	if (oxu->reclaim) {
		u32 status = readl(&oxu->regs->status);
		if (status & STS_IAA) {
			oxu_vdbg(oxu, "lost IAA\n");
			writel(STS_IAA, &oxu->regs->status);
			oxu->reclaim_ready = 1;
		}
	}

	/* stop async processing after it's idled a bit */
	if (test_bit(TIMER_ASYNC_OFF, &oxu->actions))
		start_unlink_async(oxu, oxu->async);

	/* oxu could run by timer, without IRQs ... */
	ehci_work(oxu);

	spin_unlock_irqrestore(&oxu->lock, flags);
}

/* One-time init, only for memory state.
 */
static int oxu_hcd_init(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	u32 temp;
	int retval;
	u32 hcc_params;

	spin_lock_init(&oxu->lock);

	timer_setup(&oxu->watchdog, oxu_watchdog, 0);

	/*
	 * hw default: 1K periodic list heads, one per frame.
	 * periodic_size can shrink by USBCMD update if hcc_params allows.
	 */
	oxu->periodic_size = DEFAULT_I_TDPS;
	retval = ehci_mem_init(oxu, GFP_KERNEL);
	if (retval < 0)
		return retval;

	/* controllers may cache some of the periodic schedule ... */
	hcc_params = readl(&oxu->caps->hcc_params);
	if (HCC_ISOC_CACHE(hcc_params))		/* full frame cache */
		oxu->i_thresh = 8;
	else					/* N microframes cached */
		oxu->i_thresh = 2 + HCC_ISOC_THRES(hcc_params);

	oxu->reclaim = NULL;
	oxu->reclaim_ready = 0;
	oxu->next_uframe = -1;

	/*
	 * dedicate a qh for the async ring head, since we couldn't unlink
	 * a 'real' qh without stopping the async schedule [4.8].  use it
	 * as the 'reclamation list head' too.
	 * its dummy is used in hw_alt_next of many tds, to prevent the qh
	 * from automatically advancing to the next td after short reads.
	 */
	oxu->async->qh_next.qh = NULL;
	oxu->async->hw_next = QH_NEXT(oxu->async->qh_dma);
	oxu->async->hw_info1 = cpu_to_le32(QH_HEAD);
	oxu->async->hw_token = cpu_to_le32(QTD_STS_HALT);
	oxu->async->hw_qtd_next = EHCI_LIST_END;
	oxu->async->qh_state = QH_STATE_LINKED;
	oxu->async->hw_alt_next = QTD_NEXT(oxu->async->dummy->qtd_dma);

	/* clear interrupt enables, set irq latency */
	if (log2_irq_thresh < 0 || log2_irq_thresh > 6)
		log2_irq_thresh = 0;
	temp = 1 << (16 + log2_irq_thresh);
	if (HCC_CANPARK(hcc_params)) {
		/* HW default park == 3, on hardware that supports it (like
		 * NVidia and ALI silicon), maximizes throughput on the async
		 * schedule by avoiding QH fetches between transfers.
		 *
		 * With fast usb storage devices and NForce2, "park" seems to
		 * make problems:  throughput reduction (!), data errors...
		 */
		if (park) {
			park = min(park, (unsigned) 3);
			temp |= CMD_PARK;
			temp |= park << 8;
		}
		oxu_dbg(oxu, "park %d\n", park);
	}
	if (HCC_PGM_FRAMELISTLEN(hcc_params)) {
		/* periodic schedule size can be smaller than default */
		temp &= ~(3 << 2);
		temp |= (EHCI_TUNE_FLS << 2);
	}
	oxu->command = temp;

	return 0;
}

/* Called during probe() after chip reset completes.
 */
static int oxu_reset(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);

	spin_lock_init(&oxu->mem_lock);
	INIT_LIST_HEAD(&oxu->urb_list);
	oxu->urb_len = 0;

	if (oxu->is_otg) {
		oxu->caps = hcd->regs + OXU_OTG_CAP_OFFSET;
		oxu->regs = hcd->regs + OXU_OTG_CAP_OFFSET + \
			HC_LENGTH(readl(&oxu->caps->hc_capbase));

		oxu->mem = hcd->regs + OXU_SPH_MEM;
	} else {
		oxu->caps = hcd->regs + OXU_SPH_CAP_OFFSET;
		oxu->regs = hcd->regs + OXU_SPH_CAP_OFFSET + \
			HC_LENGTH(readl(&oxu->caps->hc_capbase));

		oxu->mem = hcd->regs + OXU_OTG_MEM;
	}

	oxu->hcs_params = readl(&oxu->caps->hcs_params);
	oxu->sbrn = 0x20;

	return oxu_hcd_init(hcd);
}

static int oxu_run(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	int retval;
	u32 temp, hcc_params;

	hcd->uses_new_polling = 1;

	/* EHCI spec section 4.1 */
	retval = ehci_reset(oxu);
	if (retval != 0) {
		ehci_mem_cleanup(oxu);
		return retval;
	}
	writel(oxu->periodic_dma, &oxu->regs->frame_list);
	writel((u32) oxu->async->qh_dma, &oxu->regs->async_next);

	/* hcc_params controls whether oxu->regs->segment must (!!!)
	 * be used; it constrains QH/ITD/SITD and QTD locations.
	 * dma_pool consistent memory always uses segment zero.
	 * streaming mappings for I/O buffers, like pci_map_single(),
	 * can return segments above 4GB, if the device allows.
	 *
	 * NOTE:  the dma mask is visible through dev->dma_mask, so
	 * drivers can pass this info along ... like NETIF_F_HIGHDMA,
	 * Scsi_Host.highmem_io, and so forth.  It's readonly to all
	 * host side drivers though.
	 */
	hcc_params = readl(&oxu->caps->hcc_params);
	if (HCC_64BIT_ADDR(hcc_params))
		writel(0, &oxu->regs->segment);

	oxu->command &= ~(CMD_LRESET | CMD_IAAD | CMD_PSE |
				CMD_ASE | CMD_RESET);
	oxu->command |= CMD_RUN;
	writel(oxu->command, &oxu->regs->command);
	dbg_cmd(oxu, "init", oxu->command);

	/*
	 * Start, enabling full USB 2.0 functionality ... usb 1.1 devices
	 * are explicitly handed to companion controller(s), so no TT is
	 * involved with the root hub.  (Except where one is integrated,
	 * and there's no companion controller unless maybe for USB OTG.)
	 */
	hcd->state = HC_STATE_RUNNING;
	writel(FLAG_CF, &oxu->regs->configured_flag);
	readl(&oxu->regs->command);	/* unblock posted writes */

	temp = HC_VERSION(readl(&oxu->caps->hc_capbase));
	oxu_info(oxu, "USB %x.%x started, quasi-EHCI %x.%02x, driver %s%s\n",
		((oxu->sbrn & 0xf0)>>4), (oxu->sbrn & 0x0f),
		temp >> 8, temp & 0xff, DRIVER_VERSION,
		ignore_oc ? ", overcurrent ignored" : "");

	writel(INTR_MASK, &oxu->regs->intr_enable); /* Turn On Interrupts */

	return 0;
}

static void oxu_stop(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);

	/* Turn off port power on all root hub ports. */
	ehci_port_power(oxu, 0);

	/* no more interrupts ... */
	del_timer_sync(&oxu->watchdog);

	spin_lock_irq(&oxu->lock);
	if (HC_IS_RUNNING(hcd->state))
		ehci_quiesce(oxu);

	ehci_reset(oxu);
	writel(0, &oxu->regs->intr_enable);
	spin_unlock_irq(&oxu->lock);

	/* let companion controllers work when we aren't */
	writel(0, &oxu->regs->configured_flag);

	/* root hub is shut down separately (first, when possible) */
	spin_lock_irq(&oxu->lock);
	if (oxu->async)
		ehci_work(oxu);
	spin_unlock_irq(&oxu->lock);
	ehci_mem_cleanup(oxu);

	dbg_status(oxu, "oxu_stop completed", readl(&oxu->regs->status));
}

/* Kick in for silicon on any bus (not just pci, etc).
 * This forcibly disables dma and IRQs, helping kexec and other cases
 * where the next system software may expect clean state.
 */
static void oxu_shutdown(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);

	(void) ehci_halt(oxu);
	ehci_turn_off_all_ports(oxu);

	/* make BIOS/etc use companion controller during reboot */
	writel(0, &oxu->regs->configured_flag);

	/* unblock posted writes */
	readl(&oxu->regs->configured_flag);
}

/* Non-error returns are a promise to giveback() the urb later
 * we drop ownership so next owner (or urb unlink) can get it
 *
 * urb + dev is in hcd.self.controller.urb_list
 * we're queueing TDs onto software and hardware lists
 *
 * hcd-specific init for hcpriv hasn't been done yet
 *
 * NOTE:  control, bulk, and interrupt share the same code to append TDs
 * to a (possibly active) QH, and the same QH scanning code.
 */
static int __oxu_urb_enqueue(struct usb_hcd *hcd, struct urb *urb,
				gfp_t mem_flags)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	struct list_head qtd_list;

	INIT_LIST_HEAD(&qtd_list);

	switch (usb_pipetype(urb->pipe)) {
	case PIPE_CONTROL:
	case PIPE_BULK:
	default:
		if (!qh_urb_transaction(oxu, urb, &qtd_list, mem_flags))
			return -ENOMEM;
		return submit_async(oxu, urb, &qtd_list, mem_flags);

	case PIPE_INTERRUPT:
		if (!qh_urb_transaction(oxu, urb, &qtd_list, mem_flags))
			return -ENOMEM;
		return intr_submit(oxu, urb, &qtd_list, mem_flags);

	case PIPE_ISOCHRONOUS:
		if (urb->dev->speed == USB_SPEED_HIGH)
			return itd_submit(oxu, urb, mem_flags);
		else
			return sitd_submit(oxu, urb, mem_flags);
	}
}

/* This function is responsible for breaking URBs with big data size
 * into smaller size and processing small urbs in sequence.
 */
static int oxu_urb_enqueue(struct usb_hcd *hcd, struct urb *urb,
				gfp_t mem_flags)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	int num, rem;
	void *transfer_buffer;
	struct urb *murb;
	int i, ret;

	/* If not bulk pipe just enqueue the URB */
	if (!usb_pipebulk(urb->pipe))
		return __oxu_urb_enqueue(hcd, urb, mem_flags);

	/* Otherwise we should verify the USB transfer buffer size! */
	transfer_buffer = urb->transfer_buffer;

	num = urb->transfer_buffer_length / 4096;
	rem = urb->transfer_buffer_length % 4096;
	if (rem != 0)
		num++;

	/* If URB is smaller than 4096 bytes just enqueue it! */
	if (num == 1)
		return __oxu_urb_enqueue(hcd, urb, mem_flags);

	/* Ok, we have more job to do! :) */

	for (i = 0; i < num - 1; i++) {
		/* Get free micro URB poll till a free urb is received */

		do {
			murb = (struct urb *) oxu_murb_alloc(oxu);
			if (!murb)
				schedule();
		} while (!murb);

		/* Coping the urb */
		memcpy(murb, urb, sizeof(struct urb));

		murb->transfer_buffer_length = 4096;
		murb->transfer_buffer = transfer_buffer + i * 4096;

		/* Null pointer for the encodes that this is a micro urb */
		murb->complete = NULL;

		((struct oxu_murb *) murb)->main = urb;
		((struct oxu_murb *) murb)->last = 0;

		/* This loop is to guarantee urb to be processed when there's
		 * not enough resources at a particular time by retrying.
		 */
		do {
			ret  = __oxu_urb_enqueue(hcd, murb, mem_flags);
			if (ret)
				schedule();
		} while (ret);
	}

	/* Last urb requires special handling  */

	/* Get free micro URB poll till a free urb is received */
	do {
		murb = (struct urb *) oxu_murb_alloc(oxu);
		if (!murb)
			schedule();
	} while (!murb);

	/* Coping the urb */
	memcpy(murb, urb, sizeof(struct urb));

	murb->transfer_buffer_length = rem > 0 ? rem : 4096;
	murb->transfer_buffer = transfer_buffer + (num - 1) * 4096;

	/* Null pointer for the encodes that this is a micro urb */
	murb->complete = NULL;

	((struct oxu_murb *) murb)->main = urb;
	((struct oxu_murb *) murb)->last = 1;

	do {
		ret = __oxu_urb_enqueue(hcd, murb, mem_flags);
		if (ret)
			schedule();
	} while (ret);

	return ret;
}

/* Remove from hardware lists.
 * Completions normally happen asynchronously
 */
static int oxu_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	struct ehci_qh *qh;
	unsigned long flags;

	spin_lock_irqsave(&oxu->lock, flags);
	switch (usb_pipetype(urb->pipe)) {
	case PIPE_CONTROL:
	case PIPE_BULK:
	default:
		qh = (struct ehci_qh *) urb->hcpriv;
		if (!qh)
			break;
		unlink_async(oxu, qh);
		break;

	case PIPE_INTERRUPT:
		qh = (struct ehci_qh *) urb->hcpriv;
		if (!qh)
			break;
		switch (qh->qh_state) {
		case QH_STATE_LINKED:
			intr_deschedule(oxu, qh);
			fallthrough;
		case QH_STATE_IDLE:
			qh_completions(oxu, qh);
			break;
		default:
			oxu_dbg(oxu, "bogus qh %p state %d\n",
					qh, qh->qh_state);
			goto done;
		}

		/* reschedule QH iff another request is queued */
		if (!list_empty(&qh->qtd_list)
				&& HC_IS_RUNNING(hcd->state)) {
			int status;

			status = qh_schedule(oxu, qh);
			spin_unlock_irqrestore(&oxu->lock, flags);

			if (status != 0) {
				/* shouldn't happen often, but ...
				 * FIXME kill those tds' urbs
				 */
				dev_err(hcd->self.controller,
					"can't reschedule qh %p, err %d\n", qh,
					status);
			}
			return status;
		}
		break;
	}
done:
	spin_unlock_irqrestore(&oxu->lock, flags);
	return 0;
}

/* Bulk qh holds the data toggle */
static void oxu_endpoint_disable(struct usb_hcd *hcd,
					struct usb_host_endpoint *ep)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	unsigned long		flags;
	struct ehci_qh		*qh, *tmp;

	/* ASSERT:  any requests/urbs are being unlinked */
	/* ASSERT:  nobody can be submitting urbs for this any more */

rescan:
	spin_lock_irqsave(&oxu->lock, flags);
	qh = ep->hcpriv;
	if (!qh)
		goto done;

	/* endpoints can be iso streams.  for now, we don't
	 * accelerate iso completions ... so spin a while.
	 */
	if (qh->hw_info1 == 0) {
		oxu_vdbg(oxu, "iso delay\n");
		goto idle_timeout;
	}

	if (!HC_IS_RUNNING(hcd->state))
		qh->qh_state = QH_STATE_IDLE;
	switch (qh->qh_state) {
	case QH_STATE_LINKED:
		for (tmp = oxu->async->qh_next.qh;
				tmp && tmp != qh;
				tmp = tmp->qh_next.qh)
			continue;
		/* periodic qh self-unlinks on empty */
		if (!tmp)
			goto nogood;
		unlink_async(oxu, qh);
		fallthrough;
	case QH_STATE_UNLINK:		/* wait for hw to finish? */
idle_timeout:
		spin_unlock_irqrestore(&oxu->lock, flags);
		schedule_timeout_uninterruptible(1);
		goto rescan;
	case QH_STATE_IDLE:		/* fully unlinked */
		if (list_empty(&qh->qtd_list)) {
			qh_put(qh);
			break;
		}
		fallthrough;
	default:
nogood:
		/* caller was supposed to have unlinked any requests;
		 * that's not our job.  just leak this memory.
		 */
		oxu_err(oxu, "qh %p (#%02x) state %d%s\n",
			qh, ep->desc.bEndpointAddress, qh->qh_state,
			list_empty(&qh->qtd_list) ? "" : "(has tds)");
		break;
	}
	ep->hcpriv = NULL;
done:
	spin_unlock_irqrestore(&oxu->lock, flags);
}

static int oxu_get_frame(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);

	return (readl(&oxu->regs->frame_index) >> 3) %
		oxu->periodic_size;
}

/* Build "status change" packet (one or two bytes) from HC registers */
static int oxu_hub_status_data(struct usb_hcd *hcd, char *buf)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	u32 temp, mask, status = 0;
	int ports, i, retval = 1;
	unsigned long flags;

	/* if !PM, root hub timers won't get shut down ... */
	if (!HC_IS_RUNNING(hcd->state))
		return 0;

	/* init status to no-changes */
	buf[0] = 0;
	ports = HCS_N_PORTS(oxu->hcs_params);
	if (ports > 7) {
		buf[1] = 0;
		retval++;
	}

	/* Some boards (mostly VIA?) report bogus overcurrent indications,
	 * causing massive log spam unless we completely ignore them.  It
	 * may be relevant that VIA VT8235 controllers, where PORT_POWER is
	 * always set, seem to clear PORT_OCC and PORT_CSC when writing to
	 * PORT_POWER; that's surprising, but maybe within-spec.
	 */
	if (!ignore_oc)
		mask = PORT_CSC | PORT_PEC | PORT_OCC;
	else
		mask = PORT_CSC | PORT_PEC;

	/* no hub change reports (bit 0) for now (power, ...) */

	/* port N changes (bit N)? */
	spin_lock_irqsave(&oxu->lock, flags);
	for (i = 0; i < ports; i++) {
		temp = readl(&oxu->regs->port_status[i]);

		/*
		 * Return status information even for ports with OWNER set.
		 * Otherwise hub_wq wouldn't see the disconnect event when a
		 * high-speed device is switched over to the companion
		 * controller by the user.
		 */

		if (!(temp & PORT_CONNECT))
			oxu->reset_done[i] = 0;
		if ((temp & mask) != 0 || ((temp & PORT_RESUME) != 0 &&
				time_after_eq(jiffies, oxu->reset_done[i]))) {
			if (i < 7)
				buf[0] |= 1 << (i + 1);
			else
				buf[1] |= 1 << (i - 7);
			status = STS_PCD;
		}
	}
	/* FIXME autosuspend idle root hubs */
	spin_unlock_irqrestore(&oxu->lock, flags);
	return status ? retval : 0;
}

/* Returns the speed of a device attached to a port on the root hub. */
static inline unsigned int oxu_port_speed(struct oxu_hcd *oxu,
						unsigned int portsc)
{
	switch ((portsc >> 26) & 3) {
	case 0:
		return 0;
	case 1:
		return USB_PORT_STAT_LOW_SPEED;
	case 2:
	default:
		return USB_PORT_STAT_HIGH_SPEED;
	}
}

#define	PORT_WAKE_BITS	(PORT_WKOC_E|PORT_WKDISC_E|PORT_WKCONN_E)
static int oxu_hub_control(struct usb_hcd *hcd, u16 typeReq,
				u16 wValue, u16 wIndex, char *buf, u16 wLength)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	int ports = HCS_N_PORTS(oxu->hcs_params);
	u32 __iomem *status_reg = &oxu->regs->port_status[wIndex - 1];
	u32 temp, status;
	unsigned long	flags;
	int retval = 0;
	unsigned selector;

	/*
	 * FIXME:  support SetPortFeatures USB_PORT_FEAT_INDICATOR.
	 * HCS_INDICATOR may say we can change LEDs to off/amber/green.
	 * (track current state ourselves) ... blink for diagnostics,
	 * power, "this is the one", etc.  EHCI spec supports this.
	 */

	spin_lock_irqsave(&oxu->lock, flags);
	switch (typeReq) {
	case ClearHubFeature:
		switch (wValue) {
		case C_HUB_LOCAL_POWER:
		case C_HUB_OVER_CURRENT:
			/* no hub-wide feature/status flags */
			break;
		default:
			goto error;
		}
		break;
	case ClearPortFeature:
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;
		temp = readl(status_reg);

		/*
		 * Even if OWNER is set, so the port is owned by the
		 * companion controller, hub_wq needs to be able to clear
		 * the port-change status bits (especially
		 * USB_PORT_STAT_C_CONNECTION).
		 */

		switch (wValue) {
		case USB_PORT_FEAT_ENABLE:
			writel(temp & ~PORT_PE, status_reg);
			break;
		case USB_PORT_FEAT_C_ENABLE:
			writel((temp & ~PORT_RWC_BITS) | PORT_PEC, status_reg);
			break;
		case USB_PORT_FEAT_SUSPEND:
			if (temp & PORT_RESET)
				goto error;
			if (temp & PORT_SUSPEND) {
				if ((temp & PORT_PE) == 0)
					goto error;
				/* resume signaling for 20 msec */
				temp &= ~(PORT_RWC_BITS | PORT_WAKE_BITS);
				writel(temp | PORT_RESUME, status_reg);
				oxu->reset_done[wIndex] = jiffies
						+ msecs_to_jiffies(20);
			}
			break;
		case USB_PORT_FEAT_C_SUSPEND:
			/* we auto-clear this feature */
			break;
		case USB_PORT_FEAT_POWER:
			if (HCS_PPC(oxu->hcs_params))
				writel(temp & ~(PORT_RWC_BITS | PORT_POWER),
					  status_reg);
			break;
		case USB_PORT_FEAT_C_CONNECTION:
			writel((temp & ~PORT_RWC_BITS) | PORT_CSC, status_reg);
			break;
		case USB_PORT_FEAT_C_OVER_CURRENT:
			writel((temp & ~PORT_RWC_BITS) | PORT_OCC, status_reg);
			break;
		case USB_PORT_FEAT_C_RESET:
			/* GetPortStatus clears reset */
			break;
		default:
			goto error;
		}
		readl(&oxu->regs->command);	/* unblock posted write */
		break;
	case GetHubDescriptor:
		ehci_hub_descriptor(oxu, (struct usb_hub_descriptor *)
			buf);
		break;
	case GetHubStatus:
		/* no hub-wide feature/status flags */
		memset(buf, 0, 4);
		break;
	case GetPortStatus:
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;
		status = 0;
		temp = readl(status_reg);

		/* wPortChange bits */
		if (temp & PORT_CSC)
			status |= USB_PORT_STAT_C_CONNECTION << 16;
		if (temp & PORT_PEC)
			status |= USB_PORT_STAT_C_ENABLE << 16;
		if ((temp & PORT_OCC) && !ignore_oc)
			status |= USB_PORT_STAT_C_OVERCURRENT << 16;

		/* whoever resumes must GetPortStatus to complete it!! */
		if (temp & PORT_RESUME) {

			/* Remote Wakeup received? */
			if (!oxu->reset_done[wIndex]) {
				/* resume signaling for 20 msec */
				oxu->reset_done[wIndex] = jiffies
						+ msecs_to_jiffies(20);
				/* check the port again */
				mod_timer(&oxu_to_hcd(oxu)->rh_timer,
						oxu->reset_done[wIndex]);
			}

			/* resume completed? */
			else if (time_after_eq(jiffies,
					oxu->reset_done[wIndex])) {
				status |= USB_PORT_STAT_C_SUSPEND << 16;
				oxu->reset_done[wIndex] = 0;

				/* stop resume signaling */
				temp = readl(status_reg);
				writel(temp & ~(PORT_RWC_BITS | PORT_RESUME),
					status_reg);
				retval = handshake(oxu, status_reg,
					   PORT_RESUME, 0, 2000 /* 2msec */);
				if (retval != 0) {
					oxu_err(oxu,
						"port %d resume error %d\n",
						wIndex + 1, retval);
					goto error;
				}
				temp &= ~(PORT_SUSPEND|PORT_RESUME|(3<<10));
			}
		}

		/* whoever resets must GetPortStatus to complete it!! */
		if ((temp & PORT_RESET)
				&& time_after_eq(jiffies,
					oxu->reset_done[wIndex])) {
			status |= USB_PORT_STAT_C_RESET << 16;
			oxu->reset_done[wIndex] = 0;

			/* force reset to complete */
			writel(temp & ~(PORT_RWC_BITS | PORT_RESET),
					status_reg);
			/* REVISIT:  some hardware needs 550+ usec to clear
			 * this bit; seems too long to spin routinely...
			 */
			retval = handshake(oxu, status_reg,
					PORT_RESET, 0, 750);
			if (retval != 0) {
				oxu_err(oxu, "port %d reset error %d\n",
					wIndex + 1, retval);
				goto error;
			}

			/* see what we found out */
			temp = check_reset_complete(oxu, wIndex, status_reg,
					readl(status_reg));
		}

		/* transfer dedicated ports to the companion hc */
		if ((temp & PORT_CONNECT) &&
				test_bit(wIndex, &oxu->companion_ports)) {
			temp &= ~PORT_RWC_BITS;
			temp |= PORT_OWNER;
			writel(temp, status_reg);
			oxu_dbg(oxu, "port %d --> companion\n", wIndex + 1);
			temp = readl(status_reg);
		}

		/*
		 * Even if OWNER is set, there's no harm letting hub_wq
		 * see the wPortStatus values (they should all be 0 except
		 * for PORT_POWER anyway).
		 */

		if (temp & PORT_CONNECT) {
			status |= USB_PORT_STAT_CONNECTION;
			/* status may be from integrated TT */
			status |= oxu_port_speed(oxu, temp);
		}
		if (temp & PORT_PE)
			status |= USB_PORT_STAT_ENABLE;
		if (temp & (PORT_SUSPEND|PORT_RESUME))
			status |= USB_PORT_STAT_SUSPEND;
		if (temp & PORT_OC)
			status |= USB_PORT_STAT_OVERCURRENT;
		if (temp & PORT_RESET)
			status |= USB_PORT_STAT_RESET;
		if (temp & PORT_POWER)
			status |= USB_PORT_STAT_POWER;

#ifndef	OXU_VERBOSE_DEBUG
	if (status & ~0xffff)	/* only if wPortChange is interesting */
#endif
		dbg_port(oxu, "GetStatus", wIndex + 1, temp);
		put_unaligned(cpu_to_le32(status), (__le32 *) buf);
		break;
	case SetHubFeature:
		switch (wValue) {
		case C_HUB_LOCAL_POWER:
		case C_HUB_OVER_CURRENT:
			/* no hub-wide feature/status flags */
			break;
		default:
			goto error;
		}
		break;
	case SetPortFeature:
		selector = wIndex >> 8;
		wIndex &= 0xff;
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;
		temp = readl(status_reg);
		if (temp & PORT_OWNER)
			break;

		temp &= ~PORT_RWC_BITS;
		switch (wValue) {
		case USB_PORT_FEAT_SUSPEND:
			if ((temp & PORT_PE) == 0
					|| (temp & PORT_RESET) != 0)
				goto error;
			if (device_may_wakeup(&hcd->self.root_hub->dev))
				temp |= PORT_WAKE_BITS;
			writel(temp | PORT_SUSPEND, status_reg);
			break;
		case USB_PORT_FEAT_POWER:
			if (HCS_PPC(oxu->hcs_params))
				writel(temp | PORT_POWER, status_reg);
			break;
		case USB_PORT_FEAT_RESET:
			if (temp & PORT_RESUME)
				goto error;
			/* line status bits may report this as low speed,
			 * which can be fine if this root hub has a
			 * transaction translator built in.
			 */
			oxu_vdbg(oxu, "port %d reset\n", wIndex + 1);
			temp |= PORT_RESET;
			temp &= ~PORT_PE;

			/*
			 * caller must wait, then call GetPortStatus
			 * usb 2.0 spec says 50 ms resets on root
			 */
			oxu->reset_done[wIndex] = jiffies
					+ msecs_to_jiffies(50);
			writel(temp, status_reg);
			break;

		/* For downstream facing ports (these):  one hub port is put
		 * into test mode according to USB2 11.24.2.13, then the hub
		 * must be reset (which for root hub now means rmmod+modprobe,
		 * or else system reboot).  See EHCI 2.3.9 and 4.14 for info
		 * about the EHCI-specific stuff.
		 */
		case USB_PORT_FEAT_TEST:
			if (!selector || selector > 5)
				goto error;
			ehci_quiesce(oxu);
			ehci_halt(oxu);
			temp |= selector << 16;
			writel(temp, status_reg);
			break;

		default:
			goto error;
		}
		readl(&oxu->regs->command);	/* unblock posted writes */
		break;

	default:
error:
		/* "stall" on error */
		retval = -EPIPE;
	}
	spin_unlock_irqrestore(&oxu->lock, flags);
	return retval;
}

#ifdef CONFIG_PM

static int oxu_bus_suspend(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	int port;
	int mask;

	oxu_dbg(oxu, "suspend root hub\n");

	if (time_before(jiffies, oxu->next_statechange))
		msleep(5);

	port = HCS_N_PORTS(oxu->hcs_params);
	spin_lock_irq(&oxu->lock);

	/* stop schedules, clean any completed work */
	if (HC_IS_RUNNING(hcd->state)) {
		ehci_quiesce(oxu);
		hcd->state = HC_STATE_QUIESCING;
	}
	oxu->command = readl(&oxu->regs->command);
	if (oxu->reclaim)
		oxu->reclaim_ready = 1;
	ehci_work(oxu);

	/* Unlike other USB host controller types, EHCI doesn't have
	 * any notion of "global" or bus-wide suspend.  The driver has
	 * to manually suspend all the active unsuspended ports, and
	 * then manually resume them in the bus_resume() routine.
	 */
	oxu->bus_suspended = 0;
	while (port--) {
		u32 __iomem *reg = &oxu->regs->port_status[port];
		u32 t1 = readl(reg) & ~PORT_RWC_BITS;
		u32 t2 = t1;

		/* keep track of which ports we suspend */
		if ((t1 & PORT_PE) && !(t1 & PORT_OWNER) &&
				!(t1 & PORT_SUSPEND)) {
			t2 |= PORT_SUSPEND;
			set_bit(port, &oxu->bus_suspended);
		}

		/* enable remote wakeup on all ports */
		if (device_may_wakeup(&hcd->self.root_hub->dev))
			t2 |= PORT_WKOC_E|PORT_WKDISC_E|PORT_WKCONN_E;
		else
			t2 &= ~(PORT_WKOC_E|PORT_WKDISC_E|PORT_WKCONN_E);

		if (t1 != t2) {
			oxu_vdbg(oxu, "port %d, %08x -> %08x\n",
				port + 1, t1, t2);
			writel(t2, reg);
		}
	}

	/* turn off now-idle HC */
	del_timer_sync(&oxu->watchdog);
	ehci_halt(oxu);
	hcd->state = HC_STATE_SUSPENDED;

	/* allow remote wakeup */
	mask = INTR_MASK;
	if (!device_may_wakeup(&hcd->self.root_hub->dev))
		mask &= ~STS_PCD;
	writel(mask, &oxu->regs->intr_enable);
	readl(&oxu->regs->intr_enable);

	oxu->next_statechange = jiffies + msecs_to_jiffies(10);
	spin_unlock_irq(&oxu->lock);
	return 0;
}

/* Caller has locked the root hub, and should reset/reinit on error */
static int oxu_bus_resume(struct usb_hcd *hcd)
{
	struct oxu_hcd *oxu = hcd_to_oxu(hcd);
	u32 temp;
	int i;

	if (time_before(jiffies, oxu->next_statechange))
		msleep(5);
	spin_lock_irq(&oxu->lock);

	/* Ideally and we've got a real resume here, and no port's power
	 * was lost.  (For PCI, that means Vaux was maintained.)  But we
	 * could instead be restoring a swsusp snapshot -- so that BIOS was
	 * the last user of the controller, not reset/pm hardware keeping
	 * state we gave to it.
	 */
	temp = readl(&oxu->regs->intr_enable);
	oxu_dbg(oxu, "resume root hub%s\n", temp ? "" : " after power loss");

	/* at least some APM implementations will try to deliver
	 * IRQs right away, so delay them until we're ready.
	 */
	writel(0, &oxu->regs->intr_enable);

	/* re-init operational registers */
	writel(0, &oxu->regs->segment);
	writel(oxu->periodic_dma, &oxu->regs->frame_list);
	writel((u32) oxu->async->qh_dma, &oxu->regs->async_next);

	/* restore CMD_RUN, framelist size, and irq threshold */
	writel(oxu->command, &oxu->regs->command);

	/* Some controller/firmware combinations need a delay during which
	 * they set up the port statuses.  See Bugzilla #8190. */
	mdelay(8);

	/* manually resume the ports we suspended during bus_suspend() */
	i = HCS_N_PORTS(oxu->hcs_params);
	while (i--) {
		temp = readl(&oxu->regs->port_status[i]);
		temp &= ~(PORT_RWC_BITS
			| PORT_WKOC_E | PORT_WKDISC_E | PORT_WKCONN_E);
		if (test_bit(i, &oxu->bus_suspended) && (temp & PORT_SUSPEND)) {
			oxu->reset_done[i] = jiffies + msecs_to_jiffies(20);
			temp |= PORT_RESUME;
		}
		writel(temp, &oxu->regs->port_status[i]);
	}
	i = HCS_N_PORTS(oxu->hcs_params);
	mdelay(20);
	while (i--) {
		temp = readl(&oxu->regs->port_status[i]);
		if (test_bit(i, &oxu->bus_suspended) && (temp & PORT_SUSPEND)) {
			temp &= ~(PORT_RWC_BITS | PORT_RESUME);
			writel(temp, &oxu->regs->port_status[i]);
			oxu_vdbg(oxu, "resumed port %d\n", i + 1);
		}
	}
	(void) readl(&oxu->regs->command);

	/* maybe re-activate the schedule(s) */
	temp = 0;
	if (oxu->async->qh_next.qh)
		temp |= CMD_ASE;
	if (oxu->periodic_sched)
		temp |= CMD_PSE;
	if (temp) {
		oxu->command |= temp;
		writel(oxu->command, &oxu->regs->command);
	}

	oxu->next_statechange = jiffies + msecs_to_jiffies(5);
	hcd->state = HC_STATE_RUNNING;

	/* Now we can safely re-enable irqs */
	writel(INTR_MASK, &oxu->regs->intr_enable);

	spin_unlock_irq(&oxu->lock);
	return 0;
}

#else

static int oxu_bus_suspend(struct usb_hcd *hcd)
{
	return 0;
}

static int oxu_bus_resume(struct usb_hcd *hcd)
{
	return 0;
}

#endif	/* CONFIG_PM */

static const struct hc_driver oxu_hc_driver = {
	.description =		"oxu210hp_hcd",
	.product_desc =		"oxu210hp HCD",
	.hcd_priv_size =	sizeof(struct oxu_hcd),

	/*
	 * Generic hardware linkage
	 */
	.irq =			oxu_irq,
	.flags =		HCD_MEMORY | HCD_USB2,

	/*
	 * Basic lifecycle operations
	 */
	.reset =		oxu_reset,
	.start =		oxu_run,
	.stop =			oxu_stop,
	.shutdown =		oxu_shutdown,

	/*
	 * Managing i/o requests and associated device resources
	 */
	.urb_enqueue =		oxu_urb_enqueue,
	.urb_dequeue =		oxu_urb_dequeue,
	.endpoint_disable =	oxu_endpoint_disable,

	/*
	 * Scheduling support
	 */
	.get_frame_number =	oxu_get_frame,

	/*
	 * Root hub support
	 */
	.hub_status_data =	oxu_hub_status_data,
	.hub_control =		oxu_hub_control,
	.bus_suspend =		oxu_bus_suspend,
	.bus_resume =		oxu_bus_resume,
};

/*
 * Module stuff
 */

static void oxu_configuration(struct platform_device *pdev, void __iomem *base)
{
	u32 tmp;

	/* Initialize top level registers.
	 * First write ever
	 */
	oxu_writel(base, OXU_HOSTIFCONFIG, 0x0000037D);
	oxu_writel(base, OXU_SOFTRESET, OXU_SRESET);
	oxu_writel(base, OXU_HOSTIFCONFIG, 0x0000037D);

	tmp = oxu_readl(base, OXU_PIOBURSTREADCTRL);
	oxu_writel(base, OXU_PIOBURSTREADCTRL, tmp | 0x0040);

	oxu_writel(base, OXU_ASO, OXU_SPHPOEN | OXU_OVRCCURPUPDEN |
					OXU_COMPARATOR | OXU_ASO_OP);

	tmp = oxu_readl(base, OXU_CLKCTRL_SET);
	oxu_writel(base, OXU_CLKCTRL_SET, tmp | OXU_SYSCLKEN | OXU_USBOTGCLKEN);

	/* Clear all top interrupt enable */
	oxu_writel(base, OXU_CHIPIRQEN_CLR, 0xff);

	/* Clear all top interrupt status */
	oxu_writel(base, OXU_CHIPIRQSTATUS, 0xff);

	/* Enable all needed top interrupt except OTG SPH core */
	oxu_writel(base, OXU_CHIPIRQEN_SET, OXU_USBSPHLPWUI | OXU_USBOTGLPWUI);
}

static int oxu_verify_id(struct platform_device *pdev, void __iomem *base)
{
	u32 id;
	static const char * const bo[] = {
		"reserved",
		"128-pin LQFP",
		"84-pin TFBGA",
		"reserved",
	};

	/* Read controller signature register to find a match */
	id = oxu_readl(base, OXU_DEVICEID);
	dev_info(&pdev->dev, "device ID %x\n", id);
	if ((id & OXU_REV_MASK) != (OXU_REV_2100 << OXU_REV_SHIFT))
		return -1;

	dev_info(&pdev->dev, "found device %x %s (%04x:%04x)\n",
		id >> OXU_REV_SHIFT,
		bo[(id & OXU_BO_MASK) >> OXU_BO_SHIFT],
		(id & OXU_MAJ_REV_MASK) >> OXU_MAJ_REV_SHIFT,
		(id & OXU_MIN_REV_MASK) >> OXU_MIN_REV_SHIFT);

	return 0;
}

static const struct hc_driver oxu_hc_driver;
static struct usb_hcd *oxu_create(struct platform_device *pdev,
				unsigned long memstart, unsigned long memlen,
				void __iomem *base, int irq, int otg)
{
	struct device *dev = &pdev->dev;

	struct usb_hcd *hcd;
	struct oxu_hcd *oxu;
	int ret;

	/* Set endian mode and host mode */
	oxu_writel(base + (otg ? OXU_OTG_CORE_OFFSET : OXU_SPH_CORE_OFFSET),
				OXU_USBMODE,
				OXU_CM_HOST_ONLY | OXU_ES_LITTLE | OXU_VBPS);

	hcd = usb_create_hcd(&oxu_hc_driver, dev,
				otg ? "oxu210hp_otg" : "oxu210hp_sph");
	if (!hcd)
		return ERR_PTR(-ENOMEM);

	hcd->rsrc_start = memstart;
	hcd->rsrc_len = memlen;
	hcd->regs = base;
	hcd->irq = irq;
	hcd->state = HC_STATE_HALT;

	oxu = hcd_to_oxu(hcd);
	oxu->is_otg = otg;

	ret = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (ret < 0) {
		usb_put_hcd(hcd);
		return ERR_PTR(ret);
	}

	device_wakeup_enable(hcd->self.controller);
	return hcd;
}

static int oxu_init(struct platform_device *pdev,
				unsigned long memstart, unsigned long memlen,
				void __iomem *base, int irq)
{
	struct oxu_info *info = platform_get_drvdata(pdev);
	struct usb_hcd *hcd;
	int ret;

	/* First time configuration at start up */
	oxu_configuration(pdev, base);

	ret = oxu_verify_id(pdev, base);
	if (ret) {
		dev_err(&pdev->dev, "no devices found!\n");
		return -ENODEV;
	}

	/* Create the OTG controller */
	hcd = oxu_create(pdev, memstart, memlen, base, irq, 1);
	if (IS_ERR(hcd)) {
		dev_err(&pdev->dev, "cannot create OTG controller!\n");
		ret = PTR_ERR(hcd);
		goto error_create_otg;
	}
	info->hcd[0] = hcd;

	/* Create the SPH host controller */
	hcd = oxu_create(pdev, memstart, memlen, base, irq, 0);
	if (IS_ERR(hcd)) {
		dev_err(&pdev->dev, "cannot create SPH controller!\n");
		ret = PTR_ERR(hcd);
		goto error_create_sph;
	}
	info->hcd[1] = hcd;

	oxu_writel(base, OXU_CHIPIRQEN_SET,
		oxu_readl(base, OXU_CHIPIRQEN_SET) | 3);

	return 0;

error_create_sph:
	usb_remove_hcd(info->hcd[0]);
	usb_put_hcd(info->hcd[0]);

error_create_otg:
	return ret;
}

static int oxu_drv_probe(struct platform_device *pdev)
{
	struct resource *res;
	void __iomem *base;
	unsigned long memstart, memlen;
	int irq, ret;
	struct oxu_info *info;

	if (usb_disabled())
		return -ENODEV;

	/*
	 * Get the platform resources
	 */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev,
			"no IRQ! Check %s setup!\n", dev_name(&pdev->dev));
		return -ENODEV;
	}
	irq = res->start;
	dev_dbg(&pdev->dev, "IRQ resource %d\n", irq);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(base)) {
		ret = PTR_ERR(base);
		goto error;
	}
	memstart = res->start;
	memlen = resource_size(res);

	ret = irq_set_irq_type(irq, IRQF_TRIGGER_FALLING);
	if (ret) {
		dev_err(&pdev->dev, "error setting irq type\n");
		ret = -EFAULT;
		goto error;
	}

	/* Allocate a driver data struct to hold useful info for both
	 * SPH & OTG devices
	 */
	info = devm_kzalloc(&pdev->dev, sizeof(struct oxu_info), GFP_KERNEL);
	if (!info) {
		ret = -EFAULT;
		goto error;
	}
	platform_set_drvdata(pdev, info);

	ret = oxu_init(pdev, memstart, memlen, base, irq);
	if (ret < 0) {
		dev_dbg(&pdev->dev, "cannot init USB devices\n");
		goto error;
	}

	dev_info(&pdev->dev, "devices enabled and running\n");
	platform_set_drvdata(pdev, info);

	return 0;

error:
	dev_err(&pdev->dev, "init %s fail, %d\n", dev_name(&pdev->dev), ret);
	return ret;
}

static void oxu_remove(struct platform_device *pdev, struct usb_hcd *hcd)
{
	usb_remove_hcd(hcd);
	usb_put_hcd(hcd);
}

static int oxu_drv_remove(struct platform_device *pdev)
{
	struct oxu_info *info = platform_get_drvdata(pdev);

	oxu_remove(pdev, info->hcd[0]);
	oxu_remove(pdev, info->hcd[1]);

	return 0;
}

static void oxu_drv_shutdown(struct platform_device *pdev)
{
	oxu_drv_remove(pdev);
}

#if 0
/* FIXME: TODO */
static int oxu_drv_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct usb_hcd *hcd = dev_get_drvdata(dev);

	return 0;
}

static int oxu_drv_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct usb_hcd *hcd = dev_get_drvdata(dev);

	return 0;
}
#else
#define oxu_drv_suspend	NULL
#define oxu_drv_resume	NULL
#endif

static struct platform_driver oxu_driver = {
	.probe		= oxu_drv_probe,
	.remove		= oxu_drv_remove,
	.shutdown	= oxu_drv_shutdown,
	.suspend	= oxu_drv_suspend,
	.resume		= oxu_drv_resume,
	.driver = {
		.name = "oxu210hp-hcd",
		.bus = &platform_bus_type
	}
};

module_platform_driver(oxu_driver);

MODULE_DESCRIPTION("Oxford OXU210HP HCD driver - ver. " DRIVER_VERSION);
MODULE_AUTHOR("Rodolfo Giometti <giometti@linux.it>");
MODULE_LICENSE("GPL");
