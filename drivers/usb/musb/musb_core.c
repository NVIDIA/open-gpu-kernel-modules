// SPDX-License-Identifier: GPL-2.0
/*
 * MUSB OTG driver core code
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 */

/*
 * Inventra (Multipoint) Dual-Role Controller Driver for Linux.
 *
 * This consists of a Host Controller Driver (HCD) and a peripheral
 * controller driver implementing the "Gadget" API; OTG support is
 * in the works.  These are normal Linux-USB controller drivers which
 * use IRQs and have no dedicated thread.
 *
 * This version of the driver has only been used with products from
 * Texas Instruments.  Those products integrate the Inventra logic
 * with other DMA, IRQ, and bus modules, as well as other logic that
 * needs to be reflected in this driver.
 *
 *
 * NOTE:  the original Mentor code here was pretty much a collection
 * of mechanisms that don't seem to have been fully integrated/working
 * for *any* Linux kernel version.  This version aims at Linux 2.6.now,
 * Key open issues include:
 *
 *  - Lack of host-side transaction scheduling, for all transfer types.
 *    The hardware doesn't do it; instead, software must.
 *
 *    This is not an issue for OTG devices that don't support external
 *    hubs, but for more "normal" USB hosts it's a user issue that the
 *    "multipoint" support doesn't scale in the expected ways.  That
 *    includes DaVinci EVM in a common non-OTG mode.
 *
 *      * Control and bulk use dedicated endpoints, and there's as
 *        yet no mechanism to either (a) reclaim the hardware when
 *        peripherals are NAKing, which gets complicated with bulk
 *        endpoints, or (b) use more than a single bulk endpoint in
 *        each direction.
 *
 *        RESULT:  one device may be perceived as blocking another one.
 *
 *      * Interrupt and isochronous will dynamically allocate endpoint
 *        hardware, but (a) there's no record keeping for bandwidth;
 *        (b) in the common case that few endpoints are available, there
 *        is no mechanism to reuse endpoints to talk to multiple devices.
 *
 *        RESULT:  At one extreme, bandwidth can be overcommitted in
 *        some hardware configurations, no faults will be reported.
 *        At the other extreme, the bandwidth capabilities which do
 *        exist tend to be severely undercommitted.  You can't yet hook
 *        up both a keyboard and a mouse to an external USB hub.
 */

/*
 * This gets many kinds of configuration information:
 *	- Kconfig for everything user-configurable
 *	- platform_device for addressing, irq, and platform_data
 *	- platform_data is mostly for board-specific information
 *	  (plus recentrly, SOC or family details)
 *
 * Most of the conditional compilation will (someday) vanish.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/kobject.h>
#include <linux/prefetch.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/dma-mapping.h>
#include <linux/usb.h>
#include <linux/usb/of.h>

#include "musb_core.h"
#include "musb_trace.h"

#define TA_WAIT_BCON(m) max_t(int, (m)->a_wait_bcon, OTG_TIME_A_WAIT_BCON)


#define DRIVER_AUTHOR "Mentor Graphics, Texas Instruments, Nokia"
#define DRIVER_DESC "Inventra Dual-Role USB Controller Driver"

#define MUSB_VERSION "6.0"

#define DRIVER_INFO DRIVER_DESC ", v" MUSB_VERSION

#define MUSB_DRIVER_NAME "musb-hdrc"
const char musb_driver_name[] = MUSB_DRIVER_NAME;

MODULE_DESCRIPTION(DRIVER_INFO);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" MUSB_DRIVER_NAME);


/*-------------------------------------------------------------------------*/

static inline struct musb *dev_to_musb(struct device *dev)
{
	return dev_get_drvdata(dev);
}

enum musb_mode musb_get_mode(struct device *dev)
{
	enum usb_dr_mode mode;

	mode = usb_get_dr_mode(dev);
	switch (mode) {
	case USB_DR_MODE_HOST:
		return MUSB_HOST;
	case USB_DR_MODE_PERIPHERAL:
		return MUSB_PERIPHERAL;
	case USB_DR_MODE_OTG:
	case USB_DR_MODE_UNKNOWN:
	default:
		return MUSB_OTG;
	}
}
EXPORT_SYMBOL_GPL(musb_get_mode);

/*-------------------------------------------------------------------------*/

static int musb_ulpi_read(struct usb_phy *phy, u32 reg)
{
	void __iomem *addr = phy->io_priv;
	int	i = 0;
	u8	r;
	u8	power;
	int	ret;

	pm_runtime_get_sync(phy->io_dev);

	/* Make sure the transceiver is not in low power mode */
	power = musb_readb(addr, MUSB_POWER);
	power &= ~MUSB_POWER_SUSPENDM;
	musb_writeb(addr, MUSB_POWER, power);

	/* REVISIT: musbhdrc_ulpi_an.pdf recommends setting the
	 * ULPICarKitControlDisableUTMI after clearing POWER_SUSPENDM.
	 */

	musb_writeb(addr, MUSB_ULPI_REG_ADDR, (u8)reg);
	musb_writeb(addr, MUSB_ULPI_REG_CONTROL,
			MUSB_ULPI_REG_REQ | MUSB_ULPI_RDN_WR);

	while (!(musb_readb(addr, MUSB_ULPI_REG_CONTROL)
				& MUSB_ULPI_REG_CMPLT)) {
		i++;
		if (i == 10000) {
			ret = -ETIMEDOUT;
			goto out;
		}

	}
	r = musb_readb(addr, MUSB_ULPI_REG_CONTROL);
	r &= ~MUSB_ULPI_REG_CMPLT;
	musb_writeb(addr, MUSB_ULPI_REG_CONTROL, r);

	ret = musb_readb(addr, MUSB_ULPI_REG_DATA);

out:
	pm_runtime_put(phy->io_dev);

	return ret;
}

static int musb_ulpi_write(struct usb_phy *phy, u32 val, u32 reg)
{
	void __iomem *addr = phy->io_priv;
	int	i = 0;
	u8	r = 0;
	u8	power;
	int	ret = 0;

	pm_runtime_get_sync(phy->io_dev);

	/* Make sure the transceiver is not in low power mode */
	power = musb_readb(addr, MUSB_POWER);
	power &= ~MUSB_POWER_SUSPENDM;
	musb_writeb(addr, MUSB_POWER, power);

	musb_writeb(addr, MUSB_ULPI_REG_ADDR, (u8)reg);
	musb_writeb(addr, MUSB_ULPI_REG_DATA, (u8)val);
	musb_writeb(addr, MUSB_ULPI_REG_CONTROL, MUSB_ULPI_REG_REQ);

	while (!(musb_readb(addr, MUSB_ULPI_REG_CONTROL)
				& MUSB_ULPI_REG_CMPLT)) {
		i++;
		if (i == 10000) {
			ret = -ETIMEDOUT;
			goto out;
		}
	}

	r = musb_readb(addr, MUSB_ULPI_REG_CONTROL);
	r &= ~MUSB_ULPI_REG_CMPLT;
	musb_writeb(addr, MUSB_ULPI_REG_CONTROL, r);

out:
	pm_runtime_put(phy->io_dev);

	return ret;
}

static struct usb_phy_io_ops musb_ulpi_access = {
	.read = musb_ulpi_read,
	.write = musb_ulpi_write,
};

/*-------------------------------------------------------------------------*/

static u32 musb_default_fifo_offset(u8 epnum)
{
	return 0x20 + (epnum * 4);
}

/* "flat" mapping: each endpoint has its own i/o address */
static void musb_flat_ep_select(void __iomem *mbase, u8 epnum)
{
}

static u32 musb_flat_ep_offset(u8 epnum, u16 offset)
{
	return 0x100 + (0x10 * epnum) + offset;
}

/* "indexed" mapping: INDEX register controls register bank select */
static void musb_indexed_ep_select(void __iomem *mbase, u8 epnum)
{
	musb_writeb(mbase, MUSB_INDEX, epnum);
}

static u32 musb_indexed_ep_offset(u8 epnum, u16 offset)
{
	return 0x10 + offset;
}

static u32 musb_default_busctl_offset(u8 epnum, u16 offset)
{
	return 0x80 + (0x08 * epnum) + offset;
}

static u8 musb_default_readb(void __iomem *addr, u32 offset)
{
	u8 data =  __raw_readb(addr + offset);

	trace_musb_readb(__builtin_return_address(0), addr, offset, data);
	return data;
}

static void musb_default_writeb(void __iomem *addr, u32 offset, u8 data)
{
	trace_musb_writeb(__builtin_return_address(0), addr, offset, data);
	__raw_writeb(data, addr + offset);
}

static u16 musb_default_readw(void __iomem *addr, u32 offset)
{
	u16 data = __raw_readw(addr + offset);

	trace_musb_readw(__builtin_return_address(0), addr, offset, data);
	return data;
}

static void musb_default_writew(void __iomem *addr, u32 offset, u16 data)
{
	trace_musb_writew(__builtin_return_address(0), addr, offset, data);
	__raw_writew(data, addr + offset);
}

static u16 musb_default_get_toggle(struct musb_qh *qh, int is_out)
{
	void __iomem *epio = qh->hw_ep->regs;
	u16 csr;

	if (is_out)
		csr = musb_readw(epio, MUSB_TXCSR) & MUSB_TXCSR_H_DATATOGGLE;
	else
		csr = musb_readw(epio, MUSB_RXCSR) & MUSB_RXCSR_H_DATATOGGLE;

	return csr;
}

static u16 musb_default_set_toggle(struct musb_qh *qh, int is_out,
				   struct urb *urb)
{
	u16 csr;
	u16 toggle;

	toggle = usb_gettoggle(urb->dev, qh->epnum, is_out);

	if (is_out)
		csr = toggle ? (MUSB_TXCSR_H_WR_DATATOGGLE
				| MUSB_TXCSR_H_DATATOGGLE)
				: MUSB_TXCSR_CLRDATATOG;
	else
		csr = toggle ? (MUSB_RXCSR_H_WR_DATATOGGLE
				| MUSB_RXCSR_H_DATATOGGLE) : 0;

	return csr;
}

/*
 * Load an endpoint's FIFO
 */
static void musb_default_write_fifo(struct musb_hw_ep *hw_ep, u16 len,
				    const u8 *src)
{
	struct musb *musb = hw_ep->musb;
	void __iomem *fifo = hw_ep->fifo;

	if (unlikely(len == 0))
		return;

	prefetch((u8 *)src);

	dev_dbg(musb->controller, "%cX ep%d fifo %p count %d buf %p\n",
			'T', hw_ep->epnum, fifo, len, src);

	/* we can't assume unaligned reads work */
	if (likely((0x01 & (unsigned long) src) == 0)) {
		u16	index = 0;

		/* best case is 32bit-aligned source address */
		if ((0x02 & (unsigned long) src) == 0) {
			if (len >= 4) {
				iowrite32_rep(fifo, src + index, len >> 2);
				index += len & ~0x03;
			}
			if (len & 0x02) {
				__raw_writew(*(u16 *)&src[index], fifo);
				index += 2;
			}
		} else {
			if (len >= 2) {
				iowrite16_rep(fifo, src + index, len >> 1);
				index += len & ~0x01;
			}
		}
		if (len & 0x01)
			__raw_writeb(src[index], fifo);
	} else  {
		/* byte aligned */
		iowrite8_rep(fifo, src, len);
	}
}

/*
 * Unload an endpoint's FIFO
 */
static void musb_default_read_fifo(struct musb_hw_ep *hw_ep, u16 len, u8 *dst)
{
	struct musb *musb = hw_ep->musb;
	void __iomem *fifo = hw_ep->fifo;

	if (unlikely(len == 0))
		return;

	dev_dbg(musb->controller, "%cX ep%d fifo %p count %d buf %p\n",
			'R', hw_ep->epnum, fifo, len, dst);

	/* we can't assume unaligned writes work */
	if (likely((0x01 & (unsigned long) dst) == 0)) {
		u16	index = 0;

		/* best case is 32bit-aligned destination address */
		if ((0x02 & (unsigned long) dst) == 0) {
			if (len >= 4) {
				ioread32_rep(fifo, dst, len >> 2);
				index = len & ~0x03;
			}
			if (len & 0x02) {
				*(u16 *)&dst[index] = __raw_readw(fifo);
				index += 2;
			}
		} else {
			if (len >= 2) {
				ioread16_rep(fifo, dst, len >> 1);
				index = len & ~0x01;
			}
		}
		if (len & 0x01)
			dst[index] = __raw_readb(fifo);
	} else  {
		/* byte aligned */
		ioread8_rep(fifo, dst, len);
	}
}

/*
 * Old style IO functions
 */
u8 (*musb_readb)(void __iomem *addr, u32 offset);
EXPORT_SYMBOL_GPL(musb_readb);

void (*musb_writeb)(void __iomem *addr, u32 offset, u8 data);
EXPORT_SYMBOL_GPL(musb_writeb);

u8 (*musb_clearb)(void __iomem *addr, u32 offset);
EXPORT_SYMBOL_GPL(musb_clearb);

u16 (*musb_readw)(void __iomem *addr, u32 offset);
EXPORT_SYMBOL_GPL(musb_readw);

void (*musb_writew)(void __iomem *addr, u32 offset, u16 data);
EXPORT_SYMBOL_GPL(musb_writew);

u16 (*musb_clearw)(void __iomem *addr, u32 offset);
EXPORT_SYMBOL_GPL(musb_clearw);

u32 musb_readl(void __iomem *addr, u32 offset)
{
	u32 data = __raw_readl(addr + offset);

	trace_musb_readl(__builtin_return_address(0), addr, offset, data);
	return data;
}
EXPORT_SYMBOL_GPL(musb_readl);

void musb_writel(void __iomem *addr, u32 offset, u32 data)
{
	trace_musb_writel(__builtin_return_address(0), addr, offset, data);
	__raw_writel(data, addr + offset);
}
EXPORT_SYMBOL_GPL(musb_writel);

#ifndef CONFIG_MUSB_PIO_ONLY
struct dma_controller *
(*musb_dma_controller_create)(struct musb *musb, void __iomem *base);
EXPORT_SYMBOL(musb_dma_controller_create);

void (*musb_dma_controller_destroy)(struct dma_controller *c);
EXPORT_SYMBOL(musb_dma_controller_destroy);
#endif

/*
 * New style IO functions
 */
void musb_read_fifo(struct musb_hw_ep *hw_ep, u16 len, u8 *dst)
{
	return hw_ep->musb->io.read_fifo(hw_ep, len, dst);
}

void musb_write_fifo(struct musb_hw_ep *hw_ep, u16 len, const u8 *src)
{
	return hw_ep->musb->io.write_fifo(hw_ep, len, src);
}

static u8 musb_read_devctl(struct musb *musb)
{
	return musb_readb(musb->mregs, MUSB_DEVCTL);
}

/**
 * musb_set_host - set and initialize host mode
 * @musb: musb controller driver data
 *
 * At least some musb revisions need to enable devctl session bit in
 * peripheral mode to switch to host mode. Initializes things to host
 * mode and sets A_IDLE. SoC glue needs to advance state further
 * based on phy provided VBUS state.
 *
 * Note that the SoC glue code may need to wait for musb to settle
 * on enable before calling this to avoid babble.
 */
int musb_set_host(struct musb *musb)
{
	int error = 0;
	u8 devctl;

	if (!musb)
		return -EINVAL;

	devctl = musb_read_devctl(musb);
	if (!(devctl & MUSB_DEVCTL_BDEVICE)) {
		dev_info(musb->controller,
			 "%s: already in host mode: %02x\n",
			 __func__, devctl);
		goto init_data;
	}

	devctl |= MUSB_DEVCTL_SESSION;
	musb_writeb(musb->mregs, MUSB_DEVCTL, devctl);

	error = readx_poll_timeout(musb_read_devctl, musb, devctl,
				   !(devctl & MUSB_DEVCTL_BDEVICE), 5000,
				   1000000);
	if (error) {
		dev_err(musb->controller, "%s: could not set host: %02x\n",
			__func__, devctl);

		return error;
	}

init_data:
	musb->is_active = 1;
	musb->xceiv->otg->state = OTG_STATE_A_IDLE;
	MUSB_HST_MODE(musb);

	return error;
}
EXPORT_SYMBOL_GPL(musb_set_host);

/**
 * musb_set_peripheral - set and initialize peripheral mode
 * @musb: musb controller driver data
 *
 * Clears devctl session bit and initializes things for peripheral
 * mode and sets B_IDLE. SoC glue needs to advance state further
 * based on phy provided VBUS state.
 */
int musb_set_peripheral(struct musb *musb)
{
	int error = 0;
	u8 devctl;

	if (!musb)
		return -EINVAL;

	devctl = musb_read_devctl(musb);
	if (devctl & MUSB_DEVCTL_BDEVICE) {
		dev_info(musb->controller,
			 "%s: already in peripheral mode: %02x\n",
			 __func__, devctl);

		goto init_data;
	}

	devctl &= ~MUSB_DEVCTL_SESSION;
	musb_writeb(musb->mregs, MUSB_DEVCTL, devctl);

	error = readx_poll_timeout(musb_read_devctl, musb, devctl,
				   devctl & MUSB_DEVCTL_BDEVICE, 5000,
				   1000000);
	if (error) {
		dev_err(musb->controller, "%s: could not set peripheral: %02x\n",
			__func__, devctl);

		return error;
	}

init_data:
	musb->is_active = 0;
	musb->xceiv->otg->state = OTG_STATE_B_IDLE;
	MUSB_DEV_MODE(musb);

	return error;
}
EXPORT_SYMBOL_GPL(musb_set_peripheral);

/*-------------------------------------------------------------------------*/

/* for high speed test mode; see USB 2.0 spec 7.1.20 */
static const u8 musb_test_packet[53] = {
	/* implicit SYNC then DATA0 to start */

	/* JKJKJKJK x9 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* JJKKJJKK x8 */
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	/* JJJJKKKK x8 */
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
	/* JJJJJJJKKKKKKK x8 */
	0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	/* JJJJJJJK x8 */
	0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd,
	/* JKKKKKKK x10, JK */
	0xfc, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e

	/* implicit CRC16 then EOP to end */
};

void musb_load_testpacket(struct musb *musb)
{
	void __iomem	*regs = musb->endpoints[0].regs;

	musb_ep_select(musb->mregs, 0);
	musb_write_fifo(musb->control_ep,
			sizeof(musb_test_packet), musb_test_packet);
	musb_writew(regs, MUSB_CSR0, MUSB_CSR0_TXPKTRDY);
}

/*-------------------------------------------------------------------------*/

/*
 * Handles OTG hnp timeouts, such as b_ase0_brst
 */
static void musb_otg_timer_func(struct timer_list *t)
{
	struct musb	*musb = from_timer(musb, t, otg_timer);
	unsigned long	flags;

	spin_lock_irqsave(&musb->lock, flags);
	switch (musb->xceiv->otg->state) {
	case OTG_STATE_B_WAIT_ACON:
		musb_dbg(musb,
			"HNP: b_wait_acon timeout; back to b_peripheral");
		musb_g_disconnect(musb);
		musb->xceiv->otg->state = OTG_STATE_B_PERIPHERAL;
		musb->is_active = 0;
		break;
	case OTG_STATE_A_SUSPEND:
	case OTG_STATE_A_WAIT_BCON:
		musb_dbg(musb, "HNP: %s timeout",
			usb_otg_state_string(musb->xceiv->otg->state));
		musb_platform_set_vbus(musb, 0);
		musb->xceiv->otg->state = OTG_STATE_A_WAIT_VFALL;
		break;
	default:
		musb_dbg(musb, "HNP: Unhandled mode %s",
			usb_otg_state_string(musb->xceiv->otg->state));
	}
	spin_unlock_irqrestore(&musb->lock, flags);
}

/*
 * Stops the HNP transition. Caller must take care of locking.
 */
void musb_hnp_stop(struct musb *musb)
{
	struct usb_hcd	*hcd = musb->hcd;
	void __iomem	*mbase = musb->mregs;
	u8	reg;

	musb_dbg(musb, "HNP: stop from %s",
			usb_otg_state_string(musb->xceiv->otg->state));

	switch (musb->xceiv->otg->state) {
	case OTG_STATE_A_PERIPHERAL:
		musb_g_disconnect(musb);
		musb_dbg(musb, "HNP: back to %s",
			usb_otg_state_string(musb->xceiv->otg->state));
		break;
	case OTG_STATE_B_HOST:
		musb_dbg(musb, "HNP: Disabling HR");
		if (hcd)
			hcd->self.is_b_host = 0;
		musb->xceiv->otg->state = OTG_STATE_B_PERIPHERAL;
		MUSB_DEV_MODE(musb);
		reg = musb_readb(mbase, MUSB_POWER);
		reg |= MUSB_POWER_SUSPENDM;
		musb_writeb(mbase, MUSB_POWER, reg);
		/* REVISIT: Start SESSION_REQUEST here? */
		break;
	default:
		musb_dbg(musb, "HNP: Stopping in unknown state %s",
			usb_otg_state_string(musb->xceiv->otg->state));
	}

	/*
	 * When returning to A state after HNP, avoid hub_port_rebounce(),
	 * which cause occasional OPT A "Did not receive reset after connect"
	 * errors.
	 */
	musb->port1_status &= ~(USB_PORT_STAT_C_CONNECTION << 16);
}

static void musb_recover_from_babble(struct musb *musb);

static void musb_handle_intr_resume(struct musb *musb, u8 devctl)
{
	musb_dbg(musb, "RESUME (%s)",
			usb_otg_state_string(musb->xceiv->otg->state));

	if (devctl & MUSB_DEVCTL_HM) {
		switch (musb->xceiv->otg->state) {
		case OTG_STATE_A_SUSPEND:
			/* remote wakeup? */
			musb->port1_status |=
					(USB_PORT_STAT_C_SUSPEND << 16)
					| MUSB_PORT_STAT_RESUME;
			musb->rh_timer = jiffies
				+ msecs_to_jiffies(USB_RESUME_TIMEOUT);
			musb->xceiv->otg->state = OTG_STATE_A_HOST;
			musb->is_active = 1;
			musb_host_resume_root_hub(musb);
			schedule_delayed_work(&musb->finish_resume_work,
				msecs_to_jiffies(USB_RESUME_TIMEOUT));
			break;
		case OTG_STATE_B_WAIT_ACON:
			musb->xceiv->otg->state = OTG_STATE_B_PERIPHERAL;
			musb->is_active = 1;
			MUSB_DEV_MODE(musb);
			break;
		default:
			WARNING("bogus %s RESUME (%s)\n",
				"host",
				usb_otg_state_string(musb->xceiv->otg->state));
		}
	} else {
		switch (musb->xceiv->otg->state) {
		case OTG_STATE_A_SUSPEND:
			/* possibly DISCONNECT is upcoming */
			musb->xceiv->otg->state = OTG_STATE_A_HOST;
			musb_host_resume_root_hub(musb);
			break;
		case OTG_STATE_B_WAIT_ACON:
		case OTG_STATE_B_PERIPHERAL:
			/* disconnect while suspended?  we may
			 * not get a disconnect irq...
			 */
			if ((devctl & MUSB_DEVCTL_VBUS)
					!= (3 << MUSB_DEVCTL_VBUS_SHIFT)
					) {
				musb->int_usb |= MUSB_INTR_DISCONNECT;
				musb->int_usb &= ~MUSB_INTR_SUSPEND;
				break;
			}
			musb_g_resume(musb);
			break;
		case OTG_STATE_B_IDLE:
			musb->int_usb &= ~MUSB_INTR_SUSPEND;
			break;
		default:
			WARNING("bogus %s RESUME (%s)\n",
				"peripheral",
				usb_otg_state_string(musb->xceiv->otg->state));
		}
	}
}

/* return IRQ_HANDLED to tell the caller to return immediately */
static irqreturn_t musb_handle_intr_sessreq(struct musb *musb, u8 devctl)
{
	void __iomem *mbase = musb->mregs;

	if ((devctl & MUSB_DEVCTL_VBUS) == MUSB_DEVCTL_VBUS
			&& (devctl & MUSB_DEVCTL_BDEVICE)) {
		musb_dbg(musb, "SessReq while on B state");
		return IRQ_HANDLED;
	}

	musb_dbg(musb, "SESSION_REQUEST (%s)",
		usb_otg_state_string(musb->xceiv->otg->state));

	/* IRQ arrives from ID pin sense or (later, if VBUS power
	 * is removed) SRP.  responses are time critical:
	 *  - turn on VBUS (with silicon-specific mechanism)
	 *  - go through A_WAIT_VRISE
	 *  - ... to A_WAIT_BCON.
	 * a_wait_vrise_tmout triggers VBUS_ERROR transitions
	 */
	musb_writeb(mbase, MUSB_DEVCTL, MUSB_DEVCTL_SESSION);
	musb->ep0_stage = MUSB_EP0_START;
	musb->xceiv->otg->state = OTG_STATE_A_IDLE;
	MUSB_HST_MODE(musb);
	musb_platform_set_vbus(musb, 1);

	return IRQ_NONE;
}

static void musb_handle_intr_vbuserr(struct musb *musb, u8 devctl)
{
	int	ignore = 0;

	/* During connection as an A-Device, we may see a short
	 * current spikes causing voltage drop, because of cable
	 * and peripheral capacitance combined with vbus draw.
	 * (So: less common with truly self-powered devices, where
	 * vbus doesn't act like a power supply.)
	 *
	 * Such spikes are short; usually less than ~500 usec, max
	 * of ~2 msec.  That is, they're not sustained overcurrent
	 * errors, though they're reported using VBUSERROR irqs.
	 *
	 * Workarounds:  (a) hardware: use self powered devices.
	 * (b) software:  ignore non-repeated VBUS errors.
	 *
	 * REVISIT:  do delays from lots of DEBUG_KERNEL checks
	 * make trouble here, keeping VBUS < 4.4V ?
	 */
	switch (musb->xceiv->otg->state) {
	case OTG_STATE_A_HOST:
		/* recovery is dicey once we've gotten past the
		 * initial stages of enumeration, but if VBUS
		 * stayed ok at the other end of the link, and
		 * another reset is due (at least for high speed,
		 * to redo the chirp etc), it might work OK...
		 */
	case OTG_STATE_A_WAIT_BCON:
	case OTG_STATE_A_WAIT_VRISE:
		if (musb->vbuserr_retry) {
			void __iomem *mbase = musb->mregs;

			musb->vbuserr_retry--;
			ignore = 1;
			devctl |= MUSB_DEVCTL_SESSION;
			musb_writeb(mbase, MUSB_DEVCTL, devctl);
		} else {
			musb->port1_status |=
				  USB_PORT_STAT_OVERCURRENT
				| (USB_PORT_STAT_C_OVERCURRENT << 16);
		}
		break;
	default:
		break;
	}

	dev_printk(ignore ? KERN_DEBUG : KERN_ERR, musb->controller,
			"VBUS_ERROR in %s (%02x, %s), retry #%d, port1 %08x\n",
			usb_otg_state_string(musb->xceiv->otg->state),
			devctl,
			({ char *s;
			switch (devctl & MUSB_DEVCTL_VBUS) {
			case 0 << MUSB_DEVCTL_VBUS_SHIFT:
				s = "<SessEnd"; break;
			case 1 << MUSB_DEVCTL_VBUS_SHIFT:
				s = "<AValid"; break;
			case 2 << MUSB_DEVCTL_VBUS_SHIFT:
				s = "<VBusValid"; break;
			/* case 3 << MUSB_DEVCTL_VBUS_SHIFT: */
			default:
				s = "VALID"; break;
			} s; }),
			VBUSERR_RETRY_COUNT - musb->vbuserr_retry,
			musb->port1_status);

	/* go through A_WAIT_VFALL then start a new session */
	if (!ignore)
		musb_platform_set_vbus(musb, 0);
}

static void musb_handle_intr_suspend(struct musb *musb, u8 devctl)
{
	musb_dbg(musb, "SUSPEND (%s) devctl %02x",
		usb_otg_state_string(musb->xceiv->otg->state), devctl);

	switch (musb->xceiv->otg->state) {
	case OTG_STATE_A_PERIPHERAL:
		/* We also come here if the cable is removed, since
		 * this silicon doesn't report ID-no-longer-grounded.
		 *
		 * We depend on T(a_wait_bcon) to shut us down, and
		 * hope users don't do anything dicey during this
		 * undesired detour through A_WAIT_BCON.
		 */
		musb_hnp_stop(musb);
		musb_host_resume_root_hub(musb);
		musb_root_disconnect(musb);
		musb_platform_try_idle(musb, jiffies
				+ msecs_to_jiffies(musb->a_wait_bcon
					? : OTG_TIME_A_WAIT_BCON));

		break;
	case OTG_STATE_B_IDLE:
		if (!musb->is_active)
			break;
		fallthrough;
	case OTG_STATE_B_PERIPHERAL:
		musb_g_suspend(musb);
		musb->is_active = musb->g.b_hnp_enable;
		if (musb->is_active) {
			musb->xceiv->otg->state = OTG_STATE_B_WAIT_ACON;
			musb_dbg(musb, "HNP: Setting timer for b_ase0_brst");
			mod_timer(&musb->otg_timer, jiffies
				+ msecs_to_jiffies(
						OTG_TIME_B_ASE0_BRST));
		}
		break;
	case OTG_STATE_A_WAIT_BCON:
		if (musb->a_wait_bcon != 0)
			musb_platform_try_idle(musb, jiffies
				+ msecs_to_jiffies(musb->a_wait_bcon));
		break;
	case OTG_STATE_A_HOST:
		musb->xceiv->otg->state = OTG_STATE_A_SUSPEND;
		musb->is_active = musb->hcd->self.b_hnp_enable;
		break;
	case OTG_STATE_B_HOST:
		/* Transition to B_PERIPHERAL, see 6.8.2.6 p 44 */
		musb_dbg(musb, "REVISIT: SUSPEND as B_HOST");
		break;
	default:
		/* "should not happen" */
		musb->is_active = 0;
		break;
	}
}

static void musb_handle_intr_connect(struct musb *musb, u8 devctl, u8 int_usb)
{
	struct usb_hcd *hcd = musb->hcd;

	musb->is_active = 1;
	musb->ep0_stage = MUSB_EP0_START;

	musb->intrtxe = musb->epmask;
	musb_writew(musb->mregs, MUSB_INTRTXE, musb->intrtxe);
	musb->intrrxe = musb->epmask & 0xfffe;
	musb_writew(musb->mregs, MUSB_INTRRXE, musb->intrrxe);
	musb_writeb(musb->mregs, MUSB_INTRUSBE, 0xf7);
	musb->port1_status &= ~(USB_PORT_STAT_LOW_SPEED
				|USB_PORT_STAT_HIGH_SPEED
				|USB_PORT_STAT_ENABLE
				);
	musb->port1_status |= USB_PORT_STAT_CONNECTION
				|(USB_PORT_STAT_C_CONNECTION << 16);

	/* high vs full speed is just a guess until after reset */
	if (devctl & MUSB_DEVCTL_LSDEV)
		musb->port1_status |= USB_PORT_STAT_LOW_SPEED;

	/* indicate new connection to OTG machine */
	switch (musb->xceiv->otg->state) {
	case OTG_STATE_B_PERIPHERAL:
		if (int_usb & MUSB_INTR_SUSPEND) {
			musb_dbg(musb, "HNP: SUSPEND+CONNECT, now b_host");
			int_usb &= ~MUSB_INTR_SUSPEND;
			goto b_host;
		} else
			musb_dbg(musb, "CONNECT as b_peripheral???");
		break;
	case OTG_STATE_B_WAIT_ACON:
		musb_dbg(musb, "HNP: CONNECT, now b_host");
b_host:
		musb->xceiv->otg->state = OTG_STATE_B_HOST;
		if (musb->hcd)
			musb->hcd->self.is_b_host = 1;
		del_timer(&musb->otg_timer);
		break;
	default:
		if ((devctl & MUSB_DEVCTL_VBUS)
				== (3 << MUSB_DEVCTL_VBUS_SHIFT)) {
			musb->xceiv->otg->state = OTG_STATE_A_HOST;
			if (hcd)
				hcd->self.is_b_host = 0;
		}
		break;
	}

	musb_host_poke_root_hub(musb);

	musb_dbg(musb, "CONNECT (%s) devctl %02x",
			usb_otg_state_string(musb->xceiv->otg->state), devctl);
}

static void musb_handle_intr_disconnect(struct musb *musb, u8 devctl)
{
	musb_dbg(musb, "DISCONNECT (%s) as %s, devctl %02x",
			usb_otg_state_string(musb->xceiv->otg->state),
			MUSB_MODE(musb), devctl);

	switch (musb->xceiv->otg->state) {
	case OTG_STATE_A_HOST:
	case OTG_STATE_A_SUSPEND:
		musb_host_resume_root_hub(musb);
		musb_root_disconnect(musb);
		if (musb->a_wait_bcon != 0)
			musb_platform_try_idle(musb, jiffies
				+ msecs_to_jiffies(musb->a_wait_bcon));
		break;
	case OTG_STATE_B_HOST:
		/* REVISIT this behaves for "real disconnect"
		 * cases; make sure the other transitions from
		 * from B_HOST act right too.  The B_HOST code
		 * in hnp_stop() is currently not used...
		 */
		musb_root_disconnect(musb);
		if (musb->hcd)
			musb->hcd->self.is_b_host = 0;
		musb->xceiv->otg->state = OTG_STATE_B_PERIPHERAL;
		MUSB_DEV_MODE(musb);
		musb_g_disconnect(musb);
		break;
	case OTG_STATE_A_PERIPHERAL:
		musb_hnp_stop(musb);
		musb_root_disconnect(musb);
		fallthrough;
	case OTG_STATE_B_WAIT_ACON:
	case OTG_STATE_B_PERIPHERAL:
	case OTG_STATE_B_IDLE:
		musb_g_disconnect(musb);
		break;
	default:
		WARNING("unhandled DISCONNECT transition (%s)\n",
			usb_otg_state_string(musb->xceiv->otg->state));
		break;
	}
}

/*
 * mentor saves a bit: bus reset and babble share the same irq.
 * only host sees babble; only peripheral sees bus reset.
 */
static void musb_handle_intr_reset(struct musb *musb)
{
	if (is_host_active(musb)) {
		/*
		 * When BABBLE happens what we can depends on which
		 * platform MUSB is running, because some platforms
		 * implemented proprietary means for 'recovering' from
		 * Babble conditions. One such platform is AM335x. In
		 * most cases, however, the only thing we can do is
		 * drop the session.
		 */
		dev_err(musb->controller, "Babble\n");
		musb_recover_from_babble(musb);
	} else {
		musb_dbg(musb, "BUS RESET as %s",
			usb_otg_state_string(musb->xceiv->otg->state));
		switch (musb->xceiv->otg->state) {
		case OTG_STATE_A_SUSPEND:
			musb_g_reset(musb);
			fallthrough;
		case OTG_STATE_A_WAIT_BCON:	/* OPT TD.4.7-900ms */
			/* never use invalid T(a_wait_bcon) */
			musb_dbg(musb, "HNP: in %s, %d msec timeout",
				usb_otg_state_string(musb->xceiv->otg->state),
				TA_WAIT_BCON(musb));
			mod_timer(&musb->otg_timer, jiffies
				+ msecs_to_jiffies(TA_WAIT_BCON(musb)));
			break;
		case OTG_STATE_A_PERIPHERAL:
			del_timer(&musb->otg_timer);
			musb_g_reset(musb);
			break;
		case OTG_STATE_B_WAIT_ACON:
			musb_dbg(musb, "HNP: RESET (%s), to b_peripheral",
				usb_otg_state_string(musb->xceiv->otg->state));
			musb->xceiv->otg->state = OTG_STATE_B_PERIPHERAL;
			musb_g_reset(musb);
			break;
		case OTG_STATE_B_IDLE:
			musb->xceiv->otg->state = OTG_STATE_B_PERIPHERAL;
			fallthrough;
		case OTG_STATE_B_PERIPHERAL:
			musb_g_reset(musb);
			break;
		default:
			musb_dbg(musb, "Unhandled BUS RESET as %s",
				usb_otg_state_string(musb->xceiv->otg->state));
		}
	}
}

/*
 * Interrupt Service Routine to record USB "global" interrupts.
 * Since these do not happen often and signify things of
 * paramount importance, it seems OK to check them individually;
 * the order of the tests is specified in the manual
 *
 * @param musb instance pointer
 * @param int_usb register contents
 * @param devctl
 */

static irqreturn_t musb_stage0_irq(struct musb *musb, u8 int_usb,
				u8 devctl)
{
	irqreturn_t handled = IRQ_NONE;

	musb_dbg(musb, "<== DevCtl=%02x, int_usb=0x%x", devctl, int_usb);

	/* in host mode, the peripheral may issue remote wakeup.
	 * in peripheral mode, the host may resume the link.
	 * spurious RESUME irqs happen too, paired with SUSPEND.
	 */
	if (int_usb & MUSB_INTR_RESUME) {
		musb_handle_intr_resume(musb, devctl);
		handled = IRQ_HANDLED;
	}

	/* see manual for the order of the tests */
	if (int_usb & MUSB_INTR_SESSREQ) {
		if (musb_handle_intr_sessreq(musb, devctl))
			return IRQ_HANDLED;
		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSB_INTR_VBUSERROR) {
		musb_handle_intr_vbuserr(musb, devctl);
		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSB_INTR_SUSPEND) {
		musb_handle_intr_suspend(musb, devctl);
		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSB_INTR_CONNECT) {
		musb_handle_intr_connect(musb, devctl, int_usb);
		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSB_INTR_DISCONNECT) {
		musb_handle_intr_disconnect(musb, devctl);
		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSB_INTR_RESET) {
		musb_handle_intr_reset(musb);
		handled = IRQ_HANDLED;
	}

#if 0
/* REVISIT ... this would be for multiplexing periodic endpoints, or
 * supporting transfer phasing to prevent exceeding ISO bandwidth
 * limits of a given frame or microframe.
 *
 * It's not needed for peripheral side, which dedicates endpoints;
 * though it _might_ use SOF irqs for other purposes.
 *
 * And it's not currently needed for host side, which also dedicates
 * endpoints, relies on TX/RX interval registers, and isn't claimed
 * to support ISO transfers yet.
 */
	if (int_usb & MUSB_INTR_SOF) {
		void __iomem *mbase = musb->mregs;
		struct musb_hw_ep	*ep;
		u8 epnum;
		u16 frame;

		dev_dbg(musb->controller, "START_OF_FRAME\n");
		handled = IRQ_HANDLED;

		/* start any periodic Tx transfers waiting for current frame */
		frame = musb_readw(mbase, MUSB_FRAME);
		ep = musb->endpoints;
		for (epnum = 1; (epnum < musb->nr_endpoints)
					&& (musb->epmask >= (1 << epnum));
				epnum++, ep++) {
			/*
			 * FIXME handle framecounter wraps (12 bits)
			 * eliminate duplicated StartUrb logic
			 */
			if (ep->dwWaitFrame >= frame) {
				ep->dwWaitFrame = 0;
				pr_debug("SOF --> periodic TX%s on %d\n",
					ep->tx_channel ? " DMA" : "",
					epnum);
				if (!ep->tx_channel)
					musb_h_tx_start(musb, epnum);
				else
					cppi_hostdma_start(musb, epnum);
			}
		}		/* end of for loop */
	}
#endif

	schedule_delayed_work(&musb->irq_work, 0);

	return handled;
}

/*-------------------------------------------------------------------------*/

static void musb_disable_interrupts(struct musb *musb)
{
	void __iomem	*mbase = musb->mregs;

	/* disable interrupts */
	musb_writeb(mbase, MUSB_INTRUSBE, 0);
	musb->intrtxe = 0;
	musb_writew(mbase, MUSB_INTRTXE, 0);
	musb->intrrxe = 0;
	musb_writew(mbase, MUSB_INTRRXE, 0);

	/*  flush pending interrupts */
	musb_clearb(mbase, MUSB_INTRUSB);
	musb_clearw(mbase, MUSB_INTRTX);
	musb_clearw(mbase, MUSB_INTRRX);
}

static void musb_enable_interrupts(struct musb *musb)
{
	void __iomem    *regs = musb->mregs;

	/*  Set INT enable registers, enable interrupts */
	musb->intrtxe = musb->epmask;
	musb_writew(regs, MUSB_INTRTXE, musb->intrtxe);
	musb->intrrxe = musb->epmask & 0xfffe;
	musb_writew(regs, MUSB_INTRRXE, musb->intrrxe);
	musb_writeb(regs, MUSB_INTRUSBE, 0xf7);

}

/*
 * Program the HDRC to start (enable interrupts, dma, etc.).
 */
void musb_start(struct musb *musb)
{
	void __iomem    *regs = musb->mregs;
	u8              devctl = musb_readb(regs, MUSB_DEVCTL);
	u8		power;

	musb_dbg(musb, "<== devctl %02x", devctl);

	musb_enable_interrupts(musb);
	musb_writeb(regs, MUSB_TESTMODE, 0);

	power = MUSB_POWER_ISOUPDATE;
	/*
	 * treating UNKNOWN as unspecified maximum speed, in which case
	 * we will default to high-speed.
	 */
	if (musb->config->maximum_speed == USB_SPEED_HIGH ||
			musb->config->maximum_speed == USB_SPEED_UNKNOWN)
		power |= MUSB_POWER_HSENAB;
	musb_writeb(regs, MUSB_POWER, power);

	musb->is_active = 0;
	devctl = musb_readb(regs, MUSB_DEVCTL);
	devctl &= ~MUSB_DEVCTL_SESSION;

	/* session started after:
	 * (a) ID-grounded irq, host mode;
	 * (b) vbus present/connect IRQ, peripheral mode;
	 * (c) peripheral initiates, using SRP
	 */
	if (musb->port_mode != MUSB_HOST &&
			musb->xceiv->otg->state != OTG_STATE_A_WAIT_BCON &&
			(devctl & MUSB_DEVCTL_VBUS) == MUSB_DEVCTL_VBUS) {
		musb->is_active = 1;
	} else {
		devctl |= MUSB_DEVCTL_SESSION;
	}

	musb_platform_enable(musb);
	musb_writeb(regs, MUSB_DEVCTL, devctl);
}

/*
 * Make the HDRC stop (disable interrupts, etc.);
 * reversible by musb_start
 * called on gadget driver unregister
 * with controller locked, irqs blocked
 * acts as a NOP unless some role activated the hardware
 */
void musb_stop(struct musb *musb)
{
	/* stop IRQs, timers, ... */
	musb_platform_disable(musb);
	musb_disable_interrupts(musb);
	musb_writeb(musb->mregs, MUSB_DEVCTL, 0);

	/* FIXME
	 *  - mark host and/or peripheral drivers unusable/inactive
	 *  - disable DMA (and enable it in HdrcStart)
	 *  - make sure we can musb_start() after musb_stop(); with
	 *    OTG mode, gadget driver module rmmod/modprobe cycles that
	 *  - ...
	 */
	musb_platform_try_idle(musb, 0);
}

/*-------------------------------------------------------------------------*/

/*
 * The silicon either has hard-wired endpoint configurations, or else
 * "dynamic fifo" sizing.  The driver has support for both, though at this
 * writing only the dynamic sizing is very well tested.   Since we switched
 * away from compile-time hardware parameters, we can no longer rely on
 * dead code elimination to leave only the relevant one in the object file.
 *
 * We don't currently use dynamic fifo setup capability to do anything
 * more than selecting one of a bunch of predefined configurations.
 */
static ushort fifo_mode;

/* "modprobe ... fifo_mode=1" etc */
module_param(fifo_mode, ushort, 0);
MODULE_PARM_DESC(fifo_mode, "initial endpoint configuration");

/*
 * tables defining fifo_mode values.  define more if you like.
 * for host side, make sure both halves of ep1 are set up.
 */

/* mode 0 - fits in 2KB */
static struct musb_fifo_cfg mode_0_cfg[] = {
{ .hw_ep_num = 1, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num = 1, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num = 2, .style = FIFO_RXTX, .maxpacket = 512, },
{ .hw_ep_num = 3, .style = FIFO_RXTX, .maxpacket = 256, },
{ .hw_ep_num = 4, .style = FIFO_RXTX, .maxpacket = 256, },
};

/* mode 1 - fits in 4KB */
static struct musb_fifo_cfg mode_1_cfg[] = {
{ .hw_ep_num = 1, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_DOUBLE, },
{ .hw_ep_num = 1, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_DOUBLE, },
{ .hw_ep_num = 2, .style = FIFO_RXTX, .maxpacket = 512, .mode = BUF_DOUBLE, },
{ .hw_ep_num = 3, .style = FIFO_RXTX, .maxpacket = 256, },
{ .hw_ep_num = 4, .style = FIFO_RXTX, .maxpacket = 256, },
};

/* mode 2 - fits in 4KB */
static struct musb_fifo_cfg mode_2_cfg[] = {
{ .hw_ep_num = 1, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num = 1, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num = 2, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num = 2, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num = 3, .style = FIFO_RXTX, .maxpacket = 960, },
{ .hw_ep_num = 4, .style = FIFO_RXTX, .maxpacket = 1024, },
};

/* mode 3 - fits in 4KB */
static struct musb_fifo_cfg mode_3_cfg[] = {
{ .hw_ep_num = 1, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_DOUBLE, },
{ .hw_ep_num = 1, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_DOUBLE, },
{ .hw_ep_num = 2, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num = 2, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num = 3, .style = FIFO_RXTX, .maxpacket = 256, },
{ .hw_ep_num = 4, .style = FIFO_RXTX, .maxpacket = 256, },
};

/* mode 4 - fits in 16KB */
static struct musb_fifo_cfg mode_4_cfg[] = {
{ .hw_ep_num =  1, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  1, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  2, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  2, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  3, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  3, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  4, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  4, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  5, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  5, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  6, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  6, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  7, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  7, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  8, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  8, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  9, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  9, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num = 10, .style = FIFO_TX,   .maxpacket = 256, },
{ .hw_ep_num = 10, .style = FIFO_RX,   .maxpacket = 64, },
{ .hw_ep_num = 11, .style = FIFO_TX,   .maxpacket = 256, },
{ .hw_ep_num = 11, .style = FIFO_RX,   .maxpacket = 64, },
{ .hw_ep_num = 12, .style = FIFO_TX,   .maxpacket = 256, },
{ .hw_ep_num = 12, .style = FIFO_RX,   .maxpacket = 64, },
{ .hw_ep_num = 13, .style = FIFO_RXTX, .maxpacket = 4096, },
{ .hw_ep_num = 14, .style = FIFO_RXTX, .maxpacket = 1024, },
{ .hw_ep_num = 15, .style = FIFO_RXTX, .maxpacket = 1024, },
};

/* mode 5 - fits in 8KB */
static struct musb_fifo_cfg mode_5_cfg[] = {
{ .hw_ep_num =  1, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  1, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  2, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  2, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  3, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  3, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  4, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  4, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  5, .style = FIFO_TX,   .maxpacket = 512, },
{ .hw_ep_num =  5, .style = FIFO_RX,   .maxpacket = 512, },
{ .hw_ep_num =  6, .style = FIFO_TX,   .maxpacket = 32, },
{ .hw_ep_num =  6, .style = FIFO_RX,   .maxpacket = 32, },
{ .hw_ep_num =  7, .style = FIFO_TX,   .maxpacket = 32, },
{ .hw_ep_num =  7, .style = FIFO_RX,   .maxpacket = 32, },
{ .hw_ep_num =  8, .style = FIFO_TX,   .maxpacket = 32, },
{ .hw_ep_num =  8, .style = FIFO_RX,   .maxpacket = 32, },
{ .hw_ep_num =  9, .style = FIFO_TX,   .maxpacket = 32, },
{ .hw_ep_num =  9, .style = FIFO_RX,   .maxpacket = 32, },
{ .hw_ep_num = 10, .style = FIFO_TX,   .maxpacket = 32, },
{ .hw_ep_num = 10, .style = FIFO_RX,   .maxpacket = 32, },
{ .hw_ep_num = 11, .style = FIFO_TX,   .maxpacket = 32, },
{ .hw_ep_num = 11, .style = FIFO_RX,   .maxpacket = 32, },
{ .hw_ep_num = 12, .style = FIFO_TX,   .maxpacket = 32, },
{ .hw_ep_num = 12, .style = FIFO_RX,   .maxpacket = 32, },
{ .hw_ep_num = 13, .style = FIFO_RXTX, .maxpacket = 512, },
{ .hw_ep_num = 14, .style = FIFO_RXTX, .maxpacket = 1024, },
{ .hw_ep_num = 15, .style = FIFO_RXTX, .maxpacket = 1024, },
};

/*
 * configure a fifo; for non-shared endpoints, this may be called
 * once for a tx fifo and once for an rx fifo.
 *
 * returns negative errno or offset for next fifo.
 */
static int
fifo_setup(struct musb *musb, struct musb_hw_ep  *hw_ep,
		const struct musb_fifo_cfg *cfg, u16 offset)
{
	void __iomem	*mbase = musb->mregs;
	int	size = 0;
	u16	maxpacket = cfg->maxpacket;
	u16	c_off = offset >> 3;
	u8	c_size;

	/* expect hw_ep has already been zero-initialized */

	size = ffs(max(maxpacket, (u16) 8)) - 1;
	maxpacket = 1 << size;

	c_size = size - 3;
	if (cfg->mode == BUF_DOUBLE) {
		if ((offset + (maxpacket << 1)) >
				(1 << (musb->config->ram_bits + 2)))
			return -EMSGSIZE;
		c_size |= MUSB_FIFOSZ_DPB;
	} else {
		if ((offset + maxpacket) > (1 << (musb->config->ram_bits + 2)))
			return -EMSGSIZE;
	}

	/* configure the FIFO */
	musb_writeb(mbase, MUSB_INDEX, hw_ep->epnum);

	/* EP0 reserved endpoint for control, bidirectional;
	 * EP1 reserved for bulk, two unidirectional halves.
	 */
	if (hw_ep->epnum == 1)
		musb->bulk_ep = hw_ep;
	/* REVISIT error check:  be sure ep0 can both rx and tx ... */
	switch (cfg->style) {
	case FIFO_TX:
		musb_writeb(mbase, MUSB_TXFIFOSZ, c_size);
		musb_writew(mbase, MUSB_TXFIFOADD, c_off);
		hw_ep->tx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_tx = maxpacket;
		break;
	case FIFO_RX:
		musb_writeb(mbase, MUSB_RXFIFOSZ, c_size);
		musb_writew(mbase, MUSB_RXFIFOADD, c_off);
		hw_ep->rx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;
		break;
	case FIFO_RXTX:
		musb_writeb(mbase, MUSB_TXFIFOSZ, c_size);
		musb_writew(mbase, MUSB_TXFIFOADD, c_off);
		hw_ep->rx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;

		musb_writeb(mbase, MUSB_RXFIFOSZ, c_size);
		musb_writew(mbase, MUSB_RXFIFOADD, c_off);
		hw_ep->tx_double_buffered = hw_ep->rx_double_buffered;
		hw_ep->max_packet_sz_tx = maxpacket;

		hw_ep->is_shared_fifo = true;
		break;
	}

	/* NOTE rx and tx endpoint irqs aren't managed separately,
	 * which happens to be ok
	 */
	musb->epmask |= (1 << hw_ep->epnum);

	return offset + (maxpacket << ((c_size & MUSB_FIFOSZ_DPB) ? 1 : 0));
}

static struct musb_fifo_cfg ep0_cfg = {
	.style = FIFO_RXTX, .maxpacket = 64,
};

static int ep_config_from_table(struct musb *musb)
{
	const struct musb_fifo_cfg	*cfg;
	unsigned		i, n;
	int			offset;
	struct musb_hw_ep	*hw_ep = musb->endpoints;

	if (musb->config->fifo_cfg) {
		cfg = musb->config->fifo_cfg;
		n = musb->config->fifo_cfg_size;
		goto done;
	}

	switch (fifo_mode) {
	default:
		fifo_mode = 0;
		fallthrough;
	case 0:
		cfg = mode_0_cfg;
		n = ARRAY_SIZE(mode_0_cfg);
		break;
	case 1:
		cfg = mode_1_cfg;
		n = ARRAY_SIZE(mode_1_cfg);
		break;
	case 2:
		cfg = mode_2_cfg;
		n = ARRAY_SIZE(mode_2_cfg);
		break;
	case 3:
		cfg = mode_3_cfg;
		n = ARRAY_SIZE(mode_3_cfg);
		break;
	case 4:
		cfg = mode_4_cfg;
		n = ARRAY_SIZE(mode_4_cfg);
		break;
	case 5:
		cfg = mode_5_cfg;
		n = ARRAY_SIZE(mode_5_cfg);
		break;
	}

	pr_debug("%s: setup fifo_mode %d\n", musb_driver_name, fifo_mode);


done:
	offset = fifo_setup(musb, hw_ep, &ep0_cfg, 0);
	/* assert(offset > 0) */

	/* NOTE:  for RTL versions >= 1.400 EPINFO and RAMINFO would
	 * be better than static musb->config->num_eps and DYN_FIFO_SIZE...
	 */

	for (i = 0; i < n; i++) {
		u8	epn = cfg->hw_ep_num;

		if (epn >= musb->config->num_eps) {
			pr_debug("%s: invalid ep %d\n",
					musb_driver_name, epn);
			return -EINVAL;
		}
		offset = fifo_setup(musb, hw_ep + epn, cfg++, offset);
		if (offset < 0) {
			pr_debug("%s: mem overrun, ep %d\n",
					musb_driver_name, epn);
			return offset;
		}
		epn++;
		musb->nr_endpoints = max(epn, musb->nr_endpoints);
	}

	pr_debug("%s: %d/%d max ep, %d/%d memory\n",
			musb_driver_name,
			n + 1, musb->config->num_eps * 2 - 1,
			offset, (1 << (musb->config->ram_bits + 2)));

	if (!musb->bulk_ep) {
		pr_debug("%s: missing bulk\n", musb_driver_name);
		return -EINVAL;
	}

	return 0;
}


/*
 * ep_config_from_hw - when MUSB_C_DYNFIFO_DEF is false
 * @param musb the controller
 */
static int ep_config_from_hw(struct musb *musb)
{
	u8 epnum = 0;
	struct musb_hw_ep *hw_ep;
	void __iomem *mbase = musb->mregs;
	int ret = 0;

	musb_dbg(musb, "<== static silicon ep config");

	/* FIXME pick up ep0 maxpacket size */

	for (epnum = 1; epnum < musb->config->num_eps; epnum++) {
		musb_ep_select(mbase, epnum);
		hw_ep = musb->endpoints + epnum;

		ret = musb_read_fifosize(musb, hw_ep, epnum);
		if (ret < 0)
			break;

		/* FIXME set up hw_ep->{rx,tx}_double_buffered */

		/* pick an RX/TX endpoint for bulk */
		if (hw_ep->max_packet_sz_tx < 512
				|| hw_ep->max_packet_sz_rx < 512)
			continue;

		/* REVISIT:  this algorithm is lazy, we should at least
		 * try to pick a double buffered endpoint.
		 */
		if (musb->bulk_ep)
			continue;
		musb->bulk_ep = hw_ep;
	}

	if (!musb->bulk_ep) {
		pr_debug("%s: missing bulk\n", musb_driver_name);
		return -EINVAL;
	}

	return 0;
}

enum { MUSB_CONTROLLER_MHDRC, MUSB_CONTROLLER_HDRC, };

/* Initialize MUSB (M)HDRC part of the USB hardware subsystem;
 * configure endpoints, or take their config from silicon
 */
static int musb_core_init(u16 musb_type, struct musb *musb)
{
	u8 reg;
	char *type;
	char aInfo[90];
	void __iomem	*mbase = musb->mregs;
	int		status = 0;
	int		i;

	/* log core options (read using indexed model) */
	reg = musb_read_configdata(mbase);

	strcpy(aInfo, (reg & MUSB_CONFIGDATA_UTMIDW) ? "UTMI-16" : "UTMI-8");
	if (reg & MUSB_CONFIGDATA_DYNFIFO) {
		strcat(aInfo, ", dyn FIFOs");
		musb->dyn_fifo = true;
	}
	if (reg & MUSB_CONFIGDATA_MPRXE) {
		strcat(aInfo, ", bulk combine");
		musb->bulk_combine = true;
	}
	if (reg & MUSB_CONFIGDATA_MPTXE) {
		strcat(aInfo, ", bulk split");
		musb->bulk_split = true;
	}
	if (reg & MUSB_CONFIGDATA_HBRXE) {
		strcat(aInfo, ", HB-ISO Rx");
		musb->hb_iso_rx = true;
	}
	if (reg & MUSB_CONFIGDATA_HBTXE) {
		strcat(aInfo, ", HB-ISO Tx");
		musb->hb_iso_tx = true;
	}
	if (reg & MUSB_CONFIGDATA_SOFTCONE)
		strcat(aInfo, ", SoftConn");

	pr_debug("%s: ConfigData=0x%02x (%s)\n", musb_driver_name, reg, aInfo);

	if (MUSB_CONTROLLER_MHDRC == musb_type) {
		musb->is_multipoint = 1;
		type = "M";
	} else {
		musb->is_multipoint = 0;
		type = "";
		if (IS_ENABLED(CONFIG_USB) &&
		    !IS_ENABLED(CONFIG_USB_OTG_DISABLE_EXTERNAL_HUB)) {
			pr_err("%s: kernel must disable external hubs, please fix the configuration\n",
			       musb_driver_name);
		}
	}

	/* log release info */
	musb->hwvers = musb_readw(mbase, MUSB_HWVERS);
	pr_debug("%s: %sHDRC RTL version %d.%d%s\n",
		 musb_driver_name, type, MUSB_HWVERS_MAJOR(musb->hwvers),
		 MUSB_HWVERS_MINOR(musb->hwvers),
		 (musb->hwvers & MUSB_HWVERS_RC) ? "RC" : "");

	/* configure ep0 */
	musb_configure_ep0(musb);

	/* discover endpoint configuration */
	musb->nr_endpoints = 1;
	musb->epmask = 1;

	if (musb->dyn_fifo)
		status = ep_config_from_table(musb);
	else
		status = ep_config_from_hw(musb);

	if (status < 0)
		return status;

	/* finish init, and print endpoint config */
	for (i = 0; i < musb->nr_endpoints; i++) {
		struct musb_hw_ep	*hw_ep = musb->endpoints + i;

		hw_ep->fifo = musb->io.fifo_offset(i) + mbase;
#if IS_ENABLED(CONFIG_USB_MUSB_TUSB6010)
		if (musb->ops->quirks & MUSB_IN_TUSB) {
			hw_ep->fifo_async = musb->async + 0x400 +
				musb->io.fifo_offset(i);
			hw_ep->fifo_sync = musb->sync + 0x400 +
				musb->io.fifo_offset(i);
			hw_ep->fifo_sync_va =
				musb->sync_va + 0x400 + musb->io.fifo_offset(i);

			if (i == 0)
				hw_ep->conf = mbase - 0x400 + TUSB_EP0_CONF;
			else
				hw_ep->conf = mbase + 0x400 +
					(((i - 1) & 0xf) << 2);
		}
#endif

		hw_ep->regs = musb->io.ep_offset(i, 0) + mbase;
		hw_ep->rx_reinit = 1;
		hw_ep->tx_reinit = 1;

		if (hw_ep->max_packet_sz_tx) {
			musb_dbg(musb, "%s: hw_ep %d%s, %smax %d",
				musb_driver_name, i,
				hw_ep->is_shared_fifo ? "shared" : "tx",
				hw_ep->tx_double_buffered
					? "doublebuffer, " : "",
				hw_ep->max_packet_sz_tx);
		}
		if (hw_ep->max_packet_sz_rx && !hw_ep->is_shared_fifo) {
			musb_dbg(musb, "%s: hw_ep %d%s, %smax %d",
				musb_driver_name, i,
				"rx",
				hw_ep->rx_double_buffered
					? "doublebuffer, " : "",
				hw_ep->max_packet_sz_rx);
		}
		if (!(hw_ep->max_packet_sz_tx || hw_ep->max_packet_sz_rx))
			musb_dbg(musb, "hw_ep %d not configured", i);
	}

	return 0;
}

/*-------------------------------------------------------------------------*/

/*
 * handle all the irqs defined by the HDRC core. for now we expect:  other
 * irq sources (phy, dma, etc) will be handled first, musb->int_* values
 * will be assigned, and the irq will already have been acked.
 *
 * called in irq context with spinlock held, irqs blocked
 */
irqreturn_t musb_interrupt(struct musb *musb)
{
	irqreturn_t	retval = IRQ_NONE;
	unsigned long	status;
	unsigned long	epnum;
	u8		devctl;

	if (!musb->int_usb && !musb->int_tx && !musb->int_rx)
		return IRQ_NONE;

	devctl = musb_readb(musb->mregs, MUSB_DEVCTL);

	trace_musb_isr(musb);

	/**
	 * According to Mentor Graphics' documentation, flowchart on page 98,
	 * IRQ should be handled as follows:
	 *
	 * . Resume IRQ
	 * . Session Request IRQ
	 * . VBUS Error IRQ
	 * . Suspend IRQ
	 * . Connect IRQ
	 * . Disconnect IRQ
	 * . Reset/Babble IRQ
	 * . SOF IRQ (we're not using this one)
	 * . Endpoint 0 IRQ
	 * . TX Endpoints
	 * . RX Endpoints
	 *
	 * We will be following that flowchart in order to avoid any problems
	 * that might arise with internal Finite State Machine.
	 */

	if (musb->int_usb)
		retval |= musb_stage0_irq(musb, musb->int_usb, devctl);

	if (musb->int_tx & 1) {
		if (is_host_active(musb))
			retval |= musb_h_ep0_irq(musb);
		else
			retval |= musb_g_ep0_irq(musb);

		/* we have just handled endpoint 0 IRQ, clear it */
		musb->int_tx &= ~BIT(0);
	}

	status = musb->int_tx;

	for_each_set_bit(epnum, &status, 16) {
		retval = IRQ_HANDLED;
		if (is_host_active(musb))
			musb_host_tx(musb, epnum);
		else
			musb_g_tx(musb, epnum);
	}

	status = musb->int_rx;

	for_each_set_bit(epnum, &status, 16) {
		retval = IRQ_HANDLED;
		if (is_host_active(musb))
			musb_host_rx(musb, epnum);
		else
			musb_g_rx(musb, epnum);
	}

	return retval;
}
EXPORT_SYMBOL_GPL(musb_interrupt);

#ifndef CONFIG_MUSB_PIO_ONLY
static bool use_dma = true;

/* "modprobe ... use_dma=0" etc */
module_param(use_dma, bool, 0644);
MODULE_PARM_DESC(use_dma, "enable/disable use of DMA");

void musb_dma_completion(struct musb *musb, u8 epnum, u8 transmit)
{
	/* called with controller lock already held */

	if (!epnum) {
		if (!is_cppi_enabled(musb)) {
			/* endpoint 0 */
			if (is_host_active(musb))
				musb_h_ep0_irq(musb);
			else
				musb_g_ep0_irq(musb);
		}
	} else {
		/* endpoints 1..15 */
		if (transmit) {
			if (is_host_active(musb))
				musb_host_tx(musb, epnum);
			else
				musb_g_tx(musb, epnum);
		} else {
			/* receive */
			if (is_host_active(musb))
				musb_host_rx(musb, epnum);
			else
				musb_g_rx(musb, epnum);
		}
	}
}
EXPORT_SYMBOL_GPL(musb_dma_completion);

#else
#define use_dma			0
#endif

static int (*musb_phy_callback)(enum musb_vbus_id_status status);

/*
 * musb_mailbox - optional phy notifier function
 * @status phy state change
 *
 * Optionally gets called from the USB PHY. Note that the USB PHY must be
 * disabled at the point the phy_callback is registered or unregistered.
 */
int musb_mailbox(enum musb_vbus_id_status status)
{
	if (musb_phy_callback)
		return musb_phy_callback(status);

	return -ENODEV;
};
EXPORT_SYMBOL_GPL(musb_mailbox);

/*-------------------------------------------------------------------------*/

static ssize_t
mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct musb *musb = dev_to_musb(dev);
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&musb->lock, flags);
	ret = sprintf(buf, "%s\n", usb_otg_state_string(musb->xceiv->otg->state));
	spin_unlock_irqrestore(&musb->lock, flags);

	return ret;
}

static ssize_t
mode_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t n)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned long	flags;
	int		status;

	spin_lock_irqsave(&musb->lock, flags);
	if (sysfs_streq(buf, "host"))
		status = musb_platform_set_mode(musb, MUSB_HOST);
	else if (sysfs_streq(buf, "peripheral"))
		status = musb_platform_set_mode(musb, MUSB_PERIPHERAL);
	else if (sysfs_streq(buf, "otg"))
		status = musb_platform_set_mode(musb, MUSB_OTG);
	else
		status = -EINVAL;
	spin_unlock_irqrestore(&musb->lock, flags);

	return (status == 0) ? n : status;
}
static DEVICE_ATTR_RW(mode);

static ssize_t
vbus_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t n)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned long	flags;
	unsigned long	val;

	if (sscanf(buf, "%lu", &val) < 1) {
		dev_err(dev, "Invalid VBUS timeout ms value\n");
		return -EINVAL;
	}

	spin_lock_irqsave(&musb->lock, flags);
	/* force T(a_wait_bcon) to be zero/unlimited *OR* valid */
	musb->a_wait_bcon = val ? max_t(int, val, OTG_TIME_A_WAIT_BCON) : 0 ;
	if (musb->xceiv->otg->state == OTG_STATE_A_WAIT_BCON)
		musb->is_active = 0;
	musb_platform_try_idle(musb, jiffies + msecs_to_jiffies(val));
	spin_unlock_irqrestore(&musb->lock, flags);

	return n;
}

static ssize_t
vbus_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned long	flags;
	unsigned long	val;
	int		vbus;
	u8		devctl;

	pm_runtime_get_sync(dev);
	spin_lock_irqsave(&musb->lock, flags);
	val = musb->a_wait_bcon;
	vbus = musb_platform_get_vbus_status(musb);
	if (vbus < 0) {
		/* Use default MUSB method by means of DEVCTL register */
		devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
		if ((devctl & MUSB_DEVCTL_VBUS)
				== (3 << MUSB_DEVCTL_VBUS_SHIFT))
			vbus = 1;
		else
			vbus = 0;
	}
	spin_unlock_irqrestore(&musb->lock, flags);
	pm_runtime_put_sync(dev);

	return sprintf(buf, "Vbus %s, timeout %lu msec\n",
			vbus ? "on" : "off", val);
}
static DEVICE_ATTR_RW(vbus);

/* Gadget drivers can't know that a host is connected so they might want
 * to start SRP, but users can.  This allows userspace to trigger SRP.
 */
static ssize_t srp_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t n)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned short	srp;

	if (sscanf(buf, "%hu", &srp) != 1
			|| (srp != 1)) {
		dev_err(dev, "SRP: Value must be 1\n");
		return -EINVAL;
	}

	if (srp == 1)
		musb_g_wakeup(musb);

	return n;
}
static DEVICE_ATTR_WO(srp);

static struct attribute *musb_attrs[] = {
	&dev_attr_mode.attr,
	&dev_attr_vbus.attr,
	&dev_attr_srp.attr,
	NULL
};
ATTRIBUTE_GROUPS(musb);

#define MUSB_QUIRK_B_INVALID_VBUS_91	(MUSB_DEVCTL_BDEVICE | \
					 (2 << MUSB_DEVCTL_VBUS_SHIFT) | \
					 MUSB_DEVCTL_SESSION)
#define MUSB_QUIRK_B_DISCONNECT_99	(MUSB_DEVCTL_BDEVICE | \
					 (3 << MUSB_DEVCTL_VBUS_SHIFT) | \
					 MUSB_DEVCTL_SESSION)
#define MUSB_QUIRK_A_DISCONNECT_19	((3 << MUSB_DEVCTL_VBUS_SHIFT) | \
					 MUSB_DEVCTL_SESSION)

/*
 * Check the musb devctl session bit to determine if we want to
 * allow PM runtime for the device. In general, we want to keep things
 * active when the session bit is set except after host disconnect.
 *
 * Only called from musb_irq_work. If this ever needs to get called
 * elsewhere, proper locking must be implemented for musb->session.
 */
static void musb_pm_runtime_check_session(struct musb *musb)
{
	u8 devctl, s;
	int error;

	devctl = musb_readb(musb->mregs, MUSB_DEVCTL);

	/* Handle session status quirks first */
	s = MUSB_DEVCTL_FSDEV | MUSB_DEVCTL_LSDEV |
		MUSB_DEVCTL_HR;
	switch (devctl & ~s) {
	case MUSB_QUIRK_B_DISCONNECT_99:
		if (musb->quirk_retries && !musb->flush_irq_work) {
			musb_dbg(musb, "Poll devctl in case of suspend after disconnect\n");
			schedule_delayed_work(&musb->irq_work,
					      msecs_to_jiffies(1000));
			musb->quirk_retries--;
		}
		break;
	case MUSB_QUIRK_B_INVALID_VBUS_91:
		if (musb->quirk_retries && !musb->flush_irq_work) {
			musb_dbg(musb,
				 "Poll devctl on invalid vbus, assume no session");
			schedule_delayed_work(&musb->irq_work,
					      msecs_to_jiffies(1000));
			musb->quirk_retries--;
			return;
		}
		fallthrough;
	case MUSB_QUIRK_A_DISCONNECT_19:
		if (musb->quirk_retries && !musb->flush_irq_work) {
			musb_dbg(musb,
				 "Poll devctl on possible host mode disconnect");
			schedule_delayed_work(&musb->irq_work,
					      msecs_to_jiffies(1000));
			musb->quirk_retries--;
			return;
		}
		if (!musb->session)
			break;
		musb_dbg(musb, "Allow PM on possible host mode disconnect");
		pm_runtime_mark_last_busy(musb->controller);
		pm_runtime_put_autosuspend(musb->controller);
		musb->session = false;
		return;
	default:
		break;
	}

	/* No need to do anything if session has not changed */
	s = devctl & MUSB_DEVCTL_SESSION;
	if (s == musb->session)
		return;

	/* Block PM or allow PM? */
	if (s) {
		musb_dbg(musb, "Block PM on active session: %02x", devctl);
		error = pm_runtime_get_sync(musb->controller);
		if (error < 0)
			dev_err(musb->controller, "Could not enable: %i\n",
				error);
		musb->quirk_retries = 3;
	} else {
		musb_dbg(musb, "Allow PM with no session: %02x", devctl);
		pm_runtime_mark_last_busy(musb->controller);
		pm_runtime_put_autosuspend(musb->controller);
	}

	musb->session = s;
}

/* Only used to provide driver mode change events */
static void musb_irq_work(struct work_struct *data)
{
	struct musb *musb = container_of(data, struct musb, irq_work.work);
	int error;

	error = pm_runtime_resume_and_get(musb->controller);
	if (error < 0) {
		dev_err(musb->controller, "Could not enable: %i\n", error);

		return;
	}

	musb_pm_runtime_check_session(musb);

	if (musb->xceiv->otg->state != musb->xceiv_old_state) {
		musb->xceiv_old_state = musb->xceiv->otg->state;
		sysfs_notify(&musb->controller->kobj, NULL, "mode");
	}

	pm_runtime_mark_last_busy(musb->controller);
	pm_runtime_put_autosuspend(musb->controller);
}

static void musb_recover_from_babble(struct musb *musb)
{
	int ret;
	u8 devctl;

	musb_disable_interrupts(musb);

	/*
	 * wait at least 320 cycles of 60MHz clock. That's 5.3us, we will give
	 * it some slack and wait for 10us.
	 */
	udelay(10);

	ret  = musb_platform_recover(musb);
	if (ret) {
		musb_enable_interrupts(musb);
		return;
	}

	/* drop session bit */
	devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
	devctl &= ~MUSB_DEVCTL_SESSION;
	musb_writeb(musb->mregs, MUSB_DEVCTL, devctl);

	/* tell usbcore about it */
	musb_root_disconnect(musb);

	/*
	 * When a babble condition occurs, the musb controller
	 * removes the session bit and the endpoint config is lost.
	 */
	if (musb->dyn_fifo)
		ret = ep_config_from_table(musb);
	else
		ret = ep_config_from_hw(musb);

	/* restart session */
	if (ret == 0)
		musb_start(musb);
}

/* --------------------------------------------------------------------------
 * Init support
 */

static struct musb *allocate_instance(struct device *dev,
		const struct musb_hdrc_config *config, void __iomem *mbase)
{
	struct musb		*musb;
	struct musb_hw_ep	*ep;
	int			epnum;
	int			ret;

	musb = devm_kzalloc(dev, sizeof(*musb), GFP_KERNEL);
	if (!musb)
		return NULL;

	INIT_LIST_HEAD(&musb->control);
	INIT_LIST_HEAD(&musb->in_bulk);
	INIT_LIST_HEAD(&musb->out_bulk);
	INIT_LIST_HEAD(&musb->pending_list);

	musb->vbuserr_retry = VBUSERR_RETRY_COUNT;
	musb->a_wait_bcon = OTG_TIME_A_WAIT_BCON;
	musb->mregs = mbase;
	musb->ctrl_base = mbase;
	musb->nIrq = -ENODEV;
	musb->config = config;
	BUG_ON(musb->config->num_eps > MUSB_C_NUM_EPS);
	for (epnum = 0, ep = musb->endpoints;
			epnum < musb->config->num_eps;
			epnum++, ep++) {
		ep->musb = musb;
		ep->epnum = epnum;
	}

	musb->controller = dev;

	ret = musb_host_alloc(musb);
	if (ret < 0)
		goto err_free;

	dev_set_drvdata(dev, musb);

	return musb;

err_free:
	return NULL;
}

static void musb_free(struct musb *musb)
{
	/* this has multiple entry modes. it handles fault cleanup after
	 * probe(), where things may be partially set up, as well as rmmod
	 * cleanup after everything's been de-activated.
	 */

	if (musb->nIrq >= 0) {
		if (musb->irq_wake)
			disable_irq_wake(musb->nIrq);
		free_irq(musb->nIrq, musb);
	}

	musb_host_free(musb);
}

struct musb_pending_work {
	int (*callback)(struct musb *musb, void *data);
	void *data;
	struct list_head node;
};

#ifdef CONFIG_PM
/*
 * Called from musb_runtime_resume(), musb_resume(), and
 * musb_queue_resume_work(). Callers must take musb->lock.
 */
static int musb_run_resume_work(struct musb *musb)
{
	struct musb_pending_work *w, *_w;
	unsigned long flags;
	int error = 0;

	spin_lock_irqsave(&musb->list_lock, flags);
	list_for_each_entry_safe(w, _w, &musb->pending_list, node) {
		if (w->callback) {
			error = w->callback(musb, w->data);
			if (error < 0) {
				dev_err(musb->controller,
					"resume callback %p failed: %i\n",
					w->callback, error);
			}
		}
		list_del(&w->node);
		devm_kfree(musb->controller, w);
	}
	spin_unlock_irqrestore(&musb->list_lock, flags);

	return error;
}
#endif

/*
 * Called to run work if device is active or else queue the work to happen
 * on resume. Caller must take musb->lock and must hold an RPM reference.
 *
 * Note that we cowardly refuse queuing work after musb PM runtime
 * resume is done calling musb_run_resume_work() and return -EINPROGRESS
 * instead.
 */
int musb_queue_resume_work(struct musb *musb,
			   int (*callback)(struct musb *musb, void *data),
			   void *data)
{
	struct musb_pending_work *w;
	unsigned long flags;
	bool is_suspended;
	int error;

	if (WARN_ON(!callback))
		return -EINVAL;

	spin_lock_irqsave(&musb->list_lock, flags);
	is_suspended = musb->is_runtime_suspended;

	if (is_suspended) {
		w = devm_kzalloc(musb->controller, sizeof(*w), GFP_ATOMIC);
		if (!w) {
			error = -ENOMEM;
			goto out_unlock;
		}

		w->callback = callback;
		w->data = data;

		list_add_tail(&w->node, &musb->pending_list);
		error = 0;
	}

out_unlock:
	spin_unlock_irqrestore(&musb->list_lock, flags);

	if (!is_suspended)
		error = callback(musb, data);

	return error;
}
EXPORT_SYMBOL_GPL(musb_queue_resume_work);

static void musb_deassert_reset(struct work_struct *work)
{
	struct musb *musb;
	unsigned long flags;

	musb = container_of(work, struct musb, deassert_reset_work.work);

	spin_lock_irqsave(&musb->lock, flags);

	if (musb->port1_status & USB_PORT_STAT_RESET)
		musb_port_reset(musb, false);

	spin_unlock_irqrestore(&musb->lock, flags);
}

/*
 * Perform generic per-controller initialization.
 *
 * @dev: the controller (already clocked, etc)
 * @nIrq: IRQ number
 * @ctrl: virtual address of controller registers,
 *	not yet corrected for platform-specific offsets
 */
static int
musb_init_controller(struct device *dev, int nIrq, void __iomem *ctrl)
{
	int			status;
	struct musb		*musb;
	struct musb_hdrc_platform_data *plat = dev_get_platdata(dev);

	/* The driver might handle more features than the board; OK.
	 * Fail when the board needs a feature that's not enabled.
	 */
	if (!plat) {
		dev_err(dev, "no platform_data?\n");
		status = -ENODEV;
		goto fail0;
	}

	/* allocate */
	musb = allocate_instance(dev, plat->config, ctrl);
	if (!musb) {
		status = -ENOMEM;
		goto fail0;
	}

	spin_lock_init(&musb->lock);
	spin_lock_init(&musb->list_lock);
	musb->board_set_power = plat->set_power;
	musb->min_power = plat->min_power;
	musb->ops = plat->platform_ops;
	musb->port_mode = plat->mode;

	/*
	 * Initialize the default IO functions. At least omap2430 needs
	 * these early. We initialize the platform specific IO functions
	 * later on.
	 */
	musb_readb = musb_default_readb;
	musb_writeb = musb_default_writeb;
	musb_readw = musb_default_readw;
	musb_writew = musb_default_writew;

	/* The musb_platform_init() call:
	 *   - adjusts musb->mregs
	 *   - sets the musb->isr
	 *   - may initialize an integrated transceiver
	 *   - initializes musb->xceiv, usually by otg_get_phy()
	 *   - stops powering VBUS
	 *
	 * There are various transceiver configurations.
	 * DaVinci, TUSB60x0, and others integrate them.  OMAP3 uses
	 * external/discrete ones in various flavors (twl4030 family,
	 * isp1504, non-OTG, etc) mostly hooking up through ULPI.
	 */
	status = musb_platform_init(musb);
	if (status < 0)
		goto fail1;

	if (!musb->isr) {
		status = -ENODEV;
		goto fail2;
	}


	/* Most devices use indexed offset or flat offset */
	if (musb->ops->quirks & MUSB_INDEXED_EP) {
		musb->io.ep_offset = musb_indexed_ep_offset;
		musb->io.ep_select = musb_indexed_ep_select;
	} else {
		musb->io.ep_offset = musb_flat_ep_offset;
		musb->io.ep_select = musb_flat_ep_select;
	}

	if (musb->ops->quirks & MUSB_G_NO_SKB_RESERVE)
		musb->g.quirk_avoids_skb_reserve = 1;

	/* At least tusb6010 has its own offsets */
	if (musb->ops->ep_offset)
		musb->io.ep_offset = musb->ops->ep_offset;
	if (musb->ops->ep_select)
		musb->io.ep_select = musb->ops->ep_select;

	if (musb->ops->fifo_mode)
		fifo_mode = musb->ops->fifo_mode;
	else
		fifo_mode = 4;

	if (musb->ops->fifo_offset)
		musb->io.fifo_offset = musb->ops->fifo_offset;
	else
		musb->io.fifo_offset = musb_default_fifo_offset;

	if (musb->ops->busctl_offset)
		musb->io.busctl_offset = musb->ops->busctl_offset;
	else
		musb->io.busctl_offset = musb_default_busctl_offset;

	if (musb->ops->readb)
		musb_readb = musb->ops->readb;
	if (musb->ops->writeb)
		musb_writeb = musb->ops->writeb;
	if (musb->ops->clearb)
		musb_clearb = musb->ops->clearb;
	else
		musb_clearb = musb_readb;

	if (musb->ops->readw)
		musb_readw = musb->ops->readw;
	if (musb->ops->writew)
		musb_writew = musb->ops->writew;
	if (musb->ops->clearw)
		musb_clearw = musb->ops->clearw;
	else
		musb_clearw = musb_readw;

#ifndef CONFIG_MUSB_PIO_ONLY
	if (!musb->ops->dma_init || !musb->ops->dma_exit) {
		dev_err(dev, "DMA controller not set\n");
		status = -ENODEV;
		goto fail2;
	}
	musb_dma_controller_create = musb->ops->dma_init;
	musb_dma_controller_destroy = musb->ops->dma_exit;
#endif

	if (musb->ops->read_fifo)
		musb->io.read_fifo = musb->ops->read_fifo;
	else
		musb->io.read_fifo = musb_default_read_fifo;

	if (musb->ops->write_fifo)
		musb->io.write_fifo = musb->ops->write_fifo;
	else
		musb->io.write_fifo = musb_default_write_fifo;

	if (musb->ops->get_toggle)
		musb->io.get_toggle = musb->ops->get_toggle;
	else
		musb->io.get_toggle = musb_default_get_toggle;

	if (musb->ops->set_toggle)
		musb->io.set_toggle = musb->ops->set_toggle;
	else
		musb->io.set_toggle = musb_default_set_toggle;

	if (!musb->xceiv->io_ops) {
		musb->xceiv->io_dev = musb->controller;
		musb->xceiv->io_priv = musb->mregs;
		musb->xceiv->io_ops = &musb_ulpi_access;
	}

	if (musb->ops->phy_callback)
		musb_phy_callback = musb->ops->phy_callback;

	/*
	 * We need musb_read/write functions initialized for PM.
	 * Note that at least 2430 glue needs autosuspend delay
	 * somewhere above 300 ms for the hardware to idle properly
	 * after disconnecting the cable in host mode. Let's use
	 * 500 ms for some margin.
	 */
	pm_runtime_use_autosuspend(musb->controller);
	pm_runtime_set_autosuspend_delay(musb->controller, 500);
	pm_runtime_enable(musb->controller);
	pm_runtime_get_sync(musb->controller);

	status = usb_phy_init(musb->xceiv);
	if (status < 0)
		goto err_usb_phy_init;

	if (use_dma && dev->dma_mask) {
		musb->dma_controller =
			musb_dma_controller_create(musb, musb->mregs);
		if (IS_ERR(musb->dma_controller)) {
			status = PTR_ERR(musb->dma_controller);
			goto fail2_5;
		}
	}

	/* be sure interrupts are disabled before connecting ISR */
	musb_platform_disable(musb);
	musb_disable_interrupts(musb);
	musb_writeb(musb->mregs, MUSB_DEVCTL, 0);

	/* MUSB_POWER_SOFTCONN might be already set, JZ4740 does this. */
	musb_writeb(musb->mregs, MUSB_POWER, 0);

	/* Init IRQ workqueue before request_irq */
	INIT_DELAYED_WORK(&musb->irq_work, musb_irq_work);
	INIT_DELAYED_WORK(&musb->deassert_reset_work, musb_deassert_reset);
	INIT_DELAYED_WORK(&musb->finish_resume_work, musb_host_finish_resume);

	/* setup musb parts of the core (especially endpoints) */
	status = musb_core_init(plat->config->multipoint
			? MUSB_CONTROLLER_MHDRC
			: MUSB_CONTROLLER_HDRC, musb);
	if (status < 0)
		goto fail3;

	timer_setup(&musb->otg_timer, musb_otg_timer_func, 0);

	/* attach to the IRQ */
	if (request_irq(nIrq, musb->isr, IRQF_SHARED, dev_name(dev), musb)) {
		dev_err(dev, "request_irq %d failed!\n", nIrq);
		status = -ENODEV;
		goto fail3;
	}
	musb->nIrq = nIrq;
	/* FIXME this handles wakeup irqs wrong */
	if (enable_irq_wake(nIrq) == 0) {
		musb->irq_wake = 1;
		device_init_wakeup(dev, 1);
	} else {
		musb->irq_wake = 0;
	}

	/* program PHY to use external vBus if required */
	if (plat->extvbus) {
		u8 busctl = musb_readb(musb->mregs, MUSB_ULPI_BUSCONTROL);
		busctl |= MUSB_ULPI_USE_EXTVBUS;
		musb_writeb(musb->mregs, MUSB_ULPI_BUSCONTROL, busctl);
	}

	MUSB_DEV_MODE(musb);
	musb->xceiv->otg->state = OTG_STATE_B_IDLE;

	switch (musb->port_mode) {
	case MUSB_HOST:
		status = musb_host_setup(musb, plat->power);
		if (status < 0)
			goto fail3;
		status = musb_platform_set_mode(musb, MUSB_HOST);
		break;
	case MUSB_PERIPHERAL:
		status = musb_gadget_setup(musb);
		if (status < 0)
			goto fail3;
		status = musb_platform_set_mode(musb, MUSB_PERIPHERAL);
		break;
	case MUSB_OTG:
		status = musb_host_setup(musb, plat->power);
		if (status < 0)
			goto fail3;
		status = musb_gadget_setup(musb);
		if (status) {
			musb_host_cleanup(musb);
			goto fail3;
		}
		status = musb_platform_set_mode(musb, MUSB_OTG);
		break;
	default:
		dev_err(dev, "unsupported port mode %d\n", musb->port_mode);
		break;
	}

	if (status < 0)
		goto fail3;

	musb_init_debugfs(musb);

	musb->is_initialized = 1;
	pm_runtime_mark_last_busy(musb->controller);
	pm_runtime_put_autosuspend(musb->controller);

	return 0;

fail3:
	cancel_delayed_work_sync(&musb->irq_work);
	cancel_delayed_work_sync(&musb->finish_resume_work);
	cancel_delayed_work_sync(&musb->deassert_reset_work);
	if (musb->dma_controller)
		musb_dma_controller_destroy(musb->dma_controller);

fail2_5:
	usb_phy_shutdown(musb->xceiv);

err_usb_phy_init:
	pm_runtime_dont_use_autosuspend(musb->controller);
	pm_runtime_put_sync(musb->controller);
	pm_runtime_disable(musb->controller);

fail2:
	if (musb->irq_wake)
		device_init_wakeup(dev, 0);
	musb_platform_exit(musb);

fail1:
	if (status != -EPROBE_DEFER)
		dev_err(musb->controller,
			"%s failed with status %d\n", __func__, status);

	musb_free(musb);

fail0:

	return status;

}

/*-------------------------------------------------------------------------*/

/* all implementations (PCI bridge to FPGA, VLYNQ, etc) should just
 * bridge to a platform device; this driver then suffices.
 */
static int musb_probe(struct platform_device *pdev)
{
	struct device	*dev = &pdev->dev;
	int		irq = platform_get_irq_byname(pdev, "mc");
	void __iomem	*base;

	if (irq <= 0)
		return -ENODEV;

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base))
		return PTR_ERR(base);

	return musb_init_controller(dev, irq, base);
}

static int musb_remove(struct platform_device *pdev)
{
	struct device	*dev = &pdev->dev;
	struct musb	*musb = dev_to_musb(dev);
	unsigned long	flags;

	/* this gets called on rmmod.
	 *  - Host mode: host may still be active
	 *  - Peripheral mode: peripheral is deactivated (or never-activated)
	 *  - OTG mode: both roles are deactivated (or never-activated)
	 */
	musb_exit_debugfs(musb);

	cancel_delayed_work_sync(&musb->irq_work);
	cancel_delayed_work_sync(&musb->finish_resume_work);
	cancel_delayed_work_sync(&musb->deassert_reset_work);
	pm_runtime_get_sync(musb->controller);
	musb_host_cleanup(musb);
	musb_gadget_cleanup(musb);

	musb_platform_disable(musb);
	spin_lock_irqsave(&musb->lock, flags);
	musb_disable_interrupts(musb);
	musb_writeb(musb->mregs, MUSB_DEVCTL, 0);
	spin_unlock_irqrestore(&musb->lock, flags);
	musb_platform_exit(musb);

	pm_runtime_dont_use_autosuspend(musb->controller);
	pm_runtime_put_sync(musb->controller);
	pm_runtime_disable(musb->controller);
	musb_phy_callback = NULL;
	if (musb->dma_controller)
		musb_dma_controller_destroy(musb->dma_controller);
	usb_phy_shutdown(musb->xceiv);
	musb_free(musb);
	device_init_wakeup(dev, 0);
	return 0;
}

#ifdef	CONFIG_PM

static void musb_save_context(struct musb *musb)
{
	int i;
	void __iomem *musb_base = musb->mregs;
	void __iomem *epio;

	musb->context.frame = musb_readw(musb_base, MUSB_FRAME);
	musb->context.testmode = musb_readb(musb_base, MUSB_TESTMODE);
	musb->context.busctl = musb_readb(musb_base, MUSB_ULPI_BUSCONTROL);
	musb->context.power = musb_readb(musb_base, MUSB_POWER);
	musb->context.intrusbe = musb_readb(musb_base, MUSB_INTRUSBE);
	musb->context.index = musb_readb(musb_base, MUSB_INDEX);
	musb->context.devctl = musb_readb(musb_base, MUSB_DEVCTL);

	for (i = 0; i < musb->config->num_eps; ++i) {
		struct musb_hw_ep	*hw_ep;

		hw_ep = &musb->endpoints[i];
		if (!hw_ep)
			continue;

		epio = hw_ep->regs;
		if (!epio)
			continue;

		musb_writeb(musb_base, MUSB_INDEX, i);
		musb->context.index_regs[i].txmaxp =
			musb_readw(epio, MUSB_TXMAXP);
		musb->context.index_regs[i].txcsr =
			musb_readw(epio, MUSB_TXCSR);
		musb->context.index_regs[i].rxmaxp =
			musb_readw(epio, MUSB_RXMAXP);
		musb->context.index_regs[i].rxcsr =
			musb_readw(epio, MUSB_RXCSR);

		if (musb->dyn_fifo) {
			musb->context.index_regs[i].txfifoadd =
					musb_readw(musb_base, MUSB_TXFIFOADD);
			musb->context.index_regs[i].rxfifoadd =
					musb_readw(musb_base, MUSB_RXFIFOADD);
			musb->context.index_regs[i].txfifosz =
					musb_readb(musb_base, MUSB_TXFIFOSZ);
			musb->context.index_regs[i].rxfifosz =
					musb_readb(musb_base, MUSB_RXFIFOSZ);
		}

		musb->context.index_regs[i].txtype =
			musb_readb(epio, MUSB_TXTYPE);
		musb->context.index_regs[i].txinterval =
			musb_readb(epio, MUSB_TXINTERVAL);
		musb->context.index_regs[i].rxtype =
			musb_readb(epio, MUSB_RXTYPE);
		musb->context.index_regs[i].rxinterval =
			musb_readb(epio, MUSB_RXINTERVAL);

		musb->context.index_regs[i].txfunaddr =
			musb_read_txfunaddr(musb, i);
		musb->context.index_regs[i].txhubaddr =
			musb_read_txhubaddr(musb, i);
		musb->context.index_regs[i].txhubport =
			musb_read_txhubport(musb, i);

		musb->context.index_regs[i].rxfunaddr =
			musb_read_rxfunaddr(musb, i);
		musb->context.index_regs[i].rxhubaddr =
			musb_read_rxhubaddr(musb, i);
		musb->context.index_regs[i].rxhubport =
			musb_read_rxhubport(musb, i);
	}
}

static void musb_restore_context(struct musb *musb)
{
	int i;
	void __iomem *musb_base = musb->mregs;
	void __iomem *epio;
	u8 power;

	musb_writew(musb_base, MUSB_FRAME, musb->context.frame);
	musb_writeb(musb_base, MUSB_TESTMODE, musb->context.testmode);
	musb_writeb(musb_base, MUSB_ULPI_BUSCONTROL, musb->context.busctl);

	/* Don't affect SUSPENDM/RESUME bits in POWER reg */
	power = musb_readb(musb_base, MUSB_POWER);
	power &= MUSB_POWER_SUSPENDM | MUSB_POWER_RESUME;
	musb->context.power &= ~(MUSB_POWER_SUSPENDM | MUSB_POWER_RESUME);
	power |= musb->context.power;
	musb_writeb(musb_base, MUSB_POWER, power);

	musb_writew(musb_base, MUSB_INTRTXE, musb->intrtxe);
	musb_writew(musb_base, MUSB_INTRRXE, musb->intrrxe);
	musb_writeb(musb_base, MUSB_INTRUSBE, musb->context.intrusbe);
	if (musb->context.devctl & MUSB_DEVCTL_SESSION)
		musb_writeb(musb_base, MUSB_DEVCTL, musb->context.devctl);

	for (i = 0; i < musb->config->num_eps; ++i) {
		struct musb_hw_ep	*hw_ep;

		hw_ep = &musb->endpoints[i];
		if (!hw_ep)
			continue;

		epio = hw_ep->regs;
		if (!epio)
			continue;

		musb_writeb(musb_base, MUSB_INDEX, i);
		musb_writew(epio, MUSB_TXMAXP,
			musb->context.index_regs[i].txmaxp);
		musb_writew(epio, MUSB_TXCSR,
			musb->context.index_regs[i].txcsr);
		musb_writew(epio, MUSB_RXMAXP,
			musb->context.index_regs[i].rxmaxp);
		musb_writew(epio, MUSB_RXCSR,
			musb->context.index_regs[i].rxcsr);

		if (musb->dyn_fifo) {
			musb_writeb(musb_base, MUSB_TXFIFOSZ,
				musb->context.index_regs[i].txfifosz);
			musb_writeb(musb_base, MUSB_RXFIFOSZ,
				musb->context.index_regs[i].rxfifosz);
			musb_writew(musb_base, MUSB_TXFIFOADD,
				musb->context.index_regs[i].txfifoadd);
			musb_writew(musb_base, MUSB_RXFIFOADD,
				musb->context.index_regs[i].rxfifoadd);
		}

		musb_writeb(epio, MUSB_TXTYPE,
				musb->context.index_regs[i].txtype);
		musb_writeb(epio, MUSB_TXINTERVAL,
				musb->context.index_regs[i].txinterval);
		musb_writeb(epio, MUSB_RXTYPE,
				musb->context.index_regs[i].rxtype);
		musb_writeb(epio, MUSB_RXINTERVAL,

				musb->context.index_regs[i].rxinterval);
		musb_write_txfunaddr(musb, i,
				musb->context.index_regs[i].txfunaddr);
		musb_write_txhubaddr(musb, i,
				musb->context.index_regs[i].txhubaddr);
		musb_write_txhubport(musb, i,
				musb->context.index_regs[i].txhubport);

		musb_write_rxfunaddr(musb, i,
				musb->context.index_regs[i].rxfunaddr);
		musb_write_rxhubaddr(musb, i,
				musb->context.index_regs[i].rxhubaddr);
		musb_write_rxhubport(musb, i,
				musb->context.index_regs[i].rxhubport);
	}
	musb_writeb(musb_base, MUSB_INDEX, musb->context.index);
}

static int musb_suspend(struct device *dev)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned long	flags;
	int ret;

	ret = pm_runtime_get_sync(dev);
	if (ret < 0) {
		pm_runtime_put_noidle(dev);
		return ret;
	}

	musb_platform_disable(musb);
	musb_disable_interrupts(musb);

	musb->flush_irq_work = true;
	while (flush_delayed_work(&musb->irq_work))
		;
	musb->flush_irq_work = false;

	if (!(musb->ops->quirks & MUSB_PRESERVE_SESSION))
		musb_writeb(musb->mregs, MUSB_DEVCTL, 0);

	WARN_ON(!list_empty(&musb->pending_list));

	spin_lock_irqsave(&musb->lock, flags);

	if (is_peripheral_active(musb)) {
		/* FIXME force disconnect unless we know USB will wake
		 * the system up quickly enough to respond ...
		 */
	} else if (is_host_active(musb)) {
		/* we know all the children are suspended; sometimes
		 * they will even be wakeup-enabled.
		 */
	}

	musb_save_context(musb);

	spin_unlock_irqrestore(&musb->lock, flags);
	return 0;
}

static int musb_resume(struct device *dev)
{
	struct musb *musb = dev_to_musb(dev);
	unsigned long flags;
	int error;
	u8 devctl;
	u8 mask;

	/*
	 * For static cmos like DaVinci, register values were preserved
	 * unless for some reason the whole soc powered down or the USB
	 * module got reset through the PSC (vs just being disabled).
	 *
	 * For the DSPS glue layer though, a full register restore has to
	 * be done. As it shouldn't harm other platforms, we do it
	 * unconditionally.
	 */

	musb_restore_context(musb);

	devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
	mask = MUSB_DEVCTL_BDEVICE | MUSB_DEVCTL_FSDEV | MUSB_DEVCTL_LSDEV;
	if ((devctl & mask) != (musb->context.devctl & mask))
		musb->port1_status = 0;

	musb_enable_interrupts(musb);
	musb_platform_enable(musb);

	/* session might be disabled in suspend */
	if (musb->port_mode == MUSB_HOST &&
	    !(musb->ops->quirks & MUSB_PRESERVE_SESSION)) {
		devctl |= MUSB_DEVCTL_SESSION;
		musb_writeb(musb->mregs, MUSB_DEVCTL, devctl);
	}

	spin_lock_irqsave(&musb->lock, flags);
	error = musb_run_resume_work(musb);
	if (error)
		dev_err(musb->controller, "resume work failed with %i\n",
			error);
	spin_unlock_irqrestore(&musb->lock, flags);

	pm_runtime_mark_last_busy(dev);
	pm_runtime_put_autosuspend(dev);

	return 0;
}

static int musb_runtime_suspend(struct device *dev)
{
	struct musb	*musb = dev_to_musb(dev);

	musb_save_context(musb);
	musb->is_runtime_suspended = 1;

	return 0;
}

static int musb_runtime_resume(struct device *dev)
{
	struct musb *musb = dev_to_musb(dev);
	unsigned long flags;
	int error;

	/*
	 * When pm_runtime_get_sync called for the first time in driver
	 * init,  some of the structure is still not initialized which is
	 * used in restore function. But clock needs to be
	 * enabled before any register access, so
	 * pm_runtime_get_sync has to be called.
	 * Also context restore without save does not make
	 * any sense
	 */
	if (!musb->is_initialized)
		return 0;

	musb_restore_context(musb);

	spin_lock_irqsave(&musb->lock, flags);
	error = musb_run_resume_work(musb);
	if (error)
		dev_err(musb->controller, "resume work failed with %i\n",
			error);
	musb->is_runtime_suspended = 0;
	spin_unlock_irqrestore(&musb->lock, flags);

	return 0;
}

static const struct dev_pm_ops musb_dev_pm_ops = {
	.suspend	= musb_suspend,
	.resume		= musb_resume,
	.runtime_suspend = musb_runtime_suspend,
	.runtime_resume = musb_runtime_resume,
};

#define MUSB_DEV_PM_OPS (&musb_dev_pm_ops)
#else
#define	MUSB_DEV_PM_OPS	NULL
#endif

static struct platform_driver musb_driver = {
	.driver = {
		.name		= musb_driver_name,
		.bus		= &platform_bus_type,
		.pm		= MUSB_DEV_PM_OPS,
		.dev_groups	= musb_groups,
	},
	.probe		= musb_probe,
	.remove		= musb_remove,
};

module_platform_driver(musb_driver);
