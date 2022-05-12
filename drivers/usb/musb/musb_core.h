/* SPDX-License-Identifier: GPL-2.0 */
/*
 * MUSB OTG driver defines
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 */

#ifndef __MUSB_CORE_H__
#define __MUSB_CORE_H__

#include <linux/slab.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb.h>
#include <linux/usb/otg.h>
#include <linux/usb/musb.h>
#include <linux/phy/phy.h>
#include <linux/workqueue.h>

struct musb;
struct musb_hw_ep;
struct musb_ep;
struct musb_qh;

/* Helper defines for struct musb->hwvers */
#define MUSB_HWVERS_MAJOR(x)	((x >> 10) & 0x1f)
#define MUSB_HWVERS_MINOR(x)	(x & 0x3ff)
#define MUSB_HWVERS_RC		0x8000
#define MUSB_HWVERS_1300	0x52C
#define MUSB_HWVERS_1400	0x590
#define MUSB_HWVERS_1800	0x720
#define MUSB_HWVERS_1900	0x784
#define MUSB_HWVERS_2000	0x800

#include "musb_debug.h"
#include "musb_dma.h"

#include "musb_io.h"

#include "musb_gadget.h"
#include <linux/usb/hcd.h>
#include "musb_host.h"

/* NOTE:  otg and peripheral-only state machines start at B_IDLE.
 * OTG or host-only go to A_IDLE when ID is sensed.
 */
#define is_peripheral_active(m)		(!(m)->is_host)
#define is_host_active(m)		((m)->is_host)

/****************************** CONSTANTS ********************************/

#ifndef MUSB_C_NUM_EPS
#define MUSB_C_NUM_EPS ((u8)16)
#endif

#ifndef MUSB_MAX_END0_PACKET
#define MUSB_MAX_END0_PACKET ((u16)MUSB_EP0_FIFOSIZE)
#endif

/* host side ep0 states */
enum musb_h_ep0_state {
	MUSB_EP0_IDLE,
	MUSB_EP0_START,			/* expect ack of setup */
	MUSB_EP0_IN,			/* expect IN DATA */
	MUSB_EP0_OUT,			/* expect ack of OUT DATA */
	MUSB_EP0_STATUS,		/* expect ack of STATUS */
} __attribute__ ((packed));

/* peripheral side ep0 states */
enum musb_g_ep0_state {
	MUSB_EP0_STAGE_IDLE,		/* idle, waiting for SETUP */
	MUSB_EP0_STAGE_SETUP,		/* received SETUP */
	MUSB_EP0_STAGE_TX,		/* IN data */
	MUSB_EP0_STAGE_RX,		/* OUT data */
	MUSB_EP0_STAGE_STATUSIN,	/* (after OUT data) */
	MUSB_EP0_STAGE_STATUSOUT,	/* (after IN data) */
	MUSB_EP0_STAGE_ACKWAIT,		/* after zlp, before statusin */
} __attribute__ ((packed));

/*
 * OTG protocol constants.  See USB OTG 1.3 spec,
 * sections 5.5 "Device Timings" and 6.6.5 "Timers".
 */
#define OTG_TIME_A_WAIT_VRISE	100		/* msec (max) */
#define OTG_TIME_A_WAIT_BCON	1100		/* min 1 second */
#define OTG_TIME_A_AIDL_BDIS	200		/* min 200 msec */
#define OTG_TIME_B_ASE0_BRST	100		/* min 3.125 ms */

/****************************** FUNCTIONS ********************************/

#define MUSB_HST_MODE(_musb)\
	{ (_musb)->is_host = true; }
#define MUSB_DEV_MODE(_musb) \
	{ (_musb)->is_host = false; }

#define test_devctl_hst_mode(_x) \
	(musb_readb((_x)->mregs, MUSB_DEVCTL)&MUSB_DEVCTL_HM)

#define MUSB_MODE(musb) ((musb)->is_host ? "Host" : "Peripheral")

/******************************** TYPES *************************************/

struct musb_io;

/**
 * struct musb_platform_ops - Operations passed to musb_core by HW glue layer
 * @quirks:	flags for platform specific quirks
 * @enable:	enable device
 * @disable:	disable device
 * @ep_offset:	returns the end point offset
 * @ep_select:	selects the specified end point
 * @fifo_mode:	sets the fifo mode
 * @fifo_offset: returns the fifo offset
 * @readb:	read 8 bits
 * @writeb:	write 8 bits
 * @clearb:	could be clear-on-readb or W1C
 * @readw:	read 16 bits
 * @writew:	write 16 bits
 * @clearw:	could be clear-on-readw or W1C
 * @read_fifo:	reads the fifo
 * @write_fifo:	writes to fifo
 * @get_toggle:	platform specific get toggle function
 * @set_toggle:	platform specific set toggle function
 * @dma_init:	platform specific dma init function
 * @dma_exit:	platform specific dma exit function
 * @init:	turns on clocks, sets up platform-specific registers, etc
 * @exit:	undoes @init
 * @set_mode:	forcefully changes operating mode
 * @try_idle:	tries to idle the IP
 * @recover:	platform-specific babble recovery
 * @vbus_status: returns vbus status if possible
 * @set_vbus:	forces vbus status
 * @pre_root_reset_end: called before the root usb port reset flag gets cleared
 * @post_root_reset_end: called after the root usb port reset flag gets cleared
 * @phy_callback: optional callback function for the phy to call
 */
struct musb_platform_ops {

#define MUSB_G_NO_SKB_RESERVE	BIT(9)
#define MUSB_DA8XX		BIT(8)
#define MUSB_PRESERVE_SESSION	BIT(7)
#define MUSB_DMA_UX500		BIT(6)
#define MUSB_DMA_CPPI41		BIT(5)
#define MUSB_DMA_CPPI		BIT(4)
#define MUSB_DMA_TUSB_OMAP	BIT(3)
#define MUSB_DMA_INVENTRA	BIT(2)
#define MUSB_IN_TUSB		BIT(1)
#define MUSB_INDEXED_EP		BIT(0)
	u32	quirks;

	int	(*init)(struct musb *musb);
	int	(*exit)(struct musb *musb);

	void	(*enable)(struct musb *musb);
	void	(*disable)(struct musb *musb);

	u32	(*ep_offset)(u8 epnum, u16 offset);
	void	(*ep_select)(void __iomem *mbase, u8 epnum);
	u16	fifo_mode;
	u32	(*fifo_offset)(u8 epnum);
	u32	(*busctl_offset)(u8 epnum, u16 offset);
	u8	(*readb)(void __iomem *addr, u32 offset);
	void	(*writeb)(void __iomem *addr, u32 offset, u8 data);
	u8	(*clearb)(void __iomem *addr, u32 offset);
	u16	(*readw)(void __iomem *addr, u32 offset);
	void	(*writew)(void __iomem *addr, u32 offset, u16 data);
	u16	(*clearw)(void __iomem *addr, u32 offset);
	void	(*read_fifo)(struct musb_hw_ep *hw_ep, u16 len, u8 *buf);
	void	(*write_fifo)(struct musb_hw_ep *hw_ep, u16 len, const u8 *buf);
	u16	(*get_toggle)(struct musb_qh *qh, int is_out);
	u16	(*set_toggle)(struct musb_qh *qh, int is_out, struct urb *urb);
	struct dma_controller *
		(*dma_init) (struct musb *musb, void __iomem *base);
	void	(*dma_exit)(struct dma_controller *c);
	int	(*set_mode)(struct musb *musb, u8 mode);
	void	(*try_idle)(struct musb *musb, unsigned long timeout);
	int	(*recover)(struct musb *musb);

	int	(*vbus_status)(struct musb *musb);
	void	(*set_vbus)(struct musb *musb, int on);

	void	(*pre_root_reset_end)(struct musb *musb);
	void	(*post_root_reset_end)(struct musb *musb);
	int	(*phy_callback)(enum musb_vbus_id_status status);
	void	(*clear_ep_rxintr)(struct musb *musb, int epnum);
};

/*
 * struct musb_hw_ep - endpoint hardware (bidirectional)
 *
 * Ordered slightly for better cacheline locality.
 */
struct musb_hw_ep {
	struct musb		*musb;
	void __iomem		*fifo;
	void __iomem		*regs;

#if IS_ENABLED(CONFIG_USB_MUSB_TUSB6010)
	void __iomem		*conf;
#endif

	/* index in musb->endpoints[]  */
	u8			epnum;

	/* hardware configuration, possibly dynamic */
	bool			is_shared_fifo;
	bool			tx_double_buffered;
	bool			rx_double_buffered;
	u16			max_packet_sz_tx;
	u16			max_packet_sz_rx;

	struct dma_channel	*tx_channel;
	struct dma_channel	*rx_channel;

#if IS_ENABLED(CONFIG_USB_MUSB_TUSB6010)
	/* TUSB has "asynchronous" and "synchronous" dma modes */
	dma_addr_t		fifo_async;
	dma_addr_t		fifo_sync;
	void __iomem		*fifo_sync_va;
#endif

	/* currently scheduled peripheral endpoint */
	struct musb_qh		*in_qh;
	struct musb_qh		*out_qh;

	u8			rx_reinit;
	u8			tx_reinit;

	/* peripheral side */
	struct musb_ep		ep_in;			/* TX */
	struct musb_ep		ep_out;			/* RX */
};

static inline struct musb_request *next_in_request(struct musb_hw_ep *hw_ep)
{
	return next_request(&hw_ep->ep_in);
}

static inline struct musb_request *next_out_request(struct musb_hw_ep *hw_ep)
{
	return next_request(&hw_ep->ep_out);
}

struct musb_csr_regs {
	/* FIFO registers */
	u16 txmaxp, txcsr, rxmaxp, rxcsr;
	u16 rxfifoadd, txfifoadd;
	u8 txtype, txinterval, rxtype, rxinterval;
	u8 rxfifosz, txfifosz;
	u8 txfunaddr, txhubaddr, txhubport;
	u8 rxfunaddr, rxhubaddr, rxhubport;
};

struct musb_context_registers {

	u8 power;
	u8 intrusbe;
	u16 frame;
	u8 index, testmode;

	u8 devctl, busctl, misc;
	u32 otg_interfsel;

	struct musb_csr_regs index_regs[MUSB_C_NUM_EPS];
};

/*
 * struct musb - Driver instance data.
 */
struct musb {
	/* device lock */
	spinlock_t		lock;
	spinlock_t		list_lock;	/* resume work list lock */

	struct musb_io		io;
	const struct musb_platform_ops *ops;
	struct musb_context_registers context;

	irqreturn_t		(*isr)(int, void *);
	struct delayed_work	irq_work;
	struct delayed_work	deassert_reset_work;
	struct delayed_work	finish_resume_work;
	struct delayed_work	gadget_work;
	u16			hwvers;

	u16			intrrxe;
	u16			intrtxe;
/* this hub status bit is reserved by USB 2.0 and not seen by usbcore */
#define MUSB_PORT_STAT_RESUME	(1 << 31)

	u32			port1_status;

	unsigned long		rh_timer;

	enum musb_h_ep0_state	ep0_stage;

	/* bulk traffic normally dedicates endpoint hardware, and each
	 * direction has its own ring of host side endpoints.
	 * we try to progress the transfer at the head of each endpoint's
	 * queue until it completes or NAKs too much; then we try the next
	 * endpoint.
	 */
	struct musb_hw_ep	*bulk_ep;

	struct list_head	control;	/* of musb_qh */
	struct list_head	in_bulk;	/* of musb_qh */
	struct list_head	out_bulk;	/* of musb_qh */
	struct list_head	pending_list;	/* pending work list */

	struct timer_list	otg_timer;
	struct timer_list	dev_timer;
	struct notifier_block	nb;

	struct dma_controller	*dma_controller;

	struct device		*controller;
	void __iomem		*ctrl_base;
	void __iomem		*mregs;

#if IS_ENABLED(CONFIG_USB_MUSB_TUSB6010)
	dma_addr_t		async;
	dma_addr_t		sync;
	void __iomem		*sync_va;
	u8			tusb_revision;
#endif

	/* passed down from chip/board specific irq handlers */
	u8			int_usb;
	u16			int_rx;
	u16			int_tx;

	struct usb_phy		*xceiv;
	struct phy		*phy;

	int nIrq;
	unsigned		irq_wake:1;

	struct musb_hw_ep	 endpoints[MUSB_C_NUM_EPS];
#define control_ep		endpoints

#define VBUSERR_RETRY_COUNT	3
	u16			vbuserr_retry;
	u16 epmask;
	u8 nr_endpoints;

	int			(*board_set_power)(int state);

	u8			min_power;	/* vbus for periph, in mA/2 */

	enum musb_mode		port_mode;
	bool			session;
	unsigned long		quirk_retries;
	bool			is_host;

	int			a_wait_bcon;	/* VBUS timeout in msecs */
	unsigned long		idle_timeout;	/* Next timeout in jiffies */

	unsigned		is_initialized:1;
	unsigned		is_runtime_suspended:1;

	/* active means connected and not suspended */
	unsigned		is_active:1;

	unsigned is_multipoint:1;

	unsigned		hb_iso_rx:1;	/* high bandwidth iso rx? */
	unsigned		hb_iso_tx:1;	/* high bandwidth iso tx? */
	unsigned		dyn_fifo:1;	/* dynamic FIFO supported? */

	unsigned		bulk_split:1;
#define	can_bulk_split(musb, type) \
	(((type) == USB_ENDPOINT_XFER_BULK) && (musb)->bulk_split)

	unsigned		bulk_combine:1;
#define	can_bulk_combine(musb, type) \
	(((type) == USB_ENDPOINT_XFER_BULK) && (musb)->bulk_combine)

	/* is_suspended means USB B_PERIPHERAL suspend */
	unsigned		is_suspended:1;

	/* may_wakeup means remote wakeup is enabled */
	unsigned		may_wakeup:1;

	/* is_self_powered is reported in device status and the
	 * config descriptor.  is_bus_powered means B_PERIPHERAL
	 * draws some VBUS current; both can be true.
	 */
	unsigned		is_self_powered:1;
	unsigned		is_bus_powered:1;

	unsigned		set_address:1;
	unsigned		test_mode:1;
	unsigned		softconnect:1;

	unsigned		flush_irq_work:1;

	u8			address;
	u8			test_mode_nr;
	u16			ackpend;		/* ep0 */
	enum musb_g_ep0_state	ep0_state;
	struct usb_gadget	g;			/* the gadget */
	struct usb_gadget_driver *gadget_driver;	/* its driver */
	struct usb_hcd		*hcd;			/* the usb hcd */

	const struct musb_hdrc_config *config;

	int			xceiv_old_state;
#ifdef CONFIG_DEBUG_FS
	struct dentry		*debugfs_root;
#endif
};

/* This must be included after struct musb is defined */
#include "musb_regs.h"

static inline struct musb *gadget_to_musb(struct usb_gadget *g)
{
	return container_of(g, struct musb, g);
}

static inline char *musb_ep_xfertype_string(u8 type)
{
	char *s;

	switch (type) {
	case USB_ENDPOINT_XFER_CONTROL:
		s = "ctrl";
		break;
	case USB_ENDPOINT_XFER_ISOC:
		s = "iso";
		break;
	case USB_ENDPOINT_XFER_BULK:
		s = "bulk";
		break;
	case USB_ENDPOINT_XFER_INT:
		s = "int";
		break;
	default:
		s = "";
		break;
	}
	return s;
}

static inline int musb_read_fifosize(struct musb *musb,
		struct musb_hw_ep *hw_ep, u8 epnum)
{
	void __iomem *mbase = musb->mregs;
	u8 reg = 0;

	/* read from core using indexed model */
	reg = musb_readb(mbase, musb->io.ep_offset(epnum, MUSB_FIFOSIZE));
	/* 0's returned when no more endpoints */
	if (!reg)
		return -ENODEV;

	musb->nr_endpoints++;
	musb->epmask |= (1 << epnum);

	hw_ep->max_packet_sz_tx = 1 << (reg & 0x0f);

	/* shared TX/RX FIFO? */
	if ((reg & 0xf0) == 0xf0) {
		hw_ep->max_packet_sz_rx = hw_ep->max_packet_sz_tx;
		hw_ep->is_shared_fifo = true;
		return 0;
	} else {
		hw_ep->max_packet_sz_rx = 1 << ((reg & 0xf0) >> 4);
		hw_ep->is_shared_fifo = false;
	}

	return 0;
}

static inline void musb_configure_ep0(struct musb *musb)
{
	musb->endpoints[0].max_packet_sz_tx = MUSB_EP0_FIFOSIZE;
	musb->endpoints[0].max_packet_sz_rx = MUSB_EP0_FIFOSIZE;
	musb->endpoints[0].is_shared_fifo = true;
}

/***************************** Glue it together *****************************/

extern const char musb_driver_name[];

extern void musb_stop(struct musb *musb);
extern void musb_start(struct musb *musb);

extern void musb_write_fifo(struct musb_hw_ep *ep, u16 len, const u8 *src);
extern void musb_read_fifo(struct musb_hw_ep *ep, u16 len, u8 *dst);

extern int musb_set_host(struct musb *musb);
extern int musb_set_peripheral(struct musb *musb);

extern void musb_load_testpacket(struct musb *);

extern irqreturn_t musb_interrupt(struct musb *);

extern void musb_hnp_stop(struct musb *musb);

int musb_queue_resume_work(struct musb *musb,
			   int (*callback)(struct musb *musb, void *data),
			   void *data);

static inline void musb_platform_set_vbus(struct musb *musb, int is_on)
{
	if (musb->ops->set_vbus)
		musb->ops->set_vbus(musb, is_on);
}

static inline void musb_platform_enable(struct musb *musb)
{
	if (musb->ops->enable)
		musb->ops->enable(musb);
}

static inline void musb_platform_disable(struct musb *musb)
{
	if (musb->ops->disable)
		musb->ops->disable(musb);
}

static inline int musb_platform_set_mode(struct musb *musb, u8 mode)
{
	if (!musb->ops->set_mode)
		return 0;

	return musb->ops->set_mode(musb, mode);
}

static inline void musb_platform_try_idle(struct musb *musb,
		unsigned long timeout)
{
	if (musb->ops->try_idle)
		musb->ops->try_idle(musb, timeout);
}

static inline int  musb_platform_recover(struct musb *musb)
{
	if (!musb->ops->recover)
		return 0;

	return musb->ops->recover(musb);
}

static inline int musb_platform_get_vbus_status(struct musb *musb)
{
	if (!musb->ops->vbus_status)
		return -EINVAL;

	return musb->ops->vbus_status(musb);
}

static inline int musb_platform_init(struct musb *musb)
{
	if (!musb->ops->init)
		return -EINVAL;

	return musb->ops->init(musb);
}

static inline int musb_platform_exit(struct musb *musb)
{
	if (!musb->ops->exit)
		return -EINVAL;

	return musb->ops->exit(musb);
}

static inline void musb_platform_pre_root_reset_end(struct musb *musb)
{
	if (musb->ops->pre_root_reset_end)
		musb->ops->pre_root_reset_end(musb);
}

static inline void musb_platform_post_root_reset_end(struct musb *musb)
{
	if (musb->ops->post_root_reset_end)
		musb->ops->post_root_reset_end(musb);
}

static inline void musb_platform_clear_ep_rxintr(struct musb *musb, int epnum)
{
	if (musb->ops->clear_ep_rxintr)
		musb->ops->clear_ep_rxintr(musb, epnum);
}

/*
 * gets the "dr_mode" property from DT and converts it into musb_mode
 * if the property is not found or not recognized returns MUSB_OTG
 */
extern enum musb_mode musb_get_mode(struct device *dev);

#endif	/* __MUSB_CORE_H__ */
