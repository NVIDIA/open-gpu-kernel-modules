// SPDX-License-Identifier: GPL-2.0
/*
 * TUSB6010 USB 2.0 OTG Dual Role controller
 *
 * Copyright (C) 2006 Nokia Corporation
 * Tony Lindgren <tony@atomide.com>
 *
 * Notes:
 * - Driver assumes that interface to external host (main CPU) is
 *   configured for NOR FLASH interface instead of VLYNQ serial
 *   interface.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/prefetch.h>
#include <linux/usb.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/usb/usb_phy_generic.h>

#include "musb_core.h"

struct tusb6010_glue {
	struct device		*dev;
	struct platform_device	*musb;
	struct platform_device	*phy;
};

static void tusb_musb_set_vbus(struct musb *musb, int is_on);

#define TUSB_REV_MAJOR(reg_val)		((reg_val >> 4) & 0xf)
#define TUSB_REV_MINOR(reg_val)		(reg_val & 0xf)

/*
 * Checks the revision. We need to use the DMA register as 3.0 does not
 * have correct versions for TUSB_PRCM_REV or TUSB_INT_CTRL_REV.
 */
static u8 tusb_get_revision(struct musb *musb)
{
	void __iomem	*tbase = musb->ctrl_base;
	u32		die_id;
	u8		rev;

	rev = musb_readl(tbase, TUSB_DMA_CTRL_REV) & 0xff;
	if (TUSB_REV_MAJOR(rev) == 3) {
		die_id = TUSB_DIDR1_HI_CHIP_REV(musb_readl(tbase,
				TUSB_DIDR1_HI));
		if (die_id >= TUSB_DIDR1_HI_REV_31)
			rev |= 1;
	}

	return rev;
}

static void tusb_print_revision(struct musb *musb)
{
	void __iomem	*tbase = musb->ctrl_base;
	u8		rev;

	rev = musb->tusb_revision;

	pr_info("tusb: %s%i.%i %s%i.%i %s%i.%i %s%i.%i %s%i %s%i.%i\n",
		"prcm",
		TUSB_REV_MAJOR(musb_readl(tbase, TUSB_PRCM_REV)),
		TUSB_REV_MINOR(musb_readl(tbase, TUSB_PRCM_REV)),
		"int",
		TUSB_REV_MAJOR(musb_readl(tbase, TUSB_INT_CTRL_REV)),
		TUSB_REV_MINOR(musb_readl(tbase, TUSB_INT_CTRL_REV)),
		"gpio",
		TUSB_REV_MAJOR(musb_readl(tbase, TUSB_GPIO_REV)),
		TUSB_REV_MINOR(musb_readl(tbase, TUSB_GPIO_REV)),
		"dma",
		TUSB_REV_MAJOR(musb_readl(tbase, TUSB_DMA_CTRL_REV)),
		TUSB_REV_MINOR(musb_readl(tbase, TUSB_DMA_CTRL_REV)),
		"dieid",
		TUSB_DIDR1_HI_CHIP_REV(musb_readl(tbase, TUSB_DIDR1_HI)),
		"rev",
		TUSB_REV_MAJOR(rev), TUSB_REV_MINOR(rev));
}

#define WBUS_QUIRK_MASK	(TUSB_PHY_OTG_CTRL_TESTM2 | TUSB_PHY_OTG_CTRL_TESTM1 \
				| TUSB_PHY_OTG_CTRL_TESTM0)

/*
 * Workaround for spontaneous WBUS wake-up issue #2 for tusb3.0.
 * Disables power detection in PHY for the duration of idle.
 */
static void tusb_wbus_quirk(struct musb *musb, int enabled)
{
	void __iomem	*tbase = musb->ctrl_base;
	static u32	phy_otg_ctrl, phy_otg_ena;
	u32		tmp;

	if (enabled) {
		phy_otg_ctrl = musb_readl(tbase, TUSB_PHY_OTG_CTRL);
		phy_otg_ena = musb_readl(tbase, TUSB_PHY_OTG_CTRL_ENABLE);
		tmp = TUSB_PHY_OTG_CTRL_WRPROTECT
				| phy_otg_ena | WBUS_QUIRK_MASK;
		musb_writel(tbase, TUSB_PHY_OTG_CTRL, tmp);
		tmp = phy_otg_ena & ~WBUS_QUIRK_MASK;
		tmp |= TUSB_PHY_OTG_CTRL_WRPROTECT | TUSB_PHY_OTG_CTRL_TESTM2;
		musb_writel(tbase, TUSB_PHY_OTG_CTRL_ENABLE, tmp);
		dev_dbg(musb->controller, "Enabled tusb wbus quirk ctrl %08x ena %08x\n",
			musb_readl(tbase, TUSB_PHY_OTG_CTRL),
			musb_readl(tbase, TUSB_PHY_OTG_CTRL_ENABLE));
	} else if (musb_readl(tbase, TUSB_PHY_OTG_CTRL_ENABLE)
					& TUSB_PHY_OTG_CTRL_TESTM2) {
		tmp = TUSB_PHY_OTG_CTRL_WRPROTECT | phy_otg_ctrl;
		musb_writel(tbase, TUSB_PHY_OTG_CTRL, tmp);
		tmp = TUSB_PHY_OTG_CTRL_WRPROTECT | phy_otg_ena;
		musb_writel(tbase, TUSB_PHY_OTG_CTRL_ENABLE, tmp);
		dev_dbg(musb->controller, "Disabled tusb wbus quirk ctrl %08x ena %08x\n",
			musb_readl(tbase, TUSB_PHY_OTG_CTRL),
			musb_readl(tbase, TUSB_PHY_OTG_CTRL_ENABLE));
		phy_otg_ctrl = 0;
		phy_otg_ena = 0;
	}
}

static u32 tusb_fifo_offset(u8 epnum)
{
	return 0x200 + (epnum * 0x20);
}

static u32 tusb_ep_offset(u8 epnum, u16 offset)
{
	return 0x10 + offset;
}

/* TUSB mapping: "flat" plus ep0 special cases */
static void tusb_ep_select(void __iomem *mbase, u8 epnum)
{
	musb_writeb(mbase, MUSB_INDEX, epnum);
}

/*
 * TUSB6010 doesn't allow 8-bit access; 16-bit access is the minimum.
 */
static u8 tusb_readb(void __iomem *addr, u32 offset)
{
	u16 tmp;
	u8 val;

	tmp = __raw_readw(addr + (offset & ~1));
	if (offset & 1)
		val = (tmp >> 8);
	else
		val = tmp & 0xff;

	return val;
}

static void tusb_writeb(void __iomem *addr, u32 offset, u8 data)
{
	u16 tmp;

	tmp = __raw_readw(addr + (offset & ~1));
	if (offset & 1)
		tmp = (data << 8) | (tmp & 0xff);
	else
		tmp = (tmp & 0xff00) | data;

	__raw_writew(tmp, addr + (offset & ~1));
}

/*
 * TUSB 6010 may use a parallel bus that doesn't support byte ops;
 * so both loading and unloading FIFOs need explicit byte counts.
 */

static inline void
tusb_fifo_write_unaligned(void __iomem *fifo, const u8 *buf, u16 len)
{
	u32		val;
	int		i;

	if (len > 4) {
		for (i = 0; i < (len >> 2); i++) {
			memcpy(&val, buf, 4);
			musb_writel(fifo, 0, val);
			buf += 4;
		}
		len %= 4;
	}
	if (len > 0) {
		/* Write the rest 1 - 3 bytes to FIFO */
		memcpy(&val, buf, len);
		musb_writel(fifo, 0, val);
	}
}

static inline void tusb_fifo_read_unaligned(void __iomem *fifo,
						void *buf, u16 len)
{
	u32		val;
	int		i;

	if (len > 4) {
		for (i = 0; i < (len >> 2); i++) {
			val = musb_readl(fifo, 0);
			memcpy(buf, &val, 4);
			buf += 4;
		}
		len %= 4;
	}
	if (len > 0) {
		/* Read the rest 1 - 3 bytes from FIFO */
		val = musb_readl(fifo, 0);
		memcpy(buf, &val, len);
	}
}

static void tusb_write_fifo(struct musb_hw_ep *hw_ep, u16 len, const u8 *buf)
{
	struct musb *musb = hw_ep->musb;
	void __iomem	*ep_conf = hw_ep->conf;
	void __iomem	*fifo = hw_ep->fifo;
	u8		epnum = hw_ep->epnum;

	prefetch(buf);

	dev_dbg(musb->controller, "%cX ep%d fifo %p count %d buf %p\n",
			'T', epnum, fifo, len, buf);

	if (epnum)
		musb_writel(ep_conf, TUSB_EP_TX_OFFSET,
			TUSB_EP_CONFIG_XFR_SIZE(len));
	else
		musb_writel(ep_conf, 0, TUSB_EP0_CONFIG_DIR_TX |
			TUSB_EP0_CONFIG_XFR_SIZE(len));

	if (likely((0x01 & (unsigned long) buf) == 0)) {

		/* Best case is 32bit-aligned destination address */
		if ((0x02 & (unsigned long) buf) == 0) {
			if (len >= 4) {
				iowrite32_rep(fifo, buf, len >> 2);
				buf += (len & ~0x03);
				len &= 0x03;
			}
		} else {
			if (len >= 2) {
				u32 val;
				int i;

				/* Cannot use writesw, fifo is 32-bit */
				for (i = 0; i < (len >> 2); i++) {
					val = (u32)(*(u16 *)buf);
					buf += 2;
					val |= (*(u16 *)buf) << 16;
					buf += 2;
					musb_writel(fifo, 0, val);
				}
				len &= 0x03;
			}
		}
	}

	if (len > 0)
		tusb_fifo_write_unaligned(fifo, buf, len);
}

static void tusb_read_fifo(struct musb_hw_ep *hw_ep, u16 len, u8 *buf)
{
	struct musb *musb = hw_ep->musb;
	void __iomem	*ep_conf = hw_ep->conf;
	void __iomem	*fifo = hw_ep->fifo;
	u8		epnum = hw_ep->epnum;

	dev_dbg(musb->controller, "%cX ep%d fifo %p count %d buf %p\n",
			'R', epnum, fifo, len, buf);

	if (epnum)
		musb_writel(ep_conf, TUSB_EP_RX_OFFSET,
			TUSB_EP_CONFIG_XFR_SIZE(len));
	else
		musb_writel(ep_conf, 0, TUSB_EP0_CONFIG_XFR_SIZE(len));

	if (likely((0x01 & (unsigned long) buf) == 0)) {

		/* Best case is 32bit-aligned destination address */
		if ((0x02 & (unsigned long) buf) == 0) {
			if (len >= 4) {
				ioread32_rep(fifo, buf, len >> 2);
				buf += (len & ~0x03);
				len &= 0x03;
			}
		} else {
			if (len >= 2) {
				u32 val;
				int i;

				/* Cannot use readsw, fifo is 32-bit */
				for (i = 0; i < (len >> 2); i++) {
					val = musb_readl(fifo, 0);
					*(u16 *)buf = (u16)(val & 0xffff);
					buf += 2;
					*(u16 *)buf = (u16)(val >> 16);
					buf += 2;
				}
				len &= 0x03;
			}
		}
	}

	if (len > 0)
		tusb_fifo_read_unaligned(fifo, buf, len);
}

static struct musb *the_musb;

/* This is used by gadget drivers, and OTG transceiver logic, allowing
 * at most mA current to be drawn from VBUS during a Default-B session
 * (that is, while VBUS exceeds 4.4V).  In Default-A (including pure host
 * mode), or low power Default-B sessions, something else supplies power.
 * Caller must take care of locking.
 */
static int tusb_draw_power(struct usb_phy *x, unsigned mA)
{
	struct musb	*musb = the_musb;
	void __iomem	*tbase = musb->ctrl_base;
	u32		reg;

	/* tps65030 seems to consume max 100mA, with maybe 60mA available
	 * (measured on one board) for things other than tps and tusb.
	 *
	 * Boards sharing the CPU clock with CLKIN will need to prevent
	 * certain idle sleep states while the USB link is active.
	 *
	 * REVISIT we could use VBUS to supply only _one_ of { 1.5V, 3.3V }.
	 * The actual current usage would be very board-specific.  For now,
	 * it's simpler to just use an aggregate (also board-specific).
	 */
	if (x->otg->default_a || mA < (musb->min_power << 1))
		mA = 0;

	reg = musb_readl(tbase, TUSB_PRCM_MNGMT);
	if (mA) {
		musb->is_bus_powered = 1;
		reg |= TUSB_PRCM_MNGMT_15_SW_EN | TUSB_PRCM_MNGMT_33_SW_EN;
	} else {
		musb->is_bus_powered = 0;
		reg &= ~(TUSB_PRCM_MNGMT_15_SW_EN | TUSB_PRCM_MNGMT_33_SW_EN);
	}
	musb_writel(tbase, TUSB_PRCM_MNGMT, reg);

	dev_dbg(musb->controller, "draw max %d mA VBUS\n", mA);
	return 0;
}

/* workaround for issue 13:  change clock during chip idle
 * (to be fixed in rev3 silicon) ... symptoms include disconnect
 * or looping suspend/resume cycles
 */
static void tusb_set_clock_source(struct musb *musb, unsigned mode)
{
	void __iomem	*tbase = musb->ctrl_base;
	u32		reg;

	reg = musb_readl(tbase, TUSB_PRCM_CONF);
	reg &= ~TUSB_PRCM_CONF_SYS_CLKSEL(0x3);

	/* 0 = refclk (clkin, XI)
	 * 1 = PHY 60 MHz (internal PLL)
	 * 2 = not supported
	 * 3 = what?
	 */
	if (mode > 0)
		reg |= TUSB_PRCM_CONF_SYS_CLKSEL(mode & 0x3);

	musb_writel(tbase, TUSB_PRCM_CONF, reg);

	/* FIXME tusb6010_platform_retime(mode == 0); */
}

/*
 * Idle TUSB6010 until next wake-up event; NOR access always wakes.
 * Other code ensures that we idle unless we're connected _and_ the
 * USB link is not suspended ... and tells us the relevant wakeup
 * events.  SW_EN for voltage is handled separately.
 */
static void tusb_allow_idle(struct musb *musb, u32 wakeup_enables)
{
	void __iomem	*tbase = musb->ctrl_base;
	u32		reg;

	if ((wakeup_enables & TUSB_PRCM_WBUS)
			&& (musb->tusb_revision == TUSB_REV_30))
		tusb_wbus_quirk(musb, 1);

	tusb_set_clock_source(musb, 0);

	wakeup_enables |= TUSB_PRCM_WNORCS;
	musb_writel(tbase, TUSB_PRCM_WAKEUP_MASK, ~wakeup_enables);

	/* REVISIT writeup of WID implies that if WID set and ID is grounded,
	 * TUSB_PHY_OTG_CTRL.TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP must be cleared.
	 * Presumably that's mostly to save power, hence WID is immaterial ...
	 */

	reg = musb_readl(tbase, TUSB_PRCM_MNGMT);
	/* issue 4: when driving vbus, use hipower (vbus_det) comparator */
	if (is_host_active(musb)) {
		reg |= TUSB_PRCM_MNGMT_OTG_VBUS_DET_EN;
		reg &= ~TUSB_PRCM_MNGMT_OTG_SESS_END_EN;
	} else {
		reg |= TUSB_PRCM_MNGMT_OTG_SESS_END_EN;
		reg &= ~TUSB_PRCM_MNGMT_OTG_VBUS_DET_EN;
	}
	reg |= TUSB_PRCM_MNGMT_PM_IDLE | TUSB_PRCM_MNGMT_DEV_IDLE;
	musb_writel(tbase, TUSB_PRCM_MNGMT, reg);

	dev_dbg(musb->controller, "idle, wake on %02x\n", wakeup_enables);
}

/*
 * Updates cable VBUS status. Caller must take care of locking.
 */
static int tusb_musb_vbus_status(struct musb *musb)
{
	void __iomem	*tbase = musb->ctrl_base;
	u32		otg_stat, prcm_mngmt;
	int		ret = 0;

	otg_stat = musb_readl(tbase, TUSB_DEV_OTG_STAT);
	prcm_mngmt = musb_readl(tbase, TUSB_PRCM_MNGMT);

	/* Temporarily enable VBUS detection if it was disabled for
	 * suspend mode. Unless it's enabled otg_stat and devctl will
	 * not show correct VBUS state.
	 */
	if (!(prcm_mngmt & TUSB_PRCM_MNGMT_OTG_VBUS_DET_EN)) {
		u32 tmp = prcm_mngmt;
		tmp |= TUSB_PRCM_MNGMT_OTG_VBUS_DET_EN;
		musb_writel(tbase, TUSB_PRCM_MNGMT, tmp);
		otg_stat = musb_readl(tbase, TUSB_DEV_OTG_STAT);
		musb_writel(tbase, TUSB_PRCM_MNGMT, prcm_mngmt);
	}

	if (otg_stat & TUSB_DEV_OTG_STAT_VBUS_VALID)
		ret = 1;

	return ret;
}

static void musb_do_idle(struct timer_list *t)
{
	struct musb	*musb = from_timer(musb, t, dev_timer);
	unsigned long	flags;

	spin_lock_irqsave(&musb->lock, flags);

	switch (musb->xceiv->otg->state) {
	case OTG_STATE_A_WAIT_BCON:
		if ((musb->a_wait_bcon != 0)
			&& (musb->idle_timeout == 0
				|| time_after(jiffies, musb->idle_timeout))) {
			dev_dbg(musb->controller, "Nothing connected %s, turning off VBUS\n",
					usb_otg_state_string(musb->xceiv->otg->state));
		}
		fallthrough;
	case OTG_STATE_A_IDLE:
		tusb_musb_set_vbus(musb, 0);
		break;
	default:
		break;
	}

	if (!musb->is_active) {
		u32	wakeups;

		/* wait until hub_wq handles port change status */
		if (is_host_active(musb) && (musb->port1_status >> 16))
			goto done;

		if (!musb->gadget_driver) {
			wakeups = 0;
		} else {
			wakeups = TUSB_PRCM_WHOSTDISCON
				| TUSB_PRCM_WBUS
					| TUSB_PRCM_WVBUS;
			wakeups |= TUSB_PRCM_WID;
		}
		tusb_allow_idle(musb, wakeups);
	}
done:
	spin_unlock_irqrestore(&musb->lock, flags);
}

/*
 * Maybe put TUSB6010 into idle mode mode depending on USB link status,
 * like "disconnected" or "suspended".  We'll be woken out of it by
 * connect, resume, or disconnect.
 *
 * Needs to be called as the last function everywhere where there is
 * register access to TUSB6010 because of NOR flash wake-up.
 * Caller should own controller spinlock.
 *
 * Delay because peripheral enables D+ pullup 3msec after SE0, and
 * we don't want to treat that full speed J as a wakeup event.
 * ... peripherals must draw only suspend current after 10 msec.
 */
static void tusb_musb_try_idle(struct musb *musb, unsigned long timeout)
{
	unsigned long		default_timeout = jiffies + msecs_to_jiffies(3);
	static unsigned long	last_timer;

	if (timeout == 0)
		timeout = default_timeout;

	/* Never idle if active, or when VBUS timeout is not set as host */
	if (musb->is_active || ((musb->a_wait_bcon == 0)
			&& (musb->xceiv->otg->state == OTG_STATE_A_WAIT_BCON))) {
		dev_dbg(musb->controller, "%s active, deleting timer\n",
			usb_otg_state_string(musb->xceiv->otg->state));
		del_timer(&musb->dev_timer);
		last_timer = jiffies;
		return;
	}

	if (time_after(last_timer, timeout)) {
		if (!timer_pending(&musb->dev_timer))
			last_timer = timeout;
		else {
			dev_dbg(musb->controller, "Longer idle timer already pending, ignoring\n");
			return;
		}
	}
	last_timer = timeout;

	dev_dbg(musb->controller, "%s inactive, for idle timer for %lu ms\n",
		usb_otg_state_string(musb->xceiv->otg->state),
		(unsigned long)jiffies_to_msecs(timeout - jiffies));
	mod_timer(&musb->dev_timer, timeout);
}

/* ticks of 60 MHz clock */
#define DEVCLOCK		60000000
#define OTG_TIMER_MS(msecs)	((msecs) \
		? (TUSB_DEV_OTG_TIMER_VAL((DEVCLOCK/1000)*(msecs)) \
				| TUSB_DEV_OTG_TIMER_ENABLE) \
		: 0)

static void tusb_musb_set_vbus(struct musb *musb, int is_on)
{
	void __iomem	*tbase = musb->ctrl_base;
	u32		conf, prcm, timer;
	u8		devctl;
	struct usb_otg	*otg = musb->xceiv->otg;

	/* HDRC controls CPEN, but beware current surges during device
	 * connect.  They can trigger transient overcurrent conditions
	 * that must be ignored.
	 */

	prcm = musb_readl(tbase, TUSB_PRCM_MNGMT);
	conf = musb_readl(tbase, TUSB_DEV_CONF);
	devctl = musb_readb(musb->mregs, MUSB_DEVCTL);

	if (is_on) {
		timer = OTG_TIMER_MS(OTG_TIME_A_WAIT_VRISE);
		otg->default_a = 1;
		musb->xceiv->otg->state = OTG_STATE_A_WAIT_VRISE;
		devctl |= MUSB_DEVCTL_SESSION;

		conf |= TUSB_DEV_CONF_USB_HOST_MODE;
		MUSB_HST_MODE(musb);
	} else {
		u32	otg_stat;

		timer = 0;

		/* If ID pin is grounded, we want to be a_idle */
		otg_stat = musb_readl(tbase, TUSB_DEV_OTG_STAT);
		if (!(otg_stat & TUSB_DEV_OTG_STAT_ID_STATUS)) {
			switch (musb->xceiv->otg->state) {
			case OTG_STATE_A_WAIT_VRISE:
			case OTG_STATE_A_WAIT_BCON:
				musb->xceiv->otg->state = OTG_STATE_A_WAIT_VFALL;
				break;
			case OTG_STATE_A_WAIT_VFALL:
				musb->xceiv->otg->state = OTG_STATE_A_IDLE;
				break;
			default:
				musb->xceiv->otg->state = OTG_STATE_A_IDLE;
			}
			musb->is_active = 0;
			otg->default_a = 1;
			MUSB_HST_MODE(musb);
		} else {
			musb->is_active = 0;
			otg->default_a = 0;
			musb->xceiv->otg->state = OTG_STATE_B_IDLE;
			MUSB_DEV_MODE(musb);
		}

		devctl &= ~MUSB_DEVCTL_SESSION;
		conf &= ~TUSB_DEV_CONF_USB_HOST_MODE;
	}
	prcm &= ~(TUSB_PRCM_MNGMT_15_SW_EN | TUSB_PRCM_MNGMT_33_SW_EN);

	musb_writel(tbase, TUSB_PRCM_MNGMT, prcm);
	musb_writel(tbase, TUSB_DEV_OTG_TIMER, timer);
	musb_writel(tbase, TUSB_DEV_CONF, conf);
	musb_writeb(musb->mregs, MUSB_DEVCTL, devctl);

	dev_dbg(musb->controller, "VBUS %s, devctl %02x otg %3x conf %08x prcm %08x\n",
		usb_otg_state_string(musb->xceiv->otg->state),
		musb_readb(musb->mregs, MUSB_DEVCTL),
		musb_readl(tbase, TUSB_DEV_OTG_STAT),
		conf, prcm);
}

/*
 * Sets the mode to OTG, peripheral or host by changing the ID detection.
 * Caller must take care of locking.
 *
 * Note that if a mini-A cable is plugged in the ID line will stay down as
 * the weak ID pull-up is not able to pull the ID up.
 */
static int tusb_musb_set_mode(struct musb *musb, u8 musb_mode)
{
	void __iomem	*tbase = musb->ctrl_base;
	u32		otg_stat, phy_otg_ctrl, phy_otg_ena, dev_conf;

	otg_stat = musb_readl(tbase, TUSB_DEV_OTG_STAT);
	phy_otg_ctrl = musb_readl(tbase, TUSB_PHY_OTG_CTRL);
	phy_otg_ena = musb_readl(tbase, TUSB_PHY_OTG_CTRL_ENABLE);
	dev_conf = musb_readl(tbase, TUSB_DEV_CONF);

	switch (musb_mode) {

	case MUSB_HOST:		/* Disable PHY ID detect, ground ID */
		phy_otg_ctrl &= ~TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP;
		phy_otg_ena |= TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP;
		dev_conf |= TUSB_DEV_CONF_ID_SEL;
		dev_conf &= ~TUSB_DEV_CONF_SOFT_ID;
		break;
	case MUSB_PERIPHERAL:	/* Disable PHY ID detect, keep ID pull-up on */
		phy_otg_ctrl |= TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP;
		phy_otg_ena |= TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP;
		dev_conf |= (TUSB_DEV_CONF_ID_SEL | TUSB_DEV_CONF_SOFT_ID);
		break;
	case MUSB_OTG:		/* Use PHY ID detection */
		phy_otg_ctrl |= TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP;
		phy_otg_ena |= TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP;
		dev_conf &= ~(TUSB_DEV_CONF_ID_SEL | TUSB_DEV_CONF_SOFT_ID);
		break;

	default:
		dev_dbg(musb->controller, "Trying to set mode %i\n", musb_mode);
		return -EINVAL;
	}

	musb_writel(tbase, TUSB_PHY_OTG_CTRL,
			TUSB_PHY_OTG_CTRL_WRPROTECT | phy_otg_ctrl);
	musb_writel(tbase, TUSB_PHY_OTG_CTRL_ENABLE,
			TUSB_PHY_OTG_CTRL_WRPROTECT | phy_otg_ena);
	musb_writel(tbase, TUSB_DEV_CONF, dev_conf);

	otg_stat = musb_readl(tbase, TUSB_DEV_OTG_STAT);
	if ((musb_mode == MUSB_PERIPHERAL) &&
		!(otg_stat & TUSB_DEV_OTG_STAT_ID_STATUS))
			INFO("Cannot be peripheral with mini-A cable "
			"otg_stat: %08x\n", otg_stat);

	return 0;
}

static inline unsigned long
tusb_otg_ints(struct musb *musb, u32 int_src, void __iomem *tbase)
{
	u32		otg_stat = musb_readl(tbase, TUSB_DEV_OTG_STAT);
	unsigned long	idle_timeout = 0;
	struct usb_otg	*otg = musb->xceiv->otg;

	/* ID pin */
	if ((int_src & TUSB_INT_SRC_ID_STATUS_CHNG)) {
		int	default_a;

		default_a = !(otg_stat & TUSB_DEV_OTG_STAT_ID_STATUS);
		dev_dbg(musb->controller, "Default-%c\n", default_a ? 'A' : 'B');
		otg->default_a = default_a;
		tusb_musb_set_vbus(musb, default_a);

		/* Don't allow idling immediately */
		if (default_a)
			idle_timeout = jiffies + (HZ * 3);
	}

	/* VBUS state change */
	if (int_src & TUSB_INT_SRC_VBUS_SENSE_CHNG) {

		/* B-dev state machine:  no vbus ~= disconnect */
		if (!otg->default_a) {
			/* ? musb_root_disconnect(musb); */
			musb->port1_status &=
				~(USB_PORT_STAT_CONNECTION
				| USB_PORT_STAT_ENABLE
				| USB_PORT_STAT_LOW_SPEED
				| USB_PORT_STAT_HIGH_SPEED
				| USB_PORT_STAT_TEST
				);

			if (otg_stat & TUSB_DEV_OTG_STAT_SESS_END) {
				dev_dbg(musb->controller, "Forcing disconnect (no interrupt)\n");
				if (musb->xceiv->otg->state != OTG_STATE_B_IDLE) {
					/* INTR_DISCONNECT can hide... */
					musb->xceiv->otg->state = OTG_STATE_B_IDLE;
					musb->int_usb |= MUSB_INTR_DISCONNECT;
				}
				musb->is_active = 0;
			}
			dev_dbg(musb->controller, "vbus change, %s, otg %03x\n",
				usb_otg_state_string(musb->xceiv->otg->state), otg_stat);
			idle_timeout = jiffies + (1 * HZ);
			schedule_delayed_work(&musb->irq_work, 0);

		} else /* A-dev state machine */ {
			dev_dbg(musb->controller, "vbus change, %s, otg %03x\n",
				usb_otg_state_string(musb->xceiv->otg->state), otg_stat);

			switch (musb->xceiv->otg->state) {
			case OTG_STATE_A_IDLE:
				dev_dbg(musb->controller, "Got SRP, turning on VBUS\n");
				musb_platform_set_vbus(musb, 1);

				/* CONNECT can wake if a_wait_bcon is set */
				if (musb->a_wait_bcon != 0)
					musb->is_active = 0;
				else
					musb->is_active = 1;

				/*
				 * OPT FS A TD.4.6 needs few seconds for
				 * A_WAIT_VRISE
				 */
				idle_timeout = jiffies + (2 * HZ);

				break;
			case OTG_STATE_A_WAIT_VRISE:
				/* ignore; A-session-valid < VBUS_VALID/2,
				 * we monitor this with the timer
				 */
				break;
			case OTG_STATE_A_WAIT_VFALL:
				/* REVISIT this irq triggers during short
				 * spikes caused by enumeration ...
				 */
				if (musb->vbuserr_retry) {
					musb->vbuserr_retry--;
					tusb_musb_set_vbus(musb, 1);
				} else {
					musb->vbuserr_retry
						= VBUSERR_RETRY_COUNT;
					tusb_musb_set_vbus(musb, 0);
				}
				break;
			default:
				break;
			}
		}
	}

	/* OTG timer expiration */
	if (int_src & TUSB_INT_SRC_OTG_TIMEOUT) {
		u8	devctl;

		dev_dbg(musb->controller, "%s timer, %03x\n",
			usb_otg_state_string(musb->xceiv->otg->state), otg_stat);

		switch (musb->xceiv->otg->state) {
		case OTG_STATE_A_WAIT_VRISE:
			/* VBUS has probably been valid for a while now,
			 * but may well have bounced out of range a bit
			 */
			devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
			if (otg_stat & TUSB_DEV_OTG_STAT_VBUS_VALID) {
				if ((devctl & MUSB_DEVCTL_VBUS)
						!= MUSB_DEVCTL_VBUS) {
					dev_dbg(musb->controller, "devctl %02x\n", devctl);
					break;
				}
				musb->xceiv->otg->state = OTG_STATE_A_WAIT_BCON;
				musb->is_active = 0;
				idle_timeout = jiffies
					+ msecs_to_jiffies(musb->a_wait_bcon);
			} else {
				/* REVISIT report overcurrent to hub? */
				ERR("vbus too slow, devctl %02x\n", devctl);
				tusb_musb_set_vbus(musb, 0);
			}
			break;
		case OTG_STATE_A_WAIT_BCON:
			if (musb->a_wait_bcon != 0)
				idle_timeout = jiffies
					+ msecs_to_jiffies(musb->a_wait_bcon);
			break;
		case OTG_STATE_A_SUSPEND:
			break;
		case OTG_STATE_B_WAIT_ACON:
			break;
		default:
			break;
		}
	}
	schedule_delayed_work(&musb->irq_work, 0);

	return idle_timeout;
}

static irqreturn_t tusb_musb_interrupt(int irq, void *__hci)
{
	struct musb	*musb = __hci;
	void __iomem	*tbase = musb->ctrl_base;
	unsigned long	flags, idle_timeout = 0;
	u32		int_mask, int_src;

	spin_lock_irqsave(&musb->lock, flags);

	/* Mask all interrupts to allow using both edge and level GPIO irq */
	int_mask = musb_readl(tbase, TUSB_INT_MASK);
	musb_writel(tbase, TUSB_INT_MASK, ~TUSB_INT_MASK_RESERVED_BITS);

	int_src = musb_readl(tbase, TUSB_INT_SRC) & ~TUSB_INT_SRC_RESERVED_BITS;
	dev_dbg(musb->controller, "TUSB IRQ %08x\n", int_src);

	musb->int_usb = (u8) int_src;

	/* Acknowledge wake-up source interrupts */
	if (int_src & TUSB_INT_SRC_DEV_WAKEUP) {
		u32	reg;
		u32	i;

		if (musb->tusb_revision == TUSB_REV_30)
			tusb_wbus_quirk(musb, 0);

		/* there are issues re-locking the PLL on wakeup ... */

		/* work around issue 8 */
		for (i = 0xf7f7f7; i > 0xf7f7f7 - 1000; i--) {
			musb_writel(tbase, TUSB_SCRATCH_PAD, 0);
			musb_writel(tbase, TUSB_SCRATCH_PAD, i);
			reg = musb_readl(tbase, TUSB_SCRATCH_PAD);
			if (reg == i)
				break;
			dev_dbg(musb->controller, "TUSB NOR not ready\n");
		}

		/* work around issue 13 (2nd half) */
		tusb_set_clock_source(musb, 1);

		reg = musb_readl(tbase, TUSB_PRCM_WAKEUP_SOURCE);
		musb_writel(tbase, TUSB_PRCM_WAKEUP_CLEAR, reg);
		if (reg & ~TUSB_PRCM_WNORCS) {
			musb->is_active = 1;
			schedule_delayed_work(&musb->irq_work, 0);
		}
		dev_dbg(musb->controller, "wake %sactive %02x\n",
				musb->is_active ? "" : "in", reg);

		/* REVISIT host side TUSB_PRCM_WHOSTDISCON, TUSB_PRCM_WBUS */
	}

	if (int_src & TUSB_INT_SRC_USB_IP_CONN)
		del_timer(&musb->dev_timer);

	/* OTG state change reports (annoyingly) not issued by Mentor core */
	if (int_src & (TUSB_INT_SRC_VBUS_SENSE_CHNG
				| TUSB_INT_SRC_OTG_TIMEOUT
				| TUSB_INT_SRC_ID_STATUS_CHNG))
		idle_timeout = tusb_otg_ints(musb, int_src, tbase);

	/*
	 * Just clear the DMA interrupt if it comes as the completion for both
	 * TX and RX is handled by the DMA callback in tusb6010_omap
	 */
	if ((int_src & TUSB_INT_SRC_TXRX_DMA_DONE)) {
		u32	dma_src = musb_readl(tbase, TUSB_DMA_INT_SRC);

		dev_dbg(musb->controller, "DMA IRQ %08x\n", dma_src);
		musb_writel(tbase, TUSB_DMA_INT_CLEAR, dma_src);
	}

	/* EP interrupts. In OCP mode tusb6010 mirrors the MUSB interrupts */
	if (int_src & (TUSB_INT_SRC_USB_IP_TX | TUSB_INT_SRC_USB_IP_RX)) {
		u32	musb_src = musb_readl(tbase, TUSB_USBIP_INT_SRC);

		musb_writel(tbase, TUSB_USBIP_INT_CLEAR, musb_src);
		musb->int_rx = (((musb_src >> 16) & 0xffff) << 1);
		musb->int_tx = (musb_src & 0xffff);
	} else {
		musb->int_rx = 0;
		musb->int_tx = 0;
	}

	if (int_src & (TUSB_INT_SRC_USB_IP_TX | TUSB_INT_SRC_USB_IP_RX | 0xff))
		musb_interrupt(musb);

	/* Acknowledge TUSB interrupts. Clear only non-reserved bits */
	musb_writel(tbase, TUSB_INT_SRC_CLEAR,
		int_src & ~TUSB_INT_MASK_RESERVED_BITS);

	tusb_musb_try_idle(musb, idle_timeout);

	musb_writel(tbase, TUSB_INT_MASK, int_mask);
	spin_unlock_irqrestore(&musb->lock, flags);

	return IRQ_HANDLED;
}

static int dma_off;

/*
 * Enables TUSB6010. Caller must take care of locking.
 * REVISIT:
 * - Check what is unnecessary in MGC_HdrcStart()
 */
static void tusb_musb_enable(struct musb *musb)
{
	void __iomem	*tbase = musb->ctrl_base;

	/* Setup TUSB6010 main interrupt mask. Enable all interrupts except SOF.
	 * REVISIT: Enable and deal with TUSB_INT_SRC_USB_IP_SOF */
	musb_writel(tbase, TUSB_INT_MASK, TUSB_INT_SRC_USB_IP_SOF);

	/* Setup TUSB interrupt, disable DMA and GPIO interrupts */
	musb_writel(tbase, TUSB_USBIP_INT_MASK, 0);
	musb_writel(tbase, TUSB_DMA_INT_MASK, 0x7fffffff);
	musb_writel(tbase, TUSB_GPIO_INT_MASK, 0x1ff);

	/* Clear all subsystem interrups */
	musb_writel(tbase, TUSB_USBIP_INT_CLEAR, 0x7fffffff);
	musb_writel(tbase, TUSB_DMA_INT_CLEAR, 0x7fffffff);
	musb_writel(tbase, TUSB_GPIO_INT_CLEAR, 0x1ff);

	/* Acknowledge pending interrupt(s) */
	musb_writel(tbase, TUSB_INT_SRC_CLEAR, ~TUSB_INT_MASK_RESERVED_BITS);

	/* Only 0 clock cycles for minimum interrupt de-assertion time and
	 * interrupt polarity active low seems to work reliably here */
	musb_writel(tbase, TUSB_INT_CTRL_CONF,
			TUSB_INT_CTRL_CONF_INT_RELCYC(0));

	irq_set_irq_type(musb->nIrq, IRQ_TYPE_LEVEL_LOW);

	/* maybe force into the Default-A OTG state machine */
	if (!(musb_readl(tbase, TUSB_DEV_OTG_STAT)
			& TUSB_DEV_OTG_STAT_ID_STATUS))
		musb_writel(tbase, TUSB_INT_SRC_SET,
				TUSB_INT_SRC_ID_STATUS_CHNG);

	if (is_dma_capable() && dma_off)
		printk(KERN_WARNING "%s %s: dma not reactivated\n",
				__FILE__, __func__);
	else
		dma_off = 1;
}

/*
 * Disables TUSB6010. Caller must take care of locking.
 */
static void tusb_musb_disable(struct musb *musb)
{
	void __iomem	*tbase = musb->ctrl_base;

	/* FIXME stop DMA, IRQs, timers, ... */

	/* disable all IRQs */
	musb_writel(tbase, TUSB_INT_MASK, ~TUSB_INT_MASK_RESERVED_BITS);
	musb_writel(tbase, TUSB_USBIP_INT_MASK, 0x7fffffff);
	musb_writel(tbase, TUSB_DMA_INT_MASK, 0x7fffffff);
	musb_writel(tbase, TUSB_GPIO_INT_MASK, 0x1ff);

	del_timer(&musb->dev_timer);

	if (is_dma_capable() && !dma_off) {
		printk(KERN_WARNING "%s %s: dma still active\n",
				__FILE__, __func__);
		dma_off = 1;
	}
}

/*
 * Sets up TUSB6010 CPU interface specific signals and registers
 * Note: Settings optimized for OMAP24xx
 */
static void tusb_setup_cpu_interface(struct musb *musb)
{
	void __iomem	*tbase = musb->ctrl_base;

	/*
	 * Disable GPIO[5:0] pullups (used as output DMA requests)
	 * Don't disable GPIO[7:6] as they are needed for wake-up.
	 */
	musb_writel(tbase, TUSB_PULLUP_1_CTRL, 0x0000003F);

	/* Disable all pullups on NOR IF, DMAREQ0 and DMAREQ1 */
	musb_writel(tbase, TUSB_PULLUP_2_CTRL, 0x01FFFFFF);

	/* Turn GPIO[5:0] to DMAREQ[5:0] signals */
	musb_writel(tbase, TUSB_GPIO_CONF, TUSB_GPIO_CONF_DMAREQ(0x3f));

	/* Burst size 16x16 bits, all six DMA requests enabled, DMA request
	 * de-assertion time 2 system clocks p 62 */
	musb_writel(tbase, TUSB_DMA_REQ_CONF,
		TUSB_DMA_REQ_CONF_BURST_SIZE(2) |
		TUSB_DMA_REQ_CONF_DMA_REQ_EN(0x3f) |
		TUSB_DMA_REQ_CONF_DMA_REQ_ASSER(2));

	/* Set 0 wait count for synchronous burst access */
	musb_writel(tbase, TUSB_WAIT_COUNT, 1);
}

static int tusb_musb_start(struct musb *musb)
{
	void __iomem	*tbase = musb->ctrl_base;
	int		ret = 0;
	unsigned long	flags;
	u32		reg;

	if (musb->board_set_power)
		ret = musb->board_set_power(1);
	if (ret != 0) {
		printk(KERN_ERR "tusb: Cannot enable TUSB6010\n");
		return ret;
	}

	spin_lock_irqsave(&musb->lock, flags);

	if (musb_readl(tbase, TUSB_PROD_TEST_RESET) !=
		TUSB_PROD_TEST_RESET_VAL) {
		printk(KERN_ERR "tusb: Unable to detect TUSB6010\n");
		goto err;
	}

	musb->tusb_revision = tusb_get_revision(musb);
	tusb_print_revision(musb);
	if (musb->tusb_revision < 2) {
		printk(KERN_ERR "tusb: Unsupported TUSB6010 revision %i\n",
				musb->tusb_revision);
		goto err;
	}

	/* The uint bit for "USB non-PDR interrupt enable" has to be 1 when
	 * NOR FLASH interface is used */
	musb_writel(tbase, TUSB_VLYNQ_CTRL, 8);

	/* Select PHY free running 60MHz as a system clock */
	tusb_set_clock_source(musb, 1);

	/* VBus valid timer 1us, disable DFT/Debug and VLYNQ clocks for
	 * power saving, enable VBus detect and session end comparators,
	 * enable IDpullup, enable VBus charging */
	musb_writel(tbase, TUSB_PRCM_MNGMT,
		TUSB_PRCM_MNGMT_VBUS_VALID_TIMER(0xa) |
		TUSB_PRCM_MNGMT_VBUS_VALID_FLT_EN |
		TUSB_PRCM_MNGMT_OTG_SESS_END_EN |
		TUSB_PRCM_MNGMT_OTG_VBUS_DET_EN |
		TUSB_PRCM_MNGMT_OTG_ID_PULLUP);
	tusb_setup_cpu_interface(musb);

	/* simplify:  always sense/pullup ID pins, as if in OTG mode */
	reg = musb_readl(tbase, TUSB_PHY_OTG_CTRL_ENABLE);
	reg |= TUSB_PHY_OTG_CTRL_WRPROTECT | TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP;
	musb_writel(tbase, TUSB_PHY_OTG_CTRL_ENABLE, reg);

	reg = musb_readl(tbase, TUSB_PHY_OTG_CTRL);
	reg |= TUSB_PHY_OTG_CTRL_WRPROTECT | TUSB_PHY_OTG_CTRL_OTG_ID_PULLUP;
	musb_writel(tbase, TUSB_PHY_OTG_CTRL, reg);

	spin_unlock_irqrestore(&musb->lock, flags);

	return 0;

err:
	spin_unlock_irqrestore(&musb->lock, flags);

	if (musb->board_set_power)
		musb->board_set_power(0);

	return -ENODEV;
}

static int tusb_musb_init(struct musb *musb)
{
	struct platform_device	*pdev;
	struct resource		*mem;
	void __iomem		*sync = NULL;
	int			ret;

	musb->xceiv = usb_get_phy(USB_PHY_TYPE_USB2);
	if (IS_ERR_OR_NULL(musb->xceiv))
		return -EPROBE_DEFER;

	pdev = to_platform_device(musb->controller);

	/* dma address for async dma */
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	musb->async = mem->start;

	/* dma address for sync dma */
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!mem) {
		pr_debug("no sync dma resource?\n");
		ret = -ENODEV;
		goto done;
	}
	musb->sync = mem->start;

	sync = ioremap(mem->start, resource_size(mem));
	if (!sync) {
		pr_debug("ioremap for sync failed\n");
		ret = -ENOMEM;
		goto done;
	}
	musb->sync_va = sync;

	/* Offsets from base: VLYNQ at 0x000, MUSB regs at 0x400,
	 * FIFOs at 0x600, TUSB at 0x800
	 */
	musb->mregs += TUSB_BASE_OFFSET;

	ret = tusb_musb_start(musb);
	if (ret) {
		printk(KERN_ERR "Could not start tusb6010 (%d)\n",
				ret);
		goto done;
	}
	musb->isr = tusb_musb_interrupt;

	musb->xceiv->set_power = tusb_draw_power;
	the_musb = musb;

	timer_setup(&musb->dev_timer, musb_do_idle, 0);

done:
	if (ret < 0) {
		if (sync)
			iounmap(sync);

		usb_put_phy(musb->xceiv);
	}
	return ret;
}

static int tusb_musb_exit(struct musb *musb)
{
	del_timer_sync(&musb->dev_timer);
	the_musb = NULL;

	if (musb->board_set_power)
		musb->board_set_power(0);

	iounmap(musb->sync_va);

	usb_put_phy(musb->xceiv);
	return 0;
}

static const struct musb_platform_ops tusb_ops = {
	.quirks		= MUSB_DMA_TUSB_OMAP | MUSB_IN_TUSB |
			  MUSB_G_NO_SKB_RESERVE,
	.init		= tusb_musb_init,
	.exit		= tusb_musb_exit,

	.ep_offset	= tusb_ep_offset,
	.ep_select	= tusb_ep_select,
	.fifo_offset	= tusb_fifo_offset,
	.readb		= tusb_readb,
	.writeb		= tusb_writeb,
	.read_fifo	= tusb_read_fifo,
	.write_fifo	= tusb_write_fifo,
#ifdef CONFIG_USB_TUSB_OMAP_DMA
	.dma_init	= tusb_dma_controller_create,
	.dma_exit	= tusb_dma_controller_destroy,
#endif
	.enable		= tusb_musb_enable,
	.disable	= tusb_musb_disable,

	.set_mode	= tusb_musb_set_mode,
	.try_idle	= tusb_musb_try_idle,

	.vbus_status	= tusb_musb_vbus_status,
	.set_vbus	= tusb_musb_set_vbus,
};

static const struct platform_device_info tusb_dev_info = {
	.name		= "musb-hdrc",
	.id		= PLATFORM_DEVID_AUTO,
	.dma_mask	= DMA_BIT_MASK(32),
};

static int tusb_probe(struct platform_device *pdev)
{
	struct resource musb_resources[3];
	struct musb_hdrc_platform_data	*pdata = dev_get_platdata(&pdev->dev);
	struct platform_device		*musb;
	struct tusb6010_glue		*glue;
	struct platform_device_info	pinfo;
	int				ret;

	glue = devm_kzalloc(&pdev->dev, sizeof(*glue), GFP_KERNEL);
	if (!glue)
		return -ENOMEM;

	glue->dev			= &pdev->dev;

	pdata->platform_ops		= &tusb_ops;

	usb_phy_generic_register();
	platform_set_drvdata(pdev, glue);

	memset(musb_resources, 0x00, sizeof(*musb_resources) *
			ARRAY_SIZE(musb_resources));

	musb_resources[0].name = pdev->resource[0].name;
	musb_resources[0].start = pdev->resource[0].start;
	musb_resources[0].end = pdev->resource[0].end;
	musb_resources[0].flags = pdev->resource[0].flags;

	musb_resources[1].name = pdev->resource[1].name;
	musb_resources[1].start = pdev->resource[1].start;
	musb_resources[1].end = pdev->resource[1].end;
	musb_resources[1].flags = pdev->resource[1].flags;

	musb_resources[2].name = pdev->resource[2].name;
	musb_resources[2].start = pdev->resource[2].start;
	musb_resources[2].end = pdev->resource[2].end;
	musb_resources[2].flags = pdev->resource[2].flags;

	pinfo = tusb_dev_info;
	pinfo.parent = &pdev->dev;
	pinfo.res = musb_resources;
	pinfo.num_res = ARRAY_SIZE(musb_resources);
	pinfo.data = pdata;
	pinfo.size_data = sizeof(*pdata);

	glue->musb = musb = platform_device_register_full(&pinfo);
	if (IS_ERR(musb)) {
		ret = PTR_ERR(musb);
		dev_err(&pdev->dev, "failed to register musb device: %d\n", ret);
		return ret;
	}

	return 0;
}

static int tusb_remove(struct platform_device *pdev)
{
	struct tusb6010_glue		*glue = platform_get_drvdata(pdev);

	platform_device_unregister(glue->musb);
	usb_phy_generic_unregister(glue->phy);

	return 0;
}

static struct platform_driver tusb_driver = {
	.probe		= tusb_probe,
	.remove		= tusb_remove,
	.driver		= {
		.name	= "musb-tusb",
	},
};

MODULE_DESCRIPTION("TUSB6010 MUSB Glue Layer");
MODULE_AUTHOR("Felipe Balbi <balbi@ti.com>");
MODULE_LICENSE("GPL v2");
module_platform_driver(tusb_driver);
