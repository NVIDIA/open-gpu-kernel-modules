// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2005-2006 by Texas Instruments
 *
 * This file is part of the Inventra Controller Driver for Linux.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/usb/usb_phy_generic.h>

#include <mach/cputype.h>
#include <mach/hardware.h>

#include <asm/mach-types.h>

#include "musb_core.h"

#include "davinci.h"
#include "cppi_dma.h"


#define USB_PHY_CTRL	IO_ADDRESS(USBPHY_CTL_PADDR)
#define DM355_DEEPSLEEP	IO_ADDRESS(DM355_DEEPSLEEP_PADDR)

struct davinci_glue {
	struct device		*dev;
	struct platform_device	*musb;
	struct clk		*clk;
	bool			vbus_state;
	struct gpio_desc	*vbus;
	struct work_struct	vbus_work;
};

/* REVISIT (PM) we should be able to keep the PHY in low power mode most
 * of the time (24 MHZ oscillator and PLL off, etc) by setting POWER.D0
 * and, when in host mode, autosuspending idle root ports... PHYPLLON
 * (overriding SUSPENDM?) then likely needs to stay off.
 */

static inline void phy_on(void)
{
	u32	phy_ctrl = __raw_readl(USB_PHY_CTRL);

	/* power everything up; start the on-chip PHY and its PLL */
	phy_ctrl &= ~(USBPHY_OSCPDWN | USBPHY_OTGPDWN | USBPHY_PHYPDWN);
	phy_ctrl |= USBPHY_SESNDEN | USBPHY_VBDTCTEN | USBPHY_PHYPLLON;
	__raw_writel(phy_ctrl, USB_PHY_CTRL);

	/* wait for PLL to lock before proceeding */
	while ((__raw_readl(USB_PHY_CTRL) & USBPHY_PHYCLKGD) == 0)
		cpu_relax();
}

static inline void phy_off(void)
{
	u32	phy_ctrl = __raw_readl(USB_PHY_CTRL);

	/* powerdown the on-chip PHY, its PLL, and the OTG block */
	phy_ctrl &= ~(USBPHY_SESNDEN | USBPHY_VBDTCTEN | USBPHY_PHYPLLON);
	phy_ctrl |= USBPHY_OSCPDWN | USBPHY_OTGPDWN | USBPHY_PHYPDWN;
	__raw_writel(phy_ctrl, USB_PHY_CTRL);
}

static int dma_off = 1;

static void davinci_musb_enable(struct musb *musb)
{
	u32	tmp, old, val;

	/* workaround:  setup irqs through both register sets */
	tmp = (musb->epmask & DAVINCI_USB_TX_ENDPTS_MASK)
			<< DAVINCI_USB_TXINT_SHIFT;
	musb_writel(musb->ctrl_base, DAVINCI_USB_INT_MASK_SET_REG, tmp);
	old = tmp;
	tmp = (musb->epmask & (0xfffe & DAVINCI_USB_RX_ENDPTS_MASK))
			<< DAVINCI_USB_RXINT_SHIFT;
	musb_writel(musb->ctrl_base, DAVINCI_USB_INT_MASK_SET_REG, tmp);
	tmp |= old;

	val = ~MUSB_INTR_SOF;
	tmp |= ((val & 0x01ff) << DAVINCI_USB_USBINT_SHIFT);
	musb_writel(musb->ctrl_base, DAVINCI_USB_INT_MASK_SET_REG, tmp);

	if (is_dma_capable() && !dma_off)
		printk(KERN_WARNING "%s %s: dma not reactivated\n",
				__FILE__, __func__);
	else
		dma_off = 0;

	/* force a DRVVBUS irq so we can start polling for ID change */
	musb_writel(musb->ctrl_base, DAVINCI_USB_INT_SET_REG,
			DAVINCI_INTR_DRVVBUS << DAVINCI_USB_USBINT_SHIFT);
}

/*
 * Disable the HDRC and flush interrupts
 */
static void davinci_musb_disable(struct musb *musb)
{
	/* because we don't set CTRLR.UINT, "important" to:
	 *  - not read/write INTRUSB/INTRUSBE
	 *  - (except during initial setup, as workaround)
	 *  - use INTSETR/INTCLRR instead
	 */
	musb_writel(musb->ctrl_base, DAVINCI_USB_INT_MASK_CLR_REG,
			  DAVINCI_USB_USBINT_MASK
			| DAVINCI_USB_TXINT_MASK
			| DAVINCI_USB_RXINT_MASK);
	musb_writel(musb->ctrl_base, DAVINCI_USB_EOI_REG, 0);

	if (is_dma_capable() && !dma_off)
		WARNING("dma still active\n");
}


#define	portstate(stmt)		stmt

/*
 * VBUS SWITCHING IS BOARD-SPECIFIC ... at least for the DM6446 EVM,
 * which doesn't wire DRVVBUS to the FET that switches it.  Unclear
 * if that's a problem with the DM6446 chip or just with that board.
 *
 * In either case, the DM355 EVM automates DRVVBUS the normal way,
 * when J10 is out, and TI documents it as handling OTG.
 */

/* I2C operations are always synchronous, and require a task context.
 * With unloaded systems, using the shared workqueue seems to suffice
 * to satisfy the 100msec A_WAIT_VRISE timeout...
 */
static void evm_deferred_drvvbus(struct work_struct *work)
{
	struct davinci_glue *glue = container_of(work, struct davinci_glue,
						 vbus_work);

	gpiod_set_value_cansleep(glue->vbus, glue->vbus_state);
	glue->vbus_state = !glue->vbus_state;
}

static void davinci_musb_source_power(struct musb *musb, int is_on,
				      int immediate)
{
	struct davinci_glue *glue = dev_get_drvdata(musb->controller->parent);

	/* This GPIO handling is entirely optional */
	if (!glue->vbus)
		return;

	if (is_on)
		is_on = 1;

	if (glue->vbus_state == is_on)
		return;
	/* 0/1 vs "-1 == unknown/init" */
	glue->vbus_state = !is_on;

	if (machine_is_davinci_evm()) {
		if (immediate)
			gpiod_set_value_cansleep(glue->vbus, glue->vbus_state);
		else
			schedule_work(&glue->vbus_work);
	}
	if (immediate)
		glue->vbus_state = is_on;
}

static void davinci_musb_set_vbus(struct musb *musb, int is_on)
{
	WARN_ON(is_on && is_peripheral_active(musb));
	davinci_musb_source_power(musb, is_on, 0);
}


#define	POLL_SECONDS	2

static void otg_timer(struct timer_list *t)
{
	struct musb		*musb = from_timer(musb, t, dev_timer);
	void __iomem		*mregs = musb->mregs;
	u8			devctl;
	unsigned long		flags;

	/* We poll because DaVinci's won't expose several OTG-critical
	* status change events (from the transceiver) otherwise.
	 */
	devctl = musb_readb(mregs, MUSB_DEVCTL);
	dev_dbg(musb->controller, "poll devctl %02x (%s)\n", devctl,
		usb_otg_state_string(musb->xceiv->otg->state));

	spin_lock_irqsave(&musb->lock, flags);
	switch (musb->xceiv->otg->state) {
	case OTG_STATE_A_WAIT_VFALL:
		/* Wait till VBUS falls below SessionEnd (~0.2V); the 1.3 RTL
		 * seems to mis-handle session "start" otherwise (or in our
		 * case "recover"), in routine "VBUS was valid by the time
		 * VBUSERR got reported during enumeration" cases.
		 */
		if (devctl & MUSB_DEVCTL_VBUS) {
			mod_timer(&musb->dev_timer, jiffies + POLL_SECONDS * HZ);
			break;
		}
		musb->xceiv->otg->state = OTG_STATE_A_WAIT_VRISE;
		musb_writel(musb->ctrl_base, DAVINCI_USB_INT_SET_REG,
			MUSB_INTR_VBUSERROR << DAVINCI_USB_USBINT_SHIFT);
		break;
	case OTG_STATE_B_IDLE:
		/*
		 * There's no ID-changed IRQ, so we have no good way to tell
		 * when to switch to the A-Default state machine (by setting
		 * the DEVCTL.SESSION flag).
		 *
		 * Workaround:  whenever we're in B_IDLE, try setting the
		 * session flag every few seconds.  If it works, ID was
		 * grounded and we're now in the A-Default state machine.
		 *
		 * NOTE setting the session flag is _supposed_ to trigger
		 * SRP, but clearly it doesn't.
		 */
		musb_writeb(mregs, MUSB_DEVCTL,
				devctl | MUSB_DEVCTL_SESSION);
		devctl = musb_readb(mregs, MUSB_DEVCTL);
		if (devctl & MUSB_DEVCTL_BDEVICE)
			mod_timer(&musb->dev_timer, jiffies + POLL_SECONDS * HZ);
		else
			musb->xceiv->otg->state = OTG_STATE_A_IDLE;
		break;
	default:
		break;
	}
	spin_unlock_irqrestore(&musb->lock, flags);
}

static irqreturn_t davinci_musb_interrupt(int irq, void *__hci)
{
	unsigned long	flags;
	irqreturn_t	retval = IRQ_NONE;
	struct musb	*musb = __hci;
	struct usb_otg	*otg = musb->xceiv->otg;
	void __iomem	*tibase = musb->ctrl_base;
	struct cppi	*cppi;
	u32		tmp;

	spin_lock_irqsave(&musb->lock, flags);

	/* NOTE: DaVinci shadows the Mentor IRQs.  Don't manage them through
	 * the Mentor registers (except for setup), use the TI ones and EOI.
	 *
	 * Docs describe irq "vector" registers associated with the CPPI and
	 * USB EOI registers.  These hold a bitmask corresponding to the
	 * current IRQ, not an irq handler address.  Would using those bits
	 * resolve some of the races observed in this dispatch code??
	 */

	/* CPPI interrupts share the same IRQ line, but have their own
	 * mask, state, "vector", and EOI registers.
	 */
	cppi = container_of(musb->dma_controller, struct cppi, controller);
	if (is_cppi_enabled(musb) && musb->dma_controller && !cppi->irq)
		retval = cppi_interrupt(irq, __hci);

	/* ack and handle non-CPPI interrupts */
	tmp = musb_readl(tibase, DAVINCI_USB_INT_SRC_MASKED_REG);
	musb_writel(tibase, DAVINCI_USB_INT_SRC_CLR_REG, tmp);
	dev_dbg(musb->controller, "IRQ %08x\n", tmp);

	musb->int_rx = (tmp & DAVINCI_USB_RXINT_MASK)
			>> DAVINCI_USB_RXINT_SHIFT;
	musb->int_tx = (tmp & DAVINCI_USB_TXINT_MASK)
			>> DAVINCI_USB_TXINT_SHIFT;
	musb->int_usb = (tmp & DAVINCI_USB_USBINT_MASK)
			>> DAVINCI_USB_USBINT_SHIFT;

	/* DRVVBUS irqs are the only proxy we have (a very poor one!) for
	 * DaVinci's missing ID change IRQ.  We need an ID change IRQ to
	 * switch appropriately between halves of the OTG state machine.
	 * Managing DEVCTL.SESSION per Mentor docs requires we know its
	 * value, but DEVCTL.BDEVICE is invalid without DEVCTL.SESSION set.
	 * Also, DRVVBUS pulses for SRP (but not at 5V) ...
	 */
	if (tmp & (DAVINCI_INTR_DRVVBUS << DAVINCI_USB_USBINT_SHIFT)) {
		int	drvvbus = musb_readl(tibase, DAVINCI_USB_STAT_REG);
		void __iomem *mregs = musb->mregs;
		u8	devctl = musb_readb(mregs, MUSB_DEVCTL);
		int	err = musb->int_usb & MUSB_INTR_VBUSERROR;

		err = musb->int_usb & MUSB_INTR_VBUSERROR;
		if (err) {
			/* The Mentor core doesn't debounce VBUS as needed
			 * to cope with device connect current spikes. This
			 * means it's not uncommon for bus-powered devices
			 * to get VBUS errors during enumeration.
			 *
			 * This is a workaround, but newer RTL from Mentor
			 * seems to allow a better one: "re"starting sessions
			 * without waiting (on EVM, a **long** time) for VBUS
			 * to stop registering in devctl.
			 */
			musb->int_usb &= ~MUSB_INTR_VBUSERROR;
			musb->xceiv->otg->state = OTG_STATE_A_WAIT_VFALL;
			mod_timer(&musb->dev_timer, jiffies + POLL_SECONDS * HZ);
			WARNING("VBUS error workaround (delay coming)\n");
		} else if (drvvbus) {
			MUSB_HST_MODE(musb);
			musb->xceiv->otg->state = OTG_STATE_A_WAIT_VRISE;
			portstate(musb->port1_status |= USB_PORT_STAT_POWER);
			del_timer(&musb->dev_timer);
		} else {
			musb->is_active = 0;
			MUSB_DEV_MODE(musb);
			musb->xceiv->otg->state = OTG_STATE_B_IDLE;
			portstate(musb->port1_status &= ~USB_PORT_STAT_POWER);
		}

		/* NOTE:  this must complete poweron within 100 msec
		 * (OTG_TIME_A_WAIT_VRISE) but we don't check for that.
		 */
		davinci_musb_source_power(musb, drvvbus, 0);
		dev_dbg(musb->controller, "VBUS %s (%s)%s, devctl %02x\n",
				drvvbus ? "on" : "off",
				usb_otg_state_string(musb->xceiv->otg->state),
				err ? " ERROR" : "",
				devctl);
		retval = IRQ_HANDLED;
	}

	if (musb->int_tx || musb->int_rx || musb->int_usb)
		retval |= musb_interrupt(musb);

	/* irq stays asserted until EOI is written */
	musb_writel(tibase, DAVINCI_USB_EOI_REG, 0);

	/* poll for ID change */
	if (musb->xceiv->otg->state == OTG_STATE_B_IDLE)
		mod_timer(&musb->dev_timer, jiffies + POLL_SECONDS * HZ);

	spin_unlock_irqrestore(&musb->lock, flags);

	return retval;
}

static int davinci_musb_set_mode(struct musb *musb, u8 mode)
{
	/* EVM can't do this (right?) */
	return -EIO;
}

static int davinci_musb_init(struct musb *musb)
{
	void __iomem	*tibase = musb->ctrl_base;
	u32		revision;
	int 		ret = -ENODEV;

	musb->xceiv = usb_get_phy(USB_PHY_TYPE_USB2);
	if (IS_ERR_OR_NULL(musb->xceiv)) {
		ret = -EPROBE_DEFER;
		goto unregister;
	}

	musb->mregs += DAVINCI_BASE_OFFSET;

	/* returns zero if e.g. not clocked */
	revision = musb_readl(tibase, DAVINCI_USB_VERSION_REG);
	if (revision == 0)
		goto fail;

	timer_setup(&musb->dev_timer, otg_timer, 0);

	davinci_musb_source_power(musb, 0, 1);

	/* dm355 EVM swaps D+/D- for signal integrity, and
	 * is clocked from the main 24 MHz crystal.
	 */
	if (machine_is_davinci_dm355_evm()) {
		u32	phy_ctrl = __raw_readl(USB_PHY_CTRL);

		phy_ctrl &= ~(3 << 9);
		phy_ctrl |= USBPHY_DATAPOL;
		__raw_writel(phy_ctrl, USB_PHY_CTRL);
	}

	/* On dm355, the default-A state machine needs DRVVBUS control.
	 * If we won't be a host, there's no need to turn it on.
	 */
	if (cpu_is_davinci_dm355()) {
		u32	deepsleep = __raw_readl(DM355_DEEPSLEEP);

		deepsleep &= ~DRVVBUS_FORCE;
		__raw_writel(deepsleep, DM355_DEEPSLEEP);
	}

	/* reset the controller */
	musb_writel(tibase, DAVINCI_USB_CTRL_REG, 0x1);

	/* start the on-chip PHY and its PLL */
	phy_on();

	msleep(5);

	/* NOTE:  irqs are in mixed mode, not bypass to pure-musb */
	pr_debug("DaVinci OTG revision %08x phy %03x control %02x\n",
		revision, __raw_readl(USB_PHY_CTRL),
		musb_readb(tibase, DAVINCI_USB_CTRL_REG));

	musb->isr = davinci_musb_interrupt;
	return 0;

fail:
	usb_put_phy(musb->xceiv);
unregister:
	usb_phy_generic_unregister();
	return ret;
}

static int davinci_musb_exit(struct musb *musb)
{
	int	maxdelay = 30;
	u8	devctl, warn = 0;

	del_timer_sync(&musb->dev_timer);

	/* force VBUS off */
	if (cpu_is_davinci_dm355()) {
		u32	deepsleep = __raw_readl(DM355_DEEPSLEEP);

		deepsleep &= ~DRVVBUS_FORCE;
		deepsleep |= DRVVBUS_OVERRIDE;
		__raw_writel(deepsleep, DM355_DEEPSLEEP);
	}

	davinci_musb_source_power(musb, 0 /*off*/, 1);

	/*
	 * delay, to avoid problems with module reload.
	 * if there's no peripheral connected, this can take a
	 * long time to fall, especially on EVM with huge C133.
	 */
	do {
		devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
		if (!(devctl & MUSB_DEVCTL_VBUS))
			break;
		if ((devctl & MUSB_DEVCTL_VBUS) != warn) {
			warn = devctl & MUSB_DEVCTL_VBUS;
			dev_dbg(musb->controller, "VBUS %d\n",
				warn >> MUSB_DEVCTL_VBUS_SHIFT);
		}
		msleep(1000);
		maxdelay--;
	} while (maxdelay > 0);

	/* in OTG mode, another host might be connected */
	if (devctl & MUSB_DEVCTL_VBUS)
		dev_dbg(musb->controller, "VBUS off timeout (devctl %02x)\n", devctl);

	phy_off();

	usb_put_phy(musb->xceiv);

	return 0;
}

static const struct musb_platform_ops davinci_ops = {
	.quirks		= MUSB_DMA_CPPI,
	.init		= davinci_musb_init,
	.exit		= davinci_musb_exit,

#ifdef CONFIG_USB_TI_CPPI_DMA
	.dma_init	= cppi_dma_controller_create,
	.dma_exit	= cppi_dma_controller_destroy,
#endif
	.enable		= davinci_musb_enable,
	.disable	= davinci_musb_disable,

	.set_mode	= davinci_musb_set_mode,

	.set_vbus	= davinci_musb_set_vbus,
};

static const struct platform_device_info davinci_dev_info = {
	.name		= "musb-hdrc",
	.id		= PLATFORM_DEVID_AUTO,
	.dma_mask	= DMA_BIT_MASK(32),
};

static int davinci_probe(struct platform_device *pdev)
{
	struct resource			musb_resources[3];
	struct musb_hdrc_platform_data	*pdata = dev_get_platdata(&pdev->dev);
	struct platform_device		*musb;
	struct davinci_glue		*glue;
	struct platform_device_info	pinfo;
	struct clk			*clk;

	int				ret = -ENOMEM;

	glue = devm_kzalloc(&pdev->dev, sizeof(*glue), GFP_KERNEL);
	if (!glue)
		goto err0;

	clk = devm_clk_get(&pdev->dev, "usb");
	if (IS_ERR(clk)) {
		dev_err(&pdev->dev, "failed to get clock\n");
		ret = PTR_ERR(clk);
		goto err0;
	}

	ret = clk_enable(clk);
	if (ret) {
		dev_err(&pdev->dev, "failed to enable clock\n");
		goto err0;
	}

	glue->dev			= &pdev->dev;
	glue->clk			= clk;

	pdata->platform_ops		= &davinci_ops;

	glue->vbus = devm_gpiod_get_optional(&pdev->dev, NULL, GPIOD_OUT_LOW);
	if (IS_ERR(glue->vbus)) {
		ret = PTR_ERR(glue->vbus);
		goto err0;
	} else {
		glue->vbus_state = -1;
		INIT_WORK(&glue->vbus_work, evm_deferred_drvvbus);
	}

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

	/*
	 * For DM6467 3 resources are passed. A placeholder for the 3rd
	 * resource is always there, so it's safe to always copy it...
	 */
	musb_resources[2].name = pdev->resource[2].name;
	musb_resources[2].start = pdev->resource[2].start;
	musb_resources[2].end = pdev->resource[2].end;
	musb_resources[2].flags = pdev->resource[2].flags;

	pinfo = davinci_dev_info;
	pinfo.parent = &pdev->dev;
	pinfo.res = musb_resources;
	pinfo.num_res = ARRAY_SIZE(musb_resources);
	pinfo.data = pdata;
	pinfo.size_data = sizeof(*pdata);

	glue->musb = musb = platform_device_register_full(&pinfo);
	if (IS_ERR(musb)) {
		ret = PTR_ERR(musb);
		dev_err(&pdev->dev, "failed to register musb device: %d\n", ret);
		goto err1;
	}

	return 0;

err1:
	clk_disable(clk);

err0:
	return ret;
}

static int davinci_remove(struct platform_device *pdev)
{
	struct davinci_glue		*glue = platform_get_drvdata(pdev);

	platform_device_unregister(glue->musb);
	usb_phy_generic_unregister();
	clk_disable(glue->clk);

	return 0;
}

static struct platform_driver davinci_driver = {
	.probe		= davinci_probe,
	.remove		= davinci_remove,
	.driver		= {
		.name	= "musb-davinci",
	},
};

MODULE_DESCRIPTION("DaVinci MUSB Glue Layer");
MODULE_AUTHOR("Felipe Balbi <balbi@ti.com>");
MODULE_LICENSE("GPL v2");
module_platform_driver(davinci_driver);
